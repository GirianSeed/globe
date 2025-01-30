/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *        Emotion Engine Library Sample Program
 *
 *                         - libgp: multiwin -
 *
 *                         Version 1.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            util.c
 *		libgp + GS Service (to change draw environment)
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            Sep,20,2001      aoki
 */
#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libvu0.h>
#include <libpad.h>
#include <sifdev.h>
#include <malloc.h>
#include <libdev.h>
#include <libhig.h>
#include <libhip.h>
#include "util.h"

/* FOR PAD CTRL */
PadData	gPad;
static u_long128 _pad_buf[scePadDmaBufferMax] __attribute__((aligned(64)));




void pad_init(void)
{
    scePadInit(0);
    scePadPortOpen(0, 0, _pad_buf);
}

void pad_ctrl(void)
{
    static u_short	oldpad = 0;
    u_char		rdata[32];
    u_int data;
	

    if (scePadRead(0, 0, rdata) > 0) 
	data = 0xffff ^ ((rdata[2] << 8) | rdata[3]);
    else
	data = 0;

    data |= (data << 16);
    data &= ~oldpad;
    *(u_int *)&gPad=data;
    oldpad = data>>16;
}


void error_handling(char *file, int line, const char *mes)
{
    printf("%s:%d: %s\n",file,line,mes);
    while (1)
	;
}
