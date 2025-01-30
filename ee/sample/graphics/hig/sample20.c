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
 * sample reflection mapping with static ref_texture
 *
 *	micro switch sample
 *		0. Reflection
 *          1. Refraction
 ***************************************************/

static void	*DATA_ADDR;
#define ZOOM 1.0f
#define INDEX 0.03f

/***************************************************
 *		Micro Codes
 ***************************************************/
extern u_int sce_micro_reflectS[];
extern u_int sce_micro_refractS[];
#define MICRO_NUM 2

/***************************************************
 *		Local Functions
 ***************************************************/
#define TEXTURE__WIDTH 128 
#define TEXTURE__HEIGHT 128 
#define TEXTURE__PSM SCE_GS_PSMCT24
#define TEXTURE__IMAGE_FILE "data/refback.raw"
static void _trans_tex(u_int *texbuf, int psm, int tbp, int width, int height)
{

    sceGsLoadImage li;

    sceGsSetDefLoadImage(&li, tbp, (width+63)/64, psm, 0, 0, width, height);
    SyncDCache(&li, ((char *)&li)+sizeof(li));
    sceGsExecLoadImage(&li, (u_long128 *)texbuf);
    sceGsSyncPath(0,0);
}

union QWdata {
    u_long128   ul128;
    u_long              ul64[2];
    u_int               ui32[4];
    sceVu0FVECTOR       fvect;
};

struct {
    union QWdata    buf[9];
    u_int           count;
} pack __attribute__ ((aligned(16)));

static void _draw_back_ground(int tbp,int tbw,int psm,int tw,int th)
{
    sceVu0FVECTOR sprite[2] = {
	{1728.0, 1888.0, 1.0, 0.0},
	{2368.0,2208.0, 1.0, 0.0}};

    int                 j;
 

    sceVu0IVECTOR       v01;
    sceVu0IVECTOR       color={255,255,255,0};
    sceVu0FVECTOR       st[2]={{0.0,0.0,1.0,0.0},
			       {1.0,1.0,1.0,0.0}};
    pack.count = 0;
    pack.buf[pack.count].ul64[0] = SCE_GIF_SET_TAG(1, 1, 1, 0x1E, 0, 7);
    pack.buf[pack.count].ul64[1] = 0x4124126L;
    pack.count++;
    pack.buf[pack.count++].ul64[0] = SCE_GS_SET_TEX0(tbp,tbw,psm,tw,th,1,1,0,0,0,0,0);
    for (j = 0; j < 2; j++) {
	sceVu0FTOI4Vector(v01,sprite[j]);
        pack.buf[pack.count++].ul128 = *((u_long128 *)st[j]);
	pack.buf[pack.count++].ul128 = *((u_long128 *)color);	
	//sceVu0RotTransPers(v01, cam.world_screen, v00, 0);    
        pack.buf[pack.count++].ul128 = *((u_long128 *)v01);
    }
    *D2_QWC = pack.count;
    *D2_MADR = (u_int) &pack.buf & 0x0fffffff;
    *D_STAT = 2;
    FlushCache(0);
    *D2_CHCR = 1 | (0 << 2) | (0 << 4) | (0 << 5) | (0 << 6) | (0 << 7) | (1 << 8);
    sceGsSyncPath(0, 0);
}

/***************************************************
 *		Sample Main
 ***************************************************/
static sceHiPlug	*tex2dP;
static sceHiGsMemTbl	*reftbl = NULL;
static sceHiPlug	*frameP;
static sceHiPlug	*microP;
static sceHiPlugMicroData_t *microD;

static sceHiPlugTex2dInitArg_t	tex2d_arg;
static sceHiPlugMicroPreCalcArg_t arg;

void SAMPLE20_init(void)
{
    sceHiErr	err;
    u_int *texbuf;
    sceHiPlugMicroTbl_t micro_tbl[MICRO_NUM] = {
	{ sce_micro_reflectS,   SCE_HIP_MICRO_ATTR_NONE },
	{ sce_micro_refractS,   SCE_HIP_MICRO_ATTR_NONE }
    };
    sceHiPlugMicroInitArg_t micro_arg = { micro_tbl, MICRO_NUM };

    texbuf=file_read(TEXTURE__IMAGE_FILE);
    reftbl = sceHiGsMemAlloc(SCE_HIGS_BLOCK_ALIGN, sceHiGsBlockSize(TEXTURE__WIDTH,TEXTURE__HEIGHT,TEXTURE__PSM));
    _trans_tex(texbuf, TEXTURE__PSM, reftbl->addr/SCE_HIGS_BLOCK_ALIGN, TEXTURE__WIDTH, TEXTURE__HEIGHT);

    DATA_ADDR = file_read("data/reftorus.bin");

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

    microP->args = (u_int)&arg;
    microD->refidx = INDEX;
    microD->zoom = ZOOM;
}

enum {MICRO_MODE,CAMERA_MODE,REFLECT_MODE};

int SAMPLE20_main(void)
{
    sceHiErr	err;
    static int mode = MICRO_MODE;
    static const char *micro_name[]= {"REFLECT","REFLACT"};

    if (gPad.SelectSwitch)
	mode = (++mode) > REFLECT_MODE ? MICRO_MODE : mode;

    switch (mode) {
      case MICRO_MODE:
	  draw_debug_print(1, 4, "MODE : MICRO CHANGE");
	  if (gPad.RleftSwitch) {
	      arg.micro--;
	      if (arg.micro < 0)
		  arg.micro = MICRO_NUM - 1;
	  }
	  if (gPad.RrightSwitch) {
	      arg.micro++;
	      if (arg.micro >= MICRO_NUM)
		  arg.micro = 0;
	  }
	  draw_debug_print(1,5, "%s",micro_name[arg.micro]);
	  draw_debug_print(50, 1, "Rleft/Rright : Change Micro");
	  draw_debug_print(50, 7, "SELECT       : MODE CHANGE");
	  draw_debug_print(50, 8, "START        : EXIT");
	  break;
      case CAMERA_MODE:
	  draw_debug_print(1, 4, "MODE : CAMERA MOVE");
	  cameractrl();
	  cameraview(microD);
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
      case REFLECT_MODE:
          if (gPad.RleftOn) {
	      microD->refidx-=0.001f;
          }
          if (gPad.RrightOn) {
	      microD->refidx+=0.001f;
          }
	  if (gPad.RdownOn) {
	      microD->zoom-=0.001f;
          }
          if (gPad.RupOn) {
	      microD->zoom+=0.001f;
          }
          draw_debug_print(1, 4, "MODE : PARAMETER CTRL ");
	  draw_debug_print(1, 7, "Index %f",microD->refidx);
	  draw_debug_print(1, 8, "Zoom: %f",microD->zoom);
	  draw_debug_print(50, 1, "Rleft/Rright : Index Param");
	  draw_debug_print(50, 2, "Rup/Rdown    : Zoom Param");
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

    _draw_back_ground(reftbl->addr/SCE_HIGS_BLOCK_ALIGN,(TEXTURE__WIDTH+63)/64,TEXTURE__PSM,7,7);
    sceGsSyncPath(0,0);

    if (gPad.StartSwitch)
	return 1;
    return 0;
}

void SAMPLE20_exit(void)
{
    sceHiErr	err;

    err = sceHiCallPlug(frameP, SCE_HIG_END_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    free(DATA_ADDR);
    sceHiGsMemFree(reftbl);
}
