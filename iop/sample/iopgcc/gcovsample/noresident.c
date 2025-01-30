/*                 -*- mode: c-mode; tab-width: 4; indent-tabs-mode: nil; -*-
 SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: noresident.c,v 1.2 2003/03/25 11:45:47 tei Exp $
 */
/*
 * I/O Processor Library Sample Program
 *
 * Copyright (C) 2003 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * noresiident - noresident.c
 *   coverage testing sample
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2003-02-03      isii        create
 */
#include <kernel.h>
#include <stdio.h>
#include <iopgcov.h>

ModuleInfo Module = {"gcov_test_program", 0x0101 };

int start(int argc, char *argv[])
{
    int i;
    /* プログラム開始時に sceIopgcovDoGlobalCtors() を呼んでください。*/
    sceIopgcovDoGlobalCtors();

    for ( i = 0; i < 100; i++ ) {
        if ( i % 10 == 0 )
            printf("bar\n");
        else
            printf("foo ");
    }
    printf(" end\n");

    /* プログラム終了時に sceIopgcovDoGlobalDtors() を呼んでください。*/
    sceIopgcovDoGlobalDtors();
    return NO_RESIDENT_END;
}
