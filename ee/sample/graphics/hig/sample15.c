/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
*/
/*
 *
 *	Copyright (C) 2002 Sony Computer Entertainment Inc.
 *							All Right Reserved
 *
 */
#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <eeregs.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>
#include "util.h"
#include "camera.h"
#include "light.h"

/***************************************************
 * sample program 15
 *
 *	motion blur sample
 *	
 *	doing alpha blend of a picture of an ex-frame
 ***************************************************/

static void	*DATA_ADDR;

static sceHiPlug	*frameP;
static sceHiPlug	*microP;
static sceHiPlug	*tex2dP;
static sceHiPlugMicroData_t *microD;

static sceHiPlugTex2dInitArg_t	tex2d_arg;
static sceHiPlugMicroPreCalcArg_t	micro_arg;

static void set_texarea(sceHiGsCtx *port, int x, int y, int w, int h);
static void copy_texarea(int swap);

sceHiGsCtx *gsctx;


void SAMPLE15_init(void)
{
    int _dispw=640;
    int _disph=224;

    tex2d_arg.resident = FALSE;
    tex2d_arg.tbl = sceHiGsMemAlloc(SCE_HIGS_PAGE_ALIGN, 
				    SCE_HIGS_PAGE_ALIGN*sceHiGsMemRestSize()/SCE_HIGS_PAGE_ALIGN);

    if (tex2d_arg.tbl==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    DATA_ADDR = file_read("data/APEWALK39.bin");

    if (sceHiParseHeader((u_int *)DATA_ADDR) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (sceHiGetPlug((u_int *)DATA_ADDR, "Frame", &frameP) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


    if(sceHiGetPlug((u_int *)DATA_ADDR, "Tex2d", &tex2dP) == SCE_HIG_NO_ERR)
	tex2dP->args = (u_int)&tex2d_arg;

    if (sceHiCallPlug(frameP, SCE_HIG_INIT_PROCESS) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* camera & light */
    if (sceHiGetPlug((u_int *)DATA_ADDR, "Micro", &microP) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if((microD = sceHiPlugMicroGetData(microP)) == NULL)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    camerainit(microD);
    lightinit(microD);

    sceHiGsCtxUpdate(sceHiGsStdCtx);

    gsctx=sceHiGsCtxCreate(0);
    if (gsctx==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    sceHiGsCtxCopy(gsctx, sceHiGsStdCtx);
    gsctx->validregs=0;
    sceHiGsCtxSetZbufDepth(gsctx, 0, 0);
    gsctx->value.test.ATE=0;
    gsctx->value.test.DATE=0;
    sceHiGsCtxSetRegAlpha(gsctx,
			  SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CD,
			  (int)(0.5*128));

    sceHiGsCtxUpdate(gsctx);

    micro_arg.micro = 0;      

    set_texarea(sceHiGsStdCtx, 2048-(_dispw/2), 2048-(_disph/2), _dispw, _disph);
}

/* controll BLUR parameters */
static void blur_ctrl()
{
    static float blur=0.5f;
    float bt=0.01f;

    if (gPad.L1On) blur+=bt;
    if (gPad.L2On) blur-=bt;
    gPad.L1On=0;
    gPad.L2On=0;
    if (blur>0.9f) blur=0.9f;
    if (blur<0.0f) blur=0.0f;
    sceHiGsCtxSetRegAlpha(gsctx,
			  SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CD,
			  (int)(blur*128));
    sceHiGsCtxUpdate(gsctx);
    draw_debug_print(1, 15, "BLUR Param: %8.2f  [L1/L2]", blur);
}

int SAMPLE15_main(void)
{
    sceHiErr	err;
    float s, e;

    /* controll blue parameter */
    blur_ctrl();

    /* camera moving/set */
    cameractrl();
    cameraview(microD);

    /* micro code change? */
    microP->args = (u_int)&micro_arg;

    if (sceHiCallPlug(frameP, SCE_HIG_PRE_PROCESS) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (sceHiCallPlug(frameP, SCE_HIG_POST_PROCESS) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* transmit only 1st */
    micro_arg.micro = -1;

    utilSetDMATimer();

    err = sceHiDMASend();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiDMAWait();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* This work is same as usual drawing to this place at all */

    s= ((float)DGET_T0_COUNT() * 100.0f) / 9765.625f;
    /* transmit the context for copies */
    sceHiGsCtxSend(gsctx, 0);

    /* exec copy ex-frame */
    copy_texarea(1-sceHiGsStdCtx->swap);

    sceGsSyncPath(0, 0);
    sceHiGsCtxSend(sceHiGsStdCtx, 0);
    sceGsSyncPath(0, 0);

    e= ((float)DGET_T0_COUNT() * 100.0f) / 9765.625f;
    draw_debug_print(1, 16, "BLUR POW  : %8.2f", e-s);

    draw_debug_print(50, 1, "Rup/Rdown    : Obj Up/Down");
    draw_debug_print(50, 2, "Rleft/Rright : Obj Left/Right");
    draw_debug_print(50, 3, "R1/R2        : Obj Back/Front");
    draw_debug_print(50, 4, "Lup/Ldown    : Obj Rot X");
    draw_debug_print(50, 5, "Lleft/Lright : Obj Rot Y");
    draw_debug_print(50, 6, "L1/L2        : Blur Effect");
    draw_debug_print(50, 7, "START        : EXIT");

    if (gPad.StartSwitch)
	return 1;
    return 0;
}

void SAMPLE15_exit(void)
{
    if (sceHiCallPlug(frameP, SCE_HIG_END_PROCESS) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (tex2d_arg.tbl) sceHiGsMemFree(tex2d_arg.tbl);
    sceHiGsCtxDelete(gsctx);
    free(DATA_ADDR);

}

/* a packet to use to copy screen of an ex-fraem */
static struct {
    sceHiGsGiftag giftag;
    u_long texflush;
    u_long texflush_addr;
    sceGsTex1   tex1;
    u_long tex1_addr;
    sceGsTex0   tex0;
    u_long tex0_addr;
    sceGsPrim   prim;
    u_long prim_addr;
    sceGsRgbaq   rgbaq;
    u_long rgbaq_addr;
    sceGsUv     uv0;
    u_long uv0_addr;
    sceGsXyz  xyz0;
    u_long xyz0_addr;
    sceGsUv     uv1;
    u_long uv1_addr;
    sceGsXyz  xyz1;
    u_long xyz1_addr;
} pack[2] __attribute__ ((aligned(16)));

/* set value to packet */
static void set_texarea(sceHiGsCtx *port, int x, int y, int w, int h)
{
    int offset;
    int i;

    for (i=0; i<2; i++){
	//	pack[i].giftag.nloop= sizeof(pack[0])/sizeof(u_long128) - 1;
	pack[i].giftag.nloop= 9;
	pack[i].giftag.eop = 1;
	pack[i].giftag.id = 0;
	pack[i].giftag.pre = 0;
	pack[i].giftag.prim = 0;
	pack[i].giftag.flg = 0;
	pack[i].giftag.nreg = 1;
	pack[i].giftag.regs = SCE_GIF_PACKED_AD;

	pack[i].texflush=0;
	pack[i].texflush_addr=SCE_GS_TEXFLUSH;

	pack[i].tex1.LCM = 0;
	pack[i].tex1.MXL = 0;
	pack[i].tex1.MMAG = SCE_GS_NEAREST;
	pack[i].tex1.MMIN = SCE_GS_NEAREST;
	pack[i].tex1.MTBA = 0;
	pack[i].tex1.L = 0;
	pack[i].tex1.K = 0;
	pack[i].tex1_addr=SCE_GS_TEX1_1;

	sceHiGsCtxGetTex0(port, (u_long *)&pack[i].tex0, port->isDbuf?i:0, 0, SCE_GS_DECAL);
	pack[i].tex0_addr=SCE_GS_TEX0_1;


	offset=0;

	pack[i].prim.IIP=1;
	pack[i].prim.FGE=0;
	pack[i].prim.ABE=1;
	pack[i].prim.AA1=0;
	pack[i].prim.FST=1;
	pack[i].prim.CTXT=0;
	pack[i].prim.FIX=0;
	pack[i].prim.PRIM=SCE_GS_PRIM_SPRITE;
	pack[i].prim.TME=1;
	pack[i].prim_addr=SCE_GS_PRIM;


	pack[i].rgbaq.R=128;
	pack[i].rgbaq.G=128;
	pack[i].rgbaq.B=128;
	pack[i].rgbaq.A=128;
	pack[i].rgbaq.Q=1.0f;
	pack[i].rgbaq_addr=SCE_GS_RGBAQ;


	pack[i].uv0.U=8;
	pack[i].uv0.V=8+offset;
	pack[i].uv0_addr=SCE_GS_UV;

	pack[i].uv1.U=w*16+8;
	pack[i].uv1.V=h*16+8+offset;
	pack[i].uv1_addr=SCE_GS_UV;

	pack[i].xyz0.X=(x)*16;
	pack[i].xyz0.Y=(y)*16;
	pack[i].xyz0.Z=0xffffffff;
	pack[i].xyz0_addr=SCE_GS_XYZ2;
	pack[i].xyz1.X=(x+w)*16 ;
	pack[i].xyz1.Y=(y+h)*16;
	pack[i].xyz1.Z=0xffffffff;
	pack[i].xyz1_addr=SCE_GS_XYZ2;
    }

}

/* send packet and execute a screen copy of an ex-frame */
static void copy_texarea(int k)
{
    sceGsSyncPath(0, 0);
    *D2_QWC = 10;
    *D2_MADR = (u_int) &pack[k] & 0x1fffffff;
    *D2_CHCR = 1 | (0 << 2) | (0 << 4) | (0 << 5) | (0 << 6) | (0 << 7) | (1 << 8);
}
