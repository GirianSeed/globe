/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *              Emotion Engine Library Sample Program
 *                          Version 0.11
 *                           Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *	                  EzADPCM - main.c
 *		            main routine
 *
 *	Version		Date		Design	Log
 *  --------------------------------------------------------------------
 *	0.10		Feb. 3, 2000	kaol
 *	0.11		Feb.27, 2000	kaol	BUFFER_SIZE is modified
 */

#include <eekernel.h>
#include <string.h>	/* for strcpy() */
#include <sifdev.h>
#include <libgraph.h>	/* for sceGsSyncV() */
#include <libsdr.h>
#include <sdrcmd.h>
#include <libpad.h>
#include "ezadpcm.h"

/* IOP 側の波形データ用バッファサイズ */
#define BUFFER_SIZE      499712

/* ADPCM ストリーミングの状態 */
#define _RUN  1				/* 稼働中 */
#define _STOP 0				/* 停止中 */
/* LR チャンネル指定 */
#define _L 0
#define _R 1

/* ボリューム */
#define _VOLUME 0x3fff			/* ボリューム初期値 */
#define _LR(x) ((x) << 16 | (x))	/* ボリューム指定用簡易マクロ */

/* 波形データファイル名 */
char gFilename[2][64] __attribute__((aligned (64)));
					/* gFilename[0]: L チャンネル用
					   gFilename[1]: R チャンネル用 */

/* 外部関数 (rpc.c) */
extern int ezAdpcm_RPCinit (void);
extern int ezAdpcm_RPC (int command, int arg);

/* 使用する IOP モジュール */
#define _MOD_ROOT    "host0:/usr/local/sce/iop/modules/"
#define _MOD_SIO2MAN "sio2man.irx"
#define _MOD_PADMAN  "padman.irx"
#define _MOD_LIBSD   "host0:/usr/local/sce/iop/modules/libsd.irx"
#define _MOD_EZADPCM "host0:/usr/local/sce/iop/sample/sound/ezadpcm/ezadpcm.irx"

/* コントローラ作業用バッファ */
u_long128 pad_dma_buf [scePadDmaBufferMax]  __attribute__((aligned (64)));

/*
 * load_modules()	モジュールのロード
 *	引数:   なし
 *	返り値: なし
 */
void
load_modules (void)
{
    /* SIF RPC の初期化 */
    sceSifInitRpc (0);

    scePrintf ("loading modules ...\n");

    /*
     *	必要となる各モジュールのロード
     */
    /* シリアル通信基本モジュール */
    while (sceSifLoadModule (_MOD_ROOT _MOD_SIO2MAN, 0, NULL) < 0) {
	scePrintf ("  loading %s failed\n", _MOD_SIO2MAN);
    }
    /* コントローラ制御モジュール */
    while (sceSifLoadModule (_MOD_ROOT _MOD_PADMAN, 0, NULL) < 0) {
	scePrintf ("  loading %s failed\n", _MOD_PADMAN);
    }
    /* 低レベルサウンド制御モジュール */
    while (sceSifLoadModule(_MOD_LIBSD, 0, NULL) < 0) {
	scePrintf ("  loading libsd.irx failed\n");
    }
    /* EzADPCM 用ドライバモジュール */
    while (sceSifLoadModule (_MOD_EZADPCM, 0, NULL) < 0) {
	scePrintf ("  loading ezadpcm.irx failed\n");
    }

    scePrintf ("loading modules ... done.\n");

    return;
}

/*
 * init_adpcm()		IOP 側のセットアップ:
 *			ファイルオープン、ボイス指定、ボリューム設定など
 *	引数:   なし
 *	返り値:
 *		0 ... セットアップ正常終了
 *	       -1 ... エラー
 */
int
init_adpcm (void)
{
    int info;

    /* SIF RPC 接続の確立 */
    if (ezAdpcm_RPCinit () < 0)
	return (-1);

    /* IOP 側低レベルサウンドモジュールの初期化 */
    ezAdpcm_RPC (EzADPCM_SDINIT, SD_INIT_COLD); /* = sceSdInit() */
    /* マスターボリュームの設定 */
    ezAdpcm_RPC (SD_CORE_0 | EzADPCM_SETMASTERVOL, _LR (0x3fff)); /* CORE0 master vol */
    ezAdpcm_RPC (SD_CORE_1 | EzADPCM_SETMASTERVOL, _LR (0x3fff)); /* CORE1 master vol */

    /* IOP 側で波形データファイルをオープン */
    info = ezAdpcm_RPC (_L | EzADPCM_OPEN, (int) gFilename[_L]); /* L チャンネル用 */
    scePrintf ("  VB:%d %s  %d byte(body)\n", _L, gFilename[_L], info);

    info = ezAdpcm_RPC (_R | EzADPCM_OPEN, (int) gFilename[_R]); /* L チャンネル用 */
    scePrintf ("  VB:%d %s  %d byte(body)\n", _R, gFilename[_R], info);

    /* IOP 側で波形データ用のメモリ領域を確保 */
    ezAdpcm_RPC (EzADPCM_INIT, BUFFER_SIZE);

    /* ADPCM ストリーミングで使用するボイスを指定
     * 現在の実装では CORE0 限定 */
    ezAdpcm_RPC (_L | EzADPCM_SETVOICE, 22); /* ボイス 22 */
    ezAdpcm_RPC (_R | EzADPCM_SETVOICE, 23); /* ボイス 23 */
    ezAdpcm_RPC (EzADPCM_SETVOL, _LR(_VOLUME)); /* ボリューム初期値を指定 */

    return (0);
}

/*
 * main_loop()		処理のメインループ:
 *			コントローラ入力から IOP へ RPC
 *	引数:   なし
 *	返り値: なし
 */
void
main_loop (void)
{
    int run_status;		/* IOP 側のストリーミング状況 */
    int running = _STOP;	/*  EE 側のストリーミング指定状態 */
    u_int paddata, oldpad;
    u_char rdata [32];
    int vol = _VOLUME;		/* ボリューム初期値 */
    int isstop = 0;		/* プログラム終了フラグ */

    oldpad = paddata = 0;
    while (1) {
	/* コントローラ情報の読み込み */ 
#define _PAD_DATA (0xffff ^ ((rdata[2] << 8) | rdata[3]))
	if (scePadRead (0, 0, rdata) > 0) paddata = _PAD_DATA;
	else                              paddata = 0;

#ifdef DEBUG
	if (paddata != 0) scePrintf ("%08x\n", paddata);
#endif

	/* isstop:
	   0      ... セレクトボタンが押されていない → 通常動作
	   0 以外 ... セレクトボタンが押された       → プログラム終了へ */
	if (isstop == 0) {
	    /*
	     * コントローラ情報による処理
	     */
	    if (paddata != oldpad) {
		/* IOP 側のストリーミング稼働状態の取得 */
		run_status = ezAdpcm_RPC (EzADPCM_GETSTATUS, 0);
		if (paddata & SCE_PADstart) {
		    // ストリーミングを開始/停止
		    if (running == _STOP) {
			/* ストリーミングを開始 */
			ezAdpcm_RPC (EzADPCM_PRELOADSTART, 0);
			running = _RUN;
		    } else {
			if (run_status == EzADPCM_STATUS_RUNNING) {
			    /* ストリーミングを停止 */
			    ezAdpcm_RPC (EzADPCM_STOP, 0);
			    running = _STOP;
			}
		    }
		} else if (paddata & SCE_PADselect) {
		    /* プログラム終了 */
		    ezAdpcm_RPC (EzADPCM_SETVOL, 0);	/* ボリュームを 0 に */
		    ezAdpcm_RPC (EzADPCM_STOP,   0);	/* ストリーミングを停止 */
		    isstop ++;				/* 終了処理へ遷移 */
		}
	    }
#define _VOL_DELTA 0x80
	    if (paddata & SCE_PADLup) {
		/* ボリュームを上げる */
		vol += _VOL_DELTA;
		if (vol > 0x3fff) vol = 0x3fff;
		ezAdpcm_RPC (EzADPCM_SETVOLDIRECT, _LR (vol)); /* ボリューム設定 */
	    } else if (paddata & SCE_PADLdown) {
		/* ボリュームを下げる */
		vol -= _VOL_DELTA;
		if (vol < 0) vol = 0;
		ezAdpcm_RPC (EzADPCM_SETVOLDIRECT, _LR (vol)); /* ボリューム設定 */
	    }

	} else {
	   /* セレクトボタンが押された → プログラム終了へ */

	    if (ezAdpcm_RPC (EzADPCM_GETSTATUS, 0) == EzADPCM_STATUS_IDLE) {
		/* IOP 側の処理が終了し、IDLE 状態になった */
		ezAdpcm_RPC (EzADPCM_CLOSE, 0); /* 波形データファイルをクローズ */
		/* マスターボリュームを 0 に */
		ezAdpcm_RPC (SD_CORE_0 | EzADPCM_SETMASTERVOL, 0);
		ezAdpcm_RPC (SD_CORE_1 | EzADPCM_SETMASTERVOL, 0);
		/* IOP 側終了処理 */
		ezAdpcm_RPC (EzADPCM_QUIT, 0);
		return;
	    }
	}
	sceGsSyncV(0);
	oldpad = paddata;
    }
    return;
}

/*
 * main()		メイン関数
 */
int
main (int argc, char *argv [])
{
    /* IOP モジュールのロード */
    load_modules ();

    /* コントローラ関連の初期化 */
    scePadInit (0);
    scePadPortOpen (0, 0, pad_dma_buf);

    scePrintf ("EzADPCM initialize ...\n");

    /* プログラム起動時の引数の数をチェック */
    if (argc != 3) {
	scePrintf ("\n\nusage: %s <L-ch file>.vb <R-ch file>.vb\n\n", argv [0]);
	return 0;
    }

    /* 波形データファイル名 */
    strcpy (gFilename [_L], argv [_L + 1]);
    strcpy (gFilename [_R], argv [_R + 1]);

    /* IOP 側のセットアップ */
    init_adpcm ();

    scePrintf ("EzADPCM initialize ... done\n");

    /* ストリーミング処理をコントローラで制御する */
    main_loop ();

    scePrintf ("EzADPCM finished...\n");
    return 0;
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADD STUFF AFTER THIS */
