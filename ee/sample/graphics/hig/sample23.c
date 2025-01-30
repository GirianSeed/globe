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
#include <string.h>
#include <eeregs.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>
#include "util.h"
#include "camera.h"
#include "light.h"

/***************************************************
 * sample program 23
 *
 *	full screen antialiasing
 *	
 *	After the drawing to the draw buffer, which is vertically as twice big 
 *	as the display buffer, copy the picture to the display buffer with reducing 
 * 	bilinear filter.
 * 	
 * 	the result is equivalent to two times super-sampling in vertical, [1,1] filter in holizontal.
 *
 *  GS memory allocation on this sample
 *  +----------+
 *  |disp buff | disp height
 *  +----------+
 *  |draw buff |
 *  |          | draw height = disp height*2
 *  |          |
 *  +----------+
 *  | Z buff   |
 *  |          | draw height
 *  |          |
 *  +----------+
 *  | texture  |
 *  |   buff   |
 *  |          |
 *  |          |
 *  +----------+
 *
 * after painting picuture to draw buffer, reduce and bi-linear copy to disp buffer in VSYNC
 * 
 * the depth of the display buffer doesn't have to be the same as its of the drawing buffer.
 ***************************************************/

extern u_int sce_micro_vertex_cull[];
extern u_int sce_micro_lambert_cull[];
extern u_int sce_micro_phong_cull[];

static sceHiPlugMicroTbl_t my_micro[] = {
    /* backface culling */
    { sce_micro_vertex_cull, 	SCE_HIP_MICRO_ATTR_NONE }, /* 0 : vertex color : pallarel light */
    { sce_micro_lambert_cull,	SCE_HIP_MICRO_ATTR_NONE }, /* 1 : lambert      : pallarel light */
    { sce_micro_phong_cull,	SCE_HIP_MICRO_ATTR_NONE }, /* 2 : phong        : pallarel light */

};
#define MICRO_NUM (sizeof(my_micro) / sizeof(sceHiPlugMicroTbl_t))

enum {
    F32Z24,
    F32Z16,
    F16Z24,
    F16Z16,
    PSMCTMODENUM
};

static char *files[]={"data/dino.bin", "data/APEWALK39.bin"};
#define FILENUM (sizeof(files)/sizeof(char *))

/* the location of each model */
static sceVu0FMATRIX lw[]={
    {
	{0.0f, 0.0f, 1.0f, 0.0f},
	{0.0f, 1.0f, 0.0f, 0.0f},
	{-1.0f, 0.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, -10.0f, 1.0f}
    }, {
	{1.0f, 0.0f, 0.0f, 0.0f},
	{0.0f, 1.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 1.0f, 0.0f},
	{0.0f, 5.0f, 5.0f, 1.0f}
    }
};

/* max BLUR parameter */
#define MAXBLUR 0.9f


/* the size of drawing */
static int _dispw=640;
static int _disph=224;

typedef struct {
    void *data;
    sceHiPlug	*frameP;
    sceHiPlug	*microP;
    sceHiPlug	*hrchyP;
    sceHiPlug	*tex2dP;
    sceHiPlugMicroData_t *microD;
    sceHiPlugHrchyPreCalcArg_t hrchy_pre_arg;
    sceHiPlugMicroPreCalcArg_t micro_pre_arg;
} MODEL;

typedef struct {
    sceHiGsGiftag giftag;
    u_long texflush;
    u_long texflush_addr;
    sceGsTex0   tex0;
    u_long tex0_addr;
    sceGsPrim   prim;
    u_long prim_addr;
    sceHiGsGiftag giftag2;
    struct {
	sceGsUv     uv;
	sceGsXyz  xyz;
    } box[2];
} PACK __attribute__ ((aligned(64)));

static sceHiPlugTex2dInitArg_t tex2d_arg;
static sceHiPlugMicroInitArg_t micro_arg;


static PACK packon, packoff, *packp;
static MODEL *model;

/* port of draw buffer */
static sceHiGsCtx *gsctx_draw_aaon;	/* for AntiAlias on */  
static sceHiGsCtx *gsctx_draw_aaoff;	/* for AntiAlias off */
static sceHiGsCtx *gsctx_draw;		/* for handling */

/* port for copies to disp buffer */
static sceHiGsCtx *gsctx_disp_aaon;	/* for AntiAlias on */ 
static sceHiGsCtx *gsctx_disp_aaoff;	/* for AntiAlias off */
static sceHiGsCtx *gsctx_disp;		/* for handling */


/* gsenv for write display buffer */
static sceHiGsEnv *gsenv_disp;



/* returning hander for VSYNC */
static int (*_vblank_save)(int);

/* depth mode of the drawing buffer */
static int psmctmode;
/* string to display the current drawing buffer depth on the screen */
static char strdepth[128];

/* GS memory for draw buffer */
static sceHiGsMemTbl *AAbuf_tbl;

/* the flag for timing control for changing disp/draw setting */
static int depth_changed;

/* flag for Antialias */
static int aaon;

/* z depth value */
static float zmax;

/* local functions */
static int syncv_callback(int ca);
static void init_disp(void);
static void init_draw(void);
static void blur_ctrl(void);
static void aa_ctrl(void);
static void set_buffer_depth_draw(int psmctmode);
static void set_buffer_depth_disp(void);
static int file_init(char *file, MODEL *m);
static void file_exit(MODEL *m);
static void set_aa_packet(PACK *p, sceHiGsCtx *gsctx, int x, int y, int dw, int dh, int tw, int th, int dx, int dy);
static void wait_dmagif(void);
static void send_aa(void);
static void myscreen(sceHiPlugMicroData_t *md);

void SAMPLE23_init(void)
{
    int i;
    int size;

    model=malloc(sizeof(MODEL)* FILENUM);
    if (model==NULL) error_handling(__FILE__,__LINE__,"can't malloc model");

    /* change the display buffer to single */
    if (sceHiGsDisplayMode(SCE_HIGS_RESET|SCE_HIGS_NTSCI|SCE_HIGS_RGBA|SCE_HIGS_DEPTH0|SCE_HIGS_SINGLE)
	!= SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if (sceHiGsDisplaySize(_dispw, _disph)!= SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* set ports for copying */
    init_disp();

    /* set ports for drawing */
    init_draw();
    
    /* allocate GS memory for draw/disp */
    size=sceHiGsCtxChkSize(gsctx_draw_aaon)+sceHiGsCtxChkSize(gsctx_disp_aaon); /* maximum size */
    AAbuf_tbl=NULL;
    AAbuf_tbl = sceHiGsMemAlloc(SCE_HIGS_PAGE_ALIGN, size);
    if (AAbuf_tbl==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    sceHiGsCtxSetLumpBuffer(gsctx_draw_aaon, AAbuf_tbl->addr/2048);
    sceHiGsCtxSetLumpBuffer(gsctx_draw_aaoff, AAbuf_tbl->addr/2048);

    
    /* allocate GS memory for texture */
    tex2d_arg.tbl = sceHiGsMemAlloc(SCE_HIGS_PAGE_ALIGN, 
				    SCE_HIGS_PAGE_ALIGN*sceHiGsMemRestSize()/SCE_HIGS_PAGE_ALIGN);
    if (tex2d_arg.tbl==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    tex2d_arg.resident = FALSE;

    micro_arg.tbl = my_micro;
    micro_arg.tblnum = MICRO_NUM;

    /* read model file for drawing, and initialize */
    for (i=0; i<FILENUM; i++){
	if (file_init(files[i], &model[i])<0) return;

	/* setting location */
	model[i].hrchy_pre_arg.root= &lw[i];
	model[i].hrchyP->args= (u_int)&model[i].hrchy_pre_arg;
    }

    /* initialize camera */
    /* initialize light */
    for(i=0;i<FILENUM;i++){
	camerainit(model[i].microD);
	lightinit(model[i].microD);
    }

    psmctmode=F32Z24;
    depth_changed=0;
    aaon=1;
    zmax=(1L<<24)-1.0f;

    /* the following settings sould be called after allocating GS memoery and initializing camera */
    set_buffer_depth_draw(psmctmode);
    set_buffer_depth_disp();

    /* add VSync interrupt handler */
    _vblank_save=(int (*)(int))sceGsSyncVCallback(syncv_callback);

}

int SAMPLE23_main(void)
{
    sceHiErr	err;
    int i;
    MODEL *m;

    blur_ctrl();

    if (depth_changed) set_buffer_depth_disp();

    aa_ctrl();

    cameractrl();


    for (i=0; i<FILENUM; i++){
	m= &model[i];
	cameraview(m->microD);
	m->microP->args = (u_int)&m->micro_pre_arg;

	if (sceHiCallPlug(m->frameP, SCE_HIG_PRE_PROCESS) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	
	if (sceHiCallPlug(m->frameP, SCE_HIG_POST_PROCESS) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    utilSetDMATimer();
    err = sceHiDMASend();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiDMAWait();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (depth_changed) set_buffer_depth_draw(psmctmode);
    if((aaon)||(depth_changed)){
	for(i=0;i<FILENUM;i++) myscreen(model[i].microD);
    }

    if (gPad.StartSwitch)
	return 1;

    return 0;
}

void SAMPLE23_exit(void)
{
    int i;

    if (_vblank_save){
	sceGsSyncVCallback(_vblank_save);
	_vblank_save=NULL;
    }
    else {
	sceGsGParam *gp;

	gp=sceGsGetGParam();
	DisableIntc(INTC_VBLANK_S);	
	RemoveIntcHandler(INTC_VBLANK_S, gp->sceGsVSCid);
	gp->sceGsVSCfunc=NULL;
	gp->sceGsVSCid=0;
    }

    for (i=0; i<FILENUM; i++){
	file_exit(&model[i]);
    }

    if (AAbuf_tbl){
	sceHiGsMemFree(AAbuf_tbl);
	AAbuf_tbl=NULL;
    }

    if (tex2d_arg.tbl){
	sceHiGsMemFree(tex2d_arg.tbl);
	tex2d_arg.tbl=NULL;
    }

    /* delete all gsctx's and gsenv's */
    if (sceHiGsCtxDelete(gsctx_draw_aaon) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if (sceHiGsCtxDelete(gsctx_draw_aaoff) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if (sceHiGsCtxDelete(gsctx_disp_aaon) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if (sceHiGsCtxDelete(gsctx_disp_aaoff) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if (sceHiGsEnvDelete(gsenv_disp) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* restore the original display mode */
    if (sceHiGsDisplayMode(SCE_HIGS_RESET|SCE_HIGS_NTSCI|SCE_HIGS_RGBA|SCE_HIGS_DEPTH24)
	!= SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if (sceHiGsDisplaySize(640, 224)!= SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
}


/* execute in VSYNC interrupt */
static int syncv_callback(int ca)
{
    int odev;

    if (gsctx_disp->isInterlace){	/* need setting of interlace? */
	if (DGET_GS_CSR()&GS_CSR_FIELD_M) odev=1;
	else odev=0;
	sceHiGsCtxSwap(gsctx_disp, gsctx_disp->isDbuf?gsctx_disp->swap:0, odev);
	sceHiGsCtxFcacheI(gsctx_disp, 0);
	wait_dmagif();
    }

    /* send the port to copy buffer */
    sceHiGsEnvSend(gsenv_disp);
    sceHiGsCtxSend(gsctx_disp, 0);
    wait_dmagif();
    /* copy the draw buffer to disp buffer */
    send_aa();

    /* if there was a pre-set callback at SyncVCallback, call it here */
    ExitHandler();
    if (_vblank_save) return _vblank_save(ca);
    else return 0;
}


/* wait DMA_GIF finish */
static void wait_dmagif(void)
{
    while (DGET_D2_CHCR()&D_CHCR_STR_M){
	volatile int i;
	for (i=0; i<500; i++) ;
    }
}

/* send the packet to copy the drawing buffer to the display buffer */
static void send_aa(void)
{
    DPUT_D2_QWC((sizeof(PACK)/sizeof(u_long128)));
    DPUT_D2_MADR(((u_int) packp & 0x1fffffff));
    DPUT_D2_CHCR((1 | (0 << 2) | (0 << 4) | (0 << 5) | (0 << 6) | (0 << 7) | (1 << 8)));
}


/* set ports to copy the drawing buffer to the display buffer */
static void init_disp(void)
{
    /* port setting for copies of common */
    gsenv_disp=sceHiGsEnvCreate(SCE_HIGS_VALID_DTHE);
    if (gsenv_disp==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    sceHiGsEnvSetRegDthe(gsenv_disp, 0);	/* the display buffer of this sample is always PSMCT32, then do off dither */
    sceHiGsEnvUpdate(gsenv_disp);


    /* set the port for AntiAlias ON */
    gsctx_disp_aaon=sceHiGsCtxCreate(0);
    if (gsctx_disp_aaon==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    sceHiGsCtxCopy(gsctx_disp_aaon, sceHiGsStdCtx);	     /* same as DISP buffer basically */
    gsctx_disp_aaon->value.test.ATE=0;					      /* make sure test off */
    gsctx_disp_aaon->value.test.DATE=0;					      /* make sure test off */
    sceHiGsCtxSetClearMode(gsctx_disp_aaon, SCE_HIGS_CLEAR_KEEP);	     /* no clear needed */
    /* above here, they were common settings for both AA ON/OFF */
    /* following lines are only for AA-ON */
    sceHiGsCtxSetRegClamp(gsctx_disp_aaon, SCE_GS_REGION_CLAMP, SCE_GS_REGION_CLAMP, 0, _dispw, 0, _disph*2);
    sceHiGsCtxSetRegTex1(gsctx_disp_aaon, 0, 0, SCE_GS_LINEAR, SCE_GS_LINEAR, 0, 0, 0); /* bilinear  */
    gsctx_disp_aaon->isInterlace=1;		/* add interlace-offset here */
    sceHiGsCtxUpdate(gsctx_disp_aaon);

    /* set the port for AntiAlias OFF */
    gsctx_disp_aaoff=sceHiGsCtxCreate(0);
    if (gsctx_disp_aaoff==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    sceHiGsCtxCopy(gsctx_disp_aaoff, gsctx_disp_aaon);		/* copy from the one for Antialias-ON */
    /* following lines are only for AA-OFF */
    sceHiGsCtxSetRegClamp(gsctx_disp_aaoff, SCE_GS_REGION_CLAMP, SCE_GS_REGION_CLAMP, 0, _dispw, 0, _disph);
    sceHiGsCtxSetRegTex1(gsctx_disp_aaoff, 0, 0, SCE_GS_NEAREST, SCE_GS_NEAREST, 0, 0, 0); /* point sampling  */
    gsctx_disp_aaoff->isInterlace=0;		/* no interlace-offset here; already done in the drawing buffer */
    sceHiGsCtxUpdate(gsctx_disp_aaoff);
}

/* setting drawing port */
static void init_draw(void)
{
    int dimx[16]={
	-4, 2, -3, 3,
	0, -2, 1, -1,
	-3, 3, -2, 2,
	1, -1, 0, -4
    };

    /* setting drawing port for AntiAlias ON */
    gsctx_draw_aaon=sceHiGsCtxCreate(0);
    if (gsctx_draw_aaon==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    sceHiGsCtxSetDefaultValues(gsctx_draw_aaon, SCE_GS_PSMCT32, SCE_GS_PSMZ32, 1, _dispw, _disph*2);
    gsctx_draw_aaon->isInterlace=0;		/* no interlace-offset here */


    /* setting drawing port for AntiAlias OFF */
    gsctx_draw_aaoff=sceHiGsCtxCreate(0);
    if (gsctx_draw_aaoff==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    sceHiGsCtxSetDefaultValues(gsctx_draw_aaoff, SCE_GS_PSMCT32, SCE_GS_PSMZ32, 1, _dispw, _disph);
    gsctx_draw_aaoff->isInterlace=1;		/* add interlace-offset here */

    /* set dither matrix */
    sceHiGsEnvSetRegDimx(sceHiGsStdEnv, dimx);
    sceHiGsEnvUpdate(sceHiGsStdEnv);

}

/* control depth of motion-blur */
/* there is no over-head cost by adding motion-blur to this antialias method */
static void blur_ctrl(void)
{
    static float blur=0.0f;
    float bt=0.01f;

    if (gPad.L1On) blur+=bt;
    if (gPad.L2On) blur-=bt;
    gPad.L1On=0;
    gPad.L2On=0;
    if (blur>MAXBLUR) blur=MAXBLUR;
    if (blur<0.0f) blur=0.0f;

    sceHiGsCtxSetRegAlpha(gsctx_disp,
			SCE_GS_ALPHA_CD, SCE_GS_ALPHA_CS, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CS,
			(int)(blur*128));

    sceHiGsCtxUpdate(gsctx_disp);
    draw_debug_print(1, 13, "BLUR : %8.2f   [L1/L2]", blur);
}

/* control on/off antialias and the depth of display buffer */
static void aa_ctrl(void)
{
    depth_changed=0;

    if (gPad.RrightSwitch) {
	aaon ^= 1;

	depth_changed=1;
    }
    gPad.RrightOn=0;
    gPad.RleftOn=0;

    if (gPad.RupSwitch || gPad.RdownSwitch){
	if (gPad.RupSwitch){
	    psmctmode=(psmctmode+1)%PSMCTMODENUM;
	} else if (gPad.RdownSwitch){
	    psmctmode=(psmctmode+PSMCTMODENUM-1)%PSMCTMODENUM;
	}
	depth_changed=1;
    }
    gPad.RupOn=0;
    gPad.RdownOn=0;


    if (aaon) draw_debug_print(1, 15, "AntiAlias: On     [Rright]\n");
    else      draw_debug_print(1, 15, "AntiAlias: Off    [Rright]\n");
    draw_debug_print(1, 17, "Depth: %s    [Rup/Rdown]\n", strdepth);
}

/* setting the depth of display buffer */
/* to show how to save the GS memory on this method */
/* Since this AntiAlias method spend bigger drawing buffer than usual, */
/* GS memory allocation tends to be tight. */
/* To reduce using memory size, change color depth and/or Z buffer depth of the drawing buffer */
/* to see how the result picture is going to change on the CRT */
static void set_buffer_depth_draw(int psmctmode)
{
    switch (psmctmode){
    case F32Z24:
	sceHiGsCtxSetDepth(gsctx_draw_aaon, SCE_GS_PSMCT32, SCE_GS_PSMCT24, 1);
	sceHiGsCtxSetDepth(gsctx_draw_aaoff, SCE_GS_PSMCT32, SCE_GS_PSMCT24, 1);
	sceHiGsEnvSetRegDthe(sceHiGsStdEnv, 0); /* dither off */
	zmax=(1L<<24)-1.0f;
	strcpy(strdepth, "F:32, Z:24");
	break;
    case F32Z16:
	sceHiGsCtxSetDepth(gsctx_draw_aaon, SCE_GS_PSMCT32, SCE_GS_PSMCT16S, 1);
	sceHiGsCtxSetDepth(gsctx_draw_aaoff, SCE_GS_PSMCT32, SCE_GS_PSMCT16S, 1);
	sceHiGsEnvSetRegDthe(sceHiGsStdEnv, 0); /* dither off */
	zmax=(1L<<16)-1.0f;
	strcpy(strdepth, "F:32, Z:16");
	break;
    case F16Z24:
	sceHiGsCtxSetDepth(gsctx_draw_aaon, SCE_GS_PSMCT16S, SCE_GS_PSMCT24, 1);
	sceHiGsCtxSetDepth(gsctx_draw_aaoff, SCE_GS_PSMCT16S, SCE_GS_PSMCT24, 1);
	sceHiGsEnvSetRegDthe(sceHiGsStdEnv, 1); /* dither on */
	zmax=(1L<<24)-1.0f;
	strcpy(strdepth, "F:16, Z:24");
	break;
    case F16Z16:
	sceHiGsCtxSetDepth(gsctx_draw_aaon, SCE_GS_PSMCT16, SCE_GS_PSMCT16, 1);
	sceHiGsCtxSetDepth(gsctx_draw_aaoff, SCE_GS_PSMCT16, SCE_GS_PSMCT16, 1);
	sceHiGsEnvSetRegDthe(sceHiGsStdEnv, 1); /* dither on */
	zmax=(1L<<16)-1.0f;
	strcpy(strdepth, "F:16, Z:16");
	break;
    default:
	zmax=0.0f;
	error_handling(__FILE__,__LINE__,"invalid mode");
	break;
    }
    sceHiGsCtxUpdate(gsctx_draw_aaon);
    sceHiGsCtxUpdate(gsctx_draw_aaoff);
    sceHiGsEnvUpdate(sceHiGsStdEnv);

    if (aaon){
	gsctx_draw=gsctx_draw_aaon;
    } else{
	gsctx_draw=gsctx_draw_aaoff;
    }
    sceHiGsCtxSetDefault(gsctx_draw);

}

/* change the setting of copy-packets, depending on the depth of the drawing buffer */
static void set_buffer_depth_disp(void)
{

    set_aa_packet(&packoff, gsctx_draw_aaoff, 2048-(_dispw/2), 2048-(_disph/2), _dispw, _disph, _dispw, _disph,   -8.0f, -8.0f);
    set_aa_packet(&packon,  gsctx_draw_aaon,  2048-(_dispw/2), 2048-(_disph/2), _dispw, _disph, _dispw, _disph*2, 0.0f, 0.0f);

    if (aaon){
	gsctx_disp=gsctx_disp_aaon;
	packp= &packon;
    } else {
	gsctx_disp=gsctx_disp_aaoff;
	packp= &packoff;
    }
}

/* change the setting of screen */
static void myscreen(sceHiPlugMicroData_t *md)
{
    /* zmax, it should be changed with depth of Z buffer */
    /* r, it should be changed with vertical drawing ratio */
    /* when anti-alias is on, r=2.0f because vertical scale is at double */
    static sceVu0FVECTOR s = {640.0f, 224.0f, 512.0f, 300.0f};
    static sceVu0FVECTOR w = {1.0f, 0.47f, 2048.0f, 2048.0f};
    static sceVu0FVECTOR d = {5.0f, 16777000.0f, 1.0f, 65536.0f};

    w[1] = aaon ? 0.47f*2.0f : 0.47f;
    d[1] = zmax;
    perspective(md, s,w,d);
}

/* initialize model:  read file, initialize, set to m  */
static int file_init(char *file, MODEL *m)
{
    m->data = file_read(file);
    if (m->data==NULL){
	error_handling(__FILE__,__LINE__,file);
	return -1;
    }

    if (sceHiParseHeader((u_int *)m->data) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    
    if (sceHiGetPlug((u_int *)m->data, "Frame", &m->frameP) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (sceHiGetPlug((u_int *)m->data, "Tex2d", &m->tex2dP) == SCE_HIG_NO_ERR)
	m->tex2dP->args = (u_int)&tex2d_arg;

    if (sceHiGetPlug((u_int *)m->data, "Micro", &m->microP) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    m->microP->args = (u_int)&micro_arg;

    if (sceHiCallPlug(m->frameP, SCE_HIG_INIT_PROCESS) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if((m->microD = sceHiPlugMicroGetData(m->microP)) == NULL)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (sceHiGetPlug((u_int *)m->data, "Hrchy", &m->hrchyP) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    m->micro_pre_arg.micro = 0;

    return 0;
}

/* finish model data m */
static void file_exit(MODEL *m)
{

    if (sceHiCallPlug(m->frameP, SCE_HIG_END_PROCESS) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    free(m->data);

}

/* packet to copy the draw buffer to the display buffer */
/* the FASTER way is using many sprites divided by size to be in texutre page buffer, */
/* while this sample is using one big sprite to understand the antialias method easily */
static void set_aa_packet(PACK *p, sceHiGsCtx *gsctx, int x, int y, int dw, int dh, int tw, int th, int dx, int dy)
{
    int cnt;
    int offsetx, offsety;

    offsetx= 0;
    offsety=0;

    p->giftag.nloop= ((int)&p->giftag2-(int)&p->giftag)/sizeof(u_long128) - 1;
    p->giftag.eop = 0;
    p->giftag.pre = 0;
    p->giftag.prim = 0;
    p->giftag.flg = SCE_GIF_PACKED;
    p->giftag.nreg = 1;
    p->giftag.regs = SCE_GIF_PACKED_AD;
	    
    p->texflush=0;
    p->texflush_addr=SCE_GS_TEXFLUSH;

    sceHiGsCtxGetTex0(gsctx, (u_long *)&p->tex0, gsctx->swap, 0, SCE_GS_DECAL);

    p->tex0_addr=SCE_GS_TEX0_1;
	    
	
    p->prim.PRIM=SCE_GS_PRIM_SPRITE;
    p->prim.IIP=1;
    p->prim.TME=1;
    p->prim.FGE=0;
    p->prim.ABE=1;
    p->prim.AA1=0;
    p->prim.FST=1;
    p->prim.CTXT=0;
    p->prim.FIX=0;
    p->prim_addr=SCE_GS_PRIM;

    p->giftag2.eop = 1;
    p->giftag2.pre = 1;
    p->giftag2.prim = SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 1, 1, 0, 1, 0, 1, 0, 0);

    p->giftag2.flg = SCE_GIF_REGLIST;
    p->giftag2.nreg = 2;
    p->giftag2.regs = (SCE_GS_UV << 0) | (SCE_GS_XYZ2<<4);
	    
    p->giftag2.nloop= 2;

    cnt=0;

    p->box[cnt].uv.U=offsetx;
    p->box[cnt].uv.V=offsety;

    p->box[cnt].xyz.X=(x)*16 + dx;
    p->box[cnt].xyz.Y=(y)*16 + dy;
    p->box[cnt].xyz.Z=0xffffffff;
    cnt++;

    p->box[cnt].uv.U=tw*16+offsetx;
    p->box[cnt].uv.V=th*16+offsety;

    p->box[cnt].xyz.X=(x+dw)*16 + dx;
    p->box[cnt].xyz.Y=(y+dh)*16 + dy;
    p->box[cnt].xyz.Z=0xffffffff;
    cnt++;

    SyncDCache(p, (u_char *)p+sizeof(*p)-1);
}
