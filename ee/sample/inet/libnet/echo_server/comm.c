/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/* 
 *                          Libnet Library
 *
 *      Copyright (C) 2000-2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version      Date        Design      Log
 *  --------------------------------------------------------------------
 *       1.1.0        2002/01/24  ksh         remake
 */

#include <eekernel.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <malloc.h>
#include <libnet.h>
#include <libmrpc.h>

#include "main.h"

#define RECVSIZE	( 1 )

EXTERN	int 	g_passive_sema;
EXTERN	int 	g_fin_sema;
EXTERN	CONNECTIONDATA	g_connection[];


PUBLIC	void	PassiveOpenThread( void* data )
{
	sceSifMClientData	cd;
	u_int*	net_buf;
	sceInetParam_t	param;
	int 	sid = *( int* )data;
	int 	cid;
	char	rdata[ RECVSIZE ];
	int 	flags;
	int 	ret;

	net_buf = ( u_int* )memalign( 64, RPCSIZE );
	if ( net_buf == NULL ) {
		scePrintf( "malloc failed.\n" );
		ERR_STOP;
	}

	ret = sceLibnetInitialize( &cd, sceLIBNET_BUFFERSIZE, sceLIBNET_STACKSIZE, sceLIBNET_PRIORITY );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetInitialize() failed.(%d)\n", ret );
		ERR_STOP;
	}

	memset( &param, 0, sizeof( sceInetParam_t ) );
	param.type = sceINETT_LISTEN;
	param.local_port = PASSIVE_PORT;
	ret = sceInetName2Address( &cd, net_buf, 0, &param.remote_addr, NULL, 0, 0 );
	if ( ret < 0 ) {
		scePrintf( "sceInetName2Address() failed.(%d)\n", ret );
		ERR_STOP;
	}
	param.remote_port = sceINETP_ANY;

	cid = sceInetCreate( &cd, net_buf, &param );
	if ( cid < 0 ) {
		scePrintf( "sceInetCreate() failed.(%d)\n", cid );
		ERR_STOP;
	}

	scePrintf( "cid = 0x%x, sid = %d\n", cid, sid );

	ret = sceInetOpen( &cd, net_buf, cid, -1 );
	if ( ret < 0 ) {
		scePrintf( "sceInetOpen() failed.(%d)\n", ret );
		SignalSema( g_passive_sema );
		goto end;
	}
	SignalSema( g_passive_sema );

	for ( ; ; ) {
		flags = 0;
		ret = sceInetRecv( &cd, net_buf, cid, rdata, RECVSIZE, &flags, -1 );
		if ( ret < 0 ) {
			scePrintf( "sceInetRecv() failed.(%d)\n", ret );
			break;
		}
		if ( flags & sceINETF_FIN ) {
			if( ret != 0 ) {
				scePrintf( "FIN is received with data.\n" );
			}
			break;
		}
		flags = 0;
		ret = sceInetSend( &cd, net_buf, cid, rdata, ret, &flags, -1 );
		if ( ret < 0 ) {
			scePrintf( "sceInetSend() failed.(%d)\n", ret );
			break;
		}
	}

end:
	ret = sceInetClose( &cd, net_buf, cid, -1 );
	if ( ret < 0 ) {
		scePrintf( "sceInetClose() failed.(%d)\n", ret );
	}
	else {
		scePrintf( "sceInetClose() done. sid = %d\n", sid );
	}

	ret = sceLibnetTerminate( &cd );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetTerminate() failed.(%d)\n", ret );
		ERR_STOP;
	}
	free( net_buf );
	g_connection[ sid ].fin = 1;
	SignalSema( g_fin_sema );
	SleepThread();
}

/*** End of file ***/

