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
#include <libgraph.h>
#include <libvu0.h>
#include <libpad.h>
#include <sifdev.h>
#include <malloc.h>
#include <libdev.h>
#include <libhig.h>
#include <libhip.h>
#include "util.h"
#include "camera.h"
#include "light.h"

/********************************************************/
/*	Pad Control					*/
/********************************************************/
sceSamplePAD	gPad;
static u_long128 _pad_buf[scePadDmaBufferMax] __attribute__((aligned(64)));

void pad_init(void)
{
	scePadInit(0);
	scePadPortOpen(0, 0, _pad_buf);
}

void pad_ctrl(void)
{
	static u_short	oldpad = 0;
	u_short		pad;
	u_char		rdata[32];

	if (scePadRead(0, 0, rdata) > 0) 
		pad = 0xffff ^ ((rdata[2] << 8) | rdata[3]);
	else
		pad = 0;

	gPad.LleftOn	= pad & SCE_PADLleft ? 1 : 0;
	gPad.LupOn	= pad & SCE_PADLup ? 1 : 0;
	gPad.LrightOn	= pad & SCE_PADLright ? 1 : 0;
	gPad.LdownOn	= pad & SCE_PADLdown ? 1 : 0;
	gPad.L1On	= pad & SCE_PADL1 ? 1 : 0;
	gPad.L2On	= pad & SCE_PADL2 ? 1 : 0;
	gPad.RleftOn	= pad & SCE_PADRleft ? 1 : 0;
	gPad.RupOn	= pad & SCE_PADRup ? 1 : 0;
	gPad.RrightOn	= pad & SCE_PADRright ? 1 : 0;
	gPad.RdownOn	= pad & SCE_PADRdown ? 1 : 0;
	gPad.SelectOn	= pad & SCE_PADselect ? 1 : 0;
	gPad.StartOn	= pad & SCE_PADstart ? 1 : 0;
	gPad.R1On	= pad & SCE_PADR1 ? 1 : 0;
	gPad.R2On	= pad & SCE_PADR2 ? 1 : 0;
	gPad.LleftSwitch	= (pad & SCE_PADLleft) && (!(oldpad & SCE_PADLleft)) ? 1 : 0;
	gPad.LupSwitch		= (pad & SCE_PADLup) && (!(oldpad & SCE_PADLup)) ? 1 : 0;
	gPad.LrightSwitch	= (pad & SCE_PADLright) && (!(oldpad & SCE_PADLright)) ? 1 : 0;
	gPad.LdownSwitch	= (pad & SCE_PADLdown) && (!(oldpad & SCE_PADLdown)) ? 1 : 0;
	gPad.L1Switch		= (pad & SCE_PADL1) && (!(oldpad & SCE_PADL1)) ? 1 : 0;
	gPad.L2Switch		= (pad & SCE_PADL2) && (!(oldpad & SCE_PADL2)) ? 1 : 0;
	gPad.RleftSwitch	= (pad & SCE_PADRleft) && (!(oldpad & SCE_PADRleft)) ? 1 : 0;
	gPad.RupSwitch		= (pad & SCE_PADRup) && (!(oldpad & SCE_PADRup)) ? 1 : 0;
	gPad.RrightSwitch	= (pad & SCE_PADRright) && (!(oldpad & SCE_PADRright)) ? 1 : 0;
	gPad.RdownSwitch	= (pad & SCE_PADRdown) && (!(oldpad & SCE_PADRdown)) ? 1 : 0;
	gPad.R1Switch		= (pad & SCE_PADR1) && (!(oldpad & SCE_PADR1)) ? 1 : 0;
	gPad.R2Switch		= (pad & SCE_PADR2) && (!(oldpad & SCE_PADR2)) ? 1 : 0;
	gPad.SelectSwitch	= (pad & SCE_PADselect) && (!(oldpad & SCE_PADselect)) ? 1 : 0;
	gPad.StartSwitch	= (pad & SCE_PADstart) && (!(oldpad & SCE_PADstart)) ? 1 : 0;

	oldpad = pad;
}

/********************************************************/
/*	File Read					*/
/********************************************************/
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

/********************************************************/
/*	Debug Print					*/
/********************************************************/
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

/********************************************************/
/*	Dma Timer					*/
/********************************************************/
static int _dma_timer_s, _dma_timer_e;	/* start, end */

static int _dma_timer_handler(int ch)
{
    _dma_timer_e = *T0_COUNT;

    ExitHandler();
    return 0;
}

void utilInitDMATimer(void)
{
    AddDmacHandler(DMAC_VIF1, _dma_timer_handler, 0);
    EnableDmac(DMAC_VIF1);
}

void utilSetDMATimer(void)
{
    _dma_timer_s = *T0_COUNT;
}

int utilGetDMATimerStart(void)
{
    return _dma_timer_s;
}

int utilGetDMATimerEnd(void)
{
    return _dma_timer_e;
}

void utilResetDMATimer(void)
{
    _dma_timer_s = _dma_timer_e = 0;
}

/********************************************************/
/*	Error Handling					*/
/********************************************************/
void error_handling(char *file, int line, const char *mes)
{
    printf("%s:%d: %s\n",file,line,mes);
    while (1)
	;
}
