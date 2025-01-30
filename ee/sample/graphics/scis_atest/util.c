/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: util.c,v 1.1 2002/10/18 12:26:13 aoki Exp $
 */
/* 
 * Emotion Engine Library Sample Program
 *
 * Copyright (C) 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * scis_at - util.c
 *	controller, on-screen print functions
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2002-10-08      aoki        
 */

#include <eekernel.h>
#include <eeregs.h>
#include <stdio.h>
#include <sifdev.h>
#include <malloc.h>
#include <stdarg.h>
#include <libpad.h>
#include <libdev.h>
#include <libgp.h>
#include "util.h"

typedef struct {
    union {
	sceGifTag st;
	u_long ul;
    } giftag;
    union {
	sceGsAlpha st;
	u_long ul;
    } alpha;
    u_long alpha_addr;
} AS __attribute__((aligned (16)));

static u_long128 pad_dma_buf[scePadDmaBufferMax]  __attribute__((aligned (64)));
static int consID;		/* debug console ID */
static AS as;
static sceDmaChan *chgif;
static int ypos=1;

void pad_init(void)
{

    /* --- OPEN pad --- */
    scePadInit(0);
    scePadPortOpen(0, 0, pad_dma_buf);
}

u_int pad_read(void)
{
    u_char rdata[32];

	/* --- read pad --- */
    if(scePadRead(0, 0, rdata) > 0){
	return 0xffff ^ ((rdata[2] << 8) | rdata[3]);
    }
    else{
	return 0;
    }
}


void draw_debug_print_init(void)
{
    sceDevConsInit();
    consID = sceDevConsOpen((2048-(640/2))*16, (2048-(224/2))*16, 79, 40);


    as.giftag.ul=SCE_GIF_SET_TAG((sizeof(as)/sizeof(u_long128))-1, 1, 0, 0, 0, 1);
    as.giftag.st.REGS0=SCE_GIF_PACKED_AD;
    as.alpha.ul=SCE_GS_SET_ALPHA(SCE_GS_ALPHA_CS,SCE_GS_ALPHA_CD,SCE_GS_ALPHA_AS,SCE_GS_ALPHA_CD, 0);
    as.alpha_addr=SCE_GS_ALPHA_1;

    SyncDCache(&as, ((char *)&as)+sizeof(as)-1);

    chgif=sceDmaGetChan(SCE_DMA_GIF);

    ypos=1;
}


void draw_debug_print(char *fmt, ...)
{
    va_list ap;
    char strings[640 / 4];
    int x=1;

    va_start(ap, fmt);
    vsprintf(strings, fmt, ap);
    va_end(ap);

    if (strings[0]) {
	sceDevConsLocate(consID, x, ypos++);
	sceDevConsPrintf(consID, strings);
    }	
}

void draw_debug_print_exec(void)
{
    sceGsSyncPath(0,0);
    sceDmaSendN(chgif, &as, sizeof(as)/sizeof(u_long128));

    sceDevConsDraw(consID);
    sceDevConsClear(consID);
    ypos=1;
}
