/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *               Graphics Framework Sample Program
 *
 *
 *      Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          Name : gmain.c
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000      
 *
 */

#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libdma.h>
#include <libvu0.h>
#include <sifdev.h>
#include <string.h>
#include <malloc.h>
#include <math.h>


#include "defines.h"
#include "packbuf.h"
#include "data.h"
#include "frame_t.h"
#include "micro.h"
#include "object.h"
#include "camera.h"
#include "light.h"
#include "hrchy.h"
#include "anime.h"
#include "gmain.h"
#include "vumem1.h"
#include "pad.h"
#include "mode.h"
#include "info.h"
#include "shape.h"
#include "tex2d.h"
#include "model.h"

/***************************************************
 * constans defines
 ***************************************************/
#define BG_GRAY (0x40)

/***********************************************
 * global variable definitions
 ***********************************************/
fwLightGroup_t	fwMainParaLGroup;
fwLightGroup_t	fwMainSpotLGroup;
fwLightGroup_t	fwMainPointLGroup;

/***************************************************
 * static variable definition
 ***************************************************/

/*
 * Parallel Light Initial Value
 */
static fwLight_t _fwMainParaLightDefault	= {
    {0.7f, 0.0f, 0.7f, 0.0f},		/* color */
    {0.0f, 1.5f, 0.5f, 0.0f},		/* vector(direction) */
    {0.0f, 0.0f, 0.0f, 0.0f}		/* point (not used for Para-Light */
};

/*
 * Spot Light Initial Value
 */
static fwLight_t _fwMainSpotLightDefault[3]	= {
    {	/* FW_LIGHT_0 */
	{0.2f, 0.0f, 0.0f, 0.0f},		/* color */
	{-1.0f, 0.0f, 0.0f, 0.0f},		/* vector */
	{-7.5f, -3.0f, 6.0f, 1.0f},		/* point */
    },
    {	/* FW_LIGHT_1 */
	{0.0f, 0.2f, 0.0f, 0.0f},
	{1.0f, 0.0f, 0.0f, 0.0f},
	{7.5f, 0.0f, 0.0f, 0.0f}
    },
    {	/* FW_LIGHT_2 */
	{0.0f, 0.0f, 0.2f, 0.0f},
	{0.0f, 0.0f, 1.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 0.0f}
    }
};

/*
 * Point Light Initial Value
 */
static fwLight_t _fwMainPointLightDefault[3]	= {
    {	/* FW_LIGHT_0 */
	{0.2f, 0.0f, 0.0f, 0.0f},		/* color */
	{-1.0f, 0.0f, 0.0f, 0.0f},		/* vector */
	{-7.5f, -3.0f, 6.0f, 1.0f},		/* point */
    },
    {	/* FW_LIGHT_1 */
	{0.0f, 0.2f, 0.0f, 0.0f},
	{1.0f, 0.0f, 0.0f, 0.0f},
	{7.5f, 0.0f, 0.0f, 0.0f}
    },
    {	/* FW_LIGHT_2 */
	{0.0f, 0.0f, 0.2f, 0.0f},
	{0.0f, 0.0f, 1.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 0.0f}
    }
};

/*
 * Off Light Value
 */
static fwLight_t _fwMainOffLight = {
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f }
};

static sceGsDBuff _fwMainDBVal;		// disp&draw environment
static int _fwMainVu1workVal = 0;	// VUMEM1 workarea size
static fwModeType_t	_fwMainCurModeVal = ModelMODE;
static qword	*_fwMainGSInitChainVal;
static qword	*_fwMainGSResetChainVal;

/***********************************************
 * statical function declaration
 ***********************************************/
static void _fwMainPreCalc(u_int frame);
static void _fwMainPostCalc(u_int frame);
static void _fwMainMenuHandling(void);
static void _fwMainDMABufferInit(void);
static void _fwMainGSInit(void);
static void _fwMainVu1MemInit(void);
static void _fwMainTimerInit(void);
static void _fwMainGSFogColorInit(void);
static void _fwMainMakeGSIRChain(void);
static qword* _fwMainMakeGsPrmodecont(int);

/***************************************************
 * function definitions
 ***************************************************/
/* main loop for graphics */
void fwMain(u_int  frame)
{
    /* mode choice menu handling */
    _fwMainMenuHandling();

    /* exec mode */
    fwModeExec();

    /* user sequence */
    UserMain();

    *T0_COUNT = 0;

    /* Calculation previously Regist-Send-DMA(include camera/light) */
    _fwMainPreCalc(frame);

    /* Regist-Send-DMA */
    _fwMainPostCalc(frame);

    FlushCache(0);
    fwInfoStatusVal.cpu_count = *T0_COUNT;
    sceGsSyncPath(0, 0);

    /* Send DMA */
    fwInfoStatusVal.dma_count = *T0_COUNT;
    fwPackbufKickDMA(&fwPackbufKickBufVal);
    sceGsSyncPath(0, 0);
    fwInfoStatusVal.dma_count = *T0_COUNT - fwInfoStatusVal.dma_count;

    /* infomation disp */
    fwInfoStatusVal.frame = frame;
    fwInfoDispStatus();
    fwInfoStatusVal.vertNum = 0;
    fwInfoStatusVal.transTex = 0;
}

/* graphics operation during VSync */
void fwSync(u_int frame, int odev)
{
    static int dbp=0;
    sceGsDrawEnv1 *draw;

    draw= (dbp&1) ? &_fwMainDBVal.draw1 : &_fwMainDBVal.draw0;

    sceGsSetHalfOffset(draw, OFFX,OFFY,odev);
    SyncDCache(draw, (char *)draw+sizeof(sceGsDrawEnv1));
    sceGsSyncPath(0, 0);		// ****
    sceGsSwapDBuff(&_fwMainDBVal, dbp++);

}


/* graphics operation at the end of this program */
void fwFinish(void)
{
    sceGsSyncPath(0, 0);
    fwModeExit();
    fwInfoExit();
}


/* read and initialize model data */
fwObject_t* fwMainReadModel(fwDataHead_t *data)
{
    fwObject_t	*obj;

    obj = fwObjectCreate(data);

    return obj;
}

void fwInit(void)
{
    UserInit();	/* user initialize */
    _fwMainDMABufferInit();
    _fwMainGSInit();
    _fwMainGSFogColorInit();
    fwInfoInit();
    _fwMainVu1MemInit();
    _fwMainTimerInit();
    fwModeInit(_fwMainCurModeVal);
    _fwMainMakeGSIRChain();

    fwCameraInit();
    fwLightInit();

    /* light set */
    /* Micro Codeの制限で GROUP 0は Pararrel Light */
    /* GROUP 1は Point Light, GROUP 2は Spot Light */
    fwMainParaLGroup = FW_LGROUP_0;
    fwMainPointLGroup = FW_LGROUP_1;
    fwMainSpotLGroup = FW_LGROUP_2;
    /* pallarel light for basic/cull/anti micro code */
    fwLightSet(fwMainParaLGroup, FW_LIGHT_0, &_fwMainParaLightDefault);
    fwLightSet(fwMainParaLGroup, FW_LIGHT_1, &_fwMainOffLight);
    fwLightSet(fwMainParaLGroup, FW_LIGHT_2, &_fwMainOffLight);
    /* point light for point micro code */
    fwLightSet(fwMainPointLGroup, FW_LIGHT_2, &_fwMainPointLightDefault[0]);
    fwLightSet(fwMainPointLGroup, FW_LIGHT_0, &_fwMainPointLightDefault[1]);
    fwLightSet(fwMainPointLGroup, FW_LIGHT_1, &_fwMainPointLightDefault[2]);
    /* spot light for spot micro code */
    fwLightSet(fwMainSpotLGroup, FW_LIGHT_0, &_fwMainSpotLightDefault[0]);
    fwLightSet(fwMainSpotLGroup, FW_LIGHT_1, &_fwMainSpotLightDefault[1]);
    fwLightSet(fwMainSpotLGroup, FW_LIGHT_2, &_fwMainSpotLightDefault[2]);
}

static void _fwMainDMABufferInit(void)
{
    /* Kick List用DMAバッファの初期化 */
    fwPackbufKickBufVal.size = KICK_SIZE;
    fwPackbufKickBufVal.pHead = (qword *)memalign(16, fwPackbufKickBufVal.size * sizeof(qword));
    fwPackbufKickBufVal.pCurr = fwPackbufKickBufVal.pHead;

    /* 各種Data統轄用DMA バッファの初期化 */
    fwPackbufVal.size = PACK_SIZE + SHAPE_PACK_SIZE;
    fwPackbufVal.pHead = (qword *)memalign(16, fwPackbufVal.size * sizeof(qword));
    fwPackbufVal.pCurr = fwPackbufVal.pHead;
}

static void _fwMainGSInit(void)
{
    static struct {
	sceGifTag	giftag;
	sceGsTest	gs_test;
	long		gs_test1addr;
	sceGsAlphaEnv	gs_alpha;
	sceGsTexEnv	gs_tex;
    } texenv;

    sceGsResetPath();
    sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);

    /* --- set double buffer ---*/
    sceGsSetDefDBuff(&_fwMainDBVal, SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT,
		     SCE_GS_ZGREATER, SCE_GS_PSMZ24, SCE_GS_CLEAR);
    *(u_long *)&_fwMainDBVal.clear0.rgbaq =
      SCE_GS_SET_RGBAQ(BG_GRAY, BG_GRAY, BG_GRAY, 0x80, 0x3f800000);
    *(u_long *)&_fwMainDBVal.clear1.rgbaq =
      SCE_GS_SET_RGBAQ(BG_GRAY, BG_GRAY, BG_GRAY, 0x80, 0x3f800000);
    FlushCache(0);

    /* texture environmentの設定 */
    /* alphaの設定はしておかないと debug表示の文字が滲むので注意 */
    sceGsSetDefTexEnv(&texenv.gs_tex, 0, 6720, 256/64, SCE_GS_PSMCT32,
		      8, 8, 0, 0, 0, 0, 1);
    SCE_GIF_CLEAR_TAG(&texenv.giftag);
    texenv.giftag.NLOOP = 9;
    texenv.giftag.EOP = 1;
    texenv.giftag.NREG = 1;
    texenv.giftag.REGS0 = 0xe;
    sceGsSetDefAlphaEnv(&texenv.gs_alpha, 0);
    *(u_long *) &texenv.gs_alpha.alpha1 = SCE_GS_SET_ALPHA(0, 1, 0, 1, 0);
    *(u_long *) &texenv.gs_test = SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, 0x3);
    texenv.gs_test1addr = (long)SCE_GS_TEST_1;
    FlushCache(0);
    sceGsPutDrawEnv(&texenv.giftag);
    sceGsSyncPath(0, 0);
}

static void _fwMainVu1MemInit(void)
{
    /* ここの計算は厳密じゃないので注意 fixme! */
    u_int bufsize = (1024-120-_fwMainVu1workVal)/ 2; // size for double buffering
    u_int isize_per_vertex=4;			// in:ST/RGB/NRM/VTX
    u_int osize_per_vertex=4;			// out:(PRMODE)/ST/RGB/XYZF
    u_int ihead_size = 2;
    u_int ohead_size = 4;
    u_int vnum, isize, osize;

    vnum=bufsize/(isize_per_vertex+osize_per_vertex);
    osize=vnum*osize_per_vertex + ohead_size;
    isize=bufsize-osize-ihead_size;
    fwVumem1SetParam(isize, osize);
}

static void _fwMainTimerInit(void)
{
    // start TIMER T0
    *T0_MODE = 2  | (0<<6) | (1<<7) | (0<<8);	
}

static void _fwMainGSFogColorInit(void)
{
    struct gif_packet {
	u_long	gif_tag[2];
	qword	fogcol;
    };
    struct gif_packet	packet;

    packet.gif_tag[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 1);
    packet.gif_tag[1] = 0x0eL;
    SET_QW(packet.fogcol, 0x00000000, 0x0000003d, 0x00000000, 0x00404040);

    *D2_QWC	= 0x00000002;
    *D2_MADR	= (u_int) &packet.gif_tag & 0x0fffffff;
    *D_STAT	= 2;
    FlushCache(0);
    *D2_CHCR = 1 | (0 << 2) | (0 << 4) | (0 << 5) | (0 << 6) | (0 << 7) | (1 << 8);
    sceGsSyncPath(0, 0);
}

/*
 * _fwMainMenuHandling()
 *	PADによるモードの切替を制御する
 */
static void _fwMainMenuHandling()
{
    if (fwPadStateVal.StartSwitch) {
	_fwMainCurModeVal++;
	if (_fwMainCurModeVal >= MODE_MAX)
	    _fwMainCurModeVal = 0;
	fwModeChange(_fwMainCurModeVal);
    }
}

// pre-calculation for matrix, animation, and shared-objects w/CPU+VU0 
static void _fwMainPreCalc(u_int frame)
{
    fwModelID_t		id;
    fwObject_t		*obj;

    for (id = 0; id < MAX_MODEL; id++)
	if (fwModelIsActive(id)) {
	    obj = fwModelGetDataAddr(id);
	    /* calc total vertex of activate models */
	    if (obj->shape)
		fwInfoStatusVal.vertNum += obj->shape->VertexTotal;

	    /* calc total translation tex size of activate models */
	    if (obj->tex2d)
		fwInfoStatusVal.transTex += obj->tex2d->TransSizeTotal;

	    /* pre calc */
	    fwObjectPreCalc(obj);
	    /* camera */
	    fwCameraSet(obj);
	    /* light */
	    fwLightApply(obj);
	}
}

static void _fwMainPostCalc(u_int frame)
{
    fwMicroType_t	m;
    fwModelID_t		id;

    fwPackbufReset(&fwPackbufKickBufVal);
    m = fwMicroGetCurrent();

    for (id = 0; id < MAX_MODEL; id++) {
	if (fwModelIsActive(id))
	    fwObjectPostCalc(fwModelGetDataAddr(id));
    }
}

static void _fwMainMakeGSIRChain(void)
{
    /* GS初期化Chain */
    _fwMainGSInitChainVal = _fwMainMakeGsPrmodecont(0);

    /* GSリセットChain */
    _fwMainGSResetChainVal = _fwMainMakeGsPrmodecont(1);
}

static qword* _fwMainMakeGsPrmodecont(int ac)
{
    union {
	u_long 	ul[2];
	qword	qw;
    } gif_tag;
    qword	*buf, *curr;

    buf = fwPackbufVal.pCurr;
    curr = buf;

    gif_tag.ul[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 1);
    gif_tag.ul[1] = 0x0eL;
    RET_NOP(*curr, 2);
    ADD_FLUSH(*curr, 2);
    ADD_DIRECT(*curr, 3, 2);
    curr++;
    COPY_QW((u_int)*curr, (u_int)gif_tag.qw);
    curr++;
    SET_QW(*curr, 0x00000000, 0x0000001a, 0x00000000, ac);
    curr++;

    fwPackbufVal.pCurr = curr;

    return buf;
}
