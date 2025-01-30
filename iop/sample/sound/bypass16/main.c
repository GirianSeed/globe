/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0.2
 */
/*
 *                     I/O Processor Library
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                          bypass16 - main.c
 *         16 bit PCM data streaming with SPU2 bypass processing by libsd
 *
 *     Version   Date          Design   Log
 *  --------------------------------------------------------------------
 *               May 2001      kaol     first implementation
 */

#include <kernel.h>
#include <stdio.h>
#include <sys/file.h>
#include <libsd.h>

/*
 * CORE0 MEMIN “バイパス処理”で要求されるデータ形式
 * ----------------------------------------------------------------
 *   - 512 バイト L/R インターリーブされたデータ (CORE[0,1] MEMIN で共通)
 *   - Lch 部分は、16 bit PCM データを、インターリーブ単位ブロックの先頭
 *     から L, R, L, R ... と L/R 各 128 サンプル、計 256 サンプル
 *     (合計 512 バイト) 分並べる
 *   - Rch 部分は全て 0 (計 512 バイト)
 */
/*
 * Data format for CORE0 MEMIN bypass processing:
 * ----------------------------------------------------------------
 *   - 512-byte L/R interleaved data (common in CORE[0,1] MEMIN)
 *   - Lch part is filled with valid 16 bit PCM data;
 *     L, R, L, R, ... successively, L/R 128 samples, sum. is 256 samples.
 *   - Rch part must be filled with 0 (512 bytes)
 */
#define NORMAL_INTERLEAVE 0
#define BYPASS_INTERLEAVE 1

#define WAVE_MODE BYPASS_INTERLEAVE

#if   WAVE_MODE == NORMAL_INTERLEAVE
#define WF_FSIZE 1605632
#define WF_FILE "host1:/usr/local/sce/data/sound/wave/knot2.int"
#elif WAVE_MODE == BYPASS_INTERLEAVE
#define WF_FSIZE 3211264
#define WF_FILE "host1:/usr/local/sce/data/sound/wave/knot2.inb"
#endif

/* WF_FILE is already 512-byte L/R interleaved */
#define WF_SIZE  (WF_FSIZE + 0)  /* unit: 1024 bytes */

/* filling CORE0 MEMIN area with 0 */
char _0_buf [0x1000];

/* transfer will loop */
#define TRANSFER_LOOP

/* Transfer Handler */
#define TRANSFER_NORMAL  0
#define TRANSFER_HANDLER 1
#define TRANSFER_MODE TRANSFER_NORMAL

typedef struct TransferStatus {
    unsigned char *addr;	/* 波形データバッファ先頭アドレス	*/
    int count;			/* ハンドラ呼び出し回数			*/
} TransferStatus;
volatile TransferStatus status = {
    NULL, -1
};

#ifdef TRANSFER_LOOP
/* interrupt handler:
   -- called at the half/end point of buffer */
int
transfer_intr (int ch, void *common)
{
    int *c = (int *)common;

    (*c) ++;
    Kprintf ("    ##### interrupt occurred: count = %2d #####\n", *c);

    return 0;	/* 現在未使用。必ず 0 を返す。 */
}
#endif

#if TRANSFER_MODE == TRANSFER_HANDLER
int
transfer_handler (int ch, void *data, void **addr, int *size)
{
    TransferStatus *s = (TransferStatus *)data;
    unsigned char *a;

    a = s->addr;
    if (s->count % 2 == 0) {
	a = s->addr + WF_SIZE / 2;
    }
    *addr = (void *)a;
    *size = WF_SIZE / 2;
    s->count ++;
    if (s->count >= 10) {
	*size = 0;
    }
    Kprintf ("    ##### interrupt occurred: count = %2d #####\n", s->count);

    return 0;	/* 現在未使用: 必ず 0 を返す */
}
#endif

int
start (int argc, char *argv[])
{
    int oldei;

    printf ("\n");
    printf ("* SPU2 bypass processing sample (with libsd):\n");

    /*  initialize sound device (libsd) */
    /* libsd の初期化 */
    sceSdInit (0);

    printf ("  SPDIF/digital output setup (normal) ...\n");

    /*  Digital output: default setting */
    /*   デジタル出力: デフォルト設定 */
    sceSdSetCoreAttr (SD_C_SPDIF_MODE, (SD_SPDIF_MEDIA_CD |
					SD_SPDIF_OUT_PCM |
					SD_SPDIF_COPY_NORMAL));

    printf ("  CORE0 MEMIN: filling with 0 ...\n");

    /*  CORE0 MEMIN: cleaning with 0 */
    /* CORE0 MEMIN: 0 で初期化 */
    memset (_0_buf, 0, 0x1000);

    /* 0x4000 - 0x4fff: CORE0 MEMIN */
    sceSdVoiceTrans (SD_CORE_0, (SD_TRANS_MODE_WRITE |
				 SD_TRANS_BY_DMA),
		     _0_buf, 0x4000, 0x1000);
    sceSdVoiceTransStatus (SD_CORE_0, SD_TRANS_STATUS_WAIT);

    printf ("  Set parameters ...\n");

#if WAVE_MODE == NORMAL_INTERLEAVE
    /* CORE1 external input volume */
    sceSdSetParam (SD_CORE_1 | SD_P_AVOLL, 0x7fff);
    sceSdSetParam (SD_CORE_1 | SD_P_AVOLR, 0x7fff);
    /* CORE0 sound data input volume */
    sceSdSetParam (SD_CORE_0 | SD_P_BVOLL, 0x7fff);
    sceSdSetParam (SD_CORE_0 | SD_P_BVOLR, 0x7fff);
    /* CORE1 sound data input volume */
    sceSdSetParam (SD_CORE_1 | SD_P_BVOLL, 0);
    sceSdSetParam (SD_CORE_1 | SD_P_BVOLR, 0);
    /* Master volume */
    sceSdSetParam (SD_CORE_0 | SD_P_MVOLL, 0x3fff);
    sceSdSetParam (SD_CORE_0 | SD_P_MVOLR, 0x3fff);
    sceSdSetParam (SD_CORE_1 | SD_P_MVOLL, 0x3fff);
    sceSdSetParam (SD_CORE_1 | SD_P_MVOLR, 0x3fff);
#elif WAVE_MODE == BYPASS_INTERLEAVE
    /* SAMPLE ONLY: CORE1 external input volume = 0 */
    sceSdSetParam (SD_CORE_1 | SD_P_AVOLL, 0);
    sceSdSetParam (SD_CORE_1 | SD_P_AVOLR, 0);
    /*  MANDATORY: CORE0 BVOL `MUST' be set to 0 */
    /*      必須: CORE0 BVOL は `必ず' 0 に設定 */
    sceSdSetParam (SD_CORE_0 | SD_P_BVOLL, 0);
    sceSdSetParam (SD_CORE_0 | SD_P_BVOLR, 0);
    /* SAMPLE ONLY: CORE1 sound data input volume = 0 */
    sceSdSetParam (SD_CORE_1 | SD_P_BVOLL, 0);
    sceSdSetParam (SD_CORE_1 | SD_P_BVOLR, 0);
    /*   SAMPLE ONLY: bypass processing output does NOT depend on MVOL */
    /* サンプルのみ: バイパス出力は MVOL に依存しない */
    sceSdSetParam (SD_CORE_0 | SD_P_MVOLL, 0);
    sceSdSetParam (SD_CORE_0 | SD_P_MVOLR, 0);
    sceSdSetParam (SD_CORE_1 | SD_P_MVOLL, 0);
    sceSdSetParam (SD_CORE_1 | SD_P_MVOLR, 0);
#endif
    /*  Allocate sound data area */
    /* サウンドデータ領域を確保 */
    CpuSuspendIntr (&oldei);
    /*   SAMPLE ONLY: DTL-H10000 has over 2MB IOP memory */
    /* サンプルのみ: DTL-H10000 は 2MB 以上の IOP メモリを持つ */
    status.addr = (unsigned char *) AllocSysMemory (0, WF_SIZE, NULL);
    CpuResumeIntr (oldei);
    if (status.addr == NULL) {
	printf ("[[ Error: Not allocate IOP memory ]]\n");
	return 0;
    }

    /*  Load sound data */
    /* サウンドデータの読み込み */
    {
	int fd;
	int req = WF_FSIZE;
	int rsize = 0;
	int ret = 0;

	if ((fd = open (WF_FILE, O_RDONLY)) < 0) {
	    printf ("[[ Error: bad filename %s ]]\n", WF_FILE);
	    return (0);
	}
	while (rsize < WF_FSIZE) {
	    ret = read (fd, &status.addr [rsize], req);
	    rsize += ret;
	    if (ret < req) {
		req -= ret;
	    }
	}
	close (fd);
    }

    printf ("  SPDIF/digital output setup (bypass processing) ...\n");

#if WAVE_MODE == BYPASS_INTERLEAVE
    /*  Digital output: bypass processing */
    /* デジタル出力: バイパス指定 */
    sceSdSetCoreAttr (SD_C_SPDIF_MODE, (SD_SPDIF_MEDIA_CD |
					SD_SPDIF_OUT_BYPASS |
					SD_SPDIF_COPY_NORMAL));
#endif
    printf ("  Sound data transferring ...\n");

    /* MANDATORY: bypass processing is only supported by CORE0 MEMIN */
    /*     必須: バイパス処理は CORE0 MEMIN でのみ動作 */
#define CHANNEL_ID SD_CORE_0

#ifdef TRANSFER_LOOP
#if TRANSFER_MODE == TRANSFER_HANDLER
    /*  Start transferring */
    /* 転送開始 */
    status.count = 0;
    sceSdBlockTrans (CHANNEL_ID, (SD_TRANS_MODE_WRITE |
				  SD_BLOCK_HANDLER), /* handler */
		     status.addr, WF_SIZE / 2,
		     transfer_handler,
		     (void *) &status);
#else
    /*  interrupt handler */
    /* 割り込みハンドラ */
    sceSdSetTransIntrHandler (CHANNEL_ID,
			      (sceSdSpu2IntrHandler) transfer_intr,
			      (void *) &status.count);

    /*  Start transferring */
    /* 転送開始 */
    status.count = 0;
    sceSdBlockTrans (CHANNEL_ID, (SD_TRANS_MODE_WRITE |
				  SD_BLOCK_LOOP), /* looping */
		     status.addr, WF_SIZE);
#endif
    /*  waiting until looping 5 times (2 interruputs per 1 loop) */
    /* 5 回ループ (1 ループあたり割り込み 2 回) を待つ */
    while (status.count < 10) {
	DelayThread (100 * 1000); /* 0.1 sec */
    }
#else
    /*  Start transferring */
    /* 転送開始 */
    sceSdBlockTrans (CHANNEL_ID, (SD_TRANS_MODE_WRITE |
				  SD_BLOCK_ONESHOT), /* one shot */
		     status.addr, WF_SIZE);

    /*  wait for finishing transfer process */
    /* 転送処理が終了するまで待つ */
    while (sceSdBlockTransStatus (CHANNEL_ID, SD_TRANS_STATUS_WAIT) & 0xffffff) {
	DelayThread (100 * 1000); /* 0.1 sec */
    }
#endif

#if TRANSFER_MODE != TRANSFER_HANDLER
    /*  Finalize transferring */
    /* 転送処理の終了 */
    sceSdBlockTrans (CHANNEL_ID, SD_TRANS_MODE_STOP, NULL, 0);
#endif

    printf ("  Bypass processing transferring ... done\n");

    /*  Digital output: back to the default setting */
    /* デジタル出力: デフォルト設定に戻す */
    sceSdSetCoreAttr (SD_C_SPDIF_MODE, (SD_SPDIF_MEDIA_CD |
					SD_SPDIF_OUT_PCM |
					SD_SPDIF_COPY_NORMAL));

#if WAVE_MODE == NORMAL_INTERLEAVE
    sceSdSetParam (SD_CORE_1 | SD_P_MVOLL, 0);
    sceSdSetParam (SD_CORE_1 | SD_P_MVOLR, 0);
#endif

    printf ("* SPU2 bypass processing sample (with libsd): finished\n");

    return 0;
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADD STUFF AFTER THIS */
