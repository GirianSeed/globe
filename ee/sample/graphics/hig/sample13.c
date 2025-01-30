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
 * sample program 13
 *
 *	GS Service sample  sub windows (multi windows)
 ***************************************************/
/* display data */
static const char *files[]={"data/shuttle.bin", "data/curtain.bin"};

/* num of window */
#define WINDOW_NUM 4

/* background color of each windows */
static u_char bgcolor[WINDOW_NUM][3]={
    {0, 0, 128},
    {128, 128, 0},
    {0, 128, 128},
    {0, 128, 0}
};

/* camera setting of each windows */
static void mycamera(sceHiPlugMicroData_t *md, int idx)
{
    static sceVu0FVECTOR s = {640.0f, 224.0f, 512.0f, 300.0f};
    static sceVu0FVECTOR w = {1.0f, 0.47f, 2048.0f, 2048.0f};
    static sceVu0FVECTOR d = {5.0f, 16777000.0f, 1.0f, 65536.0f};
    static sceVu0FVECTOR p[WINDOW_NUM]={{0.0f,0.0f,-30.0f,0.0f},{80.0f,0.0f,30.0f,0.0f},{0.0f,0.0f,110.0f,0.0f},{0.0f,-60.0f,-30.0f,0.0f}};
    static sceVu0FVECTOR r = {0.0f,0.0f,0.0f,0.0f};
    static sceVu0FVECTOR i = {0.0f,0.0f,30.0f,0.0f};
    static sceVu0FVECTOR u = {0.0f,1.0f,0.0f,0.0f};

    perspective(md, s,w,d);
    viewset(p[idx],r,i,u);
    lookatview(md);
}

/* bounds */
#define WIN_FRAME_W 10
#define WIN_FRAME_H 5
/* the size of each windows */
#define SUBWIN_W (640/2-WIN_FRAME_W)
#define SUBWIN_H (224/2-WIN_FRAME_H)


#define MODEL_NUM (sizeof(files)/sizeof(char *))

static struct _model{
    u_int	*data;
    sceHiPlug	*frameP;
    sceHiPlug	*microP;
    sceHiPlug	*hrchyP;
    sceHiPlug	*tex2dP;
    sceHiPlugMicroData_t *microD;
} model[MODEL_NUM];

static struct _window{
    sceHiGsCtx	*gsctx;
    int		x, y, w, h;
    u_char	r, g, b;
} window[WINDOW_NUM];


static sceHiPlugTex2dInitArg_t		_tex2d_arg;
static sceHiPlugHrchyPreCalcArg_t	_hrchy_arg;
static sceHiPlugMicroPreCalcArg_t	_micro_arg;

static sceVu0FMATRIX root_mat2;

static void window_ctrl(struct _window *m);
static void root_ctrl(void);

static void init_model(struct _model *m, const char *file)
{
    m->data = file_read((void *)file);

    if (sceHiParseHeader(m->data) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (sceHiGetPlug(m->data, "Frame", &m->frameP) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (sceHiGetPlug(m->data, "Tex2d", &m->tex2dP) == SCE_HIG_NO_ERR)
	m->tex2dP->args = (u_int)&_tex2d_arg;

    if (sceHiCallPlug(m->frameP, SCE_HIG_INIT_PROCESS) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (sceHiGetPlug(m->data, "Micro", &m->microP) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    m->microP->args = (u_int)&_micro_arg;

    /* get hrchy-plugin block from the data */
    if (sceHiGetPlug(m->data, "Hrchy", &m->hrchyP) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* camera & light */
    if((m->microD = sceHiPlugMicroGetData(m->microP)) == NULL)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    lightinit(m->microD);
}

static void exit_model(struct _model *m)
{
    if (sceHiCallPlug(m->frameP, SCE_HIG_END_PROCESS) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    free(m->data);
}

void SAMPLE13_init(void)
{
    int i;

    /* window position */
    for (i=0; i<WINDOW_NUM; i++){
	window[i].x=SUBWIN_W*(i%2)+WIN_FRAME_W;
	window[i].y=SUBWIN_H*(i/2)+WIN_FRAME_H;
	window[i].w=SUBWIN_W;
	window[i].h=SUBWIN_H;
    }

    /* background color */
    for (i=0; i<WINDOW_NUM; i++){
	window[i].r=bgcolor[i][0];
	window[i].g=bgcolor[i][1];
	window[i].b=bgcolor[i][2];
    }

    /* initialize sub windows */
    for (i=0; i<WINDOW_NUM; i++){
	window[i].gsctx=sceHiGsCtxCreate(1);
	sceHiGsCtxCopy(window[i].gsctx, sceHiGsStdCtx);
	window[i].gsctx->validregs=0;
	sceHiGsCtxSetRect(window[i].gsctx, window[i].x, window[i].y, window[i].w, window[i].h, 
			  SCE_HIGS_FBW_KEEP);
	sceHiGsCtxSetClearColor(window[i].gsctx, window[i].r, window[i].g, window[i].b, 128);
	sceHiGsCtxUpdate(window[i].gsctx);
    }

    _tex2d_arg.resident = FALSE;
    _tex2d_arg.tbl = sceHiGsMemAlloc(SCE_HIGS_PAGE_ALIGN, sceHiGsMemRestSize());

    _micro_arg.micro = 0;
    _hrchy_arg.root= &root_mat2;

    /* initialize model data */
    for (i=0; i<MODEL_NUM; i++){
	init_model(&model[i], files[i]);
    }
}

int SAMPLE13_main(void)
{
    sceHiErr	err;
    int i, j;
    struct _model *m;
    struct _window *w;
	

    model[1].hrchyP->args = (u_int)&_hrchy_arg;  /* a model of moving */
    root_ctrl();

    window_ctrl(&window[3]);  /* a window of moving */

    for (i=0; i<MODEL_NUM; i++){
	m= &model[i];

	if (sceHiCallPlug(m->frameP, SCE_HIG_PRE_PROCESS) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }


    utilSetDMATimer();
    for (j=0; j<WINDOW_NUM; j++){ /* each window */
	w= &window[j];

	/* a registration of gsctx for that window */
	if (j==2) sceHiGsCtxRegist(w->gsctx, 0); /* if #0, willnot do make original background clear */
	else sceHiGsCtxRegist(w->gsctx, 1); /* others clear on that */

	/* registration of a data of model drawing */
	for (i=0; i<MODEL_NUM; i++){
	    m= &model[i];

	    mycamera(m->microD, j);	/* setup lookatview */

	    /* registration of transmit */
	    if (sceHiCallPlug(m->frameP, SCE_HIG_POST_PROCESS) != SCE_HIG_NO_ERR)
		error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	}

	/* trasmit that windows */
	err = sceHiDMASend();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiDMAWait();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    /* return general context for sending the characters in main.c */
    err = sceHiGsCtxRegist(sceHiGsStdCtx, 0);	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiDMASend();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiDMAWait();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (gPad.StartSwitch)
	return 1;

    _micro_arg.micro = -1;

    return 0;
}

void SAMPLE13_exit(void)
{
    int i;
	
    for (i=0; i<MODEL_NUM; i++){
	exit_model(&model[i]);
    }

    for (i=0; i<WINDOW_NUM; i++){
	if (sceHiGsCtxDelete(window[i].gsctx)!=SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    sceHiGsMemFree(_tex2d_arg.tbl);
}

static void root_ctrl(void)
{
    static sceVu0FVECTOR root_trans2={7.5f, 5.0f, 28.0f, 0.0f};

    float  dt = 0.4f;

    if(gPad.RrightOn)	root_trans2[0] += dt;
    if(gPad.RleftOn)	root_trans2[0] -= dt;
    if(gPad.RdownOn)	root_trans2[1] += dt;
    if(gPad.RupOn)	root_trans2[1] -= dt;
    if(gPad.R1On)	root_trans2[2] -= dt;
    if(gPad.R2On)	root_trans2[2] += dt;

    sceVu0UnitMatrix(root_mat2);
    sceVu0TransMatrix(root_mat2, root_mat2, root_trans2);

    /* infomation */
    draw_debug_print(1, 6, "OBJ ROOT POS : %5.1f %5.1f %5.1f [Right Cursor]",
		     root_trans2[0],
		     root_trans2[1],
		     root_trans2[2]);
}

static void window_ctrl(struct _window *m)
{
    if (gPad.LdownOn){
	m->y+=2;
	if (m->y+m->h>224) m->y=224-m->h;
	gPad.LdownOn=0;
    } else if (gPad.LupOn){
	m->y-=2;
	if (m->y<0) m->y=0;
	gPad.LupOn=0;
    }

    if (gPad.LrightOn){
	m->x+=4;
	if (m->x+m->w>640) m->x=640-m->w;
	gPad.LrightOn=0;
    } else if (gPad.LleftOn){
	m->x-=4;
	if (m->x<0) m->x=0;
	gPad.LleftOn=0;
    }

    sceHiGsCtxSetRect(m->gsctx, m->x, m->y, m->w, m->h, SCE_HIGS_FBW_KEEP);
    sceHiGsCtxUpdate(m->gsctx);


    draw_debug_print(1, 8, "WINDOW POS : %3d, %3d  [Left Cursor]", m->x, m->y);
}
