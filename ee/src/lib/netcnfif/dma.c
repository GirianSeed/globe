/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/* 
 *      Netcnf Interface Library
 *
 *                         Version 1.1
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
 */

#include <eekernel.h>
#include <sif.h>

/* IOP へデータを送信 */
int sceNetcnfifSendIOP(unsigned int data, unsigned int addr, unsigned int size)
{
    sceSifDmaData fdma;
    int id;

    fdma.data = data;
    fdma.addr = addr;
    fdma.size = (size & 0xffffffc0) + ((size % 0x40) ? 0x40 : 0);
    fdma.mode = 0;
    FlushCache(0);
    id = sceSifSetDma(&fdma, 1);
    return (id);
}

/* SIF DMA 処理状況の判定 */
int sceNetcnfifDmaCheck(int id)
{
    return (sceSifDmaStat(id) < 0 ? 0 : 1);
}
