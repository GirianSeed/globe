/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                          Libnet Library
 *
 *      Copyright (C) 2000-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version   Date        Design      Log
 *  --------------------------------------------------------------------
 *       1.1.0     2002/01/24  ksh         remake
 *       1.2.0     2003/04/02  ksh         USB to HDD Ethernet etc.
 *       1.3.0     2003/04/15  ksh         char net_buf[]
 */

/**********************************************************************/
#define	sceLibnetDisableAliases

#if 0
#define USB_ETHERNET
#endif
#if 0
#define USB_ETHERNET_WITH_PPPOE
#endif
#if 1
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
#define	IOP_MOD_NETCNFIF  MAIN_MOD_ROOT "netcnfif.irx"
#define IOP_MOD_INETLOG   SCETOP "iop/util/inet/inetlog.irx"
#define	IOP_MOD_MODEMDRV  ""
#define	NET_DB            SCETOP "conf/net/net.db"
#define INET_ARG          "debug=18"
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
#include "main.h"
#include "comm.h"

#define NETBUFSIZE	( 2048 )
#define MAX_CONNECTION	( 200 )
#define STACKSIZE	( 0x4000 )
#define PRIO	( 63 )

PRIVATE	int 	g_th_num;
PUBLIC	int 	g_passive_sema;
PUBLIC	int 	g_fin_sema;
PUBLIC	CONNECTIONDATA	g_connection[ MAX_CONNECTION ];


PRIVATE int 	LoadModule( const char *module, int args, const char *argp )
{
	int 	id;
	
	while ( ( id = sceSifLoadModule( module, args, argp ) ) < 0 ) {
		scePrintf( "%s cannot be loaded.\n", module );
	}
	
	return( id );
}

PRIVATE	void	CleanUpThread( void *data )
{
	int 	i;
	int 	ret;

	for ( ; ; ) {
		WaitSema( g_fin_sema );
		for ( i = 0; i < MAX_CONNECTION; i++ ) {
			if ( g_connection[ i ].fin ) {
				TerminateThread( g_connection[ i ].tid );
				ret = DeleteThread( g_connection[ i ].tid );
				if ( ret < 0 ) {
					continue;
				}
				free( g_connection[ i ].stack );
				g_connection[ i ].exist = 0;
				g_connection[ i ].fin = 0;
			}
		}
	}
}

PRIVATE	int 	SearchSID( void )
{
	int 	i;

	for ( i = 0; i < MAX_CONNECTION; i++ ) {
		if ( g_connection[ i ].exist == 0 ) {
			return( i );
		}
	}

	return( -1 );
}

PRIVATE	int 	CreateStartThread( void *func, char *stackp, int size, int prio, void *argp )
{
	struct ThreadParam	tp;
	int 	tid;
	int 	ret;

	tp.entry = func;
	tp.stack = stackp;
	tp.stackSize = size;
	tp.initPriority = prio;
	tp.gpReg = &_gp;

	tid = CreateThread( &tp );
	if ( tid <= 0 ) {
		scePrintf( "CreateThread() failed.(%d)\n", tid );
		return( -1 );
	}

	ret = StartThread( tid, argp );
	if ( ret < 0 ) {
		scePrintf( "StartThread() failed.(%d)\n", ret );
		DeleteThread( tid );
		return( -1 );
	}

	return( tid );
}

int 	main( int argc, char *argv[] )
{
	static u_char	net_buf[ NETBUFSIZE ] __attribute__( ( aligned( 64 ) ) );
	sceSifMClientData	cd;
	PCONNECTIONDATA	p_connection;
	struct SemaParam	semap;
	sceInetAddress_t	myaddr;
	char	myname[ 128 ];
	char	myptrname[ 128 ];
	char	*clean_up_stack;
	int 	if_id[ sceLIBNET_MAX_INTERFACE ];
	int 	tid;
	int 	sid;
	int 	i;
	int 	ret;

	if ( argc != 2 ) {
		scePrintf( "usage: %s <th_num>\n", argv[ 0 ] );
		return( -1 );
	}

	g_th_num = atoi( argv[ 1 ] );
	if ( g_th_num <= 0 ) {
		scePrintf( "g_th_num = %d\n", g_th_num );
		return( -1 );
	}


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
	LoadModule( IOP_MOD_NETCNFIF, 0, NULL );

#if defined( HDD_ETHERNET ) || defined( HDD_ETHERNET_WITH_PPPOE )
	PreparePowerOff();
#endif


	/***************************************************************
		Initialize Libnet
	***************************************************************/
	sceSifMInitRpc( 0 );
	ret = sceLibnetInitialize( &cd, sizeof( net_buf ), sceLIBNET_STACKSIZE, sceLIBNET_PRIORITY );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetInitialize() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetRegisterHandler( &cd, net_buf );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetRegisterHandler() failed.(%d)\n", ret );
		ERR_STOP;
	}

#ifdef  UP_INTERFACE_AUTO
	scePrintf( "up interface auto\n" );
	ret = load_set_conf_extra( &cd, net_buf, NET_DB, USR_CONF_NAME, sceLIBNETF_AUTO_UPIF );
	if ( ret < 0 ) {
		scePrintf( "load_set_conf_extra() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetWaitGetAddress( &cd, net_buf, if_id, sceLIBNET_MAX_INTERFACE, &myaddr, sceLIBNETF_AUTO_UPIF );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetWaitGetAddress() failed.(%d, %d)\n", ret, if_id[ 0 ] );
		ERR_STOP;
	}

#else
	scePrintf( "up interface no auto\n" );
	ret = load_set_conf_extra( &cd, net_buf, NET_DB, USR_CONF_NAME, 0 );
	if ( ret < 0 ) {
		scePrintf( "load_set_conf_extra() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetWaitGetInterfaceID( &cd, net_buf, if_id, sceLIBNET_MAX_INTERFACE );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetWaitGetInterfaceID() failed.(%d, %d)\n", ret, if_id[ 0 ] );
		ERR_STOP;
	}
	ret = sceInetCtlUpInterface( &cd, net_buf, 0 );
	if ( ret < 0 ) {
		scePrintf( "sceInetCtlUpInterface() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetWaitGetAddress( &cd, net_buf, if_id, sceLIBNET_MAX_INTERFACE, &myaddr, 0 );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetWaitGetAddress() failed.(%d, %d)\n", ret, if_id[ 0 ] );
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
	ret = sceInetAddress2String( &cd, net_buf, myptrname, 128, &myaddr );
	if ( ret < 0 ) {
		scePrintf( "sceInetAddress2String() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceInetAddress2Name( &cd, net_buf, 0, myname, 128, &myaddr, 0, 0 );
	if ( ret < 0 ) {
		scePrintf( "My address: %s\n", myptrname );
	}
	else {
		scePrintf( "My address: %s (%s)\n", myname, myptrname );
	}


	/***************************************************************
		Main
	***************************************************************/
	for ( i = 0; i < MAX_CONNECTION; i++ ) {
		memset( &g_connection[ i ], 0, sizeof( CONNECTIONDATA ) );
	}

	semap.initCount = g_th_num;
	semap.maxCount = g_th_num;
	g_passive_sema = CreateSema( &semap );
	if ( g_passive_sema <= 0 ) {
		scePrintf( "CreateSema() failed.(%d)\n", g_passive_sema );
		ERR_STOP;
	}

	semap.initCount = 0;
	semap.maxCount = MAX_CONNECTION;
	g_fin_sema = CreateSema( &semap );
	if ( g_fin_sema <= 0 ) {
		scePrintf( "CreateSema() failed.(%d)\n", g_fin_sema );
		ERR_STOP;
	}

	/* thread for clean up each conneciton thread */
	clean_up_stack = ( char* )memalign( 16, STACKSIZE );
	if ( clean_up_stack == NULL ) {
		scePrintf( "malloc failed.\n" );
		ERR_STOP;
	}

	tid = CreateStartThread( CleanUpThread, clean_up_stack, STACKSIZE, PRIO, NULL );
	if ( tid < 0 ) {
		scePrintf( "CreateStartThread() failed.(%d)\n", tid );
		ERR_STOP;
	}

	for ( ; ; ) {
		WaitSema( g_passive_sema );
		sid = SearchSID();
		if ( sid < 0 ) {
			scePrintf( "Invalid sid %d\n", sid );
			ERR_STOP;
		}
		p_connection = &g_connection[ sid ];
		p_connection->sid = sid;
		p_connection->exist = 1;
		p_connection->stack = ( char* )memalign( 16, STACKSIZE );
		if ( p_connection->stack == NULL ) {
			scePrintf( "malloc failed.\n" );
			ERR_STOP;
		}
		scePrintf( "main(): sid %d, stack = 0x%x\n", sid, ( u_int )p_connection->stack );
		tid = CreateStartThread( PassiveOpenThread, p_connection->stack, STACKSIZE, PRIO, &( p_connection->sid ) );
		if ( tid < 0 ) {
			scePrintf( "CreateStartThread() failed.(%d)\n", tid );
			ERR_STOP;
		}
		p_connection->tid = tid;
	}

	/***************************************************************
		Terminate Libnet
	***************************************************************/
	ret = sceInetCtlDownInterface( &cd, net_buf, 0 );
	if ( ret < 0 ) {
		scePrintf( "sceInetCtlDownInterface() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetUnregisterHandler( &cd, net_buf );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetUnregisterHandler() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetTerminate( &cd );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetTerminate() failed.(%d)\n", ret );
		ERR_STOP;
	}
	sceSifMExitRpc();

	return( 0 );
}

/*** End of file ***/

