/* SCE CONFIDENTIAL
   "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
*/
/*
 *     Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                   All Rights Reserved.
 *
 */

#ifndef __xvifpk__
#define __xvifpk__

#include <eekernel.h>
#include <eestruct.h>
#include <libvifpk.h>

void xVif1PkInit(sceVif1Packet* pPacket, u_long128* pBase);

void xVif1PkReset(sceVif1Packet* pPacket);
u_long128* xVif1PkTerminate(sceVif1Packet* pPacket);

u_int xVif1PkSize(sceVif1Packet* pPacket);

void xVif1PkCnt(sceVif1Packet* pPacket, u_int flag);
void xVif1PkRet(sceVif1Packet* pPacket, u_int flag);
void xVif1PkEnd(sceVif1Packet* pPacket, u_int flag);

void xVif1PkNext(sceVif1Packet* pPacket, u_long128 const* pNext, u_int flag);
void xVif1PkCall(sceVif1Packet* pPacket, u_long128 const* pCall, u_int flag);

void xVif1PkRefe(sceVif1Packet* pPacket, u_long128 const* pRef, u_int size, u_int opt1, u_int opt2, u_int flag);
void xVif1PkRef(sceVif1Packet* pPacket, u_long128 const* pRef, u_int size, u_int opt1, u_int opt2, u_int flag);
void xVif1PkRefs(sceVif1Packet* pPacket, u_long128 const* pRef, u_int size, u_int opt1, u_int opt2, u_int flag);

void xVif1PkOpenUpkCode(sceVif1Packet* pPacket, u_short vuaddr, u_int upkcmd, u_int cl, u_int wl);
void xVif1PkCloseUpkCode(sceVif1Packet* pPacket);

void xVif1PkOpenDirectCode(sceVif1Packet* pPacket, int stall);
void xVif1PkCloseDirectCode(sceVif1Packet* pPacket);

void xVif1PkOpenDirectHLCode(sceVif1Packet* pPacket, int stall);
void xVif1PkCloseDirectHLCode(sceVif1Packet* pPacket);

void xVif1PkOpenGifTag(sceVif1Packet* pPacket, u_long128 gifTag);
void xVif1PkOpenGifTag2(sceVif1Packet* pPacket, u_long gifTag, u_long regs);

void xVif1PkCloseGifTag(sceVif1Packet* pPacket);
void xVif1PkCloseGifTag2(sceVif1Packet* pPacket, u_int nloop);

void xVif1PkAddQWord(sceVif1Packet* pPacket, u_int d0, u_int d1, u_int d2, u_int d3);
u_int* xVif1PkReserve(sceVif1Packet* pPacket, u_int count);
void xVif1PkAlign(sceVif1Packet* pPacket, u_int bit, u_int pos);

void xVif1PkAddCode(sceVif1Packet* pPacket, u_int code);
void xVif1PkAddData(sceVif1Packet* pPacket, u_int data);
void xVif1PkAddDataN(sceVif1Packet* pPacket, u_int const* pData, u_int count);

void xVif1PkAddUpkData32(sceVif1Packet* pPacket, u_int data);
void xVif1PkAddUpkData64(sceVif1Packet* pPacket, u_long data);
void xVif1PkAddUpkData128(sceVif1Packet* pPacket, u_long128 data);
void xVif1PkAddUpkData32N(sceVif1Packet* pPacket, u_int const* pData, u_int count);
void xVif1PkAddUpkData64N(sceVif1Packet* pPacket, u_long const* pData, u_int count);
void xVif1PkAddUpkData128N(sceVif1Packet* pPacket, u_long128 const* pData, u_int count);

void xVif1PkAddDirectData(sceVif1Packet* pPacket, u_long128 data);
void xVif1PkAddDirectData2(sceVif1Packet* pPacket, u_long data0, u_long data1);
void xVif1PkAddDirectDataN(sceVif1Packet* pPacket, u_long128 const* pData, u_int count);

void xVif1PkAddGsData(sceVif1Packet* pPacket, u_long data);
void xVif1PkAddGsDataN(sceVif1Packet* pPacket, u_long const* pData, u_int count);

void xVif1PkAddGsPacked(sceVif1Packet* pPacket, u_long128 data);
void xVif1PkAddGsPackedN(sceVif1Packet* pPacket, u_long128 const* pData, u_int count);

void xVif1PkAddGsAD(sceVif1Packet* pPacket, u_int addr, u_long data);

void xVif1PkRefLoadImage(sceVif1Packet* pPacket, u_int bp, u_int psm, u_int bw, u_long128 const* image, u_int size, u_int x, u_int y, u_int w, u_int h);

void xVif1PkRefMpg(sceVif1Packet* pPacket, u_short vuaddr, u_long128 const* pMicro, u_int size, u_int opt1);

// 開いてる DmaTag が cnt だった場合強制的に end に置き換える。
u_long128* xVif1PkTerminateR(sceVif1Packet* pPacket);

#endif /* __xvifpk__ */

#ifdef __INLINE__

#ifndef __xvifpk_c__
#define __xvifpk_c__
#include "xvifpk.c"
#endif /* __xvifpk_c__ */

#endif /* __INLINE__ */

