/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/* 
 *                      Emotion Engine Library
 *                          Version 0.01
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        libpkt - libdmapk.c
 *                        dma packet support
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.01           Mar,26,1999     shibuya
 */

#include "libdmapk.h"

inline static u_int CV2P(void* const pPtr){
  return (u_int)pPtr & 0x9fffffff;
}

inline static u_long128* align128(u_int* pPtr){
  while((u_int)pPtr & 0xc){
    *pPtr++ = 0;
  }
  return (u_long128*)pPtr;
}

inline static u_long128* memcpy128(u_long128* pDst, u_long128 const* pSrc, u_int count){
  while(count--){
    *pDst++ = *pSrc++;
  }
  return pDst;
}

#ifdef FUNC000
void sceDmaPkInit(sceDmaPacket *pPacket, u_long128 *pBase){
  pPacket->pBase = pBase;
  pPacket->pCurrent = (u_int*)pPacket->pBase;
  pPacket->pDmaTag = NULL;
}

#endif
#ifdef FUNC001

void sceDmaPkReset(sceDmaPacket* pPacket){
  pPacket->pCurrent = (u_int*)pPacket->pBase;
  pPacket->pDmaTag = NULL;
}

#endif
#ifdef FUNC002

u_long128 *sceDmaPkTerminate(sceDmaPacket *pPacket){
  u_long128* const pDmaCur = align128(pPacket->pCurrent);
  if(pPacket->pDmaTag){
    u_int const count = pDmaCur - pPacket->pDmaTag - 1;
    ((u_int*)pPacket->pDmaTag)[0] += count;
  }
  pPacket->pCurrent = (u_int*)pDmaCur;
  pPacket->pDmaTag = NULL;
  return pDmaCur;
}

#endif
#ifdef FUNC003

u_int sceDmaPkSize(sceDmaPacket *pPacket){
  return ((u_int)pPacket->pCurrent - (u_int)pPacket->pBase) / sizeof(u_long128);
}

#endif
#ifdef FUNC004

void sceDmaPkRefe(sceDmaPacket *pPacket, u_long128 *pRef, u_int size, u_int opt1, u_int opt2, u_int flag){
  sceDmaPkTerminate(pPacket);
  *pPacket->pCurrent++ = (0 << 28) | flag | size;
  *pPacket->pCurrent++ = CV2P(pRef);
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC005

void sceDmaPkCnt(sceDmaPacket *pPacket, u_int opt1, u_int opt2, u_int flag){
  pPacket->pDmaTag = sceDmaPkTerminate(pPacket);
  *pPacket->pCurrent++ = (1 << 28) | flag;
  *pPacket->pCurrent++ = 0;
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC006

void sceDmaPkNext(sceDmaPacket *pPacket, u_long128 *pNext, u_int opt1, u_int opt2, u_int flag){
  pPacket->pDmaTag = sceDmaPkTerminate(pPacket);
  *pPacket->pCurrent++ = (2 << 28) | flag;
  *pPacket->pCurrent++ = CV2P(pNext);
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC007

void sceDmaPkRef(sceDmaPacket *pPacket, u_long128 *pRef, u_int size, u_int opt1, u_int opt2, u_int flag){
  sceDmaPkTerminate(pPacket);
  *pPacket->pCurrent++ = (3 << 28) | flag | size;
  *pPacket->pCurrent++ = CV2P(pRef);
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC008

void sceDmaPkRefs(sceDmaPacket *pPacket, u_long128 *pRef, u_int size, u_int opt1, u_int opt2, u_int flag){
  sceDmaPkTerminate(pPacket);
  *pPacket->pCurrent++ = (4 << 28) | flag | size;
  *pPacket->pCurrent++ = CV2P(pRef);
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC009

void sceDmaPkCall(sceDmaPacket *pPacket, u_long128 *pCall, u_int opt1, u_int opt2, u_int flag){
  pPacket->pDmaTag = sceDmaPkTerminate(pPacket);
  *pPacket->pCurrent++ = (5 << 28) | flag;
  *pPacket->pCurrent++ = CV2P(pCall);
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC010

void sceDmaPkRet(sceDmaPacket *pPacket, u_int opt1, u_int opt2, u_int flag){
  pPacket->pDmaTag = sceDmaPkTerminate(pPacket);
  *pPacket->pCurrent++ = (6 << 28) | flag;
  *pPacket->pCurrent++ = 0;
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC011

void sceDmaPkEnd(sceDmaPacket *pPacket, u_int opt1, u_int opt2, u_int flag){
  pPacket->pDmaTag = sceDmaPkTerminate(pPacket);
  *pPacket->pCurrent++ = (7 << 28) | flag;
  *pPacket->pCurrent++ = 0;
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC012

u_int *sceDmaPkReserve(sceDmaPacket *pPacket, u_int count){
  u_int* const result = pPacket->pCurrent;
  pPacket->pCurrent += count;
  return result;
}

#endif
#ifdef FUNC013

void sceDmaPkAddData(sceDmaPacket *pPacket, u_long128 data){
  *(u_long128*)pPacket->pCurrent = data;
  pPacket->pCurrent += sizeof(u_long128) / sizeof(u_int);
}

void sceDmaPkAddDataN(sceDmaPacket *pPacket, u_long128* pData, u_int count){
  pPacket->pCurrent = (u_int*)memcpy128((u_long128*)pPacket->pCurrent, pData, count);
}

#endif
#ifdef FUNC014

#include <stdio.h>

void sceDmaPkDump(sceDmaPacket *pkt)
{
    u_int *p;
    
    p = (u_int *)pkt->pBase;
    while(p!= (u_int *)pkt->pCurrent) {
	scePrintf("%08x : %08x %08x %08x %08x\n",(u_int)p,*p,*(p+1),*(p+2),*(p+3));
	p+=4;
    }
}

#endif
