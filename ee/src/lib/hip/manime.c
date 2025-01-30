/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.7
 */
/*
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: manime.c,v 1.4 2002/12/15 12:34:07 kaneko Exp $	*/
#include <stdlib.h>
#include <math.h>
#include <eekernel.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>
#include "quat.h"

/********************************************************/
/*		Local Errors				*/
/********************************************************/
typedef enum {
    _NO_HEAP,
    _NO_MATRIXDATA,
    _NO_ANIME_DATA,
    _NO_KEYFRAME,
    _NO_KEYVALUE,
    _NO_STACK
} _local_err;

static const sceHiErr _local2hierr[] = {
    SCE_HIG_NO_HEAP,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA
};

static const char *_err_mes[] = {
    "HiP Manime : can't allocate memory\n",
    "HiP Manime : can't find MATRIXDATA\n",
    "HiP Manime : can't find ANIME_DATA\n",
    "HiP Manime : can't find KEYFRAME\n",
    "HiP Manime : can't find KEYVALUE\n",
    "HiP Manime : plug stack is null\n"
};

/* error handling */
static sceHiErr _hip_err(_local_err err)
{
    sceHiErrState.mes = _err_mes[err];
    return _local2hierr[err];
}

/********************************************************/
/*		Internal Structure			*/
/********************************************************/
typedef struct{
    sceHiPlugHrchyHead_t	*hh;		/* matrix data */
    sceHiPlugAnimeHead_t	*ah,*fh,*vh;	/* anime, frame, value data */
    u_int			frame;		/* current frame */
    int				padding[3];
} Manime_t;

/************************************************/
/*		Keypoint			*/
/************************************************/
static int keypoint(u_int frame, u_int *frame_tbl, int nkey)
{
    int	i;
    int	ret;

    ret = 0;
    for (i = 0; i < nkey; i++) {
	if ((frame >= frame_tbl[i]) && (frame <= frame_tbl[i + 1])) {
	    ret = i;
	    break;
	}
    }

    return ret;
}

/********************************************************************************/
/************************************************/
/*		Linear				*/
/************************************************/
static float linear(float t, float a, float b)
{
  float f;

  f = (1.0f - t)*a + t*b;
  return f;
}

static void Linear(sceHiPlugHrchyMatrix_t *mtx, sceVu0FMATRIX *kv, int key, float t)
{
    sceVu0FMATRIX *m0,*m1;	/* matrix sources */
    sceVu0FVECTOR q0,q1,q;	/* quaternion */
    float dot;
    int i;

    m0 = kv + key;
    m1 = kv + key + 1;

    matrix_to_quat(q0, *m0);
    matrix_to_quat(q1, *m1);

    /*	lerp */
    dot = dot_quat(q0, q1);
    if(dot < 0.0f){
      sceVu0ScaleVector(q1, q1, -1.0f);
    }
    for(i=0;i<4;i++){
	q[i] = linear(t, q0[i], q1[i]);
    }

    normalize_quat(q, q);
    quat_to_matrix(mtx->matrix, q);

    /*	lerp translation */
    for(i=0;i<3;i++){
	mtx->matrix[3][i] = linear(t, (*m0)[3][i], (*m1)[3][i]);
    }
}

/********************************************************/
/*		Pre Func				*/
/********************************************************/
static void pre_func(Manime_t *m)
{
    int i;
    sceHiPlugAnimeData_t	*dat;
    sceHiPlugHrchyMatrix_t	*mtx;
    sceHiPlugAnimeHead_t	*kfh,*kvh;
    int nkey,kp;
    u_int frm,*end, *kfp;
    sceVu0FVECTOR *kvp;
    float t;

    for(i=0;i<m->ah->top.num;i++){
	dat = sceHiPlugAnimeGetData(m->ah, i);			/* animation index */
	mtx = sceHiPlugHrchyGetMatrix(m->hh, dat->hrchy);	/* matrix index */
	kfh = sceHiPlugAnimeGetKeyHead(m->fh, dat->keyframe);	/* frame index */
	kvh = sceHiPlugAnimeGetKeyHead(m->vh, dat->keyvalue);	/* value index */

	nkey = kfh->key.num;

	end = sceHiPlugAnimeGetFrame(kfh, nkey - 1);
	if(*end > 0)	frm = m->frame % *end;
	else		frm = 0;

	kfp = sceHiPlugAnimeGetFrame(kfh, 0);
	kvp = sceHiPlugAnimeGetValue(kvh, 0);

	if(nkey > 1)
	    kp = keypoint(frm, kfp, nkey);
	else
	    kp = 0;

	t = (float)(frm - kfp[kp]) / (float)(kfp[kp + 1] - kfp[kp]);
	Linear(mtx, (sceVu0FMATRIX *)kvp, kp, t);
    }
}

/********************************************************/
/*		Init Func				*/
/********************************************************/
static sceHiErr init_func(sceHiPlug *p, Manime_t *m)
{
    static const sceHiType t[4] = {
	{SCE_HIP_COMMON,SCE_HIP_FRAMEWORK,SCE_HIP_HRCHY,SCE_HIG_DATA_STATUS,SCE_HIP_MATRIXDATA,SCE_HIP_REVISION},
	{SCE_HIP_COMMON,SCE_HIP_FRAMEWORK,SCE_HIP_ANIME,SCE_HIG_DATA_STATUS,SCE_HIP_ANIME_DATA,SCE_HIP_REVISION},
	{SCE_HIP_COMMON,SCE_HIP_FRAMEWORK,SCE_HIP_ANIME,SCE_HIG_DATA_STATUS,SCE_HIP_KEYFRAME,SCE_HIP_REVISION},
	{SCE_HIP_COMMON,SCE_HIP_FRAMEWORK,SCE_HIP_ANIME,SCE_HIG_DATA_STATUS,SCE_HIP_KEYVALUE,SCE_HIP_REVISION}
    };

    m->hh = sceHiPlugHrchyGetHead(p, t[0]);
    if (m->hh == NULL) return _hip_err(_NO_MATRIXDATA);

    m->ah = sceHiPlugAnimeGetHead(p, t[1]);
    if (m->ah == NULL) return _hip_err(_NO_ANIME_DATA);

    m->fh = sceHiPlugAnimeGetHead(p, t[2]);
    if (m->fh == NULL) return _hip_err(_NO_KEYFRAME);

    m->vh = sceHiPlugAnimeGetHead(p, t[3]);
    if (m->vh == NULL) return _hip_err(_NO_KEYVALUE);

    m->frame = 0;

    return SCE_HIG_NO_ERR;
}

/********************************************************/
/*		Main Func				*/
/*		Matrix Animation			*/
/********************************************************/
sceHiErr sceHiPlugManime(sceHiPlug *plug, int process)
{
    sceHiErr	err;
    Manime_t	*m;

    switch(process){
    case SCE_HIG_INIT_PROCESS:
      m = (Manime_t *)sceHiMemAlign(16, sizeof(Manime_t));
      if(m == NULL) return _hip_err(_NO_HEAP);
      err = init_func(plug, m);
      if(err != SCE_HIG_NO_ERR){
	  sceHiMemFree((u_int *)((u_int)m & 0x0fffffff));
	  return err;
      }
      plug->stack = (u_int)m;
      break;

    case SCE_HIG_PRE_PROCESS:
      m = (Manime_t *)plug->stack;
      if(m == NULL) return _hip_err(_NO_STACK);
      if(plug->args){
	  sceHiPlugAnimePreCalcArg_t	*arg;
	  arg = (sceHiPlugAnimePreCalcArg_t *)plug->args;
	  if(arg->setframe_enable){
	      m->frame = arg->setframe;
	  }
	  arg->currentframe = m->frame;
      }
      pre_func(m);
      m->frame++;
      break;

    case SCE_HIG_POST_PROCESS:
      break;

    case SCE_HIG_END_PROCESS:
      m = (Manime_t *)plug->stack;
      if(m == NULL) return _hip_err(_NO_STACK);
      sceHiMemFree((u_int *)((u_int)m & 0x0fffffff));
      plug->stack = NULL;
      plug->args = NULL;
      break;

    default:
      break;
    }

    return SCE_HIG_NO_ERR;
}
