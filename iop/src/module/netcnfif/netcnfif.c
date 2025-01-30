/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/* 
 *      Netcnf Interface Library
 *
 *                         Version 1.1
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         netcnfif.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel.h>

#include "interface.h"

ModuleInfo Module = { "Netcnf_Interface", 0x0101 };

extern libhead netcnfif_entry; /* ライブラリ名_entry を参照 */

static int th_id;
static int oldstat;

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
    struct ThreadParam th_param;
    int r = 0;
    char *bp;
    int thpri = USER_LOWEST_PRIORITY;
    int thstack = 0x2000; /* 8 KB */

    /* 引数を確認 */
    for(--argc, ++argv; 0 < argc; --argc, ++argv){
	if(!strncmp("thpri=", argv[0], 6)){
	    bp = argv[0] + 6;
	    if(!isdigit(*bp))
		goto PRINT_USAGE;
	    thpri = atoi(bp);
	    if(thpri < USER_HIGHEST_PRIORITY
	       || USER_LOWEST_PRIORITY < thpri)
		goto PRINT_USAGE;
	    for( ; *bp != '\0' && isdigit(*bp); bp++)
		;
	    if(*bp != '\0')
		goto PRINT_USAGE;
	}else if(!strncmp("thstack=", argv[0], 8)){
	    bp = argv[0] + 8;
	    if(!isdigit(*bp))
		goto PRINT_USAGE;
	    thstack = atoi(bp);
	    for( ; *bp != '\0' && isdigit(*bp); bp++)
		;
	    if(!strcmp(bp, "KB"))
		thstack <<= 10;
	    else if(*bp != '\0')
		goto PRINT_USAGE;
	}else if(!strncmp("-help", argv[0], 5)){
		goto PRINT_USAGE;
	}else{
    PRINT_USAGE:
	    usage();
	    return(NO_RESIDENT_END);
	}
    }

    /* 常駐ライブラリのエントリテーブル登録
       (2度目にモジュールが読み込まれた場合に再びスレッドを作成しないために
       スレッドを作成する前に行う) */
    CpuSuspendIntr(&oldstat);
    r = RegisterLibraryEntries(&netcnfif_entry);
    CpuResumeIntr(oldstat);
    if(r != KE_OK) return (NO_RESIDENT_END);

    /* netcnf.irx のサービスを提供するスレッドを作成 */
    th_param.attr         = TH_C;
    th_param.entry        = sceNetcnfifInterfaceStart;
    th_param.initPriority = thpri;
    th_param.stackSize    = thstack;
    th_param.option       = 0;

    th_id = CreateThread(&th_param);

    /* スレッドをスタート */
    if(th_id > 0){
	StartThread(th_id, 0);
	return (REMOVABLE_RESIDENT_END);
    }else{
	CpuSuspendIntr(&oldstat);
	ReleaseLibraryEntries(&netcnfif_entry);
	CpuResumeIntr(oldstat);
	return (NO_RESIDENT_END);
    }
}

static int module_stop(int argc, char *argv[])
{
    /* Sifrpc サービスストップ */
    sceNetcnfifInterfaceStop();

    /* netcnf.irx のサービスを提供するスレッドを強制終了・削除 */
    TerminateThread(th_id);
    DeleteThread(th_id);

    /* エントリテーブルの登録抹消 */
    CpuSuspendIntr(&oldstat);
    ReleaseLibraryEntries(&netcnfif_entry);
    CpuResumeIntr(oldstat);

    return (NO_RESIDENT_END);
}

int start(int argc, char *argv[])
{
    if(argc >= 0){
	return module_start(argc, argv);
    }else{
	return module_stop(-argc, argv);
    }
}
