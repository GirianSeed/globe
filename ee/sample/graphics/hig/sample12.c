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

#define FILENAME "data/restex.bin"

/***************************************************
 * sample program 12
 *
 *	A sample of texutre "permanent residence" or "non residence"
 *
 *	RESARG:		resident with argment
 *	NORESARG:	nonresident with argment
 *	STOPPLUG:	resident with stop plug
 *	CONTPLUG:	nonresidnet with cont plug
 ***************************************************/
enum {RESARG, NORESARG, STOPPLUG, CONTPLUG};

static void	*DATA_ADDR;

static sceHiPlug	*frameP;
static sceHiPlug	*microP;
static sceHiPlug	*tex2dP;
static sceHiPlugMicroData_t *microD;

static sceHiPlugTex2dInitArg_t tex2d_arg;
static int resident=RESARG;
static sceHiPlugMicroPreCalcArg_t arg;

void SAMPLE12_init(void)
{
    sceHiErr	err;

    DATA_ADDR = file_read(FILENAME);

    if((err = sceHiParseHeader((u_int *)DATA_ADDR)) != SCE_HIG_NO_ERR) {
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    if((err = sceHiGetPlug((u_int *)DATA_ADDR, "Frame", &frameP)) != SCE_HIG_NO_ERR) {
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    if((err = sceHiGetPlug((u_int *)DATA_ADDR, "Tex2d", &tex2dP)) != SCE_HIG_NO_ERR){
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    tex2d_arg.resident = TRUE;
    tex2d_arg.tbl = sceHiGsMemAlloc(SCE_HIGS_PAGE_ALIGN, sceHiGsMemRestSize());
    tex2dP->args = (u_int)&tex2d_arg;

    if((err = sceHiCallPlug(frameP, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR) {
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    if((err = sceHiGetPlug((u_int *)DATA_ADDR, "Micro", &microP)) != SCE_HIG_NO_ERR) {
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    microP->args = (u_int)&arg;
    arg.micro = 0;

    /* camera & light */
    if((microD = sceHiPlugMicroGetData(microP)) == NULL)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    camerainit(microD);
    lightinit(microD);
}

static const char *comment[]={"ON with arg","OFF with arg","ON with stop","OFF with cont"};

int SAMPLE12_main(void)
{
    sceHiErr	err;

    /* camear moving/set */
    cameractrl();
    cameraview(microD);

    if(gPad.SelectSwitch){
	resident++;
	if(resident>CONTPLUG) resident=RESARG;

	switch(resident){
	  case RESARG:
	      if((err = sceHiCallPlug(tex2dP, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR) {
		  error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	      }
	      tex2d_arg.resident = TRUE;
	      tex2dP->args = (u_int)&tex2d_arg;
	      if((err = sceHiCallPlug(tex2dP, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR) {
		  error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	      }
	      break;

	  case NORESARG:
	      if((err = sceHiCallPlug(tex2dP, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR) {
		  error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	      }
	      tex2d_arg.resident = FALSE;
	      tex2dP->args = (u_int)&tex2d_arg;
	      if((err = sceHiCallPlug(tex2dP, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR) {
		  error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	      }
	      break;

	  case STOPPLUG:
	      sceHiStopPlugStatus(tex2dP);
	      break;

	  case CONTPLUG:
	      sceHiContPlugStatus(tex2dP);
	      break;
	}

    }

    draw_debug_print(1, 10, "TEXTURE RESIDENT: %s [SELECT]",comment[resident]);

    if((err = sceHiCallPlug(frameP, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR) {
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    if((err = sceHiCallPlug(frameP, SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR) {
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    utilSetDMATimer();
    err = sceHiDMASend();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiDMAWait();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* transmit only 1st */
    arg.micro = -1;

    /* controll print */
    draw_debug_print(50, 1, "Rup/Rdown   : Cam Up/Down");
    draw_debug_print(50, 2, "Rleft/Rright: Cam Left/Right");
    draw_debug_print(50, 3, "R1/R2       : Cam Back/Front");
    draw_debug_print(50, 4, "Lup/Ldown   : Cam Rot X");
    draw_debug_print(50, 5, "Lleft/Lright: Cam Rot Y");
    draw_debug_print(50, 6, "L1/L2       : Cam Rot Z");
    draw_debug_print(50, 7, "SELECT      : Resident on/off");
    draw_debug_print(50, 8, "START       : EXIT");

    if(gPad.StartSwitch)
	return 1;
    return 0;
}

void SAMPLE12_exit(void)
{
    sceHiErr	err;

    if((err = sceHiCallPlug(frameP, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR) {
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    sceHiGsMemFree(tex2d_arg.tbl);

    free(DATA_ADDR);
}
