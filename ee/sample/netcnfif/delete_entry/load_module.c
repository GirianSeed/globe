/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *      Netcnf Interface Library Sample Program
 *
 *                         Version 1.2
 *                         Shift-JIS
 *
 *      Copyright (C) 2002-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         load_module.c
 *
 *       Version   Date         Design   Log
 *  --------------------------------------------------------------------
 *       1.1       2002.01.28   tetsu    First version
 *       1.2       2003.04.30   ksh      remake
 */

#include <stdio.h>
#include <eekernel.h>
#include <sifdev.h>
#include "load_module.h"

void load_module(void)
{
    while (sceSifLoadModule(SIO2MAN, 0, NULL) < 0) {
		scePrintf("Can't load sio2man.irx\n");
	}
	while (sceSifLoadModule(MCMAN, 0, NULL) < 0) {
		scePrintf("Can't load mcman.irx\n");
	}
	while (sceSifLoadModule(MCSERV, 0, NULL) < 0) {
		scePrintf("Can't load mcserv.irx\n");
	}
	while (sceSifLoadModule(NETCNF, sizeof(NETCNF_ARG), NETCNF_ARG) < 0) {
		scePrintf("Can't load netcnf.irx\n");
	}
	while (sceSifLoadModule(NETCNFIF, sizeof(NETCNFIF_ARG), NETCNFIF_ARG) < 0) {
		scePrintf("Can't load netcnfif.irx\n");
	}
}

/*** End of file ***/

