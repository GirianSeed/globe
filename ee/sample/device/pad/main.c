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

/* �Q�̃p�b�h��ǂޏꍇ��,�R�����g���͂��� */
/* #define PADS */

int main()
{
	int fd,i;
	scePadData pd;

        /* GS������ */
        // sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);

/* pad �̏����� */
#ifdef PADS
	/* �Q�ǂ݂̏ꍇ */
	fd = sceOpen("pads:", SCE_RDONLY| SCE_NOWBDC);
#else
	/* �P�ǂ݂̏ꍇ */
	fd = sceOpen("pad:", SCE_RDONLY| SCE_NOWBDC);
#endif
	while(1){
		/* �K���ȑ҂����� */
		i = 0x400000;
		while(i --) ;

		/* �p�b�h�f�[�^�̎擾 */
		sceRead(fd, (void *) &pd, sizeof(pd));
		
		/* �p�b�h�f�[�^�̕\�� */
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
