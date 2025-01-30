/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
*/
/*
 *
 *	Copyright (C) 2002 Sony Computer Entertainment Inc.
 *							All Right Reserved
 *
 */
#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <eeregs.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>
#include <libgraph.h>
#include "util.h"
#include "camera.h"
#include "light.h"

/***************************************************
 * sample program : rendering with fish-eye lens
 *  
 * Micro switch:
 *     0. fish-eye lens (dir. light)
 *     1. perspective proj. (dir. light)
 ***************************************************/

static void	*DATA_ADDR;
extern u_int sce_micro_fisheye[];
extern u_int sce_micro_vertex_cull[];

static sceHiPlugMicroTbl_t my_micro[] = {
    { sce_micro_fisheye,   SCE_HIP_MICRO_ATTR_NONE },
    { sce_micro_vertex_cull,   SCE_HIP_MICRO_ATTR_NONE }
};

#define MICRO_NUM (sizeof(my_micro) / sizeof(sceHiPlugMicroTbl_t))

static sceVu0FVECTOR clip;
static float image_size = 256.0f;
static float ZA,ZB;
static void mycamerainit(sceHiPlugMicroData_t *md)
{
    static sceVu0FVECTOR s = {640.0f, 224.0f, 512.0f, 300.0f};
    static sceVu0FVECTOR w = {1.0f, 0.47f, 2048.0f, 2048.0f};
    static sceVu0FVECTOR d = {5.0f, 16777000.0f, 1.0f, 65536.0f};
    static sceVu0FVECTOR p = { 0.0f, 0.0f, -8.0f, 0.0f };
    static sceVu0FVECTOR r = { 0.0f, 0.0f, 0.0f, 0.0f };
    static sceVu0FVECTOR i = { 0.0f, 0.0f, 0.0f, 0.0f };
    static sceVu0FVECTOR u = { 0.0f, 1.0f, 0.0f, 0.0f };
    static float near=1.0,far=16777216,rmax=16000000,rmin=0.0;

    perspective(md,s,w,d);
    sceVu0CopyVector(clip, md->clp.clip);
    viewset(p,r,i,u);
    cameraview(md);

    ZA = (rmax*near-rmin*far)/(near-far);
    ZB = -(rmax*near*far-rmin*near*far)/(near-far);
}

static sceHiPlug	*frameP;
static sceHiPlug	*microP;
static sceHiPlugMicroData_t *microD;
static sceHiPlugMicroPreCalcArg_t myarg = {0, 0.0f, 0.0f, 0.0f};

void SAMPLE24_init(void)
{
    sceHiErr	err;
    sceHiPlugMicroInitArg_t	arg;

    DATA_ADDR = file_read("data/refroom.bin");
    if ((err = sceHiParseHeader((u_int *)DATA_ADDR)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Frame", &frameP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Micro", &microP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    arg.tbl = my_micro;
    arg.tblnum = MICRO_NUM;
    microP->args = (u_int)&arg;

    if ((err = sceHiCallPlug(frameP, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if((microD = sceHiPlugMicroGetData(microP)) == NULL)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    mycamerainit(microD);
    lightinit(microD);

    microP->args = (u_int)&myarg;
}

enum {CAMERA_MODE,MICRO_MODE,FISH_MODE};
static const char *micro_name[]={"fish-eye","perspective"};

int SAMPLE24_main(void)
{
    sceHiErr	err;
    static int mode = CAMERA_MODE;
 
    if (gPad.SelectSwitch)
	mode = (++mode) > FISH_MODE ? 0 : mode;

    switch (mode) {
      case CAMERA_MODE:
	  draw_debug_print(1, 4, "MODE : CAMERA CTRL");
	  cameractrl();
	  cameraview(microD);
	  break;
      case MICRO_MODE:
	  draw_debug_print(1, 4, "MODE : MICRO CHANGE");
	  if (gPad.RrightSwitch) {
	      myarg.micro++;
	      if (myarg.micro >= MICRO_NUM) myarg.micro = 0;
	  }
	  /* micro mode info */
	  draw_debug_print(1, 8, "Shading  : dir. light(vertex-color)");
	  draw_debug_print(1, 9, "Micro : %s",micro_name[myarg.micro]);
	  
	  draw_debug_print(50, 1, "Rleft/Rright : Change Micro");
	  draw_debug_print(50, 7, "SELECT       : MODE CHANGE");
	  draw_debug_print(50, 8, "START        : EXIT");     
	  break;
      case FISH_MODE:
	  if (gPad.RleftOn) {
	      image_size--;
	      if(image_size<0.0f) image_size=0.0f;
          }
          if (gPad.RrightOn) {
	      image_size++;
          }
          draw_debug_print(1, 4, "MODE : PARAMETER CTRL ");
	  draw_debug_print(1, 10, "image_size %d",(int)image_size);
          /* param. ctrl info */
          draw_debug_print(36, 1, "Rleft/Rright : decrease/increase image size");  
	  break;
    }

    if(myarg.micro){	/* perspective */
	sceVu0CopyVector(microD->clp.clip, clip);
    }
    else{		/* fish-eye */
	microD->clp.fisheye.texsize = image_size/2.0f;
	microD->clp.fisheye.ZA = ZA;
	microD->clp.fisheye.ZB = ZB;
    }


    if ((err = sceHiCallPlug(frameP, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if ((err = sceHiCallPlug(frameP, SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    err = sceHiGsCtxRegist(sceHiGsStdCtx, 0);


    utilSetDMATimer();
    if ((err = sceHiDMASend()) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if ((err = sceHiDMAWait()) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (gPad.StartSwitch)
	return 1;

    return 0;
}

void SAMPLE24_exit(void)
{
    sceHiErr	err;

    if ((err = sceHiCallPlug(frameP, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    free(DATA_ADDR);
}
