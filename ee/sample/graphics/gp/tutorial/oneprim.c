/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *        Emotion Engine Library Sample Program
 *
 *			- libgp tutorial -
 *
 *                         Version 1.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            oneprim.c
 *			 draw just one sprite
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            May,15,2002      aoki
 */
#include <eekernel.h>
#include <eestruct.h>
#include <eeregs.h>
#include <stdio.h>
#include <malloc.h>
#include <libgp.h>
#include <libpad.h>
#include <libdma.h>
#include "defines.h"

/* packet type */
static const int type=SCE_GP_PRIM_R|SCE_GP_SPRITE_F;  /* R-style packet, Sprite, Flat-shading */
static const int pnum=1; /* polygon count */
static int vnum;  /* get vnum (vertex count) using type and pnum, later */

/* initial position */
static const float SpriteVertex[2][3]={
    {-100.0f, -45.5f, 1.0f}, /* top-left corner */
    { 100.0f,  45.5f, 1.0f}  /* bottom-right corner */
};

/* libgp sprite packet pointer */
static sceGpPrimR *pPrim=NULL;


/* initialize drawing packet  */
int oneprim_init()
{    
    int index;
    int i;

    printf("entering oneprim mode\n");

    /* allocate packet memory area */
    pPrim = (sceGpPrimR *)memalign(16, sceGpChkPacketSize(type, pnum)*sizeof(u_long128));
    if (pPrim==NULL){
	printf("%s:%d: malloc error\n", __FILE__, __LINE__);
	while (1);
    }

    /* initialize packet */
    sceGpInitPacket(pPrim, type, pnum);

    /* set packet mode 1: self-transfer mode (transferable without using chain) */
    sceGpSetPacketMode(pPrim, 1);

    /* get vertex num usin type and pnum */
    vnum=sceGpChkNumPtoV(type, pnum);

    /* set rgba value to the packet */
    for (i=0; i<pnum; i++){			/* flat shading: set rgba each polygon */
	index=sceGpIndexRgba(type, i);
	sceGpSetRgba(pPrim, index, 0, 255, 0, 128); /* green */
    }

    /* set xyzf value to the packet */
    for (i=0; i<vnum; i++){			/* each vertex */
	index=sceGpIndexXyzf(type, i);
	sceGpSetXyzf(pPrim, index, 
		     convert_x(SpriteVertex[i][0]),
		     convert_y(SpriteVertex[i][1]),
		     convert_z(SpriteVertex[i][2]),
		     0 );
    }


    
    return 1;

}

/* draw the packet */
int oneprim_draw(int paddata, int padtrig)
{    
    int dx=0;
    int dy=0;
    int i;
    int index;

    /* move holizontally */
    if (paddata&SCE_PADRleft) dx= -2*16;
    if (paddata&SCE_PADRright) dx= 2*16;

    /* move vertically */
    if (paddata&SCE_PADRup) dy= -1*16;
    if (paddata&SCE_PADRdown) dy= 1*16;

    /* update packet data */
    for (i=0; i<vnum; i++){			/* set xyz each vertex */
	index=sceGpIndexXyzf(type, i);
	pPrim->reg[index].xyzf.X+=dx;
	pPrim->reg[index].xyzf.Y+=dy;
    }

    FlushCache(WRITEBACK_DCACHE);		/* to DMA transfer */

    while (sceGpKickPacket(pPrim, SCE_GP_PATH3)!=0){	/* start drawing */

	/* when DMA channel was busy, wait for a while */
	volatile int i; for (i=0; i<500; i++) ;

    }

    return 1;
}

/* clear away */
int oneprim_exit()
{
    if (pPrim) {
	/* free packet area */
	free(pPrim);
	pPrim=NULL;
    }
    printf("exiting oneprim mode\n");

    return 1;
}
