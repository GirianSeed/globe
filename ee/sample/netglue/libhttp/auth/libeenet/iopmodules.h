/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                         - http_auth -
 *                        Version 1.0.0
 *
 *      Copyright (C) 2001-2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        <iopmodules.h>
 *             <definitions about config and iop modules>
 *
 *       Version   Date           Design              Log
 *  --------------------------------------------------------------------
 *       1.0.0     Sep,19,2002    komaki              first version
 */

/* config name */
#if defined(USB_ETHERNET)
#define USR_CONF_NAME     "Combination4"
#elif defined(USB_ETHERNET_WITH_PPPOE)
#define USR_CONF_NAME     "Combination5"
#elif defined(HDD_ETHERNET)
#define USR_CONF_NAME     "Combination6"
#elif defined(HDD_ETHERNET_WITH_PPPOE)
#define USR_CONF_NAME     "Combination7"
#elif defined(MODEM)
#define USR_CONF_NAME     ""
#endif

/*  --------- DEFINE FOR IOP MODULES -------- */

#define IOP_MAIN_ROOT	"host0:/usr/local/sce/iop/modules/"
#define SETAPP_ROOT	"host0:/usr/local/sce/ee/sample/inet/setapp/"
#define IOP_MOD_DEV9	IOP_MAIN_ROOT "dev9.irx"
#define IOP_MOD_USBD	IOP_MAIN_ROOT "usbd.irx"
#define IOP_MOD_NETCNF	IOP_MAIN_ROOT "netcnf.irx"
#define NETCNF_ICON	SETAPP_ROOT "SYS_NET.ICO"
#define NETCNF_ICONSYS	SETAPP_ROOT "icon.sys"
#define NETCNF_ARG	"icon=" NETCNF_ICON "\0" "iconsys=" NETCNF_ICONSYS
#define NET_DB		"host0:/usr/local/sce/conf/net/net.db"

#define EENET_IOP_TOP		"host0:/usr/local/sce/iop/"
#define EENET_IOP_MODULES	EENET_IOP_TOP "modules/"
#define IOP_MOD_EENETCTL	EENET_IOP_MODULES "eenetctl.irx"
#define IOP_MOD_ENT_SMAP	EENET_IOP_MODULES "ent_smap.irx"
#define IOP_MOD_ENT_ETH		EENET_IOP_MODULES "ent_eth.irx"
#define IOP_MOD_ENT_PPP 	EENET_IOP_MODULES "ent_ppp.irx"
#define IOP_MOD_ENT_DEVM	EENET_IOP_MODULES "ent_devm.irx"
#define IOP_MOD_ENT_CNF		EENET_IOP_TOP "sample/libeenet/ent_cnf/ent_cnf.irx"
#define IOP_MOD_MODEMDRV	""
#define MODEMDRV_ARG		""
