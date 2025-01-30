/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: load_module.c,v 1.2 2002/10/20 13:51:31 tetsu Exp $
 */
/* 
 * Emotion Engine Library Sample Program
 *
 * Copyright (C) 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * bnnetcnf/load_entry - load_module.c
 *	"PlayStation BB Navigator" Network Configuration Read Library Sample Program
 *	load IRX module program
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2002-10-12      tetsu       first version
 *      2002-10-20      tetsu       correspond to code-review
 */

#include <stdio.h>
#include <eekernel.h>
#include <sifdev.h>

#include "poweroff.h"

#define SIO2MAN  "host0:/usr/local/sce/iop/modules/sio2man.irx"
#define NETCNF   "host0:/usr/local/sce/iop/modules/netcnf.irx"
#define NETCNFIF "host0:/usr/local/sce/iop/modules/netcnfif.irx"
#define INET     "host0:/usr/local/sce/iop/modules/inet.irx"
#define INETCTL  "host0:/usr/local/sce/iop/modules/inetctl.irx"
#define PPP      "host0:/usr/local/sce/iop/modules/ppp.irx"
#define PPPOE    "host0:/usr/local/sce/iop/modules/pppoe.irx"
#define MSIFRPC  "host0:/usr/local/sce/iop/modules/msifrpc.irx"
#define LIBNET   "host0:/usr/local/sce/iop/modules/libnet.irx"
#define DEV9     "host0:/usr/local/sce/iop/modules/dev9.irx"
#define ATAD     "host0:/usr/local/sce/iop/modules/atad.irx"
#define HDD      "host0:/usr/local/sce/iop/modules/hdd.irx"
#define PFS      "host0:/usr/local/sce/iop/modules/pfs.irx"
#define SMAP     "host0:/usr/local/sce/iop/modules/smap.irx"

void load_module(void)
{
    static char netcnf_arg[] = "-no_check_capacity\0icon=host0:/usr/local/sce/conf/neticon/japanese/SYS_NET.ICO\0iconsys=host0:/usr/local/sce/conf/neticon/japanese/icon.sys";
    static char netcnfif_arg[] = "thpri=123\0thstack=8KB";
    static char inetctl_arg[] = "-no_auto";
    static char hddarg[] = "-o" "\0" "8" "\0" "-n" "\0"	"32" "\0";
    static char pfsarg[] = "-m" "\0" "8" "\0" "-o" "\0" "10" "\0" "-n" "\0" "64" "\0";
    int         r;

    while ((r = sceSifLoadModule(SIO2MAN, 0, NULL)) < 0) {
	scePrintf("Can't load sio2man.irx (%d)\n", r);
    }

    while ((r = sceSifLoadModule(NETCNF, sizeof(netcnf_arg), netcnf_arg)) < 0) {
	scePrintf("Can't load netcnf.irx (%d)\n", r);
    }

    while ((r = sceSifLoadModule(NETCNFIF, sizeof(netcnfif_arg), netcnfif_arg)) < 0) {
	scePrintf("Can't load netcnfif.irx (%d)\n", r);
    }

    while ((r = sceSifLoadModule(INET, 0, NULL)) < 0) {
	scePrintf("Can't load inet.irx (%d)\n", r);
    }

    while ((r = sceSifLoadModule(INETCTL, sizeof(inetctl_arg), inetctl_arg)) < 0) {
	scePrintf("Can't load inetctl.irx (%d)\n", r);
    }

    while ((r = sceSifLoadModule(PPP, 0, NULL)) < 0) {
	scePrintf("Can't load ppp.irx (%d)\n", r);
    }

    while ((r = sceSifLoadModule(PPPOE, 0, NULL)) < 0) {
	scePrintf("Can't load pppoe.irx (%d)\n", r);
    }

    while ((r = sceSifLoadModule(MSIFRPC, 0, NULL)) < 0) {
	scePrintf("Can't load msifrpc.irx (%d)\n", r);
    }

    while ((r = sceSifLoadModule(LIBNET, 0, NULL)) < 0) {
	scePrintf("Can't load libnet.irx (%d)\n", r);
    }

    while ((r = sceSifLoadModule(DEV9, 0, NULL)) < 0) {
	scePrintf("Can't load dev9.irx (%d)\n", r);
    }

    /* dev9 ‚ªí’“‚µ‚Ä‚à‚µ‚È‚­‚Ä‚à dev9.irx ‚ð sceSifLoadModule() ‚·‚é‚Æ‚¢‚¤
       sˆ×‚ð‚µ‚½’iŠK‚Å poweroff ˆ—‚Ì€”õ‚Í‚·‚é */
    prepare_poweroff();

    while ((r = sceSifLoadModule(ATAD, 0, NULL)) < 0) {
	scePrintf("Can't load dev9.irx (%d)\n", r);
    }

    while ((r = sceSifLoadModule(HDD, sizeof(hddarg), hddarg)) < 0) {
	scePrintf("Can't load hdd.irx (%d)\n", r);
    }

    while ((r = sceSifLoadModule(PFS, sizeof(pfsarg), pfsarg)) < 0) {
	scePrintf("Can't load pfs.irx (%d)\n", r);
    }

    while ((r = sceSifLoadModule(SMAP, 0, NULL)) < 0) {
	scePrintf("Can't load smap.irx (%d)\n", r);
    }
}
