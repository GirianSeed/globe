/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/*
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: shadowbox.c,v 1.1 2001/05/20 14:04:00 aoki Exp $	*/
#include <eekernel.h>
#include <stdlib.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>

/***************************************************
 * Local Errors
 ***************************************************/
typedef enum {
    _NO_HEAP,
    _NO_BOX_DATA,
    _NO_BASEMATRIX,
    _NO_STACK,
} _local_err;

static const sceHiErr _local2higerr[] = {
    SCE_HIG_NO_HEAP,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
};

static const char *_err_mes[] = {
    "HiP ShadowBox : can't allocate memory\n",
    "HiP ShadowBox : can't find shadowbox data\n",
    "HiP ShadowBox : can't find basematrix data\n",
    "HiP ShadowBox : plugin stack is null\n",
};

/* error handling */
static sceHiErr _hip_err(_local_err err)
{
    sceHiErrState.mes = _err_mes[err];
    return _local2higerr[err];
}

/*******************************************************
 *	ShadowBox Structure
 *******************************************************/
#define VN 8	/* num of box vertices */
typedef struct {
    sceVu0FVECTOR	min;
    sceVu0FVECTOR	max;
    sceVu0FVECTOR	box[VN];
}Box_t;

typedef struct {
    Box_t		*box;
    sceVu0FMATRIX	*mat;
}ShadowBox;

#define QWSIZE	4
#define NUMIDX	3


/*******************************************************
 *	ShadowBox Calc
 *******************************************************/
static void shadowbox_calc(ShadowBox *sbox)
{
    int i;
    sceVu0FVECTOR box[VN] = {
	{ sbox->box->max[0], sbox->box->min[1], sbox->box->max[2], 1.0f },
	{ sbox->box->max[0], sbox->box->max[1], sbox->box->max[2], 1.0f },
	{ sbox->box->max[0], sbox->box->min[1], sbox->box->min[2], 1.0f },
	{ sbox->box->max[0], sbox->box->max[1], sbox->box->min[2], 1.0f },
	{ sbox->box->min[0], sbox->box->min[1], sbox->box->max[2], 1.0f },
	{ sbox->box->min[0], sbox->box->max[1], sbox->box->max[2], 1.0f },
	{ sbox->box->min[0], sbox->box->min[1], sbox->box->min[2], 1.0f },
	{ sbox->box->min[0], sbox->box->max[1], sbox->box->min[2], 1.0f },
    };

    for(i=0;i<VN;i++){
	sceVu0ApplyMatrix(sbox->box->box[i], *sbox->mat, box[i]);
    }
}
/*******************************************************
 *	ShadowBox Plug
 *******************************************************/
sceHiErr sceHiPlugShadowBox(sceHiPlug *plug, int process)
{
    sceHiErr	err;
    sceHiType	type;
    u_int	*data;
    ShadowBox	*sbox;
    u_int	*boxP;
    u_int	*matP;

    switch(process){
      case SCE_HIG_INIT_PROCESS:
	  type.repository = SCE_HIP_COMMON;
	  type.project = SCE_HIP_FRAMEWORK;
	  type.category = SCE_HIP_SHADOW;
	  type.status = SCE_HIG_DATA_STATUS;
	  type.id = SCE_HIP_SHADOWBOX_DATA;
	  type.revision = SCE_HIP_REVISION;
	  err = sceHiGetData(plug, &data, type);
	  if(err != SCE_HIG_NO_ERR)	return _hip_err(_NO_BOX_DATA);
	  boxP = data;

	  type.category = SCE_HIP_SHAPE;
	  type.id = SCE_HIP_BASEMATRIX;
	  err = sceHiGetData(plug, &data, type);
	  if(err != SCE_HIG_NO_ERR)	return _hip_err(_NO_BASEMATRIX);
	  matP = data;

	  sbox = (ShadowBox *)sceHiMemAlign(16, sizeof(ShadowBox));
	  if(sbox == NULL)		return _hip_err(_NO_HEAP);
	  sbox->box = (Box_t *)(boxP);
	  sbox->mat = (sceVu0FMATRIX *)(matP+QWSIZE*2);		/* only root matrix */

	  plug->args = NULL;
	  plug->stack = (u_int)sbox;
	  break;

      case SCE_HIG_PRE_PROCESS:
	  sbox = (ShadowBox *)plug->stack;
	  if(sbox == NULL)		return _hip_err(_NO_STACK);
	  shadowbox_calc(sbox);
	  break;

      case SCE_HIG_END_PROCESS:
	  sbox = (ShadowBox *)plug->stack;
	  if(sbox == NULL)		return _hip_err(_NO_STACK);
	  sceHiMemFree((u_int *)((u_int)sbox & 0x0fffffff));
	  plug->stack = NULL;
	  plug->args = NULL;
	  break;

      default:
	  break;
    }

    return SCE_HIG_NO_ERR;
}
