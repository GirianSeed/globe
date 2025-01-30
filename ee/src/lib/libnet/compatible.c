/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                          Libnet Library
 *
 *      Copyright (C) 2000-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version   Date        Design   Log
 *  --------------------------------------------------------------------
 *       1.1.0     2002/01/24  ksh      sample to library
 *       1.2.0     2003/04/09  ksh      memalign to static data
 *                 2003/04/10  ksh      char * to const char *
 *                 2003/04/15  ksh      u_int *net_buf to void *net_buf
 *                 2003/04/18  ksh      sceNetcnfif{Sync, Init}() used
 *                 2003/04/30  ksh      sceNetcnfifLoadEntryAuto() used
 */

#include <eekernel.h>
#include <stdio.h>
#include <string.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <libnet.h>
#include <libnet/common.h>
#include <netcnfif.h>

#define	PRIVATE	static
#define	PUBLIC

#define WORKAREA_SIZE	( 0x1000 )
#define IOP_MOD_NETCNFIF	( "host0:/usr/local/sce/iop/modules/netcnfif.irx" )



PRIVATE	int 	LoadSetConfiguration( sceSifMClientData *cd, void *net_buf, sceNetcnfifData_t *p_data, const char *fname, const char *usr_name, u_int flags )
{
	sceNetcnfifArg_t	if_arg;
	int 	mode;
	int 	addr;
	int 	id;
	int 	ret;

	if ( flags & sceLIBNETF_USE_WAITSEMA ) {
		mode = 0;
	}
	else {
		mode = 1;
	}

	/* Prepare workarea in IOP. */
	sceNetcnfifAllocWorkarea( WORKAREA_SIZE );
	while( sceNetcnfifSync( mode ) );
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
	if ( flags & sceLIBNETF_IFC ) {
		if ( flags & sceLIBNETF_USE_WAITSEMA ) {
			ret = sceNetcnfifLoadEntryAuto(
				fname,
				sceNetcnfifArg_type_ifc | sceNETCNFIF_F_USE_WAITSEMA,
				usr_name,
				p_data
			);
		}
		else {
			ret = sceNetcnfifLoadEntryAuto(
				fname,
				sceNetcnfifArg_type_ifc,
				usr_name,
				p_data
			);
		}
		if ( ret < 0 ) {
			return( sceLIBNETE_NG );
		}
	}
	else {
		FlushCache( 0 );
		sceNetcnfifLoadEntry( fname, sceNetcnfifArg_type_net, usr_name, p_data );
		while( sceNetcnfifSync( mode ) );
		sceNetcnfifGetResult( &if_arg );
		if ( if_arg.data < 0 ) {
			return( sceLIBNETE_NG );
		}
	}

	/* Get a destination address of sceNetcnfifData(). */
	sceNetcnfifGetAddr();
	while( sceNetcnfifSync( mode ) );
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
	while( sceNetcnfifSync( mode ) );
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

PRIVATE	int 	DoLoadSetConfiguration( sceSifMClientData *cd, void *net_buf, const char *fname, const char *usr_name, u_int flags )
{
	static sceNetcnfifData_t	data __attribute__( ( aligned( 64 ) ) );
	int 	ret;

	if ( flags & sceLIBNETF_AUTO_LOADMODULE ) {
		if ( sceSifLoadModule( IOP_MOD_NETCNFIF, 0, NULL ) < 0 ) {
			return( sceLIBNETE_NG );
		}
	}

	ret = sceNetcnfifInit();
	if ( ret < 0 ) {
		return( sceLIBNETE_NG );
	}
	ret = LoadSetConfiguration( cd, net_buf, &data, fname, usr_name, flags );

	return( ret );
}

PUBLIC	int 	load_set_conf_extra( sceSifMClientData *cd, void *net_buf, const char *fname, const char *usr_name, u_int flags )
{
	return( DoLoadSetConfiguration( cd, net_buf, fname, usr_name, flags ) );
}

/*** End of file ***/

