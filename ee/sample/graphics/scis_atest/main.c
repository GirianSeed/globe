/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: main.c,v 1.3 2003/09/12 07:14:11 masae Exp $
 */
/* 
 * Emotion Engine Library Sample Program
 *
 * Copyright (C) 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * scis_at - main.c
 *	main program
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2002-10-08      aoki        
 */

#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <libdev.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libdma.h>
#include <libgp.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libpad.h>

#include "mathfunc.h"
#include "util.h"

#define IMAGE_SIZE 256      /* flower texture size  */
#define PI 3.14159265358979323846f
#define PI2 (PI*2) 

static sceGsDBuff      db;
static sceGpLoadImage loadimage;
static sceGpTexEnv texenv;
static sceDmaChan *chvif1;
static int cutlinear=1; /* 1: scis_atest.vsm,  0: scis_atest2.vsm */


static void dev_init(void);
static void print_usage(void);
static void dbuff_init(sceGsDBuff *pDb);
static void texture_init(sceGpLoadImage *pLi, sceGpTexEnv *pTe);
static void dma_ch1_kick(void *addr);
static void obj_ctrl1(u_int paddata);
static void obj_ctrl2(u_int paddata);
static void obj_ctrl(u_int paddata, u_int padtrig);
static void Vu0ZeroVector(sceVu0FVECTOR v0);

extern u_int load_micro_code1;
extern u_int load_micro_code2;
extern u_int My_dma_start;  /* vsm setting + draw torus object (packet.dsm) */
extern u_int My_line_start;  /* draw line object (packet.dsm) */
extern u_int My_texture1;   /* texture (flower.dsm) */
extern float scissor_param[4]; /* scissoring parameter  */
extern sceVu0FMATRIX local_world;



static void dev_init(void)
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

    sceGsResetPath();

    pad_init();

    chvif1=sceDmaGetChan(SCE_DMA_VIF1);
    chvif1->chcr.TTE=1;
}

static void print_usage(void)
{
    if (cutlinear){
	draw_debug_print("function mode: LINEAR\n");
	draw_debug_print("Left Cursor: plane rotation\n");
	draw_debug_print("L1/L2: plane position\n");
	draw_debug_print("R1/R2: alpha resolution\n");
    } else{
	draw_debug_print("function mode: QUADRATIC\n");
	draw_debug_print("cursor left/right: plane position\n");
	draw_debug_print("cursor up/down: function scaling\n");
	draw_debug_print("L1/L2: function offset\n");
    }
    draw_debug_print("start: turn scissoring OFF\n");
    draw_debug_print("select: change cutting functions\n");
    draw_debug_print("\n");
}

/* init gs, setup double buffer */
static void dbuff_init(sceGsDBuff *pDb)
{
    sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);

    /* --- set double buffer --- */
    sceGsSetDefDBuff(pDb, SCE_GS_PSMCT32, 640, 224,
		     SCE_GS_ZGREATER, SCE_GS_PSMZ24, 1); 

    pDb->clear0.rgbaq.R=0x40;
    pDb->clear0.rgbaq.G=0x40;
    pDb->clear0.rgbaq.B=0x40;
    pDb->clear0.rgbaq.A=0x60;
    pDb->clear0.xyz2a.Z=0;
    pDb->clear0.xyz2b.Z=0;

    pDb->clear0.testb.AFAIL=SCE_GS_AFAIL_KEEP;
    pDb->clear0.testb.ATST=SCE_GS_ALPHA_LEQUAL;
    pDb->clear0.testb.AREF=0x80;
    pDb->clear0.testb.ATE=1;

    pDb->clear1= pDb->clear0;
}

/* texture load * texenv setup */
static void texture_init(sceGpLoadImage *pLi, sceGpTexEnv *pTe)
{
    sceGpTextureArg texarg;

    /* --- set object texture parameters --- */
    texarg.tbp=6720;
    texarg.tbw=  IMAGE_SIZE / 64;
    texarg.tpsm=  SCE_GS_PSMCT32;
    texarg.tx= texarg.ty= 0;
    texarg.tw= IMAGE_SIZE;
    texarg.th= IMAGE_SIZE;
    texarg.cbp=0;
    texarg.cpsm=0;

    /* --- set load image 1--- */
    sceGpInitLoadImage(pLi);
    sceGpSetLoadImage(pLi, &texarg, &My_texture1, 0);
    sceGpSetPacketMode(pLi, 1);	/* direct send mode */

    /* transfer texture */
    FlushCache(WRITEBACK_DCACHE);
    sceGsSyncPath(0, 0);
    sceGpKickPacket2(pLi, SCE_GP_PATH3);

    /* --- set texture env 1 --- */
    sceGpInitTexEnv(pTe, 0);
    sceGpSetTexEnv(pTe, &texarg, SCE_GS_MODULATE, SCE_GS_LINEAR);

    pTe->tex0.TCC=0; /* !! テクスチャの ALPHA は使わない!! */

    sceGpSetPacketMode(pTe, 1);	/* direct send mode */
    sceGpSyncPacket(pTe);
}


/* start dma (source-chain, TTE=ON) */
static void dma_ch1_kick(void *addr)
{

    sceGsSyncPath(0, 0);

    sceDmaSend(chvif1, addr);
}

/* v0 <= (0,0,0,1)  */
static void Vu0ZeroVector(sceVu0FVECTOR v0)
{
    __asm__ __volatile__(
        "sqc2    vf0,0x0(%0)\n"
        : : "r" (v0) : "memory");
}


/* control parameters */
static void obj_ctrl(u_int paddata, u_int padtrig)
{
    float drot= PI/180.0f;

    if (padtrig&SCE_PADselect) cutlinear= cutlinear?0:1;

    if(paddata & SCE_PADRdown){
	obj_rot[0] += drot; if(obj_rot[0]>PI) obj_rot[0] -= PI2; }
    if(paddata & SCE_PADRup){
	obj_rot[0] -= drot; if(obj_rot[0]<-PI) obj_rot[0] += PI2; }
    if(paddata & SCE_PADRright){
	obj_rot[1] += drot; if(obj_rot[1]>PI) obj_rot[1] -= PI2; }
    if(paddata & SCE_PADRleft){
	obj_rot[1] -= drot; if(obj_rot[1]<-PI) obj_rot[1] += PI2; }

    /* calculate & set matrix for vu1-use */
    cal_matrix();

    if (cutlinear) obj_ctrl1(paddata);
    else  obj_ctrl2(paddata);
}

static void obj_ctrl1(u_int paddata)
{
    sceVu0FMATRIX work;
    sceVu0FVECTOR unitv;
    float doffset= 0.5f;
    int i;
    float drot= PI/180.0f;
    static float plocal1[3]={-1.71f,-2.79f,0.0f};
    static float offset=126.0f;
    static float resolution=30.0f;

    if(paddata & SCE_PADLdown){
	plocal1[0] += drot; if(plocal1[2]>PI) plocal1[2] -= PI2; }
    if(paddata & SCE_PADLup){
	plocal1[0] -= drot; if(plocal1[2]<-PI) plocal1[2] += PI2; }
    if(paddata & SCE_PADLright){
	plocal1[1] += drot; if(plocal1[1]>PI) plocal1[1] -= PI2; }
    if(paddata & SCE_PADLleft){
	plocal1[1] -= drot; if(plocal1[1]<-PI) plocal1[1] += PI2; }


    if(paddata & SCE_PADR1){
	resolution*=1.03f;
    }
    if(paddata & SCE_PADR2){
	resolution/=1.03f;
    }
    draw_debug_print("res=%8.1f\n",resolution);

    
    /* plocal1 で指定された向きの、長さ１の法線を作る */
    sceVu0UnitMatrix(work);
    Vu0ZeroVector(unitv);
    unitv[2]= 1.0f;
    sceVu0RotMatrix(work,work,plocal1);
    sceVu0ApplyMatrix(unitv, work, unitv);


    // 下の二行を有効にすると、ワールド面固定の切断面になる
    // sceVu0InversMatrix(work, local_world);
    // sceVu0ApplyMatrix(unitv, work, unitv);


    if(paddata & SCE_PADL1) offset+=doffset;
    if(paddata & SCE_PADL2) offset-=doffset;
    draw_debug_print("offset=%8.1f\n",offset);

    for (i=0; i<3; i++){
	scissor_param[i]=unitv[i]*resolution;
    }
    scissor_param[3]=offset;
}

static void obj_ctrl2(u_int paddata)
{
    float dt= 0.5f;
    float dr= 1.03f;
    static float plocal2[3]={0.0095f,-2048.5f, -1.0f};

    if(paddata & SCE_PADLdown) plocal2[0] *= dr; 
    if(paddata & SCE_PADLup) plocal2[0] /= dr; 

    if(paddata & SCE_PADLright) plocal2[1] += dt;
    if(paddata & SCE_PADLleft) plocal2[1] -= dt;

    if(paddata & SCE_PADL1) plocal2[2]*=dr;
    if(paddata & SCE_PADL2) plocal2[2]/=dr;


    draw_debug_print("scaling: %f\n",plocal2[0]);
    draw_debug_print("offset: %f\n",plocal2[2]);


    /* コントローラ操作用の計算式: p[0](x+p[1])^2 + p[2] (p: plocal) */
    /* vu1 が実際に使う計算式:  s[0]x^2  + s[1]x + s[2] (s: scissor_param) */
    /* つまり p と s との関係は:  s[0]=p[0], s[1]=2p[0]p[1], s[2]=p[1]^2 p[0] + p[2]*/
    scissor_param[0] = plocal2[0];
    scissor_param[1] = 2.0f * plocal2[0] * plocal2[1];
    scissor_param[2] = plocal2[0]*plocal2[1]*plocal2[1] + plocal2[2];
    scissor_param[3] = 0.0f;

}


/* main */
int main(void)
{
    int    odev;
    u_int  frame;
    u_int paddata=0;
    u_int padtrig;
    static u_int oldpad=0;
    

    dev_init();					/* init controller */

    dbuff_init(&db);				/* setup gs double buffering */
    
    draw_debug_print_init();			/* init devcons */

    /* texture load & texenv setup */
    texture_init(&loadimage, &texenv);		/* init texture (flower) */

    frame = 0;
    odev = !sceGsSyncV(0);



    while(1){					/* main loop */
	sceGsSwapDBuff(&db, frame);

	oldpad=paddata;
	paddata=pad_read();
	padtrig=paddata & ~oldpad;


	print_usage();

	/* handle controller input */
	obj_ctrl(paddata, padtrig);

	sceGsSyncPath(0, 0);

	/* set texture env (flower) for object (torus) */
	sceGpKickPacket2(&texenv, SCE_GP_PATH3);

	FlushCache(WRITEBACK_DCACHE);

	/* load micro code */
	if (cutlinear) dma_ch1_kick(&load_micro_code1);
	else dma_ch1_kick(&load_micro_code2);

	/* draw object (torus) */
	dma_ch1_kick(&My_dma_start);

	/* draw on-screen info */
	draw_debug_print_exec();

	frame++;
	if (paddata&SCE_PADstart){	/* 次フレームの scissoring なし */
	    db.clear0.testb.ATE=0;
	    db.clear1.testb.ATE=0;
	} else{				/* 次フレームの scissoring あり */
	    db.clear0.testb.ATE=1;
	    db.clear1.testb.ATE=1;
	}

	sceGsSetHalfOffset((frame&1) ? &db.draw1 : &db.draw0, 2048,2048,1-odev);

	FlushCache(WRITEBACK_DCACHE);
	odev = !sceGsSyncV(0);
    }

    return 0;
}
