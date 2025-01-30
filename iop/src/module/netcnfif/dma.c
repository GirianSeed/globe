/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0.2
 */
/* $Id: dma.c,v 1.5 2003/07/17 09:33:06 ksh Exp $ */
/* 
 *      Netcnf Interface Library
 *
 *                         Version 1.5
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         dma.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 *       1.2            2002.06.26      tei         remove FlushDcache().
 *       1.3            2002.06.26      tei         add RCS ID
 *       1.4            2002.07.04      tetsu       add Log
 *       1.5            2003.07.17      ksh         add retry
 */

#include <kernel.h>
#include <sif.h>

int sceNetcnfifSendEE(unsigned int data, unsigned int addr, unsigned int size)
{
	sceSifDmaData fdma;
	int id;
	static int oldstat;

	fdma.data = data;
	fdma.addr = addr;
	fdma.size = (size & 0xffffffc0) + ((size % 0x40) ? 0x40 : 0);
	fdma.mode = 0;

	do {
		CpuSuspendIntr(&oldstat);
		id = sceSifSetDma(&fdma, 1);
		CpuResumeIntr(oldstat);
	} while (!id);

	return (id);
}

int sceNetcnfifDmaCheck(int id)
{
	return (sceSifDmaStat(id) < 0 ? 0 : 1);
}

