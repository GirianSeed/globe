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
#include <malloc.h>
#include <libgraph.h>
#include <libvu0.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libhig.h>
#include <libhip.h>
#include "util.h"

/*---------- static type ----------*/
typedef struct {
	int	frame;			/* current frame count */
	int	cpu_count;		/* cpu processing time */
	int	dma_count;		/* dma processing time */
} _status_t;

typedef void init_func_t(void);
typedef int main_func_t(void);
typedef void exit_func_t(void);

int	dma_timer;

/*---------- static constants ----------*/
static sceHiPlugTable ptable[] = {
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_MICRO, SCE_HIG_PLUGIN_STATUS, SCE_HIP_MICRO_PLUG, SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugMicro },
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_TEX2D, SCE_HIG_PLUGIN_STATUS, SCE_HIP_TEX2D_PLUG, SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugTex2D },
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_TIM2 , SCE_HIG_PLUGIN_STATUS, SCE_HIP_TIM2_PLUG , SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugTim2  },
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_SHAPE, SCE_HIG_PLUGIN_STATUS, SCE_HIP_SHAPE_PLUG, SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugShape },
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_HRCHY, SCE_HIG_PLUGIN_STATUS, SCE_HIP_HRCHY_PLUG, SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugHrchy },
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_ANIME, SCE_HIG_PLUGIN_STATUS, SCE_HIP_ANIME_PLUG, SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugAnime },
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_SHARE, SCE_HIG_PLUGIN_STATUS, SCE_HIP_SHARE_PLUG, SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugShare },
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_BUMP, SCE_HIG_PLUGIN_STATUS, SCE_HIP_CLUTBUMP_PLUG, SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugClutBump },
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_SHADOW, SCE_HIG_PLUGIN_STATUS, SCE_HIP_SHADOWMAP_PLUG, SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugShadowMap },
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_SHADOW, SCE_HIG_PLUGIN_STATUS, SCE_HIP_SHADOWBOX_PLUG, SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugShadowBox },
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_LIGHT, SCE_HIG_PLUGIN_STATUS, SCE_HIP_LIGHTMAP_PLUG, SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugLightMap },
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_REFLECT, SCE_HIG_PLUGIN_STATUS, SCE_HIP_FISHEYE_PLUG, SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugFishEye },
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_REFLECT, SCE_HIG_PLUGIN_STATUS, SCE_HIP_REFLECT_PLUG, SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugReflect },
{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_REFLECT, SCE_HIG_PLUGIN_STATUS, SCE_HIP_REFRACT_PLUG, SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugRefract },
//{{SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_CLIP, SCE_HIG_PLUGIN_STATUS, SCE_HIP_CLIP_PLUG, SCE_HIP_REVISION}, (sceHiErr *)sceHiPlugClip },
};
enum {
	HIG_HEAP_SIZE	=	8 * 1024 * 1024,	/* 5 mega byte */
	HIDMA_BUF_SIZE	=	3 * 1024 * 1204,	/* 3 mega byte */
	NTABLE		= 	sizeof(ptable)/sizeof(sceHiPlugTable)
};

/*---------- static functions ----------*/
static void _init_sample(void);
static void _main_handling(void);
static void _menu_init(void);
static int _menu(const char **, const char **, int);
static void _disp_status(_status_t *);

/*---------- SAMPLE DEFINES ----------*/
#define DEFFUNC(func,comment)	#func,
const char *func_name[] = {
#include "sample_func.tbl"
};
#undef DEFFUNC

#define DEFFUNC(func,comment)	#comment,
const char *func_comment[] = {
#include "sample_func.tbl"
};
#undef DEFFUNC

#define DEFFUNC(func,comment)	func,
enum {
#include "sample_func.tbl"
FUNC_NUM
};
#undef DEFFUNC

#define DEFFUNC(func,comment)	extern init_func_t func##_init;
#include "sample_func.tbl"
#undef DEFFUNC

#define DEFFUNC(func,comment)	extern main_func_t func##_main;
#include "sample_func.tbl"
#undef DEFFUNC

#define DEFFUNC(func,comment)	extern exit_func_t func##_exit;
#include "sample_func.tbl"
#undef DEFFUNC

#define DEFFUNC(func,comment)	func##_init,
init_func_t *init_func[FUNC_NUM] = {
#include "sample_func.tbl"
};
#undef DEFFUNC

#define DEFFUNC(func,comment)	func##_main,
main_func_t *main_func[FUNC_NUM] = {
#include "sample_func.tbl"
};
#undef DEFFUNC

#define DEFFUNC(func,comment)	func##_exit,
exit_func_t *exit_func[FUNC_NUM] = {
#include "sample_func.tbl"
};
#undef DEFFUNC

enum {
    MENU_INIT_PROCESS,
    MENU_PROCESS,
    SAMPLE_INIT_PROCESS,
    SAMPLE_PROCESS,
    SAMPLE_EXIT_PROCESS
};

_status_t	status;

/* to keep the original draw environment */
static sceHiGsCtx *_menuGsCtx;
static sceHiGsEnv *_menuGsEnv;


int main()
{
    int		odev;
    u_int	frame = 0;
    sceHiErr	err;

    _init_sample();

    /* debug print initialize */
    draw_debug_print_init();

    while (1) {
	++frame;
	status.frame = frame;
	*T0_COUNT = 0;			/* reset Timer */
	utilResetDMATimer();
	
	/***********
	 * SAMPLE PAD HANDLING
	 ***********/
	pad_ctrl();
	
	/***********
	 * SAMPLE MAIN HANDLING
	 ***********/
	sceHiGsEnvRegist(sceHiGsStdEnv);
	_main_handling();
	/***********
	 ***********/

	//status.dma_count = utilGetDMATimerEnd() - utilGetDMATimerStart();
	//status.cpu_count = utilGetDMATimerStart();
	_disp_status(&status);
	
	draw_debug_print_exec();
	
	/* switch GS Double Buffer */
	odev = !sceGsSyncV(0);
	err = sceHiGsDisplaySwap(odev);	
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    return 0;
}

void _init_sample(void)
{
    sceHiErr		err;
    
    sceSifInitRpc(0);
    while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/sio2man.irx",
			    0,
			    NULL) < 0)
	printf("Can't load module sio2man\n");
    while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/padman.irx",
			    0,
			    NULL) < 0)
	printf("Can't load module padman\n");

/***********
 * Initialize sequence of HiG
 ***********/
    err = sceHiMemInit(memalign(64, HIG_HEAP_SIZE), HIG_HEAP_SIZE);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiDMAInit(NULL, NULL, HIDMA_BUF_SIZE);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiRegistTable(ptable, NTABLE);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    sceGsResetPath();

    err = sceHiGsMemInit(SCE_HIGS_MEM_TOP, SCE_HIGS_MEM_END);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsDisplayMode(SCE_HIGS_RESET|SCE_HIGS_NTSCI|SCE_HIGS_RGBA|SCE_HIGS_DEPTH24);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsDisplaySize(640,224);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsClearColor(80, 80, 80, 128);
    err = sceHiGsClearDepth(0);
    err = sceHiGsClear(SCE_HIGS_CLEAR_ALL);
    err = sceHiGsFogcolRegs(25,25,25);
    err = sceHiGsAlphaRegs(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    err = sceHiGsTestRegs(SCE_GS_FALSE, SCE_GS_ALPHA_NEVER, 0, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GREATER);
    err = sceHiGsClampRegs(SCE_GS_CLAMP, SCE_GS_CLAMP, 0, 0, 0, 0);
    err = sceHiGsPrmodecontRegs(SCE_GS_TRUE);
    err = sceHiGsTexaRegs(128, 1, 128);
    err = sceHiGsTex1Regs(0, 0, SCE_GS_LINEAR, SCE_GS_LINEAR, 0, 0, 0);
    err = sceHiGsPrmodeRegs(SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GS_FALSE);

    /* update stdport packets */
    err = sceHiGsCtxUpdate(sceHiGsStdCtx);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    _menuGsCtx=sceHiGsCtxCreate(sceHiGsStdCtx->isDbuf);
    if (_menuGsCtx==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsCtxCopy(_menuGsCtx, sceHiGsStdCtx);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    err = sceHiGsEnvUpdate(sceHiGsStdEnv);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    _menuGsEnv=sceHiGsEnvCreate(sceHiGsStdEnv->validregs);
    if (_menuGsEnv==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsEnvCopy(_menuGsEnv, sceHiGsStdEnv);


    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsEnvRegist(sceHiGsStdEnv);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    pad_init();

    *T0_MODE = 2  | (0<<6) | (1<<7) | (0<<8); 
    *T1_MODE = 2  | (0<<6) | (1<<7) | (0<<8);    
    utilInitDMATimer();
}

static void _disp_status(_status_t *status)
{
	float	cpu_pow;

	/* 1V=9765.625f at this counter */
	cpu_pow = ((float)status->cpu_count * 100.0f) / 9765.625f;

	draw_debug_print(1, 1, "FRAME   : %8u", status->frame);
	draw_debug_print(1, 2, "Core+Vu0 POW : %3.2f%%", cpu_pow);
	draw_debug_print(20, 3, "press select to change mode");
}

static void _main_handling(void)
{
        sceHiErr	err;
	static int	mode = MENU_INIT_PROCESS;
	static int	func_num = 0;

	switch (mode) {
	  case MENU_INIT_PROCESS:
		_menu_init();
		mode = MENU_PROCESS;
		break;
	  case MENU_PROCESS:
		func_num = _menu(func_name, func_comment, FUNC_NUM);
		if (func_num >= 0)
			mode = SAMPLE_INIT_PROCESS;
		utilSetDMATimer();
		if ((err = sceHiDMASend()) != SCE_HIG_NO_ERR)
		    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
		if ((err = sceHiDMAWait()) != SCE_HIG_NO_ERR)
		    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

		break;
	  case SAMPLE_INIT_PROCESS:
		sceHiGsCtxSetDefault(sceHiGsStdCtx);
		init_func[func_num]();
		mode = SAMPLE_PROCESS;
		break;
	  case SAMPLE_PROCESS:
		if (main_func[func_num]())
			mode = SAMPLE_EXIT_PROCESS;

		break;
	  case SAMPLE_EXIT_PROCESS:
		exit_func[func_num]();
		mode = MENU_PROCESS;
		if ((err = sceHiDMASend()) != SCE_HIG_NO_ERR)
		    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
		if ((err = sceHiDMAWait()) != SCE_HIG_NO_ERR)
		    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

		err = sceHiGsCtxSetDefault(sceHiGsStdCtx);
		if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
		err = sceHiGsCtxCopy(sceHiGsStdCtx, _menuGsCtx);
		if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

		err = sceHiGsCtxUpdate(sceHiGsStdCtx);
		if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
		err = sceHiGsEnvCopy(sceHiGsStdEnv, _menuGsEnv);
		if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
		err = sceHiGsEnvUpdate(sceHiGsStdEnv);
		if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

		
		break;
	}
}

void _menu_init(void)
{
}

int _menu(const char **fname, const char **fcomment, int nbfunc)
{
	static	int item = 0;
	static  int menustart = 0;
	static  int repeat_counter = 0;
	const   int nbcolumns = 10;
	const   int move_interval = 6;
	int	x, y, i;
	int	func_num = -1;
	int     barstart, barend;
	int     nbshow = nbfunc < nbcolumns ? nbfunc : nbcolumns;

	/* move cursor */
	if (gPad.LupSwitch){
		--item;
		repeat_counter = - move_interval;
	} else if (gPad.LdownSwitch){
		++item;
		repeat_counter = - move_interval;
	}
	if(gPad.L1Switch)
		item -= 5;
	if(gPad.R1Switch)
		item += 5;

	/* key-repeat check */
	if (gPad.LupOn){
		if (repeat_counter++ > move_interval){
			--item;
			repeat_counter -= move_interval;
		}
	}
	if (gPad.LdownOn){
		if (repeat_counter++ > move_interval){
			++item;
			repeat_counter -= move_interval;
		}
	}

	item = item > 0 ? item : 0;
	item = item < nbfunc ? item : nbfunc - 1;

	/* move scroll bar */
	if (item > menustart + nbcolumns - 1)
		menustart = item - nbcolumns + 1;
	if (item < menustart)
		menustart = item;

	/* choice item */
	if (gPad.RrightSwitch)
		func_num = item;

	/* disp sample name & comment */
	x = 5;
	y = 12;
	for (i = 0; i < nbshow; i++) {
		draw_debug_print(x, y + i, "%s", fname[menustart + i]);
		draw_debug_print(x + 20, y + i, "%s", fcomment[menustart + i]);
	}

	/* disp cursor */
	x = 4;
	y = 12 + item - menustart;
	draw_debug_print(x, y, ">");

	/* disp scroll bar */
	x = 2;
	y = 12;
	barstart = menustart * nbcolumns / nbfunc;
	barend   = (menustart + nbcolumns) * nbcolumns / nbfunc;

	for (i = 0; i < nbcolumns; i++){
		draw_debug_print(x + 75, y + i, "|");
		if(i < barstart || i > barend)
			draw_debug_print(x, y + i, "|");
		else
			draw_debug_print(x, y + i, "D");
	}

	draw_debug_print(x, y - 1, "X-----------------------------------MENU-----------------------------------X");
	draw_debug_print(x, y + nbcolumns, "X--------------------------------------------------------------------------X");

	x = 60;
	y = 1;
	draw_debug_print(x, y, "O    : start");
	draw_debug_print(x, y + 1, "UP   : previous");
	draw_debug_print(x, y + 2, "DOWN : next");
	draw_debug_print(x, y + 3, "R1   : +5");
	draw_debug_print(x, y + 4, "L1   : -5");

	return func_num;
}
