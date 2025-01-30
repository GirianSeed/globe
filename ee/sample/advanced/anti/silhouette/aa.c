/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                    silhouette anti-alias sample program
 *                        Version 1.0
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version   Date           Design              Log
 *  --------------------------------------------------------------------
 *       1.00      Nov,07,2001    aoki   	first version
 */
#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <libgraph.h>
#include <eeregs.h>
#include <libgp.h>
#include <libhig.h>
#include <libdma.h>
#include <libpad.h>
#include <sifdev.h>
#include <malloc.h>
#include "aa.h"
#include "util.h"

static const char *modename[]={
    "ON: X",
    "ON: +",
    "OFF  ",
};


static struct {
    int blur[2];
    int cross;
    int testp;
    int disable;
} mode;


static int xyoffset[2];
static int rect[4];
static u_long128 *buffp; /* point to create packet */
static sceDmaTag *pAdd;
static int WBW, WBH;

static inline u_long lzc(u_long src);
static void Draw2Tex(sceGsTex0 *tex0, sceGsDrawEnv1 *draw, int tcc, int tfx);
static void Draw2Clamp(sceGsClamp *clamp, sceGsDrawEnv1 *draw);
static void aa_draw_sprite(int dx, int dy, int abe, int u, int v);
static void aa_draw_blur(int u, int v);
static void set_mode(int aa_mode);
static void add_reg(int reg, void *st);
static void add_drawenv(sceGsDrawEnv1 *draw);


void fba_change(int on)
{
    static u_long128 fbabuff[3];
    sceGpAd *fba=(sceGpAd *)fbabuff;
    
    sceGpInitAd(fba, 1);
    sceGpSetAd(fba, 0, SCE_GS_FBA_1, on?1:0);

    sceGpSetPacketMode(fba, 1);			/* for direct send  */
    sceGpSyncPacket(fba);

    sceGsSyncPath(0, 0);
    sceGpKickPacket2(fba, SCE_GP_PATH3);

}


static inline u_long lzc(u_long src)
{
    register u_long dst;

    __asm__ volatile(
        "plzcw %0,%1\n"
        : "=r" (dst): "r" (src));

    return dst;
}

static void Draw2Tex(sceGsTex0 *tex0, sceGsDrawEnv1 *draw, int tcc, int tfx)
{

    union {
        u_long ul;
        u_int ui[2];
    } texsize;

    tex0->TBP0= draw->frame1.FBP*2048/64;
    tex0->TBW= draw->frame1.FBW;
    tex0->PSM= draw->frame1.PSM;

    tex0->CPSM= 0;
    tex0->CSM= 0;
    tex0->CSA= 0;
    tex0->CLD= 0;

    texsize.ui[0]=((int)tex0->TBW*64)-1;
    texsize.ui[1]=(int)draw->scissor1.SCAY1;

    texsize.ul=lzc(texsize.ul);

    tex0->TW=32-texsize.ui[0]-1;
    tex0->TH=32-texsize.ui[1]-1;

    tex0->TCC=tcc;

    tex0->TFX=tfx;

}

static void Draw2Clamp(sceGsClamp *clamp, sceGsDrawEnv1 *draw)
{
    int w_region, h_region;
    union {
        u_long ul;
        u_int ui[2];
    } texsize, bufsize;

    texsize.ui[0]=((int)draw->frame1.FBW*64)-1;
    texsize.ui[1]=(int)draw->scissor1.SCAY1;

    bufsize.ul=lzc(texsize.ul);

    bufsize.ui[0]= (1<<bufsize.ui[0]);
    bufsize.ui[1]= (1<<bufsize.ui[1]);

    w_region=0;
    h_region=0;

    if (draw->scissor1.SCAX0!=0) w_region=1;
    if (draw->scissor1.SCAX1!=bufsize.ui[0]) w_region=1;

    if (draw->frame1.FBW*64!=bufsize.ui[0]) w_region=1;

    if (draw->scissor1.SCAY0!=0) h_region=1;
    if (draw->scissor1.SCAY1!=bufsize.ui[1]) h_region=1;

    if (w_region){
	clamp->WMS=SCE_GS_REGION_CLAMP;
	clamp->MINU=draw->scissor1.SCAX0;
	clamp->MAXU=draw->scissor1.SCAX1;
	if (clamp->MAXU>draw->frame1.FBW*64-1) clamp->MAXU=draw->frame1.FBW*64-1;
    } else{
	clamp->WMS=SCE_GS_CLAMP;
    }

    if (h_region){
	clamp->WMT=SCE_GS_REGION_CLAMP;
	clamp->MINV=draw->scissor1.SCAY0;
	clamp->MAXV=draw->scissor1.SCAY1;
    } else{
	clamp->WMT=SCE_GS_CLAMP;
    }


}

#define m16(x) ((x)*16)

static void aa_draw_sprite(int dx, int dy, int abe, int u, int v)
{
    sceGpPrimR *sprite;
    int type=SCE_GP_PRIM_R|SCE_GP_SPRITE_FMTU;
    int index;
    int u0, v0;
    int ofu, ofv;

    sprite=(sceGpPrimR *)buffp;
    buffp+=sceGpInitPrimR(sprite, type, 1);
	
    index=sceGpIndexUv(type, 0);

    u0= m16(u)-dx +8;
    if (u0<0){
	ofu= -u0;
	u0=0;
    } else{
	ofu=0;
    }

    v0= m16(v)-dy +8;
    if (v0<0){
	ofv= -v0;
	v0=0;
    } else{
	ofv=0;
    }

    sceGpSetUv(sprite, index, u0, v0);

    index=sceGpIndexUv(type, 1);
    sceGpSetUv(sprite, index, m16(u+WBW)-dx +8, m16(v+WBH)-dy +8);

    index=sceGpIndexXyzf(type, 0);
    sprite->reg[index].xyzf.X = xyoffset[0] + ofu;
    sprite->reg[index].xyzf.Y = xyoffset[1] + ofv;
    sprite->reg[index].xyzf.Z = 0xffff;

    index=sceGpIndexXyzf(type, 1);
    sprite->reg[index].xyzf.X = xyoffset[0] + m16(WBW);
    sprite->reg[index].xyzf.Y = xyoffset[1] + m16(WBH);
    sprite->reg[index].xyzf.Z = 0xffff;


    sceGpSetRgbaFM(sprite, 0xff, 0x0, 0x0, 0x80);

    sceGpSetAbe(sprite, abe);

    pAdd=sceGpAddPacket2(pAdd,  sprite);

    
}

static void aa_draw_sprite_rev(int dx, int dy, int abe)
{
    sceGpPrimR *sprite;
    int type=SCE_GP_PRIM_R|SCE_GP_SPRITE_FMTU;
    int index;

    sprite=(sceGpPrimR *)buffp;
    buffp+=sceGpInitPrimR(sprite, type, 1);
	
    index=sceGpIndexUv(type, 0);

    sceGpSetUv(sprite, index, 8, 8);

    index=sceGpIndexUv(type, 1);
    sceGpSetUv(sprite, index, m16(WBW)+8, m16(WBH)+8);

    index=sceGpIndexXyzf(type, 0);
    sprite->reg[index].xyzf.X = xyoffset[0] + m16(dx);
    sprite->reg[index].xyzf.Y = xyoffset[1] + m16(dy);
    sprite->reg[index].xyzf.Z = 0xffff;

    index=sceGpIndexXyzf(type, 1);
    sprite->reg[index].xyzf.X = xyoffset[0] + m16(dx+WBW);
    sprite->reg[index].xyzf.Y = xyoffset[1] + m16(dy+WBH);
    sprite->reg[index].xyzf.Z = 0xffff;

    sceGpSetRgbaFM(sprite, 0x80, 0x80, 0x80, 0x80);

    sceGpSetAbe(sprite, abe);

    pAdd=sceGpAddPacket2(pAdd,  sprite);

    
}





static void aa_draw_blur(int u, int v)
{

    int abe=1;

    if (mode.cross){
	aa_draw_sprite( mode.blur[0],  mode.blur[1], abe, u, v);
	aa_draw_sprite( mode.blur[0], -mode.blur[1], abe, u, v);
							
	aa_draw_sprite(-mode.blur[0],  mode.blur[1], abe, u, v);
	aa_draw_sprite(-mode.blur[0], -mode.blur[1], abe, u, v);
    }else {

	if (mode.blur[1]){
	    aa_draw_sprite( 0,  mode.blur[1], abe, u, v);
	    aa_draw_sprite( 0, -mode.blur[1], abe, u, v);
	}					 
						 
	if (mode.blur[0]){			 
	    aa_draw_sprite( mode.blur[0],  0, abe, u, v);
	    aa_draw_sprite(-mode.blur[0],  0, abe, u, v);
	}
    }

}

/* set blur and basic parameters */
void aa_init(int aa_mode, int hblur, int vblur)
{
    mode.blur[0]=hblur;
    mode.blur[1]=vblur;
    mode.testp=0;

    xyoffset[0] = 0;
    xyoffset[1] =  0;

    set_mode(aa_mode);

}

/* set work buffer parameters */
void aa_set_wb(AA_WB *wb, int wbp, int wpsm, int wbbw, int wbw, int wbh)
{
    WBW=wbw;
    WBH=wbh;

    sceGsSetDefDrawEnv(&wb->work, wpsm,
		       wbw, wbh, SCE_GS_ZALWAYS, 0);
    wb->work.zbuf1.ZMSK=1;
    wb->work.test1.ZTST=SCE_GS_ZALWAYS;

    wb->work.frame1.PSM=wpsm;
    wb->work.zbuf1.ZMSK=1;
    wb->work.xyoffset1.OFX=xyoffset[0];
    wb->work.xyoffset1.OFY=xyoffset[0];
    wb->work.frame1.FBP=wbp;
    wb->work.frame1.FBW=wbbw;

    wb->wrect[0]=wbw;
    wb->wrect[1]=wbh;

}




/* testp: ぼかす領域を赤く表示 */
/* hblur: 水平方向フィルタをかける ( 0:なし 8: 1pixel) */
/* vblur: 垂直方向フィルタをかける */
/* cross: 1:十字フィルタ 0: ななめフィルタ */

int aa_ctrl(u_int paddata, u_int padtrig)
{
    static int aa_mode=0;
    int chgpacket=0;
    int old_testp;
    

    old_testp=mode.testp;
    if (paddata&SCE_PADselect) mode.testp= 1;
    else mode.testp= 0;

    if ((mode.testp==1 && aa_mode==AA_OFF) 
	|| (padtrig&SCE_PADstart)){
	aa_mode++;
	if (aa_mode>=AA_MODE_MAX) aa_mode=0;
	chgpacket=1;

	set_mode(aa_mode);
    }

    if (old_testp!=mode.testp) chgpacket=1;

    draw_debug_print(1, 1, "Silhouette AA demo\n");
    draw_debug_print(1, 3, "AA mode= %s [START]\n", modename[aa_mode]);

    if (mode.testp) draw_debug_print(1, 5, "normal view [SELECT]\n");
    else draw_debug_print(1, 5, "test view [SELECT]\n");



    return chgpacket;
}

static void set_mode(int aa_mode)
{
    switch (aa_mode){
    case AA_CROSS_HV:
	mode.cross=1;
	mode.disable=0;
	break;
    case AA_HV:
	mode.cross=0;
	mode.disable=0;
	break;
    case AA_OFF:
	mode.disable=1;
	break;
    }
}


static void add_reg(int reg, void *st)
{
    sceGpAd *ad;

    ad=(sceGpAd *)buffp;
    buffp+=sceGpInitAd(ad, 1);

    sceGpSetAd(ad, 0, reg, *(u_long *)st);

    pAdd=sceGpAddPacket2(pAdd,  ad);
}

static void add_drawenv(sceGsDrawEnv1 *draw)
{
    sceGpAd *ad;
    int size;
    size=sizeof(sceGsDrawEnv1)/sizeof(u_long128);

    ad=(sceGpAd *)buffp;
    buffp+= sceGpInitAd(ad, size);
    *(sceGsDrawEnv1 *)ad->reg= *draw;

    pAdd=sceGpAddPacket2(pAdd, ad);
}





void aa_draw(AA_PACKET *aa_packet)
{

    while (sceGpKickChain(aa_packet->chain, SCE_GP_PATH3)<0) ;


}


void aa_mkpacket(AA_PACKET *aa_packet, sceGsDrawEnv1 *draw, AA_WB *wb, void *pPacket)
{
    int addorg=0;

    sceGsDrawEnv1 frame;
    sceGsAlpha alpha_as, alpha_fix;
    sceGsTest test_aless, test_zalways;
    sceGsTex0 tex0_frame, tex0_work, tex0_frame_mod;
    sceGsClamp clamp_frame, clamp_work;
    sceGsTex1 tex1_linear, tex1_nearest;
    u_long dummy=0;
    int fx, fy;

    aa_packet->pPacket=pPacket;

    rect[0] = draw->scissor1.SCAX0;
    rect[1] = draw->scissor1.SCAY0;
    rect[2] = 1 + draw->scissor1.SCAX1 - draw->scissor1.SCAX0;
    rect[3] = 1 + draw->scissor1.SCAY1 - draw->scissor1.SCAY0;

    /* set drawenv */
    frame= *draw;
    frame.zbuf1.ZMSK=1;
    frame.test1.ZTST=SCE_GS_ZALWAYS;		/* not use Z buffer  */
    frame.xyoffset1.OFX=xyoffset[0];
    frame.xyoffset1.OFY=xyoffset[1];

    /* set test */
    test_zalways.ATE=0;
    test_zalways.ATST=0;
    test_zalways.AREF=0;
    test_zalways.AFAIL=0;
    test_zalways.DATE=0;
    test_zalways.DATM=0;
    test_zalways.ZTE=1;
    test_zalways.ZTST=SCE_GS_ZALWAYS;		/* no depth test */

    /* test_aless: pass if (As<0x80) */
    test_aless= test_zalways;
    test_aless.ATE=1;
    test_aless.ATST=SCE_GS_ALPHA_LESS;
    test_aless.AREF=0x80;
    test_aless.AFAIL=SCE_GS_AFAIL_KEEP;


    /* set alpha */
    /*  alpha_as: interpolation by As  */
    alpha_as.A=SCE_GS_ALPHA_CS;
    alpha_as.B=SCE_GS_ALPHA_CD;
    alpha_as.C=SCE_GS_ALPHA_AS;
    alpha_as.D=SCE_GS_ALPHA_CD;
    alpha_as.FIX=0x80;

    /*  alpha_fix: interpolation by 0x30  */
    alpha_fix.A=SCE_GS_ALPHA_CS;
    alpha_fix.B=SCE_GS_ALPHA_CD;
    alpha_fix.C=SCE_GS_ALPHA_FIX;
    alpha_fix.D=SCE_GS_ALPHA_CD;
    alpha_fix.FIX=0x30;

    /* set tex0 */
    Draw2Tex(&tex0_frame, &frame, 1, SCE_GS_DECAL); /* texture: framebuffer area */
    Draw2Tex(&tex0_frame_mod, &frame, 1, SCE_GS_MODULATE); /* for testp */
    Draw2Tex(&tex0_work, &wb->work, 1, SCE_GS_DECAL); /* texture: work area */

    /* set clamp */
    Draw2Clamp(&clamp_frame, &frame); /* texture: framebuffer area */
    Draw2Clamp(&clamp_work, &wb->work); /* texture: work area */

    /* set tex1 */
    /* linear: for blur */
    tex1_linear.LCM=0;
    tex1_linear.MXL=0;
    tex1_linear.MMAG=SCE_GS_LINEAR;
    tex1_linear.MMIN=SCE_GS_LINEAR;
    tex1_linear.MTBA=0;
    tex1_linear.L=0;
    tex1_linear.K=0;

    /* nearest: for copy */
    tex1_nearest=tex1_linear;
    tex1_nearest.MMAG=SCE_GS_NEAREST;
    tex1_nearest.MMIN=SCE_GS_NEAREST;


    /* start packet-making */
    buffp=aa_packet->pPacket;

    /* create chain */
    aa_packet->chain=(sceGpChain *)buffp;
    buffp+=sizeof(sceGpChain)/sizeof(u_long128);
    buffp+=sceGpInitChain(aa_packet->chain, buffp, 1);
    pAdd=sceGpGetTailChain(aa_packet->chain, 0);

    if (!mode.disable){
	add_reg(SCE_GS_TEXFLUSH,	&dummy);

	for (fx=0; fx<rect[2]; fx+=WBW){
	    for (fy=0; fy<rect[3]; fy+=WBH){

		/* Frame Buffer => Work Buffer  */
		add_drawenv(&wb->work);
		add_reg(SCE_GS_TEX0_1,	&tex0_frame);
		add_reg(SCE_GS_CLAMP_1,	&clamp_frame);

		/* copy original */
		add_reg(SCE_GS_TEST_1,	&test_zalways);
		add_reg(SCE_GS_TEX1_1,	&tex1_nearest);
		aa_draw_sprite(0,  0, 0, fx, fy);

		/* add blur */
		add_reg(SCE_GS_ALPHA_1,	&alpha_as);
		add_reg(SCE_GS_TEST_1,	&test_aless);
		add_reg(SCE_GS_TEX1_1,	&tex1_linear);
		if (mode.testp) add_reg(SCE_GS_TEX0_1,	&tex0_frame_mod);


		aa_draw_blur(fx, fy);

		if (mode.testp) add_reg(SCE_GS_TEX0_1,	&tex0_frame);
		add_reg(SCE_GS_TEST_1,	&test_zalways);
		add_reg(SCE_GS_TEX1_1,	&tex1_nearest);


		/* add original again */
		if (addorg){
		    add_reg(SCE_GS_ALPHA_1,	&alpha_fix);
		    aa_draw_sprite(0,  0,  1, fx, fy);
		}

		/* Work Buffer => Frame Buffer  */    
		add_drawenv(&frame);
		add_reg(SCE_GS_TEXFLUSH,	&dummy);
		add_reg(SCE_GS_TEX0_1,	&tex0_work);
		add_reg(SCE_GS_CLAMP_1,	&clamp_work);

		/* copy from work buffer */
		aa_draw_sprite_rev(fx, fy, 0);
	    }
	}

	if (addorg) add_reg(SCE_GS_ALPHA_1,	&alpha_as);
    } else{
	/* for debug_print */
	add_drawenv(&frame);
    }


    SyncDCache(pPacket, buffp);
    aa_packet->packetsize=buffp-(u_long128 *)aa_packet->pPacket;

}
