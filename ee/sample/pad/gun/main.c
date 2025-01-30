/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *	Controller Liblary Sample for GUNCON (NAMCO)
 *
 *			   Version 1.0
 *			     Shift-JIS
 *
 *	Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *			  All Rights Reserved.
 *
 *			      main.c
 *
 *       Version         Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.0             Apr,21,2000     iwano       initial
 */

#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libdma.h>
#include <libpkt.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libdev.h>
#include <libpad.h>
#include "sprt.h"

#define ERR_STOP	while(1)

/* 描画環境マクロ定義 */
#define SUNIT		8
#define PACKETSIZE	(8*SUNIT)
#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	224
#define OFFX	(((4096-SCREEN_WIDTH)/2)<<4)
#define OFFY	(((4096-SCREEN_HEIGHT)/2)<<4)

/* 描画環境用 */
static int sindex = 0;
static sceGsDBuff db;
static sceDmaChan *p1;
static u_long giftag[2] = { SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0x000000000000000eL };
static sceVif1Packet packet[2];
static SPRTX sprt[32];
static int frame = 0;

/* 描画用パケットバッファ */
static u_long128 packet_buff[2][PACKETSIZE];

/* IOPインタフェース用DMAバッファ */
static u_long128 pad_dma_buf[2][scePadDmaBufferMax] __attribute__((aligned (64))); 

/* コントローラ情報 */
static unsigned short paddata = 0;

/* フォント表示用 */
static int console;
extern void sceDevConsLocate(int cd, u_int lx, u_int ly);

/* GS初期化 */
static void Gs_Initialize(void)
{
	sceDmaEnv env;
	u_long giftagAD[2] = { SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0x000000000000000eL };

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

	//sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
	sceGsResetGraph(0, 0, SCE_GS_NTSC, SCE_GS_FRAME);
	sceGsSetDefDBuff(&db, SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT,
		SCE_GS_ZGEQUAL, SCE_GS_PSMZ24, SCE_GS_CLEAR);

	db.clear0.rgbaq.R = db.clear1.rgbaq.R = 44;
	db.clear0.rgbaq.G = db.clear1.rgbaq.G = 82;
	db.clear0.rgbaq.B = db.clear1.rgbaq.B = 124;

	FlushCache(0);

	sindex = 0;
	sceVif1PkReset(&packet[sindex]);
	sceVif1PkCnt(&packet[sindex], 0);
	sceVif1PkOpenDirectCode(&packet[sindex], 0);
	sceVif1PkOpenGifTag(&packet[sindex], *(u_long128*)&giftagAD[0]);

	sceVif1PkReserve(&packet[sindex],
		sceGsSetDefAlphaEnv((sceGsAlphaEnv *)packet[sindex].pCurrent,0)
		* 4);

	sceVif1PkCloseGifTag(&packet[sindex]);
	sceVif1PkCloseDirectCode(&packet[sindex]);
	sceVif1PkEnd(&packet[sindex], 0);
	sceVif1PkTerminate(&packet[sindex]);

	/* kick Gs initialize packet */
	sceDmaSend(p1,(u_int *)(((u_int)packet[sindex].pBase)));

	sceGsSyncPath(0,0);
}

void scePacketOpen(void)
{
	/* 描画パケット構造体初期化 */
	sceVif1PkReset(&packet[sindex]);
	sceVif1PkCnt(&packet[sindex], 0);
	sceVif1PkOpenDirectCode(&packet[sindex], 0);
	sceVif1PkOpenGifTag(&packet[sindex], *(u_long128*)&giftag[0]);
}

void scePacketClose(void)
{
	/* 描画パケットの終端処理 */
	sceVif1PkReserve(&packet[sindex], sceGsSetDefAlphaEnv((sceGsAlphaEnv *)packet[sindex].pCurrent,0)* 4);
	sceVif1PkCloseGifTag(&packet[sindex]);
	sceVif1PkCloseDirectCode(&packet[sindex]);
	sceVif1PkEnd(&packet[sindex], 0);
	sceVif1PkTerminate(&packet[sindex]);
}

/* ボタン情報取得 */
static int attach_controller(int port, int slot, int phase)
{
	int i;
	int id;
	int state;
	SPRTX* p;
	unsigned char rdata[32];
	char mes_stat[80];
	char mes_data[80];

	strcpy(mes_stat, "");
	strcpy(mes_data, "");

	/* コントローラの状態チェック */
	state = scePadGetState(port,slot);
	scePadStateIntToStr(state, mes_stat);

	/* コントローラが抜かれたらPhase 0へ */
	if (state==scePadStateDiscon)
	{
		phase = 0;
	}

	/* 1P = ■ 2P = □ を表示 */
	p = &sprt[2+port];
	if (port == 0)
	{
		SuperxMakeSprt(p, 4*8+32, 32, 16, 16);
		setRGB0(p, 32, 255, 32);
	}
	else
	{
		SuperxMakeSprt(p, 13*8 + 32, 32, 16, 16);
		setRGB0(p, 255, 32, 32);
	}
	SuperxVif1AddSprite(&packet[sindex], p, 1);


	/* コントローラ情報取得、アクチュエータ設定 */
	switch(phase)
	{
		case 0:
		/* コントローラが接続されてるか？ */
		if (state != scePadStateStable &&
		  state != scePadStateFindCTP1 ) break;

		/* コントローラID取得 */
		id = scePadInfoMode(port, slot, InfoModeCurID, 0 );
		if (id==0) break;

		/* ガンコンが接続されているかを確認 */
		if (id==6)
		{
			phase = 60;
			break;
		}
		phase = 100;
		break;

		/* ガンコン座標情報取得 */
		case 60:
		if (state==scePadStateFindCTP1)
		{
			int gx, gy;

			/* ボタン情報、座標情報を取得 */
			scePadRead(port, slot, rdata);

			for( i = 0; i <16; i++ )
			{
				sprintf( strchr(mes_data, 0), "%02X ", rdata[ i ] );
			}

			/* ボタン情報を論理反転して [1= 押下, 0= リリース]にする */
			paddata = 0xffff^((rdata[2]<<8)|rdata[3]);

			/* 座標情報(ピクセルデータ)をスクリーン座標に変換 */
			gx = (int)rdata[4] + (int)rdata[5]*256 - 0x40;
			gy = (int)rdata[6] + (int)rdata[7]*256 - 0x28;

			gx = gx*SCREEN_WIDTH / 384;
			gy = gy*SCREEN_HEIGHT / 240;

			/* ターゲットカーソルを表示 */
			p = &sprt[port];
			SuperxMakeSprt(p, gx - 16, gy*2 - 16, 32, 32);
			if (port==0)
			{
				setRGB0(p, 32, 255, 32);
			}
			else
			{
				setRGB0(p, 255, 32, 32);
			}
			SuperxVif1AddSprite(&packet[sindex], p, 1);
			p++;
		}
		break;

		/* ガンコン以外のコントローラ */
		case 100:
		break;
	}

	/* フォント表示 */
	sceDevConsPrintf(console, "[PORT %d]\n", port );
	sceDevConsPrintf(console, "STATUS : %s\n", mes_stat );
	sceDevConsPrintf(console, "  DATA : %s\n", mes_data );

	return (phase);
}

int main(void)
{
	int ret;
	int phase1, phase2;
	int port;
	int slot;

	/* Sifライブラリ初期化 */
	sceSifInitRpc(0);

	/* sio2mam.irx をロード */
	while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/sio2man.irx", 0, NULL) < 0)
	{
		printf("Can't load module sio2man\n");
	}

	/* padman.irx をロード */
	while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/padman.irx", 0, NULL) < 0)
	{
		printf("Can't load module padman\n");
	}

	/* GS初期化 */
	Gs_Initialize();

	/* フォントシステム初期化 */
	sceDevConsInit();
	console = sceDevConsOpen(OFFX + (24<<4), OFFY + (16<<4), 75, 20);

	/* コントローラライブラリ初期化 */
	scePadInit(0);

	/* ポートオープン */
	port = 0;		/* 1P */
	slot = 0;

	ret = scePadPortOpen( port, slot, pad_dma_buf[port] );
	if (ret==0)
	{
		printf( "ERROR: scePadPortOpen\n" );
		ERR_STOP;
	}

	port = 1;		/* 2P */
	slot = 0;
	ret = scePadPortOpen( port, slot, pad_dma_buf[port] );
	if (ret==0)
	{
		printf( "ERROR: scePadPortOpen\n" );
		ERR_STOP;
	}

	phase1 = 0;
	phase2 = 0;
	frame = 0;

	/* display next in odd field when interlace */
	while(!sceGsSyncV(0));

	/* メインループ */
	while(1)
	{
		/* 描画パケットバッファ初期化 */
		scePacketOpen();

		/* フォントフレーム初期化 */
		sceDevConsClear(console);
		sceDevConsLocate(console, 0, 0);
		sceDevConsPrintf(console, "1P =     2P = \n");

		/* 1P 処理 */
		sceDevConsLocate(console, 0, 2);
		phase1 = attach_controller(0, slot, phase1);

		/* 2P 処理 */
		sceDevConsLocate(console, 0, 13);
		phase2 = attach_controller(1, slot, phase2);

		/* 描画パケット終端処理 */
		scePacketClose();

		/* フォント表示 */
		sceDevConsDraw(console);

		if(frame&0x01)
		{	/* interrace half pixcel adjust */
			sceGsSetHalfOffset(&db.draw1,2048,2048, sceGsSyncV(0)^0x01);
		}
		else
		{
			sceGsSetHalfOffset(&db.draw0,2048,2048, sceGsSyncV(0)^0x01);
		}

		sceGsSwapDBuff(&db, frame);

		FlushCache(0);
		sceDmaSend(p1,(u_int *)(((u_int)packet[sindex].pBase)));
		sceGsSyncPath(0,0);

		sindex ^= 0x01;
		frame++;
	}
}
