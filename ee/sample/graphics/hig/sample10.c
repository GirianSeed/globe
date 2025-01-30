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
#include <malloc.h>
#include <eeregs.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>
#include "util.h"
#include "camera.h"
#include "light.h"

/***************************************************
 * sample program sample10
 *
 * 	double-buffering sample (part 1)
 * 
 *	keep two basematrix-blocks, 
 * 	  one (the original) for CPU access, one (the shadow)for DMA transfer
 * 
 *  a1) set basematrix by CPU in the original area
 *  b1) copy it to the shadow area
 *  c1) transfer the  shadow area with DMA(draw object), 
 *		while a2) set basematrix by CPU for the next draw
 *  b2) copy it to the shadow area
 *  c2) transfer the  shadow area with DMA(draw object), 
 *		while a3) set basematrix by CPU for the next draw
 *
 *	  ...
 *
 *	If other data blocks are updated every draw, they also need to be doubled.
 * 	Possible ones are such as share data, and shape data with skin animation.
 *
 *	To avoid copying costs, see sample25 and sample27.
 * 
 *	additional feature: Animation control with pad L1 & L2
 ***************************************************/

typedef struct {
    int pad0, pad1, pad2, shapeid;
    sceVu0FMATRIX lw;
    sceVu0FMATRIX light;
} basem_mat_t;

typedef struct {
    int pad0, pad1, pad2, num;
    basem_mat_t matrix[1];
} basem_data_t;


typedef struct {
	basem_data_t	*data;
	sceHiData	*higdata;
} _higbasem_data_t;

static _higbasem_data_t basem_org, basem_shadow;

static void	*DATA_ADDR;

static sceHiPlug	*frameP;
static sceHiPlug	*microP;
static sceHiPlug	*shapeP;
static sceHiPlug	*tex2dP;
static sceHiPlug	*animeP;

static sceHiPlugTex2dInitArg_t	tex2d_arg;
static sceHiPlugAnimePreCalcArg_t	anime_pre_arg;
static sceHiPlugMicroPreCalcArg_t	micro_pre_arg;

static sceHiPlugMicroData_t *microD;

static void *_duplicate_data(void *addr, int qwsize)
{
    int i;
    sceVu0FVECTOR *src, *dst;

    dst=(sceVu0FVECTOR *)memalign(16, qwsize*sizeof(u_long128));
    if (dst==NULL) return NULL;

    src=(sceVu0FVECTOR *)addr;
    
    for (i=0; i<qwsize; i++){
	sceVu0CopyVector(dst[i], src[i]);
    }

    return (void *)dst;
}

/* copy basematrix data from original(for CPU) to shadow(for DMA) */
static void _copy_basem()
{
    int i;


    for (i=0; i<basem_org.data->num; i++){
	sceVu0CopyMatrix(basem_shadow.data->matrix[i].lw, basem_org.data->matrix[i].lw);
	sceVu0CopyMatrix(basem_shadow.data->matrix[i].light, basem_org.data->matrix[i].light);
    }
}

void SAMPLE10_init(void)
{
	sceHiErr	err;
	sceHiType	type;
	sceHiList *list;



	DATA_ADDR = file_read("data/APEWALK39.bin");

	if ((err = sceHiParseHeader((u_int *)DATA_ADDR)) != SCE_HIG_NO_ERR) {
		error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	}

	if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Frame", &frameP)) != SCE_HIG_NO_ERR) {
		error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	}

	/*	get anime-plugin block from the data	*/
	if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Anime", &animeP)) != SCE_HIG_NO_ERR){
		error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	}

	/* start replacing a basematrix block in the shape plugin */
	/* get shape-plugin block from the data */
	if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Shape", &shapeP)) != SCE_HIG_NO_ERR) {
		error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	}

	/* get basematrix block from the shape-plugin block */
	type.repository = SCE_HIP_COMMON;
	type.project	= SCE_HIP_FRAMEWORK;
	type.revision	= SCE_HIP_REVISION;

	type.category 	= SCE_HIP_SHAPE;
	type.id		= SCE_HIP_BASEMATRIX;
	err = sceHiGetData(shapeP, (u_int **)&basem_org.data, type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

	err = sceHiGetList(shapeP, &list, type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	basem_org.higdata=(sceHiData *)list->addr;

	/* remove the original basematrix from shape plugin */
	err = sceHiRmvDataBlk(shapeP, basem_org.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

	/* duplicate basematrix data */
	basem_shadow.data = _duplicate_data(basem_org.data, 
			     (basem_org.data->num*sizeof(basem_mat_t)/sizeof(u_long128))+1);

	if (basem_shadow.data==NULL) error_handling(__FILE__,__LINE__,sceHiErrState.mes);

	/* create shadow-basematrix block */
	err = sceHiMakeDataBlk((u_int *)basem_shadow.data, &basem_shadow.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

	/* add shadow-basematrix block to the shape-plugin */
	err = sceHiAddDataBlk(shapeP, basem_shadow.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

	/* finish replacing a basematrix block */

	/* only when Tex2d data exists */
	if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Tex2d", &tex2dP)) == SCE_HIG_NO_ERR){
	    tex2d_arg.resident = TRUE;
	    tex2d_arg.tbl = NULL;
	    tex2dP->args = (u_int)&tex2d_arg;
	}

	err = sceHiCallPlug(frameP, SCE_HIG_INIT_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiErrState.mes);

	/* for camera */
	if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Micro", &microP)) != SCE_HIG_NO_ERR) {
		error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	}
	if((microD = sceHiPlugMicroGetData(microP)) == NULL)
	    error_handling(__FILE__,__LINE__,sceHiErrState.mes);

	camerainit(microD);
	lightinit(microD);

	/* animation frame control */
	anime_pre_arg.setframe_enable=0;	/* disabled */
	anime_pre_arg.currentframe=0;
	animeP->args = (u_int)&anime_pre_arg;

	/* micro code change? */
	micro_pre_arg.micro = 0;      
	microP->args = (u_int)&micro_pre_arg;

}

int SAMPLE10_main(void)
{
	int		frame_counter;
	sceHiErr	err;

	utilSetDMATimer();
	sceHiDMASend();

	/* processing CPU while transmitting DMA in the back */
	sceHiDMASwap();

	if(gPad.L1On){
	    frame_counter=anime_pre_arg.currentframe+1;

	    anime_pre_arg.setframe=frame_counter;
	    anime_pre_arg.setframe_enable=1;	/* stop free run and enable setframe */

	    gPad.L1On = FALSE;
	}
	if(gPad.L2On){
	    frame_counter=anime_pre_arg.currentframe-1;
	    if(frame_counter < 0) frame_counter = 0;

	    anime_pre_arg.setframe=frame_counter;
	    anime_pre_arg.setframe_enable=1;	/* stop free run and enable setframe */

	    gPad.L2On = FALSE;
	}

	/* camear moving/set */
	cameractrl();
	cameraview(microD);

	if ((err = sceHiCallPlug(frameP, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR) {
		error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	}


	if ((err = sceHiCallPlug(frameP, SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR) {
		error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	}


	draw_debug_print(1,10,"Animation Frame : %d  [L1/L2]", anime_pre_arg.currentframe);


	/* transmit only 1st */
	micro_pre_arg.micro = -1;
	
	/* controll print */
	draw_debug_print(50, 1, "Rup/Rdown    : Cam Up/Down");
	draw_debug_print(50, 2, "Rleft/Rright : Cam Left/Right");
	draw_debug_print(50, 3, "R1/R2        : Cam Back/Front");
	draw_debug_print(50, 4, "Lup/Ldown    : Cam Rot X");
	draw_debug_print(50, 5, "Lleft/Lright : Cam Rot Y");
	draw_debug_print(50, 6, "L1/L2        : Animation");
	draw_debug_print(50, 7, "START        : EXIT");

	/* wait DMA finish */
	/* _copy_basem() is concurrent operation impossibliity with DMA, */
	/* then, should wait DMA here */
	sceHiDMAWait();

	/* copy basematrix data from original(for CPU) to shadow(for DMA) */
	_copy_basem();

	if (gPad.StartSwitch)
		return 1;
	return 0;
}

void SAMPLE10_exit(void)
{
	sceHiErr	err;

	if ((err = sceHiCallPlug(frameP, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR) {
		error_handling(__FILE__,__LINE__,sceHiErrState.mes);
	}

	sceHiGsMemFree(tex2d_arg.tbl);
	
	sceHiMemFree(basem_shadow.higdata);

	free(DATA_ADDR);
	free(basem_shadow.data);
}
