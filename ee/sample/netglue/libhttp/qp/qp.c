/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                           - qp -
 *                        Version 1.0.0
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                           <qp.c>
 *                      <main functions>
 *
 *       Version   Date           Design              Log
 *  --------------------------------------------------------------------
 *       1.0.0     Oct,22,2001    komaki              first version
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <malloc.h>
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
#include <libqp.h>

#define ERR_STOP	while(1)

#if defined(USE_ERX)
#define LIBERX_THREAD_PRIORITY      10
#define LIBERX_MEMORY_SIZE          (8*1024*1024)
#define LIBERX_MAX_MODULES          32

static int libqp_erxid;

#define EE_MOD_ROOT       "host0:/usr/local/sce/ee/modules/"
#define EE_MOD_LIBQP	  EE_MOD_ROOT "libqp.erx"

int InitErxEnvironment(void *pool, int pool_size);
#endif

int main(int argc, char **argv){
	int encode = 0;
	int ifd, ofd;
	unsigned int isize = 0;
	char *data;
	int n, m;

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

	/* load libqp.erx */
	while((libqp_erxid = sceErxLoadStartModuleFile(EE_MOD_LIBQP, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("libqp.erx cannot be loaded.\n");

#endif
	if (argc < 3){
usage:
		scePrintf("usage: run qp.elf (-encode|-decode) input output\n");
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

	data = (char *)malloc(512);
	if (data == NULL){
		scePrintf("can't allocate memory\n");
		ERR_STOP;
	}
	while (1){
		char *tmp;
		n = sceRead(ifd, data + isize, 512);
		if (n < 0){
			scePrintf("read error\n");
			ERR_STOP;
		}
		isize += n;
		if (n < 512)
			break;
		tmp = realloc(data, isize + 512);
		if (tmp == NULL){
			scePrintf("can't allocate memory\n");
			ERR_STOP;
		}
		data = tmp;
	}

	if (encode){
		char *odata = (char *)malloc(4 * isize);
		if (odata == NULL){
			scePrintf("can't allocate memory\n");
			ERR_STOP;
		}
		m = sceQPrintableEncoder(data, odata, isize);
		n = sceWrite(ofd, odata, m);
		free(odata);
		if (n != m){
			scePrintf("write error\n");
			ERR_STOP;
		}
	} else {
		char *s = data;
		char line[80];
		char obuf[240];
		int count = 0;
		int prev = 0;

		while (isize > 0){
			if (s[0] == 0x0d && s[1] == 0x0a){
				line[count] = 0, s += 2, isize -= 2;
				m = sceQPrintableLineDecoder(line, obuf,
					strlen(line));
				n = sceWrite(ofd, obuf, m);
				if (n != m){
					scePrintf("write error\n");
					ERR_STOP;
				}
				if (prev != '='){
					if (2 != sceWrite(ofd, "\r\n", 2)){
						scePrintf("write error\n");
						ERR_STOP;
					}
				}
				prev = 0;
				count = 0;
			} else
				prev = line[count++] = *s++, --isize;
		}
		if (count){
			line[count] = 0;
			m = sceQPrintableLineDecoder(line, obuf, strlen(line));
			n = sceWrite(ofd, obuf, m);
			if (n != m){
				scePrintf("write error\n");
				ERR_STOP;
			}
		}
	}

	free(data);
	sceClose(ifd);
	sceClose(ofd);

#if defined(USE_ERX)
	if(0 <= sceErxStopModule(libqp_erxid, 0, NULL, &ret))
		sceErxUnloadModule(libqp_erxid);
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
