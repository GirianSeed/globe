/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
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

#include <kernel.h>
#include <sif.h>

/* EE へデータを送信 */
int sceNetcnfifSendEE(unsigned int data, unsigned int addr, unsigned int size)
{
    sceSifDmaData fdma;
    int id;
    static int oldstat;

    fdma.data = data;
    fdma.addr = addr;
    fdma.size = (size & 0xffffffc0) + ((size % 0x40) ? 0x40 : 0);
    fdma.mode = 0;
    FlushDcache();
    CpuSuspendIntr(&oldstat);
    id = sceSifSetDma(&fdma, 1);
    CpuResumeIntr(oldstat);

    return (id);
}

/* SIF DMA 処理状況の判定 */
int sceNetcnfifDmaCheck(int id)
{
    return (sceSifDmaStat(id) < 0 ? 0 : 1);
}
