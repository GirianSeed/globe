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
 * sample program 4
 *
 *	Level Of Detail sample that only by block operation functions
 ***************************************************/

/* DATA STRUCTURE */
typedef struct {
	void		*rawdata;
	sceHiData	*higdata;
} _hig_data_t;

/* Common data */
typedef struct {
	_hig_data_t	basem;
	_hig_data_t	hrchy;
	_hig_data_t	micro;
	_hig_data_t	anim;
	_hig_data_t	key;
	_hig_data_t	val;
} _ball_common_data_t;

/* Kind distinction data */
typedef struct {
	_hig_data_t	shape;
	_hig_data_t	tex2d;
} _ball_distinct_data_t;

static _ball_common_data_t	_ball_common;
static _ball_distinct_data_t	_ball1_distinc;
static _ball_distinct_data_t	_ball2_distinc;
static _ball_distinct_data_t	_ball3_distinc;
static _ball_distinct_data_t	_ball4_distinc;

static sceHiPlug	*_ball_frame;
static sceHiPlug	*_ball_micro;
static sceHiPlug	*_ball_anim;
static sceHiPlug	*_ball_shape;
static sceHiPlug	*_ball_hrchy;
static sceHiPlug	*_ball_tex2d;

static _ball_distinct_data_t	*_save_addr;

static void _set_ball_data(_ball_distinct_data_t *);
static void _rmv_ball_data(void);
static void _change_ball1(void);
static void _change_ball2(void);
static void _change_ball3(void);
static void _change_ball4(void);
static int _check_lod(void);

static sceHiPlugTex2dInitArg_t	tex2d_arg;

static sceHiPlugMicroData_t *microD;

void SAMPLE4_init(void)
{
	sceHiType	type;
	sceHiErr	err;

	/* LOAD DATA from HOST-MACHINE */
	_ball_common.basem.rawdata 	= file_read("data/ball_basem.bin");
	_ball_common.hrchy.rawdata 	= file_read("data/ball_hrchy.bin");
	_ball_common.micro.rawdata 	= file_read("data/ball_micro.bin");
	_ball_common.anim.rawdata	= file_read("data/ball_anim.bin");
	_ball_common.key.rawdata	= file_read("data/ball_key.bin");
	_ball_common.val.rawdata	= file_read("data/ball_val.bin");

	_ball1_distinc.shape.rawdata	= file_read("data/ball1_shape.bin");
	_ball1_distinc.tex2d.rawdata	= file_read("data/ball1_tex2d.bin");

	_ball2_distinc.shape.rawdata	= file_read("data/ball2_shape.bin");
	_ball2_distinc.tex2d.rawdata	= file_read("data/ball2_tex2d.bin");

	_ball3_distinc.shape.rawdata	= file_read("data/ball3_shape.bin");
	_ball3_distinc.tex2d.rawdata	= file_read("data/ball3_tex2d.bin");

	_ball4_distinc.shape.rawdata	= file_read("data/ball4_shape.bin");
	_ball4_distinc.tex2d.rawdata	= file_read("data/ball4_tex2d.bin");

	type.repository = SCE_HIP_COMMON;
	type.project	= SCE_HIP_FRAMEWORK;
	type.revision	= SCE_HIP_REVISION;

	type.status	= SCE_HIG_DATA_STATUS;

	/* common data */
	type.category 	= SCE_HIP_SHAPE;
	type.id		= SCE_HIP_BASEMATRIX;
	err = sceHiMakeDataBlk((u_int *)_ball_common.basem.rawdata, &_ball_common.basem.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	type.category	= SCE_HIP_HRCHY;
	type.id		= SCE_HIP_HRCHY_DATA;
	err = sceHiMakeDataBlk((u_int *)_ball_common.hrchy.rawdata, &_ball_common.hrchy.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	type.category	= SCE_HIP_MICRO;
	type.id		= SCE_HIP_MICRO_DATA;
	err = sceHiMakeDataBlk((u_int *)_ball_common.micro.rawdata, &_ball_common.micro.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	type.category	= SCE_HIP_ANIME;
	type.id		= SCE_HIP_ANIME_DATA;
	err = sceHiMakeDataBlk((u_int *)_ball_common.anim.rawdata, &_ball_common.anim.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	type.category	= SCE_HIP_ANIME;
	type.id		= SCE_HIP_KEYFRAME;
	err = sceHiMakeDataBlk((u_int *)_ball_common.key.rawdata, &_ball_common.key.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	type.category	= SCE_HIP_ANIME;
	type.id		= SCE_HIP_KEYVALUE;
	err = sceHiMakeDataBlk((u_int *)_ball_common.val.rawdata, &_ball_common.val.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* shape data */
	type.category	= SCE_HIP_SHAPE;
	type.id		= SCE_HIP_SHAPE_DATA;
	err = sceHiMakeDataBlk((u_int *)_ball1_distinc.shape.rawdata, &_ball1_distinc.shape.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiMakeDataBlk((u_int *)_ball2_distinc.shape.rawdata, &_ball2_distinc.shape.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
 	err = sceHiMakeDataBlk((u_int *)_ball3_distinc.shape.rawdata, &_ball3_distinc.shape.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiMakeDataBlk((u_int *)_ball4_distinc.shape.rawdata, &_ball4_distinc.shape.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* tex2d data */
	type.category	= SCE_HIP_TEX2D;
	type.id		= SCE_HIP_TEX2D_DATA;
	err = sceHiMakeDataBlk((u_int *)_ball1_distinc.tex2d.rawdata, &_ball1_distinc.tex2d.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiMakeDataBlk((u_int *)_ball2_distinc.tex2d.rawdata, &_ball2_distinc.tex2d.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiMakeDataBlk((u_int *)_ball3_distinc.tex2d.rawdata, &_ball3_distinc.tex2d.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiMakeDataBlk((u_int *)_ball4_distinc.tex2d.rawdata, &_ball4_distinc.tex2d.higdata, &type);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* making emptry plugin */
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

	/* making plugin tree */
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
	err = sceHiAddDataBlk(_ball_shape, _ball_common.basem.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(_ball_hrchy, _ball_common.basem.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(_ball_hrchy, _ball_common.hrchy.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(_ball_anim,  _ball_common.hrchy.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(_ball_micro, _ball_common.micro.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(_ball_anim, _ball_common.anim.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(_ball_anim, _ball_common.key.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(_ball_anim, _ball_common.val.higdata);

	/* default model : High Poly */
	_set_ball_data(&_ball1_distinc);

	/* save data point */
	_save_addr = &_ball1_distinc;

	tex2d_arg.resident = TRUE;
	tex2d_arg.tbl = NULL;
	_ball_tex2d->args = (u_int)&tex2d_arg;

	sceHiCallPlug(_ball_frame, SCE_HIG_INIT_PROCESS);

	/* camera & light */
	microD = (sceHiPlugMicroData_t *)_ball_common.micro.rawdata;
	camerainit(microD);
	lightinit(microD);
}

int SAMPLE4_main(void)
{
	static int disp_ball	= 0;
	static int disp_old_ball= 0;
	static int frame	= 0;
	sceHiErr	err;

	/* camera moving */
	cameractrl();
	cameraview(microD);

	++frame;
	disp_ball = _check_lod();
	if (disp_ball != disp_old_ball) {
		switch (disp_ball) {
		  case 0:
			_change_ball1();
			break;
		  case 1:
			_change_ball2();
			break;
		  case 2:
			_change_ball3();
			break;
		  case 3:
			_change_ball4();
			break;
		}
		disp_old_ball = disp_ball;
	}

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

void SAMPLE4_exit(void)
{
	sceHiErr	err;

	err = sceHiCallPlug(_ball_frame, SCE_HIG_END_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	sceHiMemFree(_ball_common.basem.higdata);
	sceHiMemFree(_ball_common.hrchy.higdata);
	sceHiMemFree(_ball_common.micro.higdata);
	sceHiMemFree(_ball_common.anim.higdata);
	sceHiMemFree(_ball_common.key.higdata);
	sceHiMemFree(_ball_common.val.higdata);
	sceHiMemFree(_ball1_distinc.shape.higdata);
	sceHiMemFree(_ball1_distinc.tex2d.higdata);
	sceHiMemFree(_ball2_distinc.shape.higdata);
	sceHiMemFree(_ball2_distinc.tex2d.higdata);
	sceHiMemFree(_ball3_distinc.shape.higdata);
	sceHiMemFree(_ball3_distinc.tex2d.higdata);
	sceHiMemFree(_ball4_distinc.shape.higdata);
	sceHiMemFree(_ball4_distinc.tex2d.higdata);
	free(_ball_common.basem.rawdata);
	free(_ball_common.hrchy.rawdata);
	free(_ball_common.micro.rawdata);
	free(_ball_common.anim.rawdata);
	free(_ball_common.key.rawdata);
	free(_ball_common.val.rawdata);
	free(_ball1_distinc.shape.rawdata);
	free(_ball1_distinc.tex2d.rawdata);
	free(_ball2_distinc.shape.rawdata);
	free(_ball2_distinc.tex2d.rawdata);
	free(_ball3_distinc.shape.rawdata);
	free(_ball3_distinc.tex2d.rawdata);
	free(_ball4_distinc.shape.rawdata);
	free(_ball4_distinc.tex2d.rawdata);
	sceHiMemFree(_ball_frame);
	sceHiMemFree(_ball_micro);
	sceHiMemFree(_ball_anim);
	sceHiMemFree(_ball_shape);
	sceHiMemFree(_ball_hrchy);
	sceHiMemFree(_ball_tex2d);
}

static void _set_ball_data(_ball_distinct_data_t *data)
{
	sceHiErr	err;

	err = sceHiAddDataBlk(_ball_shape, data->shape.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(_ball_tex2d, data->tex2d.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiAddDataBlk(_ball_shape, data->tex2d.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
}

static void _rmv_ball_data(void)
{
	sceHiErr	err;

	err = sceHiRmvDataBlk(_ball_shape, _save_addr->shape.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiRmvDataBlk(_ball_tex2d, _save_addr->tex2d.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiRmvDataBlk(_ball_shape, _save_addr->tex2d.higdata);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
}

static void _change_ball1(void)
{
	sceHiErr	err;

	/* exit shape/tex2d plugin */
	err = sceHiCallPlug(_ball_shape, SCE_HIG_END_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiCallPlug(_ball_tex2d, SCE_HIG_END_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* swap shape/tex2d data */
	_rmv_ball_data();
	_set_ball_data(&_ball1_distinc);

	/* restart shape/tex2d plugin */
	err = sceHiCallPlug(_ball_tex2d, SCE_HIG_INIT_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiCallPlug(_ball_shape, SCE_HIG_INIT_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* save addr:ball1 */
	_save_addr = &_ball1_distinc;
}

static void _change_ball2(void)
{
	sceHiErr	err;

	/* exit shape/tex2d plugin */
	err = sceHiCallPlug(_ball_shape, SCE_HIG_END_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiCallPlug(_ball_tex2d, SCE_HIG_END_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* swap shape/tex2d data */
	_rmv_ball_data();
	_set_ball_data(&_ball2_distinc);

	/* restart shape/tex2d plugin */
	err = sceHiCallPlug(_ball_tex2d, SCE_HIG_INIT_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiCallPlug(_ball_shape, SCE_HIG_INIT_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* save addr:ball2 */
	_save_addr = &_ball2_distinc;
}

static void _change_ball3(void)
{
	sceHiErr	err;

	/* exit shape/tex2d plugin */
	err = sceHiCallPlug(_ball_shape, SCE_HIG_END_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiCallPlug(_ball_tex2d, SCE_HIG_END_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* swap shape/tex2d data */
	_rmv_ball_data();
	_set_ball_data(&_ball3_distinc);

	/* restart shape/tex2d plugin */
	err = sceHiCallPlug(_ball_tex2d, SCE_HIG_INIT_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiCallPlug(_ball_shape, SCE_HIG_INIT_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* save addr:ball3 */
	_save_addr = &_ball3_distinc;
}

static void _change_ball4(void)
{
	sceHiErr	err;

	/* exit shape/tex2d plugin */
	err = sceHiCallPlug(_ball_shape, SCE_HIG_END_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiCallPlug(_ball_tex2d, SCE_HIG_END_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* swap shape/tex2d data */
	_rmv_ball_data();
	_set_ball_data(&_ball4_distinc);

	/* restart shape/tex2d plugin */
	err = sceHiCallPlug(_ball_tex2d, SCE_HIG_INIT_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiCallPlug(_ball_shape, SCE_HIG_INIT_PROCESS);
	if (err != SCE_HIG_NO_ERR)	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* save addr:ball4 */
	_save_addr = &_ball4_distinc;
}

static int _check_lod(void)
{
typedef struct {
	float	x, y, z, w;
	float	rotx, roty, rotz, rotw;
	float	sclx, scly, sclz, sclw;
	int	shapeID;
	int	parentID;
	int	reserve[2];
} _access_hrchy_data_t;

typedef struct {
	int	reserve[3];
	int	nbHrchy;	
	_access_hrchy_data_t	data[0];
} _access_hrchy_t;

	_access_hrchy_t	*hdata;
	int	i;
	float	mx, my, mz;	/* model translation */
	float	cx, cy, cz;	/* camera coordinate */
	float	r;

	hdata = (_access_hrchy_t *)_ball_common.hrchy.rawdata;

	/* the being parentless data which found in the first place */
	/* consider it to be route. if did not exist, be ID0 in route */
	for (i = 0; i < hdata->nbHrchy; i++) {
		if (hdata->data[i].parentID == -1) {
			break;
		}
	}

	if (i >= hdata->nbHrchy)
		i = 0;

	/* get model translation */
	mx = hdata->data[i].x;
	my = hdata->data[i].y;
	mz = hdata->data[i].z;

	/* get camera coordinate */
	cx = microD->camx;
	cy = microD->camy;
	cz = microD->camz;

	/* calc distance */
	r = (mx - cx) * (mx - cx) + (my - cy) * (my - cy) + (mz - cz) * (mz - cz);

	if (r > 55 * 55)	return 3;
	if (r > 40 * 40)	return 2;
	if (r > 25 * 25)	return 1;
	return 0;
}
