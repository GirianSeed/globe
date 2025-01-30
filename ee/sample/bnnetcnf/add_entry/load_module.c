/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: load_module.c,v 1.2 2002/10/20 13:49:27 tetsu Exp $
 */
/* 
 * Emotion Engine Library Sample Program
 *
 * Copyright (C) 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * bnnetcnf/add_entry - load_module.c
 *	"PlayStation BB Navigator" Network Configuration Read Library Sample Program
 *      load IRX module program
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
#define MCMAN    "host0:/usr/local/sce/iop/modules/mcman.irx"
#define MCSERV   "host0:/usr/local/sce/iop/modules/mcserv.irx"
#define NETCNF   "host0:/usr/local/sce/iop/modules/netcnf.irx"
#define NETCNFIF "host0:/usr/local/sce/iop/modules/netcnfif.irx"
#define DEV9     "host0:/usr/local/sce/iop/modules/dev9.irx"
#define ATAD     "host0:/usr/local/sce/iop/modules/atad.irx"
#define HDD      "host0:/usr/local/sce/iop/modules/hdd.irx"
#define PFS      "host0:/usr/local/sce/iop/modules/pfs.irx"

void load_module(void)
{
    static char netcnf_arg[] = "-no_check_capacity\0icon=host0:/usr/local/sce/conf/neticon/japanese/SYS_NET.ICO\0iconsys=host0:/usr/local/sce/conf/neticon/japanese/icon.sys";
    static char netcnfif_arg[] = "thpri=123\0thstack=8KB";
    static char hddarg[] = "-o" "\0" "8" "\0" "-n" "\0"	"32" "\0";
    static char pfsarg[] = "-m" "\0" "8" "\0" "-o" "\0" "10" "\0" "-n" "\0" "64" "\0";
    int         r;

    while ((r = sceSifLoadModule(SIO2MAN, 0, NULL)) < 0) {
	scePrintf("Can't load sio2man.irx (%d)\n", r);
    }

    while ((r = sceSifLoadModule(MCMAN, 0, NULL)) < 0) {
	scePrintf("Can't load mcman.irx (%d)\n", r);
    }

    while ((r = sceSifLoadModule(MCSERV, 0, NULL)) < 0) {
	scePrintf("Can't load mcserv.irx (%d)\n", r);
    }

    while ((r = sceSifLoadModule(NETCNF, sizeof(netcnf_arg), netcnf_arg)) < 0) {
	scePrintf("Can't load netcnf.irx (%d)\n", r);
    }

    while ((r = sceSifLoadModule(NETCNFIF, sizeof(netcnfif_arg), netcnfif_arg)) < 0) {
	scePrintf("Can't load netcnfif.irx (%d)\n", r);
    }

    while ((r = sceSifLoadModule(DEV9, 0, NULL)) < 0) {
	scePrintf("Can't load dev9.irx (%d)\n", r);
    }

    /* dev9 ‚ªí’“‚µ‚Ä‚à‚µ‚È‚­‚Ä‚à dev9.irx ‚ð sceSifLoadModule() ‚·‚é‚Æ‚¢‚¤
       sˆ×‚ð‚µ‚½’iŠK‚Å poweroff ˆ—‚Ì€”õ‚Í‚·‚é */
    prepare_poweroff();

    while ((r = sceSifLoadModule(ATAD, 0, NULL)) < 0) {
	scePrintf("Can't load atad.irx (%d)\n", r);
    }

    while ((r = sceSifLoadModule(HDD, sizeof(hddarg), hddarg)) < 0) {
	scePrintf("Can't load hdd.irx (%d)\n", r);
    }

    while ((r = sceSifLoadModule(PFS, sizeof(pfsarg), pfsarg)) < 0) {
	scePrintf("Can't load pfs.irx (%d)\n", r);
    }
}
