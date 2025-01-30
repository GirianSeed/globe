/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                          Libnet Library
 *
 *      Copyright (C) 2000-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version      Date        Design      Log
 *  --------------------------------------------------------------------
 *       1.1.0        2002/06/03  ksh         first version
 *       1.2.0        2003/04/02  ksh         USB to HDD Ethernet etc.
 *       1.2.1        2003/04/10  ksh         return in sceINETCTL_IEV_Down
 *       1.3.0        2003/04/15  ksh         net_buf
 *       1.3.1        2003/04/18  ksh         DelayThread() used
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
#define	PRIVATE	static
#define	PUBLIC
#define	ERR_STOP	for ( ; ; )

#define	NETBUFSIZE	( 2048 )
#define	STACKSIZE	( 0x4000 )
#define	PRIO	( 63 )

PRIVATE	int 	g_sema;


PRIVATE int 	LoadModule( const char *module, int args, const char *argp )
{
	int 	id;
	
	while ( ( id = sceSifLoadModule( module, args, argp ) ) < 0 ) {
		scePrintf( "%s cannot be loaded.\n", module );
	}

	return( id );
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

PRIVATE	void	DoInterfaceUp1( sceSifMClientData *pcd, void *net_buf )
{
	sceInetAddress_t	myaddr;
	char	myname[ 128 ];
	char	myptrname[ 128 ];
	int 	if_id[ sceLIBNET_MAX_INTERFACE ];
	int 	ret;
	int 	i;

	ret = sceLibnetRegisterHandler( pcd, net_buf );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetRegisterHandler() failed.(%d)\n", ret );
		ERR_STOP;
	}

#ifdef  UP_INTERFACE_AUTO
	scePrintf( "up interface auto\n" );
	ret = load_set_conf_extra( pcd, net_buf, NET_DB, USR_CONF_NAME, sceLIBNETF_AUTO_UPIF );
	if ( ret < 0 ) {
		scePrintf( "load_set_conf_extra() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetWaitGetAddress( pcd, net_buf, if_id, sceLIBNET_MAX_INTERFACE, &myaddr, sceLIBNETF_AUTO_UPIF );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetWaitGetAddress() failed.(%d, %d)\n", ret, if_id[ 0 ] );
		ERR_STOP;
	}

#else
	scePrintf( "up interface no auto\n" );
	ret = load_set_conf_extra( pcd, net_buf, NET_DB, USR_CONF_NAME, 0 );
	if ( ret < 0 ) {
		scePrintf( "load_set_conf_extra() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetWaitGetInterfaceID( pcd, net_buf, if_id, sceLIBNET_MAX_INTERFACE );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetWaitGetInterfaceID() failed.(%d, %d)\n", ret, if_id[ 0 ] );
		ERR_STOP;
	}
	ret = sceInetCtlUpInterface( pcd, net_buf, 0 );
	if ( ret < 0 ) {
		scePrintf( "sceInetCtlUpInterface() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetWaitGetAddress( pcd, net_buf, if_id, sceLIBNET_MAX_INTERFACE, &myaddr, 0 );
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

	ret = sceInetAddress2String( pcd, net_buf, myptrname, 128, &myaddr );
	if ( ret < 0 ) {
		scePrintf( "sceInetAddress2String() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceInetAddress2Name( pcd, net_buf, 0, myname, 128, &myaddr, 0, 0 );
	if ( ret < 0 ) {
		scePrintf( "My address: %s\n", myptrname );
	}
	else {
		scePrintf( "My address: %s (%s)\n", myname, myptrname );
	}
}

PRIVATE	void	DoInterfaceUp2( sceSifMClientData *pcd, void *net_buf )
{
	int 	id;
	int 	type;
	int 	ret;

	ret = sceLibnetRegisterHandler( pcd, net_buf );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetRegisterHandler() failed.(%d)\n", ret );
		ERR_STOP;
	}

	ret = load_set_conf_extra( pcd, net_buf, NET_DB, USR_CONF_NAME, sceLIBNETF_AUTO_UPIF );
	if ( ret < 0 ) {
		scePrintf( "load_set_conf_extra() failed.(%d)\n", ret );
		ERR_STOP;
	}

	for ( ; ; ) {
		ret = sceLibnetWaitGetInterfaceEvent( pcd, net_buf, &id, &type );
		if ( ret < 0 ) {
			scePrintf( "sceLibnetWaitGetInterfaceEvent() failed.(%d, %d)\n", ret, id );
			ERR_STOP;
		}

		switch ( type ) {
		case sceINETCTL_IEV_Attach:
			scePrintf( "id: %d, sceINETCTL_IEV_Attach\n", id );
			break;

		case sceINETCTL_IEV_Start:
			scePrintf( "id: %d, sceINETCTL_IEV_Start\n", id );
			return;

		case sceINETCTL_IEV_Up:
			scePrintf( "id: %d, sceINETCTL_IEV_Up\n", id );
			break;

		case sceINETCTL_IEV_Down:
			scePrintf( "id: %d, sceINETCTL_IEV_Down\n", id );
			return;

		case sceINETCTL_IEV_Conf:
			scePrintf( "id: %d, sceINETCTL_IEV_Conf\n", id );
			break;

		default:
			scePrintf( "id: %d, type: %d\n", id, type );
			break;
		}
	}
}

PRIVATE	void	InterfaceThread( void *data )
{
	static u_char	net_buf[ NETBUFSIZE ] __attribute__( ( aligned( 64 ) ) );
	sceSifMClientData	cd;
	int 	ret;
	int 	id;
	int 	type;

	ret = sceLibnetInitialize( &cd, sizeof( net_buf ), sceLIBNET_STACKSIZE, sceLIBNET_PRIORITY );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetInitialize() failed.(%d)\n", ret );
		ERR_STOP;
	}

	for ( ; ; ) {
		ret = sceLibnetWaitGetInterfaceEvent( &cd, net_buf, &id, &type );
		if ( ret < 0 ) {
			scePrintf( "sceLibnetWaitGetInterfaceEvent() failed.(%d, %d)\n", ret, id );
			ERR_STOP;
		}

		switch ( type ) {
		case sceINETCTL_IEV_Attach:
			scePrintf( "id: %d, sceINETCTL_IEV_Attach\n", id );
			break;

		case sceINETCTL_IEV_Start:
			scePrintf( "id: %d, sceINETCTL_IEV_Start\n", id );
			SignalSema( g_sema );
			return;

		case sceINETCTL_IEV_Up:
			scePrintf( "id: %d, sceINETCTL_IEV_Up\n", id );
			break;

		case sceINETCTL_IEV_Down:
			scePrintf( "id: %d, sceINETCTL_IEV_Down\n", id );
			return;

		case sceINETCTL_IEV_Conf:
			scePrintf( "id: %d, sceINETCTL_IEV_Conf\n", id );
			break;

		default:
			scePrintf( "id: %d, type: %d\n", id, type );
			break;
		}
	}
	
	ret = sceLibnetTerminate( &cd );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetTerminate() failed.(%d)\n", ret );
		ERR_STOP;
	}
	SleepThread();
}

PRIVATE	void	DoInterfaceUp3( sceSifMClientData *pcd, void *net_buf )
{
	static char	stack[ STACKSIZE ] __attribute__( ( aligned( 16 ) ) );
	struct ThreadParam	tp;
	struct SemaParam	sema;
	int 	tid;
	int 	ret;
	int 	times = 0;

	sema.initCount = 0;
	sema.maxCount = 1;
	g_sema = CreateSema( &sema );
	if ( g_sema <= 0 ) {
		scePrintf( "CreateSema() failed.(%d)\n", g_sema );
		ERR_STOP;
	}

	ret = sceLibnetRegisterHandler( pcd, net_buf );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetRegisterHandler() failed.(%d)\n", ret );
		ERR_STOP;
	}

	ret = load_set_conf_extra( pcd, net_buf, NET_DB, USR_CONF_NAME, sceLIBNETF_AUTO_UPIF );
	if ( ret < 0 ) {
		scePrintf( "load_set_conf_extra() failed.(%d)\n", ret );
		ERR_STOP;
	}

	tid = CreateStartThread( InterfaceThread, stack, STACKSIZE, PRIO, NULL );
	if ( tid < 0 ) {
		scePrintf( "CreateStartThread() failed.(%d)\n", tid );
		ERR_STOP;
	}

	ReferThreadStatus( GetThreadId(), &tp );
	ChangeThreadPriority( GetThreadId(), PRIO + 1 );

	for ( ; ; ) {
		if ( PollSema( g_sema ) != -1 ) {
			break;
		}

		scePrintf( "Wait %d\n", times );
		times++;

		DelayThread( 100 * 1000 );
	}

	ChangeThreadPriority( GetThreadId(), tp.currentPriority );
	TerminateThread( tid );
	DeleteThread( tid );
	DeleteSema( g_sema );
}

int 	main( int argc, char *argv[] )
{
	static u_char	net_buf[ NETBUFSIZE ] __attribute__( ( aligned( 64 ) ) );
	sceSifMClientData	cd;
	int 	ret;
	int 	type;

	if ( argc != 2 ) {
		scePrintf( "usage: %s <function( 1 - 3 )>\n", argv[ 0 ] );
		return( 0 );
	}
	type = atoi( argv[ 1 ] );

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

	switch ( type ) {
	case 1:
		DoInterfaceUp1( &cd, net_buf );
		break;

	case 2:
		DoInterfaceUp2( &cd, net_buf );
		break;

	case 3:
		DoInterfaceUp3( &cd, net_buf );
		break;

	default:
		scePrintf( "invalid function\n" );
		ERR_STOP;
	}

	/***************************************************************
		Main
	***************************************************************/


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

