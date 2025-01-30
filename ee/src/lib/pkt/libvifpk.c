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
 *                        libpkt - libvifpk.c
 *                        vif packet support
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.01           Mar,26,1999     shibuya
 */

#include "libvifpk.h"

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

__inline__ static u_int* memcpy32(u_int* pDst, u_int const* pSrc, u_int count){
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
void sceVif0PkInit(sceVif0Packet *pPacket, u_long128 *pBase){
  pPacket->pBase = pBase;
  pPacket->pCurrent = (u_int*)pPacket->pBase;
  pPacket->pDmaTag = NULL;
}

#endif
#ifdef FUNC001

void sceVif0PkReset(sceVif0Packet* pPacket){
  pPacket->pCurrent = (u_int*)pPacket->pBase;
  pPacket->pDmaTag = NULL;
}

#endif
#ifdef FUNC002

u_long128 *sceVif0PkTerminate(sceVif0Packet *pPacket){
  u_long128* const pDmaCur = align128(pPacket->pCurrent);
  if(pPacket->pDmaTag){
    u_int count = (pDmaCur - pPacket->pDmaTag) - 1;
    ((u_int*)pPacket->pDmaTag)[0] += count;
  }
  pPacket->pCurrent = (u_int*)pDmaCur;
  pPacket->pDmaTag = NULL;
  return pDmaCur;
}

#endif
#ifdef FUNC003

u_int sceVif0PkSize(sceVif0Packet *pPacket){
  return ((u_int)pPacket->pCurrent - (u_int)pPacket->pBase) / sizeof(u_long128);
}

#endif
#ifdef FUNC004

void sceVif0PkRefe(sceVif0Packet *pPacket, u_long128 *pRef, u_int size, u_int opt1, u_int opt2, u_int flag){
  sceVif0PkTerminate(pPacket);
  *pPacket->pCurrent++ = (0 << 28) | flag | size;
  *pPacket->pCurrent++ = CV2P(pRef);
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC005

void sceVif0PkCnt(sceVif0Packet *pPacket, u_int flag){
  pPacket->pDmaTag = sceVif0PkTerminate(pPacket);
  *pPacket->pCurrent++ = (1 << 28) | flag;
  *pPacket->pCurrent++ = 0;
  pPacket->pVifCode = NULL;
}

#endif
#ifdef FUNC006

void sceVif0PkNext(sceVif0Packet *pPacket, u_long128 *pNext, u_int flag){
  pPacket->pDmaTag = sceVif0PkTerminate(pPacket);
  *pPacket->pCurrent++ = (2 << 28) | flag;
  *pPacket->pCurrent++ = CV2P(pNext);
  pPacket->pVifCode = NULL;
}

#endif
#ifdef FUNC007

void sceVif0PkRef(sceVif0Packet *pPacket, u_long128 *pRef, u_int size, u_int opt1, u_int opt2, u_int flag){
  sceVif0PkTerminate(pPacket);
  *pPacket->pCurrent++ = (3 << 28) | flag | size;
  *pPacket->pCurrent++ = CV2P(pRef);
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC008

void sceVif0PkRefs(sceVif0Packet *pPacket, u_long128 *pRef, u_int size, u_int opt1, u_int opt2, u_int flag){
  sceVif0PkTerminate(pPacket);
  *pPacket->pCurrent++ = (4 << 28) | flag | size;
  *pPacket->pCurrent++ = CV2P(pRef);
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC009

void sceVif0PkCall(sceVif0Packet *pPacket, u_long128 *pCall, u_int flag){
  pPacket->pDmaTag = sceVif0PkTerminate(pPacket);
  *pPacket->pCurrent++ = (5 << 28) | flag;
  *pPacket->pCurrent++ = CV2P(pCall);
  pPacket->pVifCode = NULL;
}

#endif
#ifdef FUNC010

void sceVif0PkRet(sceVif0Packet *pPacket, u_int flag){
  pPacket->pDmaTag = sceVif0PkTerminate(pPacket);
  *pPacket->pCurrent++ = (6 << 28) | flag;
  *pPacket->pCurrent++ = 0;
  pPacket->pVifCode = NULL;
}

#endif
#ifdef FUNC011

void sceVif0PkEnd(sceVif0Packet *pPacket, u_int flag){
  pPacket->pDmaTag = sceVif0PkTerminate(pPacket);
  *pPacket->pCurrent++ = (7 << 28) | flag;
  *pPacket->pCurrent++ = 0;
  pPacket->pVifCode = NULL;
}

#endif
#ifdef FUNC012

void sceVif0PkOpenUpkCode(sceVif0Packet *pPacket, u_short vuaddr, u_int upkcmd, u_int cl, u_int wl){
  *pPacket->pCurrent++ = (0x01 << 24) | (wl << 8) | cl;
  pPacket->pVifCode = pPacket->pCurrent;
  *pPacket->pCurrent++ = (upkcmd << 24) | vuaddr;
  {
    u_int const clVal = (cl == 0) ? 256 : cl;
    u_int const wlVal = (wl == 0) ? 256 : wl;
    u_int len = (32 >> (upkcmd & 0x03)) * (((upkcmd >> 2) & 0x03) + 1);
    if(wlVal <= clVal){
      pPacket->numlen = (1 << 16) | len;
    }else{
      len *= clVal;
      pPacket->numlen = (wlVal << 16) | len;
    }
  }
}

#endif
#ifdef FUNC013

void sceVif0PkCloseUpkCode(sceVif0Packet *pPacket){
  u_int const bitcount = (pPacket->pCurrent - (pPacket->pVifCode + 1)) * sizeof(u_int) * 8;
  u_int const numlen = pPacket->numlen;
  u_int const len = numlen & 0x0000ffff;
  u_int const num = (numlen & 0xffff0000) * ( bitcount / len);
  *pPacket->pVifCode += num;
  pPacket->pVifCode = NULL;
}

#endif
#ifdef FUNC014

u_int *sceVif0PkReserve(sceVif0Packet *pPacket, u_int count){
  u_int* const result = pPacket->pCurrent;
  pPacket->pCurrent += count;
  return result;
}

#endif
#ifdef FUNC015

void sceVif0PkAlign(sceVif0Packet *pPacket, u_int bit, u_int pos){
  u_int const mask = 0xffffffff >> (32 - ((bit + 2) & 0x1f));
  u_int target = ((u_int)pPacket->pCurrent & ~mask) + pos * sizeof(u_int);
  if(target < (u_int)pPacket->pCurrent){ target += mask + 1; }
  while(target > (u_int)pPacket->pCurrent){
    *pPacket->pCurrent++ = 0;
  }
}

#endif
#ifdef FUNC016

void sceVif0PkAddCode(sceVif0Packet *pPacket, u_int code){
  *pPacket->pCurrent++ = code;
}

#endif
#ifdef FUNC017

void sceVif0PkAddData(sceVif0Packet *pPacket, u_int data){
  *pPacket->pCurrent++ = data;
}

#endif
#ifdef FUNC018

void sceVif0PkAddDataN(sceVif0Packet *pPacket, u_int *pData, u_int count){
  pPacket->pCurrent = memcpy32(pPacket->pCurrent, pData, count);
}

#endif
#ifdef FUNC019

void sceVif0PkAddUpkData32(sceVif0Packet *pPacket, u_int data){
  *pPacket->pCurrent++ = data;
}

#endif
#ifdef FUNC020

void sceVif0PkAddUpkData32N(sceVif0Packet *pPacket, u_int* pData, u_int count){
  pPacket->pCurrent = memcpy32(pPacket->pCurrent, pData, count);
}

#endif
#ifdef FUNC021

void sceVif0PkAddUpkData64(sceVif0Packet *pPacket, u_long data){
  *pPacket->pCurrent++ = (u_int)data;
  *pPacket->pCurrent++ = (u_int)(data >> 32);
}

#endif
#ifdef FUNC121

void sceVif0PkAddUpkData64N(sceVif0Packet *pPacket, u_long* pData, u_int count){
  while(count--){
    u_long data = *pData++;
    *pPacket->pCurrent++ = (u_int)data;
    *pPacket->pCurrent++ = (u_int)(data >> 32);
  }
}

#endif
#ifdef FUNC022

void sceVif0PkAddUpkData128(sceVif0Packet *pPacket, u_long128 data){
  u_long const low = ((u_long*)&data)[0];
  u_long const high = ((u_long*)&data)[1];
  *pPacket->pCurrent++ = (u_int)low;
  *pPacket->pCurrent++ = (u_int)(low >> 32);
  *pPacket->pCurrent++ = (u_int)high;
  *pPacket->pCurrent++ = (u_int)(high >> 32);
}

#endif
#ifdef FUNC122

void sceVif0PkAddUpkData128N(sceVif0Packet *pPacket, u_long128* pData, u_int count){
  while(count--){
    u_long const low = ((u_long*)pData)[0];
    u_long const high = ((u_long*)pData)[1];
    pData++;
    *pPacket->pCurrent++ = (u_int)low;
    *pPacket->pCurrent++ = (u_int)(low >> 32);
    *pPacket->pCurrent++ = (u_int)high;
    *pPacket->pCurrent++ = (u_int)(high >> 32);
  }
}

#endif
#ifdef FUNC023

void sceVif0PkRefMpg(sceVif0Packet *pPacket, u_short vuaddr, u_long128 *pMicro, u_int size, u_int opt1){
  while(size){
    u_int const count = (size > 128) ? 128 : size;
    u_int const num = count * 2;
    sceVif0PkRef(pPacket, pMicro, count, opt1, SCE_VIF0_SET_MPG(vuaddr, num & 0xff, 0), 0);
    opt1 = 0;
    pMicro += count;
    size -= count;
    vuaddr += num;
  }
}


#endif
#ifdef FUNC024

#include <stdio.h>

void sceVif0PkDump(sceVif0Packet *pkt)
{
    u_int *p;

    p = (u_int *)pkt->pBase;
    while(p!= (u_int *)pkt->pCurrent) {
	scePrintf("%08x : %08x %08x %08x %08x\n",(u_int)p,*p,*(p+1),*(p+2),*(p+3));
	p+=4;
    }
}

#endif
#ifdef FUNC025
void sceVif1PkInit(sceVif1Packet *pPacket, u_long128 *pBase){
  pPacket->pBase = pBase;
  pPacket->pCurrent = (u_int*)pPacket->pBase;
  pPacket->pDmaTag = NULL;
}

#endif
#ifdef FUNC026

void sceVif1PkReset(sceVif1Packet* pPacket){
  pPacket->pCurrent = (u_int*)pPacket->pBase;
  pPacket->pDmaTag = NULL;
}

#endif
#ifdef FUNC027

u_long128 *sceVif1PkTerminate(sceVif1Packet *pPacket){
  u_long128* const pDmaCur = align128(pPacket->pCurrent);
  if(pPacket->pDmaTag){
    u_int count = (pDmaCur - pPacket->pDmaTag) - 1;
    ((u_int*)pPacket->pDmaTag)[0] += count;
  }
  pPacket->pCurrent = (u_int*)pDmaCur;
  pPacket->pDmaTag = NULL;
  return pDmaCur;
}

#endif
#ifdef FUNC028

u_int sceVif1PkSize(sceVif1Packet *pPacket){
  return ((u_int)pPacket->pCurrent - (u_int)pPacket->pBase) / sizeof(u_long128);
}

#endif
#ifdef FUNC029

void sceVif1PkRefe(sceVif1Packet *pPacket, u_long128 *pRef, u_int size, u_int opt1, u_int opt2, u_int flag){
  sceVif1PkTerminate(pPacket);
  *pPacket->pCurrent++ = (0 << 28) | flag | size;
  *pPacket->pCurrent++ = CV2P(pRef);
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC030

void sceVif1PkCnt(sceVif1Packet *pPacket, u_int flag){
  pPacket->pDmaTag = sceVif1PkTerminate(pPacket);
  *pPacket->pCurrent++ = (1 << 28) | flag;
  *pPacket->pCurrent++ = 0;
  pPacket->pVifCode = NULL;
}

#endif
#ifdef FUNC031

void sceVif1PkNext(sceVif1Packet *pPacket, u_long128 *pNext, u_int flag){
  pPacket->pDmaTag = sceVif1PkTerminate(pPacket);
  *pPacket->pCurrent++ = (2 << 28) | flag;
  *pPacket->pCurrent++ = CV2P(pNext);
  pPacket->pVifCode = NULL;
}

#endif
#ifdef FUNC032

void sceVif1PkRef(sceVif1Packet *pPacket, u_long128 *pRef, u_int size, u_int opt1, u_int opt2, u_int flag){
  sceVif1PkTerminate(pPacket);
  *pPacket->pCurrent++ = (3 << 28) | flag | size;
  *pPacket->pCurrent++ = CV2P(pRef);
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC033

void sceVif1PkRefs(sceVif1Packet *pPacket, u_long128 *pRef, u_int size, u_int opt1, u_int opt2, u_int flag){
  sceVif1PkTerminate(pPacket);
  *pPacket->pCurrent++ = (4 << 28) | flag | size;
  *pPacket->pCurrent++ = CV2P(pRef);
  *pPacket->pCurrent++ = opt1;
  *pPacket->pCurrent++ = opt2;
}

#endif
#ifdef FUNC034

void sceVif1PkCall(sceVif1Packet *pPacket, u_long128 *pCall, u_int flag){
  pPacket->pDmaTag = sceVif1PkTerminate(pPacket);
  *pPacket->pCurrent++ = (5 << 28) | flag;
  *pPacket->pCurrent++ = CV2P(pCall);
  pPacket->pVifCode = NULL;
}

#endif
#ifdef FUNC035

void sceVif1PkRet(sceVif1Packet *pPacket, u_int flag){
  pPacket->pDmaTag = sceVif1PkTerminate(pPacket);
  *pPacket->pCurrent++ = (6 << 28) | flag;
  *pPacket->pCurrent++ = 0;
  pPacket->pVifCode = NULL;
}

#endif
#ifdef FUNC036

void sceVif1PkEnd(sceVif1Packet *pPacket, u_int flag){
  pPacket->pDmaTag = sceVif1PkTerminate(pPacket);
  *pPacket->pCurrent++ = (7 << 28) | flag;
  *pPacket->pCurrent++ = 0;
  pPacket->pVifCode = NULL;
}

#endif
#ifdef FUNC037

void sceVif1PkOpenUpkCode(sceVif1Packet *pPacket, u_short vuaddr, u_int upkcmd, u_int cl, u_int wl){
  *pPacket->pCurrent++ = (0x01 << 24) | (wl << 8) | cl;
  pPacket->pVifCode = pPacket->pCurrent;
  *pPacket->pCurrent++ = (upkcmd << 24) | vuaddr;
  {
    u_int const clVal = (cl == 0) ? 256 : cl;
    u_int const wlVal = (wl == 0) ? 256 : wl;
    u_int len = (32 >> (upkcmd & 0x03)) * (((upkcmd >> 2) & 0x03) + 1);
    if(wlVal <= clVal){
      pPacket->numlen = (1 << 16) | len;
    }else{
      len *= clVal;
      pPacket->numlen = (wlVal << 16) | len;
    }
  }
}

#endif
#ifdef FUNC038

void sceVif1PkCloseUpkCode(sceVif1Packet *pPacket){
  u_int const bitcount = (pPacket->pCurrent - (pPacket->pVifCode + 1)) * sizeof(u_int) * 8;
  u_int const numlen = pPacket->numlen;
  u_int const len = numlen & 0x0000ffff;
  u_int const num = (numlen & 0xffff0000) * ( bitcount / len);
  *pPacket->pVifCode += num;
  pPacket->pVifCode = NULL;
}

#endif
#ifdef FUNC039

void sceVif1PkOpenDirectCode(sceVif1Packet *pPacket, int stall){
  sceVif1PkAlign(pPacket, 2, 3);

  pPacket->pVifCode = pPacket->pCurrent;
  *pPacket->pCurrent++ = ((stall ? 0x80 : 0) | 0x50) << 24;
}

#endif
#ifdef FUNC040

void sceVif1PkCloseDirectCode(sceVif1Packet *pPacket){
  u_int const count = (pPacket->pCurrent - (pPacket->pVifCode + 1)) / (sizeof(u_long128) / sizeof(u_int));
  *pPacket->pVifCode += count;
  pPacket->pVifCode = NULL;
}

#endif
#ifdef FUNC140

void sceVif1PkOpenDirectHLCode(sceVif1Packet *pPacket, int stall){
  sceVif1PkAlign(pPacket, 2, 3);

  pPacket->pVifCode = pPacket->pCurrent;
  *pPacket->pCurrent++ = ((stall ? 0x80 : 0) | 0x51) << 24;
}

#endif
#ifdef FUNC240

void sceVif1PkCloseDirectHLCode(sceVif1Packet *pPacket){
  u_int const count = (pPacket->pCurrent - (pPacket->pVifCode + 1)) / (sizeof(u_long128) / sizeof(u_int));
  *pPacket->pVifCode += count;
  pPacket->pVifCode = NULL;
}

#endif
#ifdef FUNC041

void sceVif1PkOpenGifTag(sceVif1Packet *pPacket, u_long128 tag){
  pPacket->pGifTag = (u_long*)pPacket->pCurrent;
  *(u_long128*)pPacket->pCurrent = tag;
  pPacket->pCurrent += sizeof(u_long128) / sizeof(u_int);
}

#endif
#ifdef FUNC042

void sceVif1PkCloseGifTag(sceVif1Packet *pPacket){
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
#ifdef FUNC043

u_int *sceVif1PkReserve(sceVif1Packet *pPacket, u_int count){
  u_int* const result = pPacket->pCurrent;
  pPacket->pCurrent += count;
  return result;
}

#endif
#ifdef FUNC044

void sceVif1PkAlign(sceVif1Packet *pPacket, u_int bit, u_int pos){
  u_int const mask = 0xffffffff >> (32 - ((bit + 2) & 0x1f));
  u_int target = ((u_int)pPacket->pCurrent & ~mask) + pos * sizeof(u_int);
  if(target < (u_int)pPacket->pCurrent){ target += mask + 1; }
  while(target > (u_int)pPacket->pCurrent){
    *pPacket->pCurrent++ = 0;
  }
}

#endif
#ifdef FUNC045

void sceVif1PkAddCode(sceVif1Packet *pPacket, u_int code){
  *pPacket->pCurrent++ = code;
}

#endif
#ifdef FUNC046

void sceVif1PkAddData(sceVif1Packet *pPacket, u_int data){
  *pPacket->pCurrent++ = data;
}

#endif
#ifdef FUNC047

void sceVif1PkAddDataN(sceVif1Packet *pPacket, u_int* pData, u_int count){
  while(count--){
    *pPacket->pCurrent++ = *pData++;
  }
}

#endif
#ifdef FUNC048

void sceVif1PkAddUpkData32(sceVif1Packet *pPacket, u_int data){
  *pPacket->pCurrent++ = data;
}

#endif
#ifdef FUNC049

void sceVif1PkAddUpkData32N(sceVif1Packet *pPacket, u_int* pData, u_int count){
  while(count--){
    *pPacket->pCurrent++ = *pData++;
  }
}

#endif
#ifdef FUNC050

void sceVif1PkAddUpkData64(sceVif1Packet *pPacket, u_long data){
  *pPacket->pCurrent++ = (u_int)data;
  *pPacket->pCurrent++ = (u_int)(data >> 32);
}

#endif
#ifdef FUNC150

void sceVif1PkAddUpkData64N(sceVif1Packet *pPacket, u_long* pData, u_int count){
  while(count--){
    u_long const data = *pData++;
    *pPacket->pCurrent++ = (u_int)data;
    *pPacket->pCurrent++ = (u_int)(data >> 32);
  }
}

#endif
#ifdef FUNC051

void sceVif1PkAddUpkData128(sceVif1Packet *pPacket, u_long128 data){
  u_long const low = ((u_long*)&data)[0];
  u_long const high = ((u_long*)&data)[1];
  *pPacket->pCurrent++ = (u_int)low;
  *pPacket->pCurrent++ = (u_int)(low >> 32);
  *pPacket->pCurrent++ = (u_int)high;
  *pPacket->pCurrent++ = (u_int)(high >> 32);
}

#endif
#ifdef FUNC151

void sceVif1PkAddUpkData128N(sceVif1Packet *pPacket, u_long128* pData, u_int count){
  while(count--){
    u_long const low = ((u_long*)pData)[0];
    u_long const high = ((u_long*)pData)[1];
    pData++;
    *pPacket->pCurrent++ = (u_int)low;
    *pPacket->pCurrent++ = (u_int)(low >> 32);
    *pPacket->pCurrent++ = (u_int)high;
    *pPacket->pCurrent++ = (u_int)(high >> 32);
  }
}

#endif
#ifdef FUNC052

void sceVif1PkAddDirectData(sceVif1Packet *pPacket, u_long128 data){
  u_long const low = ((u_long*)&data)[0];
  u_long const high = ((u_long*)&data)[1];
  *pPacket->pCurrent++ = (u_int)low;
  *pPacket->pCurrent++ = (u_int)(low >> 32);
  *pPacket->pCurrent++ = (u_int)high;
  *pPacket->pCurrent++ = (u_int)(high >> 32);
}

#endif
#ifdef FUNC152

void sceVif1PkAddDirectDataN(sceVif1Packet *pPacket, u_long128* pData, u_int count){
  while(count--){
    u_long const low = ((u_long*)pData)[0];
    u_long const high = ((u_long*)pData)[1];
    pData++;
    *pPacket->pCurrent++ = (u_int)low;
    *pPacket->pCurrent++ = (u_int)(low >> 32);
    *pPacket->pCurrent++ = (u_int)high;
    *pPacket->pCurrent++ = (u_int)(high >> 32);
  }
}

#endif
#ifdef FUNC053

void sceVif1PkAddGsData(sceVif1Packet* pPacket, u_long data){
  *pPacket->pCurrent++ = (u_int)data;
  *pPacket->pCurrent++ = (u_int)(data >> 32);
}

#endif
#ifdef FUNC153


void sceVif1PkAddGsDataN(sceVif1Packet* pPacket, u_long *pData, u_int count){
  while(count--){
    u_long const data = *pData++;
    *pPacket->pCurrent++ = (u_int)data;
    *pPacket->pCurrent++ = (u_int)(data >> 32);
  }
}

#endif
#ifdef FUNC253


void sceVif1PkAddGsPacked(sceVif1Packet* pPacket, u_long128 data){
  *((u_long128*)pPacket->pCurrent) = data;
  pPacket->pCurrent += sizeof(u_long128) / sizeof(u_int);
}

#endif
#ifdef FUNC353


void sceVif1PkAddGsPackedN(sceVif1Packet* pPacket, u_long128* pData, u_int count){
  pPacket->pCurrent = (u_int*)memcpy128((u_long128*)pPacket->pCurrent, pData, count);
}

#endif
#ifdef FUNC054

void sceVif1PkAddGsAD(sceVif1Packet* pPacket, u_int addr, u_long data){
  *pPacket->pCurrent++ = (u_int)data;
  *pPacket->pCurrent++ = (u_int)(data >> 32);
  *pPacket->pCurrent++ = addr;
  *pPacket->pCurrent++ = 0;
}

#endif
#ifdef FUNC055

void sceVif1PkRefLoadImage(sceVif1Packet *pPacket, u_short bp, u_char psm, u_short bw, u_long128 *image, u_int size, u_int x, u_int y, u_int w, u_int h){
  const u_long giftag[2] __ALIGNED16__ = { SCE_GIF_SET_TAG(0, 0, 0, 0, 0, 1), 0xeL };
  sceVif1PkCnt(pPacket, 0);
  sceVif1PkOpenDirectHLCode(pPacket, 0);
  sceVif1PkOpenGifTag(pPacket, *(u_long128*)giftag);
  sceVif1PkAddGsAD(pPacket, SCE_GS_BITBLTBUF, SCE_GS_SET_BITBLTBUF(0, 0, 0, bp, bw, psm));
  sceVif1PkAddGsAD(pPacket, SCE_GS_TRXPOS, SCE_GS_SET_TRXPOS(0, 0, x, y, 0));
  sceVif1PkAddGsAD(pPacket, SCE_GS_TRXREG, SCE_GS_SET_TRXREG(w, h));
  sceVif1PkAddGsAD(pPacket, SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR(0));
  sceVif1PkCloseGifTag(pPacket);
  sceVif1PkCloseDirectHLCode(pPacket);
  while(size){
    u_int const len = (size <= 0x7fff) ? size : 0x7fff;
    sceVif1PkCnt(pPacket, 0);
    sceVif1PkAlign(pPacket, 2, 3);
    sceVif1PkAddCode(pPacket, SCE_VIF1_SET_DIRECTHL(1, 0));
    {
      u_long* const tag = (u_long*)sceVif1PkReserve(pPacket, 4);
      tag[0] = SCE_GIF_SET_TAG(len, (len == size) ? 1 : 0, 0, 0, 2, 0);
      tag[1] = 0;
    }
    sceVif1PkRef(pPacket, image, len, 0, SCE_VIF1_SET_DIRECTHL(len, 0), 0);
    image += len;
    size -= len;
  }
}

#endif
#ifdef FUNC056

void sceVif1PkRefMpg(sceVif1Packet *pPacket, u_short vuaddr, u_long128 *pMicro, u_int size, u_int opt1){
  while(size){
    u_int const count = (size > 128) ? 128 : size;
    u_int const num = count * 2;
    sceVif1PkRef(pPacket, pMicro, count, opt1, SCE_VIF1_SET_MPG(vuaddr, num & 0xff, 0), 0);
    opt1 = 0;
    pMicro += count;
    size -= count;
    vuaddr += num;
  }
}


#endif
#ifdef FUNC057

#include <stdio.h>

void sceVif1PkDump(sceVif1Packet *pkt)
{
    u_int *p;

    p = (u_int *)pkt->pBase;
    while(p!= (u_int *)pkt->pCurrent) {
	scePrintf("%08x : %08x %08x %08x %08x\n",(u_int)p,*p,*(p+1),*(p+2),*(p+3));
	p+=4;
    }
}

#endif

