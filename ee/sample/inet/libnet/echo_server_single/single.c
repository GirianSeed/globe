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
 *       1.00         2002/03/14  mka         first version
 *       1.01         2002/03/14  ksh
 */

#include <eekernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <libnet.h>
#include "single.h"

#define	PRIVATE	static
#define	PUBLIC

typedef struct conn {
	struct conn	*forw;
	struct conn	*back;
	int 	cid;
	int 	events;
	int 	revents;
	int 	off;
	int 	len;
	char	buf[ 1460 ];
} CONN, *PCONN;

PRIVATE	PCONN	g_conn_head = NULL;
PRIVATE	PCONN	g_conn_tail = NULL;
PRIVATE	int 	g_conn_count = 0;

PRIVATE	sceInetPollFd_t	*g_fds_area = 0;
PRIVATE	int 	g_fds_size = 0;


PRIVATE	void	*mem_alloc( int size )
{
	void	*p;

	p = malloc( size );
	if ( p != NULL ) {
		memset( p, 0, size );
	}
	return( p );
}

PRIVATE	void	mem_free( void *p )
{
	if ( p != NULL ) {
		free( p );
	}
}

PRIVATE	int 	add_connection( sceSifMClientData* pcd, u_int* net_buf, sceInetParam_t *iparam )
{
	const int 	size = sizeof( CONN );
	PCONN	p;
	int 	r;

	p = mem_alloc( size );
	if ( p == NULL ) {
		printf( "no space for new connection( size:%d )\n", size );
		return( -1 );
	}

	r = sceInetCreate( pcd, net_buf, iparam );
	if ( r < 0 ) {
		scePrintf( "sceInetCreate -> %d\n", r );
		mem_free( p );
		return( -1 );
	}
	p->cid = r;
	r = sceInetOpen( pcd, net_buf, p->cid, 0 );
	if ( r < 0 && r != sceINETE_TIMEOUT ) {
		scePrintf( "sceInetOpen -> %d\n", r );
		mem_free( p );
		return( -1 );
	}

	( ( p->back = g_conn_tail ) != NULL ) ? p->back->forw: g_conn_head = p;
	g_conn_tail = p;
	g_conn_count++;
	return( 0 );
}

PRIVATE	int 	close_connection( sceSifMClientData* pcd, u_int* net_buf, PCONN p )
{
	int 	r;

	r = sceInetClose( pcd, net_buf, p->cid, 0 );
	if ( r < 0 ) {
		if ( r != sceINETE_CONNECTION_DOES_NOT_EXIST ) {
			scePrintf( "sceInetClose -> %d\n", r );
		}
	}

	( p->back != NULL ) ? p->back->forw: g_conn_head = p->forw;
	( p->forw != NULL ) ? p->forw->back: g_conn_tail = p->back;
	g_conn_count--;
	mem_free( p );
	return( r );
}

PRIVATE	int 	tcp_echo_server( sceSifMClientData* pcd, u_int* net_buf, sceInetParam_t *iparam )
{
	sceInetPollFd_t	*fds;
	CONN	*p;
	CONN	*q;
	int 	flags;
	int 	r;
	int 	i;

	/* re-allocate fds area if current size is less than g_conn_count */
	if ( g_fds_size < g_conn_count ) {
		mem_free( g_fds_area );
	    g_fds_size = g_conn_count;
		g_fds_area = mem_alloc( g_fds_size * sizeof( *fds ) );
    	if ( g_fds_area == NULL ) {
			scePrintf( "no space for fds area\n" );
			return( -1 );
		}
	}

	/* setup fds area */
	fds = g_fds_area;
	p = g_conn_head;
	for ( i = 0; i < g_conn_count; i++ ) {
		if ( p == NULL ) {
			scePrintf( "? p->forw(%d)\n", i );
			return( -1 );
		}
		fds->cid = p->cid;
		fds->events = ( p->events ) ? p->events: sceINET_POLLIN;
		fds++;
		p = p->forw;
	}

	/* invoke sceInetPoll() */
	r = sceInetPoll( pcd, net_buf, g_fds_area, g_conn_count, -1 );
	if ( r < 0 ) {
		scePrintf( "sceInetPoll -> %d\n", r );
		return( -1 );
	}

	/* scan fds area and copy revents to connection list */
	fds = g_fds_area;
	p = g_conn_head;
	for ( i = 0; i < g_conn_count; i++ ) {
		if ( p == NULL ) {
			scePrintf( "? p->forw 2 (%d)\n", i );
			return( -1 );
		}
		p->revents = fds->revents;
		fds++;
		p = p->forw;
	}

	/* scan connection list and read/write/close it */
	for ( p = g_conn_head; p != NULL; p = q ) {
		q = p->forw;
		if ( p->revents & ( sceINET_POLLERR | sceINET_POLLHUP | sceINET_POLLNVAL ) ) {
			scePrintf( "sceInetPoll: revent=0x%x\n", p->revents );
			return( -1 );
		}
		else if ( p->revents & sceINET_POLLIN ) {
			if ( p->events & sceINET_POLLIN ) {
				/* readable */
				scePrintf( "receive\n" );
				flags = 0;
				r = sceInetRecv( pcd, net_buf, p->cid, p->buf, sizeof( p->buf ), &flags, 0 );
				if ( r < 0 ) {
					scePrintf( "sceInetRecv -> %d\n", r );
					return( -1 );
				}
				p->off = 0;
				p->len = r;
				if ( r > 0 ) {
					p->events = sceINET_POLLOUT;
				}
				else {
					p->events = sceINET_POLLFIN;
				}
			}
			else {
				/* accepting */
				scePrintf( "accept\n" );
				if ( add_connection( pcd, net_buf, iparam ) < 0 ) {
					return( -1 );
				}
				p->events = sceINET_POLLIN;
			}
		}
		else if ( p->revents & sceINET_POLLOUT ) {
			/* writeable */
			scePrintf( "send\n" );
			flags = 0;	/* URG=0 */
			r = sceInetSend( pcd, net_buf, p->cid, p->buf + p->off, p->len, &flags, 0 );
			if ( r < 0 ) {
				scePrintf( "sceInetSend -> %d\n", r );
				return( -1 );
			}
			p->off += r;
			p->len -= r;
			if ( r <= 0 ) {
				p->events = sceINET_POLLIN;
			}
		}
		else if ( p->events & sceINET_POLLFIN ) {
			/* fin sendable */
			scePrintf( "fin\n" );
			flags = 0;	/* ms=0 */
			r = sceInetControl( pcd, net_buf, p->cid, sceINETC_CODE_SEND_FIN, &flags, sizeof( flags ) );
			if ( r < 0 ) {
				scePrintf( "SEND_FIN -> %d\n", r );
				return( -1 );
			}
			p->events = sceINET_POLLCLS;
		}
		else if ( p->events & sceINET_POLLCLS ) {
			/* closeable */
			scePrintf( "close\n" );
			close_connection( pcd, net_buf, p );
		}
	}
	return( 0 );
}

PUBLIC	void	do_tcp_echo_server( sceSifMClientData* pcd, u_int* net_buf, int backlog )
{
	sceInetParam_t	inet_param;
	sceInetParam_t	*iparam = &inet_param;
	int 	r;
	int 	i;

	memset( iparam, 0, sizeof( *iparam ) );
	iparam->type = sceINETT_LISTEN;
	iparam->local_port = 7;
	 r = sceInetName2Address( pcd, net_buf, 0, &iparam->remote_addr, NULL, 0, 0 );
	if ( r < 0 ) {
		scePrintf( "sceInetName2Address -> %d\n", r );
		return;
	}
	iparam->remote_port = sceINETP_ANY;
	scePrintf( "tcp_echo_server: listening port %d backlog %d\n", inet_param.local_port, backlog );

	/* instead of listen for backlog */
	for ( i = 0; i < backlog; i++ ) {
		if ( add_connection( pcd, net_buf, iparam ) < 0 ) {
			return;
		}
	}

	for ( ; ; ) {
		if ( tcp_echo_server( pcd, net_buf, iparam ) < 0 ) {
			return;
		};
	}
}

/*** End of file ***/

