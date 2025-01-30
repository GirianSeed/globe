/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
*/
/*
 *
 *	Copyright (C) 2002 Sony Computer Entertainment Inc.
 *							All Right Reserved
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            1.0     Feb 10, 2002     aoki      sample for slow frame rate 
 */
#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <eeregs.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include "util.h"
#include "camera.h"
#include "light.h"


/*******************************************************
 * sample27
 *	sample for slow frame rate 
 *******************************************************/
static void display_mode_noninter();
static void display_mode_inter_448();
static void display_mode_inter_224();

static struct {
    void (*func)(void);
    char name[32];
} display_mode_list[]=
{
    { display_mode_inter_448, "INTERLACE, V=448"},
    { display_mode_inter_224, "INTERLACE, V=224"},
    { display_mode_noninter, "NON INTERLACE, V=224"},
};
static int display_mode;
#define DISPLAY_MODE_NUM 3

#define INSTANCES 15
#define DATA_FILE  "data/APEWALK39.bin"

void SAMPLE27_init();
int SAMPLE27_main();
void SAMPLE27_exit();

static void	*DATA_ADDR;
static sceHiPlug *frameP;
static sceHiPlug *shapeP;
static sceHiPlug *hrchyP;
static sceHiPlugMicroData_t	*microD;

static sceHiPlugTex2dInitArg_t	tex2d_arg;
static sceHiPlugMicroPreCalcArg_t	micro_arg;
static sceHiPlugHrchyPreCalcArg_t	hrchy_arg;

static sceVu0FMATRIX root_mat={
    { 1.0f, 0.0f, 0.0f, 0.0f,}, 
    { 0.0f, 1.0f, 0.0f, 0.0f,}, 
    { 0.0f, 0.0f, 1.0f, 0.0f,}, 
    { 0.0f, 10.0f, 50.0f, 1.0f,},
}; 

static sceVu0FMATRIX clone_root_mat[INSTANCES];
static sceHiPlug *shapesP[INSTANCES];
static sceHiData *basemBlk[INSTANCES];
static u_int  *basemDat[INSTANCES];

static void sample27_camerainit(sceHiPlugMicroData_t *md, float vres)
{
    static sceVu0FVECTOR s = {640.0f, 224.0f, 512.0f, 300.0f};
    static sceVu0FVECTOR w = {1.0f, 0.47f, 2048.0f, 2048.0f};
    static sceVu0FVECTOR d = {5.0f, 16777000.0f, 1.0f, 65536.0f};
    static sceVu0FVECTOR p = { 0.0f, 0.0f, -30.0f, 0.0f };
    static sceVu0FVECTOR r = { 0.0f, 0.0f, 0.0f, 0.0f };
    static sceVu0FVECTOR i = { 0.0f, 0.0f, 0.0f, 0.0f };
    static sceVu0FVECTOR u = { 0.0f, 1.0f, 0.0f, 0.0f };
    w[1]=0.47f*vres;

    perspective(md,s,w,d);
    viewset(p,r,i,u);
    cameraview(md);
}

static void display_mode_noninter()
{
    sceHiErr	err;
    int		odev;

    odev = !sceGsSyncV(0);
    err = sceHiGsDisplaySwap(odev);	

    sample27_camerainit(microD, 1.0f);

    /* change display mode */
    if (sceHiGsDisplayMode(SCE_HIGS_RESET|SCE_HIGS_NTSC|SCE_HIGS_RGBA|SCE_HIGS_DEPTH24)
	!= SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    if (sceHiGsDisplaySize(640, 224)!= SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    err = sceHiGsDisplaySwap(odev);	
    if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    err = sceHiGsDisplaySwap(1-odev);
    if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
}

static void display_mode_inter_448()
{
    sceHiErr	err;
    int		odev;

    odev = !sceGsSyncV(0);
    err = sceHiGsDisplaySwap(odev);	

    sample27_camerainit(microD, 2.0f);


    /* change display mode */
    if (sceHiGsDisplayMode(SCE_HIGS_RESET|SCE_HIGS_NTSCIH|SCE_HIGS_RGBA|SCE_HIGS_DEPTH24)
	!= SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    if (sceHiGsDisplaySize(640, 448)!= SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);


    err = sceHiGsDisplaySwap(odev);	
    if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    err = sceHiGsDisplaySwap(1-odev);
    if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
}

static void display_mode_inter_224()
{
    sceHiErr	err;
    int		odev;

    odev = !sceGsSyncV(0);
    err = sceHiGsDisplaySwap(odev);	

    sample27_camerainit(microD, 1.0f);

    /* change display mode */
    if (sceHiGsDisplayMode(SCE_HIGS_RESET|SCE_HIGS_NTSCI|SCE_HIGS_RGBA|SCE_HIGS_DEPTH24)
	!= SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    if (sceHiGsDisplaySize(640, 224)!= SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    err = sceHiGsDisplaySwap(odev);	
    if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    err = sceHiGsDisplaySwap(1-odev);
    if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
}


static void alloc_shapesP()
{
    sceHiType type;
    sceHiErr err;
    sceHiList *list;
    u_int *orig_basemD;
    u_int size;
    int i;


    type.repository = SCE_HIP_COMMON;
    type.project = SCE_HIP_FRAMEWORK;
    type.revision = SCE_HIP_REVISION;
    type.status		= SCE_HIG_DATA_STATUS;	       
    type.category 	= SCE_HIP_SHAPE;
    type.id		= SCE_HIP_BASEMATRIX;
    err = sceHiGetData(shapeP, &orig_basemD, type);
    err = sceHiGetList(shapeP, &list, type);
    size= ((sceHiData *)(list->addr))->size;
    size--; /*header */
    size*=16; /*bytes */



    for (i=0;i<INSTANCES;i++) {
	basemDat[i]=(u_int *)memalign(16,size);
	if (basemDat[i]==NULL) error_handling(__FILE__,__LINE__,"malloc error");
	memcpy(basemDat[i],orig_basemD,size);


	type.status = SCE_HIG_PLUGIN_STATUS;    
	type.category	= SCE_HIP_SHAPE;
	type.id		= SCE_HIP_SHAPE_PLUG;
	/* allocate new shape-plugin */
	err = sceHiNewPlugBlk(0, 3, &shapesP[i], &type);  
	if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);


	/* allocate BASEMATRIX data-block */
	type.status		= SCE_HIG_DATA_STATUS;	       
	type.id		= SCE_HIP_BASEMATRIX;
	err = sceHiMakeDataBlk(basemDat[i], &(basemBlk[i]), &type);    
	if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);

	err = sceHiAddDataBlk(shapesP[i], basemBlk[i]); /* add basematrix-block */
	if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);

	type.id		= SCE_HIP_SHAPE_DATA;
	err = sceHiGetList(shapeP, &list, type); 
	if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);

	err = sceHiAddDataBlk(shapesP[i], (sceHiData *)list->addr); /* share shape-data */
	if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);

	type.category	= SCE_HIP_TEX2D;
	type.id		= SCE_HIP_TEX2D_ENV;
	err = sceHiGetList(shapeP, &list, type); 
	if (err == SCE_HIG_NO_ERR){
	     /* share tex2d env-data */
	    err=sceHiAddDataBlk(shapesP[i], (sceHiData *)list->addr);
	    if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	} else{
	    type.id = SCE_HIP_TEX2D_DATA;
	    err = sceHiGetList(shapeP, &list, type); 
	    if (err == SCE_HIG_NO_ERR){ /* for old format data without tex2d_env */
		/* share tex2d-data */
		err=sceHiAddDataBlk(shapesP[i], (sceHiData *)list->addr); 
		if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	    }
	}

	err = sceHiSetPluginApi(shapesP[i]);	
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    }
  
}



#define PI      3.14159265f
static void init_clone_root_mat()
{
    static sceVu0FMATRIX rotm={
	{ 1.0f, 0.0f, 0.0f, 0.0f,}, 
	{ 0.0f, 1.0f, 0.0f, 0.0f,}, 
	{ 0.0f, 0.0f, 1.0f, 0.0f,}, 
	{ 0.0f, 0.0f, 0.0f, 1.0f,},
    }; 
    static sceVu0FVECTOR trans={0.0f, 0.0f, 0.0f, 0.0f};
    int i;
    float n;
    float a = 2.0f * PI / INSTANCES;         /* angle for trans */
    //  float r = 14.0f;                    /* radius for trans */
    float r = 40.0f;                    /* radius for trans */

    for(i=0, n=0.0f; i<INSTANCES; i++, n++){    
	trans[0] = cosf(a * n) * r; /* X pos */
	trans[2] = sinf(a * n) * r; /* Z pos */
	sceVu0CopyMatrix(clone_root_mat[i], rotm);
	sceVu0TransMatrix(clone_root_mat[i],clone_root_mat[i],trans);    
    }
}

static void free_shapesP() 
{
    int i;

    for (i=0;i<INSTANCES;i++) {
	sceHiMemFree(shapesP[i]);
	sceHiMemFree(basemBlk[i]);
	free(basemDat[i]);
    }
}

                   

static void clone_basematrix(int dst_index)
{
    sceHiPlugShapeHead_t *hsrc,*hdst;
    sceHiPlugShapeMatrix_t *msrc,*mdst;
    sceHiType type;
    int i;

    type.repository = SCE_HIP_COMMON;
    type.project = SCE_HIP_FRAMEWORK;
    type.revision = SCE_HIP_REVISION;
    type.status		= SCE_HIG_DATA_STATUS;	       
    type.category 	= SCE_HIP_SHAPE;
    type.id		= SCE_HIP_BASEMATRIX;

    hsrc=sceHiPlugShapeGetHead(shapeP,type);
    hdst=sceHiPlugShapeGetHead(shapesP[dst_index],type);

    for(i=0;i<hsrc->top.num;i++){
	msrc=sceHiPlugShapeGetMatrix(hsrc,i);
	if (msrc->shape != -1) {
	    mdst=sceHiPlugShapeGetMatrix(hdst,i);
	    sceVu0MulMatrix(mdst->local,clone_root_mat[dst_index],msrc->local);
	    sceVu0MulMatrix(mdst->light,clone_root_mat[dst_index],msrc->light); 
	}
    }
}

void SAMPLE27_init(void)
{
    sceHiErr	err;
    sceHiType	type;
    int i;
    sceHiPlug	*microP;
    sceHiPlug	*tex2dP;

    /* alloc frame buffer at biggest size */
    if (sceHiGsDisplayMode(SCE_HIGS_RESET|SCE_HIGS_NTSCIH|SCE_HIGS_RGBA|SCE_HIGS_DEPTH24)
	!= SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    if (sceHiGsDisplaySize(640, 448)!= SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    err = sceHiGsDisplaySwap(0);	
    if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    err = sceHiGsDisplaySwap(1);
    if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    tex2d_arg.tbl = sceHiGsMemAlloc(SCE_HIGS_PAGE_ALIGN, 
				    SCE_HIGS_PAGE_ALIGN*sceHiGsMemRestSize()/SCE_HIGS_PAGE_ALIGN);
    if (tex2d_arg.tbl==NULL) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    tex2d_arg.resident = FALSE;

    init_clone_root_mat();

    DATA_ADDR = file_read(DATA_FILE);


    if ((err = sceHiParseHeader((u_int *)DATA_ADDR)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Frame", &frameP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Shape", &shapeP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Hrchy", &hrchyP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Tex2d", &tex2dP)) == SCE_HIG_NO_ERR) {
	tex2dP->args = (u_int)&tex2d_arg;
    } else
	tex2dP = NULL;

    alloc_shapesP(); /* creates shape-plug-ins */

    if ((err = sceHiCallPlug(frameP, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    sceHiPlugShapeMasterChainSetting(shapeP,1);


    for (i=0;i<INSTANCES;i++){
	if ((err = sceHiCallPlug(shapesP[i], SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	sceHiPlugShapeMasterChainSetting(shapesP[i],1); 
    }

    /* for camera */
    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Micro", &microP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    type.repository = SCE_HIP_COMMON;
    type.project = SCE_HIP_FRAMEWORK;
    type.category = SCE_HIP_MICRO;
    type.status = SCE_HIG_DATA_STATUS;
    type.id = SCE_HIP_MICRO_DATA;
    type.revision = SCE_HIP_REVISION;
    if ((err = sceHiGetData(microP, (u_int **)&microD, type)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    /* micro code change? */
    micro_arg.micro = 0;      
    microP->args = (u_int)&micro_arg;


    hrchy_arg.root= &root_mat;
    hrchyP->args = (u_int)&hrchy_arg;

    display_mode=0; /* interlace, v=448 */
    display_mode_list[display_mode].func();
    lightinit(microD);
}

int SAMPLE27_main(void)
{
    sceHiErr	err;
    int i;
 
    /* camear moving/set */
    cameractrl();
    cameraview(microD);


    if ((err = sceHiCallPlug(frameP, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    /* copy basematrix from original to clones */
    for (i=0;i<INSTANCES;i++){
	clone_basematrix(i); 
    }

    if ((err = sceHiCallPlug(frameP, SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    for (i=0;i<INSTANCES;i++){
	if ((err = sceHiCallPlug(shapesP[i], SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    }

    /* transmit only the first */
    micro_arg.micro = -1;

    utilSetDMATimer();
    if ((err = sceHiDMASend()) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    if ((err = sceHiDMAWait()) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    draw_debug_print(1, 10, "display mode: %s  [SELECT]", display_mode_list[display_mode].name);
    if (gPad.SelectSwitch) {
	display_mode++;
	if (display_mode>=DISPLAY_MODE_NUM) display_mode=0;
	display_mode_list[display_mode].func();
    }

    if (gPad.StartSwitch)
	return 1;
    return 0;
}

void SAMPLE27_exit(void)
{
    sceHiErr	err;
    int i;

    /* revert display size */
    if (sceHiGsDisplayMode(SCE_HIGS_RESET|SCE_HIGS_NTSCI|SCE_HIGS_RGBA|SCE_HIGS_DEPTH24)
	!= SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    if (sceHiGsDisplaySize(640, 224)!= SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    err = sceHiGsDisplaySwap(0);	
    if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    err = sceHiGsDisplaySwap(1);
    if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);


    /* finish model data */
    if ((err = sceHiCallPlug(frameP, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	
    for (i=0;i<INSTANCES;i++){
	if ((err = sceHiCallPlug(shapesP[i], SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    }

    free_shapesP();
    free(DATA_ADDR);

    /* free GS texture memory */
    err=sceHiGsMemFree(tex2d_arg.tbl);
    if (err!=SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);

}
