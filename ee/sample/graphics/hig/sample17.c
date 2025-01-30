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
#include "util.h"
#include "camera.h"
#include "light.h"

#define	NUM	2				/* num of Scene */
enum { CLUTBUMP0, CLUTBUMP1 };			/* BASE_OBJECT, BUMP_OBJECT */

/****************************************/
/*	Drawport			*/
/****************************************/
static sceHiGsCtx *port[NUM];		/* for ClutBump0,ClutBump1 Render */

static void drawport_init(void)
{
    sceHiErr err;

    port[CLUTBUMP0]=sceHiGsCtxCreate(1);
    if (port[CLUTBUMP0]==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsCtxCopy(port[CLUTBUMP0], sceHiGsStdCtx);
    err = sceHiGsCtxSetRegAlpha(port[CLUTBUMP0], SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_ZERO, 128);	/* NONE */
    err = sceHiGsCtxSetRegTest(port[CLUTBUMP0], SCE_GS_FALSE, SCE_GS_ALPHA_NEVER, 0, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GREATER);
    err = sceHiGsCtxUpdate(port[CLUTBUMP0]);
 
    port[CLUTBUMP1]=sceHiGsCtxCreate(1);
    if (port[CLUTBUMP1]==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsCtxCopy(port[CLUTBUMP1], sceHiGsStdCtx);
    err = sceHiGsCtxSetRegAlpha(port[CLUTBUMP1], SCE_GS_ALPHA_CD,SCE_GS_ALPHA_ZERO,SCE_GS_ALPHA_AS,SCE_GS_ALPHA_ZERO,0);	/* CD MODULATE */
    err = sceHiGsCtxSetRegTest(port[CLUTBUMP1], SCE_GS_FALSE, SCE_GS_ALPHA_NEVER, 0, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL);    
    err = sceHiGsCtxUpdate(port[CLUTBUMP1]);
}


static void drawport_exit(void)
{
    sceHiErr err;
    int i;

    for(i=0;i<NUM;i++){
	err = sceHiGsCtxDelete(port[i]);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
}

/****************************************/
/*	Copy Light Dir			*/
/****************************************/
static void mycopylightdir(sceVu0FVECTOR dst, sceHiPlugMicroData_t *src)
{
    sceVu0FVECTOR dir,pos,col;

    lightload(src, dir,pos,col,DIR_LIGHT,0);	/* direlight #0 */
    sceVu0CopyVector(dst, dir);
}

/****************************************/
/*	ClutBump Mapping Sample		*/
/****************************************/
static void		*data_addr;
static sceHiPlug	*frameP[NUM];
static sceHiPlug        *clutbumpP;
static sceHiPlug	*microP[NUM];
static sceHiPlugMicroData_t *microD[NUM];
static sceHiPlugClutBumpPreArg_t arg;

void SAMPLE17_init(void)
{
    sceHiErr	err;
    sceHiType	type;
    char	filename[20] = {"data/clutbump.bin"};
    char	framename[NUM][12] = {"Bump0","Bump1"};
    int		i;

    data_addr = file_read(filename);
    err = sceHiParseHeader((u_int *)data_addr);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /*	Get Micro Plug & Data	*/
    for(i=0;i<NUM;i++){
	err = sceHiGetPlug((u_int *)data_addr, framename[i], &frameP[i]);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	type.repository = SCE_HIP_COMMON;
	type.project = SCE_HIP_FRAMEWORK;
	type.category = SCE_HIP_MICRO;
	type.status = SCE_HIG_PLUGIN_STATUS;
	type.id = SCE_HIP_MICRO_PLUG;
	type.revision = SCE_HIP_REVISION;
	err = sceHiGetInsPlug(frameP[i], &microP[i], type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if((microD[i] = sceHiPlugMicroGetData(microP[i])) == NULL)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    type.category = SCE_HIP_BUMP;
    type.id = SCE_HIP_CLUTBUMP_PLUG;
    type.status = SCE_HIG_PLUGIN_STATUS;
    err = sceHiGetInsPlug(frameP[CLUTBUMP1], &clutbumpP, type); 
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /*	INIT PROCESS	*/
    for(i=0;i<NUM;i++){
	err = sceHiCallPlug(frameP[i], SCE_HIG_INIT_PROCESS);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    for(i=0;i<NUM;i++){
	camerainit(microD[i]);
	lightinit(microD[i]);
    }

    drawport_init();
    lgpini();	/* create light gp */

}

int SAMPLE17_main(void)
{
    sceHiErr	err;
    int		i;
    static int mode = 0;
    static float A=0.0f;
    static float B=0.5f;
    static float C=0.0f;
    static float D=0.0f;

    if (gPad.SelectSwitch)
	mode = (++mode) > 2 ? 0 : mode;
 
    switch (mode) {
      case 0:
          draw_debug_print(1, 4, "MODE : CAMERA MOVE");
          cameractrl();
	  cameraview(microD[CLUTBUMP0]);
	  cameracopy(microD[CLUTBUMP1], microD[CLUTBUMP0]);

	  /* camera mode info */
	  draw_debug_print(50, 1, "Rup/Rdown    : Cam Up/Down");
	  draw_debug_print(50, 2, "Rleft/Rright : Cam Left/Right");
	  draw_debug_print(50, 3, "R1/R2        : Cam Back/Front");
	  draw_debug_print(50, 4, "Lup/Ldown    : Cam Rot X");
	  draw_debug_print(50, 5, "Lleft/Lright : Cam Rot Y");
	  draw_debug_print(50, 6, "L1/L2        : Cam Rot Z");
	  draw_debug_print(50, 7, "SELECT       : MODE CHANGE");	    
	  draw_debug_print(50, 8, "START        : EXIT");
          break;
      case 1:
          draw_debug_print(1, 4, "MODE : LIGHT CTRL");
          lightctrl(microD[CLUTBUMP0], DIR_LIGHT);
	  lightcopy(microD[CLUTBUMP1],microD[CLUTBUMP0],DIR_LIGHT);

	  /* light info */
	  draw_debug_print(50, 1, "Rup/Rdown    : Light Up/Down");
	  draw_debug_print(50, 2, "Rleft/Rright : Light Left/Right");
	  draw_debug_print(50, 3, "R1/R2        : Light Back/Front");
	  draw_debug_print(50, 4, "Lup/Ldown    : ITEM CHANGE");
	  draw_debug_print(50, 5, "Lleft/Lright : LIGHT CHANGE");
	  draw_debug_print(50, 7, "SELECT       : MODE CHANGE");    
	  draw_debug_print(50, 8, "START        : EXIT");
          break;
      case 2:
	  if (gPad.RleftOn) {
	      A-=0.5;
	      if (A<=0) A=0;
          }
          if (gPad.RrightOn) {
	      A+=0.5;
          }    
          if (gPad.RdownOn) {
	      B-=0.01;
	      if (B<=0) B=0;    
          }
          if (gPad.RupOn) {
	      B+=0.01; 
          }    
          if (gPad.R2On) {
	      C-=0.01;
	      if (C<=0) C=0;    
          }
          if (gPad.R1On) {
	      C+=0.01; 
          }    
          if (gPad.L2Switch) {
	      D-=1;
	      if (D<=0) D=0;    
          }
          if (gPad.L1Switch) {
	      D+=1; 
          }

	  /* shading info */
          draw_debug_print(1, 4, "MODE : SHADING CTRL (cos(alpha)=lv.normal)");
	  draw_debug_print(1, 9, " %f + %f * cos(alpha) + %f * cos(alpha)^%d",A,B,C,(int)D);
	  draw_debug_print(50, 1, "Rleft/Right  : param A");
	  draw_debug_print(50, 2, "Rup/Rdown    : param B");
	  draw_debug_print(50, 3, "R1/R2        : param C");
	  draw_debug_print(50, 4, "L1/L2        : param D");
	  draw_debug_print(50, 7, "SELECT       : MODE CHANGE");	    
	  draw_debug_print(50, 8, "START        : EXIT");	    
	  break;
    }       
 
    /*  Set ClutBump Args       */
    clutbumpP->args = (u_int)&arg;
    mycopylightdir(arg.light_dir,microD[CLUTBUMP1]);
    arg.shading[0]=A;
    arg.shading[1]=B;
    arg.shading[2]=C;
    arg.shading[3]=D;    

    for(i=0;i<NUM;i++){
	err = sceHiCallPlug(frameP[i], SCE_HIG_PRE_PROCESS);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    err = sceHiGsCtxRegist(port[CLUTBUMP0], 0);
    err = sceHiCallPlug(frameP[CLUTBUMP0], SCE_HIG_POST_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    err = sceHiGsCtxRegist(port[CLUTBUMP1], 0);
    err = sceHiCallPlug(frameP[CLUTBUMP1], SCE_HIG_POST_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    err = sceHiGsCtxRegist(sceHiGsStdCtx, 0);
    if(mode==1) lgpreg();	/* render light gp */

    utilSetDMATimer();
    err = sceHiDMASend();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiDMAWait();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (gPad.StartSwitch)
	return 1;
    return 0;
}

void SAMPLE17_exit(void)
{
    sceHiErr	err;
    int		i;

    for(i=0;i<NUM;i++){
	err = sceHiCallPlug(frameP[i], SCE_HIG_END_PROCESS);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    free(data_addr);

    drawport_exit();
    lgpdel();		/* delete light gp */
}
