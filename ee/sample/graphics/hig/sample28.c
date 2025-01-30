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
#include <libgraph.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>
#include "util.h"
#include "camera.h"
#include "light.h"

/***************************************************
 * sample program 28 
 *
 ***************************************************/

static void	*DATA_ADDR;

extern u_int sce_micro_noshade[];
extern u_int sce_micro_vertexCspec[];
extern u_int sce_micro_phong_cull[];

static sceHiPlugMicroTbl_t my_micro[] = {
    /* backface culling */
    { sce_micro_noshade, 	SCE_HIP_MICRO_ATTR_NONE }, /* 0 : no shading just vertex color */
    { sce_micro_vertexCspec,	SCE_HIP_MICRO_ATTR_NONE }, /* 1 : diffuse-color=vertex-color and specular */
    { sce_micro_phong_cull,    SCE_HIP_MICRO_ATTR_NONE }
};

static const char *micro_name[]={
    "no shading",
    "vertex color + specular",
    "phong "
};

#define MICRO_NUM (sizeof(my_micro) / sizeof(sceHiPlugMicroTbl_t))

/****************************************/
/*	Shading Sample			*/
/****************************************/
static sceHiPlug		*frameP;
static sceHiPlug		*microP;
static sceHiPlugMicroData_t	*microD;
static sceHiPlugMicroPreCalcArg_t myarg = { 0, 0.0f, 1298388.0f, 93784.0f};

static sceHiGsEnv *fogenv;

void SAMPLE28_init(void)
{
    sceHiErr	err;
    sceHiPlugMicroInitArg_t	arg;

    DATA_ADDR = file_read("data/radtea.bin");

    if((err = sceHiParseHeader((u_int *)DATA_ADDR)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if((err = sceHiGetPlug((u_int *)DATA_ADDR, "Frame", &frameP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if((err = sceHiGetPlug((u_int *)DATA_ADDR, "Micro", &microP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    arg.tbl = my_micro;
    arg.tblnum = MICRO_NUM;
    microP->args = (u_int)&arg;
    if((err = sceHiCallPlug(frameP, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    microP->args = (u_int)&myarg;
    if((err = sceHiCallPlug(frameP, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if((microD = sceHiPlugMicroGetData(microP)) == NULL)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    camerainit(microD);
    lightinit(microD);
    lgpini();	/* create light gp */

    fogenv=sceHiGsEnvCreate(SCE_HIGS_VALID_FOGCOL);
    if(fogenv==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if((err = sceHiGsEnvSetRegFogcol(fogenv, 80, 80, 80)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if((err = sceHiGsEnvUpdate(fogenv)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

}

int SAMPLE28_main(void)
{
    sceHiErr	err;
    static int mode = 0; /* micro/camera/light mode */
    int i,j;

    if(gPad.SelectSwitch)
	mode = (++mode) > 2 ? 0 : mode;

    switch (mode) {
      case 0:
	  draw_debug_print(1, 4, "MODE : MICRO CHANGE");	    

	  if(gPad.RleftSwitch || gPad.RupSwitch) {
	      myarg.micro--;
	      if(myarg.micro < 0)
		  myarg.micro = MICRO_NUM - 1;
	  }
	  if(gPad.RrightSwitch || gPad.RdownSwitch) {
	      myarg.micro++;
	      if(myarg.micro >= MICRO_NUM)
		  myarg.micro = 0;
	  }

	  /* micro code change? */
	  if((err = sceHiCallPlug(frameP, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	      error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	  draw_debug_print(1, 6, "Light / Shading / ");
	  draw_debug_print(1, 7, "------------------");
	    
	  for (j=0; j<2; j++){
	      i=(int)myarg.micro+MICRO_NUM+j;
	      i%=MICRO_NUM;
	      if(i==myarg.micro) draw_debug_print(1, 8+j, ">%s", micro_name[i]);
	      else draw_debug_print(1, 8+j, " %s", micro_name[i]);
	  }

	  /* micro mode info */
	  draw_debug_print(48, 1, "Rup/Rdown    : Change Micro");
	  draw_debug_print(48, 7, "SELECT       : MODE CHANGE");
	  draw_debug_print(48, 8, "START        : EXIT");
	  break;
      case 1:
	  draw_debug_print(1, 4, "MODE : CAMERA CTRL");
	  cameractrl();
	  cameraview(microD);
	  break;
      case 2:
	  draw_debug_print(1, 4, "MODE : LIGHT CTRL");
	  switch (myarg.micro) {
	    case 0:
	    case 1:
	    case 2:
	    case 9:
	    case 10:
		lightctrl(microD, DIR_LIGHT);
		break;
	    case 3:
	    case 4:
	    case 5:
		lightctrl(microD, POINT_LIGHT);
		break;
	    case 6:
	    case 7:
	    case 8:
		lightctrl(microD, SPOT_LIGHT);
		break;
	  };
	  break;
    }

    if((err = sceHiGsEnvRegist(fogenv)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if((err = sceHiCallPlug(frameP, SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if(mode == 2) lgpreg();	/* render light gp */

    utilSetDMATimer();
    sceHiDMASend();
    sceHiDMAWait();

    if(gPad.StartSwitch)
	return 1;
    return 0;
}

void SAMPLE28_exit(void)
{
    sceHiErr	err;

    if((err = sceHiCallPlug(frameP, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    free(DATA_ADDR);

    if((err =sceHiGsEnvDelete(fogenv)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    lgpdel();	/* delete light gp */
}
