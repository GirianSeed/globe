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
 *                         load_module.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.02.18      tetsu       Change directory
 */

#ifndef __load_module_h_
#define __load_module_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SCETOP   "host0:/usr/local/sce/"
#define MODULES  SCETOP "iop/modules/"
#define SIO2MAN  MODULES "sio2man.irx"
#define PADMAN   MODULES "padman.irx"
#define NETCNF   MODULES "netcnf.irx"
#define NETCNFIF MODULES "netcnfif.irx"
#define INET     MODULES "inet.irx"
#define INETCTL  MODULES "inetctl.irx"
#define PPP      MODULES "ppp.irx"
#define USBD     MODULES "usbd.irx"
#define USBMLOAD MODULES "usbmload.irx"
#define EVENT    SCETOP "iop/sample/inet/event/event.irx"
#define DEV9     MODULES "dev9.irx"
#define SMAP     MODULES "smap.irx"

#define NETCNF_ARG   "icon=" SCETOP "conf/neticon/japanese/SYS_NET.ICO" "\0" "iconsys=" SCETOP "conf/neticon/japanese/icon.sys"
#define USBMLOAD_ARG "conffile=" SCETOP "conf/usb/usbdrvho.cnf"
#define INETCTL_ARG  "-no_auto" "\0" "-verbose"

void load_module(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__load_module_h_ */
