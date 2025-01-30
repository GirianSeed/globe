/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *              Emotion Engine Library Sample Program
 *                          Version 1.20
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        EzBGM - main.c
 *                        main routine
 *
 *     Version    Date          Design     Logf
 *  --------------------------------------------------------------------
 *     1.20       Nov.23.1999   morita     first checked in.
 *                Sep.   2000   kaol       separated status transition
 */

#include <eekernel.h>
#include <string.h>	/* for strcpy() */
#include <sifdev.h>	/* for sceSif*() */
#include <libsdr.h>
#include <sdrcmd.h>
#include "bgm_i.h"
#include <libdev.h>
#include <libpad.h>

/* パケットサイズ:

   シークが間に合わないときはPACKET_SIZEを増やせば改善しますが、レーテンシは
   増加します。
   ネットワーク越しにホストのディスクをアクセスする場合、ネットワークの遅延
   によって正常に演奏できないことがあります。

   2ファイルの同時演奏のために、かなり大きくバッファを取っていますが、1ファイ
   ルのみであれば、12kByte程度でも演奏できます。

   バッファのサイズを2ファイル間で変えているのは、ファイルリードのタイミング
   がなるべくぶつからないようにするためです。
*/

#define PACKET_SIZE1      (512*150)    //-- 512の倍数であること
#define PACKET_SIZE2      (512*170)   //-- 512の倍数であること


#define CNTL_RUN  0
#define CNTL_STOP 1
#define CNTL_SEEK 2
#define CNTL_PRELOAD 3
#define CNTL_START   4

char gFilename[2][64] __attribute__((aligned (64))); //-- 注意！このままIOPにDMA転送されます

/* 外部関数 (bgm_rpc.c) */
int ezBgmInit( void );
int ezBgm( int command,  int status );

/* コントローラ作業用バッファ */
u_long128 pad_dma_buf [scePadDmaBufferMax] __attribute__((aligned (64)));

/*
 * load_modules()	モジュールのロード
 *	引数:   なし
 *	返り値: なし
 */
void load_modules( void )
{
	/* SIF RPC の初期化 */
	sceSifInitRpc(0);

	/*
	 *	必要となる各モジュールのロード
	 */
	/* シリアル通信基本モジュール */
	while(sceSifLoadModule("host0:/usr/local/sce/iop/modules/sio2man.irx",0, NULL) < 0){
		scePrintf("loading sio2man.irx failed\n");
	}
	/* コントローラ制御モジュール */
	while(sceSifLoadModule("host0:/usr/local/sce/iop/modules/padman.irx",0, NULL) < 0){
		scePrintf("loading padman.irx failed\n");
	}

	/* 低レベルサウンド制御モジュール */
	while(sceSifLoadModule("host0:../../../../iop/modules/libsd.irx",0, NULL) < 0){
		scePrintf("loading libsd.irx failed\n");
	}
	
	/* EzBGM用ドライバモジュール */
	while(sceSifLoadModule("host0:../../../../iop/sample/sound/ezbgm/ezbgm.irx",0, NULL) < 0){
		scePrintf("loading ezbgm.irx failed\n");
	}

	return;
}

/*
 * init_bgm()		IOP 側のセットアップ:
 *			ファイルオープン、ボイス指定、ボリューム設定から
 *			演奏スタートまで
 *	引数:
 *		wav_count: 波形データの数
 *	返り値:
 *		必ず 0
 */
int init_bgm( int wav_count )
{
	int i, info;

	/* SIF RPC 接続の確立 */
	ezBgmInit();

	/* 以下３行の設定はlibsdrを使ってもよい */
	/* IOP 側低レベルサウンドモジュールの初期化 */
	ezBgm( EzBGM_SDINIT, SD_INIT_COLD );   //-- sceSdInit()を行なう
	/* マスターボリュームの設定 */
	ezBgm( 0|EzBGM_SETMASTERVOL, 0x3fff3fff ); //-- master vol core0
	ezBgm( 1|EzBGM_SETMASTERVOL, 0x3fff3fff ); //-- master vol core1

	for( i = 0; i < wav_count ; i++ ){
		/* IOP 側で波形データファイルをオープン */
		info = ezBgm( i|EzBGM_OPEN, (int)gFilename[i] );
		scePrintf("  wav:%d %s  %d byte(body)  ", i,gFilename[i],info&(~WAV_STEREO_BIT) );

		if( (info&WAV_STEREO_BIT) == 0 ){  //-- モノラル
			scePrintf("mono \n");
			ezBgm( i|EzBGM_INIT, PACKET_SIZE2*3 );
			ezBgm( i|EzBGM_SETMODE, BGM_MODE_REPEAT_FORCED|BGM_MODE_MONO );
		}
		else{ //-- ステレオ
			scePrintf("stereo \n");
			ezBgm( i|EzBGM_INIT, PACKET_SIZE1*4 );
			ezBgm( i|EzBGM_SETMODE, BGM_MODE_REPEAT_FORCED|BGM_MODE_STEREO );
		}
		ezBgm( i|EzBGM_PRELOAD, 0 );		/* 波形データをプリロード */
		ezBgm( i|EzBGM_SETVOL, 0x3fff3fff );	/* ボリューム設定 */
		ezBgm( i|EzBGM_START, 0 );		/* BGM 演奏開始 */
	}
	//--- 以上でBGM演奏がスタート ---------------------
	scePrintf("EzBGM start!\n");

	return 0;
}


/*
 * main_loop()		処理のメインループ:
 *			コントローラ入力から IOP へ RPC
 *	引数:   なし
 *	返り値: なし
 */
void main_loop( int wav_count )
{
	int run_status[2], cntl_status[2], i, tPause = 0;
	u_int paddata, oldpad, ch;
	u_char rdata[32];

	/* コントローラ関連の初期化 */
	scePadInit(0);
	scePadPortOpen(0, 0, pad_dma_buf);

	/* 状態の初期化 */
	cntl_status [0] = cntl_status [1] = CNTL_RUN;
	oldpad = paddata = 0;
	while( 1 )
	{
	    oldpad = paddata;
	    FlushCache(0);
	    /* コントローラ情報の読み込み */ 
	    if(scePadRead(0, 0, rdata) > 0){
		paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);
		if( wav_count == 1 ) paddata &= ~(SCE_PADRleft|SCE_PADRdown);
	    }
	    else{
		paddata = 0;
	    }

	    if( paddata != oldpad )
	    {
		/* IOP 側のストリーミング稼働状態の取得 */
		run_status[0] = ezBgm( 0|EzBGM_GETMODE, 0)&~BGM_MASK_STATUS;
		run_status[1] = ezBgm( 1|EzBGM_GETMODE, 0)&~BGM_MASK_STATUS;
		ch = 0;
		if ( (paddata & SCE_PADRup) || (paddata & SCE_PADRright)  ) ch = 0;
		if ( (paddata & SCE_PADRleft) || (paddata & SCE_PADRdown) ) ch = 1;

		/* ストリーミングを停止 */
		if ( (paddata & SCE_PADRup) || (paddata & SCE_PADRleft) ){  
			if( (run_status[ch] & BGM_MODE_RUNNING) != 0 ){
				ezBgm( ch|EzBGM_STOP, 0 );
				cntl_status[ch] = CNTL_STOP;
			}
		}
		/* ストリーミングを開始 ... 実際の処理は下の for() にて行う */
		else if( (paddata & SCE_PADRright ) || (paddata & SCE_PADRdown ) ){  
			if( run_status[ch] != 0 ){
				continue;
			}
			cntl_status[ch] = CNTL_SEEK;
		}
		/* ストリーミングを一時停止/一時停止解除 */
		else if (paddata & SCE_PADstart ){  
			if( tPause == 0 ){
			    for( i = 0; i < wav_count; i++ ){
				if( (run_status[i] & BGM_MODE_RUNNING) != 0 ){
					/* ストリーミングを一時停止 */
					ezBgm( i|EzBGM_STOP, 0 );
				}
			    }
			    tPause = 1;
			}
			else if ( tPause == 1 ){
			    for( i = 0; i < wav_count; i++ ){
				if( (run_status[i] == 0) && (cntl_status[i] == CNTL_RUN) ){
				    /* ストリーミングを再開: 以前止めた位置から */
				    cntl_status [i] = CNTL_PRELOAD;
				}
			    }
			    tPause = 0;
			}
		}
		/* ストリーミングを終了 → プログラム終了 */
		else if (paddata & SCE_PADselect ){  
			for( i = 0; i < wav_count; i++ ){
				ezBgm( i|EzBGM_SETVOL, 0x0 );
				ezBgm( i|EzBGM_STOP, 0 );
				ezBgm( i|EzBGM_CLOSE, 0 );
				ezBgm( i|EzBGM_QUIT, 0 );
			}
			ezBgm( 0|EzBGM_SETMASTERVOL, 0x0 ); //-- master vol core0
			ezBgm( 1|EzBGM_SETMASTERVOL, 0x0 ); //-- master vol core1
			return;
		}
	    }

	    /* cntl_status[] の値によってストリーミング処理の*/
	    for (i = 0; i < wav_count; i ++) {
		int ret;
		switch (cntl_status [i]) {
		case CNTL_SEEK:
		    ret = ezBgm (i | EzBGM_SEEK, 0);	/* 読みだし位置を先頭に */
		    if (ret != -1) {
			cntl_status [i] ++;		/* PRELOAD に状態遷移 */
		    }
		    break;
		case CNTL_PRELOAD:
		    ret = ezBgm (i | EzBGM_PRELOAD, 0);
		    if (ret == -1) {
			cntl_status [i] = CNTL_SEEK;	/* SEEK に戻って再開 */
		    } else {
			cntl_status [i] ++;		/* START に状態遷移 */
		    }
		    break;
		case CNTL_START:
		    ret = ezBgm (i | EzBGM_START, 0);
		    if (ret != -1)
			cntl_status [i] = CNTL_RUN;	/* 「稼働中」に状態遷移 */
		    break;
		case CNTL_RUN:				/* この状態では状態遷移、及び */
		case CNTL_STOP:				/* IOP に指示する内容は無し */
		default:
		    break;
		}
	    }

	    sceGsSyncV(0);
	}
	return;
}


/*----------------------
  エントリ
  ----------------------*/
int main( int argc, char *argv[]  )
{
	int i, wav_count=0;

	//--- モジュールのロード
	load_modules();

	scePrintf("EzBGM initialize...\n");

	//--- argsのパース
	if( (argc < 2) || argc > 3 ){
		scePrintf("\n\nusage: %s <wav file> [<wav file>]\n\n", argv[0]);
		return 0;
	}

	for( i = 0; i < argc-1 ; i++ ){
		strcpy( gFilename[i], argv[i+1]);
		wav_count++;
	}

	//--- BGMをスタートさせる
	init_bgm(wav_count);


	//--- BGMをコントロールする
	main_loop(wav_count);
	

	scePrintf("EzBGM finished...\n");
	return 0;
}


