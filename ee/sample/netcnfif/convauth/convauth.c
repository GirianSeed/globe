/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/* 
 *      Netcnf Interface Library Sample Program
 *
 *                         Version 1.1
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         convauth.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.05.05      tetsu       First version
 */

#include <eekernel.h>
#include <netcnfif.h>

int main(int ac, char *av[])
{
    char *s, buf[256], tmp[256];
    int i, r;

    if(ac == 2){
	/* copy string inputted */
	for(s = av[1], i = 0; s[i] != '\0' && i < 255; i++){
	    buf[i] = s[i];
	}
	buf[i] = '\0';

	/* convert to display */
	if((r = sceNetcnfifConvAuthname(buf, tmp, sizeof(tmp))) < 0){
	    scePrintf("\nsceNetcnfifConvAuthname error (%d)\n\n", r);
	}else{
	    scePrintf("\n\"%s\" -> \"%s\"\n\n", buf, tmp);
	}
    }else{
	scePrintf("\n[usage] %s <string>\n\n", av[0]);
    }

    return(0);
}
