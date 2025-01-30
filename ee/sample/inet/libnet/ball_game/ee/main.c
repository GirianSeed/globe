/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                         - ball_game -
 *
 *                         Version 1.3.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2000-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                     Name : main.c
 *                     Description : ball game main().
 *
 *
 *       Version        Date           Design    Log
 *  --------------------------------------------------------------------
 *       1.0.0          Sep,29,2000    munekis   first verison
 *       1.1.0          Dec,22,2000    komaki    remake about if event
 *       1.2.0          Jan,11,2001    komaki    load inetctl.irx
 *       1.3.0          Oct,22,2001    ksh       down if & improvement
 *       1.4.0          Apr.02,2003    ksh       USB to HDD Ethernet etc.
 */

/**********************************************************************/
#if 0
#define USB_ETHERNET
#endif
#if 0
#define USB_ETHERNET_WITH_PPPOE
#endif
#if 1
#define HDD_ETHERNET
#endif
#if 0
#define HDD_ETHERNET_WITH_PPPOE
#endif
#if 0
#define MODEM
#endif

#if 1
#define UP_INTERFACE_AUTO
#endif

#include <eekernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <libnet.h>
#include <libmrpc.h>

#if	( defined( HDD_ETHERNET ) || defined( HDD_ETHERNET_WITH_PPPOE ) )
#include "poweroff.h"
#endif

#ifndef __CDROM__

#define	SCETOP            "host0:/usr/local/sce/"
#define	MAIN_MOD_ROOT     SCETOP "iop/modules/"
#define	SETAPP_ROOT       SCETOP "conf/neticon/japanese/"
#define	IOP_MOD_INET      MAIN_MOD_ROOT "inet.irx"
#define	IOP_MOD_AN986     MAIN_MOD_ROOT "an986.irx"
#define	IOP_MOD_USBD      MAIN_MOD_ROOT "usbd.irx"
#define	IOP_MOD_NETCNF    MAIN_MOD_ROOT "netcnf.irx"
#define	IOP_MOD_INETCTL   MAIN_MOD_ROOT "inetctl.irx"
#define	IOP_MOD_MSIFRPC   MAIN_MOD_ROOT "msifrpc.irx"
#define	IOP_MOD_DEV9      MAIN_MOD_ROOT "dev9.irx"
#define	IOP_MOD_SMAP      MAIN_MOD_ROOT "smap.irx"
#define	IOP_MOD_PPP       MAIN_MOD_ROOT "ppp.irx"
#define	IOP_MOD_PPPOE     MAIN_MOD_ROOT "pppoe.irx"
#define IOP_MOD_SIO2MAN   MAIN_MOD_ROOT "sio2man.irx"
#define IOP_MOD_PADMAN    MAIN_MOD_ROOT "padman.irx"
#define	IOP_MOD_LIBNET    MAIN_MOD_ROOT "libnet.irx"
#define	IOP_MOD_MODEMDRV  ""
#define	NET_DB            SCETOP "conf/net/net.db"
#define	INETCTL_ARG       "-no_auto" "\0" "-no_decode"
#define	NETCNF_ICON       SETAPP_ROOT "SYS_NET.ICO"
#define	NETCNF_ICONSYS    SETAPP_ROOT "icon.sys"
#define	NETCNF_ARG        "icon=" NETCNF_ICON "\0" "iconsys=" NETCNF_ICONSYS
#define	MODEMDRV_ARG      ""

#else                           

#include <libcdvd.h>
#define IOP_REPLACE         "cdrom0:\\IOPRP20.IMG;1"
#define IOP_MOD_MSIFRPC     "cdrom0:\\MSIFRPC.IRX;1"
#define IOP_MOD_LIBNET      "cdrom0:\\LIBNET.IRX;1"
#define IOP_MOD_INET        "cdrom0:\\INET.IRX;1"
#define IOP_MOD_AN986       "cdrom0:\\AN986.IRX;1"
#define IOP_MOD_USBD        "cdrom0:\\USBD.IRX;1"

#define IOP_MOD_INETCTL     "cdrom0:\\INETCTL.IRX;1"
#define IOP_MOD_IFCONF      "cdrom0:\\IFCONFIG.IRX;1"

#define MOD_SIO2MAN         "cdrom0:\\SIO2MAN.IRX;1"
#define MOD_PADMAN          "cdrom0:\\PADMAN.IRX;1"

#endif	/*** __CDROM__ ***/

#if	defined( USB_ETHERNET )
#define	USR_CONF_NAME     "Combination4"
#elif	defined( USB_ETHERNET_WITH_PPPOE )
#define	USR_CONF_NAME     "Combination5"
#elif	defined( HDD_ETHERNET )
#define	USR_CONF_NAME     "Combination6"
#elif	defined( HDD_ETHERNET_WITH_PPPOE )
#define	USR_CONF_NAME     "Combination7"
#elif	defined( MODEM )
#define	USR_CONF_NAME     ""
#endif


/**********************************************************************/
#include <math.h>
#include <libdev.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libdma.h>
#include <libpad.h>
#include "mathfunc.h"
#include "main.h"
#include "udp_game.h"
#include "common.h"

#define PRIVATE static
#define PUBLIC
#define	EXTERN	extern
#define ERR_STOP        for ( ; ; )


EXTERN	u_int	My_dma_start __attribute__((section(".vudata")));
EXTERN	u_int	My_send_texture __attribute__((section(".vudata")));

EXTERN	float	My_matrix[] __attribute__((section(".vudata")));
EXTERN	float	My_light_color_matrix[] __attribute__((section(".vudata")));

EXTERN	u_int	My_dma_next  __attribute__((section(".vudata")));
EXTERN	u_int	My_texture0  __attribute__((section(".vudata")));
EXTERN	u_int	My_position0  __attribute__((section(".vudata")));
EXTERN	u_int	My_position1  __attribute__((section(".vudata")));

EXTERN	float	My_rot_trans_matrix2[] __attribute__((section(".vudata")));
EXTERN	float	My_rot_trans_matrix3[] __attribute__((section(".vudata")));
EXTERN	float	My_rot_trans_matrix4[] __attribute__((section(".vudata")));

EXTERN	u_int	My_draw_ball[] __attribute__((section(".vudata")));
EXTERN	u_int	My_draw_shadow[] __attribute__((section(".vudata")));
EXTERN	u_int	My_buffer0[] __attribute__((section(".vudata")));
EXTERN	u_int	My_buffer4[] __attribute__((section(".vudata")));


/* ---- structures and defines for physics ---- */
PUBLIC	Object_t	spheres0[ N_SPHERES ];
PUBLIC	Object_t	spheres1[ N_SPHERES ];
PRIVATE	u_int	frame;

#define SCREEN_WIDTH	( 640.0f )
#define SCREEN_HEIGHT	( 224.0f )
#define SCREEN_OFFSET_X	( 1728.0f )
#define SCREEN_OFFSET_Y	( 1936.0f )

#define IMAGE_SIZE	( 256 )

#define VIEW_LENGTH	( 60.0f )
#define INIT_THETA	( 45.0f )
#define INIT_PHI	( -30.0f )
#define ANGLE_DELTA	( 1.0f )

typedef struct {
        sceGifTag	giftag;
        sceGsTest	gs_test;
        long	gs_test1addr;
        sceGsAlphaEnv	gs_alpha;
        sceGsTexEnv	gs_tex;
} TexEnv;

PRIVATE	sceSamp0FVECTOR	camera_p =   { 50.0f, -31.0f, 50.0f, 0.0f };
PRIVATE	sceSamp0FVECTOR	camera_zd =  { -50.0f, 31.0f, -50.0f, 1.0f };
PRIVATE	sceSamp0FVECTOR	camera_yd =  { 0.0f, 1.0f, 0.0f, 1.0f };
PRIVATE	sceSamp0FVECTOR	camera_rot = { 0.0f, 0.0f,   0.0f, 0.0f };
PRIVATE	sceSamp0FVECTOR	camera_int = { 0.0f, -5.0f, 0.0f, 0.0f };

PUBLIC	sceSamp0FVECTOR	light0 = {  0.2f,  1.5f, 0.5f, 0.0f };
PUBLIC	sceSamp0FVECTOR	light1 = {  1.5f, -0.5f, 0.5f, 0.0f };
PUBLIC	sceSamp0FVECTOR	light2 = { -1.5f, -0.5f, 0.5f, 0.0f };

PRIVATE	sceSamp0FVECTOR	color0  = { 0.9f, 0.9f, 0.9f, 1.0f };
PRIVATE	sceSamp0FVECTOR	color1  = { 0.0f, 0.0f, 0.0f, 1.0f };
PRIVATE	sceSamp0FVECTOR	color2  = { 0.0f, 0.0f, 0.0f, 1.0f };

PRIVATE	sceSamp0FVECTOR	ambient = { 0.2f, 0.2f, 0.2f, 0.0f };

PRIVATE	sceSamp0FMATRIX	world_view;
PRIVATE	sceSamp0FMATRIX	view_screen;
PRIVATE	sceSamp0FMATRIX	local_screen;

PRIVATE	sceSamp0FMATRIX	light_color;


/* --- global variables for pad --- */
PRIVATE	u_int	paddata;
PRIVATE	u_char	rdata[ 32 ];
PRIVATE	u_long128	pad_dma_buf[ scePadDmaBufferMax ] __attribute__((aligned (64)));

PRIVATE	sceGsDBuff	db;
PRIVATE	TexEnv	texenv;


PRIVATE void	LoadModule( const char *module, int args, const char *argp )
{
	while ( sceSifLoadModule( module, args, argp ) < 0 ) {
		scePrintf( "%s cannot be loaded.\n", module );
	}
}

PRIVATE	int	v_callback( int i )
{
	if( syncv & GRAPH_TH ) {
		iSignalSema( graph_sema );
	}
	ExitHandler();
	return 0;
}

PRIVATE	void	SetVu1PacketMatrix( void )
{
	/* My_matrix <- local_screen */
	My_matrix[ 0 ]  = local_screen[ 0 ][ 0 ];
	My_matrix[ 1 ]  = local_screen[ 1 ][ 0 ];
	My_matrix[ 2 ]  = local_screen[ 2 ][ 0 ];
	My_matrix[ 3 ]  = local_screen[ 3 ][ 0 ];

	My_matrix[ 4 ]  = local_screen[ 0 ][ 1 ];
	My_matrix[ 5 ]  = local_screen[ 1 ][ 1 ];
	My_matrix[ 6 ]  = local_screen[ 2 ][ 1 ];
	My_matrix[ 7 ]  = local_screen[ 3 ][ 1 ];

	My_matrix[ 8 ]  = local_screen[ 0 ][ 2 ];
	My_matrix[ 9 ]  = local_screen[ 1 ][ 2 ];
	My_matrix[ 10 ] = local_screen[ 2 ][ 2 ];
	My_matrix[ 11 ] = local_screen[ 3 ][ 2 ];

	My_matrix[ 12 ] = local_screen[ 0 ][ 3 ];
	My_matrix[ 13 ] = local_screen[ 1 ][ 3 ];
	My_matrix[ 14 ] = local_screen[ 2 ][ 3 ];
	My_matrix[ 15 ] = local_screen[ 3 ][ 3 ];

	/* My_light_color */
	My_light_color_matrix[ 0 ]  = light_color[ 0 ][ 0 ];
	My_light_color_matrix[ 1 ]  = light_color[ 1 ][ 0 ];
	My_light_color_matrix[ 2 ]  = light_color[ 2 ][ 0 ];
	My_light_color_matrix[ 3 ]  = light_color[ 3 ][ 0 ];

	My_light_color_matrix[ 4 ]  = light_color[ 0 ][ 1 ];
	My_light_color_matrix[ 5 ]  = light_color[ 1 ][ 1 ];
	My_light_color_matrix[ 6 ]  = light_color[ 2 ][ 1 ];
	My_light_color_matrix[ 7 ]  = light_color[ 3 ][ 1 ];

	My_light_color_matrix[ 8 ]  = light_color[ 0 ][ 2 ];
	My_light_color_matrix[ 9 ]  = light_color[ 1 ][ 2 ];
	My_light_color_matrix[ 10 ] = light_color[ 2 ][ 2 ];
	My_light_color_matrix[ 11 ] = light_color[ 3 ][ 2 ];

	My_light_color_matrix[ 12 ] = light_color[ 0 ][ 3 ];
	My_light_color_matrix[ 13 ] = light_color[ 1 ][ 3 ];
	My_light_color_matrix[ 14 ] = light_color[ 2 ][ 3 ];
	My_light_color_matrix[ 15 ] = light_color[ 3 ][ 3 ];
}

int 	main( int argc, char *argv[] )
{
	int 	odev;
	int 	result;
	int 	ret;
	u_int	toggle_start = 0;
	float	delta;
	float	theta;
	float	phi;
	int 	i;
	int 	k;
	int 	i_data;
	sceSifMClientData	cd;
	Object_t	*spheres;
	char	myname[ 128 ];
	char	myptrname[ 128 ];
	u_int	*net_buf;
	char	*com_stack;
	struct sceInetAddress	myaddr;
	int 	if_id;

	if( argc != 2 ) {
		scePrintf( "usage: %s <saddr>\n", argv[ 0 ] );
		ERR_STOP;
	}
	strcpy( paddr, argv[ 1 ] );


	/***************************************************************
		Preparation
	***************************************************************/
	sceSifInitRpc( 0 );

#ifdef __CDROM__
	sceCdInit( SCECdINIT );
	scePrintf( "IOP rebooting...\n" );
	while ( !sceSifRebootIop( IOP_REPLACE ) ) {
	}

	scePrintf( "IOP syncing...\n" );
	while ( !sceSifSyncIop() ) {
	}

	sceSifInitRpc( 0 );
	sceCdInit( SCECdINIT );
	sceCdMmode( SCECdCD );
#endif

	LoadModule( IOP_MOD_SIO2MAN, 0, NULL );
	LoadModule( IOP_MOD_PADMAN, 0, NULL );


	/***************************************************************
		Preparation( inet only )
	***************************************************************/
	LoadModule( IOP_MOD_INET, 0, NULL );
	LoadModule( IOP_MOD_NETCNF, sizeof( NETCNF_ARG ), NETCNF_ARG );
	LoadModule( IOP_MOD_INETCTL, sizeof( INETCTL_ARG ), INETCTL_ARG );

#if defined( USB_ETHERNET ) || defined( USB_ETHERNET_WITH_PPPOE )
	LoadModule( IOP_MOD_USBD, 0, NULL );
	LoadModule( IOP_MOD_AN986, 0, NULL );
#endif

#if defined( HDD_ETHERNET ) || defined( HDD_ETHERNET_WITH_PPPOE )
	LoadModule( IOP_MOD_DEV9, 0, NULL );
	LoadModule( IOP_MOD_SMAP, 0, NULL );
#endif
#if defined( USB_ETHERNET_WITH_PPPOE ) || defined( HDD_ETHERNET_WITH_PPPOE )
	LoadModule( IOP_MOD_PPP, 0, NULL );
	LoadModule( IOP_MOD_PPPOE, 0, NULL );
#endif

#if defined( MODEM )
	LoadModule( IOP_MOD_PPP, 0, NULL );
	LoadModule( IOP_MOD_USBD, 0, NULL );
	LoadModule( IOP_MOD_MODEMDRV, sizeof( MODEMDRV_ARG ), MODEMDRV_ARG );
#endif

	LoadModule( IOP_MOD_MSIFRPC, 0, NULL );
	LoadModule( IOP_MOD_LIBNET, 0, NULL );

#if defined( HDD_ETHERNET ) || defined( HDD_ETHERNET_WITH_PPPOE )
	PreparePowerOff();
#endif


	/***************************************************************
		Initialize libnet
	***************************************************************/
	sceSifMInitRpc( 0 );
	libnet_init( &cd, 2048, 8192,32 );
	if ( ( net_buf = ( u_int* )memalign( 64, RPCSIZE ) ) == NULL ) {
		scePrintf( "malloc failed.\n" );
		ERR_STOP;
	}
	if ( ( ret = reg_handler( &cd, net_buf ) ) < 0 ) {
		scePrintf( "reg_handler() failed.\n" );
		ERR_STOP;
	}

#ifdef  UP_INTERFACE_AUTO
	scePrintf( "up interface auto\n" );
	ret = load_set_conf( &cd, net_buf, NET_DB, USR_CONF_NAME );
	if ( ret < 0 ) {
		scePrintf( "load_set_conf() failed.\n" );
		ERR_STOP;
	}
	ret = wait_get_addr( &cd, net_buf, &if_id, &myaddr );
	if ( ret < 0 ) {
		scePrintf( "wait_get_addr() failed.\n" );
		ERR_STOP;
	}
#else
	scePrintf( "up interface no auto\n" );
	ret = load_set_conf_only( &cd, net_buf, NET_DB, USR_CONF_NAME );
	if ( ret < 0 ) {
		scePrintf( "load_set_conf_only() failed.\n" );
		ERR_STOP;
	}
	
	ret = get_interface_id( &cd, net_buf, &if_id );
	if ( ret < 0 ) {
		scePrintf( "get_interface_id()\n" );
		ERR_STOP;
	}
	up_interface( &cd, net_buf, 0 );

	ret = wait_get_addr_only( &cd, net_buf, &if_id, &myaddr );
	if ( ret < 0 ) {
		scePrintf( "wait_get_addr_only() failed.\n" );
		ERR_STOP;
	}
#endif  /*** UP_INTERFACE_AUTO ***/


	/***************************************************************
		Display IP address
	***************************************************************/
	ret = sceInetAddress2String( &cd, net_buf, myptrname, 128, &myaddr );
	if ( ret != sceINETE_OK ) {
		scePrintf( "invalid addr\n" );
		ERR_STOP;
	}
	ret = sceInetAddress2Name( &cd, net_buf, 0, myname, 128, &myaddr, 0, 0 );
	if ( ret != sceINETE_OK ) {
		scePrintf( "My address: %s\n", myptrname );
	}
	else {
		scePrintf( "My address: %s (%s)\n", myname, myptrname );
	}


	/***************************************************************
		Main
	***************************************************************/
	com_init();

	AddDmacHandler( DMAC_VIF1, get_pos_handler, 0 );
	EnableDmac( DMAC_VIF1 );

	if ( ( com_stack = ( char* )memalign( 16, STACKSIZE ) ) == NULL ) {
		scePrintf( "malloc failed.\n" );
		ERR_STOP;
	}
	CThread( communicate, com_stack, STACKSIZE, PRIO, NULL );


	/* ---- Graphics Initialization ---- */
	delta = DEG_TO_RAD( ANGLE_DELTA );
	theta = DEG_TO_RAD( INIT_THETA );
	phi = DEG_TO_RAD( INIT_PHI );

	sceGsResetGraph( 0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME );

	/* --- set double buffer --- */
	sceGsSetDefDBuff( &db, SCE_GS_PSMCT32, 640, 224, SCE_GS_ZGREATER, SCE_GS_PSMZ32, SCE_GS_CLEAR );
	*( u_long* )&db.clear0.rgbaq = SCE_GS_SET_RGBAQ( 0x10, 0x10, 0x10, 0x80, 0x3f800000 );
	*( u_long* )&db.clear1.rgbaq = SCE_GS_SET_RGBAQ( 0x10, 0x10, 0x10, 0x80, 0x3f800000 );

	/* --- set texture environment --- */
	sceGsSetDefTexEnv( &texenv.gs_tex, 0, 8768, IMAGE_SIZE / 64, SCE_GS_PSMCT32, 8, 8, 0, 0, 0, 0, 1 );
	SCE_GIF_CLEAR_TAG( &texenv.giftag );
	texenv.giftag.NLOOP = 9;
	texenv.giftag.EOP = 1;
	texenv.giftag.NREG = 1;
	texenv.giftag.REGS0 = 0xe;
	sceGsSetDefAlphaEnv( &texenv.gs_alpha, 0 );

	*( u_long* )&texenv.gs_alpha.alpha1 = SCE_GS_SET_ALPHA( 0, 1, 0, 1, 0 );
	*( u_long* )&texenv.gs_test = SCE_GS_SET_TEST( 0, 0, 0, 0, 0, 0, 1, 0x3 );
	texenv.gs_test1addr = ( long )SCE_GS_TEST_1;
	*( u_long* )&texenv.gs_tex.clamp1 = SCE_GS_SET_CLAMP( 0, 0, 0, 0, 0, 0 );
	*( u_long* )&texenv.gs_tex.tex11 = SCE_GS_SET_TEX1( 0, 0, 1, 1, 0, 0, 0 );

	FlushCache( 0 );
	sceGsPutDrawEnv( &texenv.giftag );
	sceGsSyncPath( 0, 0 );

	frame = 0;
	odev = !sceGsSyncV( 0 );

	/* --- pad --- */
	if ( scePadInit( 0 ) != 1 ) {
		scePrintf( "failed to scePadInit()\n" );
		ERR_STOP;
	}
	if ( scePadPortOpen( 0, 0, pad_dma_buf ) != 1 ) {
		scePrintf( "failed to scePadPortOpen()\n" );
		ERR_STOP;
	}

	/* init sphere array */
	init_sphere( spheres0 );
	init_sphere( spheres1 );


	/* --- send texture --- */
	*D1_QWC  = 0x00;
	*D1_TADR = ( u_int )&My_send_texture & 0x0fffffff;
	*D_STAT  = 2;
	FlushCache( 0 );
	*D1_CHCR = 1 | ( 1 << 2 ) | ( 0 << 4 ) | ( 1 << 6 ) | ( 1 << 7 ) | ( 1 << 8 );
	WaitSema( sync_sema );

	WaitSema( com_init_sema );

	sceGsSyncVCallback( v_callback );

	for ( ; ; ) {
		game_get_num( &cd, net_buf );

		sceGsSetHalfOffset( ( frame&1 ) ? &db.draw1 : &db.draw0, 2048, 2048, ( *GS_CSR >> 13 ) & 0x1 );
		sceGsSwapDBuff( &db, frame );
		sceGsSyncPath( 0, 0 );
    
		spheres = new_spheres;

		set_sphere_matrix( spheres );
    
		/* --- read pad --- */
		if ( ( result = scePadRead( 0, 0, rdata ) ) > 0 ) {
			paddata = 0xffff ^ ( ( rdata[ 2 ] << 8 ) | rdata[ 3 ] );
		}
		else {
			paddata = 0;
		}

		if ( ( paddata & SCE_PADstart ) && !toggle_start ) {
			toggle_start = 1;
    
			/* --- call reset message to server --- */
			scePrintf( "reset call invoked\n" );

			ret = game_reset( &cd, net_buf, &i_data );
			if ( ret < 0 ) {
				scePrintf( "call Game_Get_Num() failed.\n" );
				ERR_STOP;
			}
			if ( i_data != 7 ) {
				scePrintf( "Reset is not completed.\n" );
			}
		}
		else if ( !( paddata & SCE_PADstart ) ) {
			toggle_start = 0;
		}
    
		if ( paddata & SCE_PADLup ) {
			phi -= delta;
			if ( phi < -0.4999f * PI ) {
				phi = -0.4999f * PI;
			}
		}
		else if ( paddata & SCE_PADLdown ) {
			phi += delta;
			if ( phi > 0.0f ) {
				phi = 0.0f;
			}
		}

		/* --- camera rotation --- */
		if ( paddata & SCE_PADLright ) {
			theta += delta;
		}
		else if ( paddata & SCE_PADLleft ) {
			theta -= delta;
		}

		camera_p[ 0 ] = VIEW_LENGTH * cosf( phi ) * cosf( theta );
		camera_p[ 1 ] = VIEW_LENGTH * sinf( phi );
		camera_p[ 2 ] = VIEW_LENGTH * cosf( phi ) * sinf( theta );

		camera_zd[ 0 ] = camera_int[ 0 ] - camera_p[ 0 ]; 
		camera_zd[ 1 ] = camera_int[ 1 ] - camera_p[ 1 ]; 
		camera_zd[ 2 ] = camera_int[ 2 ] - camera_p[ 2 ]; 
    
		sceSamp0LightColorMatrix( light_color, color0, color1, color2, ambient );
    
		/* world -> view matrix */
		sceSamp0RotCameraMatrix( world_view, camera_p, camera_zd, camera_yd, camera_rot );
    
		/* view -> screen matrix */
		sceSamp0ViewScreenMatrix( view_screen, 512.0f, 1.0f, 0.47f, 2048.0f, 2048.0f, 1.0f, 16777215.0f, 1.0f,65536.0f );
    
		/* local -> screen matrix */
		sceSamp0MulMatrix( local_screen, view_screen, world_view );
    
		SetVu1PacketMatrix();
    
		/* --- send texture --- */
		*D1_QWC  = 0x00;
		*D1_TADR = ( u_int )&My_dma_start & 0x0fffffff;
		*D_STAT = 2;
		FlushCache( 0 );

		*D1_CHCR = 1 | ( 1 << 2 ) | ( 0 << 4 ) | ( 1 << 6 ) | ( 1 << 7 ) | ( 1<< 8 );
		WaitSema( sync_sema );

		for ( i = 0; i < n_all_clients; i++ ) {
			set_shadow_matrix( i, spheres );
    
			/* --- kick dma to draw shadow --- */
			*D1_QWC  = 0x00;
			*D1_TADR = ( u_int )&My_draw_shadow & 0x0fffffff;
			*D_STAT = 2;
			FlushCache( 0 );
			*D1_CHCR = 1 | ( 1 << 2 ) | ( 0 << 4 ) | ( 1 << 6 ) | ( 1 << 7 ) | ( 1 << 8 );
			WaitSema( sync_sema );
		}
    
		for ( i = 0; i < n_all_clients; i++ ) {
			set_sphere_matrix2( i, spheres );
			k = i / n_ball;
			k = k % 4;

			if ( k == 0 ) {
				My_buffer0[ 12 ] = 0x20011e40;
			}
			else if ( k == 1 ) {
				My_buffer0[ 12 ] = 0x20012240;
			}
			else if ( k == 2 ) {
				My_buffer0[ 12 ] = 0x20012640;
			}
			else if ( k == 3 ) {
				My_buffer0[ 12 ] = 0x20012a40;
			}
    
			/* --- kick dma to draw ball --- */
			*D1_QWC  = 0x00;
			*D1_TADR = ( u_int )&My_draw_ball & 0x0fffffff;
			*D_STAT  = 2;
			FlushCache( 0 );
			*D1_CHCR = 1 | ( 1 << 2 ) | ( 0 << 4 ) | ( 1 << 6 ) | ( 1 << 7 ) | ( 1 << 8 );
			WaitSema( sync_sema );
		}
		My_buffer0[12] = 0x20011a40;

		frame++;
		syncv |= GRAPH_TH;
		WaitSema( graph_sema );
		syncv &= ~GRAPH_TH;
	}

	/***************************************************************
		Terminate libnet
	***************************************************************/
	down_interface( &cd, net_buf, 0 );
	if ( ( ret = unreg_handler( &cd, net_buf ) ) < 0 ) {
		scePrintf( "unreg_handler() failed.\n" );
		ERR_STOP;
	}
	libnet_term( &cd );
	sceSifMExitRpc();

	return 0;
}

/*** End of file ***/

