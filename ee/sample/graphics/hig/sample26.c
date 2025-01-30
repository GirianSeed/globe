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
#include <libgp.h>
#include <malloc.h>
#include "util.h"
#include "camera.h"
#include "light.h"

/***************************************************
 * sample program 26
 *
 *     A clipping plug sample
 ***************************************************/
static void	*DATA_ADDR;

static sceHiPlug		*frameP;
static sceHiPlug		*microP;
static sceHiPlug		*tex2dP;
static sceHiPlugMicroData_t	*microD;

static sceHiPlugTex2dInitArg_t	tex2d_arg;
static sceHiPlugMicroPreCalcArg_t	arg;

static sceGpPrimR	*lgp;	/* libgp's rectangle packet */
static sceHiDMAChainID_t rectid;

extern u_int sce_micro_no_clip[];
void SAMPLE26_init(void)
{
    sceHiErr	err;
    sceHiPlugMicroTbl_t micro_tbl = {sce_micro_no_clip, SCE_HIP_MICRO_ATTR_NONE};
    sceHiPlugMicroInitArg_t marg = {&micro_tbl, 1};


    DATA_ADDR = file_read("data/clip.bin");

    if ((err = sceHiParseHeader((u_int *)DATA_ADDR)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Frame", &frameP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Tex2d", &tex2dP)) == SCE_HIG_NO_ERR) {
	tex2d_arg.resident = TRUE;
	tex2d_arg.tbl = NULL;
	tex2dP->args = (u_int)&tex2d_arg;
    } else
	tex2dP = NULL;

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Micro", &microP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    microP->args = (u_int)&marg;

    if ((err = sceHiCallPlug(frameP, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if((microD = sceHiPlugMicroGetData(microP)) == NULL)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    lightinit(microD);

    microP->args = (u_int)&arg;
    arg.micro = 0;

    /* make clip area smaller than display : for sample be easy to understand */
    {
	static sceVu0FVECTOR s = {320.0f, 112.0f, 512.0f, 0.0f};
	static sceVu0FVECTOR w = {1.0f, 0.47f, 2048.0f, 2048.0f};
	static sceVu0FVECTOR d = {5.0f, 16777000.0f, 1.0f, 65536.0f};
	static sceVu0FVECTOR p = { 0.0f, 0.0f, -30.0f, 0.0f };
	static sceVu0FVECTOR r = { 0.0f, 0.0f, 0.0f, 0.0f };
	static sceVu0FVECTOR i = { 0.0f, 0.0f, 0.0f, 0.0f };
	static sceVu0FVECTOR u = { 0.0f, 1.0f, 0.0f, 0.0f };
	perspective(microD, s, w, d);
	viewset(p,r,i, u);
	cameraview(microD);
    }

    /* Drawing Clipping Area Rectangle with libgp */
    {
#define NLINE 4
	static const u_int type = SCE_GP_PRIM_R | SCE_GP_LINE_FM;
	static sceVu0IVECTOR rect[] = {
	    {0, 0, 0, 1}, {0, 112, 0, 1},
	    {0, 112, 0, 1}, {320, 112, 0, 1},
	    {320, 112, 0, 1}, {320, 0, 0, 1},
	    {320, 0, 0, 1}, {0, 0, 0, 1}
	};
	int ofsx, ofsy;
	int x, y, z, i, j; 

	lgp = memalign(16, sizeof(u_long128) * sceGpChkPacketSize(type, NLINE));
	if (lgp == NULL)
	    error_handling(__FILE__, __LINE__, "memory allocate failed.");
	sceGpInitPacket(lgp, type, NLINE);
	sceGpSetAbe(lgp, 1);

	if ((err = sceHiDMAMake_ChainStart()) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__, __LINE__, sceHiGetErrStatePtr()->mes);
	if ((err = sceHiDMAMake_WaitMicro()) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__, __LINE__, sceHiGetErrStatePtr()->mes);
	if ((err = sceHiDMAMake_LoadGS((u_int *)(&lgp->giftag1), lgp->dmanext.qwc)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__, __LINE__, sceHiGetErrStatePtr()->mes);
	if ((err = sceHiDMAMake_ChainEnd(&rectid)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__, __LINE__, sceHiGetErrStatePtr()->mes);

	/* make rectangle */
	sceGpSetRgbaFM(lgp, 255, 0, 0, 128);
	ofsx = ((4096 - 312)/2)<<4;
	ofsy = ((4096 - 112)/2)<<4;
	z = 1<<4;
	for (i = 0; i < NLINE; i++) {
	    for (j = 0; j <= 1; j++) {
		x = rect[i * 2 + j][0] << 4;
		y = rect[i * 2 + j][1] << 4;
		sceGpSetXyzf(lgp, sceGpIndexXyzfLineFM(i*2+j), x + ofsx, y + ofsy, z, 0);
	    }
	}
    }
}

int SAMPLE26_main(void)
{
    sceHiErr	err;

    /* camear moving/set */
    cameractrl();
    cameraview(microD);

    if ((err = sceHiCallPlug(frameP, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if ((err = sceHiCallPlug(frameP, SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    utilSetDMATimer();
    sceHiDMARegist(rectid);
    if ((err = sceHiDMASend()) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if ((err = sceHiDMAWait()) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (gPad.StartSwitch)
	return 1;

    /* at the 1st time, dma micro */
    arg.micro = -1;

    return 0;
}

void SAMPLE26_exit(void)
{
    sceHiErr	err;

    if ((err = sceHiCallPlug(frameP, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    free(DATA_ADDR);
    free(lgp);
    sceHiDMADel_Chain(rectid);
}
