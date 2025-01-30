/* SCE CONFIDENTIAL
   "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
*/
/*
 *     Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                   All Rights Reserved.
 *
 */

#include <eetypes.h>
#include <eeregs.h>
#include <eestruct.h>
#include <eekernel.h>

#include <libdma.h>
#include <libgraph.h>
#include <sifdev.h>
#include <sifrpc.h>

#include "misc.h"
#include "debug.h"

extern int appInit(void);
extern int appRun(int argc, char const* const argv[]);
extern void appExit(void);

#define IRXPATH(irx)	"host0:/usr/local/sce/iop/modules/"irx

static int _init(void);

int main(int argc, char const * const argv[]){

	int ret = -1;

	UNUSED_ALWAYS(argc);
	UNUSED_ALWAYS(argv);

	TRACE("main(): start\n");

	if(0 > _init()){
		return ret;
	}

	if(0 > appInit()){
		TRACE("appInit(): failed\n");

	}else{
		TRACE("appRun(): start\n");
		ret = appRun(argc, argv);
	}

	TRACE("appExit()\n");
	appExit();

	TRACE("result = %d\n", ret);

	return ret;
}

static int _init(void){

	// èâä˙âªèàóù
	{	// reset dma
		TRACE("reset dma\n");
		sceDmaReset(1);
	}

	{	// reset gs
		TRACE("reset gs\n");
		sceGsResetPath();
		sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
	}

#if 1
	{	// init iop
		TRACE("reboot iop\n");
		sceSifInitRpc(0);

		while(0 == sceSifRebootIop(IRXPATH(IOP_IMAGE_file))){
			// ê¨å˜Ç∑ÇÈÇ‹Ç≈î≤ÇØÇ»Ç¢
		}

		while(0 == sceSifSyncIop()){
			// ê¨å˜Ç∑ÇÈÇ‹Ç≈î≤ÇØÇ»Ç¢
		}
		sceSifInitRpc(0);
	}
#endif

	return 0;
}

// [eof]
