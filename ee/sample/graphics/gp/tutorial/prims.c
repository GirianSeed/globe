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
 *                            prims.c
 *			 draw several prims
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



typedef struct{
    float v[3];
} VERTEX;


typedef struct{
    int v[3];
} RGB;

typedef struct {
    int type;					/* packet type  */
    int pnum;					/* polygon num  */
    VERTEX *vertex;				/* vertex data  */
    RGB *rgb;					/* color data  */
    sceGpPrimR *pPrim;				/* packet area  */
    int vnum;					/* calculate from pnum */
} PACKET;


/*** vertex data ***/
/* for triangle packet (butterfly-shape) */
static VERTEX TriVertex[]={
    {{ 80.0f,  50.0f, 3.0f}},
    {{ 30.0f,  30.0f, 3.0f}},
    {{ 30.0f,  70.0f, 3.0f}},
    {{ 80.0f,  50.0f, 3.0f}},
    {{130.0f,  30.0f, 3.0f}},
    {{130.0f,  70.0f, 3.0f}},
};

/* for triangle-strip packet (long rectangle) */
static VERTEX StripVertex[]={
    {{ -220.0f,  -60.0f, 1.0f}},
    {{ -220.0f,   -20.0f, 1.0f}},
    {{  -140.0f,  -60.0f, 1.0f}},
    {{  -140.0f,  -20.0f, 1.0f}},
    {{  -60.0f,  -60.0f, 1.0f}},
    {{  -60.0f,   -20.0f, 1.0f}},
};

/* for triangle-fan packet (hexagon) */
static VERTEX FanVertex[]={
    {{0.0f, 0.0f, 2.0f}},
    {{ -100.0f,  0.0f, 2.0f}},
    {{ -50.0f,  -173.2f/2.0f*0.455f, 2.0f}},
    {{  50.0f,  -173.2f/2.0f*0.455f, 2.0f}},
    {{  100.0f,  0.0f, 2.0f}},
    {{  50.0f,   173.2f/2.0f*0.455f, 2.0f}},
    {{ -50.0f,   173.2f/2.0f*0.455f, 2.0f}},
    {{ -100.0f,  0.0f, 2.0f}},
};


/*** color data ***/
static RGB Rgb[]={
    {{255, 255, 255}},				/* white */
    {{255, 0, 0}},				/* red */
    {{0, 255, 0}},				/* green */
    {{0, 0, 255}},				/* blue */
    {{255, 0, 0}},				/* red */
    {{0, 255, 0}},				/* green */
    {{0, 0, 255}},				/* blue */
    {{255, 0, 0}},				/* red */
};

static PACKET packet[]={
    /* triangle, monochrome */
    { SCE_GP_PRIM_R|SCE_GP_TRI_FM, 2, TriVertex, Rgb, NULL, 0 },

    /* triangle-strip, flat shading*/
    { SCE_GP_PRIM_R|SCE_GP_TRISTRIP_F, 4, StripVertex, Rgb, NULL, 0 },

    /* triangle-fan, gouraud shading*/
    { SCE_GP_PRIM_R|SCE_GP_TRIFAN_G, 6, FanVertex, Rgb, NULL, 0 },

};


/* GP chain */
static  sceGpChain chain;

/* GP memory area */
static u_long128 *packetarea=NULL;

#define PACKET_NUM (sizeof(packet)/sizeof(PACKET))

/* ordering table resolution. 1: as this case does not need any sorting */
#define OT_LENGTH 1

/* initialize drawing packet/chain  */
int prims_init()
{    
    int index;
    int i,j;
    PACKET *pp;
    u_long128 *pMem;
    int size;

    printf("entering prims mode\n");

    /* check total memory size */
    size=0;
    size+=sceGpChkChainOtSize(OT_LENGTH);
    for (i=0; i<PACKET_NUM; i++){
	size+= sceGpChkPacketSize(packet[i].type, packet[i].pnum);
    }

    /* allocate memory */
    packetarea = (u_long128 *)memalign(16, size*sizeof(u_long128));
    if (packetarea==NULL){
	printf("%s:%d: malloc error\n", __FILE__, __LINE__);
	while (1);
    }

    pMem=packetarea;

    /*  initialize chain */
    pMem+=sceGpInitChain(&chain, pMem, OT_LENGTH);

    /* initialize draw packets */
    for (j=0; j<PACKET_NUM; j++){
	pp= &packet[j];

	/* set memory */
	pp->pPrim=(sceGpPrimR *)pMem;

	/* init packet */
	pMem+=sceGpInitPacket(pp->pPrim, pp->type, pp->pnum);

	/* get vertex num */
	pp->vnum=sceGpChkNumPtoV(pp->type, pp->pnum);

	
	/* set rgba */
	if (pp->type&SCE_GP_MONOCHROME){	/* monochrome: set rgba each prim */
	    /* special set function for monochrome primitives */
	    sceGpSetRgbaFM(pp->pPrim,
			   pp->rgb[0].v[0], pp->rgb[0].v[1], pp->rgb[0].v[2], 128);

	} else if (pp->type&SCE_GS_PRIM_IIP){ /* gouraud shading: set rgba each vertex */
	    for (i=0; i<pp->vnum; i++){
		index=sceGpIndexRgba(pp->type, i);
		sceGpSetRgba(pp->pPrim, index,  
			     pp->rgb[i].v[0], pp->rgb[i].v[1], pp->rgb[i].v[2], 128);


	    }

	} else{				/* flat shading: set rgba each polygon  */
	    for (i=0; i<pp->pnum; i++){			
		index=sceGpIndexRgba(pp->type, i);
		sceGpSetRgba(pp->pPrim, index, 
			     pp->rgb[i].v[0], pp->rgb[i].v[1], pp->rgb[i].v[2], 128);
	    }

	}

	/* set xyz */
	for (i=0; i<pp->vnum; i++){			/* set xyz each vertex */
	    index=sceGpIndexXyzf(pp->type, i);
	    sceGpSetXyzf(pp->pPrim, index, 
			 convert_x(pp->vertex[i].v[0]),
			 convert_y(pp->vertex[i].v[1]),
			 convert_z(pp->vertex[i].v[2]),
			 0 );
	}
    }

    /* add packet to chain */
    for (i=0; i<PACKET_NUM; i++){

	sceGpAddPacket(&chain, 0, packet[i].pPrim);
    }

    return 1;
}

int prims_draw(u_int paddata, u_int padtrig)
{    
    static int ctrlprim=0;
    PACKET *pp;
    int i;
    int index;
    int dx=0;
    int dy=0;

    /* change targets */
    if ((padtrig&SCE_PADLright)||(padtrig&SCE_PADLup)){
	ctrlprim++;
	if (ctrlprim>=PACKET_NUM) ctrlprim=0;

	printf("current control: %d\n", ctrlprim);
    }
    if ((padtrig&SCE_PADLleft)||(padtrig&SCE_PADLdown)){
	ctrlprim--;
	if (ctrlprim<0) ctrlprim=PACKET_NUM-1;

	printf("current control: %d\n", ctrlprim);
    }

    /* move target primitive */
    pp= &packet[ctrlprim];

    /* move holizontally */
    if (paddata&SCE_PADRleft) dx= -2*16;
    if (paddata&SCE_PADRright) dx= 2*16;

    /* move vertically */
    if (paddata&SCE_PADRup) dy= -1*16;
    if (paddata&SCE_PADRdown) dy= 1*16;

    /* update packet data */
    for (i=0; i<pp->vnum; i++){			/* set xyz each vertex */
	index=sceGpIndexXyzf(pp->type, i);
	pp->pPrim->reg[index].xyzf.X+=dx;
	pp->pPrim->reg[index].xyzf.Y+=dy;
    }


    FlushCache(WRITEBACK_DCACHE);		/* to DMA transfer */

    while (sceGpKickChain(&chain, SCE_GP_PATH3)!=0){	/* start drawing */

	/* when DMA channel was busy, wait for a while */
	volatile int i; for (i=0; i<500; i++) ;

    }

    return 1;
}

/* clear away */
int prims_exit()
{
    if (packetarea) {
	free(packetarea);
	packetarea=NULL;
    }

    printf("exiting prims mode\n");

    return 1;
}
