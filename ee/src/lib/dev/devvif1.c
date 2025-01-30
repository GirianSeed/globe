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
 *                        libpkt - devvif1.c
 *                         develop library
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.01           Mar,29,1999     shibuya
 *       0.02           Apr,29,2003     hana
 */

#include <eekernel.h>
#include <eeregs.h>
#include "devvif1.h"

static u_int sceDevVif1Sync(void);

void sceDevVif1Reset(void)
{
	DPUT_VIF1_FBRST(0x00000001);
	DPUT_VIF1_ERR(0x00000006);
}

int sceDevVif1Pause(int mode)
{
	u_int stat = sceDevVif1Sync();

	if (stat == 0) {
		return 0;
	}							/*  idle        */
	if (stat == 1) {
		return 0;
	}
	/*  stall       */
	if (mode == 0) {
		DPUT_VIF1_FBRST(0x00000002);	/*  force break */

	} else if (mode == 1) {
		DPUT_VIF1_FBRST(0x00000004);	/*  stop        */
	}
	return 1;
}

int sceDevVif1Continue(void)
{
	u_int stat = sceDevVif1Sync();

	if (stat != 1) {
		return 0;
	}							/*  not stall?  */
	DPUT_VIF1_FBRST(0x00000008);	/*  stall cancel */
	return 1;
}

u_int sceDevVif1PutErr(int interrupt, int miss1, int miss2)
{
	u_int err = interrupt | (miss1 << 1) | (miss2 << 2);

	DPUT_VIF1_ERR(err);
	return err;
}

u_int sceDevVif1GetErr(void)
{
	return DGET_VIF1_ERR();
}

static u_int sceDevVif1Sync(void)
{
	u_int stat = DGET_VIF1_STAT();

	if (0 == (stat & 0x00000003)) {
		return 0;
	}							/*  idle    */
	if (stat & 0x00000700) {
		return 1;
	}							/*  stall   */
	if (stat & 0x0000000c) {
		return 2;
	}							/*  wait    */
	if (stat & 0x00003000) {
		return 3;
	}							/*  error   */
	return 4;					/*  busy    */
}

int sceDevVif1GetCnd(sceDevVif1Cnd * cnd)
{
	u_int cycle = DGET_VIF1_CYCLE();
	u_int stat = sceDevVif1Sync();

	if (1 < stat) {
		return 0;
	}

	cnd->row[0] = DGET_VIF1_R0();
	cnd->row[1] = DGET_VIF1_R1();
	cnd->row[2] = DGET_VIF1_R2();
	cnd->row[3] = DGET_VIF1_R3();
	cnd->col[0] = DGET_VIF1_C0();
	cnd->col[1] = DGET_VIF1_C1();
	cnd->col[2] = DGET_VIF1_C2();
	cnd->col[3] = DGET_VIF1_C3();
	cnd->mask = DGET_VIF1_MASK();
	cnd->code = DGET_VIF1_CODE();
	cnd->stat = DGET_VIF1_STAT();
	cnd->itop = (u_short) DGET_VIF1_ITOP();
	cnd->itops = (u_short) DGET_VIF1_ITOPS();
	cnd->base = (u_short) DGET_VIF1_BASE();
	cnd->offset = (u_short) DGET_VIF1_OFST();
	cnd->top = (u_short) DGET_VIF1_TOP();
	cnd->tops = (u_short) DGET_VIF1_TOPS();
	cnd->mark = (u_short) DGET_VIF1_MARK();
	cnd->num = (u_short) DGET_VIF1_NUM();
	cnd->error = (u_char) DGET_VIF1_ERR();
	cnd->cl = (u_char) (cycle);
	cnd->wl = (u_char) (cycle >> 8);
	cnd->cmod = (u_char) DGET_VIF1_MODE();

	return 1;
}

int sceDevVif1PutFifo(u_long128 * addr, int n)
{
	u_int count = 0;

	while (n--) {
		if (DGET_VIF1_STAT() & 0x10000000) {
			return count;
		}						/*  fifo full? */
		count++;
		DPUT_VIF1_FIFO(*addr++);
	}

	return count;
}

int sceDevVif1GetFifo(u_long128 * addr, int n)
{
	u_int count = 0;

	while (n--) {
		if (0 == (DGET_VIF1_STAT() & 0x1f000000)) {
			return count;
		}						/*  fifo empty? */
		count++;
		*addr++ = DGET_VIF1_FIFO();
	}

	return count;
}

/* [eof] */

