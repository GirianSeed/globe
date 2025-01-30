/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/*
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: clip.c,v 1.3 2002/05/20 02:07:22 kaneko Exp $	*/
#include <eekernel.h>
#include <stdlib.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>

/********************************************************/
/*		Local Errors				*/
/********************************************************/
typedef enum {
    _NO_HEAP,
    _NO_CLIP_DATA,
    _NO_BASEMATRIX,
    _NO_MICRO_DATA,
    _NO_STACK,
} _local_err;

static const sceHiErr _local2hierr[] = {
    SCE_HIG_NO_HEAP,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
};

static const char *_err_mes[] = {
    "HiP Clip : can't allocate memory\n",
    "HiP Clip : can't find CLIP_DATA\n",
    "HiP Clip : can't find BASEMATRIX\n",
    "HiP Clip : can't find MICRO_DATA\n",
    "HiP Clip : plug stack is null\n",
};

/* error handling */
static sceHiErr _hip_err(_local_err err)
{
    sceHiErrState.mes = _err_mes[err];
    return _local2hierr[err];
}

/*******************************************************
 *	Clip Structure
 *******************************************************/

typedef struct{
    int				num;
    sceHiPlugClipData_t		*dat;
    sceHiPlugShapeMatrix_t	*mtx;
    sceHiPlugMicroData_t	*mic;
} Clip_t;

#define GS_XOFFSET 2048.0f
#define GS_YOFFSET 2048.0f

/*******************************************************
 *	Clip Calc
 *******************************************************/
static void clip_calc(Clip_t *clip)
{
    sceVu0FVECTOR min,max;	/* min,max plane */
    sceVu0FMATRIX lscreen;	/* local screen */
    sceVu0FVECTOR bbox[8];	/* bounding box */
    float near,far,width,height;
    sceHiPlugShapeMatrix_t	*mtx;
    sceHiPlugClipData_t		*dat;
    int i;

    far = clip->mic->clp.clip[2]+clip->mic->clp.clip[3];	/* (f+n)/2+(f-n)/2 */
    near = clip->mic->clp.clip[2]-clip->mic->clp.clip[3];	/* (f+n)/2-(f-n)/2 */
    width = clip->mic->clp.clip[3]/clip->mic->clp.clip[0];	/* (f-n)/2 / (f-n)/w */
    height = clip->mic->clp.clip[3]/clip->mic->clp.clip[1];	/* (f-n)/2 / (f-n)/h */

    min[0] = GS_XOFFSET-width;
    min[1] = GS_YOFFSET-height;
    min[2] = 0.0f;
    min[3] = near;
    max[0] = GS_XOFFSET+width;
    max[1] = GS_YOFFSET+height;
    max[2] = 0.0f;
    max[3] = far;

    for(i=0;i<clip->num;i++){
	dat = clip->dat + i;
	mtx = clip->mtx + i;
	sceVu0MulMatrix(lscreen, clip->mic->wscreen, mtx->local);

	bbox[0][0]=dat->max[0];bbox[0][1]=dat->min[1];bbox[0][2]=dat->max[2];bbox[0][3]=1.0f;
	bbox[1][0]=dat->max[0];bbox[1][1]=dat->max[1];bbox[1][2]=dat->max[2];bbox[1][3]=1.0f;
	bbox[2][0]=dat->max[0];bbox[2][1]=dat->min[1];bbox[2][2]=dat->min[2];bbox[2][3]=1.0f;
	bbox[3][0]=dat->max[0];bbox[3][1]=dat->max[1];bbox[3][2]=dat->min[2];bbox[3][3]=1.0f;
	bbox[4][0]=dat->min[0];bbox[4][1]=dat->min[1];bbox[4][2]=dat->max[2];bbox[4][3]=1.0f;
	bbox[5][0]=dat->min[0];bbox[5][1]=dat->max[1];bbox[5][2]=dat->max[2];bbox[5][3]=1.0f;
	bbox[6][0]=dat->min[0];bbox[6][1]=dat->min[1];bbox[6][2]=dat->min[2];bbox[6][3]=1.0f;
	bbox[7][0]=dat->min[0];bbox[7][1]=dat->max[1];bbox[7][2]=dat->min[2];bbox[7][3]=1.0f;

	mtx->flags = sceVu0ClipAll(min,max,lscreen,bbox,8) & 0x1;
    }
}

/*******************************************************
 *	Clip Init
 *******************************************************/
static sceHiErr clip_init(sceHiPlug *plug, Clip_t *clip)
{
    sceHiType type;
    sceHiPlugClipHead_t *ch;
    sceHiPlugShapeHead_t *sh;

    type.repository = SCE_HIP_COMMON;
    type.project = SCE_HIP_FRAMEWORK;
    type.category = SCE_HIP_CLIP;
    type.status = SCE_HIG_DATA_STATUS;
    type.id = SCE_HIP_CLIP_DATA;
    type.revision = SCE_HIP_REVISION;

    ch = sceHiPlugClipGetHead(plug, type);
    if (ch == NULL) return _hip_err(_NO_CLIP_DATA);
    clip->num = ch->num;
    clip->dat = sceHiPlugClipGetData(ch, 0);
    if (clip->dat == NULL) return _hip_err(_NO_CLIP_DATA);

    type.category = SCE_HIP_SHAPE;
    type.id = SCE_HIP_BASEMATRIX;
    sh = sceHiPlugShapeGetHead(plug, type);
    if(sh == NULL) return _hip_err(_NO_BASEMATRIX);
    clip->mtx = sceHiPlugShapeGetMatrix(sh, 0);
    if (clip->mtx == NULL) return _hip_err(_NO_BASEMATRIX);

    clip->mic = sceHiPlugMicroGetData(plug);
    if(clip->mic == NULL) return _hip_err(_NO_MICRO_DATA);

    return SCE_HIG_NO_ERR;
}

/*******************************************************
 *	Clip Plug
 *******************************************************/
sceHiErr sceHiPlugClip(sceHiPlug *plug, int process)
{
    sceHiErr	err;
    Clip_t	*clip;

    switch(process){
    case SCE_HIG_INIT_PROCESS:
      clip = (Clip_t *)sceHiMemAlign(16, sizeof(Clip_t));
      if(clip == NULL) return _hip_err(_NO_HEAP);
      err = clip_init(plug, clip);
      if(err != SCE_HIG_NO_ERR){
	  sceHiMemFree((u_int *)((u_int)clip & 0x0fffffff));
	  return err;
      }
      plug->args = NULL;
      plug->stack = (u_int)clip;
      break;

    case SCE_HIG_PRE_PROCESS:
      clip = (Clip_t *)plug->stack;
      if(clip == NULL) return _hip_err(_NO_STACK);
      clip_calc(clip);
      plug->args = NULL;
      break;

    case SCE_HIG_POST_PROCESS:
      break;

    case SCE_HIG_END_PROCESS:
      clip = (Clip_t *)plug->stack;
      if(clip == NULL) return _hip_err(_NO_STACK);
      sceHiMemFree((u_int *)((u_int)clip & 0x0fffffff));
      plug->stack = NULL;
      plug->args = NULL;
      break;

    default:
      break;
    }    

    return SCE_HIG_NO_ERR;
}
