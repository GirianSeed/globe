 /* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/* 
 *                          Libnet Library
 *
 *      Copyright (C) 2000-2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *  This header is used in Application(EE) & Libnet(EE) & Libnet(IOP).
 *
 *       Version      Date        Design      Log
 *  --------------------------------------------------------------------
 *       1.1.0        2002/01/24  ksh         sample to library
 */

#if ( !defined __LIBNET_LIBNETDEFS_H )
#define	__LIBNET_LIBNETDEFS_H

#if defined(__cplusplus)
extern "C" {
#endif

#define	sceLIBNET_BUFFERSIZE	( 2048 )
#define	sceLIBNET_STACKSIZE 	( 8192 )
#define	sceLIBNET_PRIORITY  	( 32 )
#define	sceLIBNET_PRIORITY_SIF 	( sceLIBNET_PRIORITY )
#define	sceLIBNET_PRIORITY_IOP	( 64 )

#define	sceLIBNET_MAX_INTERFACE	( 2 )

#define	sceLIBNETE_OK	( 0 )
#define	sceLIBNETE_NG	( -540 )
#define	sceLIBNETE_NO_RPC_CALL	( -541 )
#define	sceLIBNETE_RPC_CALL_FAILED	( -542 )
#define	sceLIBNETE_INSUFFICIENT_RESOURCES	( -543 )
#define	sceLIBNETE_RESERVED3	( -544 )
#define	sceLIBNETE_RESERVED2	( -545 )
#define	sceLIBNETE_USER 	( -546 )
#define	sceLIBNETE_USER1	( -546 )
#define	sceLIBNETE_USER2	( -547 )
#define	sceLIBNETE_USER3	( -548 )
#define	sceLIBNETE_USEREND	( -548 )
#define	sceLIBNETE_RESERVED1	( -549 )

#define	sceLIBNETF_DECODE	( 0x00000001 )
#define	sceLIBNETF_AUTO_UPIF	( 0x00000002 )
#define	sceLIBNETF_AUTO_LOADMODULE	( 0x00000004 )

#if defined(__cplusplus)
}
#endif

#endif	/*** __LIBNET_LIBNETDEFS_H ***/

/*** End of file ***/

