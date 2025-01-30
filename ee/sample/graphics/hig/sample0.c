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
 *    	the simple 1 object viewer
 *
 ***************************************************/
static void	*DATA_ADDR;

static sceHiPlug		*frameP;
static sceHiPlug		*microP;
static sceHiPlug		*tex2dP;
static sceHiPlugMicroData_t	*microD;

static sceHiPlugTex2dInitArg_t	tex2d_arg;
static sceHiPlugMicroPreCalcArg_t	arg;

static char file_list[][32]={
    "data/dino.bin",		/* basic */
    "data/shuttle.bin",		/* hierarchy */
    "data/tri.bin",		/* shared polygon */
    "data/logo.bin",		/* shared polygon */
    "data/cowstrip.bin",	/* triangle strip */
    "data/mult.bin",		/* multi-texture */
    "data/APEWALK39.bin",	/* hierarchy animation */
    "data/xyz_anim.bin"		/* matrix animation */
};

#define MAX_FILE	sizeof(file_list)/(sizeof(char)*32)

static int file_id = 0;

void SAMPLE0_init(void)
{
    sceHiErr	err;

    DATA_ADDR = file_read(file_list[file_id]);

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

    if ((err = sceHiCallPlug(frameP, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Micro", &microP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if((microD = sceHiPlugMicroGetData(microP)) == NULL)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    camerainit(microD);
    lightinit(microD);

    microP->args = (u_int)&arg;
    arg.micro = 0;
}

void SAMPLE0_exit(void)
{
    sceHiErr	err;

    if ((err = sceHiCallPlug(frameP, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    free(DATA_ADDR);
}

int SAMPLE0_main(void)
{
    sceHiErr	err;

    cameractrl();
    cameraview(microD);

    if ((err = sceHiCallPlug(frameP, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if ((err = sceHiCallPlug(frameP, SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    utilSetDMATimer();
    if ((err = sceHiDMASend()) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if ((err = sceHiDMAWait()) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (gPad.StartSwitch)
	return 1;

    /*	change file	*/
    if (gPad.SelectSwitch){
	file_id++;
	if(file_id >= MAX_FILE) file_id = 0;
	SAMPLE0_exit();
	SAMPLE0_init();
    }

    /* at the 1st time, dma micro */
    arg.micro = -1;

    return 0;
}
