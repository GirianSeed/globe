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
#include <malloc.h>
#include <string.h>
#include <math.h>
#include "util.h"
#include "camera.h"
#include "light.h"

/*******************************************************
 * Clone Sample
 *
 * A sample which creates copies of a shape-plug and assignes 
 * different animtion to each copy 
 *******************************************************/

#define INSTANCES 40
/* number of add. animation */ 
#define NANIM 2
static void	*DATA_ADDR;
static void	*ANIM_DATA;

/*	original plugblk	*/
static sceHiPlug	*frameP;
static sceHiPlug	*shapeP;
static sceHiPlug	*hrchyP;
static sceHiPlug	*microP;
static sceHiPlug	*tex2dP;
static sceHiPlugMicroData_t	*microD;

static sceHiPlugTex2dInitArg_t	tex2d_arg;
static sceHiPlugMicroPreCalcArg_t	arg;

static sceVu0FMATRIX root_mat[INSTANCES];

static sceHiPlug *shapesP[INSTANCES];
static sceHiData *basemBlk[INSTANCES];
static u_int  *basemDat[INSTANCES];
static sceHiPlug *hrchiesP[NANIM];
static sceHiData *hrchyBlk[NANIM];
static u_int *hrchyDat[NANIM];
static sceHiPlug *animesP[NANIM];


static void alloc_shapesP(void)
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
	if (basemDat[i]==NULL) printf("alloc-error\n");
	memcpy(basemDat[i],orig_basemD,size);


	type.status = SCE_HIG_PLUGIN_STATUS;    
	type.category	= SCE_HIP_SHAPE;
	type.id		= SCE_HIP_SHAPE_PLUG;
	/* allocate new shape-plugin */
	err = sceHiNewPlugBlk(0, 3, &shapesP[i], &type);  


	/* allocate BASEMATRIX data-block */
	type.status		= SCE_HIG_DATA_STATUS;	       
	type.id		= SCE_HIP_BASEMATRIX;
	err = sceHiMakeDataBlk(basemDat[i], &(basemBlk[i]), &type);    
	err = sceHiAddDataBlk(shapesP[i], basemBlk[i]); /* add basematrix-block */

	type.id		= SCE_HIP_SHAPE_DATA;
	err = sceHiGetList(shapeP, &list, type); 
	err = sceHiAddDataBlk(shapesP[i], (sceHiData *)list->addr); /* share shape-data */

	/* for HiG-data without texenv-data */
	type.category	= SCE_HIP_TEX2D;
	type.id             = SCE_HIP_TEX2D_DATA;
	err = sceHiGetList(shapeP, &list, type); 
	/* for HiG-data with texenv-data */    
	if (err != SCE_HIG_NO_ERR) {
	    type.id             = SCE_HIP_TEX2D_ENV;
	    err = sceHiGetList(shapeP, &list, type);
	}
	if (err == SCE_HIG_NO_ERR) {
	    err = sceHiAddDataBlk(shapesP[i], (sceHiData *)list->addr); /* share tex2d-data */
	}
    
	err = sceHiSetPluginApi(shapesP[i]);	
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    }
  
}


static void alloc_hrchiesP()
{
    sceHiType type;
    sceHiErr err;
    sceHiList *list;
    u_int *orig_hrchyD;
    u_int size;
    int i;


    type.repository = SCE_HIP_COMMON;
    type.project = SCE_HIP_FRAMEWORK;
    type.revision = SCE_HIP_REVISION;
    type.status		= SCE_HIG_DATA_STATUS;	       
    type.category 	= SCE_HIP_HRCHY;
    type.id		= SCE_HIP_HRCHY_DATA;
    err = sceHiGetData(hrchyP, &orig_hrchyD, type);
    err = sceHiGetList(hrchyP, &list, type);
    size= ((sceHiData *)(list->addr))->size;
    size--; /*header */
    size*=16; /*bytes */



    for (i=0;i<NANIM;i++) {
	hrchyDat[i]=(u_int *)memalign(16,size);
	if (hrchyDat[i]==NULL) printf("alloc-error\n");
	memcpy(hrchyDat[i],orig_hrchyD,size);


	type.status = SCE_HIG_PLUGIN_STATUS;
	type.id		= SCE_HIP_HRCHY_PLUG;
	/* allocate new hrchy-plugin */
	err = sceHiNewPlugBlk(0, 3, &hrchiesP[i], &type);  


	/* allocate hrchy data-block */
	type.status		= SCE_HIG_DATA_STATUS;
	type.id		= SCE_HIP_HRCHY_DATA;
	err = sceHiMakeDataBlk(hrchyDat[i], &(hrchyBlk[i]), &type);
	err = sceHiAddDataBlk(hrchiesP[i], hrchyBlk[i]); /* add hrchy-block */
	err = sceHiAddDataBlk(hrchiesP[i], basemBlk[i]); /* add basematrix-block */
 
	type.id             = SCE_HIP_PIVOT_DATA;
	err = sceHiGetList(hrchyP, &list, type);
	if (err == SCE_HIG_NO_ERR) {
	    err = sceHiAddDataBlk(hrchiesP[i], (sceHiData *)list->addr); /* share pivot-data */
	}   
	err = sceHiSetPluginApi(hrchiesP[i]);	
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    }
  
}

static void init_root_mat()
{
    int i;
    sceVu0FVECTOR trans;
    float n;
    float a = 2.0f * PI / INSTANCES;         /* angle for trans */
    float r = 14.0f;                    /* radius for trans */

    sceVu0ScaleVector(trans,trans,0.0);    
    for(i=0, n=0.0f; i<INSTANCES; i++, n++){    
	trans[0] = cosf(a * n) * r; /* X pos */
	trans[2] = sinf(a * n) * r; /* Z pos */      
	sceVu0UnitMatrix(root_mat[i]);
	sceVu0TransMatrix(root_mat[i],root_mat[i],trans);    
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
    for (i=0;i<NANIM;i++){
	free(hrchyDat[i]);
    }
}

static void mycamerainit(sceHiPlugMicroData_t *md)
{
    sceVu0FVECTOR pos={0.40f,0.20f,-26.8f};
    sceVu0FVECTOR rot={-0.51f,-0.71f,0.0f,0.0f};
    sceVu0FVECTOR tag={0.0f,0.0f,0.0f,0.0f};
    sceVu0FVECTOR up={0.0f,1.0f,0.0f,0.0f};

    camerainit(md);
    viewset(pos,rot,tag,up);
    cameraview(md);
}
                   
static void update_clone_basematrix(int dst_index,int src_index)
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

    hsrc=sceHiPlugShapeGetHead(shapesP[src_index],type);
    hdst=sceHiPlugShapeGetHead(shapesP[dst_index],type);

    for(i=0;i<hsrc->top.num;i++){
	msrc=sceHiPlugShapeGetMatrix(hsrc,i);
	if (msrc->shape != -1) {
	    mdst=sceHiPlugShapeGetMatrix(hdst,i);
	    sceVu0MulMatrix(mdst->local,root_mat[dst_index],msrc->local);
	    sceVu0MulMatrix(mdst->light,root_mat[dst_index],msrc->light); 
	}
    }
}

static void add_hrchyD_to_animesP()
{
    sceHiList *oldhrchy;
    sceHiType type;
    sceHiErr  err;
    int i;
  
    type.repository = SCE_HIP_COMMON;
    type.project = SCE_HIP_FRAMEWORK;
    type.revision = SCE_HIP_REVISION;
    type.status		= SCE_HIG_DATA_STATUS;	       
    type.category 	= SCE_HIP_HRCHY;
    type.id		= SCE_HIP_HRCHY_DATA;
  
    for(i=0;i<NANIM;i++) {
	err = sceHiGetList(animesP[i], &oldhrchy, type);
	err = sceHiRmvDataBlk(animesP[i], (sceHiData *)oldhrchy->addr); /*rmv dummy hrchy */
	err = sceHiAddDataBlk(animesP[i], hrchyBlk[i]); /* add hrchy-block */
    }
}

void SAMPLE25_init(void)
{
    sceHiErr	err;
    int i;


    init_root_mat();
    DATA_ADDR = file_read("data/cubeman.bin");   /* has it-s own animation  */
    ANIM_DATA = file_read("data/animation.bin"); /* contains 2 additional animations */ 


    if ((err = sceHiParseHeader((u_int *)ANIM_DATA)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if ((err = sceHiGetPlug((u_int *)ANIM_DATA, "Anime1", &animesP[0])) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if ((err = sceHiGetPlug((u_int *)ANIM_DATA, "Anime2", &animesP[1])) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


    if ((err = sceHiParseHeader((u_int *)DATA_ADDR)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Frame", &frameP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Shape", &shapeP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Hrchy", &hrchyP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Tex2d", &tex2dP)) == SCE_HIG_NO_ERR) {
	tex2d_arg.resident = TRUE;
	tex2d_arg.tbl = NULL;
	tex2dP->args = (u_int)&tex2d_arg;
    } else
	tex2dP = NULL;

	
    alloc_shapesP(); /* creates shape-plug-ins */
    alloc_hrchiesP(); /* creates hrchy-plug-ins */
    add_hrchyD_to_animesP();/* add hrchy-data to the animation plug */


    if ((err = sceHiCallPlug(frameP, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    sceHiPlugShapeMasterChainSetting(shapeP,1);

    for (i=0;i<NANIM;i++){
	if ((err = sceHiCallPlug(animesP[i], SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiCallPlug(hrchiesP[i], SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    for (i=0;i<INSTANCES;i++){
	if ((err = sceHiCallPlug(shapesP[i], SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	sceHiPlugShapeMasterChainSetting(shapesP[i],1); 
    }

    if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Micro", &microP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if((microD = sceHiPlugMicroGetData(microP)) == NULL)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    mycamerainit(microD);
    lightinit(microD);
    arg.micro = 0;
}

int SAMPLE25_main(void)
{
    sceHiErr	err;
    int i;
 
    /* camear moving/set */
    cameractrl();
    cameraview(microD);

    /* micro code change? */
    microP->args = (u_int)&arg;


    if ((err = sceHiCallPlug(frameP, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
 

    /* calc basematrix for animation0 and animation1 */
    for (i=0;i<NANIM;i++){
	if ((err = sceHiCallPlug(hrchiesP[i], SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	if ((err = sceHiCallPlug(animesP[i], SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    for (i=0+NANIM;i<INSTANCES;i++){
	if(i%2==0) {
	    /* copy animation 0 result */
	    /* and multiply basmatrix with root_mat */
	    update_clone_basematrix(i,0); 
	} else {
	    /* copy animation 1 result */
	    /* and multiply basmatrix with root_mat */
	    update_clone_basematrix(i,1); 
	}
    }
	
    for (i=0;i<NANIM;i++) {
	update_clone_basematrix(i,i);
    }


    if ((err = sceHiCallPlug(frameP, SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    for (i=0;i<INSTANCES;i++){
	if ((err = sceHiCallPlug(shapesP[i], SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    /* transmit only the first */
    arg.micro = -1;

    utilSetDMATimer();
    if ((err = sceHiDMASend()) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if ((err = sceHiDMAWait()) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


    if (gPad.StartSwitch)
	return 1;
    return 0;
}

void SAMPLE25_exit(void)
{
    sceHiErr	err;
    int i;
    if ((err = sceHiCallPlug(frameP, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	

    for (i=0;i<NANIM;i++){
	if ((err = sceHiCallPlug(animesP[i], SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	if ((err = sceHiCallPlug(hrchiesP[i], SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    for (i=0;i<INSTANCES;i++){
	if ((err = sceHiCallPlug(shapesP[i], SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    free_shapesP();
    free(DATA_ADDR);
}
