/*                 -*- mode: c-mode; tab-width: 4; indent-tabs-mode: nil; -*-
 SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: iop_bb.c,v 1.2 2003/03/25 11:45:47 tei Exp $
 */
/*
 * I/O Processor Library 
 *
 * Copyright (C) 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * libigcov - iop_bb.c
 *    gcov support runtime for iop-gcc & irx
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2002-01-25      isii        create
 */

#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include "iopgcovout.h"

extern ModuleInfo Module;

static SceGcovLink sGcovLink = {
    GCOVMAGIC, &Module,    NULL, NULL,   NULL,   NULL, NULL
};

/* gcc like constructor listing routine */
void __bb_init_func (struct bb *blocks)
{
    if (blocks->zero_word)
        return;

    /* Set up linked list.  */
    printf("Add GCOV output list \"%s\"\n", blocks->filename);
    blocks->zero_word = 1;
    blocks->next = sGcovLink.bb_head;
    sGcovLink.bb_head = blocks;
}

/* gcc like constructors call routine */
static void __do_global_ctors ()
{
    extern SceGcovFuncPtr _begin_of_section_ctors[], _end_of_section_ctors[];
    SceGcovFuncPtr *p;
    for ( p = _begin_of_section_ctors; p < _end_of_section_ctors; p++ ) {
        (*(p)) ();
    }
}

/* gcc like destructor call routine */
static void __do_global_dtors ()
{
    extern SceGcovFuncPtr _begin_of_section_dtors[], _end_of_section_dtors[];
    SceGcovFuncPtr *p;
    for ( p = _begin_of_section_dtors; p < _end_of_section_dtors; p++ ) {
        (*(p)) ();
    }
}

/* gcov用のデータをファイルに出力する。*/
void _sceIopgcovOutputData()
{
    int oldei;
    SceGcovFuncPtr outputfunc;

    CpuSuspendIntr(&oldei);
    outputfunc = sGcovLink.outputfunc;
    if ( outputfunc != NULL )
        outputfunc(&sGcovLink, oldei);
    else
        CpuResumeIntr(oldei);
}

/* gcov用のデータをファイルに出力し、以後の出力を禁止する。*/
static void FinalOutputData()
{
    int oldei;
    SceGcovFuncPtr terminatefunc;

    CpuSuspendIntr(&oldei);
    terminatefunc = sGcovLink.terminatefunc;
    if ( terminatefunc != NULL )
        terminatefunc(&sGcovLink, oldei);
    else
        CpuResumeIntr(oldei);
}

/* メモリ上に bb構造体のリストを作りgcov用のデータが出力できるように準備する*/
void _sceIopgcovDoGlobalCtors()
{
    static int initialized;
    int  idlist[2], idcount, oldei;
    SceGcovFuncPtr funcp;
    ModuleStatus   modstat;
    
    if ( !initialized ) {
        initialized = 1;
        __do_global_ctors();
        CpuSuspendIntr(&oldei);
        GetModuleIdListByName(GCOV_OUT_Module, idlist, 2, &idcount);
        if ( idcount==1 && ReferModuleStatus(idlist[0], &modstat) == KE_OK) {
            funcp = (SceGcovFuncPtr)modstat.text_addr;
            funcp(&sGcovLink, oldei);
        } else
            CpuResumeIntr(oldei);
    }
}

/* デストラクタをすべて呼び出し、その後 gcov用のデータをファイルに出力 */
void _sceIopgcovDoGlobalDtors()
{
    static int terminated;
    if ( !terminated ) {
        terminated = 1;
        __do_global_dtors ();
        FinalOutputData();
    }
}
