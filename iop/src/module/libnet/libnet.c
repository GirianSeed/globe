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
 *       1.1.0        2002/01/24  ksh         sample to library
 *       1.2.0        2002/05/24  ksh         add REGHANDLER initialization
 *       1.2.0        2002/05/24  ksh         add sceLIBNET_IEV_DOWN event
 *       1.2.0        2002/05/27  ksh         add WAITGETIEV
 *       1.2.0        2002/10/11  ksh         event
 *       1.2.0        2002/10/21  ksh         abort id
 *       1.2.1        2002/12/16  ksh         return added in event handler
 *       1.2.2        2003/02/28  ksh         cpu suspend used
 *       1.2.3        2003/03/17  ksh         unload
 *       1.2.4        2003/06/11  ksh         msifrpc
 *       1.3.0        2003/09/18  ksh         thread stack size 8KB->4KB
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
#define	EXTERN	extern

#define	sceLIBNET_IEV_NOTHING	( -1 )
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

#define	sceLIBNET_F_START	( 0x00000001 )
#define	sceLIBNET_F_STOP	( 0x00000002 )

#define	sceLIBNET_MAX_EVENT	( 16 )

PUBLIC	ModuleInfo Module = { "Libnet", ( ( 2 << 8 ) | 30 ) };

EXTERN	libhead Libnet_entry;

PRIVATE	int 	g_tid = -1;
PRIVATE	int 	g_fverbose = 0;

PRIVATE	sceInetCtlEventHandlers_t	g_ev_handlers;
PRIVATE	int 	g_if_id[ sceLIBNET_MAX_INTERFACE ];

PRIVATE	int 	g_if_semaid = -1;
PRIVATE	int 	g_wait_index;
PRIVATE	int 	g_current_index;
PRIVATE	volatile int	g_fevent;
PRIVATE	int 	g_id[ sceLIBNET_MAX_EVENT ];
PRIVATE	int 	g_type[ sceLIBNET_MAX_EVENT ];


PRIVATE	void	LibnetInterfaceEventHandler( int id, int type )
{
	int 	oldstat;
	int 	ret;

	g_fevent |= sceLIBNET_F_START;
	if ( g_fverbose ) {
		printf( "Libnet: type %d: id %d\n", type, id );
	}

	CpuSuspendIntr( &oldstat );
	g_id[ g_current_index ] = id;
	g_type[ g_current_index ] = type;
	g_current_index = ( g_current_index + 1 ) % sceLIBNET_MAX_EVENT;
	CpuResumeIntr( oldstat );

	if ( g_if_semaid >= 0 ) {
		ret = SignalSema( g_if_semaid );
		if ( ret != KE_OK ) {
			g_fevent |= sceLIBNET_F_STOP;
		}
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

PRIVATE	int 	SetLibnetEvent( int id )
{
	int 	flags;

	sceInetInterfaceControl( id, sceInetCC_GetFlags, &flags, sizeof( flags ) );
	if ( flags & sceInetDevF_PPPoE ) {
	}
	else {
		if ( g_if_id[ 0 ] == id ) {
			return( sceLIBNET_IEV_ATTACH );
		}
	}
	return( sceLIBNET_IEV_NOTHING );
}

PRIVATE	int 	GetEvent( int *pid, int *ptype )
{
	int 	oldstat;
	int 	ret;
	int 	count = 5;

	*pid = 0;
	*ptype = 0;
	for ( ; ; ) {
		if ( ( g_fevent & sceLIBNET_F_START ) != 0 ) {
			break;
		}
		if ( count <= 0 ) {
			return( sceLIBNETE_CALL_CANCELLED );
		}
		count--;
		if ( g_fverbose ) {
			printf( "Libnet: retry %d\n", count );
		}
		DelayThread( 300 * 1000 );
	}
	if ( ( g_fevent & sceLIBNET_F_STOP ) != 0 ) {
		*pid = -1;
		return( sceLIBNETE_CALL_CANCELLED );
	}

	ret = WaitSema( g_if_semaid );
	if ( ret != KE_OK ) {
		return( sceLIBNETE_NG );
	}

	CpuSuspendIntr( &oldstat );
	*pid = g_id[ g_wait_index ];
	*ptype = g_type[ g_wait_index ];
	g_wait_index = ( g_wait_index + 1 ) % sceLIBNET_MAX_EVENT;
	CpuResumeIntr( oldstat );
	
	return( sceLIBNETE_OK );
}

PRIVATE	int 	CheckEvent( int id, int type )
{
	int 	ret = sceLIBNET_IEV_NOTHING;

	switch ( type ) {
	case sceINETCTL_IEV_Attach:
		ret = SetLibnetEvent( id );
		break;

	case sceINETCTL_IEV_Conf:
		CheckInterface( id );
		break;

	case sceINETCTL_IEV_Start:
		ret = sceLIBNET_IEV_START;
		break;

	case sceINETCTL_IEV_Down:
		ret = sceLIBNET_IEV_DOWN;
		break;

	case sceINETCTL_IEV_Detach:
	case sceINETCTL_IEV_Error:
		if ( g_if_id[ 0 ] == id ) {
			ret = sceLIBNET_IEV_DOWN;
		}
		break;

	case sceINETCTL_IEV_NoConf:
		if ( g_if_id[ 0 ] == 0 ) {
			ret = sceLIBNET_IEV_DOWN;
		}
		break;

	default:
		break;
	}

	return( ret );
}

PRIVATE	void*	LibnetDispatch( u_int fno, void *buf, int size )
{
	int*	data = ( int* )buf;

	switch ( fno ) {
	case RPC_SCEPOLL:
		{
		const int	nfds = data[ 0 ];
		const int	ms = data[ 1 ];

		data[ 0 ] = sceInetPoll(
			( sceInetPollFd_t* )&data[ 2 ],
			nfds,
			ms
		);
		}
		break;

	case RPC_SCENAME2ADDR:
		{
		const int	flags = data[ 0 ];
		const int	ms = data[ 1 ];
		const int	nretry = data [ 2 ];
		const int	abort_id = data [ 3 ];
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
			nretry,
			abort_id
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
		const int	abort_id = data[ 8 ];

		data[ 0 ] = sceInetAddress2Name(
			flags,
			( char* )&data[ 8 ],
			len,
			addr,
			ms,
			nretry,
			abort_id
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

		data[ 0 ] = sceInetCtlUpInterface(
			id
		);
		}
		break;

	case RPC_DOWNIF:
		{
		const int	id = data[ 0 ];

		data[ 0 ] = sceInetCtlDownInterface(
			id
		);
		}
		break;

	case RPC_SETAUTOMODE:
		{
		const int 	f_auto = data[ 0 ];

		data[ 0 ] = sceInetCtlSetAutoMode(
			f_auto
		);
		}
		break;

	case RPC_GETSTATE:
		{
		const int	id = data[ 0 ];

		data[ 0 ] = sceInetCtlGetState(
			id,
			&data[ 1 ]
		);
		}
		break;

	case RPC_REGHANDLER:
		{
		struct SemaParam	sema_param;
		int 	i;

		for ( i = 0; i < sceLIBNET_MAX_INTERFACE; i++ ) {
			g_if_id[ i ] = 0;
		}
		g_wait_index = 0;
		g_current_index = 0;
		g_fevent = 0;

		sema_param.attr = SA_THPRI;
		sema_param.initCount = 0;
		sema_param.maxCount = sceLIBNET_MAX_EVENT;
		sema_param.option = 0;
		g_if_semaid = CreateSema( &sema_param );
		if ( g_if_semaid < 0 ) {
			data[ 0 ] = sceLIBNETE_NG;
			break;
		}

		g_ev_handlers.func = LibnetInterfaceEventHandler;
		data[ 0 ] = sceInetCtlRegisterEventHandler(	&g_ev_handlers );
		}
		break;

	case RPC_UNREGHANDLER:
		{
		int 	oldstat;
		int 	sid;
		int 	ret;

		data[ 0 ] = sceInetCtlUnregisterEventHandler( &g_ev_handlers );
		
		CpuSuspendIntr( &oldstat );
		sid = g_if_semaid;
		g_if_semaid = -1;
		CpuResumeIntr( oldstat );
		
		ret = DeleteSema( sid );
		if ( ret != KE_OK ) {
			data[ 0 ] = sceLIBNETE_NG;
			break;
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
		int 	id;
		int 	type;
		int 	ret;
		int 	i;

		for ( ; ; ) {
			ret = GetEvent( &id, &type );
			if ( ret < 0 ) {
				data[ 0 ] = ret;
				data[ 1 ] = id;
				break;
			}
			ret = CheckEvent( id, type );
			if ( ret == sceLIBNET_IEV_DOWN ) {
				data[ 0 ] = sceLIBNETE_CALL_CANCELLED;
				data[ 1 ] = type;
				data[ 2 ] = id;
				break;
			}
			if ( ret == sceLIBNET_IEV_ATTACH ) {
				data[ 0 ] = ret;
				for ( i = 0; i < sceLIBNET_MAX_INTERFACE; i++ ) {
					data[ i + 1 ] = g_if_id[ i ];
				}
				break;
			}
		}
		}
		break;

	case RPC_WAITIFSTARTED:
		{
		int 	id;
		int 	type;
		int 	ret;
		int 	i;

		for ( ; ; ) {
			ret = GetEvent( &id, &type );
			if ( ret < 0 ) {
				data[ 0 ] = ret;
				data[ 1 ] = id;
				break;
			}
			ret = CheckEvent( id, type );
			if ( ret == sceLIBNET_IEV_DOWN ) {
				data[ 0 ] = sceLIBNETE_CALL_CANCELLED;
				data[ 1 ] = type;
				data[ 2 ] = id;
				break;
			}
			if ( ret == sceLIBNET_IEV_START ) {
				data[ 0 ] = ret;
				for ( i = 0; i < sceLIBNET_MAX_INTERFACE; i++ ) {
					data[ i + 1 ] = g_if_id[ i ];
				}
				break;
			}
		}
		}
		break;

	case RPC_WAITGETIEV:
		{
		data[ 0 ] = GetEvent( &data[ 1 ], &data[ 2 ] );
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
	sceSifMEntryLoop( &se, LIBNET_SIFNUMBER, LibnetDispatch, 0 );
	SleepThread();
	
	return( 0 );
}

PRIVATE	int 	CreateStartLibnetThread( void )
{
	struct ThreadParam	tp;
	int 	tid;
	int 	ret;

	tp.stackSize = 4 * 1024;
	tp.initPriority = sceLIBNET_PRIORITY_IOP;
	tp.entry = LibnetThread;
	tp.attr = TH_C;
	tp.option = 0;

	tid = CreateThread( &tp );
	if ( tid <= 0 ) {
		printf( "CreateThread() in Libnet failed.(%d)\n", tid );
		return( -1 );
	}

	ret = StartThread( tid, 0 );
	if ( ret != KE_OK ) {
		printf( "StartThread() in Libnet failed.(%d)\n", ret );
		DeleteThread( tid );
		return( -2 );
	}

	return( tid );
}

PRIVATE	void	CheckOptions( int argc, char *argv[] )
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

PRIVATE	int 	module_start( void )
{
	int 	tid;
	int 	ret;

	ret = RegisterLibraryEntries( &Libnet_entry );
	if ( ret != KE_OK ) {
		return( NO_RESIDENT_END );
	}

	sceSifInitRpc( 0 );

	tid = CreateStartLibnetThread();
	if ( tid < 0 ) {
		ReleaseLibraryEntries( &Libnet_entry );
		return( NO_RESIDENT_END );
	}

	g_tid = tid;
	return( REMOVABLE_RESIDENT_END );
}

PRIVATE	int 	module_stop( void )
{
	int 	ret;

	sceSifMTermRpc( LIBNET_SIFNUMBER, 0 );
	ret = TerminateThread( g_tid );
	if ( ret != KE_OK ) {
		return( REMOVABLE_RESIDENT_END );
	}
	DeleteThread( g_tid );
	g_tid = -1;

	ReleaseLibraryEntries( &Libnet_entry );

	return( NO_RESIDENT_END );
}

PUBLIC	int 	start( int argc, char *argv[] )
{
	if ( argc < 0 ) {
		return( module_stop() );
	}

	if ( PAD_INETADDRESS != 16 ) {
		printf( "sceInetAddress_t: 16( now %d ).\n", PAD_INETADDRESS );
		return( NO_RESIDENT_END );
	}

	CheckOptions( argc, argv );

	return( module_start() );
}

/*** End of file ***/

