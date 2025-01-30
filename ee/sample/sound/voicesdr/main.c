/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 * Emotion Engine Library Sample Program
 *
 * Copyright (C) 1999, 2000, 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * voicesdr - main.c
 *	main routine
 */

#include <eekernel.h>
#include <sifdev.h>
#include <libgraph.h>
#include <libpad.h>
#include <libsdr.h>

/*
 * コンパイル時オプション
 */
#define BATCH_MODE      1	/* バッチ処理を行う		*/
#define DMA_CB_TEST	1	/* DMA 割り込みのテスト		*/
#define IRQ_CB_TEST	1	/* IRQ 割り込みのテスト		*/
#define IRQ_ADDR_OFST	0x1000	/* IRQの位置 (波形先頭からの offset) */

/* IOP メモリ → SPU2 ローカルメモリに転送する時の DMA チャンネル */
#define DMA_CH 0

#define PRINTF(x) scePrintf x

/*
 * 波形データ情報
 */
#define VAG_FILENAME	"host0:/usr/local/sce/data/sound/wave/piano.vag"

/* VAG ファイルのヘッダ部分のサイズ。ヘッダは SPU2 ローカルメモリ内では不要 */
#define VAG_HEADER_SIZE 48
/* 転送先の SPU2 ローカルメモリ内の先頭アドレス */
#define VAG_SPU2_ADDR	0x25010

#define	EFFECT_DEPTH 	0x3fff

#if BATCH_MODE
/*
 * バッチ処理情報
 * --- libsdr ではバッチコマンド配列の第一要素は空にしておくこと。
 *     第一要素は引数の受渡しのため libsdr にて使用される。
 */

/* 初期化: 設定バッチ */
sceSdBatch gBatchCommonSet [] __attribute__((aligned (64))) = {	/* アライメント設定 */
    {             0,                    0,      0 }, /* sdr のみ第一要素は 0 */
    { SD_BSET_PARAM, SD_CORE_0|SD_P_MVOLL, 0x3fff },
    { SD_BSET_PARAM, SD_CORE_0|SD_P_MVOLR, 0x3fff },
    { SD_BSET_PARAM, SD_CORE_1|SD_P_MVOLL, 0x3fff },
    { SD_BSET_PARAM, SD_CORE_1|SD_P_MVOLR, 0x3fff }, /* 4 */
};

/* 初期化: 確認バッチ */
sceSdBatch gBatchCommonGet [] __attribute__((aligned (64))) = { /* アライメント設定 */
    {             0,                    0, 0 }, /* sdr のみ第一要素は 0 */
    { SD_BGET_PARAM, SD_CORE_0|SD_P_MVOLL, 0 },
    { SD_BGET_PARAM, SD_CORE_0|SD_P_MVOLR, 0 },
    { SD_BGET_PARAM, SD_CORE_1|SD_P_MVOLL, 0 },
    { SD_BGET_PARAM, SD_CORE_1|SD_P_MVOLR, 0 },	/* 4 */
    { SD_WRITE_IOP, 0x1234, 0x90000 },
    { SD_WRITE_EE,  0x4321, 0x1000f0 },
    { SD_RETURN_EE, 16, 0x100000 }, /* 7 */
};

/* ボイス初期化バッチ: CORE0 */
sceSdBatch gBatchVoiceSet_CORE0 [] __attribute__((aligned (64))) = { /* アライメント設定 */
    {             0,                                0,       0 }, /* sdr のみ第一要素は 0 */
    { SD_BSET_PARAM, SD_CORE_0|SD_VOICE_XX|SD_VP_VOLL,  0x1fff },
    { SD_BSET_PARAM, SD_CORE_0|SD_VOICE_XX|SD_VP_VOLR,  0x1fff },
    { SD_BSET_PARAM, SD_CORE_0|SD_VOICE_XX|SD_VP_PITCH, 0x0400 },
    { SD_BSET_ADDR,  SD_CORE_0|SD_VOICE_XX|SD_VA_SSA,   VAG_SPU2_ADDR },
    { SD_BSET_PARAM, SD_CORE_0|SD_VOICE_XX|SD_VP_ADSR1, 
			SD_ADSR1(SD_ADSR_A_EXP, 30, 14, 14) },
    { SD_BSET_PARAM, SD_CORE_0|SD_VOICE_XX|SD_VP_ADSR2, 
			SD_ADSR2(SD_ADSR_S_EXP_DEC, 52, SD_ADSR_R_EXP, 13) }, /* 6 */
};

/* ボイス初期化バッチ: CORE1 */
sceSdBatch gBatchVoiceSet_CORE1 [] __attribute__((aligned (64))) = { /* アライメント設定 */
    {             0,                                0,       0 }, /* sdr のみ第一要素は 0 */
    { SD_BSET_PARAM, SD_CORE_1|SD_VOICE_XX|SD_VP_VOLL,  0x1fff },
    { SD_BSET_PARAM, SD_CORE_1|SD_VOICE_XX|SD_VP_VOLR,  0x1fff },
    { SD_BSET_PARAM, SD_CORE_1|SD_VOICE_XX|SD_VP_PITCH, 0x0400 },
    { SD_BSET_ADDR,  SD_CORE_1|SD_VOICE_XX|SD_VA_SSA,   VAG_SPU2_ADDR },
    { SD_BSET_PARAM, SD_CORE_1|SD_VOICE_XX|SD_VP_ADSR1, 
			SD_ADSR1(SD_ADSR_A_EXP, 30, 14, 14) },
    { SD_BSET_PARAM, SD_CORE_1|SD_VOICE_XX|SD_VP_ADSR2, 
			SD_ADSR2(SD_ADSR_S_EXP_DEC, 52, SD_ADSR_R_EXP, 13) }, /* 6 */
};

/* エフェクト設定バッチ */
sceSdBatch gBatchEffectSet [] __attribute__((aligned (64))) = { /* アライメント設定 */
    {              0,                              0, 0 }, /* sdr のみ第一要素は 0 */
    { SD_BSET_CORE,   SD_CORE_0 | SD_C_EFFECT_ENABLE, 1 },
    { SD_BSET_PARAM,  SD_CORE_0 | SD_P_EVOLL,         EFFECT_DEPTH },
    { SD_BSET_PARAM,  SD_CORE_0 | SD_P_EVOLR,         EFFECT_DEPTH },
    { SD_BGET_SWITCH, SD_CORE_0 | SD_S_VMIXEL,        0xffffff },
    { SD_BGET_SWITCH, SD_CORE_0 | SD_S_VMIXER,        0xffffff }, /* 5 */
};
#endif

char gDoremi [8] = {36, 38, 40, 41, 43, 45, 47, 48}; 
int gEndFlag = 0;

/* 使用する IOP モジュール */
#define _MOD_ROOT    "host0:/usr/local/sce/iop/modules/"
#define _MOD_SIO2MAN "sio2man.irx"
#define _MOD_PADMAN  "padman.irx"
#define _MOD_LIBSD   "libsd.irx"
#define _MOD_SDRDRV  "sdrdrv.irx"

/* コントローラ作業用バッファ */
u_long128 pad_dma_buf [scePadDmaBufferMax] __attribute__((aligned (64)));

/* エフェクト属性設定 */
sceSdEffectAttr effect_attr __attribute__((aligned (64)));

#define PROC_BAT_MAX 7
#define PROC_BAT_ITEMS (PROC_BAT_MAX * 24)
#define PROC_BAT_SIZE (sizeof (unsigned int) * (PROC_BAT_MAX * 24))
unsigned int proc_out [PROC_BAT_ITEMS] __attribute__((aligned (64)));

/*
 * load_modules()	モジュールのロード
 *	引数:   なし
 *	返り値: なし
 */
void
load_modules (void)
{
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
    while (sceSifLoadModule (_MOD_ROOT _MOD_LIBSD, 0, NULL) < 0) {
	scePrintf ("  loading %s failed\n", _MOD_LIBSD);
    }
    /* libsdr 用ドライバモジュール */
    while (sceSifLoadModule (_MOD_ROOT _MOD_SDRDRV, 0, NULL) < 0) {
	scePrintf ("  loading %s failed\n", _MOD_SDRDRV);
    }

    scePrintf ("loading modules ... done.\n");

    return;
}

/* ----------------------------------------------------------------
 * 割り込みハンドラ
 * ---------------------------------------------------------------- */
int
IntTrans (int ch, void * common)
{
    int *c = (int *) common;

    (*c) ++;
    scePrintf ("##### interrupt detected. count: %d CORE ch: %d #####\n", *c, ch);

    return 0;	/* 現在未使用。必ず 0 を返す。 */
}

int
IntFunc (int core, void *common)
{
    int *c = (int *) common;

    (*c) ++;
    scePrintf ("///// interrupt detected (%d). CORE-bit: %d /////\n", *c, core);

    return 0;	/* 現在未使用。必ず 0 を返す。 */
}

/* ----------------------------------------------------------------
 * データ読み込み
 *	引数:   波形データサイズの返り値
 *	返り値: IOP 内の波形データ読み込み領域の先頭アドレス
 * ---------------------------------------------------------------- */
char *
set_data (int *size)
{
    int fd;
    char *buffer;

    /* ファイルのオープン */
    while ((fd = sceOpen (VAG_FILENAME, SCE_RDONLY)) < 0) {
	scePrintf ("\nfile open failed. %s\n", VAG_FILENAME);
    };
    /* ファイルサイズの取得 */
    while ((*size = sceLseek (fd, 0, SCE_SEEK_END)) <= 0) {
	scePrintf ("\nfile seek failed. %s\n", VAG_FILENAME);
    }
    sceLseek (fd, 0, SCE_SEEK_SET);		/* 読み込み位置を先頭に */
    sceClose (fd);				/* サイズが得られたので不要 */
    /* 領域確保 */
    PRINTF (("allocate IOP heap memory - "));
    while ((buffer = sceSifAllocIopHeap (*size)) == NULL) {
	scePrintf ("\nCan't alloc IOP heap \n");
    }
    PRINTF (("alloced 0x%x  \n", (int)buffer));
    /* ファイルの内容を読み込み */
    while (sceSifLoadIopHeap (VAG_FILENAME, buffer) < 0) {
	scePrintf ("\nfile load failed. %s \n", VAG_FILENAME);
    }
    PRINTF (("file load done\n"));

    return buffer;
}

/* ----------------------------------------------------------------
 * メイン処理
 * ---------------------------------------------------------------- */

int
sound_test (void)
{
    int v;
    int size;
    char *vagBuffer;
#if BATCH_MODE
    int ret;
#else
    int i, core;
#endif

    PRINTF(("voice start...\n"));

    /* ファイルの内容を IOP メモリ内に読み込み */
    if ((vagBuffer = set_data (&size)) == NULL){
	return -1;
    }

    /* 低レベルサウンドライブラリの初期化 */
    sceSdRemote (1, rSdInit, SD_INIT_COLD);

#if BATCH_MODE
    /* バッチ処理: 4 つのコマンドを実行
     * --- libsdr は第 4 引数以降の指定が libsd と異なる */
    ret = sceSdRemote (1, rSdProcBatch2, gBatchCommonSet, 4, NULL, 0);
    PRINTF (("rSdProcBatch count = %d \n", ret));

    /* バッチ処理:
     * 4 つのコマンドを実行: 返り値を領域 proc_out に出力
     * --- libsdr は第 4 引数以降の指定が libsd と異なる */
    ret = sceSdRemote (1, rSdProcBatch2, gBatchCommonGet, 4, proc_out, PROC_BAT_SIZE);
    PRINTF ((" check MVOL (L/R): %x\n",
	     proc_out));
#else
    /* マスターボリュームを設定 */
    for (i = 0; i < 2; i ++) {
	sceSdRemote (1, rSdSetParam, i | SD_P_MVOLL, 0x3fff);
	sceSdRemote (1, rSdSetParam, i | SD_P_MVOLR, 0x3fff);
    }
#endif

    /*
     * データ転送
     */
    PRINTF (("Data transfer ...\n"));
#if DMA_CB_TEST
    /* DMA 転送終了割り込みハンドラを設定 */
    sceSdRemote (1, rSdSetTransIntrHandler,
		 DMA_CH, (sceSdSpu2IntrHandler)IntTrans, (void *) &gEndFlag);
#endif
    /* 転送開始 */
    sceSdRemote (1, rSdVoiceTrans, DMA_CH,
		 SD_TRANS_MODE_WRITE | SD_TRANS_BY_DMA,
		 (u_char *)(vagBuffer + VAG_HEADER_SIZE),
		 VAG_SPU2_ADDR, (u_int)size - VAG_HEADER_SIZE);
    PRINTF (("TSA = %x \n",     sceSdRemote (1, rSdGetAddr, DMA_CH | SD_A_TSA)));

#if DMA_CB_TEST
    while (gEndFlag == 0) {	/* 割り込みハンドラが呼ばれるまで待つ */
	sceGsSyncV (0);
    }
#else
    sceSdRemote (1, rSdVoiceTransStatus, DMA_CH, SD_TRANS_STATUS_WAIT); /* 転送終了を待つ */
#endif

#if IRQ_CB_TEST
    /*
     * SPU2 割り込みの設定
     */
    /* SPU2 割り込みが起こるアドレスを設定
     * ... SPU2 ローカルメモリ内の、波形データの先頭から 0x1000 の位置 */
    sceSdRemote (1, rSdSetAddr, SD_CORE_0 | SD_A_IRQA, VAG_SPU2_ADDR + IRQ_ADDR_OFST);
    gEndFlag = 0;
    /* SPU2 割り込みハンドラの登録 */
    sceSdRemote (1, rSdSetSpu2IntrHandler, (sceSdSpu2IntrHandler)IntFunc, (void *) &gEndFlag);
    /* SPU2 割り込みを有効に (CORE0 側) */
    sceSdRemote (1, rSdSetCoreAttr, SD_CORE_0 | SD_C_IRQ_ENABLE, 1);
#endif

#if BATCH_MODE
    /* バッチ処理: 指定ボイスを一括処理
     * --- libsdr は第 4 引数以降の指定が libsd と異なる
     * 第 7 引数/0xffffff ... 全てのビットが 1 = ボイス 0 ～ 23 を全て設定 */
    ret = sceSdRemote (1, rSdProcBatchEx2, gBatchVoiceSet_CORE0, 6, NULL,     0,             0xffffff);
    ret = sceSdRemote (1, rSdProcBatchEx2, gBatchVoiceSet_CORE1, 6, proc_out, PROC_BAT_SIZE, 0xffffff);
    PRINTF (("sceSdProcBatchEx count = %d \n", ret));
#else
    for (core = 0; core < 2; core ++) {
	for (v = 0; v < 24; v ++) {
	    /* ボイスの属性をそれぞれ設定 */
	    sceSdRemote (1, rSdSetParam, core | (v << 1) | SD_VP_VOLL,  0x1eff);
	    sceSdRemote (1, rSdSetParam, core | (v << 1) | SD_VP_VOLR,  0x1eff);
	    sceSdRemote (1, rSdSetParam, core | (v << 1) | SD_VP_PITCH, 0x0400);
	    sceSdRemote (1, rSdSetAddr,  core | (v << 1) | SD_VA_SSA,   VAG_SPU2_ADDR);
	    sceSdRemote (1, rSdSetParam, core | (v << 1) | SD_VP_ADSR1,
			 SD_ADSR1 (SD_ADSR_A_EXP, 30, 14, 14));
	    sceSdRemote (1, rSdSetParam, core | (v << 1) | SD_VP_ADSR2,
			 SD_ADSR2 (SD_ADSR_S_EXP_DEC, 52, SD_ADSR_R_EXP, 13));
	}
    }
#endif

    /*
     * エフェクト属性の設定
     */
    effect_attr.depth_L  = 0;	/* 最初は 0 に */
    effect_attr.depth_R  = 0;
    /* effect_attr.delay    = 30;  *//* モード: ホールには不要 */
    /* effect_attr.feedback = 200; *//* モード: ホールには不要 */
    /* モード: ホール + ワークエリアを初期化 */
    effect_attr.mode = SD_REV_MODE_HALL | SD_REV_MODE_CLEAR_WA;
    sceSdRemote (1, rSdSetEffectAttr, SD_CORE_0, &effect_attr);

#if BATCH_MODE
    /* バッチ処理: 5 つのコマンドを実行
     * --- libsdr は第 4 引数以降の指定が libsd と異なる */
    ret = sceSdRemote (1, rSdProcBatch2, gBatchEffectSet, 5, NULL, 0);
    PRINTF (("rSdProcBatch count = %d \n", ret));
#else
    /* CORE0: エフェクト有効 */
    sceSdRemote (1, rSdSetCoreAttr, SD_CORE_0 | SD_C_EFFECT_ENABLE, 1);
    /* CORE0: エフェクトセンドボリューム */
    sceSdRemote (1, rSdSetParam,    SD_CORE_0 | SD_P_EVOLL, EFFECT_DEPTH);
    sceSdRemote (1, rSdSetParam,    SD_CORE_0 | SD_P_EVOLR, EFFECT_DEPTH);
    /* CORE0: 全てのボイスの L/R 出力をエフェクトに接続  */
    sceSdRemote (1, rSdSetSwitch,   SD_CORE_0 | SD_S_VMIXEL, 0xffffff);
    sceSdRemote (1, rSdSetSwitch,   SD_CORE_0 | SD_S_VMIXER, 0xffffff);
#endif

    // Ring!
    for (v = 15; v < 23; v ++) {
	unsigned int pitch, c;
#if IRQ_CB_TEST
	/* SPU2 割り込みが起きた場合、一旦無効にして再設定する */
	sceSdRemote (1, rSdSetCoreAttr, SD_CORE_0 | SD_C_IRQ_ENABLE, 0);
	sceSdRemote (1, rSdSetCoreAttr, SD_CORE_0 | SD_C_IRQ_ENABLE, 1);
#endif
	/* ピッチ設定 */
	pitch = sceSdRemote (1, rSdNote2Pitch, 60, 0, gDoremi[v - 15], 0);
	sceSdRemote (1, rSdSetParam, SD_CORE_0 | (v << 1) | SD_VP_PITCH, pitch);

	/* 発音 */
	sceSdRemote (1, rSdSetSwitch, SD_CORE_0 | SD_S_KON, 1 << v);

	PRINTF ((" ++ key_on  pitchA %x, pitchB %x \n", 
		 pitch,
		 sceSdRemote (1, rSdGetParam, SD_CORE_0 | (v << 1) | SD_VP_PITCH)));
	
	for (c = 0; c < 60; c ++) { /* 1 秒待つ */
	    sceGsSyncV (0);
	}

	/* 消音 */
	sceSdRemote (1, rSdSetSwitch, SD_CORE_0 | SD_S_KOFF, 1 << v);
    }

    /* 念のため CORE0 全てのボイスを消音 */
    sceSdRemote (1, rSdSetSwitch, SD_CORE_0 | SD_S_KOFF, 0xffffff);

    PRINTF(("voice completed...\n"));

    return 0;
}

/* ----------------------------------------------------------------
 * スタート関数
 * ---------------------------------------------------------------- */

int
main (int argc, char *argv [])
{
    int thid;

    /* SIF RPC の初期化 */
    sceSifInitRpc (0);

    /* IOP モジュールのロード */
    load_modules ();

    /* コントローラ関連の初期化 */
    scePadInit (0);
    scePadPortOpen (0, 0, pad_dma_buf);

    /* IOP heap 処理の初期化 */
    sceSifInitIopHeap ();

    thid = GetThreadId ();
    ChangeThreadPriority (thid , 10);

    /* libsdr の初期化 */
    sceSdRemoteInit();
    thid = sceSdRemoteCallbackInit (5);

    /* メイン処理 */
    sound_test ();

    /* コールバック・スレッドの削除 */
    if (TerminateThread (thid) == thid) {
	DeleteThread (thid);
    }

    return 0;
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADD STUFF AFTER THIS */
