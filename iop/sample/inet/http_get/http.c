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
 */

#include <kernel.h>
#include <stdio.h>
#include <errno.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>
#include <sys/file.h>
#include <inet/inet.h>

#define	PRIVATE	static
#define	PUBLIC

#define BUFSIZE	( 1460 )
#define STACKSIZE	( 0x4000 )
#define PRIO	( USER_LOWEST_PRIORITY )

#define REQUEST_GET1 "GET / HTTP/1.1\r\nHOST: "
#define REQUEST_GET2 "\r\n\r\n"

PUBLIC	ModuleInfo Module = { "http_client_iop", ( 1 << 8 | 60 ) };

PRIVATE	char	buf[ BUFSIZE ];
PRIVATE	char	server_name[ 128 ];


PRIVATE	void	http_client( void )
{
	struct sceInetParam	inet_param;
	int 	ret;
	int 	cid;
	int 	flags;
	int 	fd;
	int 	total = 0;
	char	get_cmd[ 128 ];

	bzero( &inet_param, sizeof( inet_param ) );
	inet_param.type = sceINETT_CONNECT;
	inet_param.local_port = sceINETP_AUTO;
	ret = sceInetName2Address( 0, &inet_param.remote_addr, server_name, 0, 0 );
	if ( ret < 0 ) {
		printf( "sceInetName2Address() failed (%d)\n", ret );
		return;
	}
	inet_param.remote_port = 80;

	ret = sceInetAddress2String( buf, 128, &inet_param.remote_addr );
	if ( ret < 0 ) {
		printf( "sceInetAddress2String() failed (%d)\n", ret );
		return;
	}
	printf( "retrieved address for %s = %s\n", server_name, buf );

	cid = sceInetCreate( &inet_param );
	if ( cid < 0 ) {
		printf( "sceInetCreate() failed (%d)\n", cid );
		return;
	}
	
	ret = sceInetOpen( cid, -1 );
	if ( ret < 0 ) {
		printf( "sceInetOpen() failed (%d)\n", ret );
		sceInetClose( cid, -1 );
		return;
	}

	strcpy( get_cmd, REQUEST_GET1 );
	strcat( get_cmd, server_name );
	strcat( get_cmd, REQUEST_GET2 );

	flags = 0;
	ret = sceInetSend( cid, get_cmd, strlen( get_cmd ), &flags, -1 );

	fd = open( "host1:index.html", O_CREAT | O_TRUNC | O_RDWR );
	if ( fd < 0 ) {
		printf( "failed to open index.html\n" );
		sceInetClose( cid, -1 );
		return;
	}
 
	for ( ; ; ) {
		flags = 0;
		ret = sceInetRecv( cid, buf, BUFSIZE, &flags, -1 );
		if ( ret < 0 ) {
			printf( "sceInetRecv() failed (%d)\n", ret );
			break;
		}
		if ( ret > 0 ) {
			total += ret;
			printf( "received %d bytes, total %d\n", ret, total );
			write( fd, buf, ret );
		}
		if ( flags & sceINETF_FIN ) {
			break;
		}
	}

	close( fd );
	sceInetClose( cid, -1 );
	printf( "done.\n" );
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
	return( 0 );
}

int 	start( int ac, char* av[] )
{
	int 	ret;
	
	if( ac != 2 ) {
		printf( "usage: %s <URL>\n", av[0] );
		return( NO_RESIDENT_END );
	}
	strcpy( server_name, av[ 1 ] );

	ret = start_thread( http_client, STACKSIZE, TH_C, PRIO, NULL );
	if ( ret < 0 ) {
		return( NO_RESIDENT_END );
	}
	return( RESIDENT_END );
}

/*** End of file ***/

