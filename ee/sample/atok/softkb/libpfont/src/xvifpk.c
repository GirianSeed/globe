/* SCE CONFIDENTIAL
   "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
*/
/*
 *     Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                   All Rights Reserved.
 *
 */

#include <eeregs.h>

#ifdef __INLINE__
#	undef __INLINE__
#	define FUNCTYPE	extern __inline__
#	define __INLINE__
#else
#	include "xvifpk.h"
#	define FUNCTYPE
#endif

#ifndef __ALIGNED16__
#	define __ALIGNED16__ __attribute__((aligned(16)))
#endif


static u_int CV2P(void const* pPtr);
static u_long128* align128(u_int* pPtr);
static u_long* align64(u_int* pPtr);
static u_int* memcpy32(u_int* pDst, u_int const* pSrc, u_int count);
static u_long128* memcpy128(u_long128* pDst, u_long128 const* pSrc, u_int count);

static __inline__ u_int CV2P(void const* pPtr){
	return (u_int)pPtr & 0x8fffffffU;
}

static  __inline__ u_long128* align128(u_int* pPtr){
	int count = (4 - ((u_int)pPtr >> 2)) & 3;
	while(count--){
		*pPtr++ = 0;
	}
	return (u_long128*)(void*)pPtr;
}

static __inline__ u_long* align64(u_int* pPtr){
	if(0 != ((u_int)pPtr & 0x00000004U)){
		*pPtr++ = 0;
	}
	return (u_long*)(void*)pPtr;
}

static __inline__ u_int* memcpy32(u_int* pDst, u_int const* pSrc, u_int count){
	while(count--){
		*pDst++ = *pSrc++;
	}
	return pDst;
}

static __inline__ u_long128* memcpy128(u_long128* pDst, u_long128 const* pSrc, u_int count){
	while(count--){
		*pDst++ = *pSrc++;
	}
	return pDst;
}

FUNCTYPE void xVif1PkAlign(sceVif1Packet* pPacket, u_int bit, u_int pos){
	u_int const mask = 0xffffffffU >> (32 - ((bit + 2) & 0x0000001fU));
	u_int target = ((u_int)pPacket->pCurrent & ~mask) + pos * sizeof(u_int);

	if(target < (u_int)pPacket->pCurrent){
		target += mask + 1;
	}

	while(target > (u_int)pPacket->pCurrent){
		*pPacket->pCurrent++ = 0x00000000U;
	}
}

FUNCTYPE void xVif1PkInit(sceVif1Packet* pPacket, u_long128* pBase){
	pPacket->pBase = pBase;
	pPacket->pCurrent = (u_int*)pPacket->pBase;
	pPacket->pDmaTag = NULL;
}

FUNCTYPE void xVif1PkReset(sceVif1Packet* pPacket){
	pPacket->pCurrent = (u_int*)pPacket->pBase;
	pPacket->pDmaTag = NULL;
}

FUNCTYPE u_long128* xVif1PkTerminate(sceVif1Packet* pPacket){
	u_long128* const pDmaCur = align128(pPacket->pCurrent);
	if(pPacket->pDmaTag){
		u_int count = (pDmaCur - pPacket->pDmaTag) - 1;
		((u_int*)pPacket->pDmaTag)[0] += count;
	}
	pPacket->pCurrent = (u_int*)pDmaCur;
	pPacket->pDmaTag = NULL;

	return pDmaCur;
}

FUNCTYPE u_int xVif1PkSize(sceVif1Packet* pPacket){
	return ((u_int)pPacket->pCurrent - (u_int)pPacket->pBase) / sizeof(u_long128);
}

FUNCTYPE void xVif1PkRefe(sceVif1Packet* pPacket, u_long128 const* pRef, u_int size, u_int opt1, u_int opt2, u_int flag){
	xVif1PkTerminate(pPacket);
	*pPacket->pCurrent++ = (0 << 28) | flag | size;
	*pPacket->pCurrent++ = CV2P(pRef);
	*pPacket->pCurrent++ = opt1;
	*pPacket->pCurrent++ = opt2;
}

FUNCTYPE void xVif1PkCnt(sceVif1Packet* pPacket, u_int flag){
	pPacket->pDmaTag = xVif1PkTerminate(pPacket);
	*pPacket->pCurrent++ = (1 << 28) | flag;
	*pPacket->pCurrent++ = 0;
	pPacket->pVifCode = NULL;
}

FUNCTYPE void xVif1PkNext(sceVif1Packet* pPacket, u_long128 const* pNext, u_int flag){
	pPacket->pDmaTag = xVif1PkTerminate(pPacket);
	*pPacket->pCurrent++ = (2 << 28) | flag;
	*pPacket->pCurrent++ = CV2P(pNext);
	pPacket->pVifCode = NULL;
}

FUNCTYPE void xVif1PkRef(sceVif1Packet* pPacket, u_long128 const* pRef, u_int size, u_int opt1, u_int opt2, u_int flag){
	xVif1PkTerminate(pPacket);
	*pPacket->pCurrent++ = (3 << 28) | flag | size;
	*pPacket->pCurrent++ = CV2P(pRef);
	*pPacket->pCurrent++ = opt1;
	*pPacket->pCurrent++ = opt2;
}

FUNCTYPE void xVif1PkRefs(sceVif1Packet* pPacket, u_long128 const*pRef, u_int size, u_int opt1, u_int opt2, u_int flag){
	xVif1PkTerminate(pPacket);
	*pPacket->pCurrent++ = (4 << 28) | flag | size;
	*pPacket->pCurrent++ = CV2P(pRef);
	*pPacket->pCurrent++ = opt1;
	*pPacket->pCurrent++ = opt2;
}

FUNCTYPE void xVif1PkCall(sceVif1Packet* pPacket, u_long128 const* pCall, u_int flag){
	pPacket->pDmaTag = xVif1PkTerminate(pPacket);
	*pPacket->pCurrent++ = (5 << 28) | flag;
	*pPacket->pCurrent++ = CV2P(pCall);
	pPacket->pVifCode = NULL;
}

FUNCTYPE void xVif1PkRet(sceVif1Packet* pPacket, u_int flag){
	pPacket->pDmaTag = xVif1PkTerminate(pPacket);
	*pPacket->pCurrent++ = (6 << 28) | flag;
	*pPacket->pCurrent++ = 0;
	pPacket->pVifCode = NULL;
}

FUNCTYPE void xVif1PkEnd(sceVif1Packet* pPacket, u_int flag){
	pPacket->pDmaTag = xVif1PkTerminate(pPacket);
	*pPacket->pCurrent++ = (7 << 28) | flag;
	*pPacket->pCurrent++ = 0;
	pPacket->pVifCode = NULL;
}

FUNCTYPE void xVif1PkOpenUpkCode(sceVif1Packet* pPacket, u_short vuaddr, u_int upkcmd, u_int cl, u_int wl){
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

FUNCTYPE void xVif1PkCloseUpkCode(sceVif1Packet* pPacket){
	u_int const bitcount = (pPacket->pCurrent - (pPacket->pVifCode + 1)) * sizeof(u_int) * 8;
	u_int const numlen = pPacket->numlen;
	u_int const len = numlen & 0x0000ffffU;
	u_int const num = (numlen & 0xffff0000U) * ((bitcount + len - 1) / len);
	*pPacket->pVifCode += num;
	pPacket->pVifCode = NULL;
}

FUNCTYPE void xVif1PkOpenDirectCode(sceVif1Packet* pPacket, int stall){
	xVif1PkAlign(pPacket, 2, 3);

	pPacket->pVifCode = pPacket->pCurrent;
	*pPacket->pCurrent++ = ((stall ? 0x80 : 0) | 0x50) << 24;
}

FUNCTYPE void xVif1PkCloseDirectCode(sceVif1Packet* pPacket){
	u_int const count = (pPacket->pCurrent - (pPacket->pVifCode + 1)) / (sizeof(u_long128) / sizeof(u_int));
	*pPacket->pVifCode += count;
	pPacket->pVifCode = NULL;
}

FUNCTYPE void xVif1PkOpenDirectHLCode(sceVif1Packet* pPacket, int stall){
	xVif1PkAlign(pPacket, 2, 3);

	pPacket->pVifCode = pPacket->pCurrent;
	*pPacket->pCurrent++ = ((stall ? 0x80 : 0) | 0x51) << 24;
}

FUNCTYPE void xVif1PkCloseDirectHLCode(sceVif1Packet* pPacket){
	u_int const count = (pPacket->pCurrent - (pPacket->pVifCode + 1)) / (sizeof(u_long128) / sizeof(u_int));
	*pPacket->pVifCode += count;
	pPacket->pVifCode = NULL;
}

FUNCTYPE void xVif1PkOpenGifTag(sceVif1Packet* pPacket, u_long128 tag){
	//u_long128* ptr = align128(pPacket->pCurrent);
	u_long128* ptr = (u_long128*)(void*)pPacket->pCurrent;
	pPacket->pGifTag = (u_long*)ptr;
	*ptr++ = tag;
	pPacket->pCurrent = (u_int*)ptr;
}

FUNCTYPE void xVif1PkOpenGifTag2(sceVif1Packet* pPacket, u_long gifTag, u_long regs){
	//u_long* ptr = align128(pPacket->pCurrent);
	u_long* ptr = (u_long*)(void*)pPacket->pCurrent;
	pPacket->pGifTag = ptr;
	*ptr++ = gifTag;
	*ptr++ = regs;
	pPacket->pCurrent = (u_int*)ptr;
}

FUNCTYPE void xVif1PkCloseGifTag(sceVif1Packet* pPacket){
	u_long const tag = *pPacket->pGifTag;
	u_int const mode = (u_int)(tag >> 58) & 3;
	u_int nloop = ((u_long*)(void*)pPacket->pCurrent - pPacket->pGifTag) - 2;
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

FUNCTYPE void xVif1PkCloseGifTag2(sceVif1Packet* pPacket, u_int nloop){
	*pPacket->pGifTag += nloop;
	pPacket->pGifTag = NULL;
	pPacket->pCurrent = (u_int*)align128(pPacket->pCurrent);
}

FUNCTYPE u_int* xVif1PkReserve(sceVif1Packet* pPacket, u_int count){
	u_int* const result = pPacket->pCurrent;
	pPacket->pCurrent += count;
	return result;
}

FUNCTYPE void xVif1PkAddCode(sceVif1Packet* pPacket, u_int code){
	*pPacket->pCurrent++ = code;
}

FUNCTYPE void xVif1PkAddData(sceVif1Packet* pPacket, u_int data){
	*pPacket->pCurrent++ = data;
}

FUNCTYPE void xVif1PkAddDataN(sceVif1Packet* pPacket, u_int const* pData, u_int count){
	u_int* ptr = pPacket->pCurrent;
	while(count--){
		*ptr++ = *pData++;
	}
	pPacket->pCurrent = ptr;
}

FUNCTYPE void xVif1PkAddUpkData32(sceVif1Packet* pPacket, u_int data){
	*pPacket->pCurrent++ = data;
}

FUNCTYPE void xVif1PkAddUpkData32N(sceVif1Packet* pPacket, u_int const* pData, u_int count){
	u_int* ptr = pPacket->pCurrent;
	while(count--){
		*ptr++ = *pData++;
	}
	pPacket->pCurrent = ptr;
}

FUNCTYPE void xVif1PkAddUpkData64(sceVif1Packet* pPacket, u_long data){
	u_int* ptr = pPacket->pCurrent;
	*ptr++ = (u_int)data;
	*ptr++ = (u_int)(data >> 32);
	pPacket->pCurrent = ptr;
}

FUNCTYPE void xVif1PkAddUpkData64N(sceVif1Packet* pPacket, u_long const* pData, u_int count){
	u_int* ptr = pPacket->pCurrent;
	while(count--){
		u_long const data = *pData++;
		*ptr++ = (u_int)data;
		*ptr++ = (u_int)(data >> 32);
	}
	pPacket->pCurrent = ptr;
}

FUNCTYPE void xVif1PkAddUpkData128(sceVif1Packet* pPacket, u_long128 data){
	u_int* ptr = pPacket->pCurrent;
	union{
		u_long128 q;
		u_int w[4];
	} const tmp = { data };
	*ptr++ = tmp.w[0];
	*ptr++ = tmp.w[1];
	*ptr++ = tmp.w[2];
	*ptr++ = tmp.w[3];
	pPacket->pCurrent = ptr;
}

FUNCTYPE void xVif1PkAddUpkData128N(sceVif1Packet* pPacket, u_long128 const* pData, u_int count){
	u_int* ptr = pPacket->pCurrent;
	while(count--){
		union{
			u_long128 q;
			u_int w[4];
		} const tmp = { *pData++ };
		*ptr++ = tmp.w[0];
		*ptr++ = tmp.w[1];
		*ptr++ = tmp.w[2];
		*ptr++ = tmp.w[3];
	}
	pPacket->pCurrent = ptr;
}

FUNCTYPE void xVif1PkAddDirectData(sceVif1Packet* pPacket, u_long128 data){
	//u_long* ptr = align64(pPacket->pCurrent);
	u_long* ptr = (u_long*)(void*)pPacket->pCurrent;
	union{
		u_long128 q;
		u_long d[2];
	} const tmp = { data };
	*ptr++ = tmp.d[0];
	*ptr++ = tmp.d[1];
	pPacket->pCurrent = (u_int*)ptr;
}

FUNCTYPE void xVif1PkAddDirectData2(sceVif1Packet* pPacket, u_long data0, u_long data1){
	//u_long* ptr = align64(pPacket->pCurrent);
	u_long* ptr = (u_long*)(void*)pPacket->pCurrent;
	*ptr++ = data0;
	*ptr++ = data1;
	pPacket->pCurrent = (u_int*)ptr;
}

FUNCTYPE void xVif1PkAddDirectDataN(sceVif1Packet *pPacket, u_long128 const* pData, u_int count){
	//u_long* ptr = align64(pPacket->pCurrent);
	u_long* ptr = (u_long*)(void*)pPacket->pCurrent;
	u_long const* src = (u_long const*)pData;
	while(count--){
		*ptr++ = *src;
	}
	pPacket->pCurrent = (u_int*)ptr;
}

FUNCTYPE void xVif1PkAddGsData(sceVif1Packet* pPacket, u_long data){
	//u_long* ptr = align64(pPacket->pCurrent);
	u_long* ptr = (u_long*)(void*)pPacket->pCurrent;
	*ptr++ = data;
	pPacket->pCurrent = (u_int*)ptr;
}

FUNCTYPE void xVif1PkAddGsDataN(sceVif1Packet* pPacket, u_long const* pData, u_int count){
	//u_long* ptr = align64(pPacket->pCurrent);
	u_long* ptr = (u_long*)(void*)pPacket->pCurrent;
	while(count--){
		u_long const data = *pData++;
		*ptr++ = data;
	}
	pPacket->pCurrent = (u_int*)ptr;
}

FUNCTYPE void xVif1PkAddQWord(sceVif1Packet* pPacket, u_int d0, u_int d1, u_int d2, u_int d3){
	u_int* ptr = pPacket->pCurrent;
	*ptr++ = d0;
	*ptr++ = d1;
	*ptr++ = d2;
	*ptr++ = d3;
	pPacket->pCurrent = ptr;
}

FUNCTYPE void xVif1PkAddGsPacked(sceVif1Packet* pPacket, u_long128 data){
	//u_long128* ptr = align128(pPacket->pCurrent);
	u_long128* ptr = (u_long128*)(void*)pPacket->pCurrent;
	*ptr++ = data;
	pPacket->pCurrent = (u_int*)ptr;
}

FUNCTYPE void xVif1PkAddGsPackedN(sceVif1Packet* pPacket, u_long128 const* pData, u_int count){
	//u_long128* ptr = align128(pPacket->pCurrent);
	u_long128* ptr = (u_long128*)(void*)pPacket->pCurrent;
	pPacket->pCurrent = (u_int*)memcpy128(ptr, pData, count);
}

FUNCTYPE void xVif1PkAddGsAD(sceVif1Packet* pPacket, u_int addr, u_long data){
	//u_long* ptr = align64(pPacket->pCurrent);
	u_long* ptr = (u_long*)(void*)pPacket->pCurrent;
	*ptr++ = data;
	*ptr++ = (u_long)addr;
	pPacket->pCurrent = (u_int*)ptr;
}

FUNCTYPE void xVif1PkRefLoadImage(sceVif1Packet* pPacket, u_int bp, u_int psm, u_int bw, u_long128 const* image, u_int size, u_int x, u_int y, u_int w, u_int h){
	xVif1PkCnt(pPacket, 0);
	xVif1PkOpenDirectHLCode(pPacket, 0);
	xVif1PkOpenGifTag2(pPacket, SCE_GIF_SET_TAG(0, 0, 0, 0, 0, 1), 0xeUL);
	xVif1PkAddGsAD(pPacket, SCE_GS_BITBLTBUF, SCE_GS_SET_BITBLTBUF(0, 0, 0, bp, bw, psm));
	xVif1PkAddGsAD(pPacket, SCE_GS_TRXPOS, SCE_GS_SET_TRXPOS(0, 0, x, y, 0));
	xVif1PkAddGsAD(pPacket, SCE_GS_TRXREG, SCE_GS_SET_TRXREG(w, h));
	xVif1PkAddGsAD(pPacket, SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR(0));
	xVif1PkCloseGifTag(pPacket);
	xVif1PkCloseDirectHLCode(pPacket);
	while(size){
		u_int const len = (size < 0x00008000U) ? size : 0x00007ff8U;
		xVif1PkCnt(pPacket, 0);
		xVif1PkOpenDirectHLCode(pPacket, 0);
		xVif1PkOpenGifTag2(pPacket, SCE_GIF_SET_TAG(0, (len == size) ? 1 : 0, 0, 0, 2, 0), 0);
		xVif1PkCloseGifTag2(pPacket, len);
		xVif1PkCloseDirectHLCode(pPacket);
		xVif1PkRef(pPacket, image, len, 0, SCE_VIF1_SET_DIRECTHL(len, 0), 0);
		image += len;
		size -= len;
	}
}

FUNCTYPE void xVif1PkRefMpg(sceVif1Packet* pPacket, u_short vuaddr, u_long128 const* pMicro, u_int size, u_int opt1){
	while(size){
		u_int const count = (size > 128) ? 128 : size;
		u_int const num = count * 2;
		xVif1PkRef(pPacket, pMicro, count, opt1, SCE_VIF1_SET_MPG(vuaddr, num & 0xff, 0), 0);
		opt1 = 0;
		pMicro += count;
		size -= count;
		vuaddr += num;
	}
}


FUNCTYPE u_long128* xVif1PkTerminateR(sceVif1Packet* pPacket){
	u_long128* const pDmaCur = align128(pPacket->pCurrent);

	if(NULL != pPacket->pDmaTag){
		u_int count = (pDmaCur - pPacket->pDmaTag) - 1;
		u_int tag = *((u_int*)pPacket->pDmaTag);
		u_int id = (tag >> 28) & 7;

		tag += count;
		if(1 == id){
			// cnt ‚Í end ‚É
			tag = tag | 0x70000000U;
		}

		*((u_int*)pPacket->pDmaTag) = tag;
	}

	pPacket->pCurrent = (u_int*)pDmaCur;
	pPacket->pDmaTag = NULL;

	return pDmaCur;
}

// [eof]
