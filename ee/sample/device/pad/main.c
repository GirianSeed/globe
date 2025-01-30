/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                         - <pad> -
 *
 *                         Version 0.10
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            main.c
 *                     read pad data
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.10           Mar,26,1999     ywashizu    structure/macro
 */

#include <eekernel.h>
#include <eeregs.h>
#include <sifdev.h>
#include <libgraph.h>
#include <stdio.h>

/* ================================================================
 *
 *      Program
 *
 * ================================================================ */

/* ２つのパッドを読む場合は,コメントをはずす */
/* #define PADS */

int main()
{
	int fd,i;
	scePadData pd;

        /* GS初期化 */
        // sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);

/* pad の初期化 */
#ifdef PADS
	/* ２つ読みの場合 */
	fd = sceOpen("pads:", SCE_RDONLY| SCE_NOWBDC);
#else
	/* １つ読みの場合 */
	fd = sceOpen("pad:", SCE_RDONLY| SCE_NOWBDC);
#endif
	while(1){
		/* 適当な待ち時間 */
		i = 0x400000;
		while(i --) ;

		/* パッドデータの取得 */
		sceRead(fd, (void *) &pd, sizeof(pd));
		
		/* パッドデータの表示 */
		printf("cnt %d status %x ",pd.cnt,pd.status);

		if ((pd.status & 0xff) == SCE_PAD_ANALOG)
		  printf("button1 %x ana %x %x %x %x ",
			 pd.button, pd.r3h,pd.r3v,pd.l3h,pd.l3v);
		else if ((pd.status & 0xff) == SCE_PAD_NORMAL)
		  printf("button1 %x ", pd.button);
		
		if (((pd.status >> 8) & 0xff) == SCE_PAD_ANALOG )
		  printf("button2 %x ana %x %x %x %x ",
			 pd.button2, pd.r3h2,pd.r3v2,pd.l3h2,pd.l3v2);
		else if (((pd.status >> 8) & 0xff) == (SCE_PAD_NORMAL << 8))
		  printf("button2 %x ", pd.button2);
		
		printf("\n");


	}

	sceClose(fd);
}

/* __main(){} */
