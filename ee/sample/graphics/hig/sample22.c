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
 * sample specular lighting with texture 
 ***************************************************/

static void	*DATA_ADDR;

/***************************************************
 *		Micro Codes
 ***************************************************/
extern u_int sce_micro_reflectS[];
#define MICRO_NUM 1
#define ZOOM 1.0f

/***************************************************
 *		Sample Main
 ***************************************************/
static sceHiPlug	*tex2dP;
static sceHiGsMemTbl	*reftbl = NULL;
static sceHiPlug	*frameP;
static sceHiPlug	*microP;
static sceHiPlugMicroData_t *microD;

static sceHiPlugTex2dInitArg_t	tex2d_arg;

void SAMPLE22_init(void)
{
    sceHiErr	err;
    sceHiPlugMicroTbl_t micro_tbl[MICRO_NUM] = {
	{ sce_micro_reflectS,   SCE_HIP_MICRO_ATTR_NONE },
    };
    sceHiPlugMicroInitArg_t micro_arg = { micro_tbl, MICRO_NUM };

    DATA_ADDR = file_read("data/teapot.bin");

    err = sceHiParseHeader((u_int *)DATA_ADDR);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGetPlug((u_int *)DATA_ADDR, "Frame", &frameP);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGetPlug((u_int *)DATA_ADDR, "Tex2d", &tex2dP);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    tex2d_arg.resident = TRUE;
    tex2d_arg.tbl = NULL; 
    tex2dP->args = (u_int)&tex2d_arg;

    err = sceHiGetPlug((u_int *)DATA_ADDR, "Micro", &microP);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    microP->args = (u_int)&micro_arg;

    err = sceHiCallPlug(frameP, SCE_HIG_INIT_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if((microD = sceHiPlugMicroGetData(microP)) == NULL)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    camerainit(microD);
    lightinit(microD);

    microD->zoom = ZOOM;
}


int SAMPLE22_main(void)
{
    sceHiErr	err;
    static int mode = 0; /* camera/parameter mode */
 
 
    if (gPad.SelectSwitch)
        mode = (++mode) > 1 ? 0 : mode;   

    switch (mode) {
      case 0:
          draw_debug_print(1, 4, "MODE : CAMERA MOVE");
          cameractrl();
	  cameraview(microD);
          break;
      case 1:
          if (gPad.RdownOn) microD->zoom-=0.01;
          if (gPad.RupOn) microD->zoom+=0.01;
          draw_debug_print(1, 4, "MODE : PARAMETER CTRL ");
          draw_debug_print(1, 8, "       Zoom: %f",microD->zoom);
	  draw_debug_print(50, 1, "Rup/Rdown    : Zoom Param");
	  draw_debug_print(50, 7, "SELECT       : MODE CHANGE");	    
	  draw_debug_print(50, 8, "START        : EXIT");
          break;
    }

    err = sceHiCallPlug(frameP, SCE_HIG_PRE_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiCallPlug(frameP, SCE_HIG_POST_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    err = sceHiGsCtxRegist(sceHiGsStdCtx, 0);  
    utilSetDMATimer();
    err = sceHiDMASend();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiDMAWait();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (gPad.StartSwitch)
	return 1;
    return 0;
}

void SAMPLE22_exit(void)
{
    sceHiErr	err;

    err = sceHiCallPlug(frameP, SCE_HIG_END_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    free(DATA_ADDR);
    sceHiGsMemFree(reftbl);
}
