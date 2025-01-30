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
 *                          Name : packbuf.c
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            0.30      Apr,4,2000     thatake     Beta0
 *            0.40      Jun,16,2000     	   +animation, clut
 *            2.1       Nov,17,2000
 */

#include <eekernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <eeregs.h>
#include <libvu0.h>
#include <malloc.h>
#include "defines.h"
#include "packbuf.h"
#include <eestruct.h>

fwPackbuf_t fwPackbufVal;
fwPackbuf_t fwPackbufKickBufVal;

void fwPackbufInit(fwPackbuf_t *packet, int size, u_int mask){
    packet->pHead = (qword*)memalign(64, bound(sizeof(u_long128)*size, 64));
    if (packet->pHead == NULL){
	ErrPrintf("Vu1PacketBufInit: cannot malloc\n");
	return;
    }
    packet->pHead=(qword *)(mask|(u_int)packet->pHead);
    packet->pCurr = packet->pHead;
    packet->size=size;
}

void fwPackbufFinish(fwPackbuf_t *packet)
{
    free((u_int *)Paddr(packet->pHead));
}

void fwPackbufReset(fwPackbuf_t *packet)
{
    packet->pCurr = packet->pHead;
    SET_QW(*(packet->pCurr), 0x0, 0x0, 0x0, 0x70000000);
}

int fwPackbufCheckOverflow(fwPackbuf_t *packet){
    if ((int)packet->pCurr > (int)packet->pHead + (packet->size*sizeof(u_long128))){
	return 1;				// too many packets
    } else{
	return 0;				// OK
    }
}

/*
  fwPackbufAddCall(pbuf, tagbuf)
  pbuf‚Ìcurrent pointer‚Étagbuf‚ðcall‚·‚éDMA tag‚ð’Ç‰Á
*/
void fwPackbufAddCall(qword *dst, qword *tag)
{
    SET_QW(*dst, 0x0, 0x0, ((u_int)tag)&0x0fffffff, 0x50000000); // call
    SET_QW(*(dst + 1), 0x0, 0x0, 0x0, 0x70000000); // end
}

void fwPackbufKickDMA(fwPackbuf_t *kickbuf)
{
    *D1_QWC  = 0x00;
    *D1_TADR = Paddr(kickbuf->pHead);
    *D_STAT = 2;
    *D1_CHCR = 1 | (1<<2) | (0<<4) | (1<<6) | (0<<7) | (1<<8);
}
