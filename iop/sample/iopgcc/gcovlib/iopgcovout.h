/*                 -*- mode: c-mode; tab-width: 4; indent-tabs-mode: nil; -*-
 SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: iopgcovout.h,v 1.1 2003/02/04 06:55:00 tei Exp $
 */
/*
 * I/O Processor Library
 *
 * Copyright (C) 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * libiopgcov - iopgcovout.h
 *   common header for iopgcovout.c & iopgcovctl.c & iop_bb.c
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2002-12-26      isii        create
 */

/* GCC basic block struct from gcc source */
struct bb
{
    long zero_word;
    const char *filename;
    long *counts;
    long ncounts;
    struct bb *next;
    const unsigned long *addresses;
  
    long nwords;
    const char **functions;
    const long *line_nums;
    const char **filenames;
    char *flags;
};

#define GCOV_OUT_Module "iop_gcov_output"
#define GCOVMAGIC       "GCOV0001"

typedef void (*SceGcovFuncPtr) ();
typedef struct SceGcovLink {
    char               magic[8];
    ModuleInfo         *modinfo;
    struct SceGcovLink *next;
    struct SceGcovLink *prev;
    struct bb          *bb_head;
    SceGcovFuncPtr     outputfunc;
    SceGcovFuncPtr     terminatefunc;
} SceGcovLink;
