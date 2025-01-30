/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4.2
 */
/*
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: lightmap.c,v 1.2 2001/05/20 23:40:44 kaneko Exp $	*/
#include <eekernel.h>
#include <stdlib.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>
#include <math.h>

/***************************************************
 * Local Errors
 ***************************************************/
typedef enum {
    _NO_HEAP,
    _NO_MICRO_PLUG,
    _NO_MICRO_DATA,
    _NO_STACK,
    _NO_ARGS,
} _local_err;

static const sceHiErr _local2higerr[] = {
    SCE_HIG_NO_HEAP,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_VALUE,
};

static const char *_err_mes[] = {
    "HiP LightMap : can't allocate memory\n",
    "HiP LightMap : can't find micro plug\n",
    "HiP LightMap : can't find micro data\n",
    "HiP LightMap : plugin stack is null\n",
    "HiP LightMap : plugin args is null\n",
};

/* error handling */
static sceHiErr _hip_err(_local_err err)
{
    sceHiErrState.mes = _err_mes[err];
    return _local2higerr[err];
}

/*******************************************************
 *	LightMap Structure
 *******************************************************/
typedef struct {
    sceVu0FMATRIX	vector;
    sceVu0FMATRIX	point;
    sceVu0FMATRIX	color;
} micro_light_t;
 
typedef struct {
    sceVu0FMATRIX       wscr;
    sceVu0FMATRIX       wclp;
    sceVu0FMATRIX       view;
    sceVu0FMATRIX       init_data;
    micro_light_t       lightgrp[3];
} micro_data_t;

typedef struct {
    int			width;
    int			height;
    int			fov;	/* TRUE:width , FALSE:height */
    u_int		*mdata;
}LightMap;

enum {DIR, POINT, SPOT};

/********************************************************/
/*	LightMap View					*/
/********************************************************/
static void lightmap_view(LightMap *lightmap)
{
    sceVu0FMATRIX m,tm;
    sceVu0FVECTOR yd = { 0.0f, 1.0f, 0.0f, 1.0f };
    sceVu0FVECTOR vec;
    float scrz,ax,ay;
    float angle;
    int i;

    micro_data_t *mdata = (micro_data_t *)lightmap->mdata;

    angle = acosf(mdata->lightgrp[DIR].point[0][3]);		/* spot angle */
    if(lightmap->fov){
	scrz = ((float)lightmap->width/2.0f)/(tanf(angle));
	ax = 1.0f;
	ay = (float)lightmap->height/(float)lightmap->width;
    }
    else{
	scrz = ((float)lightmap->height/2.0f)/(tanf(angle));
	ax = (float)lightmap->width/(float)lightmap->height;
	ay = 1.0f;
    }

    for(i=0;i<4;i++){
	vec[i] = mdata->lightgrp[DIR].vector[i][0];		/* spot vector */
    }
    sceVu0ScaleVector(vec,vec,-1.0f);

    sceVu0UnitMatrix(m);
    sceVu0CameraMatrix(m, mdata->lightgrp[DIR].point[0], vec, yd);
    sceVu0UnitMatrix(tm);
    tm[0][0] = scrz * 0.5f * ax;
    tm[1][1] = scrz * 0.5f * ay;
    tm[2][0] = 0.5f;
    tm[2][1] = 0.5f;
    tm[2][2] = 0.0f;
    tm[2][3] = 1.0f;
    tm[3][2] = 0.0f;
    tm[3][3] = 0.0f;

    sceVu0MulMatrix(tm, tm, m);

    sceVu0CopyMatrix(mdata->wclp, tm);				/* projtex matrix */
}
/*******************************************************
 *	LightMap Plug		       
 *******************************************************/
sceHiErr sceHiPlugLightMap(sceHiPlug *plug, int process)
{
    sceHiErr	err;
    sceHiType	type;
    sceHiPlug	*micro;
    u_int	*mdata;
    LightMap	*lightmap;

    switch(process){
      case SCE_HIG_INIT_PROCESS:
	  type.repository = SCE_HIP_COMMON;
	  type.project = SCE_HIP_FRAMEWORK;
	  type.category = SCE_HIP_MICRO;
	  type.status = SCE_HIG_PLUGIN_STATUS;
	  type.id = SCE_HIP_MICRO_PLUG;
	  type.revision = SCE_HIP_REVISION;
	  err = sceHiGetInsPlug(plug, &micro, type);
	  if(err != SCE_HIG_NO_ERR) return _hip_err(_NO_MICRO_PLUG);

	  type.status = SCE_HIG_DATA_STATUS;
	  type.id = SCE_HIP_MICRO_DATA;
	  err = sceHiGetData(micro, &mdata, type);
	  if(err != SCE_HIG_NO_ERR) return _hip_err(_NO_MICRO_DATA);

	  lightmap = (LightMap *)sceHiMemAlign(16, sizeof(LightMap));
	  if(lightmap == NULL) return _hip_err(_NO_HEAP);
	  lightmap->mdata = mdata;

	  if(plug->args != NULL){
	      lightmap->width = ((sceHiPlugLightMapInitArg_t *)plug->args)->width;
	      lightmap->height = ((sceHiPlugLightMapInitArg_t *)plug->args)->height;
	      lightmap->fov = ((sceHiPlugLightMapInitArg_t *)plug->args)->fov;
	  }
	  else{
	      sceHiMemFree((u_int *)((u_int)lightmap & 0x0fffffff));
	      return _hip_err(_NO_ARGS);
	  }

	  plug->args = NULL;
	  plug->stack = (u_int)lightmap;
	  break;

      case SCE_HIG_PRE_PROCESS:
	  lightmap = (LightMap *)plug->stack;
	  if(lightmap == NULL)	return _hip_err(_NO_STACK);
	  lightmap_view(lightmap);
	  break;

      case SCE_HIG_POST_PROCESS:
	  break;

      case SCE_HIG_END_PROCESS:
	  lightmap = (LightMap *)plug->stack;
	  if(lightmap == NULL)	return _hip_err(_NO_STACK);
	  sceHiMemFree((u_int *)((u_int)lightmap & 0x0fffffff));
	  plug->stack = NULL;
	  plug->args = NULL;
	  break;

      default:
	  break;
    }

    return SCE_HIG_NO_ERR;
}
