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
 * sample program 3
 *
 *	Data construction sample only by block operation function
 ***************************************************/

/* DATA STRUCTURE */
typedef struct {
	void		*rawdata;
	sceHiData	*higdata;
} _hig_data_t;

/* ball data */
typedef struct {
	_hig_data_t	basem;
	_hig_data_t	hrchy;
	_hig_data_t	micro;
	_hig_data_t	anim;
	_hig_data_t	key;
	_hig_data_t	val;
	_hig_data_t	shape;
	_hig_data_t	tex2d;
} _ball_data_t;

_ball_data_t	_ball_data;

static sceHiPlug	*_ball_frame;
static sceHiPlug	*_ball_micro;
static sceHiPlug	*_ball_anim;
static sceHiPlug	*_ball_shape;
static sceHiPlug	*_ball_hrchy;
static sceHiPlug	*_ball_tex2d;

static sceHiPlugTex2dInitArg_t	tex2d_arg;

static sceHiPlugMicroData_t *microD;

void SAMPLE3_init(void)
{
	sceHiType	type;
	sceHiErr	err;

	/* LOAD DATA from HOST-MACHINE */
	_ball_data.basem.rawdata 	= file_read("data/ball_basem.bin");
	_ball_data.hrchy.rawdata 	= file_read("data/ball_hrchy.bin");
	_ball_data.micro.rawdata 	= file_read("data/ball_micro.bin");
	_ball_data.anim.rawdata 	= file_read("data/ball_anim.bin");
	_ball_data.key.rawdata		= file_read("data/ball_key.bin");
	_ball_data.val.rawdata		= file_read("data/ball_val.bin");
	_ball_data.tex2d.rawdata	= file_read("data/ball1_tex2d.bin");
	_ball_data.shape.rawdata	= file_read("data/ball1_shape.bin");

	type.repository = SCE_HIP_COMMON;
	type.project	= SCE_HIP_FRAMEWORK;
	type.revision	= SCE_HIP_REVISION;

	type.status	= SCE_HIG_DATA_STATUS;

	/* common data */
	type.category 	= SCE_HIP_SHAPE;
	type.id		= SCE_HIP_BASEMATRIX;
	err = sceHiMakeDataBlk((u_int *)_ball_data.basem.rawdata, &_ball_data.basem.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	type.category	= SCE_HIP_HRCHY;
	type.id		= SCE_HIP_HRCHY_DATA;
	err = sceHiMakeDataBlk((u_int *)_ball_data.hrchy.rawdata, &_ball_data.hrchy.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	type.category	= SCE_HIP_MICRO;
	type.id		= SCE_HIP_MICRO_DATA;
	sceHiMakeDataBlk((u_int *)_ball_data.micro.rawdata, &_ball_data.micro.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	type.category	= SCE_HIP_ANIME;
	type.id		= SCE_HIP_ANIME_DATA;
	err = sceHiMakeDataBlk((u_int *)_ball_data.anim.rawdata, &_ball_data.anim.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	type.category	= SCE_HIP_ANIME;
	type.id		= SCE_HIP_KEYFRAME;
	err = sceHiMakeDataBlk((u_int *)_ball_data.key.rawdata, &_ball_data.key.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	type.category	= SCE_HIP_ANIME;
	type.id		= SCE_HIP_KEYVALUE;
	err = sceHiMakeDataBlk((u_int *)_ball_data.val.rawdata, &_ball_data.val.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


	/* shape data */
	type.category	= SCE_HIP_SHAPE;
	type.id		= SCE_HIP_SHAPE_DATA;
	err = sceHiMakeDataBlk((u_int *)_ball_data.shape.rawdata, &_ball_data.shape.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* tex2d data */
	type.category	= SCE_HIP_TEX2D;
	type.id		= SCE_HIP_TEX2D_DATA;
	err = sceHiMakeDataBlk((u_int *)_ball_data.tex2d.rawdata, &_ball_data.tex2d.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* make empty plugin */
	type.status	= SCE_HIG_PLUGIN_STATUS;

	/* frame plugin */
	type.category	= SCE_HIP_FRAME;
	type.id		= SCE_HIP_FRAME_PLUG;
	err = sceHiNewPlugBlk(5, 0, &_ball_frame, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* micro plugin */
	type.category	= SCE_HIP_MICRO;
	type.id		= SCE_HIP_MICRO_PLUG;
	err = sceHiNewPlugBlk(0, 1, &_ball_micro, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* anime plugin */
	type.category	= SCE_HIP_ANIME;
	type.id		= SCE_HIP_ANIME_PLUG;
	err = sceHiNewPlugBlk(0, 4, &_ball_anim, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* shape plugin */
	type.category	= SCE_HIP_SHAPE;
	type.id		= SCE_HIP_SHAPE_PLUG;
	err = sceHiNewPlugBlk(0, 3, &_ball_shape, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* hrchy plugin */
	type.category	= SCE_HIP_HRCHY;
	type.id		= SCE_HIP_HRCHY_PLUG;
	err = sceHiNewPlugBlk(0, 2, &_ball_hrchy, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* tex2d plugin */
	type.category	= SCE_HIP_TEX2D;
	type.id		= SCE_HIP_TEX2D_PLUG;
	err = sceHiNewPlugBlk(0, 1, &_ball_tex2d, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* make plugin tree */
	err = sceHiAddPlugBlk(_ball_frame, _ball_micro);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddPlugBlk(_ball_frame, _ball_tex2d);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddPlugBlk(_ball_frame, _ball_shape);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddPlugBlk(_ball_frame, _ball_hrchy);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddPlugBlk(_ball_frame, _ball_anim);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* set plugin api */
	err = sceHiSetPluginApi(_ball_frame);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiSetPluginApi(_ball_micro);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiSetPluginApi(_ball_anim);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiSetPluginApi(_ball_shape);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiSetPluginApi(_ball_hrchy);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiSetPluginApi(_ball_tex2d);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* insert Common Data */
	err = sceHiAddDataBlk(_ball_shape, _ball_data.basem.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(_ball_hrchy, _ball_data.basem.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(_ball_hrchy, _ball_data.hrchy.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(_ball_anim,  _ball_data.hrchy.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(_ball_micro, _ball_data.micro.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(_ball_anim,  _ball_data.anim.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(_ball_anim,  _ball_data.key.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(_ball_anim,  _ball_data.val.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(_ball_shape, _ball_data.shape.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(_ball_tex2d, _ball_data.tex2d.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(_ball_shape, _ball_data.tex2d.higdata);

	tex2d_arg.resident = TRUE;
	tex2d_arg.tbl = NULL;
	_ball_tex2d->args = (u_int)&tex2d_arg;

	sceHiCallPlug(_ball_frame, SCE_HIG_INIT_PROCESS);

	/* camra & light */
	microD = (sceHiPlugMicroData_t *)_ball_data.micro.rawdata;
	camerainit(microD);
	lightinit(microD);
}

int SAMPLE3_main(void)
{
	sceHiErr	err;

	/* camera moving */
	cameractrl();
	cameraview(microD);

	err = sceHiCallPlug(_ball_frame, SCE_HIG_PRE_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiCallPlug(_ball_frame, SCE_HIG_POST_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

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

void SAMPLE3_exit(void)
{
	sceHiErr	err;

	err = sceHiCallPlug(_ball_frame, SCE_HIG_END_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	sceHiMemFree(_ball_data.basem.higdata);
	sceHiMemFree(_ball_data.hrchy.higdata);
	sceHiMemFree(_ball_data.micro.higdata);
	sceHiMemFree(_ball_data.anim.higdata);
	sceHiMemFree(_ball_data.key.higdata);
	sceHiMemFree(_ball_data.val.higdata);
	sceHiMemFree(_ball_data.shape.higdata);
	sceHiMemFree(_ball_data.tex2d.higdata);
	free(_ball_data.basem.rawdata);
	free(_ball_data.hrchy.rawdata);
	free(_ball_data.micro.rawdata);
	free(_ball_data.anim.rawdata);
	free(_ball_data.key.rawdata);
	free(_ball_data.val.rawdata);
	free(_ball_data.shape.rawdata);
	free(_ball_data.tex2d.rawdata);
	sceHiMemFree(_ball_frame);
	sceHiMemFree(_ball_micro);
	sceHiMemFree(_ball_anim);
	sceHiMemFree(_ball_shape);
	sceHiMemFree(_ball_hrchy);
	sceHiMemFree(_ball_tex2d);
}
