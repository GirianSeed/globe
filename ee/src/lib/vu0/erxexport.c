/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: erxexport.c,v 1.2 2003/04/17 12:04:52 xkazama Exp $
*/

#include <liberx.h>

extern SceErxLibraryHeader _sceVu0_entry;

void *sceVu0GetErxEntries(void)
{
    return (&_sceVu0_entry);
}
