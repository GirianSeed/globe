/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.7
 */
/*
 *                      Emotion Engine Library
 *                          Version 0.01
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        libpkt - libgifpk.c
 *                        gif packet support
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.01           Mar,26,1999     shibuya
 */

#include "libgifpk.h"

#include <eeregs.h>

#define __ALIGNED16__ __attribute__((aligned(16)))

__inline__ static u_int CV2P(void* const pPtr){
  return (u_int)pPtr & 0x9fffffff;
}

__inline__ static u_long128* align128(u_int* pPtr){
  while((u_int)pPtr & 0xc){
    *pPtr++ = 0;
  }
  return (u_long128*)pPtr;
}

__inline__ static u_long* memcpy64(u_long* pDst, u_long const* pSrc, u_int count){
  while(count--){
    *pDst++ = *pSrc++;
  }
  return pDst;
}

__inline__ static u_long128* memcpy128(u_long128* pDst, u_long128 const* pSrc, u_int count){
  while(count--){
    *pDst++ = *pSrc++;
  }
  return pDst;
}

#ifdef FUNC000
void sceGifPkInit(sceGifPacket *pPacket, u_long128 *pBase){
  pPacket->pBase = pBase;
  pPacket->pCurrent = (u_int*)pPacket->pBase;
  pPacket->pDmaTag = NULL;
}

#endif
#ifdef FUNC001

void sceGifPkReset(sceGifPacket* pPacket){
  pPacket->pCurrent = (u_int*)pPacket->pBase;
  pPacket->pDmaTag = NULL;
}

#endif
#ifdef FUNC002

u_long128 *sceGifPkTerminate(sceGifPacket *pPacket){
  u_long128* const pDmaCur = align128(pPacket->pCurrent);
  if(pPacket->pDmaTag){
    u_int const count = (pDmaCur - pPacket->pDmaTag) - 1;
    ((u_int*)pPacket->pDmaTag)[0] += count;
  }
  pPacket->pCurrent = (u_int*)pDmaCur;
  pPacket->pDmaTag = NULL;
  return pDmaCur;
}

#endif
#ifdef FUNC003

u_int sceGifPkSize(sceGifPacket *pPacket){
  return ((u_int)pPacket->pCurrent - (u_int)pPacket->pBase) / sizeof(u_long128);
}

#endif
#ifdef FUNC004

void sceGifPkRefe(sceGifPacket *pPacket, u_long128 *pRef, u_int size, u_int opt1, u_int opt2, u_int flag){
  sceGifPkTerminate(pPacket);
  *pPacket->pCurrent++ = (0 << 28) | flag | size;
  *pPacket->pCurrent++ = CV2P(pRef);
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC005

void sceGifPkCnt(sceGifPacket *pPacket, u_int opt1, u_int opt2, u_int flag){
  pPacket->pDmaTag = sceGifPkTerminate(pPacket);
  *pPacket->pCurrent++ = (1 << 28) | flag;
  *pPacket->pCurrent++ = 0;
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC006

void sceGifPkNext(sceGifPacket *pPacket, u_long128 *pNext, u_int opt1, u_int opt2, u_int flag){
  pPacket->pDmaTag = sceGifPkTerminate(pPacket);
  *pPacket->pCurrent++ = (2 << 28) | flag;
  *pPacket->pCurrent++ = CV2P(pNext);
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC007

void sceGifPkRef(sceGifPacket *pPacket, u_long128 *pRef, u_int size, u_int opt1, u_int opt2, u_int flag){
  sceGifPkTerminate(pPacket);
  *pPacket->pCurrent++ = (3 << 28) | flag | size;
  *pPacket->pCurrent++ = CV2P(pRef);
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC008

void sceGifPkRefs(sceGifPacket *pPacket, u_long128 *pRef, u_int size, u_int opt1, u_int opt2, u_int flag){
  sceGifPkTerminate(pPacket);
  *pPacket->pCurrent++ = (4 << 28) | flag | size;
  *pPacket->pCurrent++ = CV2P(pRef);
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC009

void sceGifPkCall(sceGifPacket *pPacket, u_long128 *pCall, u_int opt1, u_int opt2, u_int flag){
  pPacket->pDmaTag = sceGifPkTerminate(pPacket);
  *pPacket->pCurrent++ = (5 << 28) | flag;
  *pPacket->pCurrent++ = CV2P(pCall);
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC010

void sceGifPkRet(sceGifPacket *pPacket, u_int opt1, u_int opt2, u_int flag){
  pPacket->pDmaTag = sceGifPkTerminate(pPacket);
  *pPacket->pCurrent++ = (6 << 28) | flag;
  *pPacket->pCurrent++ = 0;
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC011

void sceGifPkEnd(sceGifPacket *pPacket, u_int opt1, u_int opt2, u_int flag){
  pPacket->pDmaTag = sceGifPkTerminate(pPacket);
  *pPacket->pCurrent++ = (7 << 28) | flag;
  *pPacket->pCurrent++ = 0;
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC012

u_int *sceGifPkReserve(sceGifPacket *pPacket, u_int count){
  u_int* const result = pPacket->pCurrent;
  pPacket->pCurrent += count;
  return result;
}

#endif
#ifdef FUNC013

void sceGifPkOpenGifTag(sceGifPacket *pPacket, u_long128 tag){
  pPacket->pGifTag = (u_long*)pPacket->pCurrent;
  *(u_long128*)pPacket->pCurrent = tag;
  pPacket->pCurrent += sizeof(u_long128) / sizeof(u_int);
}

#endif
#ifdef FUNC014

void sceGifPkCloseGifTag(sceGifPacket* pPacket){
  u_long const tag = *pPacket->pGifTag;
  u_int const mode = (u_int)(tag >> 58) & 3;
  u_int nloop = ((u_long*)pPacket->pCurrent - pPacket->pGifTag) - 2;
  if(1 != mode){
	  nloop /= 2;
  }
  if(2 != mode){
	  u_int nreg = tag >> 60;
	  if(0 == nreg){
		  nreg = 16;
	  }
	  nloop = (nloop + nreg - 1) / nreg;
  }
  *pPacket->pGifTag = tag + nloop;
  pPacket->pGifTag = NULL;
  pPacket->pCurrent = (u_int*)align128(pPacket->pCurrent);
}

#endif
#ifdef FUNC015

void sceGifPkAddGsData(sceGifPacket* pPacket, u_long data){
  *pPacket->pCurrent++ = (u_int)data;
  *pPacket->pCurrent++ = (u_int)(data >> 32);
}

#endif
#ifdef FUNC115

void sceGifPkAddGsDataN(sceGifPacket* pPacket, u_long* pData, u_int count){
  pPacket->pCurrent = (u_int*)memcpy64((u_long*)pPacket->pCurrent, pData, count);
}

#endif
#ifdef FUNC016

void sceGifPkAddGsPacked(sceGifPacket* pPacket, u_long128 data){
  *((u_long128*)pPacket->pCurrent) = data;
  pPacket->pCurrent += sizeof(u_long128) / sizeof(u_int);
}

#endif
#ifdef FUNC116

void sceGifPkAddGsPackedN(sceGifPacket* pPacket, u_long128* pData, u_int count){
  pPacket->pCurrent = (u_int*)memcpy128((u_long128*)pPacket->pCurrent, pData, count);
}

#endif
#ifdef FUNC017

void sceGifPkAddGsAD(sceGifPacket* pPacket, u_int addr, u_long data){
  u_long* pGifCur = (u_long*)pPacket->pCurrent;
  *pGifCur++ = data;
  *pGifCur++ = (u_long)addr;
  pPacket->pCurrent = (u_int*)pGifCur;
}

#endif
#ifdef FUNC018

void sceGifPkRefLoadImage(sceGifPacket *pPacket, u_short bp, u_char psm, u_short bw, u_long128 *image, u_int size, u_int x, u_int y, u_int w, u_int h){
  const u_long giftag[2] __ALIGNED16__ = { SCE_GIF_SET_TAG(0, 0, 0, 0, 0, 1), 0xeL };
  sceGifPkCnt(pPacket, 0, 0, 0);
  sceGifPkOpenGifTag(pPacket, *(u_long128*)giftag);
  sceGifPkAddGsAD(pPacket, SCE_GS_BITBLTBUF, SCE_GS_SET_BITBLTBUF(0, 0, 0, bp, bw, psm));
  sceGifPkAddGsAD(pPacket, SCE_GS_TRXPOS, SCE_GS_SET_TRXPOS(0, 0, x, y, 0));
  sceGifPkAddGsAD(pPacket, SCE_GS_TRXREG, SCE_GS_SET_TRXREG(w, h));
  sceGifPkAddGsAD(pPacket, SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR(0));
  sceGifPkCloseGifTag(pPacket);
  while(size){
    u_int const len = (size <= 0x7fff) ? size : 0x7fff;
    sceGifPkCnt(pPacket, 0, 0, 0);
    {
      u_long* const tag = (u_long*)sceGifPkReserve(pPacket, 4);
      tag[0] = SCE_GIF_SET_TAG(len, (len == size) ? 1 : 0, 0, 0, 2, 0);
      tag[1] = 0;
    }
    sceGifPkRef(pPacket, image, len, 0, 0, 0);
    image += len;
    size -= len;
  }
}

#endif
#ifdef FUNC019



#include <stdio.h>


void sceGifPkDump(sceGifPacket *pkt)
{
    u_int *p;

    p = (u_int *)pkt->pBase;
    while(p!= (u_int *)pkt->pCurrent) {
	scePrintf("%08x : %08x %08x %08x %08x\n",(u_int)p,*p,*(p+1),*(p+2),*(p+3));
	p+=4;
    }
}
#endif

