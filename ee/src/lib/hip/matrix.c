/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0.2
 */
/*
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: matrix.c,v 1.3 2003/01/28 05:26:53 xkazama Exp $	*/
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
    _NO_MATRIXDATA,
    _NO_BASEMATRIX,
    _NO_STACK
} _local_err;

static const sceHiErr _local2hierr[] = {
    SCE_HIG_NO_HEAP,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA
};

static const char *_err_mes[] = {
    "HiP Matrix : can't allocate memory\n",
    "HiP Matrix : can't find MATRIXDATA\n",
    "HiP Matrix : can't find BASEMATRIX\n",
    "HiP Matrix : plug stack is null\n"
};

/* error handling */
static sceHiErr _hip_err(_local_err err)
{
    sceHiErrStateType *s = sceHiGetErrStatePtr();

    s->mes = _err_mes[err];
    return _local2hierr[err];
}

/********************************************************/
/*		Internal Structure			*/
/********************************************************/
typedef struct{
    int				num;
    sceHiPlugHrchyMatrix_t	*hm;
    sceHiPlugShapeMatrix_t	*sm;
    sceVu0FMATRIX		*root;
} Matrix_t;

static void sub_func(Matrix_t *mtx, sceVu0FMATRIX m, int idx)
{
    sceHiPlugHrchyMatrix_t *m0;
    sceHiPlugShapeMatrix_t *m1;
    int i;

    m0 = mtx->hm + idx;
    m1 = mtx->sm + idx;

    sceVu0MulMatrix(m1->local, m, m0->matrix);
    sceVu0CopyMatrix(m1->light, m1->local);
    __asm__ __volatile__("sqc2    vf0,0x30(%0)": : "r" (m1->light) : "memory");

    for(i=0;i<mtx->num;i++){
	m0 = mtx->hm + i;
	if(m0->parent == idx){
	    sub_func(mtx, m1->local, i);
	}
    }
}

/********************************************************/
/*		Pre Func				*/
/********************************************************/
static void pre_func(Matrix_t *mtx)
{
    int i;
    sceVu0FMATRIX m;
    sceHiPlugHrchyMatrix_t *m0;

    for(i=0;i<mtx->num;i++){
	m0 = mtx->hm + i;
	if(m0->parent == -1){	/* root */
	    if(mtx->root){
		sceVu0CopyMatrix(m, *mtx->root);
	    }
	    else{
		sceVu0UnitMatrix(m);
	    }
	    sub_func(mtx, m, i);
	}
    }
}

/********************************************************/
/*		Init Func				*/
/********************************************************/
static sceHiErr init_func(sceHiPlug *p, Matrix_t *mtx)
{
    static const sceHiType t[2] = {
	{SCE_HIP_COMMON,SCE_HIP_FRAMEWORK,SCE_HIP_HRCHY,SCE_HIG_DATA_STATUS,SCE_HIP_MATRIXDATA,SCE_HIP_REVISION},
	{SCE_HIP_COMMON,SCE_HIP_FRAMEWORK,SCE_HIP_SHAPE,SCE_HIG_DATA_STATUS,SCE_HIP_BASEMATRIX,SCE_HIP_REVISION}
    };
    sceHiPlugHrchyHead_t *hh;
    sceHiPlugShapeHead_t *sh;

    hh = sceHiPlugHrchyGetHead(p, t[0]);
    if (hh == NULL) return _hip_err(_NO_MATRIXDATA);
    mtx->num = hh->num;
    mtx->hm = sceHiPlugHrchyGetMatrix(hh, 0);

    sh = sceHiPlugShapeGetHead(p, t[1]);
    if(sh == NULL) return _hip_err(_NO_BASEMATRIX);
    mtx->sm = sceHiPlugShapeGetMatrix(sh, 0);

    return SCE_HIG_NO_ERR;
}

/********************************************************/
/*		Main Func				*/
/********************************************************/
sceHiErr sceHiPlugMatrix(sceHiPlug *plug, int process)
{
    sceHiErr	err;
    Matrix_t	*mtx;

    switch(process){
    case SCE_HIG_INIT_PROCESS:
      mtx = (Matrix_t *)sceHiMemAlign(16, sizeof(Matrix_t));
      if(mtx == NULL) return _hip_err(_NO_HEAP);
      err = init_func(plug, mtx);
      if(err != SCE_HIG_NO_ERR){
	  sceHiMemFree((u_int *)((u_int)mtx & 0x0fffffff));
	  return err;
      }
      plug->stack = (u_int)mtx;
      break;

    case SCE_HIG_PRE_PROCESS:
      mtx = (Matrix_t *)plug->stack;
      if(mtx == NULL) return _hip_err(_NO_STACK);
      if(plug->args)
	  mtx->root = ((sceHiPlugHrchyPreCalcArg_t *)(plug->args))->root;
      else
	  mtx->root = NULL;
      pre_func(mtx);
      break;

    case SCE_HIG_POST_PROCESS:
      break;

    case SCE_HIG_END_PROCESS:
      mtx = (Matrix_t *)plug->stack;
      if(mtx == NULL) return _hip_err(_NO_STACK);
      sceHiMemFree((u_int *)((u_int)mtx & 0x0fffffff));
      plug->stack = NULL;
      plug->args = NULL;
      break;

    default:
      break;
    }    

    return SCE_HIG_NO_ERR;
}
