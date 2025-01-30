/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 * Emotion Engine Library Sample Program
 *
 * Copyright (C) 2001, 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#include <eekernel.h>
#include <sif.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libpad.h>
#include <libgraph.h>		/* sceGsSyncV() */
#include <libsdr.h>
#include <libspu2m.h>

#include <sk/common.h>
#include <sk/errno.h>
#include <sk/sk.h>
#include <sk/sound.h>

/*
 * IOP 使用モジュール
 * ---------------------------------------------------------------- */

#define _MOD_ROOT    "host0:/usr/local/sce/iop/modules/"

#define _MOD_SIO2MAN   "sio2man.irx"
#define _MOD_PADMAN    "padman.irx"
#define _MOD_LIBSD     "libsd.irx"
#define _MOD_SDRDRV    "sdrdrv.irx"
#define _MOD_MODHSYN   "modhsyn.irx"
#define _MOD_MODMIDI   "modmidi.irx"
#define _MOD_SK_SOUND  "sksound.irx"
#define _MOD_SK_HSYNTH "skhsynth.irx"
#define _MOD_SK_MIDI   "skmidi.irx"

/*
 * variables
 * ---------------------------------------------------------------- */

// スレッド ID
static int thid_main, thid_sound;

// セマフォ
int sema_main;

// メイン処理スレッドのスタック
#define STACK_SIZE_MAIN (512 * 128)
static unsigned char stack_main [STACK_SIZE_MAIN] __attribute__((aligned(16)));

// 転送用 EE 側一時バッファ
#define EEBUF_SIZE (2048 * 64)
static unsigned char eebuf [EEBUF_SIZE] __attribute__((aligned(64)));

// 転送用 IOP 側一時バッファのサイズ
#define IOPBUF_SIZE (64 * 1024)			/* 64 KB */

// sceSpu2MemAllocate() により SPU2 ローカルメモリで 32 領域確保可能 
#define SPU2MEM_AREANUM 32
static unsigned char spu2mem_table [SCESPU2MEM_TABLE_UNITSIZE * (SPU2MEM_AREANUM + 1)];

/*
 * functions
 * ---------------------------------------------------------------- */

// 再読み込み機能付 read 関数
static int
readX (int fd, unsigned char *rbuf, int size)
{
    int req = size;
    int red = 0;
    int ret = 0;

    while (red < size) {	/* 要求量だけファイルの内容を読む */
	ret = sceRead (fd, &(rbuf [red]), req);
	if (ret < 0) {
	    red = -1;
	    break;
	}
	red += ret;
	if (ret < req) {	/* 読めたバイト数が要求量より少ない */
	    req -= ret;		/* 読めたバイト数だけ要求量を減らす */
	}
    }

    return red;			/* 最終的に読めたバイト数 or (-1) */
}

/*
 * 転送関数
 *	ファイルの内容 → EE メモリ (一時バッファ) →
 *		IOP メモリ (一時バッファ) → SPU2 ローカルメモリ
 *    引数:
 *	fname:		ファイル名
 *	buf:		EE メモリ内の一時バッファの先頭アドレス
 *      bufsize:	EE メモリ内の一時バッファのサイズ
 *	ret_spu_addr:	(返り値) 確保した SPU2 ローカルメモリ内の領域の先頭アドレス
 *	ret_size:	(返り値) 確保した SPU2 ローカルメモリ内の領域のサイズ
 *    返り値:
 *	0:		正常終了
 *	< 0:		異常終了
 */
static int
sendtoSPU (char *fname,
	   char *buf, unsigned int bufsize,
	   unsigned int *ret_spu_addr, unsigned int *ret_size)
{
    int fd, fsize, red, req;
    unsigned char *iop_addr;
    unsigned int iop_size;
    unsigned int spu2_addr, saddr;
    int sent_size, rest, rsize;
    sceSifDmaData ddata;

    // オープン
    if ((fd = sceOpen (fname, SCE_RDONLY)) < 0) {
	// Can't open file [fname]
	return (-1);
    }

    // ファイルサイズを取得
    if ((fsize = sceLseek (fd, 0, SCE_SEEK_END)) <= 0) {
	// File read something wrong [fname]
	sceClose (fd);
	return (-2);
    }
    sceLseek (fd, 0, SCE_SEEK_SET); /* 先頭に戻す */

    // IOP メモリに一時領域を確保
    iop_size = IOPBUF_SIZE;
    if (iop_size > bufsize) {
	iop_size = bufsize;
    }
    if ((iop_addr = (unsigned char *) sceSifAllocSysMemory (0, iop_size, NULL)) == NULL) {
	// IOP heap, not enough memory [iop_size]
	sceClose (fd);
	return (-3);
    }

    // SPU2 ローカルメモリに領域を確保
    if ((spu2_addr = sceSpu2MemAllocate (fsize)) < 0) {
	// SPU2 local memory, not enough memory [fsize]
	sceSifFreeSysMemory ((void *)iop_addr);	
	sceClose (fd);
	return (-4);
    }

    // 分割転送
    sent_size = 0;
    ddata.data = (unsigned int)buf;		// EE top (fixed area)
    ddata.addr = (unsigned int)iop_addr;	// IOP top (fixed area)
    ddata.size = iop_size;			// size
    ddata.mode = 0;				// always 0
    saddr = spu2_addr;
    while (sent_size < fsize) {
	rest = fsize - sent_size;		// 残りサイズを計算
	if (rest > iop_size) rest = iop_size;	// 一時バッファのサイズに
	rsize = readX (fd, buf, rest);		// データをバッファに読み込み
	if (rsize < 0) {	// 読み込み失敗
	    // File Read Error
	    sceSifFreeSysMemory ((void *)iop_addr);
	    sceSpu2MemFree (spu2_addr);
	    sceClose (fd);
	    return (-4);
	}
	FlushCache (0);
	ddata.size = rest;			// 転送サイズ
	if (sceSifSetDma (&ddata, 1) == 0) {	// 転送
	    // EE -> IOP transfer Error
	    sceSifFreeSysMemory ((void *)iop_addr);
	    sceSpu2MemFree (spu2_addr);
	    sceClose (fd);
	    return (-4);
	}
	sceSkSsSend (SCESK_SEND_IOP2SPU2,	// IOP memory -> SPU2 local memory
		     1,	/* DMA ch */
		     iop_addr, saddr, rest);
	sent_size  += rest;
	saddr      += rest;
    }

    // IOP メモリ内の一時バッファ領域を解放
    sceSifFreeSysMemory ((void *)iop_addr);

    // クローズ
    sceClose (fd);

    // 返り値
    *ret_spu_addr = (unsigned int)spu2_addr;
    *ret_size     = (unsigned int)fsize;

    return (0);
}

/*
 * 転送関数
 *	ファイルの内容 → IOP メモリ
 *
 *    引数:
 *	fname:		ファイル名
 *	ret_iop_addr:	(返り値) 確保した IOP 内の領域の先頭アドレス
 *	ret_size:	(返り値) 確保した IOP 内の領域のサイズ
 *    返り値:
 *	0:		正常終了
 *	< 0:		異常終了
 */
static int
sendtoIOP (char *fname,
	   unsigned int *ret_iop_addr, unsigned int *ret_size)
{
    int fd, fsize, red, req;
    unsigned char *iop_addr;
    int sent_size, rest, rsize;
    sceSifDmaData ddata;

    // オープン
    if ((fd = sceOpen (fname, SCE_RDONLY)) < 0) {
	// Can't open file [fname]
	return (-1);
    }

    // ファイルサイズを取得
    if ((fsize = sceLseek (fd, 0, SCE_SEEK_END)) <= 0) {
	// File read something wrong [fname]
	sceClose (fd);
	return (-2);
    }
    sceLseek (fd, 0, SCE_SEEK_SET); /* 先頭に戻す */

    // IOP メモリに領域を確保
    if ((iop_addr = (unsigned char *) sceSifAllocSysMemory (0, fsize, NULL)) == NULL) {
	// IOP heap not enough memory [fsize]
	sceClose (fd);
	return (-3);
    }

    // ファイルサイズが取得できたのでファイルをクローズ
    sceClose (fd);

    // ファイルの全内容を IOP メモリに読み込む
    if (sceSifLoadIopHeap (fname, iop_addr) < 0) {
	// File read is something wrong
	sceSifFreeSysMemory (iop_addr);
	return (-4);
    }

    // 確保した IOP メモリ内の領域の情報を返す
    *ret_iop_addr = (unsigned int)iop_addr;
    *ret_size     = (unsigned int)fsize;

    return (0);
}

/*
 * メイン処理スレッド
 *	サウンドデータを読み込んで演奏。演奏終了後、スレッド終了
 *
 *    引数:
 *	argp:	処理終了時に signal を出すセマフォ ID
 */
static void
main_sound (void *argp)
{
    int ret;
    unsigned int sq_addr, sq_size;
    unsigned int hd_addr, hd_size;
    unsigned int bd_addr, bd_size;
    int hdbd_id, sq_id, track_id;

    /*
     * データの読み込み
     * ---------------------------------------------------------------- */

    /*
     * SQ データの読み込み
     */
    if ((ret = sendtoIOP ("host0:/usr/local/sce/data/sound/seq/overload.sq",
			  &sq_addr, &sq_size)) < 0) {
	SignalSema (*(int *)argp);		// main() に戻る
	return;
    }
    scePrintf ("SQ: (%08x) %d\n", sq_addr, sq_size);

    /*
     * HD データの読み込み
     */
    if ((ret = sendtoIOP ("host0:/usr/local/sce/data/sound/wave/overload.hd",
			  &hd_addr, &hd_size)) < 0) {
	sceSifFreeSysMemory ((void *)sq_addr);
	SignalSema (*(int *)argp);		// main() に戻る
	return;
    }
    scePrintf ("HD: (%08x) %d\n", hd_addr, hd_size);

    /*
     * BD データの読み込み ... IOP メモリからさらに SPU2 ローカルメモリへ転送
     *                     ... bd_addr: SPU2 ローカルメモリ内のアドレス
     */
    if ((ret = sendtoSPU ("host0:/usr/local/sce/data/sound/wave/overload.bd",
			  eebuf, EEBUF_SIZE, &bd_addr, &bd_size)) < 0) {
	sceSifFreeSysMemory ((void *)hd_addr);
	sceSifFreeSysMemory ((void *)sq_addr);
	SignalSema (*(int *)argp);		// main() に戻る
	return;
    }
    scePrintf ("BD: (%08x) %d\n", bd_addr, bd_size);
    
    /*
     * Standard Kit / Sound System ... 各データの登録
     * ---------------------------------------------------------------- */

    /*
     * HD/BD データの登録
     */
    hdbd_id = sceSkSsBindHDBD (SCESK_AUTOASSIGNMENT,
			       hd_addr, hd_size,  /* HD アドレス/サイズ情報 */
			       bd_addr, bd_size); /* BD アドレス/サイズ情報 */

    scePrintf ("BIND HDBD / return: %d\n", hdbd_id);

    /*
     * SQ データの登録
     */
    sq_id = sceSkSsBindSQ (MIDI, SCESK_AUTOASSIGNMENT,
			   sq_addr, sq_size);	/* SQ アドレス/サイズ情報 */

    scePrintf ("BIND SQ (MIDI) / return: %d\n", sq_id);

    /*
     * HD/BD データ ID を、トラック (ID: 自動割り当て) に登録
     */
    track_id = sceSkSsBindTrackHDBD (SCESK_AUTOASSIGNMENT,
				     hdbd_id,
				     0);	/* バンク番号: 0 */

    scePrintf ("BIND TRACK (HDBD) / return: %d\n", track_id);

    /*
     * SQ データ ID を、HD/BD データ ID を登録したトラックに登録
     */
    ret = sceSkSsBindTrackSQ (MIDI, track_id, sq_id);

    scePrintf ("BIND TRACK (SQ (MIDI)) / return: %d\n", ret);

    /*
     * Standard Kit / Sound System ... 演奏
     * ---------------------------------------------------------------- */

    /*
     * MIDI チャンクを演奏
     *		0: MIDI Data Block #
     */
    ret = sceSkSsPlayMIDI (track_id, 0);

    scePrintf ("Play (SQ (MIDI)) %d / return: %d\n", track_id, ret);

    /*
     * 再生指定直後は、IOP 側の再生が始まっていない場合がある。
     * ... 演奏開始後、sceSkSsStatusMIDI(,SCESK_ISPLAYING) が 0 以外を
     *     返せば、演奏が開始されている
     */
    while (1) {
	if (sceSkSsStatusMIDI (track_id, SCESK_ISPLAYING) != 0) {
	    break;
	}
	sceGsSyncV (0);
    }

    /*
     * 演奏終了を待つ
     */
    while (1) {
	if (! sceSkSsStatusMIDI (track_id, SCESK_ISPLAYING)) {
	    break;
	}
	sceGsSyncV (0);
    }

    /*
     * MIDI チャンクの演奏を停止
     */
    ret = sceSkSsStopMIDI (track_id);

    scePrintf ("Stop (SQ (MIDI)) %d / return: %d\n", track_id, ret);

    /*
     * Standard Kit / Sound System ... 終了処理
     * ---------------------------------------------------------------- */

    /*
     * トラックから HD/BD データ ID の割り当てを解除
     */
    ret = sceSkSsUnbindTrackHDBD (track_id);

    scePrintf ("UNBIND TRACK (HDBD) / return: %d\n", ret);

    /*
     * トラックから SQ データ ID の割り当てを解除
     */
    ret = sceSkSsUnbindTrackSQ (MIDI, track_id);

    scePrintf ("UNBIND TRACK ((SQ (MIDI)) / return: %d\n", ret);

    /*
     * HD/BD データ ID を解除
     */
    ret = sceSkSsUnbindHDBD (hdbd_id);

    scePrintf ("UNBIND HDBD / return: %d\n", ret);

    /*
     * SQ データ ID を解除
     */
    ret = sceSkSsUnbindSQ (MIDI, sq_id);

    scePrintf ("UNBIND SQ (MIDI) / return: %d\n", ret);

    /*
     * 領域を解放
     * ---------------------------------------------------------------- */

    /*
     * sendtoSPU() 内で確保した、BD データ (SPU2 ローカルメモリ内) の領域を解放
     */
    sceSpu2MemFree (bd_addr);

    /*
     * sendtoIOP() 内で確保した、SQ/HD データ (IOP メモリ内) の領域を解放
     */
    sceSifFreeSysMemory ((void *)hd_addr);
    sceSifFreeSysMemory ((void *)sq_addr);

    /*
     * メイン処理スレッドを終了
     * ---------------------------------------------------------------- */

    SignalSema (*(int *) argp);		// main() に戻る
    return;
}

/*
 * SPU2 設定
 */
void
setup_spu2 (void)
{
    // 光デジタル出力の設定
    sceSkSsSetDigitalOut (SCESK_DOUT_CD_NORMAL);

    // エフェクトの設定
    sceSkSsSetEffectMode (SCESK_EFFECT_MODE_HALL | SCESK_EFFECT_MODE_CLEAR_WA,
			  0, 0); /* delay, feedback */
    sceSkSsSetEffect (SCESK_EFFECT_ON); 
    sceSkSsSetEffectVolume (SCESK_EFFECT_VOLUME_MAX / 2,  /* L */
			    SCESK_EFFECT_VOLUME_MAX / 2); /* R */

    /*
     * マスターボリューム設定
     */
    sceSkSsSetMasterVolume (SCESK_MASTER_VOLUME_MAX,  /* L */
			    SCESK_MASTER_VOLUME_MAX); /* R */

    return;
}

#define LOAD_MODULE(mod) \
while (sceSifLoadModule (_MOD_ROOT mod, 0, NULL) < 0) { \
    scePrintf ("  loading %s failed\n", mod); \
}

/*
 * スタートエントリ
 */
int
main (int argc, char *argv [])
{
    struct ThreadParam tp;
    struct SemaParam sp;

    // SIF 関連の初期化
    sceSifInitRpc (0);
    sceSifInitIopHeap ();

    // モジュールのロード
    LOAD_MODULE (_MOD_SIO2MAN);
    //LOAD_MODULE (_MOD_PADMAN);	/* このサンプルではコントローラは未使用 */
    LOAD_MODULE (_MOD_LIBSD);		/* 低レベルサウンドライブラリ */
    LOAD_MODULE (_MOD_SDRDRV);		/* 低レベルサウンドライブラリ (for EE) */
    LOAD_MODULE (_MOD_MODHSYN);		/* ハードウェアシンセサイザ */
    LOAD_MODULE (_MOD_MODMIDI);		/* MIDI シーケンサ */
    LOAD_MODULE (_MOD_SK_SOUND);	/* 標準キット: サウンドファンダメンタル */
    LOAD_MODULE (_MOD_SK_HSYNTH);	/* 標準キット: ハードウェアシンセサイザ */
    LOAD_MODULE (_MOD_SK_MIDI);		/* 標準キット: MIDI シーケンサ */

    // Standard Kit ... 初期化
    sceSkInit (0);		/* Standard Kit 全体の初期化 */
    sceSkSsInit (32);		/* SK/Sound System の初期化:
				 *   引数 ... sdr 割り込みハンドラスレッド優先度 */
    // SPU2 メモリ管理の初期化
    sceSpu2MemInit (spu2mem_table, SPU2MEM_AREANUM, /* 領域数: SPU2MEM_AREANUM */
		    SCESPU2MEM_USE_EFFECT);	    /* エフェクトは使用する */

    // SPU2 の全般に関わる属性の設定
    setup_spu2 ();

    /*
     * 待ち受けセマフォの生成
     */
    sp.initCount = 0;
    sp.maxCount  = 1;
    sp.option    = 0;
    if ((sema_main = CreateSema (&sp)) < 0) { /* メイン処理スレッド終了用 */
	// Can't create semaphore
	return (-1);
    }

    /*
     * メイン処理スレッドの生成と起動
     */
    tp.stackSize    = STACK_SIZE_MAIN;
    tp.gpReg 	    = &_gp;
    tp.entry 	    = main_sound;
    tp.stack 	    = (void *) stack_main;
    tp.initPriority = 3;
    if ((thid_main = CreateThread (&tp)) < 0) {
	// Can't create thread
	return (-2);
    }
    StartThread (thid_main, (void *)&sema_main);

    // メイン処理スレッドの終了を待つ
    WaitSema (sema_main);

    // リソースを解放
    if (TerminateThread (thid_main) == thid_main) {
	DeleteThread (thid_main);
    }
    DeleteSema (sema_main);

    // SPU2 メモリ管理の終了
    sceSpu2MemQuit();

    // Fine ...
    scePrintf ("Fine ...\n");

    return (0);
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADD STUFF AFTER THIS */
