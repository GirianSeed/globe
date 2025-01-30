/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *      Inet Event Sample Program
 *
 *                         Version 1.1
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         pad.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.02.18      tetsu       Change directory
 */

#include <eeregs.h>
#include <libpad.h>

static int port, slot;

/* パッドの初期化 */
void pad_setup(void)
{
    static u_long128 pad_dma_buf[scePadDmaBufferMax] __attribute__((aligned (64)));

    scePadInit(0);

    port = 0;
    slot = 0;
    scePadPortOpen( port, slot, pad_dma_buf );
}

/* パッド情報の取得 */
u_short pad_read(void)
{
    unsigned char rdata[32];
    unsigned short paddata, tpad;
    static unsigned short rpad = 0;
    static unsigned short rept = 0;

    if(scePadRead(port, slot, rdata) == 0) return (0);

    if(rdata[0] == 0){
	paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);

	if(paddata == rpad){
	    rept++;
	}else{
	    rept = 0;
	}

	tpad = paddata & ~rpad;
	rpad = paddata;

	/* リピートスピードの調整 */
	if(rept >= 20){
	    tpad = paddata;
	    rept -= 7;
	}
	return (tpad); 
    }else {
	return (0);
    }
}
