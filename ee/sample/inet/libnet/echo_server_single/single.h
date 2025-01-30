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
 *       1.0.0     2002/03/14     ksh         first
 */

#if ( !defined __SINGLE_H )
#define	__SINGLE_H

#include <stdio.h>
#include <libmrpc.h>

void	do_tcp_echo_server( sceSifMClientData* pcd, u_int* net_buf, int backlog );

#endif	/*** __SINGLE_H ***/

/*** End of file ***/

