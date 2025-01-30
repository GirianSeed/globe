/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/* 
 *                        libinsck Library
 *
 *      Copyright (C) 2000-2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           2002/03/15      mka         first version
 *       1.01           2002/03/15      ksh
 */

#include <eekernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <libnet.h>
#include <errno.h>
#include <libinsck.h>

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

PRIVATE	int 	add_connection( int sock )
{
	const int 	size = sizeof( CONN );
	PCONN	p;
	int 	r;
	struct sockaddr_in	sin;
	int 	len;

	p = mem_alloc( size );
	if ( p == NULL ) {
		printf( "no space for new connection( size:%d )\n", size );
		return( -1 );
	}

	len = sizeof( sin );
	r = accept( sock, ( struct sockaddr * )&sin, &len );
	if ( r < 0 ) {
		scePrintf( "accept: errno=%d\n", sceInsockErrno );
		mem_free( p );
		return( -1 );
	}
	scePrintf( "connected from %s:%d\n", inet_ntoa( sin.sin_addr ), ntohs( sin.sin_port ) );
	p->cid = r;
	p->events = sceINET_POLLIN;

	( (p->back = g_conn_tail ) != NULL ) ? p->back->forw: g_conn_head = p;
	g_conn_tail = p;
	g_conn_count++;
	return( 0 );
}

PRIVATE	int 	close_connection( PCONN p )
{
	int 	r;

	r = shutdown( p->cid, SHUT_RDWR );
	if ( r < 0 ) {
		if ( sceInsockErrno != ENOTCONN ) {
			scePrintf( "shutdown: errno=%d\n", sceInsockErrno );
		}
	}

	( p->back != NULL ) ? p->back->forw: g_conn_head = p->forw;
	( p->forw != NULL ) ? p->forw->back: g_conn_tail = p->back;
	g_conn_count--;
	mem_free( p );
	return( r );
}

PRIVATE	int 	tcp_echo_server( int sock )
{
	sceInetPollFd_t	*fds;
	PCONN	p;
	PCONN	q;
	int 	r;
	int 	i;

	/* re-allocate fds area if current size is less than conn_count */
	if ( g_fds_size < 1 + g_conn_count ) {
		mem_free( g_fds_area );
		g_fds_size = 1 + g_conn_count;
		g_fds_area = mem_alloc( g_fds_size * sizeof( *fds ) );
		if ( g_fds_area == NULL ) {
			scePrintf( "no space for fds area\n" );
			return( -1 );
		}
	}

	/* setup fds area */
	fds = g_fds_area;
	p = g_conn_head;
	fds->cid = sock;
	fds->events = sceINET_POLLIN;
	fds++;
	for ( i = 0; i < g_conn_count; i++ ) {
		if ( p == NULL ) {
			scePrintf( "? p->forw(%d)\n", i );
			return( -1 );
		}
		fds->cid = p->cid;
		fds->events = p->events;
		fds++;
		p = p->forw;
	}

	/* invoke sceInsockPoll() */
	r = sceInsockPoll( g_fds_area, 1 + g_conn_count, -1 );
	if ( r < 0 ) {
		scePrintf( "sceInetPoll: errno=%d\n", sceInsockErrno );
		return( -1 );
	}

	/* scan fds area and copy revents to connection list */
	fds = g_fds_area + 1;
	p = g_conn_head;
	for ( i = 0; i < g_conn_count; i++ ) {
		if ( p == NULL ) {
			scePrintf( "? p->forw(%d)\n", i );
			return( -1 );
		}
		p->revents = fds->revents;
		fds++;
		p = p->forw;
	}

	if ( g_fds_area->revents & sceINET_POLLIN ) {
		add_connection( sock );
	}

	/* scan connection list and read/write/close it */
	for ( p = g_conn_head; p != NULL; p = q ) {
		q = p->forw;
		if ( p->revents & ( sceINET_POLLERR | sceINET_POLLHUP | sceINET_POLLNVAL ) ) {
			scePrintf( "sceInetPoll: revent=0x%x\n", p->revents );
			return( -1 );
		}
		else if ( p->revents & sceINET_POLLIN ) {
			/* readable */
			r = recv( p->cid, p->buf, sizeof( p->buf ), 0 );
			if ( r < 0 ) {
				scePrintf( "recv: errno=%d\n", sceInsockErrno );
				return( -1 );
			}
			p->off = 0;
			p->len = r;
			if ( p->len > 0 ) {
				p->events = sceINET_POLLOUT;
			}
			else {
				p->events = sceINET_POLLFIN;
			}
		}
		else if ( p->revents & sceINET_POLLOUT ) {
			/* writeable */
			r = send( p->cid, p->buf + p->off, p->len, 0 );
			if ( r < 0 ) {
				scePrintf( "send: errno=%d\n", sceInsockErrno );
				return( -1 );
			}
			p->off += r;
			p->len -= r;
			if ( p->len <= 0 ) {
				p->events = sceINET_POLLIN;
			}
		}
		else if ( p->events & sceINET_POLLFIN ) {
			/* fin sendable */
			r = sceInsockSendFin( p->cid, 0 );
			if ( r < 0 ) {
				scePrintf( "sceInsockSendFin -> %d\n", r );
				return( -1 );
			}
			p->events = sceINET_POLLCLS;
		}
		else if ( p->events & sceINET_POLLCLS ) {
			/* closeable */
			close_connection( p );
		}
	}
	return( 0 );
}

PUBLIC	void	do_tcp_echo_server( int backlog )
{
	struct sockaddr_in	sin;
	int 	sock;

	sock = socket( AF_INET, SOCK_STREAM, 0 );
	if ( sock < 0 ) {
		scePrintf( "socket: errno=%d\n", sceInsockErrno );
		return;
	}

	memset( &sin, 0, sizeof( sin ) );
	sin.sin_family = AF_INET;
	sin.sin_port = htons( 7 );
	scePrintf( "echo_server: listening port %d backlog %d\n", htons( sin.sin_port ), backlog );

	if ( bind( sock, ( struct sockaddr * )&sin, sizeof( sin ) ) < 0 ) {
		scePrintf( "bind: errno=%d\n", sceInsockErrno );
		return;
	}
	if ( listen( sock, backlog ) < 0 ) {
		scePrintf( "listen: errno=%d\n", sceInsockErrno );
		return;
	}

	for ( ; ; ) {
		if ( tcp_echo_server( sock ) < 0 ) {
			return;
		}
	}
}

/*** End of file ***/

