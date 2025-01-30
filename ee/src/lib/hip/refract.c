/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/*
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: refract.c,v 1.1 2001/05/20 14:04:00 aoki Exp $	*/
#include <eekernel.h>
#include <stdlib.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>
#include <math.h>


typedef struct{
  u_int         *microD;
  int zdepth;
  int color_format;
  int tex_size;
} Refract;

#define PADDR_MASK 0x0fffffff
#define Paddr(x) ((u_int)(x)&PADDR_MASK)

typedef struct {
    sceVu0FMATRIX       vector;
    sceVu0FMATRIX       point;
    sceVu0FMATRIX       color;
} micro_light_t;
 
typedef struct {
    sceVu0FMATRIX       wscr;
    sceVu0FMATRIX       wclp;
    sceVu0FMATRIX       cscr;
    sceVu0FMATRIX       init_data;
    micro_light_t       lightgrp[3];
} micro_data_t;            

/***************************************************
 * Local Errors
 ***************************************************/
typedef enum {
    _NO_HEAP,
    _NO_MICRO_PLUG,
    _NO_MICRO_DATA,
    _STACK_NULL
} _local_err;

static const sceHiErr _local2higerr[] = {
    SCE_HIG_NO_HEAP,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
};

static const char *_refle_err_mes[] = {
    "HiP Refract : can't allocate memory from HiG Heap\n",
    "HiP Refract : can't find micro-plug",
    "HiP Refract : can't find micro-data",
    "HiP Refract : plugin's stack is null\n"
};

/* error handling */
static sceHiErr _hip_refle_err(_local_err err)
{
    sceHiErrState.mes = _refle_err_mes[err];
    return _local2higerr[err];
}

/************************************************/
/*		Refract Calc			*/
/************************************************/
static void RefractCalc(Refract *refract, sceHiPlug *plug)
{
    u_int		*microD;
    sceVu0FVECTOR	*camera_pos;
    sceVu0FVECTOR	*camera_zdir;
    sceVu0FVECTOR	*camera_up;
    sceVu0FVECTOR	vertical;
    sceVu0FMATRIX	world_view;
    micro_data_t	*mdata;
 
    camera_pos=((sceHiPlugRefractPreArg_t *)plug->args)->camera_pos; 
    camera_zdir=((sceHiPlugRefractPreArg_t *)plug->args)->camera_zdir;
    camera_up=((sceHiPlugRefractPreArg_t *)plug->args)->camera_up;
    sceVu0ScaleVector(vertical,*camera_up,-1.0f);
    sceVu0CameraMatrix(world_view, *camera_pos, *camera_zdir, vertical);
    microD=refract->microD;
    sceVu0CopyMatrix((float (*)[4])(&microD[(sizeof(sceVu0FMATRIX)>>2)]), world_view);

    mdata = (micro_data_t *)microD;
    mdata->init_data[3][1]=((sceHiPlugRefractPreArg_t *)plug->args)->refract_index;
    mdata->init_data[3][2]=((sceHiPlugRefractPreArg_t *)plug->args)->zoom;
    mdata->init_data[3][3]=((sceHiPlugRefractPreArg_t *)plug->args)->z_shift;
}

/************************************************/
/*		Refract Init			*/
/************************************************/
static sceHiErr RefractInit(Refract *refract, sceHiPlug *plug)
{
    sceHiType	type;
    sceHiErr	err;
    sceHiPlug	*microP;
    u_int	*microD;

    type.repository = SCE_HIP_COMMON;
    type.project = SCE_HIP_FRAMEWORK;
    type.status = SCE_HIG_PLUGIN_STATUS;
    type.revision = SCE_HIP_REVISION;
    type.category = SCE_HIP_MICRO;
    type.id = SCE_HIP_MICRO_PLUG;
    err = sceHiGetInsPlug(plug, &microP, type);
    if(err != SCE_HIG_NO_ERR) { return _hip_refle_err(_NO_MICRO_PLUG); }

    type.status = SCE_HIG_DATA_STATUS;
    type.id = SCE_HIP_MICRO_DATA;
    err=sceHiGetData(microP, &microD, type);
    if(err != SCE_HIG_NO_ERR) { return _hip_refle_err(_NO_MICRO_DATA); }
    refract->microD=microD;

    return SCE_HIG_NO_ERR;
}

/************************************************/
/*		Refract Plug			*/
/************************************************/
sceHiErr sceHiPlugRefract(sceHiPlug *plug, int process)
{
    sceHiErr err;
    Refract *refract;

    switch(process){
      case SCE_HIG_INIT_PROCESS:
	  refract = (Refract *)sceHiMemAlign(16, sizeof(Refract));
	  if(refract == NULL) { return _hip_refle_err(_NO_HEAP); }
	  err = RefractInit(refract, plug);
	  if(err != SCE_HIG_NO_ERR){
	     sceHiMemFree((u_int *)Paddr(refract));
	     return err;
	  }
	  plug->stack = (u_int)refract;
	  plug->args = NULL;
	  break;

      case SCE_HIG_PRE_PROCESS:
	  refract=(Refract *)plug->stack;
	  if(refract == NULL) return _hip_refle_err(_STACK_NULL);
	  if(plug->args != NULL){
	      RefractCalc(refract, plug);
	  }
	  plug->args = NULL;
	  break;

      case SCE_HIG_POST_PROCESS:
	  break;

      case SCE_HIG_END_PROCESS:
	  refract=(Refract *)plug->stack;
	  if(refract == NULL) return _hip_refle_err(_STACK_NULL);
	  sceHiMemFree((u_int *)Paddr(refract));
	  plug->stack = NULL;
	  plug->args = NULL;
	  break;
    }

    return SCE_HIG_NO_ERR;
}


