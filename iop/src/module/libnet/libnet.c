/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/* 
 *                          Libnet Library
 *
 *      Copyright (C) 2000-2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version      Date        Design      Log
 *  --------------------------------------------------------------------
 *       1.1.0        2002/01/24  ksh         sample to library
 */

#include <stdio.h>
#include <kernel.h>
#include <netcnf.h>
#include <inet/inet.h>
#include <inet/inetctl.h>
#include <inet/in.h>
#include <inet/netdev.h>
#include <sifrpc.h>
#include <msifrpc.h>

#include <libnet/common.h>
#include <libnet/libnetdefs.h>

#define	PRIVATE	static
#define	PUBLIC

#define	sceLIBNET_IEV_ATTACH	( 0x0001 )
#define	sceLIBNET_IEV_DETACH	( 0x0002 )
#define	sceLIBNET_IEV_START	( 0x0004 )
#define	sceLIBNET_IEV_STOP	( 0x0008 )
#define	sceLIBNET_IEV_ERROR	( 0x0010 )
#define	sceLIBNET_IEV_CONF	( 0x0020 )
#define	sceLIBNET_IEV_NOCONF	( 0x0040 )
#define	sceLIBNET_IEV_UP	( 0x0080 )
#define	sceLIBNET_IEV_DOWN	( 0x0100 )
#define	sceLIBNET_IEV_RETRY	( 0x0200 )


PUBLIC	ModuleInfo Module = { "Libnet", ( ( 1 << 8 ) | 10 ) };

PRIVATE	int 	g_fverbose = 0;

PRIVATE	sceInetCtlEventHandlers_t	g_ev_handlers;
PRIVATE	int 	g_if_evid;
PRIVATE	int 	g_if_id[ sceLIBNET_MAX_INTERFACE ];


PRIVATE	void	SetInterfaceEvent( int id, u_long flags )
{
	int 	ret;

	ret = SetEventFlag( g_if_evid, flags );
	if ( ret != KE_OK ) {
		printf( "SetEventFlag() in Libnet failed.(%d)\n", ret );
	}
}

PRIVATE	void	CheckInterface( int id )
{
	int 	flags;
	int 	i;

	sceInetInterfaceControl( id, sceInetCC_GetFlags, &flags, sizeof( flags ) );
	if ( flags & sceInetDevF_PPPoE ) {
		for ( i = 1; i < sceLIBNET_MAX_INTERFACE; i++ ) {
			if ( g_if_id[ i ] == 0 ) {
				g_if_id[ i ] = id;
				break;
			}
		}
	}
	else {
		g_if_id[ 0 ] = id;
	}
}

PRIVATE	void	SetLibnetEvent( int id )
{
	int 	flags;

	sceInetInterfaceControl( id, sceInetCC_GetFlags, &flags, sizeof( flags ) );
	if ( flags & sceInetDevF_PPPoE ) {
	}
	else {
		if ( g_if_id[ 0 ] == id ) {
			SetInterfaceEvent( id, sceLIBNET_IEV_ATTACH );
		}
	}
}

PRIVATE	void	LibnetInterfaceEventHandler( int id, int type )
{
	switch ( type ) {
	case sceINETCTL_IEV_Attach:
		if ( g_fverbose ) {
			printf( "Libnet: attach:%d\n", id );
		}
		SetLibnetEvent( id );
		break;

	case sceINETCTL_IEV_Conf:
		if ( g_fverbose ) {
			printf( "Libnet: conf:%d\n", id );
		}
		CheckInterface( id );
		break;

	case sceINETCTL_IEV_Start:
		if ( g_fverbose ) {
			printf( "Libnet: start:%d\n", id );
		}
		SetInterfaceEvent( id, sceLIBNET_IEV_START );
		break;

	default:
		if ( g_fverbose ) {
			printf( "Libnet: type %d: id %d\n", type, id );
		}
		break;
	}
}

PRIVATE	void*	LibnetDispatch( u_int fno, void* buf, int size )
{
	int*	data = ( int* )buf;

	switch ( fno ) {
	case RPC_SCENAME2ADDR:
		{
		const int	flags = data[ 0 ];
		const int	ms = data[ 1 ];
		const int	nretry = data [ 2 ];
		const int	len = data[ 4 ];
		char*	name;

		if ( len <= 0 ) {
			name = NULL;
		}
		else {
			name = ( char* )&data[ 5 ];
		}

		data[ 0 ] = sceInetName2Address(
			flags,
			( sceInetAddress_t* )&data[ 1 ],
			name,
			ms,
			nretry
		);
		}
		break;

	case RPC_SCEADDR2STR:
		{
		const int	len = data[ 0 ];
		sceInetAddress_t*	addr = ( sceInetAddress_t* )&data[ 1 ];

		data[ 0 ] = sceInetAddress2String(
			( char* )&data[ 5 ],
			len,
			addr
		);
		}
		break;

	case RPC_SCEADDR2NAME:
		{
		const int	flags = data[ 0 ];
		const int	len = data[ 1 ];
		const int	ms = data[ 2 ];
		const int	nretry = data[ 3 ];
		sceInetAddress_t*	addr = ( sceInetAddress_t* )&data[ 4 ];

		data[ 0 ] = sceInetAddress2Name(
			flags,
			( char* )&data[ 8 ],
			len,
			addr,
			ms,
			nretry
		);
		}
		break;

	case RPC_SCECREATE:
		{
		sceInetParam_t*	param = ( sceInetParam_t* )&data[ 1 ];

		data[ 0 ] = sceInetCreate(
			param
		);
		}
		break;

	case RPC_SCEOPEN:
		{
		const int	cid = data[ 0 ];
		const int	ms = data[ 1 ];

		data[ 0 ] = sceInetOpen(
			cid,
			ms
		);
		}
		break;

	case RPC_SCECLOSE:
		{
		const int	cid = data[ 0 ];
		const int	ms = data[ 1 ];

		data[ 0 ] = sceInetClose(
			cid,
			ms
		);
		}
		break;

	case RPC_SCERECV:
		{
		const int	cid = data[ 0 ];
		const int	count = data[ 2 ];
		const int	ms = data[ 3 ];

		data[ 0 ] = sceInetRecv(
			cid,
			&data[ 2 ],
			count,
			&data[ 1 ],
			ms
		);
		}
		break;

	case RPC_SCERECVF:
		{
		const int	cid = data[ 0 ];
		const int	count = data[ 2 ];
		const int	ms = data[ 3 ];

		data[ 0 ] = sceInetRecvFrom(
			cid,
			&data[ 7 ],
			count,
			&data[ 1 ],
			( sceInetAddress_t* )&data[ 2 ],
			&data[ 6 ],
			ms
		);
		}
		break;

	case RPC_SCESEND:
		{
		const int	cid = data[ 0 ];
		const int	count = data[ 2 ];
		const int	ms = data[ 3 ];
		int*	ptr = &data[ 4 ];

		data[ 0 ] = sceInetSend(
			cid,
			ptr,
			count,
			&data[ 1 ],
			ms
		);
		}
		break;

	case RPC_SCESENDT:
		{
		const int	cid = data[ 0 ];
		const int	count = data[ 2 ];
		const int	ms = data[ 3 ];
		const int	port = data[ 4 ];
		sceInetAddress_t*	addr = ( sceInetAddress_t* )&data[ 5 ];
		int*	ptr = &data[ 9 ];

		data[ 0 ] = sceInetSendTo(
			cid,
			ptr,
			count,
			&data[ 1 ],
			addr,
			port,
			ms
		);
		}
		break;

	case RPC_SCEABORT:
		{
		const int	cid = data[ 0 ];
		const int	flags = data[ 1 ];

		data[ 0 ] = sceInetAbort(
			cid,
			flags
		);
		}
		break;

	case RPC_SCECTL:
		{
		const int	cid = data[ 0 ];
		const int	code = data[ 1 ];
		const int	len = data[ 2 ];

		if ( len == 0 ) {
			data[ 0 ] = sceInetControl(
				cid,
				code,
				NULL,
				0
			);
		}
		else {
			data[ 0 ] = sceInetControl(
				cid,
				code,
				&data[ 3 ],
				len
			);
		}
		}
		break;

	case RPC_SCEGIFLIST:
		{
		const int	n = data[ 0 ];

		data[ 0 ] = sceInetGetInterfaceList(
			&data[ 1 ],
			n
		);
		}
		break;

	case RPC_SCEIFCTL:
		{
		const int	interface_id = data[ 0 ];
		const int	code = data[ 1 ];
		const int	len = data[ 2 ];

		if ( len == 0 ) {
			data[ 0 ] = sceInetInterfaceControl(
				interface_id,
				code,
				NULL,
				0
			);
		}
		else {
			data[ 0 ] = sceInetInterfaceControl(
				interface_id,
				code,
				&data[ 3 ],
				len
			);
		}
		}
		break;

	case RPC_SCEGETRT:
		{
		const int	n = data[ 0 ];

		data[ 0 ] = sceInetGetRoutingTable(
			( sceInetRoutingEntry_t* )&data[ 1 ],
			n
		);
		}
		break;

	case RPC_SCEGETNS:
		{
		const int	n = data[ 0 ];

		data[ 0 ] = sceInetGetNameServers(
			( sceInetAddress_t* )&data[ 1 ],
			n
		);
		}
		break;

	case RPC_SCECHPRI:
		{
		const int	prio = data[ 0 ];

		data[ 0 ] = sceInetChangeThreadPriority(
			prio
		);
		}
		break;

	case RPC_SCEGETLOG:
		{
		const int	len = data[ 0 ];
		const int	ms = data[ 1 ];

		data[ 0 ] = sceInetGetLog(
			( char* )&data[ 1 ],
			len,
			ms
		);
		}
		break;

	case RPC_SCEABORTLOG:
		{
		data[ 0 ] = sceInetAbortLog();
		}
		break;

	case RPC_UPIF:
		{
		const int	id = data[ 0 ];

		data[ 0 ] = sceInetCtlUpInterface( id );
		}
		break;

	case RPC_DOWNIF:
		{
		const int	id = data[ 0 ];

		data[ 0 ] = sceInetCtlDownInterface( id );
		}
		break;

	case RPC_SETAUTOMODE:
		{
		const int 	f_auto = data[ 0 ];

		data[ 0 ] = sceInetCtlSetAutoMode( f_auto );
		}
		break;

	case RPC_REGHANDLER:
		{
		struct	EventFlagParam	ev_param;

		ev_param.attr = EA_MULTI;
		ev_param.initPattern = 0;
		ev_param.option = 0;

		g_if_evid = CreateEventFlag( &ev_param );
		if ( g_if_evid <= 0 ) {
			data[ 0 ] = sceLIBNETE_NG;
		}
		else {
			g_ev_handlers.func = LibnetInterfaceEventHandler;
			data[ 0 ] = sceInetCtlRegisterEventHandler(	&g_ev_handlers );
		}
		}
		break;

	case RPC_UNREGHANDLER:
		{
		int 	ret;

		ret = DeleteEventFlag( g_if_evid );
		if ( ret != KE_OK ) {
			data[ 0 ] = sceLIBNETE_NG;
		}
		else {
			data[ 0 ] = sceInetCtlUnregisterEventHandler( &g_ev_handlers );
		}
		}
		break;

	case RPC_SETCONFIG:
		{
		u_int	env_addr = ( u_int )data[ 0 ];

		data[ 0 ] = sceInetCtlSetConfiguration( ( sceNetCnfEnv_t* )env_addr );
		}
		break;

	case RPC_WAITIFATTACHED:
		{
		int 	ret;
		int 	i;

		ret = WaitEventFlag( g_if_evid, sceLIBNET_IEV_ATTACH, EW_OR, NULL );
		if ( ret != KE_OK ) {
			data[ 0 ] = sceLIBNETE_NG;
		}
		else {
			data[ 0 ] = ret;
			for ( i = 0; i < sceLIBNET_MAX_INTERFACE; i++ ) {
				data[ i + 1 ] = g_if_id[ i ];
			}
		}
		}
		break;

	case RPC_WAITIFSTARTED:
		{
		int 	ret;
		int 	i;

		ret = WaitEventFlag( g_if_evid, sceLIBNET_IEV_START, EW_OR, NULL );
		if ( ret != KE_OK ) {
			data[ 0 ] = sceLIBNETE_NG;
		}
		else {
			data[ 0 ] = ret;
			for ( i = 0; i < sceLIBNET_MAX_INTERFACE; i++ ) {
				data[ i + 1 ] = g_if_id[ i ];
			}
		}
		}
		break;

	default:
		data[ 0 ] = sceLIBNETE_NO_RPC_CALL;
		break;
	}

	return( data );
}

PRIVATE	int 	LibnetThread( void )
{
	sceSifMServeEntry	se;

	sceSifMInitRpc( 0 );
	sceSifMEntryLoop( &se, LIBNET_SIFNUMER, LibnetDispatch, 0 );

	return( 0 );
}

PRIVATE	int 	CreateLibnetThread( void )
{
	struct ThreadParam	tp;
	int 	tid;
	int 	ret;

	tp.stackSize = sceLIBNET_STACKSIZE;
	tp.initPriority = sceLIBNET_PRIORITY_IOP;
	tp.entry = LibnetThread;
	tp.attr = TH_C;
	tp.option = 0;

	tid = CreateThread( &tp );
	if ( tid <= 0 ) {
		printf( "CreateThread() in Libnet failed.(%d)\n", tid );
		return( NO_RESIDENT_END );
	}

	ret = StartThread( tid, 0 );
	if ( ret != KE_OK ) {
		printf( "StartThread() in Libnet failed.(%d)\n", ret );
		return( NO_RESIDENT_END );
	}

	return( RESIDENT_END );
}

PRIVATE	void	CheckOptions( int argc, char* argv[] )
{
	for ( ; ; ) {
		argc--;
		argv++;
		if ( argc <= 0 ) {
			break;
		}
		if ( strcmp( "-verbose", argv[ 0 ] ) == 0 ) {
			g_fverbose = 1;
		}
		else {
			break;
		}
	}
}

PUBLIC	int 	start( int argc, char* argv[] )
{
	if ( PAD_INETADDRESS != 16 ) {
		printf( "Size of sceInetAddress_t must be 16.\n" );
		return( NO_RESIDENT_END );
	}

	CheckOptions( argc, argv );

	sceSifInitRpc( 0 );

	return( CreateLibnetThread() );
}

/*** End of file ***/

