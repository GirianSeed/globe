/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *		 Controller Library 2 Vibration Sample
 *
 *                          Version 1.00
 *	                    Shift-JIS
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *	                 All Rights Reserved.
 *
 *                              main.c
 *
 *      Version         Date            Design     Log
 *  --------------------------------------------------------------------
 *      1.00            Jun,14,2001	nozomu     Initial
 *	
 */

#define NUM	2

#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libdma.h>
#include <libpkt.h>
#include <sifdev.h>
#include <libdev.h>
#include <sifrpc.h>
#include <libdbc.h>
#include <libpad2.h>
#include <libvib.h>

#define SIO2MAN	"host0:/usr/local/sce/iop/modules/sio2man.irx"
#define DBCMAN	"host0:/usr/local/sce/iop/modules/dbcman.irx"
#define SIO2D	"host0:/usr/local/sce/iop/modules/sio2d.irx"
#define DS2U	"host0:/usr/local/sce/iop/modules/pad2/ds2u.irx"

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	224
#define OFFX		(((4096-SCREEN_WIDTH)/2)<<4)
#define OFFY		(((4096-SCREEN_HEIGHT)/2)<<4)

/* IOPインタフェース用DMAバッファ */
static u_long128 pad_buf[ NUM ][SCE_PAD2_DMA_BUFFER_MAX] __attribute__((aligned (64)));

/* 描画環境 */
#define SUNIT		0x01
#define PACKETSIZE	(0x100*SUNIT)
static u_long128 packet_buff[2][PACKETSIZE];
static sceGsDBuff db;

/* フォント表示用 */
static int console;
extern void sceDevConsLocate(int cd, u_int lx, u_int ly);

/* GS初期化 */
static void Gs_Initialize(void)
{
	int sindex;
	sceVif1Packet packet[2];
	sceDmaEnv env;
	sceDmaChan *p1;
	u_long giftagAD[2] = { SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1),
			 0x000000000000000eL };

	sceDevVif0Reset();
	sceDevVu0Reset();
	sceGsResetPath();
	sceDmaReset(1);

	sceVif1PkInit(&packet[0], (u_long128 *)packet_buff[0]);
	sceVif1PkInit(&packet[1], (u_long128 *)packet_buff[1]);

	sceDmaGetEnv(&env);
	env.notify = 1<<SCE_DMA_VIF1; /* notify channel */
	sceDmaPutEnv(&env);

	p1 = sceDmaGetChan(SCE_DMA_VIF1);
	p1->chcr.TTE = 1;

	sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
	sceGsSetDefDBuff(&db, SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT,
		SCE_GS_ZGEQUAL, SCE_GS_PSMZ24, SCE_GS_CLEAR);

	db.clear0.rgbaq.R = 0x40;
	db.clear0.rgbaq.G = 0x40;
	db.clear0.rgbaq.B = 0x80;

	db.clear1.rgbaq.R = 0x40;
	db.clear1.rgbaq.G = 0x40;
	db.clear1.rgbaq.B = 0x80;

	sindex = 0;
	sceVif1PkReset(&packet[sindex]);
	sceVif1PkCnt(&packet[sindex], 0);
	sceVif1PkOpenDirectCode(&packet[sindex], 0);
	sceVif1PkOpenGifTag(&packet[sindex], *(u_long128*)&giftagAD[0]);

	sceVif1PkReserve(&packet[sindex],
		sceGsSetDefAlphaEnv((sceGsAlphaEnv *)packet[sindex].pCurrent,0) * 4);

	sceVif1PkCloseGifTag(&packet[sindex]);
	sceVif1PkCloseDirectCode(&packet[sindex]);
	sceVif1PkEnd(&packet[sindex], 0);
	sceVif1PkTerminate(&packet[sindex]);

	FlushCache(0);
	sceDmaSend(p1,(u_int *)(((u_int)packet[sindex].pBase)));
	sceGsSyncPath(0,0);

	/* display next in odd field when interlace */
	while(!sceGsSyncV(0));
}

int main(void)
{
	int i;
	int port;
	int frame;

	int socket_number[ NUM ];
	int state[ NUM ];
	int pad_state[ NUM ];
	int rdata_length[ NUM ];
	unsigned char rdata[ NUM ][ SCE_PAD2_BUTTON_DATA_SIZE ];
	int profile_size[ NUM ];
	unsigned char profile[ NUM ][ SCE_PAD2_BUTTON_PROFILE_SIZE ];
	int vib_profile_size[ NUM ];
	unsigned char vib_profile[ NUM ][ SCE_VIB_PROFILE_SIZE ];
	unsigned char out_profile[ NUM ][ SCE_VIB_PROFILE_SIZE ];
	unsigned char out_data[ NUM ][ SCE_VIB_DATA_SIZE ];

	unsigned char mes_data[ NUM ][ 80 ];
	unsigned char mes_prodata[ NUM ][ 80 ];
	unsigned char mes_vib_prodata[ NUM ][ 80 ];
	unsigned char mes_state[ NUM ][ SCE_PAD2_STATESTR_SIZE ];

	sceSifInitRpc(0);

	/* sio2man.irxをロードする */
	while( sceSifLoadModule( SIO2MAN, 0, NULL ) < 0 )
	{
	 	printf("Can't load module sio2man\n" );
		return 0;
	}

	/* dbcman.irxをロードする */
	while( sceSifLoadModule( DBCMAN, 0, NULL) < 0 )
	{
	    printf("Can't load module dbcman\n");
	}

	/* sio2d.irxをロードする */
	while( sceSifLoadModule( SIO2D, 0, NULL) < 0 )
	{
	    printf("Can't load module sio2d\n");
	}

	/* コントローラドライバーモジュールをロードする */
	for( i = 0; i < NUM; i++ )
	{
		while( sceSifLoadModule( DS2U, 0, NULL ) < 0 )
		{
		    printf("Can't load module ds2man\n");
		}
	}

	/* GS初期化 */
	Gs_Initialize();

	/* フォントシステム初期化 */
	sceDevConsInit();
	console = sceDevConsOpen(OFFX + (16<<4), OFFY + (24<<4), 75, 30);
	sceDevConsClear(console);

	/* DBCライブラリ初期化 */
	sceDbcInit();

	/* PAD2ライブラリ初期化 */
	scePad2Init(0);

	/* バーチャルソケットの作成 */
	for( port = 0; port < NUM; port++ )
	{
		socket_number[ port ] = scePad2CreateSocket( NULL, pad_buf[ port ] );
		if( socket_number[ port ] < 0 )
		{
			return 0;
		}

		state[ port ] = 0;
		strcpy( mes_data[ port ], "");
		strcpy( mes_prodata[ port ], "" );
		strcpy( mes_vib_prodata[ port ], "" );
		strcpy( mes_state[ port ], "" );
	}

	frame = 0;
	sceGsSyncPath(0,0);

	while(1)
	{
		sceGsSwapDBuff(&db, frame);
		++frame;

		for( port = 0; port < NUM; port++ )
		{
			/* コントローラの状態チェック */
			pad_state[ port ] = scePad2GetState( socket_number[ port ] );
			scePad2StateIntToStr( pad_state[ port ], mes_state[ port ] );

			if( pad_state[ port ] == scePad2StateStable )
			{
				switch( state[ port ] )
				{
					case 0:
					/* ボタンプロファイルの取得 */
					profile_size[ port ] = 
						scePad2GetButtonProfile( socket_number[ port ], profile[ port ] );

					strcpy( mes_prodata[ port ], "");
					for( i = 0; i < profile_size[ port ]; i++ )
					{
						sprintf( mes_prodata[ port ]+ strlen( mes_prodata[ port ]),
										"%02x ", profile[ port ][ i ] );
					}

					/* 振動プロファイルの取得 */
					vib_profile_size[ port ] = 
						sceVibGetProfile( socket_number[ port ], vib_profile[ port ] );

					if( vib_profile_size[ port ] > 0 )
					{
						strcpy( mes_vib_prodata[ port ], "");
						for( i = 0; i < vib_profile_size[ port ]; i++ )
						{
							sprintf( mes_vib_prodata[ port ]+
								strlen( mes_vib_prodata[ port ]),
									"%02x ", vib_profile[ port ][ i ] );
						}
					}
					state[ port ]++;
					break;

					case 1:
					/* ボタン情報の取得 */
					rdata_length[ port ] = 
						scePad2Read( socket_number[ port ], rdata[ port ] );

					strcpy( mes_data[ port ], "");
					for( i = 0; i < rdata_length[ port ]; i++ )
					{
						sprintf( mes_data[ port ]+ strlen( mes_data[ port ]),
									"%02x ", rdata[ port ][ i ] );
					}

					if ( scePad2GetButtonInfo( socket_number[ port ], 
									rdata[ port ], SCE_PAD2_UP ) )
					{
						/* 小モータ回転 */
						out_profile[ port ][ 0 ] = 0x01;
						out_data[ port ][ 0 ] = 0x01;
						sceVibSetActParam( socket_number[ port ], 
									SCE_VIB_PROFILE_SIZE, out_profile[ port ],
									1, out_data[ port ] );
					}
					else if( scePad2GetButtonInfo( socket_number[ port ],
									rdata[ port ], SCE_PAD2_DOWN )  )
					{
						/* 小モータ停止 */
						out_profile[ port ][ 0 ] = 0x01;
						out_data[ port ][ 0 ]  = 0x00;
						sceVibSetActParam( socket_number[ port ],
									SCE_VIB_PROFILE_SIZE, out_profile[ port ],
									1, out_data[ port ] );
					}
					else if( scePad2GetButtonInfo( socket_number[ port ],
									rdata[ port ], SCE_PAD2_LEFT ) )
					{
						/* 大モータ回転 */
						out_profile[ port ][ 0 ] = 0x02;
						out_data[ port ][ 0 ] = 200;
						sceVibSetActParam( socket_number[ port ],
									SCE_VIB_PROFILE_SIZE, out_profile[ port ],
									1, out_data[ port ] );
					}
					else if( scePad2GetButtonInfo( socket_number[ port ], 
									rdata[ port ], SCE_PAD2_RIGHT ) )
					{
						/* 大モータ停止 */
						out_profile[ port ][ 0 ] = 0x02;
						out_data[ port ][ 0 ] = 0x00;
						sceVibSetActParam( socket_number[ port ],
									SCE_VIB_PROFILE_SIZE, out_profile[ port ],
									1, out_data[ port ] );
					}
					break;
				}
			}
			else
			{
				state[ port ] = 0;

				strcpy( mes_data[ port ], "");
				strcpy( mes_prodata[ port ], "" );
				strcpy( mes_vib_prodata[ port ], "" );
			}
		}

		/* フォント表示 */
		sceDevConsClear(console);
		sceDevConsLocate(console, 0, 0);
		sceDevConsPrintf(console, "LIBPAD2 SAMPLE\n" );
		sceDevConsPrintf(console, "------------------------------------------------------------------------\n" );
		sceDevConsPrintf(console, "\n" );

		for( port = 0; port < NUM; port++ )
		{
			sceDevConsAttribute( console, 5 );
			sceDevConsPrintf( console, "  SOCKET NUM[%d]\n", socket_number[ port ] );
			sceDevConsPrintf( console, "  PROFILE		: %s\n", mes_prodata[ port ] );
			sceDevConsPrintf( console, "  VIB_PROFILE	: %s\n", mes_vib_prodata[ port ] );
			sceDevConsAttribute( console, 7 );
			sceDevConsPrintf( console, "  STATUS : %s\n", mes_state[ port ] );
			sceDevConsPrintf( console, "PAD DATA : %s\n", mes_data[ port ] );
			sceDevConsPrintf( console, "\n" );
		}
		sceDevConsDraw(console);

		if(frame&0x01)
		{	/* interrace half pixcel adjust */
			sceGsSetHalfOffset(&db.draw1,2048,2048, sceGsSyncV(0)^0x01);
		}
		else
		{
			sceGsSetHalfOffset(&db.draw0,2048,2048, sceGsSyncV(0)^0x01);
		}
		FlushCache(0);
		sceGsSyncPath(0,0);
	}

	return 0;
}
