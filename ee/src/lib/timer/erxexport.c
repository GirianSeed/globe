/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                       Timer Library
 *                        Version 1.0
 *                         Shift-JIS
 *
 *      Copyright (C) 2001-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         erxexport.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.0            2003/05/15      kono        First version
 */


#include <liberx.h>
#include "libtimer.h"

extern SceErxLibraryHeader _sceTimer_entry;

void *sceTimerGetErxEntries(void)
{
	return (&_sceTimer_entry);
}

