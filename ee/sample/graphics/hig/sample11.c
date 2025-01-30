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
 * sample program sample11
 *
 *	The 2 screen synthesis that used pcrtc
 *	
 *      compose the assistant window which set in DISPLAY1 with pcrtc
 ***************************************************/

/* a infomation of assistant window */
#define WINDOW_WIDTH 128
#define WINDOW_HEIGHT 64
#define WINDOW_PSM SCE_GS_PSMCT32
/* Using bitmap image for sub-window */
#define WINDOW_IMAGE_FILE "data/subwin.raw"


/* arguments for DISPLAY1 */
static union {
    tGS_DISPFB1 st;
    u_long ul;
} _dispfb1;
static union {
    tGS_DISPLAY1 st;
    u_long ul;
} _display1;

/* parameters for indicate a assistatnt window */
static int _sendp=0;
static int _scalew, _scaleh;			// expansion rate
static int _fbw;				// buffer width of display1
static int _dispw, _disph;			// dimention on framebuf
static int _fbp;				// address of display
static int _dispx, _dispy;			// location on display
static int _psm;				// drawing mode
static int _magh, _magv;				// expantion rate of display2

/* for Vsync interrupt hander return */
static int (*_vblank_save)(int);

/* the data for normal model */
static void	*DATA_ADDR;

static sceHiPlug	*frameP;
static sceHiPlug	*microP;
static sceHiPlug	*tex2dP;
static sceHiGsMemTbl	*tbl = NULL;
static sceHiPlugTex2dInitArg_t		_tex2d_arg;
static sceHiPlugMicroPreCalcArg_t	_micro_arg;
static sceHiPlugMicroData_t *microD;

/* VSync End Interrupt function */
/* Send the change of DISPLAY1 to GS */
static int _set_display1(int ca)
{
    if (_sendp){				// only when something was changed
	DPUT_GS_DISPFB1(_dispfb1.ul);
	DPUT_GS_DISPLAY1(_display1.ul);
	_sendp=0;
    } 
    ExitHandler();
    if (_vblank_save) return _vblank_save(ca);
    else return 0;
}

/* setting of assistant window paramters */
static void _set_display1_size()
{
    int dw, dh, dx, dy;

    _dispfb1.st.FBP=_fbp;
    _dispfb1.st.FBW=(_fbw+63)/64;
    _dispfb1.st.PSM=_psm;
    _dispfb1.st.DBX=0;
    _dispfb1.st.DBY=0;

    dw=_dispw*_scalew*_magh;
    dh=_disph*_scaleh*_magv;
    dx=_dispx;
    dy=_dispy;
    _display1.st.DW= (dw-1)&0xfff;
    _display1.st.DH= (dh-1)&0xfff;

    _display1.st.DX=(dx+636)&0xfff;

    if (DGET_GS_CSR()&GS_CSR_FIELD_M) _display1.st.DY=(dy+50)&0xfff;
    else _display1.st.DY=(dy+50-(2*_scaleh)+1)&0xfff;

    _display1.st.MAGH=_scalew*_magh-1;
    _display1.st.MAGV=_scaleh*_magv-1;

    _sendp=1;					/* if changed, flag is on */
}

/* initialize for DISPLAY1 */
static void _set_display1_init()
{
    sceHiGsDisplay *_dsp;

    /* initial value */
    _dsp=sceHiGsDisplayStatus();
    _dsp->dbuf.disp[0].pmode.EN1=1;		// enable DISPLAY1
    _dsp->dbuf.disp[0].pmode.MMOD=0;		// Alpha (0: display1-alpha, 1: use ALP reg)
    _dsp->dbuf.disp[0].pmode.ALP=0x80;		// 0xff=1.0
    _dsp->dbuf.disp[1].pmode.EN1=1;		// enable DISPLAY1
    _dsp->dbuf.disp[1].pmode.MMOD=0;		// Alpha (0: display1-alpha, 1: use ALP reg)
    _dsp->dbuf.disp[1].pmode.ALP=0x80;		// 0xff=1.0


    _psm=WINDOW_PSM;
    _scalew=2;					/* expanstion rate (Horizen) */
    _scaleh=1;					// expanstion rate (Vertical) */
    _dispw=WINDOW_WIDTH;
    _disph=WINDOW_HEIGHT*2;			// *2: for interlace
    _fbw=_dispw;

    _magh=_dsp->dbuf.disp[0].display.MAGH+1;	/* get the viewing scale factor of usual window(display 2) */
    _magv=_dsp->dbuf.disp[0].display.MAGV+1;

    _dispx= ((640-(_dispw*_scalew))/2)*_magh;			/* location on Horizen */
    _dispy= (7*(448-(_disph*_scaleh))/8)*_magv;			/* location on verticle */


    /* send initial value */
    _set_display1_size();
    FlushCache(WRITEBACK_DCACHE);

    /* add VSync interrupt handler */
    _vblank_save=(int (*)(int))sceGsSyncVCallback(_set_display1);


}

/* finish display1 */
static void _set_display1_exit()
{
    sceHiGsDisplay *_dsp;

    _dsp=sceHiGsDisplayStatus();
    _dsp->dbuf.disp[0].pmode.EN1=0;
    _dsp->dbuf.disp[1].pmode.EN1=0;

    _sendp=0;

    if (_vblank_save) sceGsSyncVCallback(_vblank_save);
    else {
	sceGsGParam *gp;

	gp=sceGsGetGParam();
	DisableIntc(INTC_VBLANK_S);	
	RemoveIntcHandler(INTC_VBLANK_S, gp->sceGsVSCid);
	gp->sceGsVSCfunc=NULL;
	gp->sceGsVSCid=0;
    }

}

/* controll sub window */
static void _display1_ctrl()
{
    int isChanged=0;
	
    if (gPad.LrightSwitch){ _dispx+=96; isChanged++; }
    if (gPad.LleftSwitch) {_dispx-=96; isChanged++; }
    if (gPad.LdownSwitch) { _dispy+=32; isChanged++; }
    if (gPad.LupSwitch) { _dispy-=32; isChanged++; }

    if (gPad.L2Switch || gPad.RrightSwitch){ _scalew++; if (_scalew>4) _scalew=1; isChanged++; }
    if (gPad.RleftSwitch){ _scalew--; if (_scalew<1) _scalew=3; isChanged++; }
    if (gPad.L1Switch || gPad.RdownSwitch){ _scaleh++; if (_scaleh>4) _scaleh=1; isChanged++; }
    if (gPad.RupSwitch){ _scaleh--; if (_scaleh<1) _scaleh=3; isChanged++; }

    if (isChanged) _set_display1_size();	/* changese */
}

/* transmit the image of sub-window to framebuffer */
static void _trans_tex(u_int *texbuf, int psm, int tbp, int width, int height)
{

    sceGsLoadImage li;

    sceGsSetDefLoadImage(&li, tbp, (width+63)/64, psm, 0, 0, width, height);
    SyncDCache(&li, ((char *)&li)+sizeof(li));
    sceGsExecLoadImage(&li, (u_long128 *)texbuf);
    sceGsSyncPath(0,0);
}


void SAMPLE11_init(void)
{
	sceHiErr	err;
	u_int *texbuf;

	/* read a bitmap image for sub-window */
	texbuf=file_read(WINDOW_IMAGE_FILE);

	/* get a framebuffer area for subwindow */
	tbl = sceHiGsMemAlloc(SCE_HIGS_PAGE_ALIGN, WINDOW_WIDTH*WINDOW_HEIGHT);
	_fbp=tbl->addr/2048;

	/* transmit the bitmap to framebuffer */
	_trans_tex(texbuf, WINDOW_PSM, _fbp*(2048/64),
		  WINDOW_WIDTH, WINDOW_HEIGHT);

	/* init DISPLAY1 */
	_set_display1_init();

	/* viewing initialize */
	_tex2d_arg.resident = FALSE;
	_tex2d_arg.tbl = sceHiGsMemAlloc(SCE_HIGS_PAGE_ALIGN, 
					 SCE_HIGS_PAGE_ALIGN*sceHiGsMemRestSize()/SCE_HIGS_PAGE_ALIGN);

	DATA_ADDR = file_read("data/dino.bin");

	if ((err = sceHiParseHeader((u_int *)DATA_ADDR)) != SCE_HIG_NO_ERR) {
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	}

	if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Frame", &frameP)) != SCE_HIG_NO_ERR) {
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	}

	if((err = sceHiGetPlug((u_int *)DATA_ADDR, "Tex2d", &tex2dP)) == SCE_HIG_NO_ERR){
	    tex2dP->args = (u_int)&_tex2d_arg;
	}

	if ((err = sceHiCallPlug(frameP, SCE_HIG_INIT_PROCESS) != SCE_HIG_NO_ERR)) {
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	}

	/* camera & light */
	if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Micro", &microP)) != SCE_HIG_NO_ERR) {
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	}
	if((microD = sceHiPlugMicroGetData(microP)) == NULL)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	camerainit(microD);
	lightinit(microD);

	_micro_arg.micro = 0;      
}

int SAMPLE11_main(void)
{
	sceHiErr	err;

	/* controll subwindow */
	_display1_ctrl();

	/* micro code change? */
	microP->args = (u_int)&_micro_arg;

	if ((err = sceHiCallPlug(frameP, SCE_HIG_PRE_PROCESS) != SCE_HIG_NO_ERR)) {
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	}

	if ((err = sceHiCallPlug(frameP, SCE_HIG_POST_PROCESS) != SCE_HIG_NO_ERR)) {
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	}

	/* transmit only 1st */
	_micro_arg.micro = -1;

	utilSetDMATimer();
	err = sceHiDMASend();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiDMAWait();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	/* controll print */
	draw_debug_print(50, 1, "Rup/Rdown    : Scale Window Y");
	draw_debug_print(50, 2, "Rleft/Rright : Scale Window X");
	draw_debug_print(50, 3, "Lup/Ldown    : Move Window Y");
	draw_debug_print(50, 4, "Lleft/Lright : Move Window X");
	draw_debug_print(50, 5, "L1           : Scale Window Y");
	draw_debug_print(50, 6, "L2           : Scale Window X");
	draw_debug_print(50, 7, "START        : EXIT");

	if (gPad.StartSwitch)
		return 1;
	return 0;
}

void SAMPLE11_exit(void)
{
	sceHiErr	err;
	
	/* finish DISPLAY1 */
	_set_display1_exit();	

	if ((err = sceHiCallPlug(frameP, SCE_HIG_END_PROCESS) != SCE_HIG_NO_ERR)) {
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	}

	sceHiGsMemFree(tbl);
	sceHiGsMemFree(_tex2d_arg.tbl);

	free(DATA_ADDR);
}
