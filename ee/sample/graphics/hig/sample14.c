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

/***************************************************
 * sample program 14
 *
 *	GS Service sample(2)
 *	A sample for drawing to texture area.
 *	dynamic texture drawing
 ***************************************************/

/* drawing texture */
#define WINDOW_WIDTH 256
#define WINDOW_HEIGHT 256
#define WINDOW_PSM SCE_GS_PSMCT16
#define WINDOW_ZPSM SCE_GS_PSMZ16

static void *DATA_TEX_ADDR;
static void *DATA_BASE_ADDR;

static sceHiPlug	*frameP_tex;
static sceHiPlug	*frameP_base;
static sceHiPlug	*microP_tex, *microP_base;
static sceHiPlug	*tex2dP;
static sceHiPlug	*hrchyP_tex;

static sceHiPlugMicroData_t *microD_tex,*microD_base;

static sceHiGsMemTbl	*drawtexarea = NULL;

static sceHiPlugTex2dInitArg_t		_tex2d_arg;
static sceHiPlugHrchyPreCalcArg_t	_hrchy_arg;
static sceHiPlugMicroPreCalcArg_t	_micro_arg;

static sceVu0FMATRIX root_mat_tex;
static sceVu0FVECTOR root_trans_tex={1.0f, 7.6f, 0.0f, 0.0f};
static sceVu0FVECTOR root_rot_tex={-0.16f, -0.42f, -0.05f, 0.0f};

static void _obj_tex_ctrl(void);


/* context for draw texture */
static sceHiGsCtx *gsctx_tex;

static void mycamerainit(sceHiPlugMicroData_t *md)
{
    static sceVu0FVECTOR s = {256.0f, 256.0f, 512.0f, 100.0f};
    static sceVu0FVECTOR w = {1.0f, 1.0f, 2048.0f, 2048.0f};
    static sceVu0FVECTOR d = {5.0f, 65535.0f, 1.0f, 65536.0f};
    static sceVu0FVECTOR p = { 0.0f, 0.0f, -30.0f, 0.0f };
    static sceVu0FVECTOR r = { 0.0f, 0.0f, 0.0f, 0.0f };
    static sceVu0FVECTOR i = { 0.0f, 0.0f, 0.0f, 0.0f };
    static sceVu0FVECTOR u = { 0.0f, 1.0f, 0.0f, 0.0f };

    perspective(md, s,w,d);
    viewset(p,r,i,u);
    cameraview(md);
}
static void mylightinit(sceHiPlugMicroData_t *md)
{
    static sceVu0FVECTOR d = {-3.0f,-1.5f,1.0f,0.0f};
    static sceVu0FVECTOR p = {0.0f,0.0f,0.0f,0.0f};
    static sceVu0FVECTOR c = {0.5f,0.5f,0.5f,0.0f};

    lightinit(md);
    lightstore(md, d,p,c,DIR_LIGHT,1);
}


void SAMPLE14_init(void)
{
    /* allocate drawing area for texture */
    drawtexarea = sceHiGsMemAlloc(SCE_HIGS_PAGE_ALIGN, SCE_HIGS_PAGE_ALIGN*sceHiGsMemRestSize()/SCE_HIGS_PAGE_ALIGN);
    if (drawtexarea==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* the context of drawing texture */
    gsctx_tex=sceHiGsCtxCreate(0);
    if (gsctx_tex==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    sceHiGsCtxSetDefaultValues(gsctx_tex, WINDOW_PSM, WINDOW_ZPSM, 1, WINDOW_WIDTH, WINDOW_HEIGHT);

    /* set buffer pointers */
    sceHiGsCtxSetLumpBuffer(gsctx_tex, drawtexarea->addr/2048);
    /* set clear color */
    sceHiGsCtxSetClearColor(gsctx_tex, 64, 128, 64, 128);
    /* update packet buffers */
    sceHiGsCtxUpdate(gsctx_tex);

    _tex2d_arg.resident = TRUE;
    _tex2d_arg.tbl=drawtexarea;

    /* texture object */
    DATA_TEX_ADDR = file_read("data/APEWALK39.bin");

    /* base object */
    DATA_BASE_ADDR = file_read("data/curtain.bin");

    if (sceHiParseHeader((u_int *)DATA_TEX_ADDR) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (sceHiParseHeader((u_int *)DATA_BASE_ADDR) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (sceHiGetPlug((u_int *)DATA_TEX_ADDR, "Frame", &frameP_tex) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (sceHiGetPlug((u_int *)DATA_BASE_ADDR, "Frame", &frameP_base) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* This sample is only for tex2d-texture  */
    if (sceHiGetPlug((u_int *)DATA_BASE_ADDR, "Tex2d", &tex2dP) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    tex2dP->args = (u_int)&_tex2d_arg;
	    
    if (sceHiCallPlug(frameP_tex, SCE_HIG_INIT_PROCESS) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (sceHiCallPlug(frameP_base, SCE_HIG_INIT_PROCESS) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* for camera */
    if (sceHiGetPlug((u_int *)DATA_TEX_ADDR, "Micro", &microP_tex) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (sceHiGetPlug((u_int *)DATA_BASE_ADDR, "Micro", &microP_base) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    _micro_arg.micro = 0;
    microP_tex->args = (u_int)&_micro_arg;
    microP_base->args = (u_int)&_micro_arg;


    /* get hrchy-plugin block from the data */
    if (sceHiGetPlug(DATA_TEX_ADDR, "Hrchy", &hrchyP_tex) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    _hrchy_arg.root= &root_mat_tex;
    hrchyP_tex->args = (u_int)&_hrchy_arg;


    /* camera & light */
    if((microD_tex = sceHiPlugMicroGetData(microP_tex)) == NULL)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if((microD_base = sceHiPlugMicroGetData(microP_base)) == NULL)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    mycamerainit(microD_tex);
    camerainit(microD_base);
    mylightinit(microD_tex);
    mylightinit(microD_base);
}

int SAMPLE14_main(void)
{
    sceHiErr	err;

    /* control texture object */
    _obj_tex_ctrl();

    if (sceHiCallPlug(frameP_tex, SCE_HIG_PRE_PROCESS) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (sceHiCallPlug(frameP_base, SCE_HIG_PRE_PROCESS) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* register context for texture-object */
    err = sceHiGsCtxRegist(gsctx_tex, 1);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* register texture-object draw */
    if (sceHiCallPlug(frameP_tex, SCE_HIG_POST_PROCESS) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* register context for base-object */
    sceHiGsCtxRegist(sceHiGsStdCtx, 0);

    /* register base object draw */
    if (sceHiCallPlug(frameP_base, SCE_HIG_POST_PROCESS) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* register context for debug_print */
    err = sceHiGsCtxRegist(sceHiGsStdCtx, 0);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    utilSetDMATimer();
	
    err = sceHiDMASend();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiDMAWait();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    draw_debug_print(50, 1, "Rup/Rdown    : Obj Up/Down");
    draw_debug_print(50, 2, "Rleft/Rright : Obj Left/Right");
    draw_debug_print(50, 3, "R1/R2        : Obj Back/Front");
    draw_debug_print(50, 4, "Lup/Ldown    : Obj Rot X");
    draw_debug_print(50, 5, "Lleft/Lright : Obj Rot Y");
    draw_debug_print(50, 6, "L1/L2        : Obj Rot Z");
    draw_debug_print(50, 7, "START        : EXIT");

    if (gPad.StartSwitch)
	return 1;

    _micro_arg.micro = -1;


    return 0;
}

void SAMPLE14_exit(void)
{

    sceHiDMAWait();
    sceHiDMASend();
    sceHiDMAWait();

    sceHiGsCtxDelete(gsctx_tex);

    if (sceHiCallPlug(frameP_tex, SCE_HIG_END_PROCESS) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if (sceHiCallPlug(frameP_base, SCE_HIG_END_PROCESS) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    sceHiGsMemFree(drawtexarea);

    free(DATA_TEX_ADDR);
    free(DATA_BASE_ADDR);

}


static void _obj_tex_ctrl(void)
{
    float  dr = 0.5f / 180.0f * PI;
    float  dt = 0.2f;

    if (gPad.LdownOn) {
	root_rot_tex[0]-=dr; 
	if(root_rot_tex[0]<-PI) root_rot_tex[0] += 2.0f*PI; 
    }
    if(gPad.LupOn) {
	root_rot_tex[0]+=dr;
	if(root_rot_tex[0]>PI) root_rot_tex[0] -= 2.0f*PI; 
    }
    if(gPad.LrightOn) {
	root_rot_tex[1]+=dr;
	if(root_rot_tex[1]>PI) root_rot_tex[1] -= 2.0f*PI; 
    }
    if(gPad.LleftOn) {
	root_rot_tex[1]-=dr;
	if(root_rot_tex[1]<-PI) root_rot_tex[1] += 2.0f*PI; 
    }
    if(gPad.L1On) {
	root_rot_tex[2]+=dr;
	if(root_rot_tex[2]>PI) root_rot_tex[2] -= 2.0f*PI; 
    }
    if(gPad.L2On) {
	root_rot_tex[2]-=dr;
	if(root_rot_tex[2]<-PI) root_rot_tex[2] += 2.0f*PI; 
    }
    if(gPad.RdownOn)
	root_trans_tex[1]+=dt;
    if(gPad.RupOn)
	root_trans_tex[1] -= dt;
    if(gPad.RrightOn)
	root_trans_tex[0] += dt;
    if(gPad.RleftOn)
	root_trans_tex[0] -= dt;
    if(gPad.R1On)
	root_trans_tex[2] -= dt;
    if(gPad.R2On)
	root_trans_tex[2] += dt;

    sceVu0UnitMatrix(root_mat_tex);
    sceVu0RotMatrixX(root_mat_tex, root_mat_tex, root_rot_tex[0]);
    sceVu0RotMatrixY(root_mat_tex, root_mat_tex, root_rot_tex[1]);
    sceVu0RotMatrixZ(root_mat_tex, root_mat_tex, root_rot_tex[2]);
    sceVu0TransMatrix(root_mat_tex, root_mat_tex, root_trans_tex);

    /* infomation */
    draw_debug_print(1, 6, "TEXTURE OBJ POS : %5.1f %5.1f %5.1f",
		     root_trans_tex[0],
		     root_trans_tex[1],
		     root_trans_tex[2]);
    draw_debug_print(1, 7, "TEXTURE OBJ ROT : %5.2f %5.2f %5.2f",
		     root_rot_tex[0],
		     root_rot_tex[1],
		     root_rot_tex[2]);
			 
}

