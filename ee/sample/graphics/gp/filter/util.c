/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *        Emotion Engine Library Sample Program
 *
 *                         - libgp: filtering -
 *
 *                         Version 1.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            util.c
 *                     simple 2D filtering w/ libgp
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

void *file_read(char *fname)
{
	char	file[256];
	size_t	size;
	void	*dptr;
	int	fd;

	sprintf(file, "host0:%s", fname);
	if ((fd = sceOpen(file, SCE_RDONLY)) < 0) {
		printf("Can't open file %s\n", fname);
		while (1)
			;
	}
	size = sceLseek(fd, 0, SCE_SEEK_END);
	sceLseek(fd, 0, SCE_SEEK_SET);

	dptr = memalign(64, size);
	if (dptr == NULL) {
		printf("Can't allocate memory for file reading %s\n", fname);
		while (1)
			;
	}
	if (sceRead(fd, (u_int *)dptr, size) < 0) {
		printf("Can't read file %s\n", fname);
		while (1)
			;
	}
	FlushCache(0);
	sceClose(fd);
	return dptr;
}

static int	_consID;		/* debug console ID */

void draw_debug_print_init(void)
{
    sceDevConsInit();
    _consID = sceDevConsOpen(((4096 - 640) / 2) << 4,
			     ((4096 - 224) / 2) << 4,
			     79,
			     40);
}

void draw_debug_print(int x, int y, char *fmt, ...)
{
	va_list	ap;
	char	strings[640 / 4];

	va_start(ap, fmt);
	vsprintf(strings, fmt, ap);
	va_end(ap);

	if (strings[0]) {
		sceDevConsLocate(_consID, x, y);
		sceDevConsPrintf(_consID, strings);
	}	
}

void draw_debug_print_exec(void)
{
	sceDevConsDraw(_consID);
	sceDevConsClear(_consID);
}

void error_handling(char *file, int line, const char *mes)
{
    printf("%s:%d: %s\n",file,line,mes);
    while (1)
	;
}
