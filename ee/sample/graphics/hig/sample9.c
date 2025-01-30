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
 * sample program 0
 *
 *	Tim2 texture + mipmap sample
 ***************************************************/

static void	*DATA_ADDR;

static sceHiPlug	*frameP;
static sceHiPlug	*microP;
static sceHiPlug	*tim2P;
static u_int		*tim2D;
static sceHiPlugMicroPreCalcArg_t	arg;

static sceHiPlugMicroData_t *microD;

void SAMPLE9_init(void)
{
	sceHiErr	err;
	sceHiType	type;

	DATA_ADDR = file_read("data/tim2obj.bin");

	if ((err = sceHiParseHeader((u_int *)DATA_ADDR)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Frame", &frameP)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Tim2", &tim2P)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Micro", &microP)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	type.repository = SCE_HIP_COMMON;
	type.project = SCE_HIP_FRAMEWORK;
	type.category = SCE_HIP_TIM2;
	type.id = SCE_HIP_TIM2_DATA;
	type.revision = SCE_HIP_REVISION;
	type.status = SCE_HIG_DATA_STATUS;
	sceHiGetData(tim2P, &tim2D, type);
	if (tim2D == NULL)
	    printf("cant find tim2 data\n");

	{
	    int num, i;
	    char       *name;
	    u_int	*data;

	    num = sceHiPlugTim2Num(tim2P);
	    for (i = 0; i < num; i++) {
		name = sceHiPlugTim2GetName(tim2P, i);
		data = file_read(name);
		sceHiPlugTim2SetData(tim2P, i, data);  
	    }
	}

	if ((err = sceHiCallPlug(frameP, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if((microD = sceHiPlugMicroGetData(microP)) == NULL)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	camerainit(microD);
	lightinit(microD);
	arg.micro = 0;      
}

int SAMPLE9_main(void)
{
	sceHiErr	err;

	/* camear moving/set */
	cameractrl();
	cameraview(microD);

	/* micro code change? */
	microP->args = (u_int)&arg;

	if ((err = sceHiCallPlug(frameP, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiCallPlug(frameP, SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* transmit only 1st */
	arg.micro = -1;

	utilSetDMATimer();
	err = sceHiGsCtxRegist(sceHiGsStdCtx, 0);     
	sceHiDMASend();
	sceHiDMAWait();

	/* controll print */
	draw_debug_print(50, 1, "Rup/Rdown    : Cam Up/Down");
	draw_debug_print(50, 2, "Rleft/Rright : Cam Left/Right");
	draw_debug_print(50, 3, "R1/R2        : Cam Back/Front");
	draw_debug_print(50, 4, "Lup/Ldown    : Cam Rot X");
	draw_debug_print(50, 5, "Lleft/Lright : Cam Rot Y");
	draw_debug_print(50, 6, "L1/L2        : Cam Rot Z");
	draw_debug_print(50, 7, "START        : EXIT");

	if (gPad.StartSwitch)
		return 1;
	return 0;
}

void SAMPLE9_exit(void)
{
	sceHiErr	err;

	if ((err = sceHiCallPlug(frameP, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	free(DATA_ADDR);
}
