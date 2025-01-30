/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *	Controller Library Sample
 *
 *                          Version 0.12
 *	                    Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *	                 All Rights Reserved.
 *
 *                              main.c
 *
 *      Version         Date            Design     Log
 *  --------------------------------------------------------------------
 *      0.10            Oct, 8,1999     makoto     Initial
 *	0.11            Oct,14,1999     iwano      update
 *	0.12            Nov,10,2000     iwano      update
 *	0.13            Aug, 3,2001     iwano      update
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
#include <libdev.h>
#include <sifrpc.h>
#include <libpad.h>

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	224
#define OFFX		(((4096-SCREEN_WIDTH)/2)<<4)
#define OFFY		(((4096-SCREEN_HEIGHT)/2)<<4)

/* 描画環境 */
#define SUNIT		0x01
#define PACKETSIZE	(0x100*SUNIT)
static u_long128 packet_buff[2][PACKETSIZE];
static sceGsDBuff db;

/* IOPインタフェース用DMAバッファ */
static u_long128 pad_dma_buf[scePadDmaBufferMax] __attribute__((aligned (64)));

/* コントローラ状態管理 */
static int term_id = 0;		// コントローラID
static unsigned char act_direct[6];
static unsigned char act_align[6];

/* フォント表示用 */
static int console;
static char mes_state[80];
static char mes_term[80];
static char mes_data[80];
extern void sceDevConsLocate(int cd, u_int lx, u_int ly);

/***************************************************************************


***************************************************************************/
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

/* ボタン情報取得など */
static unsigned short pad_button_read(int port, int slot)
{
	int i;
	unsigned char rdata[32];
	unsigned short paddata, tpad;
	static unsigned short rpad = 0;

	strcpy(mes_data, "");

	tpad = 0;

	/* ボタン情報取得 */
	if (scePadRead( port, slot, rdata )==0)
	{	
		/* 取得失敗 */
		return 0;
	}

	/* rdataの第１バイトが 0であればボタン情報は有効(0以外の時は無効) */
	if( rdata[0] == 0 )
	{
		/* ボタン情報を論理反転して [1= 押下, 0= リリース]にする */
		paddata = 0xffff ^ ((rdata[2]<<8)|rdata[3]);

		for( i = 0; i < 15; i++ )
		{
			sprintf( mes_data+strlen(mes_data), "%02x ", rdata[ i ] );
		}


		/* トリガ処理 */
		tpad = paddata & ~rpad;
		rpad = paddata;

		term_id = (rdata[1]>>4);

		switch(term_id)
		{
			case 7:
			/* 小モータ回転 */
			if( tpad & SCE_PADLup ) {
				act_direct[ 0 ] = 1;
				scePadSetActDirect( port, slot, act_direct );
			}
			/* 小モータ停止 */
			if( tpad & SCE_PADLdown ) {
				act_direct[ 0 ] = 0;
				scePadSetActDirect( port, slot, act_direct );
			}
			/* 大モータ回転 */
			if( tpad & SCE_PADLleft ) {
				act_direct[ 1 ] = 200;
				scePadSetActDirect( port, slot, act_direct );
			}
			/* 大モータ停止 */
			if( tpad & SCE_PADLright ) {
				act_direct[ 1 ] = 0;
				scePadSetActDirect( port, slot, act_direct );
			}
			break;
		}
	}
	return (tpad);
}

/* メイン */
int main(void)
{
	int i;
	int id;
	int frame;
	int phase;
	int state;
	int port;
	int slot;
	int ret;
	int exid;
	unsigned short pad;

	sceSifInitRpc(0);

	/* sio2man.irxをロードする */
	while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/sio2man.irx",
			    0, NULL) < 0)
	{
	    printf("Can't load module sio2man\n");
	}

	/* padman.irxをロードする */
	while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/padman.irx",
			    0, NULL) < 0)
	{
	    printf("Can't load module padman\n");
	}

	/* GS初期化 */
	Gs_Initialize();

	/* フォントシステム初期化 */
	sceDevConsInit();
	console = sceDevConsOpen(OFFX + (16<<4), OFFY + (24<<4), 75, 30);
	sceDevConsClear(console);
	strcpy(mes_data, "");
	strcpy(mes_state, "");
	strcpy(mes_term, "");

	/* コントローラライブラリ初期化 */
	scePadInit(0);
	port = 0;
	slot = 0;

	id = 0;
	term_id = 0;

	/* アクチュエータ情報初期化 */
	for (i=0; i<6; i++)
		act_direct[i] = 0x00;

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

		/* コントローラの状態チェック */
		state = scePadGetState(port,slot);
		if (state>=0 && state<=7)
			scePadStateIntToStr(state, mes_state);
		else
			sprintf(mes_state,"%d", state);
		if (state==scePadStateDiscon)
		{
			phase = 0;
		}

		/* コントローラ情報取得、アクチュエータ設定 */
		switch(phase)
		{
			case 0:
			strcpy(mes_term, "");
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
				/* ネジコン */
				case 2:
				sprintf(mes_term, "%04x [%s]", id, "NeGi-CON");
				phase = 99;
				break;

				/* コナミ製ガンコントローラ */
				case 3:
				sprintf(mes_term, "%03x [%s]", id, "GunCON(Konami)");
				phase = 99;
				break;

				/* 標準 */
				case 4:
				sprintf(mes_term, "%03x [%s]", id, "STANDARD");
				phase = 40;
				break;

				/* アナログジョイスティック */
				case 5:
				sprintf(mes_term, "%03x [%s]", id, "JOYSTICK");
				phase = 99;
				break;

				/* ナムコ製ガンコントローラ */
				case 6:
				sprintf(mes_term, "%03x [%s]", id, "GunCON(NAMCO)");
				phase = 99;
				break;

				/* アナログコントローラ */
				case 7:
				sprintf(mes_term, "%03x [%s]", id, "ANALOG");
				phase = 70;
				break;

				/* 釣りコン */
				case 0x100:
				sprintf(mes_term, "%03x [%s]", id, "TSURI-CON");
				phase = 99;
				break;

				/* ジョグコン */
				case 0x300:
				sprintf(mes_term, "%03x [%s]", id, "JOG-CON");
				phase = 99;
				break;

				default:
				sprintf(mes_term, "%03x [%s]", id, "UNKNOWN");
				phase = 99;
				break;
			}
			break;

			case 40:
			/* アナログ系コントローラか調べる */
			if (scePadInfoMode(port, slot, InfoModeCurExID,0)==0)
			{
				phase = 99;
				break;
			}
			
			/* アナログモードに移行 */
			if (scePadSetMainMode(port, slot, 1, 0)==1)
			{
				phase++;
			}
			break;

			case 41:
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

			case 70:
			/* アクチュエータをもっているかをチェック */
			if (scePadInfoAct(port, slot, -1, 0)==0)
			{
				/* SCPH-1150 */
				phase = 99;
			}
			/* アクチュエータ設定 (DUALSHOK用) */
			act_align[ 0 ] = 0;
			act_align[ 1 ] = 1;
			for( i = 2; i < 6; i++ )
				act_align[ i ] = 0xff;
			if (scePadSetActAlign( port, slot, act_align ))
			{
				phase++;
			}
			break;

			case 71:
			/* scePadSetActAlgin()の実行結果をチェック */
			if (scePadGetReqState(port, slot)==scePadReqStateFaild) phase--;
			if (scePadGetReqState(port, slot)==scePadReqStateComplete) phase = 99;
			break;

			default:
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

		/* フォント表示 */
		sceDevConsClear(console);
		sceDevConsLocate(console, 0, 0);
		sceDevConsPrintf(console, "LIBPAD SAMPLE\n" );
		sceDevConsPrintf(console, "------------------------------------------------------------------------\n" );
		sceDevConsPrintf(console, "\n" );
		sceDevConsPrintf(console, "  STATUS : %s\n", mes_state );
		sceDevConsPrintf(console, "      ID : %s\n", mes_term );
		sceDevConsPrintf(console, "PAD DATA : %s\n", mes_data );

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
