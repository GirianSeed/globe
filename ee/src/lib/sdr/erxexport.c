/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: erxexport.c,v 1.5 2003/05/29 06:50:26 kaol Exp $
 */
/* 
 * Emotion Engine Library
 *
 * Copyright (C) 2003 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * libsdr - erxexport.c
 *     erx export funcitons
 */

#include <liberx.h>
#include <libsdr.h>

extern SceErxLibraryHeader _sceSdr_entry;

void *sceSdGetErxEntries(void)
{
    return(&_sceSdr_entry);
}
