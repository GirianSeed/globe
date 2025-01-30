/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                    silhouette anti-alias sample program
 *                        Version 1.0
 *
 *      Copyright (C) 1998-2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version   Date           Design              Log
 *  --------------------------------------------------------------------
 *       1.00      Nov,07,2001    aoki   	first version (originally from basic3d-vu1)
 */

#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <libdev.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libdma.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libpad.h>

#include "mathfunc.h"
#include "util.h"
#include "aa.h"


/* silhouette-AA filter packet area */
#define BUFFSIZE (128*140)
static u_long128 tmpbuff[2][BUFFSIZE];


extern u_int My_dma_start;  /* object (packet.dsm) */
extern u_int My_texture1;   /* texture (flower.dsm) */
#define IMAGE_SIZE 256      /* flower texture size  */


#define PI 3.14159265358979323846f
#define PI2 (PI*2) 

void dev_init(void)
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
}

/* init gs, setup double buffer */
void dbuff_init(sceGsDBuff *pDb)
{
    
    sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);

    /* --- set double buffer --- */
    sceGsSetDefDBuff(pDb, SCE_GS_PSMCT32, 640, 224,
		     SCE_GS_ZGREATER, SCE_GS_PSMZ24, 1);  /* clear only for Z buffer */


}

/* texture load * texenv setup */
void texture_init(sceGpLoadImage *pLi, sceGpTexEnv *pTe)
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
    sceGpSetPacketMode(pLi, 1);	/* for direct send  */

    /* transfer texture */
    sceGpSyncPacket(pLi);
    sceGsSyncPath(0, 0);
    sceGpKickPacket2(pLi, SCE_GP_PATH3);

    /* --- set texture env 1 --- */
    sceGpInitTexEnv(pTe, 0);
    sceGpSetTexEnv(pTe, &texarg, SCE_GS_MODULATE, SCE_GS_LINEAR);
    sceGpSetPacketMode(pTe, 1);	/* for direct send  */
    sceGpSyncPacket(pTe);
}


/* start dma (source-chain, TTE=ON) */
void dma_ch1_kick(void *addr)
{

    sceGsSyncPath(0, 0);

    DPUT_D1_QWC(0x00);
    DPUT_D1_TADR((u_int)addr & 0x1fffffff);
    FlushCache(0);
    DPUT_D1_CHCR(D_CHCR_DIR_M | (1<<D_CHCR_MOD_O) | D_CHCR_TTE_M | D_CHCR_STR_M);

}

/* control camera/object */
void obj_ctrl(u_int paddata)
{
    float  delta = 1.0f / 180.0f * PI;

    if(paddata & SCE_PADLdown){
	obj_rot[0] += delta; if(obj_rot[0]>PI) obj_rot[0] -= PI2; }
    if(paddata & SCE_PADLup){
	obj_rot[0] -= delta; if(obj_rot[0]<-PI) obj_rot[0] += PI2; }
    if(paddata & SCE_PADLright){
	obj_rot[1] += delta; if(obj_rot[1]>PI) obj_rot[1] -= PI2; }
    if(paddata & SCE_PADLleft){
	obj_rot[1] -= delta; if(obj_rot[1]<-PI) obj_rot[1] += PI2; }

    if(paddata & SCE_PADL1){
	obj_rot[2] += delta; if(obj_rot[2]>PI) obj_rot[2] -= PI2; }
    if(paddata & SCE_PADL2){
	obj_rot[2] -= delta; if(obj_rot[2]<-PI) obj_rot[2] += PI2; }

    if(paddata & SCE_PADRdown){
	camera_rot[0] += delta; if(camera_rot[0]>PI) camera_rot[0] -= PI2; }
    if(paddata & SCE_PADRup){
	camera_rot[0] -= delta; if(camera_rot[0]<-PI) camera_rot[0] += PI2; }
    if(paddata & SCE_PADRright){
	camera_rot[1] += delta; if(camera_rot[1]>PI) camera_rot[1] -= PI2; }
    if(paddata & SCE_PADRleft){
	camera_rot[1] -= delta; if(camera_rot[1]<-PI) camera_rot[1] += PI2; }

    if(paddata & SCE_PADR1){
	camera_p[2] -= delta*10; if(camera_p[2]<-100) camera_p[2] = -100; }
    if(paddata & SCE_PADR2){
	camera_p[2] += delta*10; if(camera_p[2]>-10) camera_p[2] = -10; }

}

/* main */
sceGpLoadImage loadimage;
sceGpTexEnv texenv;
sceGsDBuff      db;
AA_PACKET aa_packet[2];
AA_WB aa_wb;
int main()
{
    int    odev;
    u_int  frame;
    u_int paddata=0;
    u_int padtrig=0;
    static u_int oldpad=0;

    dev_init();

    dbuff_init(&db);
    
    draw_debug_print_init();

    /* texture load & texenv setup */
    texture_init(&loadimage, &texenv);

    /* background texture setup */
    init_bg_texture();

    /* --- silhouette-AA setup --- */
    /* set AA parameters */
    aa_init(AA_CROSS_HV, 8, 4);
    /* set work buffer information */
    aa_set_wb(&aa_wb, db.draw0.zbuf1.ZBP, SCE_GS_PSMCT32, db.draw0.frame1.FBW, 32, 32);
    /* setup packet for AA-filtering */
    aa_mkpacket(&aa_packet[0], &db.draw0, &aa_wb, tmpbuff[0]);
    aa_mkpacket(&aa_packet[1], &db.draw1, &aa_wb, tmpbuff[1]);

    frame = 0;
    odev = !sceGsSyncV(0);

    while(1){					/* main loop */
	sceGsSwapDBuff(&db, frame);

	load_bg_texture((frame&1) ? db.draw1.frame1.FBP : db.draw0.frame1.FBP, odev);


	paddata=pad_read();
	padtrig=paddata& ~oldpad;
	oldpad=paddata;

	/* change filter parameters  */
	if (aa_ctrl(paddata, padtrig)>0){
	    /* update filter packet */
	    aa_mkpacket(&aa_packet[0], &db.draw0, &aa_wb, aa_packet[0].pPacket);
	    aa_mkpacket(&aa_packet[1], &db.draw1, &aa_wb, aa_packet[1].pPacket);
	}

	/* object rotate & change view point */
	obj_ctrl(paddata);

	/* calculate & set matrix for vu1-use */
	cal_matrix();

	sceGsSyncPath(0, 0);

	/* texture env for object */
	sceGpKickPacket2(&texenv, SCE_GP_PATH3);

	/* FBA ON (for silhouette-aa) */
	fba_change(1);

	/* draw object */
	dma_ch1_kick(&My_dma_start);


	/* FBA off */
	fba_change(0);

	/* apply silhouette-antialias filter */
	aa_draw(&aa_packet[frame&1]);


	sceGsSyncPath(0, 0);
	draw_debug_print_exec();

	frame++;
	sceGsSetHalfOffset((frame&1) ? &db.draw1 : &db.draw0, 2048,2048,1-odev);

	FlushCache(0);
	odev = !sceGsSyncV(0);

    }

    printf("finish!\n");
    return 0;

}
