/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *      Inet Event Sample Program
 *
 *                         Version 1.1
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         load_module.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.02.18      tetsu       Change directory
 */

#include <stdio.h>
#include <eekernel.h>
#include <sifdev.h>
#include "load_module.h"

void load_module(void)
{
    while(sceSifLoadModule(SIO2MAN, 0, NULL) < 0){
	printf("Can't load sio2man.irx\n");
    }

    while(sceSifLoadModule(PADMAN, 0, NULL) < 0){
	printf("Can't load padman.irx\n");
    }

    while(sceSifLoadModule(NETCNF, sizeof(NETCNF_ARG), NETCNF_ARG) < 0){
	printf("Can't load netcnf.irx\n");
    }

    while(sceSifLoadModule(NETCNFIF, 0, NULL) < 0){
	printf("Can't load netcnfif.irx\n");
    }

    while(sceSifLoadModule(INET, 0, NULL) < 0){
	printf("Can't load inet.irx\n");
    }

    while(sceSifLoadModule(INETCTL, sizeof(INETCTL_ARG), INETCTL_ARG) < 0){
	printf("Can't load inetctl.irx\n");
    }

    while(sceSifLoadModule(PPP, 0, NULL) < 0){
	printf("Can't load ppp.irx\n");
    }

    while(sceSifLoadModule(USBD, 0, NULL) < 0){
	printf("Can't load usbd.irx\n");
    }

    while(sceSifLoadModule(USBMLOAD, sizeof(USBMLOAD_ARG), USBMLOAD_ARG) < 0){
	printf("Can't load usbmload.irx\n");
    }

    while(sceSifLoadModule(EVENT, 0, NULL) < 0){
	printf("Can't load event.irx\n");
    }

    while(sceSifLoadModule(DEV9, 0, NULL) < 0){
	printf("Can't load dev9.irx\n");
    }

    while(sceSifLoadModule(SMAP, 0, NULL) < 0){
	printf("Can't load smap.irx\n");
    }
}
