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
#include <string.h>
#include <eeregs.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>
#include <libgraph.h>
#include "util.h"
#include "camera.h"
#include "light.h"

/***************************************************
 * sample program 7
 *
 *	A sample to interpret data of es2raw
 ***************************************************/

/* es2raw format structure */
typedef struct {
    u_int	*shape_data;
    u_int	*basematrix;
    u_int	*tex2d_data;
    u_int	*hrchy_data;
    u_int	*micro_data;
    u_int	*anime_data;
    u_int	*keyframe;
    u_int	*keyvalue;
    u_int	*share_data;
    u_int	*srcdstvertex;
    u_int	*srcdstnormal;
    u_int	*vertexindex;
    u_int	*normalindex;
    u_int	*sharevertex;
    u_int	*sharenormal;
} es2raw_t;

/* hig data structure */
typedef struct {
    sceHiData	*basem;
    sceHiData	*hrchy;
    sceHiData	*micro;
    sceHiData	*anim;
    sceHiData	*key;
    sceHiData	*val;
    sceHiData	*shape;
    sceHiData	*tex2d;
    sceHiData	*share;
    sceHiData	*srcdstV;
    sceHiData	*srcdstN;
    sceHiData	*Vindex;
    sceHiData	*Nindex;
    sceHiData	*shareV;
    sceHiData	*shareN;
} es2hig_t;

es2raw_t	*rawdata;
es2hig_t	higdata;
void		*DATA_ADDR;
sceHiPlug	*frameP, *microP, *animeP, *shapeP, *shareP, *tex2dP, *hrchyP;
static sceHiPlugMicroPreCalcArg_t	arg;
static sceHiPlugMicroData_t *microD;

void SAMPLE7_init(void)
{
    sceHiErr	err;
    sceHiType	type;

    DATA_ADDR = file_read("data/mbox_raw.bin");

    /* construct raw data */
    rawdata = (es2raw_t *)DATA_ADDR;
    if (rawdata->shape_data != NULL)
	rawdata->shape_data += (u_int) DATA_ADDR / 4;
    if (rawdata->basematrix != NULL)
	rawdata->basematrix += (u_int) DATA_ADDR / 4;
    if (rawdata->tex2d_data != NULL)
	rawdata->tex2d_data += (u_int) DATA_ADDR / 4;
    if (rawdata->hrchy_data != NULL)
	rawdata->hrchy_data += (u_int) DATA_ADDR / 4;
    if (rawdata->micro_data != NULL)
	rawdata->micro_data += (u_int) DATA_ADDR / 4;
    if (rawdata->anime_data != NULL)
	rawdata->anime_data += (u_int) DATA_ADDR / 4;
    if (rawdata->keyframe != NULL)
	rawdata->keyframe   += (u_int) DATA_ADDR / 4;
    if (rawdata->keyvalue != NULL)
	rawdata->keyvalue   += (u_int) DATA_ADDR / 4;
    if (rawdata->share_data != NULL)
	rawdata->share_data += (u_int) DATA_ADDR / 4;
    if (rawdata->srcdstvertex != NULL)
	rawdata->srcdstvertex += (u_int) DATA_ADDR / 4;
    if (rawdata->srcdstnormal != NULL)
	rawdata->srcdstnormal += (u_int) DATA_ADDR / 4;
    if (rawdata->vertexindex != NULL)
	rawdata->vertexindex += (u_int) DATA_ADDR / 4;
    if (rawdata->normalindex != NULL)
	rawdata->normalindex += (u_int) DATA_ADDR / 4;
    if (rawdata->sharevertex != NULL)
	rawdata->sharevertex += (u_int) DATA_ADDR / 4;
    if (rawdata->sharenormal != NULL)
	rawdata->sharenormal += (u_int) DATA_ADDR / 4;

    /* construct hig data */
    memset(&higdata, (int)NULL, sizeof(es2hig_t));

    type.repository	= SCE_HIP_COMMON;
    type.project	= SCE_HIP_FRAMEWORK;
    type.revision	= SCE_HIP_REVISION;
    type.status		= SCE_HIG_DATA_STATUS;

    if (rawdata->shape_data != NULL) {
	type.category	= SCE_HIP_SHAPE;
	type.id		= SCE_HIP_SHAPE_DATA;
	err = sceHiMakeDataBlk(rawdata->shape_data, &higdata.shape, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (rawdata->basematrix != NULL) {
	type.category 	= SCE_HIP_SHAPE;
	type.id		= SCE_HIP_BASEMATRIX;
	err = sceHiMakeDataBlk(rawdata->basematrix, &higdata.basem, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (rawdata->tex2d_data != NULL) {
	type.category	= SCE_HIP_TEX2D;
	type.id		= SCE_HIP_TEX2D_DATA;
	err = sceHiMakeDataBlk(rawdata->tex2d_data, &higdata.tex2d, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (rawdata->hrchy_data != NULL) {
	type.category	= SCE_HIP_HRCHY;
	type.id		= SCE_HIP_HRCHY_DATA;
	err = sceHiMakeDataBlk(rawdata->hrchy_data, &higdata.hrchy, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (rawdata->micro_data != NULL) {
	type.category	= SCE_HIP_MICRO;
	type.id		= SCE_HIP_MICRO_DATA;
	err = sceHiMakeDataBlk(rawdata->micro_data, &higdata.micro, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (rawdata->anime_data != NULL) {
	type.category	= SCE_HIP_ANIME;
	type.id		= SCE_HIP_ANIME_DATA;
	err = sceHiMakeDataBlk(rawdata->anime_data, &higdata.anim, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (rawdata->keyframe != NULL) {
	type.category	= SCE_HIP_ANIME;
	type.id		= SCE_HIP_KEYFRAME;
	err = sceHiMakeDataBlk(rawdata->keyframe, &higdata.key, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (rawdata->keyvalue != NULL) {
	type.category	= SCE_HIP_ANIME;
	type.id		= SCE_HIP_KEYVALUE;
	err = sceHiMakeDataBlk(rawdata->keyvalue, &higdata.val, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (rawdata->share_data != NULL) {
	type.category	= SCE_HIP_SHARE;
	type.id		= SCE_HIP_SHARE_DATA;
	err = sceHiMakeDataBlk(rawdata->share_data, &higdata.share, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (rawdata->srcdstvertex != NULL) {
	type.category	= SCE_HIP_SHARE;
	type.id		= SCE_HIP_SRCDSTVERTEX;
	err = sceHiMakeDataBlk(rawdata->srcdstvertex, &higdata.srcdstV, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (rawdata->srcdstnormal != NULL) {
	type.category	= SCE_HIP_SHARE;
	type.id		= SCE_HIP_SRCDSTNORMAL;
	err = sceHiMakeDataBlk(rawdata->srcdstnormal, &higdata.srcdstN, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (rawdata->vertexindex != NULL) {
	type.category	= SCE_HIP_SHARE;
	type.id		= SCE_HIP_VERTEXINDEX;
	err = sceHiMakeDataBlk(rawdata->vertexindex, &higdata.Vindex, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (rawdata->normalindex != NULL) {
	type.category	= SCE_HIP_SHARE;
	type.id		= SCE_HIP_NORMALINDEX;
	err = sceHiMakeDataBlk(rawdata->normalindex, &higdata.Nindex, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (rawdata->sharevertex != NULL) {
	type.category	= SCE_HIP_SHARE;
	type.id		= SCE_HIP_SHAREVERTEX;
	err = sceHiMakeDataBlk(rawdata->sharevertex, &higdata.shareV, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (rawdata->sharenormal != NULL) {
	type.category	= SCE_HIP_SHARE;
	type.id		= SCE_HIP_SHARENORMAL;
	err = sceHiMakeDataBlk(rawdata->sharenormal, &higdata.shareN, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    /* make plugin tree */
    frameP = microP = shapeP = shareP = hrchyP = animeP = tex2dP = NULL;
    type.status		= SCE_HIG_PLUGIN_STATUS;

    type.category	= SCE_HIP_FRAME;
    type.id		= SCE_HIP_FRAME_PLUG;
    err = sceHiNewPlugBlk(6, 0, &frameP, &type);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (higdata.micro != NULL) {
	type.category	= SCE_HIP_MICRO;
	type.id		= SCE_HIP_MICRO_PLUG;
	err = sceHiNewPlugBlk(0, 1, &microP, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (higdata.anim != NULL) {
	type.category	= SCE_HIP_ANIME;
	type.id		= SCE_HIP_ANIME_PLUG;
	err = sceHiNewPlugBlk(0, 4, &animeP, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (higdata.hrchy != NULL) {
	type.category	= SCE_HIP_HRCHY;
	type.id		= SCE_HIP_HRCHY_PLUG;
	err = sceHiNewPlugBlk(0, 2, &hrchyP, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (higdata.shape != NULL) {
	type.category	= SCE_HIP_SHAPE;
	type.id		= SCE_HIP_SHAPE_PLUG;
	err = sceHiNewPlugBlk(0, 3, &shapeP, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (higdata.share != NULL) {
	type.category	= SCE_HIP_SHARE;
	type.id		= SCE_HIP_SHARE_PLUG;
	err = sceHiNewPlugBlk(0, 9, &shareP, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (higdata.tex2d != NULL) {
	type.category	= SCE_HIP_TEX2D;
	type.id		= SCE_HIP_TEX2D_PLUG;
	err = sceHiNewPlugBlk(0, 1, &tex2dP, &type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    if (microP != NULL) {
	err = sceHiAddPlugBlk(frameP, microP);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (tex2dP != NULL) {
	err = sceHiAddPlugBlk(frameP, tex2dP);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (shapeP != NULL) {
	err = sceHiAddPlugBlk(frameP, shapeP);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (hrchyP != NULL) {
	err = sceHiAddPlugBlk(frameP, hrchyP);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (animeP != NULL) {
	err = sceHiAddPlugBlk(frameP, animeP);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (shareP != NULL) {
	err = sceHiAddPlugBlk(frameP, shareP);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    /* set plugin API */
    if (frameP != NULL){
	err = sceHiSetPluginApi(frameP);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (microP != NULL){
	err = sceHiSetPluginApi(microP);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (animeP != NULL){
	err = sceHiSetPluginApi(animeP);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (shapeP != NULL){
	err = sceHiSetPluginApi(shapeP);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (hrchyP != NULL){
	err = sceHiSetPluginApi(hrchyP);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (tex2dP != NULL){
	err = sceHiSetPluginApi(tex2dP);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (shareP != NULL){
	err = sceHiSetPluginApi(shareP);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    /* insert data */
    if (shapeP != NULL) {
	err = sceHiAddDataBlk(shapeP, higdata.shape);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(shapeP, higdata.basem);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	if (higdata.tex2d != NULL){
	    err = sceHiAddDataBlk(shapeP, higdata.tex2d);
	    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	}
    }
    if (hrchyP != NULL) {
	err = sceHiAddDataBlk(hrchyP, higdata.hrchy);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	if (higdata.basem != NULL){
	    err = sceHiAddDataBlk(hrchyP, higdata.basem);
	    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	}
    }
    if (microP != NULL) {
	err = sceHiAddDataBlk(microP, higdata.micro);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (animeP != NULL) {
	err = sceHiAddDataBlk(animeP, higdata.anim);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(animeP, higdata.key);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(animeP, higdata.val);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	if (higdata.hrchy != NULL){
	    err = sceHiAddDataBlk(animeP, higdata.hrchy);
	    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	}
    }
    if (tex2dP != NULL) {
	err = sceHiAddDataBlk(tex2dP, higdata.tex2d);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    if (shareP != NULL) {
	err = sceHiAddDataBlk(shareP, higdata.share);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(shareP, higdata.srcdstV);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(shareP, higdata.srcdstN);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(shareP, higdata.Vindex);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(shareP, higdata.Nindex);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(shareP, higdata.shareV);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(shareP, higdata.shareN);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	if (higdata.basem != NULL){
	    err = sceHiAddDataBlk(shareP, higdata.basem);
	    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	}
	if (higdata.shape != NULL){
	    err = sceHiAddDataBlk(shareP, higdata.shape);
	    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	}
    }

    err = sceHiCallPlug(frameP, SCE_HIG_INIT_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    microD = (sceHiPlugMicroData_t *)rawdata->micro_data;
    camerainit(microD);
    lightinit(microD);
    arg.micro = 0;
}

int SAMPLE7_main(void)
{
    sceHiErr	err;
    microP->args = (u_int) &arg;

    cameractrl();
    cameraview(microD);

    err = sceHiCallPlug(frameP, SCE_HIG_PRE_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiCallPlug(frameP, SCE_HIG_POST_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    utilSetDMATimer();
    err = sceHiDMASend();
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiDMAWait();
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    arg.micro = -1;

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

void SAMPLE7_exit(void)
{
    sceHiErr	err;
    err = sceHiCallPlug(frameP, SCE_HIG_END_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if (higdata.basem != NULL)
	sceHiMemFree(higdata.basem);
    if (higdata.hrchy != NULL)
	sceHiMemFree(higdata.hrchy);
    if (higdata.micro != NULL)
	sceHiMemFree(higdata.micro);
    if (higdata.anim != NULL)
	sceHiMemFree(higdata.anim);
    if (higdata.key != NULL)
	sceHiMemFree(higdata.key);
    if (higdata.val != NULL)
	sceHiMemFree(higdata.val);
    if (higdata.shape != NULL)
	sceHiMemFree(higdata.shape);
    if (higdata.tex2d != NULL)
	sceHiMemFree(higdata.tex2d);
    if (higdata.share != NULL)
	sceHiMemFree(higdata.share);
    if (higdata.srcdstV != NULL)
	sceHiMemFree(higdata.srcdstV);
    if (higdata.srcdstN != NULL)
	sceHiMemFree(higdata.srcdstN);
    if (higdata.Vindex != NULL)
	sceHiMemFree(higdata.Vindex);
    if (higdata.Nindex != NULL)
	sceHiMemFree(higdata.Nindex);
    if (higdata.shareV != NULL)
	sceHiMemFree(higdata.shareV);
    if (higdata.shareN != NULL)
	sceHiMemFree(higdata.shareN);
    if (frameP != NULL)
	sceHiMemFree(frameP);
    if (microP != NULL)
	sceHiMemFree(microP);
    if (shapeP != NULL)
	sceHiMemFree(shapeP);
    if (shareP != NULL)
	sceHiMemFree(shareP);
    if (hrchyP != NULL)
	sceHiMemFree(hrchyP);
    if (animeP != NULL)
	sceHiMemFree(animeP);
    if (tex2dP != NULL)
	sceHiMemFree(tex2dP);
    free(DATA_ADDR);

}
