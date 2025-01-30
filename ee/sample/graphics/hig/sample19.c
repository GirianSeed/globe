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
#include <math.h>
#include "util.h"
#include "camera.h"
#include "light.h"

#define	NUM	2			/* num of Scene */
enum { WALL, LIGHTMAP};			/* Scene ID */

static sceHiGsMemTbl	*gtbl[NUM];	/* Base & Light Texture GsMem */

/****************************************/
/*	LightMap Texture		*/
/****************************************/
#define LIGHTMAP_WIDTH	256
#define LIGHTMAP_HEIGHT	256
#define LIGHTMAP_PSM	SCE_GS_PSMCT24
#define	LIGHTMAP_FILE	"data/spot.raw"
static u_int *texbuf;

static void trans_tex(u_int *texbuf, int tbp, int psm, int width, int height)
{
    sceGsLoadImage li;

    sceGsSetDefLoadImage(&li, tbp, (width+63)/64, psm, 0, 0, width, height);
    SyncDCache(&li, ((char *)&li)+sizeof(li));
    sceGsExecLoadImage(&li, (u_long128 *)texbuf);
    sceGsSyncPath(0,0);
}

static void load_tex(void)
{
    texbuf=file_read(LIGHTMAP_FILE);
    trans_tex(texbuf, gtbl[LIGHTMAP]->addr/64, LIGHTMAP_PSM,LIGHTMAP_WIDTH, LIGHTMAP_HEIGHT);
}


/****************************************/
/*	Micro Codes			*/
/****************************************/
extern u_int sce_micro_cull[];
extern u_int sce_micro_lightmapSTQCull[];

/****************************************/
/*	Drawport			*/
/****************************************/
static sceHiGsCtx *port[NUM];
#define WALLTEX_WIDTH	256
#define	WALLTEX_HEIGHT	256
#define	WALLTEX_PSM	SCE_GS_PSMCT32

static void drawport_init(void)
{
    sceHiErr err;
 
    port[WALL]=sceHiGsCtxCreate(1);
    if (port[WALL]==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsCtxCopy(port[WALL], sceHiGsStdCtx);
    err = sceHiGsCtxSetRegAlpha(port[WALL], SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_ZERO, 128); /* NONE */
    err = sceHiGsCtxSetRegTest(port[WALL], SCE_GS_FALSE, SCE_GS_ALPHA_NEVER, 0, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GREATER);    
    err = sceHiGsCtxSetRegClamp(port[WALL], SCE_GS_CLAMP, SCE_GS_CLAMP, 0, 0, 0, 0);
    err = sceHiGsCtxSetRegTex0(port[WALL], gtbl[WALL]->addr/64, WALLTEX_WIDTH/64, WALLTEX_PSM, (u_int)(logf((float)WALLTEX_WIDTH)/logf(2.0f)), (u_int)(logf((float)WALLTEX_HEIGHT)/logf(2.0f)), 0, SCE_GS_DECAL, 0, 0, 0, 0, 0);
    err = sceHiGsCtxUpdate(port[WALL]);
 
    port[LIGHTMAP]=sceHiGsCtxCreate(1);
    if (port[LIGHTMAP]==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsCtxCopy(port[LIGHTMAP], sceHiGsStdCtx);
    err = sceHiGsCtxSetRegAlpha(port[LIGHTMAP], SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CD, 0x47);	/* ADD */
    err = sceHiGsCtxSetRegTest(port[LIGHTMAP], SCE_GS_FALSE, SCE_GS_ALPHA_NEVER, 0, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL);   
    err = sceHiGsCtxSetRegClamp(port[LIGHTMAP], SCE_GS_CLAMP, SCE_GS_CLAMP, 0, 0, 0, 0);
    err = sceHiGsCtxSetRegTex0(port[LIGHTMAP], gtbl[LIGHTMAP]->addr/64, LIGHTMAP_WIDTH/64, LIGHTMAP_PSM, (u_int)(logf((float)LIGHTMAP_WIDTH)/logf(2.0f)), (u_int)(logf((float)LIGHTMAP_HEIGHT)/logf(2.0f)), 0, SCE_GS_MODULATE, 0, 0, 0, 0, 0);
    err = sceHiGsCtxUpdate(port[LIGHTMAP]);
}


static void drawport_exit(void)
{
    sceHiErr	err;
    int		i;

    for(i=0;i<NUM;i++){
	err = sceHiGsCtxDelete(port[i]);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
}


static void drawport_alpha(sceHiPlugMicroData_t *md)
{
    sceHiErr err;
    int	alpha;

    alpha = (int)md->light[DIR_LIGHT].col[0][3];	/* Spot Intensity */

    if(alpha > 255)	alpha = 255;
    if(alpha < 0)	alpha = 0;

    err = sceHiGsCtxSetRegAlpha(port[LIGHTMAP], SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CD, alpha);	/* ADD */
    err = sceHiGsCtxUpdate(port[LIGHTMAP]);
}

/****************************************/
/*	Default Light Position		*/
/***************************************/
static void mylightinit(sceHiPlugMicroData_t *md)
{
    sceVu0FVECTOR dir={0.0f,0.0f,1.0f,0.0f};
    sceVu0FVECTOR pos={0.0f,0.0f,-10.0f,0.7f};
    sceVu0FVECTOR col={0.8f,0.8f,0.8f,128.0f};

    lightinit(md);
    lightstore(md, dir, pos, col, DIR_LIGHT, 0);
}

/****************************************/
/*	Light Mapping Sample		*/
/****************************************/
static void		*data_addr[NUM];
static sceHiPlug	*frameP[NUM];
static sceHiPlug	*microP[NUM];
static sceHiPlugMicroData_t *microD[NUM];

static sceHiPlugTex2dInitArg_t	tex2d_arg;

void SAMPLE19_init(void)
{
    sceHiErr	err;
    sceHiType	type;
    char	filename[NUM][20] = {"data/wall.bin","data/litmap.bin"};
    char	framename[NUM][12] = {"Wall","LightMap"};
    int		i;
    sceHiPlug	*tex2dP;

    sceHiPlugMicroTbl_t	micro_tbl[NUM] = {
	{sce_micro_cull, SCE_HIP_MICRO_ATTR_NONE},		/*	WALL		*/
	{sce_micro_lightmapSTQCull, SCE_HIP_MICRO_ATTR_NONE},	/*	LIGHTMAP	*/
    };
    sceHiPlugMicroInitArg_t	micro_arg[NUM] = {
	{&micro_tbl[WALL], 1},{&micro_tbl[LIGHTMAP], 1}
    };
    sceHiPlugLightMapInitArg_t	lightmap_arg;

    /*	Get Micro Plug & Data	*/
    for(i=0;i<NUM;i++){
	data_addr[i] = file_read(filename[i]);
	err = sceHiParseHeader((u_int *)data_addr[i]);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiGetPlug((u_int *)data_addr[i], framename[i], &frameP[i]);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	type.repository = SCE_HIP_COMMON;
	type.project = SCE_HIP_FRAMEWORK;
	type.category = SCE_HIP_MICRO;
	type.status = SCE_HIG_PLUGIN_STATUS;
	type.id = SCE_HIP_MICRO_PLUG;
	type.revision = SCE_HIP_REVISION;
	err = sceHiGetInsPlug(frameP[i], &microP[i], type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	microP[i]->args = (u_int)&micro_arg[i];		/* exec micro id default 0 */

	if((microD[i] = sceHiPlugMicroGetData(microP[i])) == NULL)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    /*	Tex2D Args	*/
    gtbl[WALL] = sceHiGsMemAlloc(SCE_HIGS_BLOCK_ALIGN, sceHiGsBlockSize(WALLTEX_WIDTH, WALLTEX_HEIGHT, WALLTEX_PSM));
    tex2d_arg.resident = TRUE;
    tex2d_arg.tbl = gtbl[WALL];
    type.category = SCE_HIP_TEX2D;
    err = sceHiGetInsPlug(frameP[WALL], &tex2dP, type);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    tex2dP->args = (u_int)&tex2d_arg;

    /*	LightMap Args	*/
    gtbl[LIGHTMAP] = sceHiGsMemAlloc(SCE_HIGS_BLOCK_ALIGN, sceHiGsBlockSize(LIGHTMAP_WIDTH, LIGHTMAP_HEIGHT, LIGHTMAP_PSM));
    lightmap_arg.width = LIGHTMAP_WIDTH;
    lightmap_arg.height = LIGHTMAP_HEIGHT;
    lightmap_arg.fov = FALSE;
    frameP[LIGHTMAP]->args = (u_int)&lightmap_arg;


    /*	INIT PROCESS	*/
    for(i=0;i<NUM;i++){
	err = sceHiCallPlug(frameP[i], SCE_HIG_INIT_PROCESS);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    for(i=0;i<NUM;i++){
	camerainit(microD[i]);
	mylightinit(microD[i]);
    }
    drawport_init();
    load_tex();
    lgpini();
}

enum {CAMERA_MODE, LIGHT_MODE};

int SAMPLE19_main(void)
{
    sceHiErr	err;
    static int	mode = CAMERA_MODE;	/* mode select */
    int		i;

    if (gPad.SelectSwitch)	mode = (++mode) > LIGHT_MODE ? CAMERA_MODE : mode;

    switch (mode) {
      case CAMERA_MODE:
          draw_debug_print(1, 4, "MODE : CAMERA MOVE");
          cameractrl();
	  cameraview(microD[WALL]);
	  cameracopy(microD[LIGHTMAP], microD[WALL]);
          break;
      case LIGHT_MODE:
          draw_debug_print(1, 4, "MODE : LIGHT CTRL");
          lightctrl(microD[WALL], DIR_LIGHT);
          break;
    }


    lightcopy(microD[LIGHTMAP], microD[WALL], DIR_LIGHT);  
    drawport_alpha(microD[LIGHTMAP]);

    for(i=0;i<NUM;i++){
	err = sceHiCallPlug(frameP[i], SCE_HIG_PRE_PROCESS);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    err = sceHiGsCtxRegist(port[WALL], 0);
    err = sceHiCallPlug(frameP[WALL], SCE_HIG_POST_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsCtxRegist(port[LIGHTMAP], 0);
    err = sceHiCallPlug(frameP[LIGHTMAP], SCE_HIG_POST_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiStopPlugStatus(microP[WALL]);
    err = sceHiCallPlug(frameP[WALL], SCE_HIG_POST_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiContPlugStatus(microP[WALL]);

    err = sceHiGsCtxRegist(sceHiGsStdCtx, 0);
    if(mode==LIGHT_MODE) lgpreg();

    utilSetDMATimer();
    err = sceHiDMASend();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiDMAWait();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (gPad.StartSwitch)
	return 1;
    return 0;
}


void SAMPLE19_exit(void)
{
    sceHiErr	err;
    int		i;

    for(i=0;i<NUM;i++){
	err = sceHiCallPlug(frameP[i], SCE_HIG_END_PROCESS);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    for(i=0;i<NUM;i++){
	err = sceHiGsMemFree(gtbl[i]);
    }
    for(i=0;i<NUM;i++){
	free(data_addr[i]);
    }
    free(texbuf);
    drawport_exit();
    lgpdel();
}
