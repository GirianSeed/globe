/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/* 
 *                        libinsck Library
 *
 *      Copyright (C) 2000-2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version      Date        Design      Log
 *  --------------------------------------------------------------------
 *       1.0.0     2002/03/15     ksh         first
 */

/**********************************************************************/
#define	sceLibnetDisableAliases

#if 1
#define USB_ETHERNET
#endif
#if 0
#define USB_ETHERNET_WITH_PPPOE
#endif
#if 0
#define HDD_ETHERNET
#endif
#if 0
#define HDD_ETHERNET_WITH_PPPOE
#endif
#if 0
#define MODEM
#endif

#if 1
#define UP_INTERFACE_AUTO
#endif

#include <eekernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <libnet.h>
#include <libmrpc.h>

#if	( defined( HDD_ETHERNET ) || defined( HDD_ETHERNET_WITH_PPPOE ) )
#include "poweroff.h"
#endif

#define	SCETOP            "host0:/usr/local/sce/"
#define	MAIN_MOD_ROOT     SCETOP "iop/modules/"
#define	SETAPP_ROOT       SCETOP "conf/neticon/japanese/"
#define	IOP_MOD_INET      MAIN_MOD_ROOT "inet.irx"
#define	IOP_MOD_AN986     MAIN_MOD_ROOT "an986.irx"
#define	IOP_MOD_USBD      MAIN_MOD_ROOT "usbd.irx"
#define	IOP_MOD_NETCNF    MAIN_MOD_ROOT "netcnf.irx"
#define	IOP_MOD_INETCTL   MAIN_MOD_ROOT "inetctl.irx"
#define	IOP_MOD_MSIFRPC   MAIN_MOD_ROOT "msifrpc.irx"
#define	IOP_MOD_DEV9      MAIN_MOD_ROOT "dev9.irx"
#define	IOP_MOD_SMAP      MAIN_MOD_ROOT "smap.irx"
#define	IOP_MOD_PPP       MAIN_MOD_ROOT "ppp.irx"
#define	IOP_MOD_PPPOE     MAIN_MOD_ROOT "pppoe.irx"
#define	IOP_MOD_LIBNET    MAIN_MOD_ROOT "libnet.irx"
#define	IOP_MOD_INETLOG   SCETOP "iop/util/iet/inetlog.irx"
#define	IOP_MOD_MODEMDRV  ""
#define	NET_DB            SCETOP "conf/net/net.db"
#define	INET_ARG          "debug=18"
#define	INETCTL_ARG       "-no_auto" "\0" "-no_decode"
#define	LIBNET_ARG        "-verbose"
#define	NETCNF_ICON       SETAPP_ROOT "SYS_NET.ICO"
#define	NETCNF_ICONSYS    SETAPP_ROOT "icon.sys"
#define	NETCNF_ARG        "icon=" NETCNF_ICON "\0" "iconsys=" NETCNF_ICONSYS
#define	MODEMDRV_ARG      ""

#if	defined( USB_ETHERNET )
#define	USR_CONF_NAME     "Combination4"
#elif	defined( USB_ETHERNET_WITH_PPPOE )
#define	USR_CONF_NAME     "Combination5"
#elif	defined( HDD_ETHERNET )
#define	USR_CONF_NAME     "Combination6"
#elif	defined( HDD_ETHERNET_WITH_PPPOE )
#define	USR_CONF_NAME     "Combination7"
#elif	defined( MODEM )
#define	USR_CONF_NAME     ""
#endif


/**********************************************************************/
#include "single.h"

#define	PRIVATE	static
#define	PUBLIC
#define ERR_STOP	for ( ; ; )

#define	NETBUFSIZE	( 1024 )


PRIVATE	u_int	g_net_buf[ NETBUFSIZE ]	__attribute__( ( aligned( 64 ) ) );


PRIVATE void	LoadModule( const char* module, int args, const char* argp )
{
	while ( sceSifLoadModule( module, args, argp ) < 0 ) {
		scePrintf( "%s cannot be loaded.\n", module );
	}
}

int 	main( int argc, char* argv[] )
{
	sceSifMClientData	cd;
	sceInetAddress_t	myaddr;
	char	myname[ 128 ];
	char	myptrname[ 128 ];
	int 	if_id[ sceLIBNET_MAX_INTERFACE ];
	int 	i;
	int 	ret;
	int 	backlog;

	if ( argc != 2 ) {
		scePrintf( "usage: %s <backlog>\n", argv[ 0 ] );
		ERR_STOP;
	}
	backlog = atoi( argv[ 1 ] );


	/***************************************************************
		Preparation
	***************************************************************/
	sceSifInitRpc( 0 );

	LoadModule( IOP_MOD_INET, 0, NULL );
	LoadModule( IOP_MOD_NETCNF, sizeof( NETCNF_ARG ), NETCNF_ARG );
	LoadModule( IOP_MOD_INETCTL, sizeof( INETCTL_ARG ), INETCTL_ARG );

#if defined( USB_ETHERNET ) || defined( USB_ETHERNET_WITH_PPPOE )
	LoadModule( IOP_MOD_USBD, 0, NULL );
	LoadModule( IOP_MOD_AN986, 0, NULL );
#endif

#if defined( HDD_ETHERNET ) || defined( HDD_ETHERNET_WITH_PPPOE )
	LoadModule( IOP_MOD_DEV9, 0, NULL );
	LoadModule( IOP_MOD_SMAP, 0, NULL );
#endif

#if defined( USB_ETHERNET_WITH_PPPOE ) || defined( HDD_ETHERNET_WITH_PPPOE )
	LoadModule( IOP_MOD_PPP, 0, NULL );
	LoadModule( IOP_MOD_PPPOE, 0, NULL );
#endif

#if defined( MODEM )
	LoadModule( IOP_MOD_PPP, 0, NULL );
	LoadModule( IOP_MOD_USBD, 0, NULL );
	LoadModule( IOP_MOD_MODEMDRV, sizeof( MODEMDRV_ARG ), MODEMDRV_ARG );
#endif

	LoadModule( IOP_MOD_MSIFRPC, 0, NULL );
	LoadModule( IOP_MOD_LIBNET, sizeof( LIBNET_ARG ), LIBNET_ARG );

#if defined( HDD_ETHERNET ) || defined( HDD_ETHERNET_WITH_PPPOE )
	PreparePowerOff();
#endif


	/***************************************************************
		Initialize Libnet
	***************************************************************/
	sceSifMInitRpc( 0 );
	ret = sceLibnetInitialize( &cd, sizeof( g_net_buf ), sceLIBNET_STACKSIZE, sceLIBNET_PRIORITY );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetInitialize() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetRegisterHandler( &cd, g_net_buf );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetRegisterHandler() failed.(%d)\n", ret );
		ERR_STOP;
	}

#ifdef  UP_INTERFACE_AUTO
	scePrintf( "up interface auto\n" );
	ret = load_set_conf_extra( &cd, g_net_buf, NET_DB, USR_CONF_NAME, sceLIBNETF_AUTO_LOADMODULE | sceLIBNETF_AUTO_UPIF );
	if ( ret < 0 ) {
		scePrintf( "load_set_conf_extra() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetWaitGetAddress( &cd, g_net_buf, if_id, sceLIBNET_MAX_INTERFACE, &myaddr, sceLIBNETF_AUTO_UPIF );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetWaitGetAddress() failed.(%d)\n", ret );
		ERR_STOP;
	}

#else
	scePrintf( "up interface no auto\n" );
	ret = load_set_conf_extra( &cd, g_net_buf, NET_DB, USR_CONF_NAME, sceLIBNETF_AUTO_LOADMODULE );
	if ( ret < 0 ) {
		scePrintf( "load_set_conf_extra() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetWaitGetInterfaceID( &cd, g_net_buf, if_id, sceLIBNET_MAX_INTERFACE );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetWaitGetInterfaceID() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceInetCtlUpInterface( &cd, g_net_buf, 0 );
	if ( ret < 0 ) {
		scePrintf( "sceInetCtlUpInterface() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetWaitGetAddress( &cd, g_net_buf, if_id, sceLIBNET_MAX_INTERFACE, &myaddr, 0 );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetWaitGetAddress() failed.(%d)\n", ret );
		ERR_STOP;
	}

#endif  /*** UP_INTERFACE_AUTO ***/
	for ( i = 0; i < sceLIBNET_MAX_INTERFACE; i++ ) {
		if ( if_id[ i ] == 0 ) {
			break;
		}
		scePrintf( "interface: %d\n", if_id[ i ] );
	}


	/***************************************************************
		Display IP address
	***************************************************************/
	ret = sceInetAddress2String( &cd, g_net_buf, myptrname, 128, &myaddr );
	if ( ret < 0 ) {
		scePrintf( "sceInetAddress2String() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceInetAddress2Name( &cd, g_net_buf, 0, myname, 128, &myaddr, 0, 0 );
	if ( ret < 0 ) {
		scePrintf( "My address: %s\n", myptrname );
	}
	else {
		scePrintf( "My address: %s (%s)\n", myname, myptrname );
	}


	/***************************************************************
		Main
	***************************************************************/
	do_tcp_echo_server( backlog );


	/***************************************************************
		Terminate Libnet
	***************************************************************/
	ret = sceInetCtlDownInterface( &cd, g_net_buf, 0 );
	if ( ret < 0 ) {
		scePrintf( "sceInetCtlDownInterface() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetUnregisterHandler( &cd, g_net_buf );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetUnregisterHandler() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetTerminate( &cd );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetTerminate() failed.(%d)\n", ret );
		ERR_STOP;
	}

	return( 0 );
}

/*** End of file ***/

