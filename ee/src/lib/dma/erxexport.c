/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                      Emotion Engine Library
 *                          Version 1.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          erxexport.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            Apr,16,2003     hana        initial
 */

#include <liberx.h>
#include <libdma.h>

extern SceErxLibraryHeader _sceDma_entry;

void *sceDmaGetErxEntries(void)
{
	return (&_sceDma_entry);
}

