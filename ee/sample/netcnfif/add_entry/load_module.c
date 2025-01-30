/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *      Netcnf Interface Library Sample Program
 *
 *                         Version 1.4
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
 *       1.2       2002.01.31   tetsu    Add modules relation to libnet
 *       1.3       2002.03.13   tetsu    Add delay_thread()
 *       1.4       2003.04.30   ksh      remake
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
	while (sceSifLoadModule(INET, 0, NULL) < 0) {
		scePrintf("Can't load inet.irx\n");
    }
	while (sceSifLoadModule(INETCTL, sizeof(INETCTL_ARG), INETCTL_ARG) < 0) {
		scePrintf("Can't load inetctl.irx\n");
	}
	while (sceSifLoadModule(MSIFRPC, 0, NULL) < 0) {
		scePrintf("Can't load msifrpc.irx\n");
	}
	while (sceSifLoadModule(LIBNET, 0, NULL) < 0) {
		scePrintf("Can't load libnet.irx\n");
	}
	while (sceSifLoadModule(DEV9, 0, NULL) < 0) {
		scePrintf("Can't load dev9.irx\n");
	}
	while (sceSifLoadModule(SMAP, 0, NULL) < 0) {
		scePrintf("Can't load smap.irx\n");
	}
}

/*** End of file ***/

