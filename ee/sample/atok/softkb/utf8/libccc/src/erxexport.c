/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library libccc Version 1.1
 */
/* 
 *                      Emotion Engine Library
 *                          Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          erxexport.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           Jan,29,2003     hana        initial
 */

#include <liberx.h>
#include "libccc.h"

extern SceErxLibraryHeader _sceCcc_entry;

void *sceCccGetErxEntries(void)
{
	return (&_sceCcc_entry);
}

