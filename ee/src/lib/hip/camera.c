/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.7
 */
/*
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: camera.c,v 1.3 2002/10/16 10:09:50 kaneko Exp $	*/
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
    _NO_CAMERA_DATA,
    _NO_MICRO_DATA,
    _NO_STACK,
} _local_err;

static const sceHiErr _local2hierr[] = {
    SCE_HIG_NO_HEAP,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
};

static const char *_err_mes[] = {
    "HiP Camera : can't allocate memory\n",
    "HiP Camera : can't find CAMERA_DATA\n",
    "HiP Camera : can't find MICRO_DATA\n",
    "HiP Camera : plug stack is null\n",
};

/* error handling */
static sceHiErr _hip_err(_local_err err)
{
    sceHiErrState.mes = _err_mes[err];
    return _local2hierr[err];
}

/*******************************************************
 *	Camera Structure
 *******************************************************/
typedef struct {
    sceVu0FMATRIX		sm;	/* screen matrix */
    sceVu0FVECTOR		cv;	/* clip volume */
    sceHiPlugCameraData_t	*cd;	/* camera data */
    sceHiPlugMicroData_t	*md;	/* micro data */
}Camera_t;

/*******************************************************
 *	Camera Calc
 *******************************************************/
static void camera_calc(Camera_t *cam)
{
    sceVu0FVECTOR xd,yd,zd;
    sceVu0FMATRIX m;

    sceVu0UnitMatrix(m);
    sceVu0RotMatrix(m, m, cam->cd->rotation);
    sceVu0ApplyMatrix(zd, m, cam->cd->interest);
    sceVu0ApplyMatrix(yd, m, cam->cd->upvector);
    sceVu0ApplyMatrix(xd, m, cam->cd->position);

    sceVu0SubVector(zd, zd, xd);
    sceVu0CameraMatrix(m, xd, zd, yd);

    sceVu0MulMatrix(cam->md->wscreen, cam->sm, m);

    /*	for specific micro code	*/
    sceVu0CopyMatrix(cam->md->mtx.wview, m);
    sceVu0CopyVectorXYZ(&cam->md->camx, xd);
}

/*******************************************************
 *	Camera Init
 *******************************************************/
static sceHiErr camera_init(sceHiPlug *plug, Camera_t *cam)
{
    cam->cd = sceHiPlugCameraGetData(plug);
    if(cam->cd == NULL) return _hip_err(_NO_CAMERA_DATA);
    cam->md = sceHiPlugMicroGetData(plug);
    if(cam->md == NULL) return _hip_err(_NO_MICRO_DATA);

    sceVu0ViewScreenMatrix(cam->sm, cam->cd->screen[2], cam->cd->window[0], cam->cd->window[1], cam->cd->window[2], cam->cd->window[3], cam->cd->depth[0], cam->cd->depth[1], cam->cd->depth[2], cam->cd->depth[3]);
    cam->cv[0] = (cam->cd->depth[3] - cam->cd->depth[2])/(cam->cd->screen[0] + cam->cd->screen[3]);	/* (f-n)/(w+) */
    cam->cv[1] = (cam->cd->depth[3] - cam->cd->depth[2])/(cam->cd->screen[1] + cam->cd->screen[3]);	/* (f-n)/(h+) */
    cam->cv[2] = (cam->cd->depth[3] + cam->cd->depth[2])/2.0f;		/* (f+n)/2 */
    cam->cv[3] = (cam->cd->depth[3] - cam->cd->depth[2])/2.0f;		/* (f-n)/2 */
    sceVu0CopyVector(cam->md->clp.clip, cam->cv);

    return SCE_HIG_NO_ERR;
}

/*******************************************************
 *	Camera Plug
 *******************************************************/
sceHiErr sceHiPlugCamera(sceHiPlug *plug, int process)
{
    sceHiErr	err;
    Camera_t	*camera;

    switch(process){
    case SCE_HIG_INIT_PROCESS:
      camera = (Camera_t *)sceHiMemAlign(16, sizeof(Camera_t));
      if(camera == NULL) return _hip_err(_NO_HEAP);
      err = camera_init(plug, camera);
      if(err != SCE_HIG_NO_ERR){
	  sceHiMemFree((u_int *)((u_int)camera & 0x0fffffff));
	  return err;
      }
      plug->args = NULL;
      plug->stack = (u_int)camera;
      break;

    case SCE_HIG_PRE_PROCESS:
      camera = (Camera_t *)plug->stack;
      if(camera == NULL) return _hip_err(_NO_STACK);
      camera_calc(camera);
      plug->args = NULL;
      break;

    case SCE_HIG_POST_PROCESS:
      break;

    case SCE_HIG_END_PROCESS:
      camera = (Camera_t *)plug->stack;
      if(camera == NULL) return _hip_err(_NO_STACK);
      sceHiMemFree((u_int *)((u_int)camera & 0x0fffffff));
      plug->stack = NULL;
      plug->args = NULL;
      break;

    default:
      break;
    }    

    return SCE_HIG_NO_ERR;
}
