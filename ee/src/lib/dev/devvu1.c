/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                      Emotion Engine Library
 *                          Version 0.01
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        libpkt - devvu1.c
 *                         develop library
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.01           Mar,29,1999     shibuya
 *       0.02           Apr,29,2003     hana
 */

#include <eekernel.h>
#include <eeregs.h>
#include "devvu1.h"

typedef unsigned char u_int8;
typedef unsigned short u_int16;
typedef unsigned int u_int32;
typedef unsigned long u_int64;
typedef u_long128 u_int128;

typedef struct {
	union {
		u_int128 v;
		u_int128 q[1];
		u_int64 d[2];
		u_int32 s[4];
	} _;
} qword;

static int sceDevVu1Sync();

static u_int sceVuCheckBusy()
{
	register u_int reg;
	__asm__ volatile ("cfc2 %0, $vi29" : "=r"(reg));	/*  vpu stat    */

	if (0x00000001 == (reg & 0x0000000f)) {
		return 1;
	}							/*  vu0 busy ?  */
	if (0x00000100 == (reg & 0x00000f00)) {
		return 1;
	}							/*  vu1 busy ?  */
	return 0;
}

/*	VU0の直接見えないアドレスへデータを無理やり書き込む	*/
static void sceVu0MemWriteQ(u_short vuaddr, qword value)
{
	register u_long128 reg = value._.v;
	register u_long128 fbak;
	register u_int ibak;

	__asm__ volatile (
		"cfc2 %0, $vi01\n"				/*  VU0-VI01を待避              */
		"qmfc2 %1, $vf01\n"				/*  VU0-VF01をfbakへ待避        */

		"qmtc2 %2, $vf01\n"				/*  VU0-VF01へvalueをロード     */

		"ctc2 %3, $vi01\n"				/*  VU0-VI01にアドレスをセット  */
		"vsqi.xyzw $vf01, ($vi01++)\n"	/*  VU0-MEM[vuaddr]へデータ設定 */

		"ctc2 %0, $vi01\n"				/*  VU0-VI01を復帰              */
		"qmtc2 %1, $vf01\n"				/*  VU0-VF01を復帰              */
		: "=&r"(ibak), "=&r"(fbak) : "r"(reg), "r"(vuaddr)
	);
}

/*	VU0の直接見えないアドレスからデータを無理やり読み出す	*/
static qword sceVu0MemReadQ(u_short vuaddr)
{
	qword result;
	register u_long128 fbak;
	register u_int ibak;
	register u_long128 reg;

	__asm__ volatile (
		"cfc2 %0, $vi01\n"				/*  VU0-VI01を待避              */
		"qmfc2 %1, $vf01\n"				/*  VU0-VF01をfbakへ待避        */

		"ctc2 %3, $vi01\n"				/*  VU0-VI01にアドレスをセット  */
		"vlqi.xyzw $vf01, ($vi01++)\n"	/*  VU0-MEM[vuaddr]のデータ取得 */

		"qmfc2 %2, $vf01\n"				/*  VU0-VF01からデータ取得      */

		"ctc2 %0, $vi01\n"				/*  VU0-VI01を復帰              */
		"qmtc2 %1, $vf01\n"				/*  VU0-VF01を復帰              */
		: "=&r"(ibak), "=&r"(fbak), "=r"(reg) : "r"(vuaddr)
	);

	result._.v = reg;
	return result;
}


void sceDevVu1Reset(void)
{
	register u_int reg;
	__asm__ volatile ("cfc2 %0, $vi28" : "=r"(reg));

	reg |= 0x00000200;
	__asm__ volatile ("ctc2 %0, $vi28" : : "r"(reg));
}

int sceDevVu1Pause(void)
{
	int result = 0;

	switch (sceDevVu1Sync()) {
	case 1:
	case 2:
		break;

	case 4:
		{
			register u_int reg;
			__asm__ volatile ("cfc2 %0, $vi28" : "=r"(reg));

			reg |= 0x00000100;
			__asm__ volatile ("ctc2 %0, $vi28" : : "r"(reg));
		}
	case 0:
	case 3:
		result = 1;
		break;
	}

	return result;
}

int sceDevVu1Continue(void)
{
	int result = 0;

	switch (sceDevVu1Sync()) {
	case 0:
	case 4:
		break;

	case 1:
	case 2:
	case 3:
		{
			sceDevVu1Exec(sceDevVu1GetTpc());
		}
		result = 1;
		break;
	}

	return result;
}

void sceDevVu1PutDBit(int dbit)
{
	register u_int reg;

	__asm__ volatile ("cfc2 %0, $vi28" : "=r"(reg));

	if (1 == dbit) {
		reg |= 0x00000400;

	} else {
		reg &= ~0x00000400;
	}
	__asm__ volatile ("ctc2 %0, $vi28" : : "r"(reg));
}

void sceDevVu1PutTBit(int tbit)
{
	register u_int reg;

	__asm__ volatile ("cfc2 %0, $vi28" : "=r"(reg));

	if (1 == tbit) {
		reg |= 0x00000800;

	} else {
		reg &= ~0x00000800;
	}
	__asm__ volatile ("ctc2 %0, $vi28" : : "r"(reg));
}

int sceDevVu1GetDBit(void)
{
	register u_int reg;
	__asm__ volatile ("cfc2 %0, $vi28" : "=r"(reg));

	return (int)((reg >> 10) & 1);
}

int sceDevVu1GetTBit(void)
{
	register u_int reg;
	__asm__ volatile ("cfc2 %0, $vi28" : "=r"(reg));

	return (int)((reg >> 11) & 1);
}

static int sceDevVu1Sync(void)
{
	int status = 0;
	register u_int reg;
	__asm__ volatile ("cfc2 %0, $vi29" : "=r"(reg));

	if (reg & 0x00000200) {
		status = 1;				//  d stop

	} else if (reg & 0x00000400) {
		status = 2;				//  t stop

	} else if (reg & 0x00000800) {
		status = 3;				//  force break

	} else if (reg & 0x00000100) {
		status = 4;				//  run

	} else {
		status = 0;				//  idle
	}

	return status;
}

void sceDevVu1Exec(u_short addr)
{
	register u_int reg = addr;
	__asm__ volatile ("ctc2 %0, $vi31" : : "r"(reg));
}

u_short sceDevVu1GetTpc(void)
{
	return (u_short)(sceVu0MemReadQ(0x43a)._.d[0]);
}

int sceDevVu1GetCnd(sceDevVu1Cnd * cnd)
{
	u_int i;

	if (sceVuCheckBusy()) {
		return 0;
	}

	for (i = 0; i < 32; i++) {
		cnd->vf[i] = sceVu0MemReadQ(0x400 + i)._.v;
	}

	cnd->status = sceVu0MemReadQ(0x430)._.d[0];
	cnd->mac = sceVu0MemReadQ(0x431)._.d[0];
	cnd->clipping = sceVu0MemReadQ(0x432)._.d[0];

	cnd->r = sceVu0MemReadQ(0x434)._.d[0];
	cnd->i = sceVu0MemReadQ(0x435)._.d[0];
	cnd->q = sceVu0MemReadQ(0x436)._.d[0];
	cnd->p = sceVu0MemReadQ(0x437)._.d[0];

	for (i = 0; i < 16; i++) {
		cnd->vi[i] = (u_short) sceVu0MemReadQ(0x420 + i)._.d[0];
	}
	return 1;
}

int sceDevVu1PutCnd(sceDevVu1Cnd * cnd)
{
	u_int i;
	qword work;

	if (sceVuCheckBusy()) {
		return 0;
	}

	for (i = 0; i < 32; i++) {
		sceVu0MemWriteQ(0x400 + i, (work._.v = cnd->vf[i], work));
	}

	work._.v = 0;

	sceVu0MemWriteQ(0x430, (work._.d[0] = cnd->status, work));
	sceVu0MemWriteQ(0x431, (work._.d[0] = cnd->mac, work));
	sceVu0MemWriteQ(0x432, (work._.d[0] = cnd->clipping, work));

	sceVu0MemWriteQ(0x434, (work._.d[0] = cnd->r, work));
	sceVu0MemWriteQ(0x435, (work._.d[0] = cnd->i, work));
	sceVu0MemWriteQ(0x436, (work._.d[0] = cnd->q, work));
	sceVu0MemWriteQ(0x437, (work._.d[0] = cnd->p, work));

	for (i = 0; i < 16; i++) {
		sceVu0MemWriteQ(0x420 + i, (work._.d[0] = (u_int) cnd->vi[i], work));
	}
	return 1;
}

/* [eof] */

