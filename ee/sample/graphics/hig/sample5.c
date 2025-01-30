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
 * sample program 5
 *
 *	A sample of primitives
 ***************************************************/

static const char *files[]={
    "data/point.bin",			/* point primitive */
    "data/line.bin",			/* line primitive */
    "data/linestrip.bin",		/*  line strip primitive */
    "data/triangle.bin",		/*  triangle primitive */
    "data/tristrip.bin",		/*  triangle strip primitive */
    "data/trifan.bin",			/* triangle fan primitive */    
    "data/point_t.bin",			/* point primitive w/texture */
    "data/line_t.bin",			/* line primitive w/texture */
    "data/linestrip_t.bin",		/*  line strip primitive w/texture */
    "data/triangle_t.bin",		/*  triangle primitive w/texture */
    "data/tristrip_t.bin",		/*  triangle strip primitive w/texture */
    "data/trifan_t.bin",		/* triangle fan primitive w/texture */
};
#define DATA_NUM 12

static void	*DATA_ADDR[DATA_NUM];

static sceHiPlug	*frameP[DATA_NUM];
static sceHiPlug	*microP[DATA_NUM];
static sceHiPlug	*tex2dP;

static sceHiPlugTex2dInitArg_t	tex2d_arg[DATA_NUM];
static sceHiGsMemTbl 	*tbl;
static sceHiPlugMicroData_t *microD[DATA_NUM];

void SAMPLE5_init(void)
{
	int 		i;
	sceHiErr	err;

	/* Construct the same area across every Tex2D here, */
	/* and use it by non resident */
  	tbl=sceHiGsMemAlloc(SCE_HIGS_PAGE_ALIGN, sceHiGsMemRestSize());
	if(tbl==NULL) error_handling(__FILE__,__LINE__,"cannot alloc gsmem");

	for (i=0; i<DATA_NUM; i++)
	    DATA_ADDR[i] = file_read((char *)files[i]);

	for (i=0; i<DATA_NUM; i++)
	    if ((err = sceHiParseHeader((u_int *)DATA_ADDR[i])) != SCE_HIG_NO_ERR)
		error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	for (i=0; i<DATA_NUM; i++)
	    if ((err = sceHiGetPlug((u_int *)DATA_ADDR[i], "Frame", &frameP[i])) != SCE_HIG_NO_ERR)
		error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	for (i=0; i<DATA_NUM; i++) {
	    tex2d_arg[i].resident = FALSE;		/* non-resident */ 
	    tex2d_arg[i].tbl = tbl;			/* non-resident (cache) area */ 
	    if ((err = sceHiGetPlug((u_int *)DATA_ADDR[i], "Tex2d", &tex2dP)) == SCE_HIG_NO_ERR){
		tex2dP->args = (u_int)&tex2d_arg[i];
	    }
	}

	for (i=0; i<DATA_NUM; i++)
	    if ((err = sceHiCallPlug(frameP[i], SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
		error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	for (i=0; i<DATA_NUM; i++) {
	    if ((err = sceHiGetPlug((u_int *)DATA_ADDR[i], "Micro", &microP[i])) != SCE_HIG_NO_ERR)
		error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	    if((microD[i] = sceHiPlugMicroGetData(microP[i])) == NULL)
		error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	    camerainit(microD[i]);
	    lightinit(microD[i]);
	}
}

int SAMPLE5_main(void)
{
 	int 		i;
	sceHiErr	err;

	cameractrl();
	cameraview(microD[0]);

	for (i=1; i<DATA_NUM; i++){
	    cameracopy(microD[i], microD[0]);
	}

	for (i=0; i<DATA_NUM; i++)
	    if ((err = sceHiCallPlug(frameP[i], SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
		error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	for (i=0; i<DATA_NUM; i++)
	    if ((err = sceHiCallPlug(frameP[i], SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR)
		error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	utilSetDMATimer();
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

void SAMPLE5_exit(void)
{
	int		i;
	sceHiErr	err;

	for (i=0; i<DATA_NUM; i++)
	    if ((err = sceHiCallPlug(frameP[i], SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
		error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	sceHiGsMemFree(tbl);

	for (i=0; i<DATA_NUM; i++){
	    free(DATA_ADDR[i]);
	}
}
