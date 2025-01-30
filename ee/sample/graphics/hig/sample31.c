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
extern void MatrixDispSet(sceHiPlugShapeHead_t *sh, sceHiPlugMicroData_t *md);


static const char *files[]={
    "data/skin0.bin",
};


/************************************************/
/*	Skin Vu0				*/
/************************************************/

static sceHiPlugMicroPreCalcArg_t micro_pre_arg = { 0, 0.0f, 1298388.0f, 93784.0f};

#define DATA_NUM (sizeof(files)/sizeof(char *))

static void	*DATA_ADDR[DATA_NUM];

static sceHiPlug	*frameP[DATA_NUM];
static sceHiPlug	*microP[DATA_NUM];
static sceHiPlug	*tex2dP;

static sceHiPlugTex2dInitArg_t	tex2d_arg[DATA_NUM];
static sceHiGsMemTbl 	*gsmemtbl;
static sceHiPlugMicroData_t *microD[DATA_NUM];
static sceHiPlug		*hrchyP[DATA_NUM];

static sceHiPlug		*shapeP0;
static sceHiPlug		*skinP0;
static sceHiPlugHrchyHead_t	*hrchyH0;

static sceHiPlugShapeHead_t	*skinmh;		/* matrix header */
static sceHiPlugSkinHead_t	*skindh;		/* skin_data header */


static sceHiPlugHrchyPreCalcArg_t	hrchy_arg1;
static sceVu0FMATRIX root_mat1;

static void myinit(sceHiPlug *p)
{
    const sceHiType t[2]={
	{					/* base_matrix */
	    SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_SHAPE, 
	    SCE_HIG_DATA_STATUS, SCE_HIP_BASEMATRIX, SCE_HIP_REVISION
	},
	{					/* skin_data */
	    SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_SKIN, 
	    SCE_HIG_DATA_STATUS, SCE_HIP_SKIN_DATA, SCE_HIP_REVISION
	},
    };


    skinmh = sceHiPlugShapeGetHead(p, t[0]);
    if (skinmh==NULL) error_handling(__FILE__,__LINE__,"Unexpected Skin Plugin data (lack of BASEMATRIX Data Block)");

    skindh = sceHiPlugSkinGetHead(p, t[1]);
    if (skindh==NULL) error_handling(__FILE__,__LINE__,"Unexpected Skin Plugin data (lack of SKIN_DATA Data Block)");


}

void SAMPLE31_init(void)
{
    sceHiErr	err;
    sceHiType	type;
    int i;

    type.repository = SCE_HIP_COMMON;
    type.project = SCE_HIP_FRAMEWORK;

    gsmemtbl=sceHiGsMemAlloc(SCE_HIGS_PAGE_ALIGN, sceHiGsMemRestSize());
    if(gsmemtbl==NULL) error_handling(__FILE__,__LINE__,"cannot alloc gsmem");

    for (i=0; i<DATA_NUM; i++)
	DATA_ADDR[i] = file_read((char *)files[i]);

    for (i=0; i<DATA_NUM; i++)
	if ((err = sceHiParseHeader((u_int *)DATA_ADDR[i])) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


    for (i=0; i<DATA_NUM; i++)
	if ((err = sceHiGetPlug((u_int *)DATA_ADDR[i], "Frame", &frameP[i])) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    sceHiGetPlug((u_int *)DATA_ADDR[0], "Shape", &shapeP0);

    for (i=0; i<DATA_NUM; i++) {
	tex2d_arg[i].resident = FALSE;		/* non-resident */ 
	tex2d_arg[i].tbl = gsmemtbl;			/* non-resident (cache) area */ 
	if ((err = sceHiGetPlug((u_int *)DATA_ADDR[i], "Tex2d", &tex2dP)) == SCE_HIG_NO_ERR){
	    tex2dP->args = (u_int)&tex2d_arg[i];
	}
    }


    type.category = SCE_HIP_SKIN;
    type.status = SCE_HIG_PLUGIN_STATUS;
    type.id = SCE_HIP_SKIN_PLUG;
    type.revision = SCE_HIP_REVISION;
    if ((err = sceHiGetInsPlug(frameP[0], &skinP0, type)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);



    for (i=0; i<DATA_NUM; i++)
	if ((err = sceHiGetPlug((u_int *)DATA_ADDR[i], "Hrchy", &hrchyP[i])) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    for (i=0; i<DATA_NUM; i++) {
	if ((err = sceHiGetPlug((u_int *)DATA_ADDR[i], "Micro", &microP[i])) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if((microD[i] = sceHiPlugMicroGetData(microP[i])) == NULL)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    for (i=0; i<DATA_NUM; i++)
	if ((err = sceHiCallPlug(frameP[i], SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


    for (i=0; i<DATA_NUM; i++){

	sceVu0FVECTOR p, r, ii, u;


	camerainit(microD[i]);


	viewget(p,r,ii,u);
	r[0] = PI;
	r[1] = PI;
	viewset(p,r,ii,u);
	cameraview(microD[i]);


	lightinit(microD[i]);
	microP[i]->args = (u_int)&micro_pre_arg;
    }


    myinit(skinP0);

    type.category = SCE_HIP_HRCHY;
    type.status = SCE_HIG_DATA_STATUS;
    type.id = SCE_HIP_HRCHY_DATA;
    type.revision = SCE_HIP_REVISION;
    hrchyH0 = sceHiPlugHrchyGetHead(hrchyP[0], type);
    if(hrchyH0 == NULL)	error_handling(__FILE__, __LINE__, sceHiGetErrStatePtr()->mes);

    sceVu0UnitMatrix(root_mat1);

    hrchyP[0]->args = (u_int)&hrchy_arg1;
    hrchy_arg1.root= &root_mat1;
}

#define MAXMODE 1
static void bonectrl(sceHiPlugHrchyHead_t *hH, sceHiPlugShapeHead_t *mh);


int SAMPLE31_main(void)
{


    sceHiErr	err;
    static int mode = 1;
    int i;

    if (gPad.SelectSwitch)
	mode = (++mode) > MAXMODE ? 0 : mode;

    switch(mode){
      case 0:
	  cameractrl();
	  for (i=0; i<DATA_NUM; i++)
	      cameraview(microD[i]);
	  break;
      case 1:
	  bonectrl(hrchyH0, skinmh);
	  break;
    }


    for (i=0; i<DATA_NUM; i++)
	if ((err = sceHiCallPlug(frameP[i], SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


    for (i=0; i<DATA_NUM; i++)
	if ((err = sceHiCallPlug(frameP[i], SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


    MatrixDispSet(skinmh, microD[0]);

    utilSetDMATimer();
    if ((err = sceHiDMASend()) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if ((err = sceHiDMAWait()) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (gPad.StartSwitch)
	return 1;


    return 0;
}

void SAMPLE31_exit(void)
{
    sceHiErr	err;
    int i;

    for (i=0; i<DATA_NUM; i++)
	if ((err = sceHiCallPlug(frameP[i], SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    sceHiGsMemFree(gsmemtbl);

    for (i=0; i<DATA_NUM; i++)
	free(DATA_ADDR[i]);

}


static void bonectrl(sceHiPlugHrchyHead_t *hH, sceHiPlugShapeHead_t *mh)
{
    float  dr = 1.5f / 180.0f * PI;
    float  dt = 0.03f;
    sceHiPlugHrchyData_t *d;
    sceVu0FMATRIX *m;
    int i;
    static int idx = 3;

    m = (sceVu0FMATRIX *)(sceHiPlugShapeGetMatrix(mh, idx)->local);

    d = sceHiPlugHrchyGetData(hH, idx);

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

    if (gPad.L1On) {
	d->trans[1] -= dt;
    }
    if(gPad.L2On) {
	d->trans[1] += dt;
    }

    if (gPad.R1On) {
	d->trans[0] -= dt;
    }
    if(gPad.R2On) {
	d->trans[0] += dt;
    }


    if(gPad.RrightSwitch){
	idx += 1;
	if(idx >= mh->top.num) idx = mh->top.num-1;

    }
    if(gPad.RleftSwitch){
	idx -= 1;
	if(idx < 0) idx = 0;
    }

    draw_debug_print(1, 6, "BONE %d: %5.2f %5.2f %5.2f",idx,d->rot[0],d->rot[1],d->rot[2]);

    for(i=0;i<4;i++){
	draw_debug_print(1,8+i,"%5.2f %5.2f %5.2f %5.2f\n",(*m)[i][0],(*m)[i][1],(*m)[i][2],(*m)[i][3]);
    }

}
