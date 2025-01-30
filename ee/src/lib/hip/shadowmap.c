/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/*
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: shadowmap.c,v 1.3 2001/07/17 12:15:55 kaneko Exp $	*/
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
    _NO_MICRO_PLUG,
    _NO_MICRO_DATA,
    _NO_STACK,
    _NO_ARGS,
    _NO_BOX_DATA
} _local_err;

static const sceHiErr _local2higerr[] = {
    SCE_HIG_NO_HEAP,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_VALUE,
    SCE_HIG_INVALID_DATA
};

static const char *_err_mes[] = {
    "HiP ShadowMap : can't allocate memory\n",
    "HiP ShadowMap : can't find micro plug\n",
    "HiP ShadowMap : can't find micro data\n",
    "HiP ShadowMap : plugin stack is null\n",
    "HiP ShadowMap : plugin args is null\n",
    "HiP ShadowMap : can't find shadowbox data\n"
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
#define VN	8	/* num of bounding box vertices */
typedef struct {
    sceVu0FVECTOR	min;
    sceVu0FVECTOR	max;
    sceVu0FVECTOR	box[VN];
}Box_t;

/*******************************************************
 *	ShadowMap Structure
 *******************************************************/
typedef struct {
    sceVu0FMATRIX	vector;
    sceVu0FMATRIX	point;
    sceVu0FMATRIX	color;
} micro_light_t;
 
typedef struct {
    sceVu0FMATRIX       wscr;
    sceVu0FMATRIX       wclp;
    sceVu0FMATRIX       cscr;
    sceVu0FMATRIX       init_data;
    micro_light_t       lightgrp[3];
} micro_data_t;

typedef struct {
    int			width;	/* shadowtex width */
    int			height;	/* shadowtex height */
    u_int		*mdata;	/* shadowmap micro data for texture */
    u_int		*rdata;	/* shadowmap micro data for mapping */
    u_int		*box;	/* shadowobj bbox */
}ShadowMap;

enum {DIR, POINT, SPOT};
/********************************************************/
/*	ShadowMap View					*/
/********************************************************/
#define ABS(x)	(((x<0.0f)?(-x):(x)))
static void shadowmap_view(ShadowMap *shadowmap)
{
    int i;
    sceVu0FVECTOR LX,LY,LZ;	/* light coordinate system */
    sceVu0FMATRIX m;		/* model view matrix */
    float Rx,Ry;		/* reciever x,y */
    sceVu0FMATRIX m0,m1;	/* projection matrix */
    micro_data_t *mdata = (micro_data_t *)shadowmap->mdata;
    micro_data_t *rdata = (micro_data_t *)shadowmap->rdata;
    Box_t	*box = (Box_t *)shadowmap->box;
    sceVu0FVECTOR V;

    /*	average light -> shadowobj direction	*/
    sceVu0ScaleVector(LZ, LZ, 0.0f);
    for(i=0;i<VN;i++){
	sceVu0CopyVector(V, box->box[i]);
	sceVu0SubVector(V, V, mdata->lightgrp[DIR].point[0]);
	sceVu0AddVector(LZ, LZ, V);
    }
    LZ[3] = 0.0f;
    sceVu0DivVector(LZ, LZ, (float)VN);
    sceVu0Normalize(LZ, LZ);

    if(ABS(LZ[0])<=ABS(LZ[1])){
	if(ABS(LZ[2])<=ABS(LZ[0])) { V[0] = 0.0f; V[1] = 0.0f; V[2] = 1.0f; }
	else { V[0] = 1.0f; V[1] = 0.0f; V[2] = 0.0f; }
    }
    else{
	if(ABS(LZ[2])<=ABS(LZ[1])) { V[0] = 0.0f; V[1] = 0.0f; V[2] = 1.0f; }
	else { V[0] = 0.0f; V[1] = 1.0f; V[2] = 0.0f; }
    }

    sceVu0OuterProduct(LX, LZ,V);
    sceVu0Normalize(LX, LX);
    sceVu0OuterProduct(LY, LZ, LX);
    sceVu0CameraMatrix(m, mdata->lightgrp[DIR].point[0], LZ, LY);

    Rx = Ry = 0.0f;
    for(i=0;i<VN;i++){
        sceVu0CopyVector(V, box->box[i]);
        sceVu0ApplyMatrix(V, m, V);
        if(V[2] == 0.0f){
            V[0] = V[1] = 0.0f;
        }

        else{
            V[0] = V[0]/V[2];
            V[1] = V[1]/V[2];
        }

        if(Rx < ABS(V[0]))      Rx = ABS(V[0]);
        if(Ry < ABS(V[1]))      Ry = ABS(V[1]);
    }

    sceVu0UnitMatrix(m0);
    m0[0][0] = (float)(shadowmap->width) * 0.49f / Rx;  /* for clamp */
    m0[1][1] = (float)(shadowmap->height) * 0.49f / Ry;  /* for clamp */ 
    m0[2][0] = 2048.0f;
    m0[2][1] = 2048.0f;
    m0[2][2] = 0.0f;
    m0[2][3] = 1.0f;
    m0[3][2] = 0.0f;
    m0[3][3] = 0.0f;
    sceVu0MulMatrix(m0,m0,m);

    sceVu0UnitMatrix(m1);
    m1[0][0] = 0.5f / Rx;
    m1[1][1] = 0.5f / Ry;
    m1[2][0] = 0.5f;
    m1[2][1] = 0.5f;
    m1[2][2] = 0.0f;
    m1[2][3] = 1.0f;
    m1[3][2] = 0.0f;
    m1[3][3] = 0.0f;
    sceVu0MulMatrix(m1, m1, m);

    sceVu0CopyMatrix(mdata->wscr, m0);				/* shadowmap projection matrix */
    sceVu0CopyMatrix(rdata->wclp, m1);				/* shadowmap texture matrix for reciever */
    sceVu0CopyVector(rdata->lightgrp[DIR].vector[0], LZ);	/* light dir for reciever */
}

/*******************************************************
 *	ShadowMap Init
 *******************************************************/
static sceHiErr shadowmap_init(ShadowMap *shadowmap, sceHiPlug *plug)
{
    sceHiErr	err;
    sceHiType	type;
    sceHiPlug	*micro;
    u_int	*data;

    type.repository = SCE_HIP_COMMON;
    type.project = SCE_HIP_FRAMEWORK;
    type.category = SCE_HIP_MICRO;
    type.status = SCE_HIG_PLUGIN_STATUS;
    type.id = SCE_HIP_MICRO_PLUG;
    type.revision = SCE_HIP_REVISION;
    err = sceHiGetInsPlug(plug, &micro, type);
    if(err != SCE_HIG_NO_ERR)	return _hip_err(_NO_MICRO_PLUG);
    type.status = SCE_HIG_DATA_STATUS;
    type.id = SCE_HIP_MICRO_DATA;
    err = sceHiGetData(micro, &data, type);
    if(err != SCE_HIG_NO_ERR)	return _hip_err(_NO_MICRO_DATA);
    shadowmap->mdata = data;
    err = sceHiGetData(plug, &data, type);
    if(err != SCE_HIG_NO_ERR)	return _hip_err(_NO_MICRO_DATA);
    shadowmap->rdata = data;

    if(plug->args != NULL){
	shadowmap->width = ((sceHiPlugShadowMapInitArg_t *)plug->args)->width;
	shadowmap->height = ((sceHiPlugShadowMapInitArg_t *)plug->args)->width;
	shadowmap->box = ((sceHiPlugShadowMapInitArg_t *)plug->args)->box;
    }
    else	return _hip_err(_NO_ARGS);

    if(shadowmap->box == NULL){
	type.category = SCE_HIP_SHADOW;
	type.status = SCE_HIG_PLUGIN_STATUS;
	type.id = SCE_HIP_SHADOWBOX_DATA;
	err = sceHiGetData(plug, &data, type);
	if(err != SCE_HIG_NO_ERR)	return _hip_err(_NO_BOX_DATA);
	shadowmap->box = data;
    }

    return SCE_HIG_NO_ERR;
}

/*******************************************************
 *	ShadowMap Plug		       
 *******************************************************/
sceHiErr sceHiPlugShadowMap(sceHiPlug *plug, int process)
{
    sceHiErr	err;
    ShadowMap	*shadowmap;

    switch(process){
      case SCE_HIG_INIT_PROCESS:
	  shadowmap = (ShadowMap *)sceHiMemAlign(16, sizeof(ShadowMap));
	  if(shadowmap == NULL)		return _hip_err(_NO_HEAP);
	  err = shadowmap_init(shadowmap, plug);
	  if(err != SCE_HIG_NO_ERR){
	      sceHiMemFree((u_int *)((u_int)shadowmap & 0x0fffffff));
	      return err;
	  }
	  plug->args = NULL;
	  plug->stack = (u_int)shadowmap;
	  break;

      case SCE_HIG_PRE_PROCESS:
	  shadowmap = (ShadowMap *)plug->stack;
	  if(shadowmap == NULL)		return _hip_err(_NO_STACK);
	  shadowmap_view(shadowmap);
	  break;

      case SCE_HIG_POST_PROCESS:
	  break;

      case SCE_HIG_END_PROCESS:
	  shadowmap = (ShadowMap *)plug->stack;
	  if(shadowmap == NULL)		return _hip_err(_NO_STACK);
	  sceHiMemFree((u_int *)((u_int)shadowmap & 0x0fffffff));
	  plug->stack = NULL;
	  plug->args = NULL;
	  break;

      default:
	  break;
    }

    return SCE_HIG_NO_ERR;
}
