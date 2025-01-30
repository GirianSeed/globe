/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/* 
 *        Emotion Engine Library Sample Program
 *
 *                      - libgp: gp tutorial -
 *
 *                         Version 1.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        	main.c 
 *		   global setting / launch sub routines
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            May,15,2002      aoki
 */
#include <stdio.h>
#include <stdlib.h>
#include <libdma.h>
#include <libpad.h>
#include <libgraph.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <malloc.h>
#include <libgp.h>
#include <libhig.h>
#include <libvu0.h>
#include <math.h>
#include <libdev.h>
#include "defines.h"
#include "oneprim.h"
#include "prims.h"

/* drawing mode */
typedef struct {
    int (*init_func)();
    int (*draw_func)(u_int, u_int);
    int (*exit_func)();
} MODE_LIST;

MODE_LIST mode_list[]={
    { oneprim_init, oneprim_draw, oneprim_exit}, /* in oneprim.c  */
    { prims_init, prims_draw, prims_exit},	/* in prims.c  */
};
#define mode_num (sizeof(mode_list)/sizeof(MODE_LIST))


/* work area for controller input */
static u_long128 pad_dma_buf[scePadDmaBufferMax]  __attribute__((aligned (64)));

/* initialize sif and controller port */
void pad_init()
{
    sceSifInitRpc(0);
    while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/sio2man.irx",
			0, NULL) < 0){
        printf("Can't load module sio2man\n");
    }
    while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/padman.irx",
			0, NULL) < 0){
        printf("Can't load module padman\n");
    }

    scePadInit(0);
    scePadPortOpen(0, 0, pad_dma_buf);

}

/* read controller input */
u_int pad_read()	
{
    u_char rdata[32];
    u_int paddata;

    if(scePadRead(0, 0, rdata) > 0){
	paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);
    }else{
	paddata = 0;
    }

    return paddata;
}

/* set GS double buffer */
static sceGsDBuff db;
void db_init()
{
    sceGsSetDefDBuff(&db, SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT,
		     SCE_GS_ZGREATER, SCE_GS_PSMZ24, SCE_GS_CLEAR);

    /* set background clear color */
    db.clear0.rgbaq.R= db.clear0.rgbaq.G= db.clear0.rgbaq.B=0x40;
    db.clear0.rgbaq.A=0;
    db.clear0.rgbaq.Q=1.0f;
    db.clear1.rgbaq= db.clear0.rgbaq;
}

/* swap GS draw/display double buffer */
void db_swap(int odev)
{
    static u_int fr=0;
    sceGsDrawEnv1 *de;

    de=(fr&1) ? &db.draw1 : &db.draw0;		/* toggle buffer to use next  */

    sceGsSetHalfOffset(de, 2048,2048, 1-odev);	/* to handle interlace properly */
    SyncDCache(de, ((char *)de)+sizeof(sceGsDrawEnv1)-1); /* flush cache *de area */
    sceGsSwapDBuff(&db, fr++);			/* toggle draw/display buffer */
}


int main( void )
{
    u_int paddata, padtrig, oldpaddata;		/* for controller input handling */
    int mode=0;					/* drawing mode  */
    int odev;					/* interlace odd/even */


    /* initialize sif and controller port */
    pad_init();
    paddata=0;

    /* set double buffer */
    db_init();


    /* drawing mode */    
    mode=0;					
    mode_list[mode].init_func();		/* initialize the first drawing mode */

    
    printf("SELECT button to change drawing mode\n");

    while(1){
	/* wait V-blank */
        odev = sceGsSyncV(0);

	/* swap GS draw/display double buffer */
	db_swap(odev);

	/* get controller input */
	oldpaddata=paddata;
	paddata = pad_read();
	padtrig=paddata & ~oldpaddata;

	if (padtrig&SCE_PADselect){	/* move to the next drawing mode */
	    mode_list[mode].exit_func();	/* exit current drawing mode */

	    mode++;
	    if (mode>=mode_num) mode=0;		/* next mode number */

	    mode_list[mode].init_func();	/* initialize new drawing mode */
	}

	/* draw */
	mode_list[mode].draw_func(paddata, padtrig);
    }

    /* never reach here, actually */

    /* exit current drawing mode */
    mode_list[mode].exit_func();

    return 0;
}
