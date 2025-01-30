/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4.2
 */
/*
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: anime.c,v 1.7 2001/02/13 11:12:04 aoki Exp $	*/
#include <eekernel.h>
#include <stdio.h>
#include <eeregs.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>

#define PADDR_MASK 0x0fffffff
#define Paddr(x) ((u_int)(x)&PADDR_MASK)

/************************************************/
/*		Structure			*/
/************************************************/
typedef struct	_Hierarchy{
    sceVu0FVECTOR	trans;
    sceVu0FVECTOR	rot;
    sceVu0FVECTOR	scale;
    int			shape;		/* not use */
    int			parent;
    int			child;
    int			sibling;
}Hierarchy;

typedef struct _Animation{
  u_int	hierarchy;
  u_int	nkey;
  u_int	keyframe;
  u_int	keyvalue;
}Animation;

typedef struct _ANIME_FRAME{
  Animation	*anime;		/* animation data pointer */
  Hierarchy	*hrchy;		/* hierarchy data pointer */
  u_int		**keyframe;	/* keyframe data pointer */
  float		**keyvalue;	/* keyvalue data pointer */
  int		num;		/* num of animation */
  u_int		frame;		/* frame counter */
  u_int		padding0;	/* padding */
  u_int		padding1;	/* padding */
}ANIME_FRAME;

/************************************************/
/*		Constant Values			*/
/************************************************/
#define QWORD 4

enum _interp_t{
  CONSTANT	= (1<<0),
  LINEAR	= (1<<1),
  HERMITE	= (1<<2),
  BEZIER	= (1<<3),
  BSPLINE	= (1<<4)
};

enum _fcurve_t{
  TX = (1<<0),
  TY = (1<<1),
  TZ = (1<<2),
  RX = (1<<3),
  RY = (1<<4),
  RZ = (1<<5),
  SX = (1<<6),
  SY = (1<<7),
  SZ = (1<<8),
  TXYZ = (1<<9),
  RXYZ = (1<<10),
  RXZY = (1<<11),
  RYXZ = (1<<12),
  RYZX = (1<<13),
  RZXY = (1<<14),
  RZYX = (1<<15),
  SXYZ = (1<<16)
};

enum _fcurve_m{
  TXYZ_M = (0x01<<9),
  RXYZ_M = (0x3f<<10),
  SXYZ_M = (0x01<<16)
};

enum _factor_t{
  X=0,Y,Z,XYZ
};

/* local err id */
typedef enum {
    _NULL_POINTER,
    _NEG_ANIME_FRAME_NUM,
    _NEG_KEY_FRAME_NUM,
    _NO_HEAP,
    _NEG_KEYVALUE_NUM,
    _PLGBLK_STACK_BROKEN
} _local_err;

/* local err id => hig err id */
static const sceHiErr _local2higerr[] = {
    SCE_HIG_INVALID_VALUE,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_NO_HEAP,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA
};

/* error message */
static const char *_anime_err_mes[] = {
    "HiP Anime : null pointer\n",
    "HiP Anime : the frame number is negative\n",
    "HiP Anime : the key frame number is negative\n",
    "HiP Anime : can't allocate memory from HiG Heap\n",
    "HiP Anime : the key value number is negative\n",
    "HiP Anime : the plugin's stack is broken(maybe didnt do INIT_PROCESS)\n",
};

/* error handling */
static sceHiErr _hip_anime_err(_local_err err)
{
    sceHiErrState.mes = _anime_err_mes[err];
    return _local2higerr[err];
}

/************************************************/
/*		Keypoint			*/
/************************************************/
static u_int keypoint(u_int frame, u_int *kf, u_int nkey)
{
  u_int i;
  u_int ret;

  ret = 0;
  for(i=0;i<nkey;i++){
    if((frame>=kf[i]) && (frame<=kf[i+1])){
      ret = i;
      break;
    }
  }
  return ret;
}

/************************************************/
/*		Spline				*/
/************************************************/
/*

  f(t) = ((a*t + b)*t + c)*t + d
  q = { a, b, c, d }
  q = m * p

*/
static float spline(float t, sceVu0FMATRIX m, sceVu0FVECTOR p)
{
  sceVu0FVECTOR q;

  float f;

  sceVu0ApplyMatrix(q, m, p);
  f = ((q[0]*t + q[1])*t + q[2])*t +q[3];
  return f;
}

/************************************************/
/*		Hermite				*/
/************************************************/
static void Hermite(Hierarchy *hrc, float *kv, u_int key, float t)
{
  float f;
  u_int i;
  sceVu0FVECTOR *p;
  sceVu0FMATRIX m ={
    { 2.0f, -3.0f, 0.0f, 1.0f},
    {-2.0f,  3.0f, 0.0f, 0.0f},
    { 1.0f, -2.0f, 1.0f, 0.0f},
    { 1.0f, -1.0f, 0.0f, 0.0f}
  };
  u_int	*fcurve;
  u_int offset;

  fcurve = (u_int *)(kv - QWORD);
  if(!(*fcurve)) *fcurve = TXYZ|RYXZ;

  offset = 0;
  if((*fcurve) & TXYZ_M) offset++;
  if((*fcurve) & RXYZ_M) offset++;
  if((*fcurve) & SXYZ_M) offset++;

  p = (sceVu0FVECTOR *)kv;
  p += key * XYZ * offset;

  if((*fcurve) & TXYZ_M){
    for(i=0;i<XYZ;i++){
      f = spline(t, m, *p);
      hrc->trans[i] = f;
      p ++;
    }
    hrc->trans[XYZ] = 0.0f;
  }

  if((*fcurve) & RXYZ_M){
    for(i=0;i<XYZ;i++){
      f = spline(t, m, *p);
      hrc->rot[i] = f;
      p++;
    }
    hrc->rot[XYZ] = 0.0f;
  }

  if((*fcurve) & SXYZ_M){
    for(i=0;i<XYZ;i++){
      f = spline(t, m, *p);
      hrc->scale[i] = f;
      p++;
    }
    hrc->scale[XYZ] = 0.0f;
  }
}

/************************************************/
/*		Bezier				*/
/************************************************/
static void Bezier(Hierarchy *hrc, float *kv, u_int key, float t)
{
  float f;
  u_int i;
  sceVu0FVECTOR *p;
  sceVu0FMATRIX m ={
    {-1.0f,  3.0f, -3.0f, 1.0f},
    { 3.0f, -6.0f,  3.0f, 0.0f},
    {-3.0f,  3.0f,  0.0f, 0.0f},
    { 1.0f,  0.0f,  0.0f, 0.0f}
  };
  u_int	*fcurve;
  u_int offset;

  fcurve = (u_int *)(kv - QWORD);
  if(!(*fcurve)) *fcurve = TXYZ|RYXZ;

  offset = 0;
  if((*fcurve) & TXYZ_M) offset++;
  if((*fcurve) & RXYZ_M) offset++;
  if((*fcurve) & SXYZ_M) offset++;

  p = (sceVu0FVECTOR *)kv;
  p += key * XYZ * offset;

  if((*fcurve) & TXYZ_M){
    for(i=0;i<XYZ;i++){
      f = spline(t, m, *p);
      hrc->trans[i] = f;
      p ++;
    }
    hrc->trans[XYZ] = 0.0f;
  }

  if((*fcurve) & RXYZ_M){
    for(i=0;i<XYZ;i++){
      f = spline(t, m, *p);
      hrc->rot[i] = f;
      p++;
    }
    hrc->rot[XYZ] = 0.0f;
  }

  if((*fcurve) & SXYZ_M){
    for(i=0;i<XYZ;i++){
      f = spline(t, m, *p);
      hrc->scale[i] = f;
      p++;
    }
    hrc->scale[XYZ] = 0.0f;
  }
}
/************************************************/
/*		Linear				*/
/************************************************/
static float linear(float t, float a, float b)
{
  float f;

  f = (1.0f - t)*a + t*b;
  return f;
}
static void Linear(Hierarchy *hrc, float *kv, u_int key, float t)
{
  float a,b;
  float f;
  u_int i;
  sceVu0FVECTOR *p,*q;
  u_int	*fcurve;
  u_int offset;

  fcurve = (u_int *)(kv - QWORD);
  if(!(*fcurve)) *fcurve = TXYZ|RYXZ;

  offset = 0;
  if((*fcurve) & TXYZ_M) offset++;
  if((*fcurve) & RXYZ_M) offset++;
  if((*fcurve) & SXYZ_M) offset++;

  p = q = (sceVu0FVECTOR *)kv;
  p += key * offset;
  q += (key+1) * offset;

  if((*fcurve) & TXYZ_M){
    for(i=0;i<XYZ;i++){
      a = (*p)[i];
      b = (*q)[i];
      f = linear(t, a, b);
      hrc->trans[i] = f;
    }
    hrc->trans[XYZ] = 0.0f;
    p++; q++;
  }
  if((*fcurve) & RXYZ_M){
    for(i=0;i<XYZ;i++){
      a = (*p)[i];
      b = (*q)[i];

      f = linear(t, a, b);
      hrc->rot[i] = f;
    }
    hrc->rot[XYZ] = 0.0f;
    p++; q++;
  }
  if((*fcurve) & SXYZ_M){
    for(i=0;i<XYZ;i++){
      a = (*p)[i];
      b = (*q)[i];

      f = linear(t, a, b);
      hrc->scale[i] = f;
    }
    hrc->scale[XYZ] = 0.0f;
  }
  
}

/************************************************/
/*		Constant			*/
/************************************************/
static void Constant(Hierarchy *hrc, float *kv, u_int key)
{
  sceVu0FVECTOR *p;
  u_int	*fcurve;
  u_int offset;

  fcurve = (u_int *)(kv - QWORD);
  if(!(*fcurve)) *fcurve = TXYZ|RYXZ;

  offset = 0;
  if((*fcurve) & TXYZ_M) offset++;
  if((*fcurve) & RXYZ_M) offset++;
  if((*fcurve) & SXYZ_M) offset++;

  p = (sceVu0FVECTOR *)kv;
  p += key * offset;

  if((*fcurve) & TXYZ_M){
    sceVu0CopyVector(hrc->trans, *p);
    p++;
  }
  if((*fcurve) & RXYZ_M){
    sceVu0CopyVector(hrc->rot, *p);
    p++;
  }
  if((*fcurve) & SXYZ_M){
    sceVu0CopyVector(hrc->scale, *p);
  }
}

/************************************************/
/*		Animation			*/
/************************************************/
static sceHiErr SetAnimation(ANIME_FRAME *af)
{
  u_int nkey,hier;
  u_int frm, key, end;
  u_int *kf;
  float *kv;
  Hierarchy *hrc;
  u_int	*interp;	/* address = keyframe - QWORD */

  float t;
  u_int i;

  for(i=0;i<af->num;i++){
    nkey = af->anime[i].nkey;
    hier = af->anime[i].hierarchy;
    kf = af->keyframe[af->anime[i].keyframe];
    kv = af->keyvalue[af->anime[i].keyvalue];
    end = kf[nkey-1];

    hrc = af->hrchy + hier;
    interp = (kf - QWORD);	/* interpolation type */

    if(end > 0)
      frm = (u_int)(af->frame % end);
    else
      frm = 0;

    if(nkey > 1)
      key = keypoint(frm, kf, nkey);
    else
      key = 0;

    switch(*interp){
    case CONSTANT:
      Constant(hrc, kv, key);
      break;
    case HERMITE:
      t = (float)(frm - kf[key])/(float)(kf[key+1] - kf[key]);
      Hermite(hrc, kv, key, t);
      break;
    case BEZIER:
      t = (float)(frm - kf[key])/(float)(kf[key+1] - kf[key]);
      Bezier(hrc, kv, key, t);
      break;
    case BSPLINE:
      break;

    case LINEAR:
    default:
      t = (float)(frm - kf[key])/(float)(kf[key+1] - kf[key]);
      Linear(hrc, kv, key, t);
      break;
    }
  }

  return SCE_HIG_NO_ERR;
}

#define SIZIDX 2
#define NUMIDX 3
#define QWSIZE 4

/************************************************/
/*		Animation Init			*/
/************************************************/
static sceHiErr AnimeInit(ANIME_FRAME *af, u_int *anmP, u_int *frmP, u_int *valP, u_int *hrcP)
{
    int i;
    int num;

    if((af == NULL) || (anmP == NULL) || (frmP == NULL) || (valP == NULL) || (hrcP == NULL))
      return _hip_anime_err(_NULL_POINTER);

    /*	Animation	*/
    af->num = anmP[NUMIDX];
    if(af->num <= 0) return _hip_anime_err(_NEG_ANIME_FRAME_NUM);
    af->anime = (Animation *)(anmP + QWSIZE);

    /*	Hierarchy	*/
    af->hrchy = (Hierarchy *)(hrcP+QWSIZE);

    /*	Keyframe	*/
    num = frmP[NUMIDX];
    if(num <= 0) return _hip_anime_err(_NEG_KEY_FRAME_NUM);
    af->keyframe = (u_int **)sceHiMemAlign(16, sizeof(u_int *)*num);
    if(af->keyframe == NULL) return _hip_anime_err(_NO_HEAP);
    frmP += QWSIZE;
    for(i=0;i<num;i++){
	af->keyframe[i] = (u_int *)(frmP + QWSIZE);
	frmP += frmP[SIZIDX] + QWSIZE;
    }

    /*	Keyvalue	*/
    num = valP[NUMIDX];
    if(num <= 0) return _hip_anime_err(_NEG_KEYVALUE_NUM);
    af->keyvalue = (float **)sceHiMemAlign(16, sizeof(float *)*num);
    if(af->keyvalue == NULL) return _hip_anime_err(_NO_HEAP);
    valP += QWSIZE;
    for(i=0;i<num;i++){
	af->keyvalue[i] = (float *)(valP + QWSIZE);
	valP += valP[SIZIDX] + QWSIZE;
    }


    return SCE_HIG_NO_ERR;
}

/************************************************/
/*		Animation Plug			*/
/************************************************/
sceHiErr sceHiPlugAnime(sceHiPlug *plug, int process)
{
    sceHiType	type;
    u_int	*data;
    sceHiErr	err;
    ANIME_FRAME	*anime_frame;
    sceHiPlugAnimePreCalcArg_t *arg;
    u_int	*anmP;
    u_int	*frmP;
    u_int	*valP;
    u_int	*hrcP;

    switch(process){
      case SCE_HIG_INIT_PROCESS:
	type.repository = SCE_HIP_COMMON;
	type.project = SCE_HIP_FRAMEWORK;
	type.category = SCE_HIP_ANIME;
	type.id = SCE_HIP_ANIME_DATA;
	type.status = SCE_HIG_DATA_STATUS;
	type.revision = SCE_HIP_REVISION;

	err = sceHiGetData(plug, &data, type);	if(err != SCE_HIG_NO_ERR) return err;
	anmP = data;

	type.id = SCE_HIP_KEYFRAME;
	err = sceHiGetData(plug, &data, type);	if(err != SCE_HIG_NO_ERR) return err;
	frmP = data;

	type.id = SCE_HIP_KEYVALUE;
	err = sceHiGetData(plug, &data, type);	if(err != SCE_HIG_NO_ERR) return err;
	valP = data;

	type.category = SCE_HIP_HRCHY;
	type.id = SCE_HIP_HRCHY_DATA;
	err = sceHiGetData(plug, &data, type);	if(err != SCE_HIG_NO_ERR) return err;
	hrcP = data;

	anime_frame = (ANIME_FRAME *)sceHiMemAlign(16, sizeof(ANIME_FRAME) * 1);
	if(anime_frame == NULL) return _hip_anime_err(_NO_HEAP);

	err = AnimeInit(anime_frame, anmP, frmP, valP, hrcP);	if(err != SCE_HIG_NO_ERR) return err;
	anime_frame->frame = 0;
	plug->stack = (u_int)anime_frame;
	break;

      case SCE_HIG_PRE_PROCESS:
	anime_frame = (ANIME_FRAME *)plug->stack;
	if(anime_frame == NULL) return _hip_anime_err(_PLGBLK_STACK_BROKEN);
	arg = (sceHiPlugAnimePreCalcArg_t *)plug->args;
	if(arg != NULL){
	    if (arg->setframe_enable)	anime_frame->frame = arg->setframe;
	    else anime_frame->frame++;

	    arg->currentframe=anime_frame->frame;
	}
	else{
	  anime_frame->frame++;
	}
	err = SetAnimation(anime_frame);	if(err != SCE_HIG_NO_ERR) return err;
	break;

      case SCE_HIG_POST_PROCESS:
	break;

      case SCE_HIG_END_PROCESS:
	anime_frame = (ANIME_FRAME *)plug->stack;
	if(anime_frame == NULL) return _hip_anime_err(_PLGBLK_STACK_BROKEN);
	anime_frame->frame = 0;
	sceHiMemFree((u_int *)Paddr(anime_frame->keyframe));
	sceHiMemFree((u_int *)Paddr(anime_frame->keyvalue));
	sceHiMemFree((u_int *)Paddr(anime_frame));
	plug->stack = NULL;
	plug->args = NULL;
	break;

      default:
	break;
    }

    return SCE_HIG_NO_ERR;
}
