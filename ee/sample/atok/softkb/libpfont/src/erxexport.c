/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
 $Id: erxexport.c,v 1.3 2003/11/14 07:55:04 xkazama Exp $
 *
 *     Copyright (C) 2003 Sony Computer Entertainment Inc.
 *                   All Rights Reserved.
 *
 */

#include <liberx.h>
#include <libpfont.h>

extern SceErxLibraryHeader _scePFont_entry;

void *scePFontGetErxEntries(void)
{
    return (&_scePFont_entry);
}
