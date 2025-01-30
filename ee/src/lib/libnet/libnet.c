/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                          Libnet Library
 *
 *      Copyright (C) 2000-2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version   Date        Design   Log
 *  --------------------------------------------------------------------
 *       1.1.0     2002/01/24  ksh      sample to library
 *       1.2.0     2002/05/24  ksh      add Initialize() check
 *       1.2.0     2002/05/28  ksh      add WaitGetInterfaceEvent()
 *       1.2.0     2002/10/28  ksh      add abort_id
 *       1.2.1     2003/04/07  ksh      error message & va_end() added
 *       1.3.0     2003/04/15  ksh      u_int *net_buf to void *net_buf
 */

#include <eekernel.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sifrpc.h>
#include <libnet.h>
#include <libnet/common.h>

#define	PRIVATE	static
#define	PUBLIC



/**************************************
 * common
 **************************************/
PRIVATE	int 	CallRpc( sceSifMClientData *cd, u_int fno, void *net_buf, int ssize, int rsize )
{
	const int	maxbufsize = sceSifMGetBufferSize( cd );
	const int	spadsize = sceLIBNET_PAD( ssize );
	const int	rpadsize = sceLIBNET_PAD( rsize );
	void	*send = net_buf;
	void	*receive = net_buf;
	int 	ret;

	if ( spadsize > maxbufsize || rpadsize > maxbufsize ) {
		scePrintf( "[fatal error] insufficient buffer size in Libnet\n" );
		scePrintf( "(fno=%d, ssize=%d, rsize=%d, buffer=%d)\n", fno, spadsize, rpadsize, maxbufsize );
		return( sceLIBNETE_INSUFFICIENT_RESOURCES );
	}

	ret = sceSifMCallRpc( cd, fno, 0, send, spadsize, receive, rpadsize, NULL, NULL );

	return( ret );
}

PRIVATE	int 	MemcpyD( sceSifMClientData *cd, int *buf, int index, const void *src, int n )
{
	const int 	maxbufsize = sceSifMGetBufferSize( cd ) - index * sizeof( int );

	if ( src == NULL ) {
		return( sceLIBNETE_OK );
	}
	if ( n < 0 || n > maxbufsize ) {
		return( sceLIBNETE_INSUFFICIENT_RESOURCES );
	}

	memcpy( &buf[ index ], src, n );

	return( sceLIBNETE_OK );
}

PRIVATE	int 	MemcpyS( sceSifMClientData *cd, void *dest, const int *buf, int index, int n )
{
	const int 	maxbufsize = sceSifMGetBufferSize( cd ) - index * sizeof( int );

	if ( dest == NULL ) {
		return( sceLIBNETE_OK );
	}

	if ( n < 0 || n > maxbufsize ) {
		return( sceLIBNETE_INSUFFICIENT_RESOURCES );
	}

	memcpy( dest, &buf[ index ], n );

	return( sceLIBNETE_OK );
}

PRIVATE	int 	StrcpyD( sceSifMClientData *cd, int *buf, int index, const char *src )
{
	const int 	maxbufsize = sceSifMGetBufferSize( cd ) - index * sizeof( int );
	int 	len;

	if ( src == NULL ) {
		return( 0 );
	}

	len = strlen( src ) + 1;
	if ( len < 0 || len > maxbufsize ) {
		return( sceLIBNETE_INSUFFICIENT_RESOURCES );
	}

	strcpy( ( char* )&buf[ index ], src );

	return( len );
}

PRIVATE	int 	StrcpyS( char *dest, int *buf, int index )
{
	if ( dest == NULL ) {
		return( 0 );
	}

	strcpy( dest, ( char* )&buf[ index ] );

	return( 0 );
}


/**************************************
 * INET
 **************************************/
PUBLIC	int 	sceInetPoll( sceSifMClientData *cd, void *net_buf, sceInetPollFd_t *fds, int nfds, int ms )
{
	int 	*buf = ( int* )net_buf;
	const int	size = sizeof( sceInetPollFd_t ) * nfds;
	int 	ret;

	buf[ 0 ] = nfds;
	buf[ 1 ] = ms;
	ret = MemcpyD( cd, buf, 2, fds, size );
	if ( ret < 0 ) {
		return( ret );
	}

	ret = CallRpc(
		cd,
		RPC_SCEPOLL,
		net_buf,
		sizeof( u_int ) * 2 + size,
		sizeof( u_int ) + size
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	ret = MemcpyS( cd, fds, buf, 2, size );
	if ( ret < 0 ) {
		return( ret );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetName2Address( sceSifMClientData *cd, void *net_buf, int flags, struct sceInetAddress *paddr, const char *name, int ms, int nretry, ... )
{
	int 	*buf = ( int* )net_buf;
	va_list	ap;
	int 	ret;

	buf[ 0 ] = flags;
	buf[ 1 ] = ms;
	buf[ 2 ] = nretry;
	buf[ 3 ] = 0;
	if ( flags & sceInetN2AF_Abort ) {
		va_start( ap, nretry );
		buf[ 3 ] = va_arg( ap, int );
		va_end( ap );
	}
	ret = StrcpyD( cd, buf, 5, name );
	if ( ret < 0 ) {
		return( ret );
	}
	buf[ 4 ] = ret;

	ret = CallRpc(
		cd,
		RPC_SCENAME2ADDR,
		net_buf,
		sizeof( u_int ) * 5 + buf[ 4 ],
		sizeof( u_int ) * 5
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	ret = MemcpyS( cd, paddr, buf, 1, sizeof( sceInetAddress_t ) );
	if ( ret < 0 ) {
		return( ret );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetAddress2String( sceSifMClientData *cd, void *net_buf, char *name, int len, struct sceInetAddress *paddr )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	buf[ 0 ] = len;
	ret = MemcpyD( cd, buf, 1, paddr, sizeof( sceInetAddress_t ) );
	if ( ret < 0 ) {
		return( ret );
	}

	ret = CallRpc(
		cd,
		RPC_SCEADDR2STR,
		net_buf,
		sizeof( u_int ) * 5,
		sizeof( u_int ) * 5 + len
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	ret = StrcpyS( name, buf, 5 );
	if ( ret < 0 ) {
		return( ret );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetAddress2Name( sceSifMClientData *cd, void *net_buf, int flags, char *name, int len, struct sceInetAddress *paddr, int ms, int nretry, ... )
{
	int 	*buf = ( int* )net_buf;
	va_list	ap;
	int 	ret;

	buf[ 0 ] = flags;
	buf[ 1 ] = len;
	buf[ 2 ] = ms;
	buf[ 3 ] = nretry;
	ret = MemcpyD( cd, buf, 4, paddr, sizeof( sceInetAddress_t ) );
	if ( ret < 0 ) {
		return( ret );
	}
	buf[ 8 ] = 0;
	if ( flags & sceInetN2AF_Abort ) {
		va_start( ap, nretry );
		buf[ 8 ] = va_arg( ap, int );
		va_end( ap );
	}
						 
	ret = CallRpc(
		cd,
		RPC_SCEADDR2NAME,
		net_buf,
		sizeof( u_int ) * 9,
		sizeof( u_int ) * 8 + len
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	ret = StrcpyS( name, buf, 8 );
	if ( ret < 0 ) {
		return( ret );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetCreate( sceSifMClientData *cd, void *net_buf, struct sceInetParam *param )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	( void )buf[ 0 ];
	ret = MemcpyD( cd, buf, 1, param, sizeof( struct sceInetParam ) );
	if ( ret < 0 ) {
		return( ret );
	}

	ret = CallRpc(
		cd,
		RPC_SCECREATE,
		net_buf,
		sizeof( u_int ) + sizeof( struct sceInetParam ),
		sizeof( u_int )
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	if ( buf[ 0 ] == 0 ) {
		return( sceLIBNETE_NG );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetOpen( sceSifMClientData *cd, void *net_buf, int cid, int ms )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	buf[ 0 ] = cid;
	buf[ 1 ] = ms;

	ret = CallRpc(
		cd,
		RPC_SCEOPEN,
		net_buf,
		sizeof( u_int ) * 2,
		sizeof( u_int )
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetClose( sceSifMClientData *cd, void *net_buf, int cid, int ms )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	buf[ 0 ] = cid;
	buf[ 1 ] = ms;

	ret = CallRpc(
		cd,
		RPC_SCECLOSE,
		net_buf,
		sizeof( u_int ) * 2,
		sizeof( u_int )
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetRecv( sceSifMClientData *cd, void *net_buf, int cid, void *ptr, int count, int *pflags, int ms )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	buf[ 0 ] = cid;
	if( pflags ) {
		buf[ 1 ] = *pflags;
	}
	else {
		buf[ 1 ] = 0;
	}
	buf[ 2 ] = count;
	buf[ 3 ] = ms;

	ret = CallRpc(
		cd,
		RPC_SCERECV,
		net_buf,
		sizeof( u_int ) * 4,
		sizeof( u_int ) * 2 + count
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}
	if ( buf[ 0 ] < 0 ) {
		return( buf[ 0 ] );
	}

	if ( pflags ) {
		*pflags = buf[ 1 ];
	}
	ret = MemcpyS( cd, ptr, buf, 2, buf[ 0 ] );
	if ( ret < 0 ) {
		return( ret );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetRecvFrom( sceSifMClientData *cd, void *net_buf, int cid, void *ptr, int count, int *pflags, struct sceInetAddress *addr, int *port, int ms )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	buf[ 0 ] = cid;
	if ( pflags ) {
		buf[ 1 ] = *pflags;
	}
	else {
		buf[ 1 ] = 0;
	}
	buf[ 2 ] = count;
	buf[ 3 ] = ms;

	ret = CallRpc(
		cd,
		RPC_SCERECVF,
		net_buf,
		sizeof( u_int ) * 4,
		sizeof( u_int ) * 7 + count
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}
	if ( buf[ 0 ] < 0 ) {
		return( buf[ 0 ] );
	}

	if ( pflags ) {
		*pflags = buf[ 1 ];
	}
	ret = MemcpyS( cd, addr, buf, 2, sizeof( sceInetAddress_t ) );
	if ( ret < 0 ) {
		return( ret );
	}
	if ( port ) {
		*port = buf[ 6 ];
	}
	ret = MemcpyS( cd, ptr, buf, 7, buf[ 0 ] );
	if ( ret < 0 ) {
		return( ret );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetSend( sceSifMClientData *cd, void *net_buf, int cid, const void *ptr, int count, int *pflags, int ms )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	buf[ 0 ] = cid;
	if ( pflags ) {
		buf[ 1 ] = *pflags;
	}
	else {
		buf[ 1 ] = 0;
	}
	buf[ 2 ] = count;
	buf[ 3 ] = ms;
	ret = MemcpyD( cd, buf, 4, ptr, count );
	if ( ret < 0 ) {
		return( ret );
	}

	ret = CallRpc(
		cd,
		RPC_SCESEND,
		net_buf,
		sizeof( u_int ) * 4 + count,
		sizeof( u_int ) * 2
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	if ( pflags ) {
		*pflags = buf[ 1 ];
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetSendTo( sceSifMClientData *cd, void *net_buf, int cid, void *ptr, int count, int *pflags, struct sceInetAddress *addr, int port, int ms )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	buf[ 0 ] = cid;
	if ( pflags ) {
		buf[ 1 ] = *pflags;
	}
	else {
		buf[ 1 ] = 0;
	}
	buf[ 2 ] = count;
	buf[ 3 ] = ms;
	buf[ 4 ] = port;
	ret = MemcpyD( cd, buf, 5, addr, sizeof( sceInetAddress_t ) );
	if ( ret < 0 ) {
		return( ret );
	}
	ret = MemcpyD( cd, buf, 9, ptr, count );
	if ( ret < 0 ) {
		return( ret );
	}

	ret = CallRpc(
		cd,
		RPC_SCESENDT,
		net_buf,
		sizeof( u_int ) * 9 + count,
		sizeof( u_int ) * 2
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	if ( pflags ) {
		*pflags = buf[ 1 ];
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetAbort( sceSifMClientData *cd, void *net_buf, int cid, int flags )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	buf[ 0 ] = cid;
	buf[ 1 ] = flags;

	ret = CallRpc(
		cd,
		RPC_SCEABORT,
		net_buf,
		sizeof( u_int ) * 2,
		sizeof( u_int )
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetControl( sceSifMClientData *cd, void *net_buf, int cid, int code, void *ptr, int len )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	buf[ 0 ] = cid;
	buf[ 1 ] = code;
	buf[ 2 ] = len;
	ret = MemcpyD( cd, buf, 3, ptr, len );
	if ( ret < 0 ) {
		return( ret );
	}

	ret = CallRpc(
		cd,
		RPC_SCECTL,
		net_buf,
		sizeof( u_int ) * 3 + len,
		sizeof( u_int ) * 3 + len
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	ret = MemcpyS( cd, ptr, buf, 3, len );
	if ( ret < 0 ) {
		return( ret );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetGetInterfaceList( sceSifMClientData *cd, void *net_buf, int *interface_id_list, int n )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	buf[ 0 ] = n;

	ret = CallRpc(
		cd,
		RPC_SCEGIFLIST,
		net_buf,
		sizeof( u_int ),
		sizeof( u_int ) + sizeof( int ) * n
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}
	if ( buf[ 0 ] < 0 ) {
		return( buf[ 0 ] );
	}

	ret = MemcpyS( cd, interface_id_list, buf, 1, sizeof( int ) * buf[ 0 ] );
	if ( ret < 0 ) {
		return( ret );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetInterfaceControl( sceSifMClientData *cd, void *net_buf, int interface_id, int code, void *ptr, int len )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	buf[ 0 ] = interface_id;
	buf[ 1 ] = code;
	buf[ 2 ] = len;
	ret = MemcpyD( cd, buf, 3, ptr, len );
	if ( ret < 0 ) {
		return( ret );
	}

	ret = CallRpc(
		cd,
		RPC_SCEIFCTL,
		net_buf,
		sizeof( u_int ) * 3 + len,
		sizeof( u_int ) * 3 + len
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	ret = MemcpyS( cd, ptr, buf, 3, len );
	if ( ret < 0 ) {
		return( ret );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetGetRoutingTable( sceSifMClientData *cd, void *net_buf, struct sceInetRoutingEntry *p, int n )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	buf[ 0 ] = n;

	ret = CallRpc(
		cd,
		RPC_SCEGETRT,
		net_buf,
		sizeof( u_int ),
		sizeof( u_int ) + sizeof( sceInetRoutingEntry_t ) * n
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}
	if ( buf[ 0 ] < 0 ) {
		return( buf[ 0 ] );
	}

	ret = MemcpyS( cd, p, buf, 1, sizeof( sceInetRoutingEntry_t ) * buf[ 0 ] );
	if ( ret < 0 ) {
		return( ret );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetGetNameServers( sceSifMClientData *cd, void *net_buf, struct sceInetAddress *paddr, int n )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	buf[ 0 ] = n;

	ret = CallRpc(
		cd,
		RPC_SCEGETNS,
		net_buf,
		sizeof( u_int ),
		sizeof( u_int ) + sizeof( sceInetAddress_t ) * n
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}
	if ( buf[ 0 ] < 0 ) {
		return( buf[ 0 ] );
	}

	ret = MemcpyS( cd, paddr, buf, 1, sizeof( sceInetAddress_t ) * buf[ 0 ] );
	if ( ret < 0 ) {
		return( ret );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetChangeThreadPriority( sceSifMClientData *cd, void *net_buf, int prio )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	buf[ 0 ] = prio;

	ret = CallRpc(
		cd,
		RPC_SCECHPRI,
		net_buf,
		sizeof( u_int ),
		sizeof( u_int )
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetGetLog( sceSifMClientData *cd, void *net_buf, char *log_buf, int len, int ms )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	buf[ 0 ] = len;
	buf[ 1 ] = ms;

	ret = CallRpc(
		cd,
		RPC_SCEGETLOG,
		net_buf,
		sizeof( u_int ) * 2,
		sizeof( u_int ) + len
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	ret = MemcpyS( cd, log_buf, buf, 1, len );
	if ( ret < 0 ) {
		return( ret );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetAbortLog( sceSifMClientData *cd, void *net_buf )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	ret = CallRpc(
		cd,
		RPC_SCEABORTLOG,
		net_buf,
		0,
		sizeof( u_int )
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	return( buf[ 0 ] );
}


/**************************************
 * INETCTL
 **************************************/
PUBLIC	int 	sceInetCtlUpInterface( sceSifMClientData *cd, void *net_buf, int id )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	buf[ 0 ] = id;

	ret = CallRpc(
		cd,
		RPC_UPIF,
		net_buf,
		sizeof( u_int ),
		sizeof( u_int )
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetCtlDownInterface( sceSifMClientData *cd, void *net_buf, int id )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	buf[ 0 ] = id;

	ret = CallRpc(
		cd,
		RPC_DOWNIF,
		net_buf,
		sizeof( u_int ),
		sizeof( u_int )
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetCtlSetAutoMode( sceSifMClientData *cd, void *net_buf, int f_auto )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	buf[ 0 ] = f_auto;

	ret = CallRpc(
		cd,
		RPC_SETAUTOMODE,
		net_buf,
		sizeof( u_int ),
		sizeof( u_int )
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceInetCtlGetState( sceSifMClientData *cd, void *net_buf, int id, int *pstate )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	buf[ 0 ] = id;
	if ( pstate ) {
		buf[ 1 ] = *pstate;
	}
	else {
		buf[ 1 ] = 0;
	}

	ret = CallRpc(
		cd,
		RPC_GETSTATE,
		net_buf,
		sizeof( u_int ) * 2,
		sizeof( u_int ) * 2
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	if ( pstate ) {
		*pstate = buf[ 1 ];
	}

	return( buf[ 0 ] );
}


/**************************************
 * Libnet
 **************************************/
PUBLIC	int 	sceLibnetInitialize( sceSifMClientData *cd, int buffersize, int stacksize, int priority )
{
	int 	i;

	if ( buffersize < sceLIBNET_MIN_NETBUF ) {
		return( sceLIBNETE_INSUFFICIENT_RESOURCES );
	}
	if ( buffersize != sceLIBNET_PAD( buffersize ) ) {
		return( sceLIBNETE_INSUFFICIENT_RESOURCES );
	}

	for ( ; ; ) {
		if ( sceSifMBindRpcParam( cd, LIBNET_SIFNUMBER, 0, buffersize, stacksize, priority ) < 0 ) {
			return( sceLIBNETE_NG );
		}
		if ( cd->serve != 0 ) {
			break;
		}
		i = 0x10000;
		while ( i-- );
	}

	return( sceLIBNETE_OK );
}

PUBLIC	int 	sceLibnetTerminate( sceSifMClientData *cd )
{
	int 	ret;
	int 	i;

	for ( ; ; ) {
		ret = sceSifMUnBindRpc( cd, 0 );
		if ( ret < 0 ) {
			return( sceLIBNETE_NG );
		}
		if ( ret == SIFM_UB_OK ) {
			break;
		}
		i = 0x10000;
		while ( i-- );
	}

	return( sceLIBNETE_OK );
}

PUBLIC	int 	sceLibnetRegisterHandler( sceSifMClientData *cd, void *net_buf )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	ret = CallRpc(
		cd,
		RPC_REGHANDLER,
		net_buf,
		0,
		sizeof( u_int )
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceLibnetUnregisterHandler( sceSifMClientData *cd, void *net_buf )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	ret = CallRpc(
		cd,
		RPC_UNREGHANDLER,
		net_buf,
		0,
		sizeof( u_int )
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	return( buf[ 0 ] );
}

PUBLIC	int 	sceLibnetSetConfiguration( sceSifMClientData *cd, void *net_buf, u_int env_addr )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	buf[ 0 ] = ( int )env_addr;

	ret = CallRpc(
		cd,
		RPC_SETCONFIG,
		net_buf,
		sizeof( u_int ),
		sizeof( u_int )
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	return( buf[ 0 ] );
}

PRIVATE	int 	WaitInterface( sceSifMClientData *cd, void *net_buf, int *if_id, int n, u_int fno )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;
	int 	i;

	ret = CallRpc(
		cd,
		fno,
		net_buf,
		0,
		sizeof( u_int ) + sizeof( int ) * n
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	for ( i = 0; i < n; i++ ) {
		if_id[ i ] = buf[ i + 1 ];
	}

	return( buf[ 0 ] );
}

PRIVATE	int 	WaitInterfaceAttached( sceSifMClientData *cd, void *net_buf, int *if_id, int n )
{
	int 	ret;

	ret = WaitInterface( cd, net_buf, if_id, n, RPC_WAITIFATTACHED );

	return( ret );
}

PRIVATE	int 	WaitInterfaceStarted( sceSifMClientData *cd, void *net_buf, int *if_id, int n )
{
	int 	ret;

	ret = WaitInterface( cd, net_buf, if_id, n, RPC_WAITIFSTARTED );

	return( ret );
}

PRIVATE	int 	GetInterfaceID( sceSifMClientData *cd, void *net_buf, int *if_id )
{
	int 	if_count;
	int 	id;
	int 	if_list[ 256 ];
	int 	flag;
	int 	ret;

	if_count = sceInetGetInterfaceList( cd, net_buf, if_list, sizeof( if_list ) / sizeof( int ) );
	if ( if_count < 0 ) {
		return( -1 );
	}
	if ( if_count == 0 ) {
		return( -2 );
	}

	for ( id = 0; id < if_count; id++ ) {
		ret = sceInetInterfaceControl( cd, net_buf, if_list[ id ], sceInetCC_GetFlags, &flag, sizeof( int ) );
		if ( ret < 0 ) {
			return( -1 );
		}
		if ( flag & sceInetDevF_Running ) {
			break;
		}
	}
	if ( id == if_count ) {
		return( -2 );
	}

	*if_id = if_list[ id ];

	return( 0 );
}

PRIVATE	int 	GetMyAddress( sceSifMClientData *cd, void *net_buf, int if_id, struct sceInetAddress *myaddr )
{
	int 	ret;

	ret = sceInetInterfaceControl( cd, net_buf, if_id, sceInetCC_GetAddress, myaddr, sizeof( sceInetAddress_t ) );

	return( ret );
}

PRIVATE	int 	WaitGetAddressAutoUpIf( sceSifMClientData *cd, void *net_buf, int *if_id, int n, sceInetAddress_t *addr )
{
	int 	ret;

	ret = GetInterfaceID( cd, net_buf, if_id );
	if ( ret < 0 ) {
		ret = WaitInterfaceStarted( cd, net_buf, if_id, n );
		if ( ret < 0 ) {
			return( ret );
		}
	}

	ret = GetMyAddress( cd, net_buf, *if_id, addr );
	if ( ret < 0 ) {
		return( ret );
	}

	return( sceLIBNETE_OK );
}

PRIVATE	int 	WaitGetAddressNoAutoUpIf( sceSifMClientData *cd, void *net_buf, int *if_id, int n, sceInetAddress_t *addr )
{
	int 	ret;

	ret = WaitInterfaceStarted( cd, net_buf, if_id, n );
	if ( ret < 0 ) {
		return( ret );
	}

	ret = GetMyAddress( cd, net_buf, *if_id, addr );
	if ( ret < 0 ) {
		return( ret );
	}

	return( sceLIBNETE_OK );
}

PUBLIC	int 	sceLibnetWaitGetInterfaceID( sceSifMClientData *cd, void *net_buf, int *if_id, int n )
{
	int 	ret;

	if ( if_id == NULL ) {
		return( sceLIBNETE_NG );
	}

	ret = GetInterfaceID( cd, net_buf, if_id );
	if ( ret < 0 ) {
		ret = WaitInterfaceAttached( cd, net_buf, if_id, n );
		if ( ret < 0 ) {
			return( ret );
		}
	}

	return( sceLIBNETE_OK );
}

PUBLIC	int 	sceLibnetWaitGetAddress( sceSifMClientData *cd, void *net_buf, int *if_id, int n, sceInetAddress_t *addr, u_int flags )
{
	int 	ret;

	if ( flags & sceLIBNETF_AUTO_UPIF ) {
		ret = WaitGetAddressAutoUpIf( cd, net_buf, if_id, n, addr );
	}
	else {
		ret = WaitGetAddressNoAutoUpIf( cd, net_buf, if_id, n, addr );
	}

	return( ret );
}

PUBLIC	int 	sceLibnetWaitGetInterfaceEvent( sceSifMClientData *cd, void *net_buf, int *id, int *type )
{
	int 	*buf = ( int* )net_buf;
	int 	ret;

	ret = CallRpc(
		cd,
		RPC_WAITGETIEV,
		net_buf,
		0,
		sizeof( u_int ) * 3
	);
	if ( ret < 0 ) {
		return( sceLIBNETE_RPC_CALL_FAILED );
	}

	if ( id != NULL ) {
		*id = buf[ 1 ];
	}
	if ( type != NULL ) {
		*type = buf[ 2 ];
	}

	return( buf[ 0 ] );
}

/*** End of file ***/

