/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *
 *      Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*******************************************************/
/***************** dma kick utilities ******************/	
/*******************************************************/
#include <eekernel.h>
#include <eeregs.h>
#include <math.h>
#include <libgraph.h>
#include <libdma.h>
#include <libvu0.h>
#include <libdev.h>
#include <sifdev.h>

#define GetDp(id) ((id)?(0x70002000):(0x70000000))
#define FlushDCache()   FlushCache(0)

void WaitDMA(void);
void WaitVU1(void);
void dma_kick_normal(u_int madr, u_int qwc);
void dma_kick_normal_noflushD(u_int madr, u_int qwc);
void dma_kick_schain(u_int tadr);
void dma_kick_schain_waitVU1(u_int tadr);
void dma_kick_schain_noflushD(u_int tadr);
void dma_kick_schain_SPR(u_int tadr);

//Source Chain mode DMA kick from ScratchPad RAM to GIF(PATH3)
// tadr: source data start address
void dma_kick_schain_SPR_path3(u_int tadr)
{
	while(*D2_CHCR & 0x0100);
        *D2_QWC  = 0;
        *D2_TADR = tadr | 0x80000000;
	*D2_CHCR = 1 | (1<<2) | (0<<4) | (0<<6) | (0<<7) | (1<<8);
}

//Source Chain mode DMA kick from ScratchPad RAM to VIF1(PATH1 or PATH2)
// tadr: source data start address
void dma_kick_schain_SPR(u_int tadr)
{
	while(*D1_CHCR & 0x0100);
        *D1_QWC  = 0;
        *D1_TADR = tadr | 0x80000000;
	*D1_CHCR = 1 | (1<<2) | (0<<4) | (0<<6) | (0<<7) | (1<<8);
}

//Source Chain mode DMA kick from RDRAM to VIF1(PATH1 or PATH2)
// tadr: source data start address
void dma_kick_schain(u_int tadr)
{
	while(*D1_CHCR & 0x0100);
        *D1_QWC  = 0;
        *D1_TADR = tadr & 0x0fffffff;
        FlushCache(0);
	*D1_CHCR = 1 | (1<<2) | (0<<4) | (0<<6) | (0<<7) | (1<<8);
}

//Source Chain mode DMA kick from RDRAM to VIF1(PATH1 or PATH2) after VU1 end
// tadr: source data start address
void dma_kick_schain_waitVU1(u_int tadr)
{
	while(*D1_CHCR & 0x0100);
        *D1_QWC  = 0;
        *D1_TADR = tadr & 0x0fffffff;
        FlushCache(0);
	WaitVU1();
	*D1_CHCR = 1 | (1<<2) | (0<<4) | (0<<6) | (0<<7) | (1<<8);
}

//Source Chain mode DMA kick from RDRAM to VIF1(PATH1 or PATH2) without FlushDcache
// tadr: source data start address
void dma_kick_schain_noflushD(u_int tadr)
{
	while(*D1_CHCR & 0x0100);
        *D1_QWC  = 0;
        *D1_TADR = tadr & 0x0fffffff;
	*D1_CHCR = 1 | (1<<2) | (0<<4) | (0<<6) | (0<<7) | (1<<8);
}

//Normal mode DMA kick from RDRAM to VIF1(PATH1 or PATH2)
// tadr: source data start address
// qwc: Qword count
void dma_kick_normal(u_int madr, u_int qwc)
{
	while(*D1_CHCR & 0x0100);
        *D1_QWC  = qwc;
        *D1_MADR = madr & 0x0fffffff;
        FlushCache(0);
        *D1_CHCR = 1 | (0<<2) | (0<<4) | (0<<6) | (0<<7) | (1<<8);
}

//Normal mode DMA kick from RDRAM to VIF1(PATH1 or PATH2) without FlushDcache
// tadr: source data start address
// qwc: Qword count
void dma_kick_normal_noflushD(u_int madr, u_int qwc)
{
	while(*D1_CHCR & 0x0100);
        *D1_QWC  = qwc;
        *D1_MADR = madr & 0x0fffffff;
        *D1_CHCR = 1 | (0<<2) | (0<<4) | (0<<6) | (0<<7) | (1<<8);
}

//Check VPU status
unsigned int check_VPU_STAT(void)
{
	register u_int reg;

    	__asm__ volatile (" cfc2    %0,$vi29" : "=r"(reg));

	return(reg);
}

//Get cycle counter value
unsigned int GetCOUNT(void)
{
	register u_int reg;

	__asm__ volatile (" mfc0 %0, $09" : "=r"(reg));

	return(reg);
}

//Wait DMA end
void WaitDMA(void)
{
	__asm__ volatile ("\n\
WDMA:\n\
	bc0f WDMA\n\
	nop"
	);
}

//Wait VU1 end
void WaitVU1(void)
{
	__asm__ volatile ("\n\
 WVU1:\n\
	bc2t WVU1\n\
	nop"
	);
}

