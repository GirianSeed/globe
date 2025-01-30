/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0.2
 */
/* 
 *      Netcnf Interface Library
 *
 *                         Version 1.2
 *                         Shift-JIS
 *
 *      Copyright (C) 2002-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         netcnfif.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 *       1.2            2003.09.18      ksh         heap
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel.h>

#include "interface.h"
#include "mem.h"

ModuleInfo Module = { "Netcnf_Interface", (2<<8)|30 };


static int g_tid;

static void usage(void)
{
	printf("Usage: netcnfif <option>\n");
	printf("    <option>:\n");
	printf("    thpri=<digit>     - set thread priority\n");
	printf("    thstack=<digit>KB - set thread stack size(Kbyte)\n");
	printf("    thstack=<digit>   - set thread stack size(byte)\n");
	printf("    -help             - print usage\n");
}

static int module_start(int argc, char *argv[])
{
	extern libhead netcnfif_entry;
	struct ThreadParam th_param;
	char *bp;
	int thpri = USER_LOWEST_PRIORITY;
	int thstack = 4 * 1024;
	int r;

	for (--argc, ++argv; 0 < argc; --argc, ++argv) {
		if (!strncmp("thpri=", argv[0], 6)) {
			bp = argv[0] + 6;
			if (!isdigit(*bp)) {
				goto bad_usage;
			}
			thpri = atoi(bp);
			if (thpri < USER_HIGHEST_PRIORITY || USER_LOWEST_PRIORITY < thpri) {
				goto bad_usage;
			}
			for ( ; *bp != '\0' && isdigit(*bp); bp++)
			;
			if (*bp != '\0') {
				goto bad_usage;
			}
		}
		else if (!strncmp("thstack=", argv[0], 8)) {
			bp = argv[0] + 8;
			if (!isdigit(*bp)) {
				goto bad_usage;
			}
			thstack = atoi(bp);
			for ( ; *bp != '\0' && isdigit(*bp); bp++)
			;
			if (!strcmp(bp, "KB")) {
				thstack <<= 10;
			}
			else if (*bp != '\0') {
				goto bad_usage;
			}
		}
		else {
			goto bad_usage;
		}
	}

	r = RegisterLibraryEntries(&netcnfif_entry);
	if (r != KE_OK) {
		printf("netcnfif: RegisterLibraryEntries(%d)\n", r);
		return (NO_RESIDENT_END);
	}

	r = my_create_heap();
	if (r < 0) {
		printf("netcnfif: c_heap(%d)\n", r);
		goto bad1;
	}

	th_param.attr = TH_C;
    th_param.entry = sceNetcnfifInterfaceStart;
	th_param.initPriority = thpri;
	th_param.stackSize = thstack;
	th_param.option = 0;

	g_tid = CreateThread(&th_param);
	if (g_tid < 0) {
		printf("netcnfif: c_thread(%d)\n", g_tid);
		goto bad2;
	}

	r = StartThread(g_tid, 0);
	if (r < 0) {
		printf("netcnfif: s_thread(%d)\n", r);
		goto bad3;
	}

	return (REMOVABLE_RESIDENT_END);

bad3:
	TerminateThread(g_tid);
	DeleteThread(g_tid);
bad2:
	my_delete_heap();
bad1:
	ReleaseLibraryEntries(&netcnfif_entry);
	return (NO_RESIDENT_END);

bad_usage:
	usage();
	return (NO_RESIDENT_END);
}

static int module_stop(int argc, char *argv[])
{
	extern libhead netcnfif_entry;

	(void)argc;
	(void)argv;
	sceNetcnfifInterfaceStop();

	TerminateThread(g_tid);
	DeleteThread(g_tid);
	my_delete_heap();
	ReleaseLibraryEntries(&netcnfif_entry);

	return (NO_RESIDENT_END);
}

int start(int argc, char *argv[])
{
	if (argc >= 0) {
		return (module_start(argc, argv));
	}
	return (module_stop(-argc, argv));
}

/*** End of file ***/

