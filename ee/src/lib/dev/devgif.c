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
 *                        libpkt - devgif.c
 *                         develop library
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.01           Mar,29,1999     shibuya
 *       0.02           Apr,29,2003     hana
 */

#include <eekernel.h>
#include <eeregs.h>
#include "devgif.h"

static int sceDevGifSync(void);

void sceDevGifReset(void)
{
	DPUT_GIF_CTRL(0x00000001);
}

int sceDevGifPause(void)
{
	DPUT_GIF_CTRL(0x00000008);
	return 1;
}

int sceDevGifContinue(void)
{
	int stat = sceDevGifSync();

	if (stat != 1) {
		return 0;
	}
	/*  not pause   */
	DPUT_GIF_CTRL(0x00000000);
	return 1;
}

void sceDevGifPutImtMode(int mode)
{
	if (mode == 0) {
		DPUT_GIF_MODE(sceDevGifGetImtMode() & ~0x00000004);

	} else if (mode == 1) {
		DPUT_GIF_MODE(sceDevGifGetImtMode() | 0x00000004);
	}
}

u_int sceDevGifGetImtMode(void)
{
	return (DGET_GIF_STAT() & 0x00000004) >> 2;
}

int sceDevGifPutP3msk(int enable)
{
	if (enable == 0) {
		DPUT_GIF_MODE(sceDevGifGetImtMode() & ~1);

	} else if (enable == 1) {
		DPUT_GIF_MODE(sceDevGifGetImtMode() | 1);
	}
	return 1;
}

int sceDevGifGetP3msk(void)
{
	return (sceDevGifGetImtMode() & 1);
}

static int sceDevGifSync(void)
{
	u_int stat = DGET_GIF_STAT();

	if (stat & 0x00000008) {
		return 1;
	}							/*  PSE         */
	if ((stat & 0x00000fc0) != 0) {
		return 2;
	}							/*  ‚È‚ñ‚ç‚©‚Ìƒf[ƒ^‚ ‚è    */
	return 0;
}

int sceDevGifGetCnd(sceDevGifCnd * cnd)
{
	u_int *pTag;
	int stat = sceDevGifSync();

	if (2 == stat) {
		return 0;
	}

	pTag = (u_int *) & cnd->tag;
	pTag[0] = DGET_GIF_TAG0();
	pTag[1] = DGET_GIF_TAG1();
	pTag[2] = DGET_GIF_TAG2();
	pTag[3] = DGET_GIF_TAG3();
	cnd->stat = DGET_GIF_STAT();
	cnd->count = DGET_GIF_CNT();
	cnd->p3count = DGET_GIF_P3CNT();
	cnd->p3tag = DGET_GIF_P3TAG();
	return 1;
}

int sceDevGifPutFifo(u_long128 * addr, int n)
{
	u_int count = 0;

	while (n--) {
		if (DGET_GIF_STAT() & 0x10000000) {
			return count;
		}						/*  fifo full? */
		count++;
		DPUT_GIF_FIFO(*addr++);
	}
	return count;
}

/* [eof] */

