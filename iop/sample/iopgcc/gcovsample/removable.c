/*                 -*- mode: c-mode; tab-width: 4; indent-tabs-mode: nil; -*-
 SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: removable.c,v 1.2 2003/03/25 11:45:47 tei Exp $
 */
/*
 * I/O Processor Library Sample Program
 *
 * Copyright (C) 2003 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * removable - removable.c
 *   coverage testing sample
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2003-02-03      isii        create
 */
#include <kernel.h>
#include <stdio.h>
#include <iopgcov.h>

#define BASE_priority  32

ModuleInfo Module = {"gcov_test_program_removable_resident", 0x0101 };

int thid;
int thread();
int module_start(int argc, char *argv[]);
int module_stop(int argc, char *argv[]);

int start(int argc, char *argv[])
{
    if ( argc >= 0 )    return module_start(argc,argv);
    else                return module_stop(-argc,argv);
}

int module_start(int argc, char *argv[])
{
    struct ThreadParam param;
    /* プログラム開始時に sceIopgcovDoGlobalCtors() を呼んでください。*/
    sceIopgcovDoGlobalCtors();

    param.attr         = TH_C;
    param.entry        = thread;
    param.initPriority = BASE_priority;
    param.stackSize    = 0x800;
    param.option       = 0;
    thid = CreateThread(&param);
    if ( thid > 0 && StartThread(thid,0) == KE_OK ) {
        /* 常駐終了する場合は、sceIopgcovDoGlobalDtors() を呼ばないでください。*/
        return REMOVABLE_RESIDENT_END;
    } else
        /* プログラム終了時に sceIopgcovDoGlobalDtors() を呼んでください。*/
        sceIopgcovDoGlobalDtors();
        return NO_RESIDENT_END;        
}

int module_stop(int argc, char *argv[])
{
    if ( TerminateThread(thid) == KE_OK && DeleteThread(thid) == KE_OK ) {
        /* プログラム終了時に sceIopgcovDoGlobalDtors() を呼んでください。*/
        sceIopgcovDoGlobalDtors();
        return NO_RESIDENT_END;
    } else {
        return REMOVABLE_RESIDENT_END;
    }
}

int thread()
{
    int i, l;

    l = 0;
    while (1) {
        for ( i = 0; i < 100; i++ ) {
            if ( i % 10 == 0 )
                printf("hoi\n");
            else
                printf("hei ");
        }
        printf("[%d]\n", ++l);
        DelayThread(5*1000*1000);
    }
}
