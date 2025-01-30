/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *        Emotion Engine Library Sample Program
 *
 *                         - libgp: filtering -
 *
 *                         Version 1.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            main.c
 *                     simple 2D filtering w/ libgp
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            Sep,20,2001      aoki
 */
#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <eeregs.h>
#include <malloc.h>
#include <libgraph.h>
#include <libvu0.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libhig.h>
#include <libhip.h>
#include <libgp.h>
#include "util.h"
#include "camera.h"


/***********************************************
 *	for filter
 ***********************************************/
#define WINDOW_WIDTH 180
#define WINDOW_HEIGHT 90

#define WINDOW_POS_X 0
#define WINDOW_POS_Y 0

#define CTXT 0
#define ZBUF_MAX ((1L<<24)-1)
#define RESOLUTION 1

static sceGpTexEnv texenv;
static sceGpPrimR *sprite;
static sceGpAd *ad;

static sceGpChain chain;


/***********************************************
 *	for HiG
 ***********************************************/
/* display data */
static const char *files[]={"../data/shuttle.bin", "../data/curtain.bin"};

/*---------- static constants ----------*/
static sceHiPlugTable ptable[] = {
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_MICRO, SCE_HIG_PLUGIN_STATUS, SCE_HIP_MICRO_PLUG, SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugMicro },
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_TEX2D, SCE_HIG_PLUGIN_STATUS, SCE_HIP_TEX2D_PLUG, SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugTex2D },
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_TIM2 , SCE_HIG_PLUGIN_STATUS, SCE_HIP_TIM2_PLUG , SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugTim2  },
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_SHAPE, SCE_HIG_PLUGIN_STATUS, SCE_HIP_SHAPE_PLUG, SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugShape },
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_HRCHY, SCE_HIG_PLUGIN_STATUS, SCE_HIP_HRCHY_PLUG, SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugHrchy },
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_ANIME, SCE_HIG_PLUGIN_STATUS, SCE_HIP_ANIME_PLUG, SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugAnime },
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_SHARE, SCE_HIG_PLUGIN_STATUS, SCE_HIP_SHARE_PLUG, SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugShare },
};
enum {
	HIG_HEAP_SIZE	=	8 * 1024 * 1024,	/* 5 mega byte */
	HIDMA_BUF_SIZE	=	3 * 1024 * 1204,	/* 3 mega byte */
	NTABLE		= 	sizeof(ptable)/sizeof(sceHiPlugTable)
};

static CameraPos win_camerapos = {
    { 0.0f, 0.0f, -10.0f, 0.0f },		// pos
    { 0.0f, 0.0f, 1.0f, 1.0f },			// zdir
    { 0.0f, 1.0f, 0.0f, 1.0f },			// ydir
    { 0.0f, 0.0f, 0.0f, 0.0f },			// rot
};

#define PI	3.14159265f

#define MODEL_NUM (sizeof(files)/sizeof(char *))

static struct _model{
    u_int	*data;
    sceHiPlug	*frameP;
    sceHiPlug	*microP;
    sceHiPlug	*hrchyP;
    sceHiPlug	*tex2dP;
    u_int	*microD;
} model[MODEL_NUM];

static sceHiPlugTex2dInitArg_t		tex2d_arg;
static sceHiPlugHrchyPreCalcArg_t	hrchy_arg;
static sceHiPlugMicroPreCalcArg_t	micro_arg;
static sceVu0FMATRIX root_mat2;

extern CameraPos camerapos;			/* in camera.c  */

/***********************************************
 *	local functions
 ***********************************************/
extern void sif_init(void);
extern void hig_init(void);
static void filter_dmachain_init();
static void filter_ctrl();
static void filter_texture_init();
static void filter_ad_init();
static void filter_primitive_init();
static void filter_init();
static void filter_main();
static void filter_draw();
static void filter_exit();
static void init_model(struct _model *m, const char *file);
static void exit_model(struct _model *m);
static void hig_main(void);
static void hig_exit(void);
static void _obj2_ctrl(void);



/***********************************************
 *	main function
 ***********************************************/
int main()
{
    int		odev;
    u_int	frame = 0;
    sceHiErr	err;


    sif_init();

    pad_init();

    /* HiG initilize */
    hig_init();

    camera_init();
    camerapos=win_camerapos;

    light_init();

    /* libgp inittialize */
    filter_init();

    /* debug print initialize */
    draw_debug_print_init();


    while (1) {
	++frame;
	

	pad_ctrl();
	
	hig_main();

	sceHiDMASend();

	filter_main();

	sceHiDMAWait();

	filter_draw();


	sceGsSyncPath(0, 0);

	draw_debug_print(1, 1, "FRAME   : %8u", frame);

	/* return to standard context (for draw_debug_print_exec) */
	sceHiGsCtxSend(sceHiGsStdCtx, 0);

	sceGsSyncPath(0, 0);
	draw_debug_print_exec();
	

	/* switch GS Double Buffer */
	odev = !sceGsSyncV(0);
	err = sceHiGsDisplaySwap(odev);	
	if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    filter_exit();
    hig_exit();
    
    return 0;
}

void sif_init(void)
{
    sceSifInitRpc(0);
    while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/sio2man.irx",
			    0,
			    NULL) < 0)
	printf("Can't load module sio2man\n");
    while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/padman.irx",
			    0,
			    NULL) < 0)
	printf("Can't load module padman\n");
}

void hig_init(void)
{
    sceHiErr	err;
    int i;

    err = sceHiMemInit(memalign(64, HIG_HEAP_SIZE), HIG_HEAP_SIZE);
    if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiDMAInit(NULL, NULL, HIDMA_BUF_SIZE);
    if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiRegistTable(ptable, NTABLE);
    if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    sceGsResetPath();

    err = sceHiGsMemInit(SCE_HIGS_MEM_TOP, SCE_HIGS_MEM_END);
    if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsDisplayMode(SCE_HIGS_RESET|SCE_HIGS_NTSCI|SCE_HIGS_RGBA|SCE_HIGS_DEPTH24);
    if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsDisplaySize(640,224);
    if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsClearColor(0, 0, 0, 0);
    err = sceHiGsClearDepth(0);
    err = sceHiGsClear(SCE_HIGS_CLEAR_ALL);
    err = sceHiGsFogcolRegs(25,25,25);
    err = sceHiGsAlphaRegs(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    err = sceHiGsTestRegs(SCE_GS_FALSE, SCE_GS_ALPHA_NEVER, 0, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GREATER);
    err = sceHiGsClampRegs(SCE_GS_CLAMP, SCE_GS_CLAMP, 0, 0, 0, 0);
    err = sceHiGsPrmodecontRegs(SCE_GS_TRUE);
    err = sceHiGsTexaRegs(128, 1, 128);
    err = sceHiGsTex1Regs(0, 0, SCE_GS_LINEAR, SCE_GS_LINEAR, 0, 0, 0);
    err = sceHiGsPrmodeRegs(SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GS_FALSE);

    /* update stdport packets */
    err = sceHiGsCtxUpdate(sceHiGsStdCtx);
    if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


    if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsEnvRegist(sceHiGsStdEnv);
    if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);



    tex2d_arg.resident = FALSE;
    tex2d_arg.tbl = sceHiGsMemAlloc(SCE_HIGS_PAGE_ALIGN, sceHiGsMemRestSize());

    micro_arg.micro = 0;
    hrchy_arg.root= &root_mat2;

    /* initialize model data */
    for (i=0; i<MODEL_NUM; i++){
	init_model(&model[i], files[i]);
    }

}




static void filter_dmachain_init()
{
    u_long128 *ot;
    int size;

    size=sceGpChkChainOtSize(RESOLUTION);
    ot=memalign(16, size*sizeof(u_long128));
    if (ot==NULL) error_handling( __FILE__, __LINE__, "malloc failed\n");

    sceGpInitChain(&chain, ot, RESOLUTION);

}

static void filter_ctrl()
{
#define VMAX 5
    static int vmsk=3;
    int umsk=vmsk+1;
    int dx=32;
    int dy=16;

    /* mesh size */
    if (gPad.R1Switch){
	vmsk=(vmsk+1)%VMAX;
	umsk=vmsk+1;
    }
    if (gPad.R2Switch){
	vmsk=(vmsk+VMAX-1)%VMAX;
	umsk=vmsk+1;
    }

    texenv.clamp.MINU = ~((1<<umsk)-1);
    texenv.clamp.MINV = ~((1<<vmsk)-1);


    /* position */
    if (gPad.LleftOn){
	if (sprite->reg[sceGpIndexUvSpriteFMTU(0)].uv.U>dx){
	    sprite->reg[sceGpIndexUvSpriteFMTU(0)].uv.U-=dx;
	    sprite->reg[sceGpIndexUvSpriteFMTU(1)].uv.U-=dx;

	    sprite->reg[sceGpIndexXyzfSpriteFMTU(0)].xyzf.X-=dx;
	    sprite->reg[sceGpIndexXyzfSpriteFMTU(1)].xyzf.X-=dx;
	}
    }
    if (gPad.LrightOn){
	if (sprite->reg[sceGpIndexUvSpriteFMTU(1)].uv.U+dx<640*16){
	    sprite->reg[sceGpIndexUvSpriteFMTU(0)].uv.U+=dx;
	    sprite->reg[sceGpIndexUvSpriteFMTU(1)].uv.U+=dx;

	    sprite->reg[sceGpIndexXyzfSpriteFMTU(0)].xyzf.X+=dx;
	    sprite->reg[sceGpIndexXyzfSpriteFMTU(1)].xyzf.X+=dx;
	}
    }
    if (gPad.LupOn) {
	if (sprite->reg[sceGpIndexUvSpriteFMTU(0)].uv.V>dy){
	    sprite->reg[sceGpIndexUvSpriteFMTU(0)].uv.V-=dy;
	    sprite->reg[sceGpIndexUvSpriteFMTU(1)].uv.V-=dy;

	    sprite->reg[sceGpIndexXyzfSpriteFMTU(0)].xyzf.Y-=dy;
	    sprite->reg[sceGpIndexXyzfSpriteFMTU(1)].xyzf.Y-=dy;
	}
    }
    if (gPad.LdownOn) {
	if (sprite->reg[sceGpIndexUvSpriteFMTU(1)].uv.V+dx<224*16){
	    sprite->reg[sceGpIndexUvSpriteFMTU(0)].uv.V+=dy;
	    sprite->reg[sceGpIndexUvSpriteFMTU(1)].uv.V+=dy;
	    
	    sprite->reg[sceGpIndexXyzfSpriteFMTU(0)].xyzf.Y+=dy;
	    sprite->reg[sceGpIndexXyzfSpriteFMTU(1)].xyzf.Y+=dy;
	}
    }
    SyncDCache(&sprite->reg[0], &sprite->reg[sceGpIndexXyzfSpriteFMTU(1)]);    
}

static void filter_texture_init()
{
    sceGpTextureArg texarg;
    int rect[4];


    sceGpInitPacket(&texenv, SCE_GP_TEXENV, CTXT);
    texarg.tbp=sceHiGsStdCtx->fbp[1];
    texarg.tbw=sceHiGsStdCtx->value.frame.FBW;

    sceHiGsCtxGetRect(sceHiGsStdCtx, rect);

    texarg.tx=rect[0];
    texarg.ty=rect[1];
    texarg.tw=rect[2];
    texarg.th=rect[3];

    sceGpSetTexEnv(&texenv, &texarg, SCE_GS_DECAL, SCE_GS_NEAREST);


    texenv.clamp.WMS = SCE_GS_REGION_REPEAT;
    texenv.clamp.WMT = SCE_GS_REGION_REPEAT;
		 
    texenv.clamp.MINU = 0xff0;
    texenv.clamp.MINV = 0xff8;
		 
    texenv.clamp.MAXU = 4;
    texenv.clamp.MAXV = 4;


    sceHiGsCtxGetTex0(sceHiGsStdCtx, (u_long *)&texenv.tex0, 0, 0, SCE_GS_DECAL);
}

static void filter_ad_init()
{
    u_int type= SCE_GP_AD;
    int pnum=1;

    ad= memalign(16, sizeof(u_long128) * sceGpChkPacketSize(type, pnum));
    if (ad==NULL) error_handling( __FILE__, __LINE__, "malloc failed\n");

    sceGpInitPacket(ad, type, pnum);
    sceGpSetAd(ad, 0, SCE_GS_TEXFLUSH, 0);
}

static void filter_primitive_init()
{
    int index;
    u_int type= SCE_GP_PRIM_R | SCE_GP_SPRITE_FMTU;
    int pnum=1;

    sprite= memalign(16, sizeof(u_long128) * sceGpChkPacketSize(type, pnum));
    if (sprite==NULL) error_handling( __FILE__, __LINE__, "malloc failed\n");

    sceGpInitPacket(sprite, type, pnum);

    index=sceGpIndexUv(type, 0);
    sceGpSetUv(sprite, index, 
	       (320+WINDOW_POS_X)*16, (112+WINDOW_POS_Y)*16);

    index=sceGpIndexUv(type, 1);
    sceGpSetUv(sprite, index,
	       (320+WINDOW_POS_X+WINDOW_WIDTH)*16, (112+WINDOW_POS_Y+WINDOW_HEIGHT)*16);


    index=sceGpIndexXyzf(type, 0);
    sceGpSetXyzf(sprite, index, 
		 (2048+WINDOW_POS_X)*16, (2048+WINDOW_POS_Y)*16, ZBUF_MAX-1, 0);
    index=sceGpIndexXyzf(type, 1);
    sceGpSetXyzf(sprite, index, 
		 (2048+WINDOW_POS_X+WINDOW_WIDTH)*16, (2048+WINDOW_POS_Y+WINDOW_HEIGHT)*16,
		 ZBUF_MAX-1, 0);

}

static void filter_init()
{
    sceGpSetDefaultAbe(0);

    filter_dmachain_init();
    filter_texture_init();
    filter_primitive_init();
    filter_ad_init();

    sceGpAddPacket(&chain,  0, sprite);

    sceGpAddPacket(&chain, 0, &texenv);

    sceGpAddPacket(&chain,  0, ad);

}


static void filter_main()
{
    texenv.tex0.TBP0=sceHiGsStdCtx->fbp[sceHiGsStdCtx->swap]*32;
    SyncDCache(&texenv.tex1, &texenv.clamp);

    filter_ctrl();


}

static void filter_draw()
{
    sceGpKickChain(&chain, SCE_GP_PATH3);
}

static void filter_exit()
{
    free(sprite);
    free(ad);
}


static void init_model(struct _model *m, const char *file)
{
    sceHiType type;

    m->data = file_read((void *)file);

    if (sceHiParseHeader(m->data) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (sceHiGetPlug(m->data, "Frame", &m->frameP) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (sceHiGetPlug(m->data, "Tex2d", &m->tex2dP) == SCE_HIG_NO_ERR)
	m->tex2dP->args = (u_int)&tex2d_arg;

    if (sceHiCallPlug(m->frameP, SCE_HIG_INIT_PROCESS) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* for camera */
    if (sceHiGetPlug(m->data, "Micro", &m->microP) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


    m->microP->args = (u_int)&micro_arg;

	/* get hrchy-plugin block from the data */
    if (sceHiGetPlug(m->data, "Hrchy", &m->hrchyP) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    type.repository = SCE_HIP_COMMON;
    type.project = SCE_HIP_FRAMEWORK;
    type.category = SCE_HIP_MICRO;
    type.status = SCE_HIG_DATA_STATUS;
    type.id = SCE_HIP_MICRO_DATA;
    type.revision = SCE_HIP_REVISION;

    if (sceHiGetData(m->microP, &m->microD, type) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


}

static void exit_model(struct _model *m)
{
    if (sceHiCallPlug(m->frameP, SCE_HIG_END_PROCESS) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    free(m->data);
}


static void hig_main(void)
{
    int i;
    struct _model *m;
	


    model[1].hrchyP->args = (u_int)&hrchy_arg;  /* control model[1] */
    _obj2_ctrl();


    for (i=0; i<MODEL_NUM; i++){
	m= &model[i];

	if (sceHiCallPlug(m->frameP, SCE_HIG_PRE_PROCESS) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    for (i=0; i<MODEL_NUM; i++){
	m= &model[i];

	/* camera/light setting */
	camera_set((u_int *)m->microD);
	light_set((u_int *)m->microD);

	/* registration of transmit */
	if (sceHiCallPlug(m->frameP, SCE_HIG_POST_PROCESS) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }


    micro_arg.micro = -1;
}


static void hig_exit(void)
{
    int i;

    for (i=0; i<MODEL_NUM; i++){
	exit_model(&model[i]);
    }

    sceHiGsMemFree(tex2d_arg.tbl);
}

static void _obj2_ctrl(void)
{
    static sceVu0FVECTOR root_trans2={7.5f, 5.0f, 20.0f, 0.0f};

    float  dt = 0.4f;

    if(gPad.RrightOn)	root_trans2[0] += dt;
    if(gPad.RleftOn)	root_trans2[0] -= dt;
    if(gPad.RdownOn)	root_trans2[1] += dt;
    if(gPad.RupOn)	root_trans2[1] -= dt;

    sceVu0UnitMatrix(root_mat2);
    sceVu0TransMatrix(root_mat2, root_mat2, root_trans2);
}
