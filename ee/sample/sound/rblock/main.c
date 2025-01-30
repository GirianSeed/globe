/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                           - rblock -
 *
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999, 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                             main.c
 *
 *     Version   Date          Design     Log
 *  --------------------------------------------------------------------
 *     1.50      Mar.30.2000   kaol       For new DMA/SPU2 interrupt handler
 *     0.60      Oct.14.1999   morita     first checked in.
 */

#include <eekernel.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libgraph.h>
#include <libsdr.h>
#include <sdrcmd.h>

/* ループする場合 1 に */
#define IS_LOOP		0

/* L/R 512 バイトでインタリーブされたストレート PCM 波形データファイル */
#define PCM_FILENAME	"host0:/usr/local/sce/data/sound/wave/knot.int"
#define PCM_SIZE	1604888
#define ALLOC_IOP_HEAP_SIZE   1604888

// デバッグプリント
//#define PRINTF(x)
#define PRINTF(x) scePrintf x
#define TRANS_CH  0

int gCallbackThreadID;		/* libsdr のコールバックスレッドのスレッド ID */
int gMainThreadID;		/* このプログラム自身のスレッド ID */
static int gDataTopAddr;	/* IOP 側でデータが置かれた領域の先頭アドレス */

volatile int trans_flag = 0;	/* DMA 転送割り込みをカウントアップ */
volatile int intr_flag = 0;	/* SPU2 割り込みをカウントアップ */

/*
 * ファイルの内容を IOP メモリ内に転送
 */
void dataset(void)
{
    /* IOP 側のヒープ領域を操作するための初期化 */
    sceSifInitIopHeap();

    /* IOP メモリ内の領域を確保 */
    PRINTF(("allocate IOP heap memory - " ));
    while ((gDataTopAddr = (int)(int *)sceSifAllocSysMemory (0, ALLOC_IOP_HEAP_SIZE, NULL)) == (int)NULL) {
        scePrintf( "\nCan't alloc heap \n");
    }
    /* ファイルの内容を IOP メモリ内に転送 */
    PRINTF(("alloced 0x%x  ", gDataTopAddr));
    while (sceSifLoadIopHeap (PCM_FILENAME, (void*)gDataTopAddr) < 0) {
        scePrintf( "\nCan't load PCM file to iop heap \n"); 
    }
    PRINTF(("- data loaded 0x%x \n", (int)gDataTopAddr ));
    
    return;
}

/* 転送終了割り込みハンドラ */
int
IntTrans (int core, void *common)
{
    (* ((int *) common)) ++;
    PRINTF (("///// AutoDMA interrupt detected (%d). /////\n",
	     (* ((int *) common))));
    return 0;
}

/* SPU2 割り込みハンドラ */
int
IntSPU2 (int core_bit, void *common)
{
    /* SPU2 割り込みを無効に */
    sceSdRemote (1, rSdSetCoreAttr, SD_CORE_0 | SD_C_IRQ_ENABLE, 0);
    (* ((int *) common)) ++;
    PRINTF (("##### SPU2 interrupt detected (%d). ######\n",
	     (* ((int *) common))));
    return 0;
}

/*
 * ブロック転送の処理
 */
int rAutoDma( void )
{
    int do_intr;

    // 低レベルサウンドライブラリの初期化
    sceSdRemote( 1, rSdInit , SD_INIT_COLD );

    // SPU2 の属性の設定
    sceSdRemote( 1, rSdSetParam, SD_CORE_0 | SD_P_MVOLL , 0x3fff );
    sceSdRemote( 1, rSdSetParam, SD_CORE_0 | SD_P_MVOLR , 0x3fff );
    sceSdRemote( 1, rSdSetParam, SD_CORE_1 | SD_P_MVOLL , 0x3fff );
    sceSdRemote( 1, rSdSetParam, SD_CORE_1 | SD_P_MVOLR , 0x3fff );
    // ボイスミキシング: サウンドデータ入力はエフェクト側に入れない
    sceSdRemote( 1, rSdSetParam, SD_CORE_0 | SD_P_MMIX ,
		 ((~(SD_MMIX_MINEL | SD_MMIX_MINER)) & 0x0ff0));
    sceSdRemote( 1, rSdSetParam, SD_CORE_1 | SD_P_MMIX ,
		 ((~(SD_MMIX_MINEL | SD_MMIX_MINER)) & 0x0fff));

    // 割り込み回数変数の初期化
    trans_flag = intr_flag = 0;
    // SPU2 割り込みアドレスの設定
    sceSdRemote (1, rSdSetAddr, SD_CORE_0 | SD_A_IRQA, 0x4100);
    // 転送、SPU2 の各割り込みハンドラの設定
    sceSdRemote (1, rSdSetTransIntrHandler, TRANS_CH, IntTrans, &trans_flag); 
    sceSdRemote (1, rSdSetSpu2IntrHandler,            IntSPU2,  &intr_flag);

#if IS_LOOP
#define TRANS_MODE (SD_TRANS_MODE_WRITE | SD_BLOCK_LOOP)
#define TRANS_SIZE (PCM_SIZE/512)*512
#else
#define TRANS_MODE (SD_TRANS_MODE_WRITE | SD_BLOCK_ONESHOT)
#define TRANS_SIZE PCM_SIZE
#endif
    // ブロック転送の開始
    sceSdRemote (1, rSdBlockTrans, TRANS_CH, TRANS_MODE, gDataTopAddr, TRANS_SIZE);
    // サウンドデータ入力のボリュームを上げる
    sceSdRemote (1, rSdSetParam, TRANS_CH|SD_P_BVOLL , 0x3fff);
    sceSdRemote (1, rSdSetParam, TRANS_CH|SD_P_BVOLR , 0x3fff);

    do_intr = 0;
    while (1) {
#if !IS_LOOP
	// 0 を返したらブロック転送が終了
	if (sceSdRemote (1, rSdBlockTransStatus, TRANS_CH) == 0) {
	    break;
	}
#endif
	if ((trans_flag % 2) == 1) {
	    if (do_intr == 0) {
		// SPU2 割り込みを有効に
		sceSdRemote (1, rSdSetCoreAttr, SD_CORE_0 | SD_C_IRQ_ENABLE, 1);
		do_intr = 1;
	    }
	} else {
	    if (do_intr == 1) do_intr = 0;
	}
	sceGsSyncV(0);
    }
#if !IS_LOOP
    scePrintf ("waiting .");
    while (trans_flag == 0) {
	scePrintf (".");
    }
    scePrintf ("\n");
#endif
    // 終了処理
    sceSdRemote(1, rSdBlockTrans, TRANS_CH, SD_TRANS_MODE_STOP, NULL, 0 );
    if (TerminateThread( gCallbackThreadID ) == gCallbackThreadID) {
	DeleteThread( gCallbackThreadID );
    }
    PRINTF(("Quit...\n"));

    return 0;
}

int
main(void)
{
    // SIF RPC API の初期化 
    sceSifInitRpc(0);

    // IOP モジュールのロード
    while (sceSifLoadModule("host0:../../../../iop/modules/libsd.irx",
                        0, NULL) < 0){
        scePrintf("Can't load module libsd\n");
    }
    while (sceSifLoadModule("host0:../../../../iop/modules/sdrdrv.irx",
                        0, NULL) < 0){
        scePrintf("Can't load module sdrdrv\n");
    }

    // Callback Threadを動かすために、mainを最高プライオリティから下げる
    gMainThreadID = GetThreadId();
    ChangeThreadPriority(gMainThreadID , 10 );

    // libsdr の初期化
    sceSdRemoteInit();

    // Callback Threadをmainよりも高いプライオリティで作る
    gCallbackThreadID = sceSdRemoteCallbackInit(5);
    dataset();
    
    rAutoDma();

    return 0;
}
