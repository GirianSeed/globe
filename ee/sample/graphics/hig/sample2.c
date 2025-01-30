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
 * sample program 2
 *
 *	A sample of block operation function by the data which used es2hig,
 *	and Move the model which used hierarchy root
 ***************************************************/

static void *DATA1_ADDR;
static void *DATA2_ADDR;

static sceHiPlug	*frameP1;
static sceHiPlug	*frameP2;
static sceHiPlug	*shapeP1;
static sceHiPlug	*shapeP2;
static sceHiPlug	*hrchyP1;
static sceHiPlug	*hrchyP2;
static sceHiPlug	*microP1;
static sceHiPlug	*tex2dP, *tex2dP2;
static sceHiPlugMicroData_t *microD1;
static sceHiPlugTex2dInitArg_t	tex2d_arg;

void SAMPLE2_init(void)
{
	sceHiErr	err;

	DATA1_ADDR = file_read("data/dino.bin");
	DATA2_ADDR = file_read("data/shuttle.bin");

	if ((err = sceHiParseHeader((u_int *)DATA1_ADDR)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiParseHeader((u_int *)DATA2_ADDR)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiGetPlug((u_int *)DATA1_ADDR, "Frame", &frameP1)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiGetPlug((u_int *)DATA2_ADDR, "Frame", &frameP2)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	tex2d_arg.resident = TRUE;
	tex2d_arg.tbl = NULL;
	if ((err = sceHiGetPlug((u_int *)DATA1_ADDR, "Tex2d", &tex2dP)) == SCE_HIG_NO_ERR){
	    tex2dP->args = (u_int)&tex2d_arg;
	}

	if ((err = sceHiGetPlug((u_int *)DATA2_ADDR, "Tex2d", &tex2dP2)) == SCE_HIG_NO_ERR) {
	    tex2dP2->args = (u_int)&tex2d_arg;
	}

	if ((err = sceHiCallPlug(frameP1, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiGetPlug((u_int *)DATA1_ADDR, "Micro", &microP1)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiGetPlug((u_int *)DATA1_ADDR, "Shape", &shapeP1)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiGetPlug((u_int *)DATA1_ADDR, "Hrchy", &hrchyP1)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiGetPlug((u_int *)DATA2_ADDR, "Shape", &shapeP2)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiGetPlug((u_int *)DATA2_ADDR, "Hrchy", &hrchyP2)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if((microD1 = sceHiPlugMicroGetData(microP1)) == NULL)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	camerainit(microD1);
	lightinit(microD1);
}

int SAMPLE2_main(void)
{
	sceHiErr	err;
	static int	obj = 0;

	/* camera moving */
	cameractrl();
	cameraview(microD1);

	/* object change */
	if (gPad.SelectSwitch) {
		if ((err = sceHiCallPlug(frameP1, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
		    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

		switch (obj) {
		  case 0:	/* current=DINO : next=SHUTTLE */
		    if ((err = sceHiRmvPlugBlk(frameP1, shapeP1)) != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
		    if ((err = sceHiRmvPlugBlk(frameP1, hrchyP1)) != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
		    if ((err = sceHiAddPlugBlk(frameP1, shapeP2)) != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
		    if ((err = sceHiAddPlugBlk(frameP1, hrchyP2)) != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
			break;
		  default:	/* current=SHUTTLE : next = DINO */
		    if ((err = sceHiRmvPlugBlk(frameP1, shapeP2)) != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
		    if ((err = sceHiRmvPlugBlk(frameP1, hrchyP2)) != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
		    if ((err = sceHiAddPlugBlk(frameP1, shapeP1)) != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
		    if ((err = sceHiAddPlugBlk(frameP1, hrchyP1)) != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
			break;
		}
		if ((err = sceHiCallPlug(frameP1, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
		    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

		obj = !obj;
	}

	if ((err = sceHiCallPlug(frameP1, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiCallPlug(frameP1, SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	utilSetDMATimer();
	if ((err = sceHiDMASend()) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	if ((err = sceHiDMAWait()) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* controll print */
	draw_debug_print(50, 1, "Rup/Rdown    : Cam Up/Down");
	draw_debug_print(50, 2, "Rleft/Rright : Cam Left/Right");
	draw_debug_print(50, 3, "R1/R2        : Cam Back/Front");
	draw_debug_print(50, 4, "Lup/Ldown    : Cam Rot X");
	draw_debug_print(50, 5, "Lleft/Lright : Cam Rot Y");
	draw_debug_print(50, 6, "L1/L2        : Cam Rot Z");
	draw_debug_print(50, 7, "SELECT       : Change Object");
	draw_debug_print(50, 8, "START        : EXIT");

	if (gPad.StartSwitch)
		return 1;
	return 0;
}

void SAMPLE2_exit(void)
{
	sceHiErr	err;

	if ((err = sceHiCallPlug(frameP1, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	free(DATA1_ADDR);
	free(DATA2_ADDR);
}

