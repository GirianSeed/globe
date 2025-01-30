/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/* 
 *      Netcnf Interface Library Sample Program
 *
 *                         Version 1.3
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         load_module.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 *       1.2            2002.01.31      tetsu       Add modules relation to libnet
 *       1.3            2002.03.13      tetsu       Add delay_thread()
 */

#include <stdio.h>
#include <eekernel.h>
#include <sifdev.h>

#include "delay_thread.h"

#define SIO2MAN  "host0:../../../../iop/modules/sio2man.irx"
#define MCMAN    "host0:../../../../iop/modules/mcman.irx"
#define MCSERV   "host0:../../../../iop/modules/mcserv.irx"
#define NETCNF   "host0:../../../../iop/modules/netcnf.irx"
#define NETCNFIF "host0:../../../../iop/modules/netcnfif.irx"
#define INET     "host0:../../../../iop/modules/inet.irx"
#define INETCTL  "host0:../../../../iop/modules/inetctl.irx"
#define MSIFRPC  "host0:../../../../iop/modules/msifrpc.irx"
#define LIBNET   "host0:../../../../iop/modules/libnet.irx"
#define USBD     "host0:../../../../iop/modules/usbd.irx"
#define AN986    "host0:../../../../iop/modules/an986.irx"

void load_module(void)
{
    char netcnf_arg[] = "-no_check_capacity\0icon=host0:../../../../conf/neticon/japanese/SYS_NET.ICO\0iconsys=host0:../../../../conf/neticon/japanese/icon.sys";
    char netcnfif_arg[] = "thpri=123\0thstack=8KB";
    char inetctl_arg[] = "-no_auto";

    while(sceSifLoadModule(SIO2MAN, 0, NULL) < 0){
	scePrintf("Can't load sio2man.irx\n");
    }

    while(sceSifLoadModule(MCMAN, 0, NULL) < 0){
	scePrintf("Can't load mcman.irx\n");
    }

    while(sceSifLoadModule(MCSERV, 0, NULL) < 0){
	scePrintf("Can't load mcserv.irx\n");
    }

    while(sceSifLoadModule(NETCNF, sizeof(netcnf_arg), netcnf_arg) < 0){
	scePrintf("Can't load netcnf.irx\n");
    }

    while(sceSifLoadModule(NETCNFIF, sizeof(netcnfif_arg), netcnfif_arg) < 0){
	scePrintf("Can't load netcnfif.irx\n");
    }

    while(sceSifLoadModule(INET, 0, NULL) < 0){
	scePrintf("Can't load inet.irx\n");
    }

    while(sceSifLoadModule(INETCTL, sizeof(inetctl_arg), inetctl_arg) < 0){
	scePrintf("Can't load inetctl.irx\n");
    }

    while(sceSifLoadModule(MSIFRPC, 0, NULL) < 0){
	scePrintf("Can't load msifrpc.irx\n");
    }

    while(sceSifLoadModule(LIBNET, 0, NULL) < 0){
	scePrintf("Can't load libnet.irx\n");
    }

    while(sceSifLoadModule(USBD, 0, NULL) < 0){
	scePrintf("Can't load usbd.irx\n");
    }

    while(sceSifLoadModule(AN986, 0, NULL) < 0){
	scePrintf("Can't load an986.irx\n");
    }

    /* USB ドライバをロードした場合、USB ドライバが正常動作する状態になるまで
       時間がかかるため 1 秒程度待つ */
    delay_thread(263 * 60); /* 263HSync == 1VSync */
}
