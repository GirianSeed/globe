/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *        Emotion Engine Library Sample Program
 *
 *                         - libgp: multiwin -
 *
 *                         Version 1.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            main.c
 *		libgp + GS Service (to change draw environment)
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

#define PI	3.14159265f

/* memory for HiG */
enum {
	HIG_HEAP_SIZE	=	2 * 1024 * 1024,	/* 2 mega byte (too much for this sample)*/
	HIDMA_BUF_SIZE	=	1 * 1024 * 1204,	/* 1 mega byte (too much, again ) */
};

/* this sample works on either SCE_GP_PATH3 or SCE_GP_PATH2 */
#define LIBGP_TRANSFER_MODE SCE_GP_PATH3

/* num of window */
#define WINDOW_NUM 4

#define CTXT 0

/* sceGpRef packet num to handle GS services: */
/* 2 for each gsctx registration with clear, 1 for each gsctx-registration without clear*/
#define REFPACKET_NUM (WINDOW_NUM*2)


/* main DMA chain OT resolution */
#define RESOLUTION (REFPACKET_NUM+1)
/* sub DMA chain (for each window) OT resolution */
#define SUBCHAIN_RESOLUTION 1

/* background colors for each window */
static u_char bgcolor[WINDOW_NUM][3]={
    {0, 0, 128},
    {128, 128, 0},
    {0, 128, 128},
    {0, 128, 0}
};

/* sub-window outside margin */
#define WIN_FRAME_W 10
#define WIN_FRAME_H 5
/* sub-window size */
#define WINDOW_W (640/2-WIN_FRAME_W)
#define WINDOW_H (224/2-WIN_FRAME_H)


static struct _window{
    sceHiGsCtx	*gsctx;
    int		x, y, w, h;
    u_char	r, g, b;
} window[WINDOW_NUM];

/* main DMA chain */
static sceGpChain chain;

/* sub DMA chains */
#define SUBCHAIN_NUM 2
static sceGpChain subchain[SUBCHAIN_NUM];

/* call packet to add subchain to the main chain with "call" method */
static sceGpCall call[WINDOW_NUM-1];

/* object : just a triangle */
static sceGpPrimP *object[SUBCHAIN_NUM];
/* ref packet to handle GS services */
static sceGpRef refs[REFPACKET_NUM];

/* point to add packet */
static sceDmaTag *pAdd;

/* current ref packet number */
static int refnum=0;

/*---------- local functions ----------*/
static void init_sample(void);
static void window_init();
static void window_exit();
static void window_main();
static void window_pos_init();
static void window_draw();
static void window_ctrl(struct _window *m);

	  u_long128 prcenarea[3];
	  sceGpAd *pmcen;

int main()
{
    int		odev;
    u_int	frame = 0;
    sceHiErr	err;


    pmcen=(sceGpAd*)prcenarea;
    sceGpInitAd(pmcen, 1);
    sceGpSetAd(pmcen, 0, SCE_GS_PRMODECONT, 0);



    init_sample();




    window_init();


    while (1) {
	++frame;
	*T0_COUNT = 0;			/* reset Timer */
	
	pad_ctrl();
	

	window_main();
	

	window_draw();

	sceGsSyncPath(0, 0);

	
	/* switch GS Double Buffer */
	odev = !sceGsSyncV(0);
	err = sceHiGsDisplaySwap(odev);	
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }


    window_exit();
    return 0;
}

void init_sample(void)
{
    sceHiErr		err;
    
    sceSifInitRpc(0);
    while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/sio2man.irx",
			    0,
			    NULL) < 0)
	printf("Can't load module sio2man\n");
    while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/padman.irx",
			    0,
			    NULL) < 0)
	printf("Can't load module padman\n");

/***********
 * Initialize sequence of HiG
 ***********/
    err = sceHiMemInit(memalign(64, HIG_HEAP_SIZE), HIG_HEAP_SIZE);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiDMAInit(NULL, NULL, HIDMA_BUF_SIZE);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    sceGsResetPath();

    err = sceHiGsMemInit(SCE_HIGS_MEM_TOP, SCE_HIGS_MEM_END);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsDisplayMode(SCE_HIGS_RESET|SCE_HIGS_NTSCI|SCE_HIGS_RGBA|SCE_HIGS_DEPTH24);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsDisplaySize(640,224);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
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
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


    err = sceHiGsEnvUpdate(sceHiGsStdEnv);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsEnvRegist(sceHiGsStdEnv);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    sceHiDMASend();
    sceHiDMAWait();

    pad_init();

}

/* handle gs service registration: sceHiGsServiceSetRegistFunc(add_to_gppacket) */

int add_to_gppacket(void *addr, int size)
{
    sceGpSetRef(&refs[refnum], addr, size, SCE_GP_PATH2);
    pAdd=sceGpAddPacket2(pAdd, &refs[refnum]);
    refnum++;

    return 0;
}

static void window_dmachain_init()
{
    u_long128 *ot;
    int size;
    int i;

    /* init the main chain */
    size=sceGpChkChainOtSize(RESOLUTION);
    ot=memalign(16, size*sizeof(u_long128));
    if (ot==NULL) error_handling( __FILE__, __LINE__, "malloc failed\n");

    sceGpInitChain(&chain, ot, RESOLUTION);

    /* init subchains */
    size=sceGpChkChainOtSize(SUBCHAIN_RESOLUTION);
    for (i=0; i<SUBCHAIN_NUM; i++){
	ot=memalign(16, size*sizeof(u_long128));
	if (ot==NULL) error_handling( __FILE__, __LINE__, "malloc failed\n");
	sceGpInitChain(&subchain[i], ot, SUBCHAIN_RESOLUTION);

    }

}

#define TRIRAD 50
#define TRIRAD2 (TRIRAD/2)
#define TRIRAD3 43

static void window_object_init()
{
    int size;
    int i,j,k;
    int index;

    static u_int trivtx[2][4][4]={
	{
	    {2048*16, (2048-TRIRAD)*16, 0xffff, 0},
	    {(2048-TRIRAD3)*16, (2048+TRIRAD2)*16, 0xffff, 0},
	    {(2048+TRIRAD3)*16, (2048+TRIRAD2)*16, 0xffff, 0},
	},{
	    {2048*16, (2048-TRIRAD2)*16, 0xffff, 0},
	    {(2048-TRIRAD)*16, (2048+TRIRAD2)*16, 0xffff, 0},
	    {(2048+TRIRAD)*16, (2048+TRIRAD2)*16, 0xffff, 0},
	    {2048*16, (2048-TRIRAD2)*16, 0xffff, 0},
	}
    };
    static u_int type[2]= { 
	SCE_GP_PRIM_P | SCE_GP_TRI_FM, 
	SCE_GP_PRIM_P | SCE_GP_LINESTRIP_FM 
    };
    static int pnum[2]={1, 3};

    sceGpSetDefaultCtxt(CTXT);

    /* init triangle packets */
    for (i=0; i<SUBCHAIN_NUM; i++){
	size=sceGpChkPacketSize(type[i], pnum[i]);
	object[i]=memalign(64, size*sizeof(u_long128));
	if (object[i]==NULL) error_handling( __FILE__, __LINE__, "malloc failed\n");
    
	sceGpInitPrimP(object[i], type[i], pnum[i]);

	for (j=0; j<sceGpChkNumPtoV(type[i], pnum[i]); j++){
	    index=sceGpIndexXyzf(type[i], j);
	    for (k=0; k<4; k++) object[i]->reg[index].ui[k]= trivtx[i][j][k];
	}

	/* add to sub-chain */
	sceGpAddPacket(&subchain[i], 0, object[i]);

    }

    /* set color */
    sceGpSetRgbaFM(object[0], 0xff, 0x0, 0x0, 0x80);
    sceGpSetRgbaFM(object[1], 0x0, 0xff, 0xff, 0x80);
}

static void window_init()
{
    int i;

    window_dmachain_init();

    for (i=0; i<REFPACKET_NUM; i++){
	sceGpInitRef(&refs[i]);
    }

    window_object_init();

    window_pos_init();


}

static void window_reset()
{
    refnum=0;
    sceGpResetChain(&chain);

}

/* draw window */
static void window_draw()
{
    int i;
    static int position=3;
    int num;

    if (gPad.RrightSwitch|gPad.RdownSwitch) position=0;
    if (gPad.RleftSwitch|gPad.RupSwitch) position=3;


    /* add subchain to the main chain */
       num=0;
    for (i=0; i<WINDOW_NUM; i++){
	if (i==position){
	    /* "add" */
	    sceGpAddChain(&chain, i*2+1, &subchain[1]);
	} else{
	    /* with "call": to enable to regist the same subchain more than once */
	    sceGpCallChain(&chain, i*2+1, &subchain[0], &call[num++]);
	}
    }

    FlushCache(0);
    while ( sceGpKickChain(&chain, LIBGP_TRANSFER_MODE) <0){
	for (i=0; i<1000; i++) ;
    }
    sceGsSyncPath(0, 0);

	
}


static void window_pos_init()
{
    int i;

    /* set window position */
    for (i=0; i<WINDOW_NUM; i++){
	window[i].x=WINDOW_W*(i%2)+WIN_FRAME_W;
	window[i].y=WINDOW_H*(i/2)+WIN_FRAME_H;
	window[i].w=WINDOW_W;
	window[i].h=WINDOW_H;
    }

    /* set background color */
    for (i=0; i<WINDOW_NUM; i++){
	window[i].r=bgcolor[i][0];
	window[i].g=bgcolor[i][1];
	window[i].b=bgcolor[i][2];
    }

    /* initialize contexts for sub-windows */
    for (i=0; i<WINDOW_NUM; i++){
	window[i].gsctx=sceHiGsCtxCreate(1);
	sceHiGsCtxCopy(window[i].gsctx, sceHiGsStdCtx);
	sceHiGsCtxSetRect(window[i].gsctx, window[i].x, window[i].y, window[i].w, window[i].h, 
			  SCE_HIGS_FBW_KEEP);
	sceHiGsCtxSetClearColor(window[i].gsctx, window[i].r, window[i].g, window[i].b, 128);
	
	window[i].gsctx->ctxt=CTXT;
	sceHiGsCtxUpdate(window[i].gsctx);
    }


}

static void window_main(void)
{
    int j;
    struct _window *w;

    window_ctrl(&window[3]);  /* move #3 sub-window */

    /* reset DMA chains */
    window_reset();
    /* change GS service regist-func to libgp ref packet */
    sceHiGsServiceSetRegistFunc(add_to_gppacket);

    for (j=0; j<WINDOW_NUM; j++){ /* each window */
	w= &window[j];
	pAdd=sceGpGetTailChain(&chain, j*2);
	/* registrate sub-window's context */
	if (j==2) sceHiGsCtxRegist(w->gsctx, 0); /*  Draw without BG-clear for #2 */
	else sceHiGsCtxRegist(w->gsctx, 1); /* w/BG-clear for other sub-windows  */

    }

}

static void window_exit(void)
{
    int i;

    for (i=0; i<WINDOW_NUM; i++){
	if (sceHiGsCtxDelete(window[i].gsctx)!=SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

}


static void window_ctrl(struct _window *m)
{
    if (gPad.LdownOn){
	m->y+=2;
	if (m->y+m->h>224) m->y=224-m->h;
	gPad.LdownOn=0;
    } else if (gPad.LupOn){
	m->y-=2;
	if (m->y<0) m->y=0;
	gPad.LupOn=0;
    }

    if (gPad.LrightOn){
	m->x+=4;
	if (m->x+m->w>640) m->x=640-m->w;
	gPad.LrightOn=0;
    } else if (gPad.LleftOn){
	m->x-=4;
	if (m->x<0) m->x=0;
	gPad.LleftOn=0;
    }

    sceHiGsCtxSetRect(m->gsctx, m->x, m->y, m->w, m->h, SCE_HIGS_FBW_KEEP);
    sceHiGsCtxUpdate(m->gsctx);

}
