/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *               Graphics Framework Sample Program
 *                  
 * 
 *      Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          Name : main.c
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            0.30      Apr,4,2000     thatake     Beta0
 *            0.40      Jun,16,2000     	   +animation, clut
 *            0.50      Jul,17,2000
 *            2.1       Nov,17,2000                
 */

#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libdma.h>
#include <libpkt.h>
#include <libvu0.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libdev.h>
#include <libpad.h>
#include <math.h>
#include "defines.h"
#include "data.h"
#include "frame_t.h"
#include "micro.h"
#include "object.h"
#include "light.h"
#include "gmain.h"
#include "camera.h"
#include "pad.h"

extern fwDataHead_t head_top;

u_char rdata[32];
u_long128 pad_dma_buf[scePadDmaBufferMax] __attribute__((aligned(64)));

void ErrPrintf(char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}

// load iop modules
void load_modules ()
{
    sceSifInitRpc (0);

    while (sceSifLoadModule ("host0:/usr/local/sce/iop/modules/sio2man.irx", 0, NULL) < 0) {
	ErrPrintf("  loading %s failed", "sio2man");
    }
    while (sceSifLoadModule ("host0:/usr/local/sce/iop/modules/padman.irx", 0, NULL) < 0) {
	ErrPrintf("  loading %s failed", "padman");
    }

    printf ("loading modules ... done.\n");

    return;
}

/************************/
/*	Main		*/
/************************/

int main(int argc, char *argv[])
{
    u_int frame;
    int    odev;

    load_modules ();

    /* --- Frame Work Initialization ---*/
    fwInit();

    /* --- open pad --- */
    fwPadInit();

    frame = 0;
    odev = !sceGsSyncV(0);

    // clear one buffer (GS)		// ****
    fwSync(frame, 1-odev);		// ****

    // clear the other buffer and set drawenv		// ****
    fwSync(frame, odev);				// ****

    while (1){
	/* --- read pad --- */
	fwPadCheck();

	/* --- Frame Work Main Operation --- */
	fwMain(frame);
	
	frame++;

	odev = !sceGsSyncV(0);

	/* --- Frame Work Sync Operation --- */
	fwSync(frame, odev);
    }

    fwFinish();

    return 0;

}

