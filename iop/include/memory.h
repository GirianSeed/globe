/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/*
 *                     I/O Processor Library 
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        memory.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.4.0
 */

#ifndef _MEMORY_H
#define _MEMORY_H

#ifndef _TYPES_H
#include <sys/types.h>
#endif

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif
/* To avoid conflicting */
extern void *memcpy (/*  unsigned char *, unsigned char *, int  */); 
extern void *memmove(unsigned char *, unsigned char *, int);
/* To avoid conflicting */
extern int   memcmp (/*  unsigned char *, unsigned char *, int  */);
extern void *memchr (unsigned char *, unsigned char, int);
extern void *memset (/* unsigned char *, unsigned char, int */);

extern void *bcopy(unsigned char *, unsigned char *, int); /* src,dest */
extern void *bzero(unsigned char *, int);
extern int   bcmp (unsigned char *, unsigned char *, int);
#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif  /* _MEMORY_H */





