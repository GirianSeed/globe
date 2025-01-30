/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/* 
 *                   I/O Proseccor sample Library
 *                          Version 0.50
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                       rs2drv.irx - rsd_main.c
 *                           entry function
 *
 *   Version   Date            Design    Log
 *  --------------------------------------------------------------------
 *   0.30      Jun.17.1999     morita    first checked in
 *   0.50      Aug.18.1999     morita    SpuStSetCore etc. added.
 *                                       rewrite for new siflib.
 */

#include <kernel.h>
#include <sys/types.h>
#include <stdio.h>
#include <sif.h>
#include <sifcmd.h>
#include <sifrpc.h>

ModuleInfo Module = {"rspu2_driver", 0x0101 };

#define BASE_priority  32

extern int sce_spu2_loop();
extern volatile int gStThid;

int create_th();

int start ()
{
	BootParam *bp;
	struct ThreadParam param;
	int th;

	FlushDcache();
  
	bp = QueryBootParam(KEY_IOPbootparam);
	if( bp != NULL && (bp->eword[0] & IBP_no_iop_service)!=0 ) {
	 	printf(" No rspu2 driver \n");
	return 1;
	}

	CpuEnableIntr();

	param.attr         = TH_C;
	param.entry        = create_th;
	param.initPriority = BASE_priority;
	param.stackSize    = 0x200;
	param.option       = 0;
	th = CreateThread(&param);
	if (th > 0) {
		StartThread(th,0);
		printf(" Exit rsd_main_main \n");
		return 0;
	}else{
		return 1;
	}
}



int create_th()
{
	struct ThreadParam param;
	int th;

	if( ! sceSifCheckInit() )
		sceSifInit();
	sceSifInitRpc(0);

	printf("rspu2 driver module version 0.5.0 (C)SCEI\n");

	/* SPU2 thread start */
	param.attr         = TH_C;
	param.entry        = sce_spu2_loop;
	param.initPriority = BASE_priority + 2;
	param.stackSize    = 0x800;
	param.option       = 0;
	th = CreateThread(&param);
	StartThread(th,0);
	SleepThread();

	printf(" Exit rsd_main \n");
	return 0;
}


/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* DON'T ADD STUFF AFTER THIS */

