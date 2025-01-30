/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *        Emotion Engine Library Sample Program
 *
 *                         - libgp: withhig -
 *
 *                         Version 1.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            main.c
 *			    libgp + HiG
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
 *	defines/variables for libgp
 ***********************************************/
#define WINDOW_TEX_WIDTH 128
#define WINDOW_TEX_HEIGHT 64

#define WINDOW_WIDTH 256
#define WINDOW_HEIGHT 64

#define WINDOW_POS_X (-WINDOW_WIDTH/2)
#define WINDOW_POS_Y ((224/2)-WINDOW_HEIGHT)

#define WINDOW_PSM SCE_GS_PSMCT32

/* sub-window bitmap image file */
#define WINDOW_IMAGE_FILE "../data/subwin.raw"


#define CTXT 0
#define RESOLUTION 1


static sceGpPrimR *psprite;
static sceGpTexEnv texenv;
static sceGpLoadImage loadimage;
static sceGpAlphaEnv alphaenv;

static sceGpChain chain;

static sceHiGsMemTbl	*subwin_gstbl = NULL;
#define ZBUF_MAX ((1L<<24)-1)

/***********************************************
 *	defines/variables for libhig
 ***********************************************/
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

#define HIG_DATA_FILE "../data/dino.bin"
static void	*DATA_ADDR;
static sceHiPlug	*frameP;
static u_int		*microD;
static sceHiPlug	*microP;
static sceHiPlug	*tex2dP;

static sceHiPlugTex2dInitArg_t	tex2d_arg;
static sceHiPlugMicroPreCalcArg_t	micro_arg;

/*---------- local functions ----------*/
extern int main();
extern void sif_init(void);
static void subwin_alpha_init();
static void subwin_dmachain_init();
static void subwin_texture_init();
static void subwin_primitive_init();
static void subwin_init();
static void subwin_main();
static void subwin_draw();
static void subwin_exit(void);
static void hig_init(void);
static void hig_main(void);
static void hig_exit(void);

int main()
{
    int		odev;
    u_int	frame = 0;
    sceHiErr	err;

    sif_init();

    pad_init();

    /* initilalize hig part */
    hig_init();

    /* initilalize gp part */
    subwin_init();


    camera_init();
    light_init();


    /* debug print initialize */
    draw_debug_print_init();


    while (1) {
	++frame;
	
	pad_ctrl();

	/* hig part */
	hig_main();

	/* send hig part */
	sceHiDMASend();

	/* update gp packets during hig-transfer */
	subwin_main();

	/* wait hig-transfer done */
	sceHiDMAWait();

	/* send gp part */
	subwin_draw();


	draw_debug_print(1, 1, "FRAME   : %8u", frame);
	draw_debug_print(50, 1, "left cursor : rotate");
	draw_debug_print(50, 2, "right cursor: move");

	/* wait gp-transfer done */
	sceGsSyncPath(0, 0);

	/* send devfont */
	draw_debug_print_exec();
	
	/* switch GS Double Buffer */
	odev = !sceGsSyncV(0);
	err = sceHiGsDisplaySwap(odev);	
	if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    subwin_exit();
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


/***********************************************
 *	functions for gp
 ***********************************************/
static void subwin_alpha_init()
{

    sceGpInitAlphaEnv(&alphaenv, CTXT);
    sceGpSetAlphaEnv(&alphaenv, SCE_GP_ALPHA_INTER_AS, 128); /* (CS-CD)*AS+CD */
    
}

static void subwin_dmachain_init()
{
    u_long128 *ot;
    int size;

    size=sceGpChkChainOtSize(RESOLUTION);
    ot=memalign(16, size*sizeof(u_long128));
    if (ot==NULL) error_handling( __FILE__, __LINE__, "malloc failed\n");

    sceGpInitChain(&chain, ot, RESOLUTION);
}

static void subwin_texture_init()
{
   sceGpTextureArg texarg;
   u_int *addr;

    /* read a bitmap image for sub-window */
    addr = file_read(WINDOW_IMAGE_FILE);

    /* get a framebuffer area for subwindow */
    subwin_gstbl = sceHiGsMemAlloc(SCE_HIGS_BLOCK_ALIGN, WINDOW_TEX_WIDTH*WINDOW_TEX_HEIGHT);

    texarg.tbp = subwin_gstbl->addr/64;
    texarg.tx = texarg.ty = 0;
    texarg.tbw = (WINDOW_TEX_WIDTH+63)/64;
    texarg.tpsm = WINDOW_PSM;
    texarg.tw = WINDOW_TEX_WIDTH;
    texarg.th = WINDOW_TEX_HEIGHT;

    sceGpInitPacket(&texenv, SCE_GP_TEXENV, CTXT);
    sceGpSetTexEnv(&texenv, &texarg, SCE_GS_MODULATE, SCE_GS_LINEAR);

    sceGpInitPacket(&loadimage, SCE_GP_LOADIMAGE, 0);
    sceGpSetLoadImage(&loadimage, &texarg, addr, 0);
}

static void subwin_primitive_init()
{
    int index;
    u_int type= SCE_GP_PRIM_R | SCE_GP_SPRITE_FTU;

    psprite= memalign(16, sizeof(u_long128) * sceGpChkPacketSize(type, 1));
    if (psprite==NULL) error_handling( __FILE__, __LINE__, "malloc failed\n");

    sceGpInitPacket(psprite, type, 1);

    index=sceGpIndexRgba(type, 0);
    sceGpSetRgba(psprite, index, 0x80, 0x80, 0x80, 0x40);


    index=sceGpIndexUv(type, 0);
    sceGpSetUv(psprite, index, 0, 0);
    index=sceGpIndexUv(type, 1);
    sceGpSetUv(psprite, index, WINDOW_TEX_WIDTH*16, WINDOW_TEX_HEIGHT*16);

    index=sceGpIndexXyzf(type, 0);
    sceGpSetXyzf(psprite, index, 
		 (2048+WINDOW_POS_X)*16, (2048+WINDOW_POS_Y)*16, ZBUF_MAX-1, 0);
    index=sceGpIndexXyzf(type, 1);
    sceGpSetXyzf(psprite, index, 
		 (2048+WINDOW_POS_X+WINDOW_WIDTH)*16, (2048+WINDOW_POS_Y+WINDOW_HEIGHT)*16,
		 ZBUF_MAX-1, 0);

}

static void subwin_init()
{
    /* default alpha-blending mode : ON */
    sceGpSetDefaultAbe(1);

    subwin_dmachain_init();
    subwin_texture_init();
    subwin_primitive_init();
    subwin_alpha_init();

    /* packet&chain structure in this sample is permanent */
    sceGpAddPacket(&chain,  0, psprite);
    sceGpAddPacket(&chain, 0, &texenv);
    sceGpAddPacket(&chain, 0, &loadimage);
    sceGpAddPacket(&chain, 0, &alphaenv);

}


static void subwin_main()
{
    int dx=1*16;
    int dy=dx/2;

    /* move window position */
    if (gPad.LleftOn){
	if (psprite->reg[sceGpIndexXyzfSpriteFTU(0)].xyzf.X-dx>(2048-320)*16){
	    psprite->reg[sceGpIndexXyzfSpriteFTU(0)].xyzf.X-=dx;
	    psprite->reg[sceGpIndexXyzfSpriteFTU(1)].xyzf.X-=dx;
	}
    }
    if (gPad.LrightOn){
	if (psprite->reg[sceGpIndexXyzfSpriteFTU(1)].xyzf.X+dx<(2048+320)*16){
	    psprite->reg[sceGpIndexXyzfSpriteFTU(0)].xyzf.X+=dx;
	    psprite->reg[sceGpIndexXyzfSpriteFTU(1)].xyzf.X+=dx;
	}
    }
    if (gPad.LupOn) {
	if (psprite->reg[sceGpIndexXyzfSpriteFTU(0)].xyzf.Y-dy>(2048-112)*16){
	    psprite->reg[sceGpIndexXyzfSpriteFTU(0)].xyzf.Y-=dy;
	    psprite->reg[sceGpIndexXyzfSpriteFTU(1)].xyzf.Y-=dy;
	}
    }
    if (gPad.LdownOn) {
	if (psprite->reg[sceGpIndexXyzfSpriteFTU(1)].xyzf.Y+dy<(2048+112)*16){
	    psprite->reg[sceGpIndexXyzfSpriteFTU(0)].xyzf.Y+=dy;
	    psprite->reg[sceGpIndexXyzfSpriteFTU(1)].xyzf.Y+=dy;
	}
    }
    SyncDCache(&psprite->reg[0], &psprite->reg[sceGpIndexXyzfSpriteFTU(1)]);    

    /* packet-chain linkage has already made up at subwin_init() */

}

static void subwin_draw()
{
    sceGpKickChain(&chain, SCE_GP_PATH3);
}

/***********************************************
 *	functions for HiG
 ***********************************************/
void hig_init(void)
{
    sceHiErr	err;
    sceHiType	type;

    /* set up HiG env */
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


    /* HiG object handling */
    DATA_ADDR = file_read(HIG_DATA_FILE);

    if ((err = sceHiParseHeader((u_int *)DATA_ADDR)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Frame", &frameP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Tex2d", &tex2dP)) == SCE_HIG_NO_ERR) {
	tex2d_arg.resident = TRUE;
	tex2d_arg.tbl = NULL;
	tex2dP->args = (u_int)&tex2d_arg;
    } else
	tex2dP = NULL;

    if ((err = sceHiCallPlug(frameP, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* for camera */
    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Micro", &microP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    type.repository = SCE_HIP_COMMON;
    type.project = SCE_HIP_FRAMEWORK;
    type.category = SCE_HIP_MICRO;
    type.status = SCE_HIG_DATA_STATUS;
    type.id = SCE_HIP_MICRO_DATA;
    type.revision = SCE_HIP_REVISION;
    if ((err = sceHiGetData(microP, &microD, type)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* to send #0 microcode */
    micro_arg.micro = 0;      
}

static void hig_main(void)
{
    sceHiErr	err;

    /* camear moving/set */
    camera_ctrl();
    camera_set((u_int *)microD);
    light_set((u_int *)microD);

    /* set micro arg (to control microcode transfer)*/
    microP->args = (u_int)&micro_arg;

    if ((err = sceHiCallPlug(frameP, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if ((err = sceHiCallPlug(frameP, SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* not to transfer microcode anymore */
    micro_arg.micro = -1;


}

static void hig_exit(void)
{
    sceHiErr	err;

    if ((err = sceHiCallPlug(frameP, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    free(DATA_ADDR);
    DATA_ADDR=NULL;
}

static void subwin_exit(void)
{
    free(chain.ot);
    chain.ot=NULL;

    free(psprite);
    psprite=NULL;

    sceHiGsMemFree(subwin_gstbl);
    subwin_gstbl=NULL;
}
