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
 *                         load_module.h
 *
 *       Version   Date         Design   Log
 *  --------------------------------------------------------------------
 *       1.1       2002.01.28   tetsu    First version
 *       1.2       2003.04.30   ksh      SCETOP added
 */

#ifndef __load_module_h_
#define __load_module_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SCETOP   "host0:/usr/local/sce/"
#define MODULE   SCETOP "iop/modules/"
#define SIO2MAN  MODULE "sio2man.irx"
#define MCMAN    MODULE "mcman.irx"
#define MCSERV   MODULE "mcserv.irx"
#define NETCNF   MODULE "netcnf.irx"
#define NETCNFIF MODULE "netcnfif.irx"
#define INET     MODULE "inet.irx"
#define INETCTL  MODULE "inetctl.irx"
#define PPP      MODULE "ppp.irx"
#define PPPOE    MODULE "pppoe.irx"
#define MSIFRPC  MODULE "msifrpc.irx"
#define LIBNET   MODULE "libnet.irx"
#define DEV9     MODULE "dev9.irx"
#define SMAP     MODULE "smap.irx"

#define IOPRP    SCETOP "iop/modules/" IOP_IMAGE_file

#define NETCNF_ARG   "-no_check_capacity" "\0" "icon=" SCETOP "conf/neticon/japanese/SYS_NET.ICO" "\0" "iconsys=" SCETOP "conf/neticon/japanese/icon.sys"
#define NETCNFIF_ARG "thpri=123" "\0" "thstack=8KB"
#define INETCTL_ARG  "-no_auto"

void load_module(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __load_module_h_ */
