/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0.2
 $Id: erxexport.c,v 1.3 2003/05/27 07:16:28 xkazama Exp $
*/

/*
 * High Graphics Library
 */

#include <liberx.h>

extern SceErxLibraryHeader _sceHip_entry;

void *sceHiPlugGetErxEntries(void)
{
    return &_sceHip_entry;
}
