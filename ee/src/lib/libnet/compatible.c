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
 *       1.1.0        2002/01/24  ksh         sample to library
 */

#include <eekernel.h>
#include <stdio.h>
#include <string.h>
#include <sifrpc.h>
#include <libnet.h>
#include <libnet/common.h>
#include <netcnfif.h>

#define	PRIVATE	static
#define	PUBLIC

#define WORKAREA_SIZE	( 0x1000 )
#define IOP_MOD_NETCNFIF	( "host0:/usr/local/sce/iop/modules/netcnfif.irx" )


PRIVATE	int 	LoadSetConfiguration( sceSifMClientData *cd, u_int *net_buf, sceNetcnfifData_t* p_data, char *fname, char *usr_name, u_int flags )
{
	sceNetcnfifArg_t	if_arg;
	int 	addr;
	int 	id;
	int 	ret;

	if ( flags & sceLIBNETF_AUTO_LOADMODULE ) {
		if ( sceSifLoadModule( IOP_MOD_NETCNFIF, 0, NULL ) < 0 ) {
			return( sceLIBNETE_NG );
		}
	}

	/* Setup Netcnfif. */
	sceNetcnfifSetup();

	/* Prepare workarea in IOP. */
	sceNetcnfifAllocWorkarea( WORKAREA_SIZE );
	while( sceNetcnfifCheck() );
	sceNetcnfifGetResult( &if_arg );
	if ( if_arg.data < 0 ) {
		return( sceLIBNETE_NG );
	}

	if ( flags & sceLIBNETF_DECODE ) {
		sceNetcnfifSetFNoDecode( sceNetcnfifArg_f_no_decode_off );
	}
	else {
		sceNetcnfifSetFNoDecode( sceNetcnfifArg_f_no_decode_on );
	}

	/* Get setting information. */
	FlushCache( 0 );
	sceNetcnfifLoadEntry( fname, sceNetcnfifArg_type_net, usr_name, p_data );
	while( sceNetcnfifCheck() );
	sceNetcnfifGetResult( &if_arg );
	if ( if_arg.data < 0 ) {
		return( sceLIBNETE_NG );
	}

	/* Get a destination address of sceNetcnfifData(). */
	sceNetcnfifGetAddr();
	while( sceNetcnfifCheck() );
	sceNetcnfifGetResult( &if_arg );
	if ( if_arg.data < 0 ) {
		return( sceLIBNETE_NG );
	}
	addr = if_arg.addr;

	/* Transfer sceNetcnfifData() to IOP. */
	id = sceNetcnfifSendIOP( ( u_int )p_data, ( u_int )addr, ( u_int )sizeof( sceNetcnfifData_t ) );
	if ( id == 0 ) {
		return( sceLIBNETE_NG );
	}
	while( sceNetcnfifDmaCheck( id ) );

	/* Set env. */
	sceNetcnfifSetEnv( sceNetcnfifArg_type_net );
	while( sceNetcnfifCheck() );
	sceNetcnfifGetResult( &if_arg );
	if ( if_arg.data < 0 ) {
		return( sceLIBNETE_NG );
	}
	addr = if_arg.addr;

	if ( flags & sceLIBNETF_AUTO_UPIF ) {
		ret = sceInetCtlSetAutoMode( cd, net_buf, !0 );
	}
	else {
		ret = sceInetCtlSetAutoMode( cd, net_buf, 0 );
	}
	if ( ret < 0 ) {
		return( ret );
	}

	ret = sceLibnetSetConfiguration( cd, net_buf, ( u_int )addr );
	if ( ret < 0 ) {
		return( ret );
	}

	return( sceLIBNETE_OK );
}

PRIVATE	int 	DoLoadSetConfiguration( sceSifMClientData *cd, u_int *net_buf, char *fname, char *usr_name, u_int flags )
{
	sceNetcnfifData_t*	p_data;
	int 	ret;

	p_data = ( sceNetcnfifData_t* )memalign( 64, sizeof( sceNetcnfifData_t ) );
	if ( p_data == NULL ) {
		return( sceLIBNETE_INSUFFICIENT_RESOURCES );
	}

	ret = LoadSetConfiguration( cd, net_buf, p_data, fname, usr_name, flags );

	free( p_data );

	return( ret );
}

PUBLIC	int 	load_set_conf_extra( sceSifMClientData *cd, u_int *net_buf, char *fname, char *usr_name, u_int flags )
{
	return( DoLoadSetConfiguration( cd, net_buf, fname, usr_name, flags ) );
}

/*** End of file ***/

