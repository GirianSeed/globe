/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
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

#if ( !defined __LIBNET_SAMPLE_MAIN_H )
#define	__LIBNET_SAMPLE_MAIN_H

#if defined(__cplusplus)
extern "C" {
#endif

#define	PRIVATE	static
#define	PUBLIC
#define	EXTERN	extern
#define ERR_STOP	for ( ; ; )

typedef	struct connection_data {
    int 	exist;
	int 	fin;
	int 	sid;
	int 	tid;
	char*	stack;
} CONNECTIONDATA, *PCONNECTIONDATA;

#if defined(__cplusplus)
}
#endif

#endif	/*** __LIBNET_SAMPLE_MAIN_H ***/

/*** End of file ***/

