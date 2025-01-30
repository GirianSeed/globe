/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
 */
/* 
 *                libpfont Library Sample Program
 *
 *                           - basic -
 *
 *                          Version 1.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                              main.c
 *                      main function of basic.
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            Jan,14,2002      gens
 */

#include <eetypes.h>
#include <eeregs.h>
#include <eestruct.h>
#include <eekernel.h>

#include <libdma.h>
#include <libgraph.h>
#include <sifdev.h>

#include "sample.h"

static void _init(void);

int main(int argc, char* argv[])
{
	int		ret = -1;
	int		i;

	scePrintf("main(): start\n");
	scePrintf("  argc = %d\n", argc - 1);

	for(i = 0; i < argc; i++)
	{
		scePrintf("  argv[%d] = %s\n", i, argv[i]);
	}
	/* ��{������ */
	_init();

	for(i = 0; i < 2; i++)
	{
		/* �t�H���g�T���v���A�v�������� */
		if(0 <= appInit())
		{
			/* ���� */
			scePrintf("appRun(): start\n");

			/* �t�H���g�T���v���A�v�����s */
			ret	= appRun(argc, argv);
		}
		else
		{
			/* ���s */
			scePrintf("appInit(): failed\n");
		}
		scePrintf("appExit()\n");

		/* �t�H���g�T���v���A�v�������� */
		appExit();
	}
	scePrintf("result = %d\n", ret);
	return ret;
}

static void _init(void)
{
	/* ���������� */
	/* reset dma */
	scePrintf("reset dma\n");
	sceDmaReset(1);

	/* reset gs */
	scePrintf("reset gs\n");
	sceGsResetPath();
	sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
}

/* [eof] */
