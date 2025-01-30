/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                         - base64 -
 *                        Version 1.0.0
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          <base64.c>
 *                       <main functions>
 *
 *       Version   Date           Design              Log
 *  --------------------------------------------------------------------
 *       1.0.0     Oct,22,2001    komaki              first version
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <eekernel.h>
#if defined(USE_ERX)
#include <liberx.h>
#include <libgcc_export.h>
#include <libc_export.h>
#include <malloc.h>
#endif 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <eeregs.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <errno.h>
#include <base64.h>

#define ERR_STOP	while(1)

#define	LEN		((76 / 4) * 3)

#if defined(USE_ERX)
#define LIBERX_THREAD_PRIORITY      10
#define LIBERX_MEMORY_SIZE          (8*1024*1024)
#define LIBERX_MAX_MODULES          32

static int base64_erxid;

#define EE_MOD_ROOT       "host0:/usr/local/sce/ee/modules/"
#define EE_MOD_BASE64	  EE_MOD_ROOT "base64.erx"

int InitErxEnvironment(void *pool, int pool_size);
#endif

int main(int argc, char **argv){
	int encode = 0;
	int ifd, ofd;
	int n, m;
	char lbuf[80];
	char obuf[80];
#if defined(USE_ERX)
	int ret;
	void *liberx_pool;
#endif

#if defined(USE_ERX)
	/* init erx library */
	if((liberx_pool = memalign(256, LIBERX_MEMORY_SIZE)) == NULL){
		scePrintf("memalign() failed.\n");
		ERR_STOP;
	}
	if(InitErxEnvironment(liberx_pool, LIBERX_MEMORY_SIZE) < 0){
		scePrintf("InitErxEnvironment() failed.\n");
		ERR_STOP;
	}

	/* load base64.erx */
	while((base64_erxid = sceErxLoadStartModuleFile(EE_MOD_BASE64, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("base64.erx cannot be loaded.\n");

#endif

	if (argc < 3){
usage:
		scePrintf("usage: run base64.elf (-encode|-decode) input output\n");
		ERR_STOP;
	}

	if (strcmp (argv[1], "-encode") == 0)
		encode = 1;
	else if (strcmp (argv[1], "-decode") == 0)
		encode = 0;
	else
		goto usage;

	ifd = sceOpen(argv[2], SCE_RDONLY);
	if (ifd < 0){
		scePrintf("can't open %s\n", argv[2]);
		ERR_STOP;
	}
	ofd = sceOpen(argv[3], SCE_WRONLY|SCE_CREAT|SCE_TRUNC);
	if (ofd < 0){
		scePrintf("can't open %s\n", argv[3]);
		ERR_STOP;
	}

	if (encode){
		while (1){
			n = sceRead(ifd, lbuf, LEN);
			if (n == 0)
				break;
			if (n < 0){
				scePrintf("read error\n");
				ERR_STOP;
			}
			m = sceBASE64Encoder(lbuf, obuf, n);
			obuf[m] = 0x0d; obuf[m+1] = 0x0a;
			n = sceWrite(ofd, obuf, m+2);
			if (n != m+2){
				scePrintf("write error\n");
				ERR_STOP;
			}
		}
	} else {
		while (1){
			n = sceRead(ifd, lbuf, 76 + 2);
			if (n == 0)
				break;
			if (n < 0){
				scePrintf("read error\n");
				ERR_STOP;
			}
			n -= 2;
			m = sceBASE64LineDecoder(lbuf, obuf, n);
			n = sceWrite(ofd, obuf, m);
			if (n != m){
				scePrintf("write error\n");
				ERR_STOP;
			}
		}
	}

	sceClose(ifd);
	sceClose(ofd);

#if defined(USE_ERX)
	if(0 <= sceErxStopModule(base64_erxid, 0, NULL, &ret))
		sceErxUnloadModule(base64_erxid);
#endif

	return 0;
}

#if defined(USE_ERX)
int InitErxEnvironment(
	void *pool,
	int pool_size
	)
{
	int res;

	res = sceErxInit(LIBERX_THREAD_PRIORITY, pool,
		pool_size, LIBERX_MAX_MODULES);
	scePrintf("sceErxInit: %d\n", res);

	/* export libgcc function */
	res = sceErxRegisterLibraryEntries(sceLibgccCommonGetErxEntries());
	scePrintf("sceErxExportLibgccCommon:   %08X\n", res);
	res = sceErxRegisterLibraryEntries(sceLibcStdioGetErxEntries());
	scePrintf("sceErxExportLibcStdio:   %08X\n", res);
	res = sceErxRegisterLibraryEntries(sceLibcStringGetErxEntries());
	scePrintf("sceErxExportLibcString:   %08X\n", res);
	res = sceErxRegisterLibraryEntries(sceLibcStdlibGetErxEntries());
	scePrintf("sceErxExportLibcStdlib:   %08X\n", res);
	res = sceErxRegisterLibraryEntries(sceLibcMallocGetErxEntries());
	scePrintf("sceErxExportLibcMalloc:   %08X\n", res);
	res = sceErxRegisterLibraryEntries(sceLibcErrnoGetErxEntries());
	scePrintf("sceErxExportLibcErrno:   %08X\n", res);

	return 0;
}
#endif
