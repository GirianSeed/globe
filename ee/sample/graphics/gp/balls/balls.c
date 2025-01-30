/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *        Emotion Engine Library Sample Program
 *
 *                         - libgp: balls -
 *
 *                         Version 1.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            balls.c
 *	        elementary balls sample with libgp
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            Sep,20,2001      ogiso
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
#include <math.h>
#include <devfont.h>


#define SUNIT    0x08           /* プリセットするボールの数*/
#define MAXOBJ   (2500*SUNIT)   /* 表示するボールの数の最大 */

/* scratch pad area */
#define PACKETSIZE (sizeof(u_long128)*sceGpChkPacketSize(SCE_GP_PRIM_R | SCE_GP_SPRITE_FMTU, SUNIT))
#define OTSIZE    (sizeof(u_long128)*sceGpChkChainOtSize(1))
#define WORKSIZE   (0x0)	/* no attribute loading */
#define WORKBASE   0x70000000
#define OTUPPER    (WORKBASE+WORKSIZE)
#define SUPPER     (WORKBASE+WORKSIZE+OTSIZE)
#define OTLOWER    (WORKBASE+WORKSIZE+OTSIZE+PACKETSIZE)
#define SLOWER     (WORKBASE+WORKSIZE+OTSIZE+PACKETSIZE+OTSIZE)

/* display */
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 224
#define OFFX          (((4096-SCREEN_WIDTH)/2)<<4)
#define OFFY          (((4096-SCREEN_HEIGHT)/2)<<4)
#define WALL_X        ((SCREEN_WIDTH-16)<<4)	/* reflection point */
#define WALL_Y        ((SCREEN_HEIGHT-8)<<4)



extern int texture_image[];

typedef struct {
    u_short x, y;		/* 現在の位置 */
    u_short z, pad;
    u_short dx, dy;		/* 速度 */
    u_short pad1,pad2;
} POS;

#define BALL_SIZE  32
#define IMAGE_SIZE BALL_SIZE*8

u_long128 pad_dma_buf[scePadDmaBufferMax]  __attribute__((aligned (64)));


void  init_point(POS *pos,int n);


#define TRANSFER_MODE_SELECT SCE_GP_PATH3

int main( void )
{
    static sceGsDBuff  db;
    static sceGpLoadImage loadimage;
    static sceGpTexEnv texenv;
    static sceGpAlphaEnv alphaenv;

    sceGpChain chain[2];
    sceGpPrimR *sprpac[2];
    sceGpTextureArg texarg;

    u_int       fr;
    int         odev, sindex, count, nobj=1;
    int i,j;
    POS *pp,*pos;
    int x,y,w,h;
    int level;
    
    u_int paddata;
    u_char rdata[32];
        
    int consId;

    sceSifInitRpc(0);
    
    while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/sio2man.irx",
    			0, NULL) < 0){
        printf("Can't load module sio2man\n");
    }
    while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/padman.irx",
				0, NULL) < 0){
        printf("Can't load module padman\n");
    }

    *T0_MODE = T_MODE_CLKS_M | T_MODE_CUE_M; /* Hcount Timer Setting */
    sindex = 0;

    pos = (POS *)memalign( 16, MAXOBJ * sizeof(POS) );

    init_point(pos, MAXOBJ);	/* initialize ball attribute buffer */

    sceGsResetPath();
    sceDmaReset(1);
    sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);

    // --- set double buffer ---
    sceGsSetDefDBuff(&db, SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT,
		     SCE_GS_ZALWAYS, SCE_GS_PSMZ24, SCE_GS_CLEAR);
    *(u_long *)&db.clear0.rgbaq
      = SCE_GS_SET_RGBAQ(0x10, 0x60, 0x60, 0x80, 0x3f800000);
    *(u_long *)&db.clear1.rgbaq
      = SCE_GS_SET_RGBAQ(0x10, 0x60, 0x60, 0x80, 0x3f800000);
    
    sceGpInitChain(&chain[0], (void*)OTUPPER, 1);
    sceGpInitChain(&chain[1], (void*)OTLOWER, 1);
    
    sprpac[0] = (sceGpPrimR*)SUPPER;
    sprpac[1] = (sceGpPrimR*)SLOWER;
    
    texarg.tbp = 6720;
    texarg.tbw = IMAGE_SIZE / 64;
    texarg.tpsm = SCE_GS_PSMCT32;
    texarg.tx = texarg.ty = 0;
    texarg.tw = IMAGE_SIZE;
    texarg.th = IMAGE_SIZE / 8;
    
    sceGpInitLoadImage(&loadimage);
    sceGpInitTexEnv(&texenv, 0);
    sceGpInitAlphaEnv(&alphaenv, 0);
    
    sceGpSetLoadImage(&loadimage, &texarg, texture_image, 0);
    sceGpSetTexEnv(&texenv, &texarg, SCE_GS_MODULATE, SCE_GS_LINEAR);
    sceGpSetAlphaEnv(&alphaenv, SCE_GP_ALPHA_INTER_AS, 0);

    sceGpAddPacket(&chain[0], 0, &loadimage);
    sceGpAddPacket(&chain[0], 0, &texenv);
    sceGpAddPacket(&chain[0], 0, &alphaenv);
    
    FlushCache(0);
    sceGpKickChain(&chain[0], TRANSFER_MODE_SELECT);

    sceGpSetDefaultAbe(1);
    sceGpInitPacket((void*)sprpac[0], SCE_GP_PRIM_R | SCE_GP_SPRITE_FMTU, SUNIT);
    sceGpInitPacket((void*)sprpac[1], SCE_GP_PRIM_R | SCE_GP_SPRITE_FMTU, SUNIT);
    
    consId = sceDevConsOpen(OFFX, OFFY, 79, 40);

    odev = !sceGsSyncV(0);
    fr = 0;

    scePadInit(0);
    scePadPortOpen(0, 0, pad_dma_buf);
    
    
    sceGpSetRgbaFM(sprpac[0], 0x80, 0x80, 0x80, 0x80);
    sceGpSetRgbaFM(sprpac[1], 0x80, 0x80, 0x80, 0x80);    
    
    for(i=0;i<SUNIT;i++){
        level = sceGpIndexUvSpriteFMTU(2*i);
        sceGpSetUv(sprpac[0], level, (i*BALL_SIZE)<<4, 0<<4);
        sceGpSetUv(sprpac[1], level, (i*BALL_SIZE)<<4, 0<<4);

        level = sceGpIndexUvSpriteFMTU(2*i+1);
        sceGpSetUv(sprpac[0], level, ((i+1)*BALL_SIZE)<<4, BALL_SIZE<<4);
	sceGpSetUv(sprpac[1], level, ((i+1)*BALL_SIZE)<<4, BALL_SIZE<<4);
        
    }
    
    sceGpResetChain(&chain[0]);
    sceGpResetChain(&chain[1]);

    sceGpAddPacket(&chain[0],0,sprpac[0]);
    sceGpAddPacket(&chain[1],0,sprpac[1]);
    
    while(1){
    	*T0_COUNT = 0;		/* Hcounter reset */

        sceGsSwapDBuff(&db, fr);
	
        if(scePadRead(0, 0, rdata) > 0){
            paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);
        }else{
            paddata = 0;
        }

        if ((paddata&SCE_PADLup) && nobj<MAXOBJ/SUNIT) {
            nobj++;
        } else if ((paddata&SCE_PADLdown) && nobj>1) {
            nobj--;
        }

        pp = pos;
        for(j=0;j<nobj;j++) {
            for(i=0;i<SUNIT;i++){
                if ((u_int)(x = (pp->x + pp->dx*fr) % (WALL_X*2)) >= WALL_X)
                    x = WALL_X*2 - x;
                if ((y = (pp->y + pp->dy*fr) % (WALL_Y*2)) >= WALL_Y)
                    y = WALL_Y*2 - y;
		
                pp++;

                w = 32<<4;
                h = 16<<4;
		        
                level = sceGpIndexXyzfSpriteFMTU(2*i);
                sceGpSetXyzf(sprpac[sindex], level, x+OFFX, y+OFFY, 10,0xff);
                level = sceGpIndexXyzfSpriteFMTU(2*i+1);
                sceGpSetXyzf(sprpac[sindex], level, x+w+OFFX, y+h+OFFY, 10,0xff);

            }

            while(sceGpKickChain(&chain[sindex],TRANSFER_MODE_SELECT)<0)
                ;
            
            sindex ^= 0x01;
        }
	    
        count = *T0_COUNT;

        sceGsSetHalfOffset((fr&1) ? &db.draw1 : &db.draw0, 2048,2048,odev);
        
        if(!(fr%2)){
            sceDevConsLocate(consId, 1, 1);
            sceDevConsPrintf(consId, "BALLS = %d (H time %d)\n",nobj*SUNIT,count);
        }
	
        fr++;
        sceDevConsDraw(consId);
        
        FlushCache(0);
        odev = !sceGsSyncV(0);

    }
    free(pos);
    sceDevConsClose(consId);
}


/* attribute intialize */
void init_point(POS *pos , int num)
{
    int i;

    for (i = 0; i < num; i++) {
        pos->x  = rand();		/* スタート座標 Ｘ */
        pos->y  = rand();		/* スタート座標 Ｙ */
        pos->z  = 0x02<<4;
        pos->dx = (rand()%64) + 1;	/* 移動距離 Ｘ (1<=x<=4) */
        pos->dy = (rand()%32) + 1;	/* 移動距離 Ｙ (1<=y<=4) */
        pos++;
    }
}
