/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4.2
 */
/*
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: fisheye.c,v 1.1 2001/05/20 14:04:00 aoki Exp $	*/
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
} FishEye;

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
    _NO_MICRO,
    _MICRO_DATA_ERR,
    _PLGBLK_STACK_BROKEN
} _local_err;

static const sceHiErr _local2higerr[] = {
    SCE_HIG_NO_HEAP,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
};

static const char *_fish_err_mes[] = {
    "HiP FishEye : can't allocate memory from HiG Heap\n",
    "HiP FishEye : can't find micro-plug",
    "HiP FishEye : can't find micro-data",
    "HiP FishEye : the plugin's stack is broken\n"
};

/* error handling */
static sceHiErr _hip_fish_err(_local_err err)
{
    sceHiErrState.mes = _fish_err_mes[err];
    return _local2higerr[err];
}
/************************************************/
/*		FishEye Calc			*/
/************************************************/
static void FishEyeCalc(FishEye *fisheye, sceHiPlug *plug)
{
    sceVu0FVECTOR               *camera_pos;
    sceVu0FVECTOR               *camera_zdir;
    sceVu0FVECTOR               *camera_up;
    sceVu0FMATRIX               world_view;
    float	tex_size;
    u_int *microD;
    micro_data_t                *mdata;

    camera_pos=((sceHiPlugFishEyePreArg_t *)plug->args)->camera_pos; 
    camera_zdir=((sceHiPlugFishEyePreArg_t *)plug->args)->camera_zdir;
    camera_up=((sceHiPlugFishEyePreArg_t *)plug->args)->camera_up; 
    tex_size=((sceHiPlugFishEyePreArg_t *)plug->args)->tex_size;
    sceVu0CameraMatrix(world_view, *camera_pos, *camera_zdir, *camera_up);
    microD=fisheye->microD;
    sceVu0CopyMatrix((float (*)[4])(&microD[(sizeof(sceVu0FMATRIX)>>2)]), world_view);

    mdata = (micro_data_t *)microD;
    sceVu0CopyVector(mdata->init_data[0], *camera_pos);
    mdata->init_data[2][0] = tex_size/2.0f;
}

/************************************************/
/*		FishEye Init			*/
/************************************************/
static sceHiErr FishEyeInit(FishEye *fisheye, sceHiPlug *plug)
{
    sceHiErr	err;
    sceHiType	type;
    sceHiPlug	*microP;
    u_int 	*microD;
    micro_data_t                *mdata;
    float rmin,rmax,near,far,ZA,ZB;
    u_int zdepth;

    type.repository = SCE_HIP_COMMON;
    type.project = SCE_HIP_FRAMEWORK;
    type.status = SCE_HIG_PLUGIN_STATUS;
    type.revision = SCE_HIP_REVISION;
    type.category = SCE_HIP_MICRO;
    type.id = SCE_HIP_MICRO_PLUG;
    err = sceHiGetInsPlug(plug, &microP, type);
    if(err != SCE_HIG_NO_ERR) { return _hip_fish_err(_NO_MICRO);}

    type.status = SCE_HIG_DATA_STATUS;
    type.revision = SCE_HIP_REVISION;
    type.id = SCE_HIP_MICRO_DATA;
    err=sceHiGetData(microP, &microD, type);
    if(err != SCE_HIG_NO_ERR) { return _hip_fish_err(_MICRO_DATA_ERR);}
    fisheye->microD=microD;

    if (plug->args!=NULL) {
	zdepth=((sceHiPlugFishEyeInitArg_t *)plug->args)->zdepth;
	rmax=((sceHiPlugFishEyeInitArg_t *)plug->args)->rmax;
	rmin=((sceHiPlugFishEyeInitArg_t *)plug->args)->rmin;
	switch(zdepth) {
	  case SCE_GS_PSMZ32:
	      far=4294967296;
	      break;
	  case SCE_GS_PSMZ24:
	      far=16777216;
	      break;
	  case SCE_GS_PSMZ16:
	  case SCE_GS_PSMZ16S:
	      far=65536;
	      break;
	  default:
	      far=16777216;
	      break;
	}
    } else {
	far=16777216;
	rmax=16000000;
	rmin=0.0;
    }

    near=1;
    ZA=(rmax*near-rmin*far)/(near-far);
    ZB=-(rmax*near*far-rmin*near*far)/(near-far);
    microD=fisheye->microD;
    mdata = (micro_data_t *)microD;
    mdata->init_data[2][1] = ZA;
    mdata->init_data[2][2] = ZB;

    return SCE_HIG_NO_ERR;
}

/************************************************/
/*		FishEye Plug			*/
/************************************************/
sceHiErr sceHiPlugFishEye(sceHiPlug *plug, int process)
{
    sceHiErr err;
    FishEye *fisheye;

    switch(process){
    case SCE_HIG_INIT_PROCESS:
      fisheye = (FishEye *)sceHiMemAlign(16, sizeof(FishEye));
      if(fisheye == NULL) { return _hip_fish_err(_NO_HEAP); }
      err = FishEyeInit(fisheye, plug);
      if(err != SCE_HIG_NO_ERR){
	  sceHiMemFree((u_int *)Paddr(fisheye));
	  return err;
      }
      plug->args = NULL;
      plug->stack = (u_int)fisheye;

      break;
    case SCE_HIG_PRE_PROCESS:
      fisheye=(FishEye *)plug->stack;
      if(fisheye == NULL) return _hip_fish_err(_PLGBLK_STACK_BROKEN);
      if(plug->args != NULL){
	  FishEyeCalc(fisheye, plug);
      }
      plug->args=NULL;
      break;
  
    case SCE_HIG_POST_PROCESS:
      break;
        
    case SCE_HIG_END_PROCESS:
      fisheye=(FishEye *)plug->stack;
      if(fisheye == NULL) return _hip_fish_err(_PLGBLK_STACK_BROKEN);
      sceHiMemFree((u_int *)Paddr(fisheye));
      plug->args=NULL;
      plug->stack = NULL;
     break;
    }
    
    return SCE_HIG_NO_ERR;
}


