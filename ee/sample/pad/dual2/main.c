/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *	Controller Liblary Sample for DUALSHOCK2
 *
 *			   Version 0.10
 *			     Shift-JIS
 *
 *	Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *			  All Rights Reserved.
 *
 *			      main.c
 *
 *      Version   Date            Design     Log
 *  --------------------------------------------------------------------
 *      0.10      Nov,22,1999     iwano      Initial
 *      0.11      Nov,10,2000     iwano      update
 *      0.12      Aug, 3,2001     iwano      update
 *
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

/* 描画環境 */
#define SUNIT		800		/* 最大描画オブジェクト数 */
#define PACKETSIZE	(8*SUNIT)	/* 描画パケットバッファ   */
#define SCREEN_WIDTH	640		/* 画面解像度(x)          */
#define SCREEN_HEIGHT	224		/* 画面解像度(y)          */
#define OFFX	(((4096-SCREEN_WIDTH)/2)<<4)	/* スクリーン左上始点座標(x) */
#define OFFY	(((4096-SCREEN_HEIGHT)/2)<<4)	/* スクリーン左上始点座標(y) */
#define BARX	16			/* 圧力ゲージ表示位置(x)  */
#define BARY	74			/* 圧力ゲージ表示位置(y)  */

static int sindex = 0;
static sceGsDBuff db;
static u_long giftag[2] = { SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0x000000000000000eL };
static sceDmaChan *p1;
static sceVif1Packet packet[2];
static SPRTX sprt[32];

/* 描画用パケットバッファ */
static u_long128 packet_buff[2][PACKETSIZE];

/* IOPインタフェース用DMAバッファ */
static u_long128 pad_dma_buf[scePadDmaBufferMax] __attribute__((aligned (64)));

/* コントローラ状態管理 */
static int term_id;
static int g_error_count = 0;

/* フォント表示用 */
static int console;
static int cons2;
extern void sceDevConsLocate(int cd, u_int lx, u_int ly);

/* メッセージ表示用 */
static char mes_state[80];
static char mes_term[80];
static char mes_data[80];
static char mes_press[80];


/* GS初期化 */
static void Gs_Initialize(void)
{
	sceDmaEnv env;
	static u_long giftagAD[2] __attribute__ (( aligned (16) )) =
		{ SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0x000000000000000eL };

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
		sceGsSetDefAlphaEnv((sceGsAlphaEnv *)packet[sindex].pCurrent,0)
		* 4);
	sceVif1PkCloseGifTag(&packet[sindex]);
	sceVif1PkCloseDirectCode(&packet[sindex]);
	sceVif1PkEnd(&packet[sindex], 0);
	sceVif1PkTerminate(&packet[sindex]);

	/* kick Gs initialize packet */
	FlushCache(0);
	sceDmaSend(p1,(u_int *)(((u_int)packet[sindex].pBase)));
	sceGsSyncPath(0,0);

	/* display next in odd field when interlace */
	while(!sceGsSyncV(0));
}

static unsigned char* button_mess[] =
	{ "R", "L", "U", "D", "T", "C", "X", "S", "L1", "R1", "L2", "R2" };

/* ボタン情報取得 */
static unsigned short pad_button_read(int port, int slot)
{
	int i;
	int rr;
	int rgb;
	int posx;
	SPRTX* p;
	unsigned char rdata[32];
	unsigned short paddata, tpad = 0;
	static unsigned short rpad = 0;

	strcpy(mes_data, "");
	strcpy(mes_press, "");

	/* ボタン情報取得 */
	if (scePadRead( port, slot, rdata )==0)
	{
		/* 取得失敗 */
		return 0;
	}

	p = &sprt[0];

	sceMakeSprt(p, BARX+32+511, BARY*2+16, 1, 12*16 );
	setRGB0(p, 160, 160, 160);
	sceVif1AddSprite(&packet[sindex], p, 1);

	sceDevConsClear(cons2);
	sceDevConsLocate(cons2, 0, 0);

	/* rdataの第１バイトが 0であればボタン情報は有効(0以外の時は無効) */
	if( rdata[0] == 0 )
	{
		/* ボタン情報を論理反転して [1= 押下, 0= リリース]にする */
		paddata = 0xffff ^ ((rdata[2]<<8)|rdata[3]);
		for( i = 0; i <8; i++ )
		{
			sprintf( mes_data+strlen(mes_data), "%02x ", rdata[ i ] );
		}

		/* ボタン情報と感圧ゲージの表示 */
		for( i = 8; i <20; i++ )
		{
			rgb = 0;
			posx = BARX+32;
			p = &sprt[i];
			sprintf( mes_press+strlen(mes_press), "%02x ", rdata[i] );
			sceMakeSprt(p, posx, (BARY*2)+(i-8+1)*16, 8, 12 );
			setRGB0(p, 16, 16, 255);
			sceVif1AddSprite(&packet[sindex], p, 1);
			while(rdata[i])
			{
				rr = rdata[i]; if (rr>4) rr = 4;
				rdata[i] -= rr;
				sceMakeSprt(p, posx, (BARY*2)+(i-8+1)*16, rr*2, 12 );
				setRGB0(p, rgb, rgb, 255);
				sceVif1AddSprite(&packet[sindex], p, 1);
				posx += (rr*2);
				rgb += rr;
			}
			sceDevConsLocate(cons2, 0, i);
			sceDevConsPrintf(cons2, "%s\n", button_mess[i-8]);
		}

		/* トリガ処理 */
		tpad = paddata & ~rpad;
		rpad = paddata;

		term_id = (int)rdata[1]>>4;
	}
	sceDevConsDraw(cons2);
	return (tpad);
}

static void scePacketOpen(void)
{

	/* 描画パケット構造体初期化 */
	sceVif1PkReset(&packet[sindex]);
	sceVif1PkCnt(&packet[sindex], 0);
	sceVif1PkOpenDirectCode(&packet[sindex], 0);
	sceVif1PkOpenGifTag(&packet[sindex], *(u_long128*)&giftag[0]);
}

static void scePacketClose(void)
{
	/* 描画パケットの終端処理 */
	sceVif1PkReserve(&packet[sindex], sceGsSetDefAlphaEnv((sceGsAlphaEnv *)packet[sindex].pCurrent,0)* 4);
	sceVif1PkCloseGifTag(&packet[sindex]);
	sceVif1PkCloseDirectCode(&packet[sindex]);
	sceVif1PkEnd(&packet[sindex], 0);
	sceVif1PkTerminate(&packet[sindex]);
}

/* メイン */
int main(void)
{
	int id;
	int exid;
	int frame;
	int phase;
	int state;
	int port;
	int slot;
	int ret;
	int pad = 0;

	/* Sifライブラリ初期化 */
	sceSifInitRpc(0);

	/* load sio2man.irx */
	while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/sio2man.irx",
			    0, NULL) < 0)
	{
	    printf("Can't load module sio2man\n");
	}

	/* load padman.irx */
	while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/padman.irx",
			    0, NULL) < 0)
	{
	    printf("Can't load module padman\n");
	}

	/* GS初期化 */
	Gs_Initialize();

	/* フォントシステム初期化 */
	sceDevConsInit();
	console = sceDevConsOpen(OFFX + (16<<4), OFFY + (16<<4), 75, 8);
	cons2 = sceDevConsOpen(OFFX + (BARX<<4), OFFY + (BARY<<4), 16, 14);

	strcpy(mes_data, "");
	strcpy(mes_state, "");
	strcpy(mes_term, "");

	/* コントローラライブラリ初期化 */
	scePadInit(0);
	port = 0;
	slot = 0;
	id = 0;
	term_id = 0;

	/* ポートオープン */
	ret = scePadPortOpen( port, slot, pad_dma_buf );
	if (ret==0)
	{
		printf( "ERROR: scePadPortOpen\n" );
		return 0;
	}

	phase = 0;
	frame = 0;
	sceGsSyncPath(0,0);

	/* メインループ */
	while(1)
	{
		sceGsSwapDBuff(&db, frame);
		++frame;

		scePacketOpen();

		/* コントローラの状態チェック */
		state = scePadGetState(port,slot);
		if (state>=0 && state<=7)
			scePadStateIntToStr(state, mes_state);
		else
			sprintf(mes_state,"%d", state);

		/* コントローラが抜かれたらPhase 0へ */
		if (state==scePadStateDiscon)
		{
			g_error_count = 0;
			phase = 0;
		}

		/* コントローラ情報取得、アクチュエータ設定 */
		switch(phase)
		{
			case 0:
			strcpy(mes_term, "");

			/* コントローラが接続されてるか？ */
			if (state != scePadStateStable &&
			  state != scePadStateFindCTP1 ) break;

			/* コントローラID取得 */
			id = scePadInfoMode(port, slot, InfoModeCurID, 0 );
			if (id==0) break;

			/* 拡張ID 取得 */
			exid = scePadInfoMode(port, slot, InfoModeCurExID,0);
			if (exid>0) id = exid;

			switch(id)
			{
				/* STANDARD */
				case 4:
				sprintf(mes_term, "%03x", id);
				phase = 40;
				break;

				/* ANALOG */
				case 7:
				sprintf(mes_term, "%03x", id);
				phase = 70;
				break;

				default:
				phase = 99;
				break;
			}
			sprintf(mes_term, "%03x", id);
			break;

			/* アナログ系コントローラか調べる */
			case 40:
			if (scePadInfoMode(port, slot, InfoModeIdTable, -1)==0)
			{
				phase = 99;
				break;
			}
			phase++;

			/* アナログモードに移行 */
			case 41:
			if (scePadSetMainMode(port, slot, 1, 0)==1)
			{
				phase++;
			}
			break;

			case 42:
			/* scePadSetMainMode()の実行結果をチェック */
			if (scePadGetReqState(port, slot)==scePadReqStateFaild)
			{
				phase--;
			}
			if (scePadGetReqState(port, slot)==scePadReqStateComplete)
			{
				phase = 0;
			}
			break;

			/* DUALSHOCK2か調べる */
			case 70:
			if (scePadInfoPressMode(port,slot)==1)
			{
				phase = 76;
				break;
			}
			phase = 99;
			break;

			/* 感圧モードへ移行 */
			case 76:
			if (scePadEnterPressMode(port, slot)==1)
			{
				phase++;
			}
			break;

			case 77:
			/* scePadEnterPressMode()の実行結果をチェック */
			if (scePadGetReqState(port, slot)==scePadReqStateFaild)
			{
				phase--;
			}
			if (scePadGetReqState(port, slot)==scePadReqStateComplete)
			{
				phase = 99;
			}
			break;

			default:
			/* エラーが発生していた場合はエラー数をカウントする */
			if (state==scePadStateError) g_error_count++;

			/* 定常状態の処理 */
			if (state==scePadStateStable || state==scePadStateFindCTP1)
			{
				pad = pad_button_read( port, slot );

				/* コントローラIDが変わったら最初に戻る */
				if (term_id != id)
				{
					phase = 0;
				}
			}
			break;
		}

		scePacketClose();

		/* フォント表示 */
		sceDevConsClear(console);
		sceDevConsLocate(console, 0, 0);
		sceDevConsPrintf(console, "STATUS : %s\n", mes_state );
		sceDevConsPrintf(console, "    ID : %s\n", mes_term );
		sceDevConsPrintf(console, "DATA-1 : %s\n", mes_data );
		sceDevConsPrintf(console, "DATA-2 : %s\n", mes_press );
		sceDevConsPrintf(console, "ERROR  : %d\n", g_error_count );

		sceDevConsLocate(console, 4, 6);
		sceDevConsPrintf(console, "0");
		sceDevConsLocate(console, 5+10, 6);
		sceDevConsPrintf(console, "50");
		sceDevConsLocate(console, 5+23, 6);
		sceDevConsPrintf(console, "100");
		sceDevConsLocate(console, 5+35, 6);
		sceDevConsPrintf(console, "150");
		sceDevConsLocate(console, 5+47, 6);
		sceDevConsPrintf(console, "200");
		sceDevConsLocate(console, 5+60, 6);
		sceDevConsPrintf(console, "255");
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
		sceDmaSend(p1,(u_int *)(((u_int)packet[sindex].pBase)));

		sceGsSyncPath(0,0);
		sindex ^= 0x01;
	}

	return 0;
}
