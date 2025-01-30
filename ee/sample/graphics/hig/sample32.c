/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
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
 * sample program 32
 *
 *     Tim2 Texture Change Animation Sample with Tim2 Plug accesser
 ***************************************************/
static void	*DATA_ADDR;

static sceHiPlug		*frameP;
static sceHiPlug		*microP;
static sceHiPlug		*tim2P;
static sceHiPlugMicroData_t	*microD;

static sceHiPlugMicroPreCalcArg_t	arg;

static int num_anim;

void SAMPLE32_init(void)
{
    sceHiErr	err;
    sceHiType	type;
    sceHiPlugTim2Data_t	*tim2d;
    sceHiPlugTim2Head_t	*tim2dH;
    int i;

    DATA_ADDR = file_read("data/water.bin");

    if ((err = sceHiParseHeader((u_int *)DATA_ADDR)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Frame", &frameP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Tim2", &tim2P)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__, __LINE__, sceHiErrState.mes);

    type.repository = SCE_HIP_COMMON;
    type.project = SCE_HIP_FRAMEWORK;
    type.category = SCE_HIP_TIM2;
    type.id = SCE_HIP_TIM2_DATA;
    type.revision = SCE_HIP_REVISION;
    type.status = SCE_HIG_DATA_STATUS;
    tim2dH = sceHiPlugTim2GetHead(tim2P, type);
    if (tim2dH == NULL)
	error_handling(__FILE__, __LINE__, "tim2 data not found\n");
    for (i = 0; i < tim2dH->num; i++) {
	tim2d = sceHiPlugTim2GetData(tim2dH, i);
	if (tim2d == NULL)
	    error_handling(__FILE__, __LINE__, "tim2 data was broken\n");
	sceHiPlugTim2SetData(tim2P, i, file_read(tim2d->fname));
    }

    if ((err = sceHiPlugTim2GetNPictures(tim2P, 0, &num_anim)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__, __LINE__, sceHiErrState.mes);

    if ((err = sceHiCallPlug(frameP, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Micro", &microP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    if((microD = sceHiPlugMicroGetData(microP)) == NULL)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    camerainit(microD);
    lightinit(microD);

    microP->args = (u_int)&arg;
    arg.micro = 0;
}

int SAMPLE32_main(void)
{
    static int anim = 0;	/* frame no. */
    static int ratio = 1;	/* skip ratio */
    static int good_to_go = ~0;	/* stop? */
    static int way = 1;		/* animation way(origin:1, inverse:-1) */
    sceHiErr	err;

    /* camear moving/set */
/*    cameractrl();*/
    cameraview(microD);

    /* check value */
    if (anim / ratio >= num_anim)	anim = 0;
    if (anim / ratio < 0)		anim = (num_anim - 1) * ratio;
    /* set value */
    if ((err = sceHiPlugTim2SetPicture(tim2P, 0, anim / ratio)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__, __LINE__, sceHiErrState.mes);

    draw_debug_print(1, 10, "Frame No.  : %d(/%d)", anim/ratio, num_anim);
    draw_debug_print(1, 11, "Direction  : %s", way == 1 ? "Regular" : "Reverse");
    draw_debug_print(1, 12, "Skip Ratio : %d", ratio - 1);

    draw_debug_print(45, 1, "SELECT       : %s", good_to_go ? "Stop" : "Resume");
    draw_debug_print(45, 2, "L up/down    : change direction");
    draw_debug_print(45, 3, "R up/down    : change skip ratio");
    if (!good_to_go) {
	draw_debug_print(45, 4, "L left/right : frame feed");
    }

    /* sequencial animation */
    if (good_to_go) {
	anim += way;
    }

    if ((err = sceHiCallPlug(frameP, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    if ((err = sceHiCallPlug(frameP, SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    utilSetDMATimer();
    if ((err = sceHiDMASend()) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    if ((err = sceHiDMAWait()) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    /* stop */
    if (gPad.SelectSwitch) {
	good_to_go = ~good_to_go;
    }
    /* frame feed */
    if (gPad.LupSwitch) {
	way = 1;
    }
    if (gPad.LdownSwitch) {
	way = -1;
    }
    /* playback direction */
    if (gPad.LrightSwitch && !good_to_go)
	anim += way * ratio;
    if (gPad.LleftSwitch && !good_to_go)
	anim -= way * ratio;
    /* skip frame ratio */
    if (gPad.RupSwitch)
	++ratio;
    if (gPad.RdownSwitch) {
	--ratio;
	if (ratio < 1)
	    ratio = 1;
    }

    if (gPad.StartSwitch)
	return 1;

    /* at the 1st time, dma micro */
    arg.micro = -1;

    return 0;
}

void SAMPLE32_exit(void)
{
    sceHiErr	err;

    if ((err = sceHiCallPlug(frameP, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    free(DATA_ADDR);
}
