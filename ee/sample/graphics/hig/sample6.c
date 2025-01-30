/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
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

/****************************************/
/*	My Light			*/
/****************************************/
static void mylightinit(sceHiPlugMicroData_t *md)
{
    int i,j;
    static sceVu0FVECTOR dir[4]={{1.0f, 0.0f, 0.6f, 0.0f},{0.0f, 1.0f, 0.6f, 0.0f},{-1.0f, 0.0f, 0.6f, 0.0f},{0.0f, 0.0f, 0.0f, 1.0f}};
    static sceVu0FVECTOR pos[4]={{-8.0f, 0.0f, -6.0f, 0.7f},{0.0f, -8.0f, -6.0f, 0.7f},{8.0f, 0.0f, -6.0f, 0.7f},{0.0f, 0.0f, 0.0f, 1.0f}};
    static sceVu0FVECTOR col[4]={{0.8f, 0.0f, 0.0f, 30.0f},{0.0f, 0.8f, 0.0f, 30.0f},{0.0f, 0.0f, 0.8f, 30.0f},{0.2f, 0.2f, 0.2f, 0.0f}};

    for(i=0;i<3;i++){		/* light type 3 */
	for(j=0;j<4;j++){	/* light number 3+1 */
	    lightstore(md, dir[j], pos[j], col[j], i, j);	/* to micro data */
	}
    }
}

/***************************************************
 * sample program 6
 *
 *	micro switch sample
 *	Shading Library Switch
 *		1. Vertex Color
 *		2. Lambert
 *		3. Phong
 ***************************************************/

static void	*DATA_ADDR;

extern u_int sce_micro_vertex_cull[];
extern u_int sce_micro_lambert_cull[];
extern u_int sce_micro_phong_cull[];
extern u_int sce_micro_vertex_point[];
extern u_int sce_micro_lambert_point[];
extern u_int sce_micro_phong_point[];
extern u_int sce_micro_vertex_spot[];
extern u_int sce_micro_lambert_spot[];
extern u_int sce_micro_phong_spot[];

extern u_int sce_micro_anti[];
extern u_int sce_micro_fog[];


static sceHiPlugMicroTbl_t my_micro[] = {
    /* backface culling */
    { sce_micro_vertex_cull, 	SCE_HIP_MICRO_ATTR_NONE }, /* 0 : vertex color : pallarel */
    { sce_micro_lambert_cull,	SCE_HIP_MICRO_ATTR_NONE }, /* 1 : lambert      : pallarel */
    { sce_micro_phong_cull,	SCE_HIP_MICRO_ATTR_NONE }, /* 2 : phong        : pallarel */

    /* backface culling & point light */
    { sce_micro_vertex_point,	SCE_HIP_MICRO_ATTR_NONE }, /* 3 : vertex color : point light */
    { sce_micro_lambert_point,	SCE_HIP_MICRO_ATTR_NONE }, /* 4 : lambert      : point light */
    { sce_micro_phong_point,	SCE_HIP_MICRO_ATTR_NONE }, /* 5 : phong        : point light */

    /* backface culling & spot light */
    { sce_micro_vertex_spot,	SCE_HIP_MICRO_ATTR_NONE }, /* 6 : vertex color : spot light */
    { sce_micro_lambert_spot,	SCE_HIP_MICRO_ATTR_NONE }, /* 7 : lambert      : spot light */
    { sce_micro_phong_spot,	SCE_HIP_MICRO_ATTR_NONE }, /* 8 : phong        : spot light */

    /* others */
    { sce_micro_fog, SCE_HIP_MICRO_ATTR_FGE },
    { sce_micro_anti, SCE_HIP_MICRO_ATTR_ANTI },

};

static const char *micro_name[]={
    "Dir.  / V Color",
    "Dir.  / Lambert",
    "Dir.  / Phong  ",
    "Point / V Color",
    "Point / Lambert",
    "Point / Phong  ",
    "Spot  / V Color",
    "Spot  / Lambert",
    "Spot  / Phong  ",

    "Dir.  / V Color / Fog  ",
    "Dir.  / V Color / AA1-polygon ",
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

void SAMPLE6_init(void)
{
    sceHiErr	err;
    sceHiPlugMicroInitArg_t	arg;

    DATA_ADDR = file_read("data/ballS.bin");

    if((err = sceHiParseHeader((u_int *)DATA_ADDR)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    if((err = sceHiGetPlug((u_int *)DATA_ADDR, "Frame", &frameP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    if((err = sceHiGetPlug((u_int *)DATA_ADDR, "Micro", &microP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    arg.tbl = my_micro;
    arg.tblnum = MICRO_NUM;
    microP->args = (u_int)&arg;
    if((err = sceHiCallPlug(frameP, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    microP->args = (u_int)&myarg;
    if((err = sceHiCallPlug(frameP, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    if((microD = sceHiPlugMicroGetData(microP)) == NULL)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    camerainit(microD);
    mylightinit(microD);
    lgpini();	/* create light gp */

    fogenv=sceHiGsEnvCreate(SCE_HIGS_VALID_FOGCOL);
    if(fogenv==NULL) error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    if((err = sceHiGsEnvSetRegFogcol(fogenv, 80, 80, 80)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    if((err = sceHiGsEnvUpdate(fogenv)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

}

int SAMPLE6_main(void)
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
	      error_handling(__FILE__,__LINE__,sceHiErrState.mes);

	  draw_debug_print(1, 6, "Light / Shading / ");
	  draw_debug_print(1, 7, "------------------");
	    
	  for (j=0; j<7; j++){
	      i=(int)myarg.micro+MICRO_NUM-3+j;
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
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    if((err = sceHiCallPlug(frameP, SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    if(mode == 2) lgpreg();	/* render light gp */

    utilSetDMATimer();
    sceHiDMASend();
    sceHiDMAWait();

    if(gPad.StartSwitch)
	return 1;
    return 0;
}

void SAMPLE6_exit(void)
{
    sceHiErr	err;

    if((err = sceHiCallPlug(frameP, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    free(DATA_ADDR);

    if((err =sceHiGsEnvDelete(fogenv)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    lgpdel();	/* delete light gp */
}
