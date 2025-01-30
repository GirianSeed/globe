/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4.2
 */
/*
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: reflect.c,v 1.1 2001/05/20 14:04:00 aoki Exp $	*/
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
} Reflect;

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
    "HiP Reflect : can't allocate memory from HiG Heap\n",
    "HiP Reflect : can't find micro-plug",
    "HiP Reflect : can't find micro-data",
    "HiP Reflect : plugin's stack is null\n"
};

/* error handling */
static sceHiErr _hip_refle_err(_local_err err)
{
    sceHiErrState.mes = _refle_err_mes[err];
    return _local2higerr[err];
}

/************************************************/
/*		Reflect Calc			*/
/************************************************/
static void ReflectCalc(Reflect *reflect, sceHiPlug *plug)
{
    u_int		*microD;
    sceVu0FVECTOR	*camera_pos;
    sceVu0FVECTOR	*camera_zdir;
    sceVu0FVECTOR	*camera_up;
    sceVu0FVECTOR	vertical;
    sceVu0FMATRIX	world_view;
    micro_data_t	*mdata;
 
    camera_pos=((sceHiPlugReflectPreArg_t *)plug->args)->camera_pos; 
    camera_zdir=((sceHiPlugReflectPreArg_t *)plug->args)->camera_zdir;
    camera_up=((sceHiPlugReflectPreArg_t *)plug->args)->camera_up;
    sceVu0ScaleVector(vertical,*camera_up,-1.0f);
    sceVu0CameraMatrix(world_view, *camera_pos, *camera_zdir, vertical);
    microD=reflect->microD;
    sceVu0CopyMatrix((float (*)[4])(&microD[(sizeof(sceVu0FMATRIX)>>2)]), world_view);

    mdata = (micro_data_t *)microD;
    mdata->init_data[3][2]=((sceHiPlugReflectPreArg_t *)plug->args)->zoom;
    mdata->init_data[3][3]=((sceHiPlugReflectPreArg_t *)plug->args)->z_shift;
}

/************************************************/
/*		Reflect Init			*/
/************************************************/
static sceHiErr ReflectInit(Reflect *reflect, sceHiPlug *plug)
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
    reflect->microD=microD;

    return SCE_HIG_NO_ERR;
}

/************************************************/
/*		Reflect Plug			*/
/************************************************/
sceHiErr sceHiPlugReflect(sceHiPlug *plug, int process)
{
    sceHiErr err;
    Reflect *reflect;

    switch(process){
      case SCE_HIG_INIT_PROCESS:
	  reflect = (Reflect *)sceHiMemAlign(16, sizeof(Reflect));
	  if(reflect == NULL) { return _hip_refle_err(_NO_HEAP); }
	  err = ReflectInit(reflect, plug);
	  if(err != SCE_HIG_NO_ERR){
	     sceHiMemFree((u_int *)Paddr(reflect));
	     return err;
	  }
	  plug->stack = (u_int)reflect;
	  plug->args = NULL;
	  break;

      case SCE_HIG_PRE_PROCESS:
	  reflect=(Reflect *)plug->stack;
	  if(reflect == NULL) return _hip_refle_err(_STACK_NULL);
	  if(plug->args != NULL){
	      ReflectCalc(reflect, plug);
	  }
	  plug->args = NULL;
	  break;

      case SCE_HIG_POST_PROCESS:
	  break;

      case SCE_HIG_END_PROCESS:
	  reflect=(Reflect *)plug->stack;
	  if(reflect == NULL) return _hip_refle_err(_STACK_NULL);
	  sceHiMemFree((u_int *)Paddr(reflect));
	  plug->stack = NULL;
	  plug->args = NULL;
	  break;
    }

    return SCE_HIG_NO_ERR;
}


