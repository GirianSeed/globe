/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *      Copyright (C) 2000-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version   Date        Design   Log
 *  --------------------------------------------------------------------
 *       1.0.0     2003/01/22  ksh      first version
 */
 
#include <liberx.h>

extern SceErxLibraryHeader _sceNetGlue_entry;

void *sceNetGlueGetErxEntries(void)
{
	return(&_sceNetGlue_entry);
}

