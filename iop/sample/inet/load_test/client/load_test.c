/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                         Version <1.10>
 *                           Shift-JIS
 *
 *      Copyright (C) 2000-2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           May,23,2001     komaki      first version
 *       1.10           Dec,27,2001     ksh         irx poweroff
 *       1.20           Dec,05,2002     ksh         CpuSuspendIntr & free added
 */

#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <inet/inet.h>
#include <inet/in.h>

#define	PRIVATE	static
#define	PUBLIC

#define BUFSIZE	( 2048 )
#define STACKSIZE	( 0x4000 )
#define PRIO	( USER_LOWEST_PRIORITY )
#define SERVER_PORT	( 9012 )

typedef	struct connection_data {
	int 	sema;
	int 	fid;
} CONNECTIONDATA, *PCONNECTIONDATA;

PUBLIC	ModuleInfo Module = { "load_test_iop", ( 1 << 8 | 87 ) };

PRIVATE	int 	pktsize = 0;
PRIVATE	int 	th_num = 0;
PRIVATE	char	server_name[ 128 ];
PRIVATE	PCONNECTIONDATA	connection;


PRIVATE	int 	recvmsg( int cid, u_char *ptr, int count, int *pflags, int ms )
{
	int 	left = count;
	int 	len;

	while ( left > 0 ) {
		len = sceInetRecv( cid, ptr, count, pflags, ms );
		if ( len < 0 ) {
			return( len );
		}
		if ( len == 0 ) {
			break;
		}
		left -= len;
		ptr += len;
	}

	return( count - left );
}

PRIVATE	void	make_connection( int fid )
{
	int 	w = 1500;
	int 	ret;
	int 	cid;
	struct sceInetParam	param;
	u_char	trans_buf[ BUFSIZE ];
	char	mem[ BUFSIZE ];
	int 	ps;
	int 	flag;

	memset( &param, 0, sizeof( struct sceInetParam ) );

	param.type = sceINETT_CONNECT;
	param.local_port = sceINETP_AUTO;
	ret = sceInetName2Address( 0, &param.remote_addr, server_name, 0, 0 );
	if ( ret < 0 ) {
		printf( "sceInetName2Address() failed (%d)\n", ret );
		return;
	}
	param.remote_port = SERVER_PORT;

	cid = sceInetCreate( &param );
	if ( cid <= 0 ) {
		printf( "sceInetCreate() failed (%d)\n", cid );
		return;
	}

	ret = sceInetOpen( cid, -1 );
	if ( ret != sceINETE_OK ) {
		printf( "sceInetOpen() failed.(%d)\n", ret );
		sceInetClose( cid, -1 );
		return;
	}

	ps = htonl( pktsize );
	flag = 0;
	ret = sceInetSend( cid, &ps, 4, &flag, -1 );

	for ( ; w > 0; w-- ) {
		if ( w % 100 == 0 ) {
			printf( "thread %d : w = %d\n", fid, w );
		}

		memcpy( mem, trans_buf, pktsize );

		flag = 0;
		ret = sceInetSend( cid, trans_buf, pktsize, &flag, -1 );
		if (ret != pktsize ) {
			printf( "write count is not %d byte, but %d byte\n", pktsize, ret );
		}

		flag = 0;
		ret = recvmsg( cid, trans_buf, pktsize, &flag, -1 );
		if ( flag & sceINETF_FIN ) {
			break;
		}
		if ( ret != pktsize ) {
			printf( "read count is not %d byte, but %d byte\n", pktsize, ret );
		}

		if ( memcmp( trans_buf, mem, pktsize ) ) {
			printf( "sent data and received data is not same.\n" );
		}
	}

	sceInetClose( cid, -1 );
	printf( "done %d\n", fid );
}

PRIVATE	void	load_test_each_thread( void *data )
{
	int 	fid = *( int* )data;

	make_connection( fid );
	SignalSema( connection[fid].sema );
}

PRIVATE	int 	start_thread( void *func, int size, int attr, int prio, void *argp )
{
	struct ThreadParam	tparam;
	int 	tid;
	int 	ret;

	tparam.attr = attr;
	tparam.entry = func;
	tparam.initPriority = prio;
	tparam.stackSize = size;
	tparam.option = 0;

	tid = CreateThread( &tparam );
	if ( tid <= 0 ) {
		printf( "CreateThread() failed (%d)\n", tid );
		return( -1 );
	}

	ret = StartThread( tid, ( u_long )argp );
	if ( ret != KE_OK ) {
		printf( "StartThread() failed (%d)\n", ret );
		DeleteThread( tid );
		return( -1 );
	}
	return( tid );
}

PRIVATE	void	load_test_thread( u_long arg )
{
	struct SemaParam	sparam;
	int 	old;
	int 	i;

	printf( "pktsize = %d, th_num = %d\n", pktsize, th_num );

	CpuSuspendIntr( &old );
	connection = ( PCONNECTIONDATA )AllocSysMemory( 0, sizeof( CONNECTIONDATA ) * th_num, NULL );
	CpuResumeIntr( old );
	if ( connection == NULL ) {
		printf( "AllocSysMemory() failed.\n" );
		return;
	}

	sparam.initCount = 0;
	sparam.maxCount = 1;
	sparam.attr = SA_THFIFO;
	for ( i = 0; i < th_num; i++ ) {
		connection[ i ].sema = CreateSema( &sparam );
	}

	for ( i = 0; i < th_num; i++ ) {
		connection[ i ].fid = i;
		start_thread( load_test_each_thread, STACKSIZE, TH_C, PRIO, &connection[ i ].fid );
	}

	for ( i = 0; i < th_num; i++ ) {
		WaitSema( connection[i].sema );
		DeleteSema( connection[i].sema );
	}
	
	CpuSuspendIntr( &old );
	FreeSysMemory( connection );
	CpuResumeIntr( old );
}

int 	start( int argc, char* argv[] )
{
	int 	ret;

	if ( argc != 4 ) {
		printf( "usage: %s <saddr> <th_num> <pktsize>\n", argv[ 0 ] );
		return( NO_RESIDENT_END );
	}

	strcpy( server_name, argv[ 1 ] );

	th_num = atoi( argv[ 2 ] );
	if ( th_num <= 0 ) {
		printf( "th_num = %d failed\n", th_num );
		return( NO_RESIDENT_END );
	}

	pktsize = atoi( argv[ 3 ] );
	if ( pktsize <= 0 || pktsize > BUFSIZE ) {
		printf( "pktsize = %d failed\n", pktsize );
		return( NO_RESIDENT_END );
	}

	ret = start_thread( load_test_thread, STACKSIZE, TH_C, PRIO, NULL );
	if( ret < 0 ) {
		return( NO_RESIDENT_END );
	}
	return( RESIDENT_END );
}

/*** End of file ***/

