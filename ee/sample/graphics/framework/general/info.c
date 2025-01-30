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
 *                          Name : info.c
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000  
 *
 */

/*******************************************************
 * 概要
 *	Frame Work内における 情報表示の為の関数群
 *	毎フレーム fwInfoDisp()を呼出す事
 *	実際の描画は そこでおこなわれる
 *******************************************************/

#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <libvu0.h>
#include <libdev.h>
#include <string.h>

#include "defines.h"
#include "info.h"

/*******************************************************
 * static constants
 *******************************************************/
/* buff size for DrawStatus() */
#define STRING_SIZE (SCREEN_WIDTH/4)

/*******************************************************
 * global variables
 *******************************************************/
fwInfoStatus_t	fwInfoStatusVal;

/*******************************************************
 * static variables
 *******************************************************/
static int		_fwInfoConsIDVal;

/*******************************************************
 * static functions
 *******************************************************/

/*******************************************************
 * fwInfoInit
 *******************************************************/
void fwInfoInit(void)
{
    sceDevConsInit();
    _fwInfoConsIDVal = sceDevConsOpen(((4096 - SCREEN_WIDTH) / 2) << 4,
				      ((4096 - SCREEN_HEIGHT) / 2) << 4,
				      79, 40);
    fwInfoStatusVal.micro_name = NULL;
    fwInfoStatusVal.vertNum = 0;
    fwInfoStatusVal.transTex = 0;
    fwInfoStatusVal.cpu_count = 0;
    fwInfoStatusVal.dma_count = 0;
    fwInfoStatusVal.mode[0] = '\0';
    fwInfoStatusVal.frame = 0;
}

/*******************************************************
 * fwInfoExit
 *******************************************************/
void fwInfoExit(void)
{
    sceDevConsClose(_fwInfoConsIDVal);
}

/*******************************************************
 * fwInfoDispStatus
 *******************************************************/
void fwInfoDispStatus(void)
{
    float	cpu_pow, dma_pow;
    int		xpos, ypos;

    xpos = ypos = 1;

    cpu_pow = ((float) fwInfoStatusVal.cpu_count * 100.0f) / 9765.625f;
    dma_pow = ((float) fwInfoStatusVal.dma_count * 100.0f) / 9765.625f;

    if (fwInfoStatusVal.filename) {
	fwInfoDraw(xpos, ypos++, "%s", fwInfoStatusVal.filename);
    }
    fwInfoDraw(xpos, ypos++, fwInfoStatusVal.micro_name);
    fwInfoDraw(xpos, ypos++, "VERTICES %d, TEXTURE %.2fMb",
					fwInfoStatusVal.vertNum,
					fwInfoStatusVal.transTex);
    fwInfoDraw(xpos, ypos++, "USAGE: CPU %3.1f%%, DMA %3.1f%%",
					cpu_pow, dma_pow);
    fwInfoDraw(xpos, ypos++, "FRAME : %6u", fwInfoStatusVal.frame);
    fwInfoDraw(xpos, ypos++, "MODE : %s", fwInfoStatusVal.mode);

    sceDevConsDraw(_fwInfoConsIDVal);
    sceDevConsClear(_fwInfoConsIDVal);
}

void fwInfoDraw(int x, int y, const char *fmt, ...)
{
    va_list	ap;
    char	sbuf[STRING_SIZE];

    va_start(ap, fmt);
    vsprintf(sbuf, fmt, ap);
    va_end(ap);

    if (sbuf[0]) {	/* not a null string */
	sceDevConsLocate(_fwInfoConsIDVal, x, y);
	sceDevConsPrintf(_fwInfoConsIDVal, sbuf);
    }
}
