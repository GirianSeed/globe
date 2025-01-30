/*                 -*- mode: c-mode; tab-width: 4; indent-tabs-mode: nil; -*-
 SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: iopgcov.h,v 1.1 2003/02/04 06:55:00 tei Exp $
 */
/*
 * I/O Processor Library
 *
 * Copyright (C) 2003 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * libiopgcov - iopgcov.h
 *   gcov support runtime for iop-gcc & irx
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2003-01-30      isii        create
 */

#ifndef _SCE_IOPGCOV_H
#define _SCE_IOPGCOV_H

#ifdef __GNUC__
/* GCC */

#if __GNUC__ > 3 || ( __GNUC__ == 3 && __GNUC_MINOR__ > 1 )
/* GCC 3.2 or later */
void _sceIopgcovDoGlobalCtors() __attribute__ ((weak));
void _sceIopgcovDoGlobalDtors() __attribute__ ((weak));
void _sceIopgcovOutputData() __attribute__ ((weak));
#else
/* old GCC */
__asm__( ".weak _sceIopgcovDoGlobalCtors");
void _sceIopgcovDoGlobalCtors();
__asm__( ".weak _sceIopgcovDoGlobalDtors");
void _sceIopgcovDoGlobalDtors();
__asm__( ".weak _sceIopgcovOutputData");
void _sceIopgcovOutputData();
#endif /* __GNUC__ > 3 || ( __GNUC__ == 3 && __GNUC_MINOR__ > 1 ) */

#define sceIopgcovOutputData() \
    do{if(_sceIopgcovOutputData!=NULL) _sceIopgcovOutputData();}while(0)

#define sceIopgcovDoGlobalCtors() \
    do{if(_sceIopgcovDoGlobalCtors!=NULL) _sceIopgcovDoGlobalCtors();}while(0)
#define sceIopgcovDoGlobalDtors() \
    do{if(_sceIopgcovDoGlobalDtors!=NULL) _sceIopgcovDoGlobalDtors();}while(0)

#else  /* __GNUC__ */

/* not GCC */
#define sceIopgcovOutputData()       do{}while(0)

#define sceIopgcovDoGlobalCtors()    do{}while(0)
#define sceIopgcovDoGlobalDtors()    do{}while(0)

#endif /* __GNUC__ */

#endif /* _SCE_IOPGCOV_H */
