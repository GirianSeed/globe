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
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>
#include "util.h"
#include "camera.h"
#include "light.h"
#include <math.h>

/****************************************/
/*	Micro Codes			*/
/****************************************/
extern u_int sce_micro_cull[];
extern u_int sce_micro_shadowTex[];
extern u_int sce_micro_shadowSTQCull[];

/****************************************/
/*	Gs Context			*/
/****************************************/
static sceHiGsCtx *port0, *port1;
static sceHiGsPacked *flush;
static void gsctx_init(sceHiGsMemTbl *tbl, int width, int height)
{
    sceHiErr err;
    u_char addr[] = {SCE_GS_TEXFLUSH};

    /*	Texture Render	*/
    port0=sceHiGsCtxCreate(SCE_GS_FALSE);
    if(port0 == NULL) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    err = sceHiGsCtxSetDefaultValues(port0, SCE_GS_PSMCT32, SCE_GS_PSMZ24, SCE_GS_FALSE, width, height);
    err = sceHiGsCtxSetLumpBuffer(port0, tbl->addr/2048);
    err = sceHiGsCtxSetClearColor(port0, 0,0,0,128);
    err = sceHiGsCtxSetRegClamp(port0, SCE_GS_CLAMP, SCE_GS_CLAMP, 0, 0, 0, 0);
    err = sceHiGsCtxUpdate(port0);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    /*	Mapping Render	*/
    port1=sceHiGsCtxCreate(SCE_GS_TRUE);
    if(port1 == NULL) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    err = sceHiGsCtxCopy(port1, sceHiGsStdCtx);
    err = sceHiGsCtxSetDepth(port1, SCE_GS_PSMCT32, SCE_GS_PSMZ24, SCE_GS_TRUE);
    err = sceHiGsCtxSetRegAlpha(port1, SCE_GS_ALPHA_ZERO,SCE_GS_ALPHA_CS,SCE_GS_ALPHA_FIX,SCE_GS_ALPHA_CD,0x47);	/* SUB */
    err = sceHiGsCtxSetRegTest(port1, SCE_GS_FALSE, SCE_GS_ALPHA_NEVER, 0, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL);
    err = sceHiGsCtxSetRegClamp(port1, SCE_GS_CLAMP, SCE_GS_CLAMP, 0, 0, 0, 0);
    err = sceHiGsCtxSetRegTex0(port1, tbl->addr/64, width/64, SCE_GS_PSMCT32, (u_int)(logf((float)width)/logf(2.0f)), (u_int)(logf((float)height)/logf(2.0f)), 0, SCE_GS_DECAL, 0, 0, 0, 0, 0);
    err = sceHiGsCtxUpdate(port1);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    /*	Texflush	*/
    flush = sceHiGsPackedCreate(addr, sizeof(addr)/sizeof(u_char));
    if(flush == NULL) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
}

static void gsctx_exit(void)
{
    sceHiErr err;

    err = sceHiGsCtxDelete(port0);
    err = sceHiGsCtxDelete(port1);
    err = sceHiGsPackedDelete(flush);
}

static void alpha_ctrl(void)
{
    sceHiErr err;
    static int alpha = 0x47;

    if(gPad.RrightOn){
	alpha++;
	if(alpha > 255) alpha = 255;
    }
    if(gPad.RleftOn){
	alpha--;
	if(alpha < 0) alpha = 0;
    }

    err = sceHiGsCtxSetRegAlpha(port1, SCE_GS_ALPHA_ZERO,SCE_GS_ALPHA_CS,SCE_GS_ALPHA_FIX,SCE_GS_ALPHA_CD,alpha);	/* SUB */
    err = sceHiGsCtxUpdate(port1);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    draw_debug_print(1, 6, "SHADOW ALPHA : %d",alpha);
}

/****************************************/
/*	Prototypes			*/
/****************************************/
static void root_ctrl(void);

/****************************************/
/*	Root Matrix			*/
/****************************************/
static sceVu0FMATRIX root_mat;
static sceVu0FVECTOR root_rot;
static sceVu0FVECTOR root_trans;

static void myrootinit(void)
{
    static sceVu0FVECTOR rot = {0.0f,PI/4.0f,-PI/4.0f,0.0f};

    sceVu0UnitMatrix(root_mat);
    sceVu0CopyVector(root_rot, rot);
    sceVu0RotMatrix(root_mat, root_mat, root_rot);
    sceVu0ScaleVector(root_trans, root_trans, 0.0f);
    sceVu0TransMatrix(root_mat, root_mat, root_trans);
}

/****************************************/
/*	My Camera			*/
/****************************************/
static void mycamerainit(sceHiPlugMicroData_t *md)
{
    static sceVu0FVECTOR s = {640.0f, 224.0f, 512.0f, 300.0f};
    static sceVu0FVECTOR w = {1.0f, 0.47f, 2048.0f, 2048.0f};
    static sceVu0FVECTOR d = {5.0f, 16777000.0f, 1.0f, 65536.0f};
    static sceVu0FVECTOR p = { 0.0f, 0.0f, -12.0f, 0.0f };
    static sceVu0FVECTOR r = { 0.0f, 0.0f, 0.0f, 0.0f };
    static sceVu0FVECTOR i = { 0.0f, 0.0f, 0.0f, 0.0f };
    static sceVu0FVECTOR u = { 0.0f, 1.0f, 0.0f, 0.0f };

    perspective(md, s,w,d);
    viewset(p,r,i,u);
    cameraview(md);
}

/****************************************/
/*	My Light			*/
/****************************************/
static void mylightinit(sceHiPlugMicroData_t *md)
{
    static sceVu0FVECTOR dir={-7.0f,7.0f,5.0f,0.0f};
    static sceVu0FVECTOR pos={7.0f,-7.0f,-5.0f,0.8f};
    static sceVu0FVECTOR col={0.8f,0.8f,0.8f,0.0f};

    lightinit(md);
    lightstore(md, dir,pos,col,DIR_LIGHT,0);
}

static void mycopylightdir(sceHiPlugMicroData_t *dst, sceHiPlugMicroData_t *src)
{
    int i;

    /*	copy transpose & invert vector	*/
    for(i=0;i<3;i++){
	dst->light[DIR_LIGHT].dir[i][0] = -src->light[DIR_LIGHT].dir[0][i];
    }
}

/****************************************/
/*	Sample Main			*/
/****************************************/
void SAMPLE18_init();
int SAMPLE18_main();
void SAMPLE18_exit();

#define NUM_OF_FILE	2		/* num of files */
#define	NUM_OF_FRAME	5		/* num of frames */
#define	NUM_OF_MICRO	4		/* num of micro plug (ROOM1 without micro) */

static void		*data_addr[NUM_OF_FILE];
static sceHiPlug	*frameP[NUM_OF_FRAME];
static sceHiPlug	*microP[NUM_OF_MICRO];
static sceHiPlugMicroData_t *microD[NUM_OF_MICRO];

static sceHiPlugShadowMapInitArg_t	shadow_arg;
static sceHiGsMemTbl			*shadowtbl;
static sceHiPlugHrchyPreCalcArg_t	hrchyA;		/* for TORUS */
static sceHiPlugTex2dInitArg_t		tex2dA;		/* for ROOM */

enum { TORUS, ROOM, SHADOWMAP0, SHADOWMAP1, ROOM1};	/* frame id */

#define	SHADOWMAP_WIDTH	256
#define	SHADOWMAP_HEIGHT 256

void SAMPLE18_init(void)
{
	sceHiErr	err;
	sceHiType	type;
	char		filename[NUM_OF_FILE][20] = {"data/shadowmap0.bin","data/shadowmap1.bin"};
	char		framename[NUM_OF_FRAME][12] = {"ShadowObj","ShadowRec","ShadowTex","ShadowMap","ShadowRec1"};
	int		fileid[NUM_OF_FRAME] = {0,1,0,0,1};				/* for frame */
	int		frameid[NUM_OF_MICRO] = {TORUS, ROOM, SHADOWMAP0, SHADOWMAP1};	/* for micro */
	int		i;
	size_t		size;
	sceHiPlugMicroTbl_t	micro_tbl[NUM_OF_MICRO] = {
	    {sce_micro_cull, SCE_HIP_MICRO_ATTR_NONE},			/* TORUS */
	    {sce_micro_cull, SCE_HIP_MICRO_ATTR_NONE},			/* ROOM */
	    {sce_micro_shadowTex, SCE_HIP_MICRO_ATTR_NONE},		/* SHADOWMAP0 */
	    {sce_micro_shadowSTQCull, SCE_HIP_MICRO_ATTR_NONE},		/* SHADOWMAP1 */
	};
	sceHiPlugMicroInitArg_t	micro_arg[NUM_OF_MICRO] = {
	    {&micro_tbl[TORUS], 1},					/* TORUS */
	    {&micro_tbl[ROOM], 1},					/* ROOM */
	    {&micro_tbl[SHADOWMAP0], 1},				/* SHADOWMAP0 */
	    {&micro_tbl[SHADOWMAP1], 1},				/* SHADOWMAP1 */
	};

	/*	for Torus		*/
	sceHiPlug	*plug;
	u_int		*boxP;

	/*	Files		*/
	for(i=0;i<NUM_OF_FILE;i++){
	    data_addr[i] = file_read(filename[i]);
	    err = sceHiParseHeader((u_int *)data_addr[i]);
	    if(err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	}

	/*	Frames		*/
	for(i=0;i<NUM_OF_FRAME;i++){
	    err = sceHiGetPlug((u_int *)data_addr[fileid[i]], framename[i], &frameP[i]);
	    if(err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	}

	/*	Micro Plug & Data	*/
	type.repository = SCE_HIP_COMMON;
	type.project = SCE_HIP_FRAMEWORK;
	type.category = SCE_HIP_MICRO;
	type.status = SCE_HIG_PLUGIN_STATUS;
	type.id = SCE_HIP_MICRO_PLUG;
	type.revision = SCE_HIP_REVISION;
	for(i=0;i<NUM_OF_MICRO;i++){
	    err = sceHiGetInsPlug(frameP[frameid[i]], &microP[i], type);
	    if(err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	    microP[i]->args = (u_int)&micro_arg[i];		/* exec micro id default 0 */

	    if((microD[i] = sceHiPlugMicroGetData(microP[i])) == NULL)
		error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	}

	/*	Torus Hrchy	*/
	type.category = SCE_HIP_HRCHY;
	type.status = SCE_HIG_PLUGIN_STATUS;
	type.id = SCE_HIP_HRCHY_PLUG;
	err = sceHiGetInsPlug(frameP[TORUS], &plug, type);
	if(err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	hrchyA.root = &root_mat;
	plug->args = (u_int)&hrchyA;

	/*	Torus ShadowBox	*/
	type.category = SCE_HIP_SHADOW;				/* Get ShadowBox Plug */
	type.status = SCE_HIG_PLUGIN_STATUS;
	type.id = SCE_HIP_SHADOWBOX_PLUG;
	err = sceHiGetInsPlug(frameP[TORUS], &plug, type);
	if(err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	type.status = SCE_HIG_DATA_STATUS;			/* Get ShadowBox Data */
	type.id = SCE_HIP_SHADOWBOX_DATA;
	err = sceHiGetData(plug, &boxP, type);
	if(err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

	/*	Room Tex2d	*/
	type.category = SCE_HIP_TEX2D;
	type.status = SCE_HIG_PLUGIN_STATUS;
	type.id = SCE_HIP_TEX2D_PLUG;
	err = sceHiGetInsPlug(frameP[ROOM], &plug, type);
	if(err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	size = sceHiPlugTex2DSize(plug);
	tex2dA.resident = TRUE;
	tex2dA.tbl = sceHiGsMemAlloc(SCE_HIGS_PAGE_ALIGN, size);
	plug->args = (u_int)&tex2dA;

	/*	ShadowMap GsMem	*/
	size = sceHiGsPageSize(SHADOWMAP_WIDTH,SHADOWMAP_HEIGHT,SCE_GS_PSMCT32);
	shadowtbl = sceHiGsMemAlloc(SCE_HIGS_PAGE_ALIGN, size);	/* because texture render needs fbp align */

	/*	ShadowMap Args	*/
	shadow_arg.width = SHADOWMAP_WIDTH;
	shadow_arg.height = SHADOWMAP_HEIGHT;
	shadow_arg.box = boxP;					/* or = NULL when shadowmap has shadowbox */

	frameP[SHADOWMAP0]->args = (u_int)&shadow_arg;

	/*	INIT PROCESS	*/
	for(i=0;i<NUM_OF_FRAME;i++){
	    err = sceHiCallPlug(frameP[i], SCE_HIG_INIT_PROCESS);
	    if(err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	}

	/*	Init Etc	*/
	for(i=0;i<NUM_OF_MICRO;i++){
	    mycamerainit(microD[i]);
	    mylightinit(microD[i]);
	}
	myrootinit();
	gsctx_init(shadowtbl, shadow_arg.width, shadow_arg.height);
	lgpini();
}

enum {CAMERA_MODE,LIGHT_MODE,ROOT_MODE,ALPHA_MODE};

int SAMPLE18_main(void)
{
	sceHiErr	err;
	static int mode = LIGHT_MODE;
	int i;

	if (gPad.SelectSwitch)
	    mode = (++mode) > ALPHA_MODE ? CAMERA_MODE : mode;

	switch(mode){
	  case CAMERA_MODE:
	      cameractrl();
	      cameraview(microD[TORUS]);
	      for(i=1;i<NUM_OF_MICRO;i++){
		  cameracopy(microD[i], microD[TORUS]);
	      }
	      break;
	  case LIGHT_MODE:
	      lightctrl(microD[TORUS],DIR_LIGHT);
	      for(i=1;i<NUM_OF_MICRO;i++){
		  lightcopy(microD[i], microD[TORUS], DIR_LIGHT);
	      }
	      break;
	  case ROOT_MODE:
	      root_ctrl();
	      break;
	  case ALPHA_MODE:
	      alpha_ctrl();
	      break;
 	  default:
            break;
	}

	/*	Calc Scene	*/
	for(i=0;i<NUM_OF_FRAME;i++){
	    err = sceHiCallPlug(frameP[i], SCE_HIG_PRE_PROCESS);	/* ShadowMap0 after Torus PreProcessing */
	    if(err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	}

	/*	Copy LightDir for DirectionalLight Scene	*/
	for(i=0;i<NUM_OF_MICRO;i++){
	    if(i==SHADOWMAP1) continue;
	    mycopylightdir(microD[i],microD[SHADOWMAP1]);
	}

	/*	Render Room	*/
	err = sceHiGsCtxRegist(sceHiGsStdCtx, SCE_GS_FALSE);
	err = sceHiCallPlug(frameP[ROOM], SCE_HIG_POST_PROCESS);
	if(err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

	/*	Render ShadowTexture by Torus	*/
	err = sceHiGsCtxRegist(port0, SCE_GS_TRUE);
	err = sceHiCallPlug(frameP[SHADOWMAP0], SCE_HIG_POST_PROCESS);
	if(err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	err = sceHiStopPlugStatus(microP[TORUS]);			/* Stopping Torus Micro */
	err = sceHiCallPlug(frameP[TORUS], SCE_HIG_POST_PROCESS);
	if(err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	err = sceHiContPlugStatus(microP[TORUS]);			/* Continue Torus Micro */

	/*	Render ShadowMapping by Room1	*/
	err = sceHiDMARegist(flush->giftag->id);
	err = sceHiGsCtxRegist(port1, SCE_GS_FALSE);
	err = sceHiCallPlug(frameP[SHADOWMAP1], SCE_HIG_POST_PROCESS);
	if(err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	err = sceHiCallPlug(frameP[ROOM1], SCE_HIG_POST_PROCESS);	/* Room1 has no Micro */
	if(err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

	/*	Render Torus	*/
	err = sceHiGsCtxRegist(sceHiGsStdCtx, SCE_GS_FALSE);
	err = sceHiCallPlug(frameP[TORUS], SCE_HIG_POST_PROCESS);
	if(err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

        err = sceHiGsCtxRegist(sceHiGsStdCtx, 0);
	if(mode == LIGHT_MODE) lgpreg();	/* render light gp */

	utilSetDMATimer();
	err = sceHiDMASend();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	err = sceHiDMAWait();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);

	if(gPad.StartSwitch)	return 1;
	return 0;
}

void SAMPLE18_exit(void)
{
	sceHiErr	err;
	int i;

	for(i=0;i<NUM_OF_FRAME;i++){
	    err = sceHiCallPlug(frameP[i], SCE_HIG_END_PROCESS);
	    if(err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	}
	err = sceHiGsMemFree(tex2dA.tbl);
	err = sceHiGsMemFree(shadowtbl);
	for(i=0;i<NUM_OF_FILE;i++){
	    free(data_addr[i]);
	}

	gsctx_exit();
	lgpdel();
}

/****************************************************************/
/*	Root Ctrl			                        */
/*  shifts also the light_pos for the light used for the shadow */
/****************************************************************/

static void root_ctrl(void)
{
    float  dr = 0.5f / 180.0f * PI;
    float  dt = 0.2f;
    int i;
 
	if (gPad.LdownOn) {
		root_rot[0]-=dr; 
		if(root_rot[0]<-PI) root_rot[0] += 2.0f*PI; 
	}
	if(gPad.LupOn) {
		root_rot[0]+=dr;
		if(root_rot[0]>PI) root_rot[0] -= 2.0f*PI; 
	}
	if(gPad.LrightOn) {
		root_rot[1]+=dr;
		if(root_rot[1]>PI) root_rot[1] -= 2.0f*PI; 
	}
	if(gPad.LleftOn) {
		root_rot[1]-=dr;
		if(root_rot[1]<-PI) root_rot[1] += 2.0f*PI; 
	}
	if(gPad.L1On) {
		root_rot[2]+=dr;
		if(root_rot[2]>PI) root_rot[2] -= 2.0f*PI; 
	}
	if(gPad.L2On) {
		root_rot[2]-=dr;
		if(root_rot[2]<-PI) root_rot[2] += 2.0f*PI; 
	}
	if(gPad.RdownOn) {
                microD[TORUS]->light[0].pos[0][1]+=dt;     
		root_trans[1]+=dt;
        }
	if(gPad.RupOn) {
                microD[TORUS]->light[0].pos[0][1]-=dt;     
		root_trans[1] -= dt;
        }
	if(gPad.RrightOn) {
                microD[TORUS]->light[0].pos[0][0]+=dt;  
		root_trans[0] += dt;
        }
	if(gPad.RleftOn)  {
                microD[TORUS]->light[0].pos[0][0]-=dt;  
		root_trans[0] -= dt;
        }
	if(gPad.R1On) {
                microD[TORUS]->light[0].pos[0][2]-=dt;  
		root_trans[2] -= dt;
        }
	if(gPad.R2On) {
                microD[TORUS]->light[0].pos[0][2]+=dt;  
		root_trans[2] += dt;
        }

        for(i=1;i<NUM_OF_MICRO;i++){
                lightcopy(microD[i], microD[TORUS], DIR_LIGHT);
        }              

	sceVu0UnitMatrix(root_mat);
	sceVu0RotMatrix(root_mat, root_mat, root_rot);
	sceVu0TransMatrix(root_mat, root_mat, root_trans);

	/* infomation */
	draw_debug_print(1, 6, "TORUS ROOT POS : %5.2f %5.2f %5.2f",
			 root_trans[0],
			 root_trans[1],
			 root_trans[2]);
	draw_debug_print(1, 7, "TORUS ROOT ROT : %5.2f %5.2f %5.2f",
			 root_rot[0],
			 root_rot[1],
			 root_rot[2]);
}
