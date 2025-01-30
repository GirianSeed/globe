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
 * sample program 1
 *
 *	A sample to change Hrhcy root
 ***************************************************/
static void *DATA1_ADDR;
static void *DATA2_ADDR;

static sceHiPlug	*frameP1;
static sceHiPlug	*frameP2;
static sceHiPlug	*microP1, *microP2;
static sceHiPlug	*tex2dP1, *tex2dP2;
static sceHiPlug	*hrchyP2;
static sceHiPlugMicroData_t *microD1,*microD2;

static sceHiPlugTex2dInitArg_t		tex2d_arg;
static sceHiPlugHrchyPreCalcArg_t	hrchy_arg;
static sceHiPlugMicroPreCalcArg_t	micro_arg;

static sceVu0FMATRIX root_mat;
static sceVu0FVECTOR root_rot;
static sceVu0FVECTOR root_trans;

static void root_ctrl(void);


void SAMPLE1_init(void)
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
	if((err = sceHiGetPlug((u_int *)DATA1_ADDR, "Tex2d", &tex2dP1)) == SCE_HIG_NO_ERR) {
	    tex2dP1->args = (u_int)&tex2d_arg;
	}

	if ((err = sceHiGetPlug((u_int *)DATA2_ADDR, "Tex2d", &tex2dP2)) == SCE_HIG_NO_ERR) {
	    tex2dP2->args = (u_int)&tex2d_arg;
	}

	if ((err = sceHiCallPlug(frameP1, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiCallPlug(frameP2, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiGetPlug((u_int *)DATA1_ADDR, "Micro", &microP1)) != SCE_HIG_NO_ERR) 
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiGetPlug((u_int *)DATA2_ADDR, "Micro", &microP2)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	micro_arg.micro = 0;
	microP1->args = (u_int)&micro_arg;
	microP2->args = (u_int)&micro_arg;


	/* get hrchy-plugin block from the data */
	if (sceHiGetPlug(DATA2_ADDR, "Hrchy", &hrchyP2) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	hrchy_arg.root= &root_mat;
	hrchyP2->args = (u_int)&hrchy_arg;


	/* camera & light */
	if((microD1 = sceHiPlugMicroGetData(microP1)) == NULL)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if((microD2 = sceHiPlugMicroGetData(microP2)) == NULL)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	camerainit(microD1);
	camerainit(microD2);
	lightinit(microD1);
	lightinit(microD2);

	/* initial parameters	*/
	sceVu0UnitMatrix(root_mat);
	sceVu0ScaleVector(root_rot, root_rot, 0.0f);
	sceVu0ScaleVector(root_trans, root_trans, 0.0f);
}

int SAMPLE1_main(void)
{
	sceHiErr	err;

	/* move with Hrchy Root of shuttle */
	root_ctrl();

	if ((err = sceHiCallPlug(frameP1, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiCallPlug(frameP2, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiCallPlug(frameP1, SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiCallPlug(frameP2, SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	utilSetDMATimer();
	sceHiDMASend();
	sceHiDMAWait();

	/* controll print */
	draw_debug_print(50, 1, "Rup/Rdown    : Obj Up/Down");
	draw_debug_print(50, 2, "Rleft/Rright : Obj Left/Right");
	draw_debug_print(50, 3, "R1/R2        : Obj Back/Front");
	draw_debug_print(50, 4, "Lup/Ldown    : Obj Rot X");
	draw_debug_print(50, 5, "Lleft/Lright : Obj Rot Y");
	draw_debug_print(50, 6, "L1/L2        : Obj Rot Z");
	draw_debug_print(50, 7, "START        : EXIT");

	if (gPad.StartSwitch)
		return 1;

	micro_arg.micro = -1;

	return 0;
}

void SAMPLE1_exit(void)
{
	sceHiErr	err;

	if ((err = sceHiCallPlug(frameP1, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiCallPlug(frameP2, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	free(DATA1_ADDR);
	free(DATA2_ADDR);
}

static void root_ctrl(void)
{
    float  dr = 0.5f / 180.0f * PI;
    float  dt = 0.2f;

	if (gPad.LdownOn) {
		root_rot[0]-=dr; 
		if(root_rot[0]<-PI) root_rot[0] += 2.0f*PI; 
	}
	if(gPad.LupOn) {
		root_rot[0]+=dr;
		if(root_rot[0]>PI) root_rot[0] -= 2.0f*PI; 
	}
	if(gPad.LrightOn) {
		root_rot[1]+=dr;
		if(root_rot[1]>PI) root_rot[1] -= 2.0f*PI; 
	}
	if(gPad.LleftOn) {
		root_rot[1]-=dr;
		if(root_rot[1]<-PI) root_rot[1] += 2.0f*PI; 
	}
	if(gPad.L1On) {
		root_rot[2]+=dr;
		if(root_rot[2]>PI) root_rot[2] -= 2.0f*PI; 
	}
	if(gPad.L2On) {
		root_rot[2]-=dr;
		if(root_rot[2]<-PI) root_rot[2] += 2.0f*PI; 
	}
	if(gPad.RdownOn)
		root_trans[1]+=dt;
	if(gPad.RupOn)
		root_trans[1] -= dt;
	if(gPad.RrightOn)
		root_trans[0] += dt;
	if(gPad.RleftOn)
		root_trans[0] -= dt;
	if(gPad.R1On)
		root_trans[2] -= dt;
	if(gPad.R2On)
		root_trans[2] += dt;

	sceVu0UnitMatrix(root_mat);
	sceVu0RotMatrixX(root_mat, root_mat, root_rot[0]);
	sceVu0RotMatrixY(root_mat, root_mat, root_rot[1]);
	sceVu0RotMatrixZ(root_mat, root_mat, root_rot[2]);
	sceVu0TransMatrix(root_mat, root_mat, root_trans);

	/* infomation */
	draw_debug_print(1, 6, "SHUTTLE ROOT POS : %5.2f %5.2f %5.2f",
			 root_trans[0],
			 root_trans[1],
			 root_trans[2]);
	draw_debug_print(1, 7, "SHUTTLE ROOT ROT : %5.2f %5.2f %5.2f",
			 root_rot[0],
			 root_rot[1],
			 root_rot[2]);
			 
}
