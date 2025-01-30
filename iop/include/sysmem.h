/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/*
 *                     I/O Processor Library 
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        sysmem.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.4.0
 */

#ifndef _SYSMEM_H
#define _SYSMEM_H

#ifndef NULL
#define NULL (void*)0
#endif

extern void *AllocSysMemory(int type, unsigned long size, void *addr);
extern int FreeSysMemory(void *area);
extern unsigned long QueryMemSize();
extern unsigned long QueryMaxFreeMemSize();
extern unsigned long QueryTotalFreeMemSize();
extern void *QueryBlockTopAddress(void *addr);
extern unsigned long QueryBlockSize(void *addr);

extern void *Kprintf(const char *format, ...);

#endif /* _SYSMEM_H */
