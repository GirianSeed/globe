/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                          Libnet Library
 *
 *      Copyright (C) 2000-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version   Date        Design   Log
 *  --------------------------------------------------------------------
 *       1.0.0     2003/01/22  ksh      first version
 */
 
#include <liberx.h>

extern SceErxLibraryHeader _sceLibnetif_entry;

void *sceLibnetifGetErxEntries(void)
{
	return(&_sceLibnetif_entry);
}

