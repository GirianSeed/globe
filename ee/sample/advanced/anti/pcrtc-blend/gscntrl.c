/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *      Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*
*
*	VU & GS test data generator
*
*/
#include <stdlib.h>
#include <stdio.h>
#include <eekernel.h>
#include <eeregs.h>
#include <math.h>
#include <libgraph.h>
#include <libdma.h>
#include <libpkt.h>
#include <libvu0.h>
#include <libdev.h>
#include <sifdev.h>
#include <eestruct.h>
#include "base.h"

void GS_runtime_preset(u_int frm, int flg);

/*  PCRTCのマージ回路によるアンチエイリアシング */


void GS_runtime_preset(u_int frm, int flg) 		// flg=１でアンチエイリアシング OFF
{
	int dx,dy;
	dx=0;
	dy=20;
	//  GS RUNTIME MODE 
	// 
	/* flg=1でマージ回路のブレンド係数が0.5（127）にセットされ、DISPLAY1とDISPLAY2がブレンドされる。*/
	/* DISPLAY1とDISPLAY2は同じ表示バッファを出力するがdyの操作(+1)により、ODD，EVENがブレンドされる*/
	/* ことになりアンチエイリアシングが動作する。*/

	//                            (en1, en2, crtmd, mmod, amod,slbg, alp) 
	*GS_PMODE    = SCE_GS_SET_PMODE(1,   1,   0     ,1,    0,   0,    (flg&1)?(255):(127));	// PMODE
	// (fbp, fbw, psm, dbx, dby)
	*GS_DISPFB2  =  SCE_GS_SET_DISPFB((frm&1)?(DBUF1):(DBUF0), DISPWIDTH/64, 0, 0, 0);	// DISPFB2
	// (dx, dy, magh, magv, dw, dh) 
	*GS_DISPLAY2 = SCE_GS_SET_DISPLAY(dx, dy, 3, 0, DISPWIDTH*4-1, DISPHEIGHT-1);	// DISPLAY2
	// (fbp, fbw, psm, dbx, dby)
	*GS_DISPFB1  =  SCE_GS_SET_DISPFB((frm&1)?(DBUF1):(DBUF0), DISPWIDTH/64, 0, 0, 0);			// DISPFB1
	// (dx, dy, magh, magv, dw, dh) 
	*GS_DISPLAY1 =  SCE_GS_SET_DISPLAY(dx, dy+1, 3, 0, DISPWIDTH*4-1, DISPHEIGHT-1);		// DISPLAY1
	// (write)
	*GS_EXTWRITE = SCE_GS_SET_EXTWRITE(0); 						// NO EXTWRITE
}

