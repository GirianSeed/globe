/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                      Emotion Engine Library
 *                          Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                   <libeenet - erxexport.c>
 *                       <export for erx>
 *
 *       Version      Date        Design      Log
 *  --------------------------------------------------------------------
 *       1.0.0        2003/01/23  komaki      first version
 */

#include <liberx.h>

extern SceErxLibraryHeader _sceNetGlue_entry;

void *sceNetGlueGetErxEntries(void)
{
	return(&_sceNetGlue_entry);
}

