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

static sceVu0FMATRIX root_mat={{1.0f,0.0f,0.0f,0.0f},{0.0f,1.0f,0.0f,0.0f},{0.0f,0.0f,1.0f,0.0f},{0.0f,0.0f,0.0f,1.0f}};
static sceVu0FVECTOR root_rot={0.0f,0.0f,0.0f,0.0f};
static sceVu0FVECTOR root_trans={0.0f,0.0f,0.0f,0.0f};

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
	draw_debug_print(1, 6, "ROOT POS : %5.2f %5.2f %5.2f",
			 root_trans[0],
			 root_trans[1],
			 root_trans[2]);
	draw_debug_print(1, 7, "ROOT ROT : %5.2f %5.2f %5.2f",
			 root_rot[0],
			 root_rot[1],
			 root_rot[2]);
			 
}


/*
  micro
*/
extern u_int sce_micro_skin[];

/***************************************************
 * skin sample program
 *
 *     control 3 bone rotation
 ***************************************************/

static void	*DATA_ADDR;

static sceHiPlug	*frameP;
static sceHiPlug	*microP;
static sceHiPlug	*hrchyP;
static sceHiPlug	*tim2P;
static sceHiPlugHrchyHead_t	*hrchyH;
static sceHiPlugMicroData_t	*microD;
static sceHiPlugMicroPreCalcArg_t myarg = {0, 0.0f, 0.0f, 0.0f};
static sceHiPlugHrchyPreCalcArg_t hrchy_arg;
static sceHiPlugShapeHead_t	*mh;		/* matrix header */

void SAMPLE30_init(void)
{
    sceHiErr	err;
    sceHiType	type;
    sceHiPlugMicroTbl_t	micro_tbl[]={
	{sce_micro_skin, SCE_HIP_MICRO_ATTR_NONE}
    };
    sceHiPlugMicroInitArg_t micro_arg[]={
	{&micro_tbl[0], 1}
    };


    DATA_ADDR = file_read("data/skin1.bin");

    if ((err = sceHiParseHeader((u_int *)DATA_ADDR)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Frame", &frameP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);


    /*	get plug	*/
    type.repository = SCE_HIP_COMMON;
    type.project = SCE_HIP_FRAMEWORK;
    type.category = SCE_HIP_MICRO;
    type.status = SCE_HIG_PLUGIN_STATUS;
    type.id = SCE_HIP_MICRO_PLUG;
    type.revision = SCE_HIP_REVISION;
    if ((err = sceHiGetInsPlug(frameP, &microP, type)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    type.category = SCE_HIP_HRCHY;
    type.id = SCE_HIP_HRCHY_PLUG;
    if ((err = sceHiGetInsPlug(frameP, &hrchyP, type)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    microP->args = (u_int)&micro_arg;

    /*	tim2	*/
    type.category = SCE_HIP_TIM2;
    type.id = SCE_HIP_TIM2_PLUG;
    if ((err = sceHiGetInsPlug(frameP, &tim2P, type)) == SCE_HIG_NO_ERR){
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
    }

    /*	init process	*/
    if ((err = sceHiCallPlug(frameP, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    /*	get data	*/
    if((microD = sceHiPlugMicroGetData(microP)) == NULL)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    type.repository = SCE_HIP_COMMON;
    type.project = SCE_HIP_FRAMEWORK;
    type.category = SCE_HIP_HRCHY;
    type.status = SCE_HIG_DATA_STATUS;
    type.id = SCE_HIP_HRCHY_DATA;
    type.revision = SCE_HIP_REVISION;
    hrchyH = sceHiPlugHrchyGetHead(hrchyP, type);
    if(hrchyH == NULL)	error_handling(__FILE__, __LINE__, sceHiErrState.mes);

    /*	init camera & light	*/
    camerainit(microD);
    lightinit(microD);

    myarg.micro = 0;
    myarg.anticutoff = 0.0f;
    myarg.fogbegin = 0.0f;
    myarg.fogend = 0.0f;
    microP->args = (u_int)&myarg;

    hrchyP->args = (u_int)&hrchy_arg;
    hrchy_arg.root = &root_mat;

    type.category = SCE_HIP_SHAPE;
    type.id = SCE_HIP_BASEMATRIX;
    mh = sceHiPlugShapeGetHead(hrchyP, type);
    if(mh == NULL)	error_handling(__FILE__, __LINE__, sceHiErrState.mes);
}

#define MAXMODE 2

static void bone_ctrl(void);

enum {CAMERA_MODE,BONE_MODE,ROOT_MODE};

int SAMPLE30_main(void)
{
    sceHiErr	err;
    static int mode = CAMERA_MODE;

    if (gPad.SelectSwitch)
	mode = (++mode) > MAXMODE ? CAMERA_MODE : mode;

    switch(mode){
      case CAMERA_MODE:
	  cameractrl();
	  cameraview(microD);
	  break;
      case BONE_MODE:
	  bone_ctrl();
	  break;
      case ROOT_MODE:
	  root_ctrl();
	  break;
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

    myarg.micro = -1;	/* dma microcode at 1 time */

    if (gPad.StartSwitch)
	return 1;
    return 0;
}

void SAMPLE30_exit(void)
{
    sceHiErr	err;

    if ((err = sceHiCallPlug(frameP, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    free(DATA_ADDR);
}


static int idx = 0;
#define NBONE	3
static void bone_ctrl(void)
{
    float  dr = 0.5f / 180.0f * PI;
    sceHiPlugHrchyData_t *d;
    sceVu0FMATRIX *m;
    int i;

    m = (sceVu0FMATRIX *)(sceHiPlugShapeGetMatrix(mh, idx)->local);

    d = sceHiPlugHrchyGetData(hrchyH, idx);

    if (gPad.LdownOn) {
	d->rot[0] -= dr;
	if(d->rot[0] < -PI/2.0f) d->rot[0] = -PI/2.0f;
    }
    if(gPad.LupOn) {
	d->rot[0] += dr;
	if(d->rot[0] > PI/2.0f) d->rot[0] = PI/2.0f;
    }
    if (gPad.LleftOn) {
	d->rot[1] -= dr;
	if(d->rot[1] < -PI/2.0f) d->rot[1] = -PI/2.0f;
    }
    if(gPad.LrightOn) {
	d->rot[1] += dr;
	if(d->rot[1] > PI/2.0f) d->rot[1] = PI/2.0f;
    }

    if(gPad.RrightSwitch){
	idx += 1;
	if(idx > NBONE) idx = NBONE;
    }
    if(gPad.RleftSwitch){
	idx -= 1;
	if(idx < 0) idx = 1;
    }

    draw_debug_print(1, 6, "BONE %d: %5.2f %5.2f %5.2f",idx,d->rot[0],d->rot[1],d->rot[2]);

    for(i=0;i<4;i++){
	draw_debug_print(1,8+i,"%5.2f %5.2f %5.2f %5.2f\n",(*m)[i][0],(*m)[i][1],(*m)[i][2],(*m)[i][3]);
    }
}
