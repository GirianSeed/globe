/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 * Emotion Engine Library Sample Program
 *
 * Copyright (C) 2001 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#include <eekernel.h>
#include <sif.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libpad.h>
#include <libgraph.h>		/* sceGsSyncV() */
#include <libsdr.h>
#include <sdrcmd.h>

#include <libspu2m.h>

#include <sk/common.h>
#include <sk/errno.h>
#include <sk/sk.h>
#include <sk/sound.h>

#define __printf scePrintf

#define __DEBUG
#ifdef __DEBUG
#define PRINTF(x) __printf x
#else
#define PRINTF(x)
#endif

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
#define _MOD_MODMSIN   "modmsin.irx"
#define _MOD_MODSESQ   "modsesq.irx"
#define _MOD_SK_SOUND  "sksound.irx"
#define _MOD_SK_HSYNTH "skhsynth.irx"
#define _MOD_SK_MIDI   "skmidi.irx"
#define _MOD_SK_MSIN   "skmsin.irx"
#define _MOD_SK_SESQ   "sksesq.irx"

/*
 * variables
 * ---------------------------------------------------------------- */

// コントローラ情報
static u_long128 pad_dma_buf [scePadDmaBufferMax] __attribute__((aligned (64)));

// スレッド ID
static int thid_main, thid_sound;

// セマフォ
int sema_main;
int sema_sound;
int sema_sound_send;
int sema_sound_complete;

// メイン処理スレッドのスタック
#define STACK_SIZE_MAIN (512 * 128)
static unsigned char stack_main [STACK_SIZE_MAIN] __attribute__((aligned(16)));

// サウンド処理スレッドのスタック
#define STACK_SIZE_SOUND (512 * 128)
static unsigned char stack_sound [STACK_SIZE_SOUND] __attribute__((aligned(16)));

// 転送用 EE 側一時バッファ
#define EEBUF_SIZE (2048 * 64)
static unsigned char eebuf [EEBUF_SIZE] __attribute__((aligned(64)));

// 転送用 IOP 側一時バッファのサイズ
#define IOPBUF_SIZE (64 * 1024)			/* 64 KB */

// sceSpu2MemAllocate() により SPU2 ローカルメモリで 32 領域確保可能。
#define SPU2MEM_AREANUM 32
static unsigned char spu2mem_table [SCESPU2MEM_TABLE_UNITSIZE * (SPU2MEM_AREANUM + 1)];

// サウンドスレッド指示用コマンド
#define _IDLE     0
#define _SEND     1
#define _PLAY     2
#define _PAUSE    3
#define _STOP     4
#define _VOLUME   5
#define _TEMPO    6
#define _NOTEON  12
#define _NOTEOFF 13
#define _PLAY2   22
#define _STOP2   24
#define _PAN     25
#define _PBEND   26
#define _EXP     27
#define _QUIT    (-1)
#define _NOOP     0

// サウンド処理スレッド・コマンド送信
static int sound_command = _IDLE;
static int sound_index;
static int sound_value;
static int sound_return;

typedef struct _sound_info {
    char *sq;
    char *hd;
    char *bd;
    unsigned int sq_addr;
    unsigned int hd_addr;
    unsigned int bd_addr;
    unsigned int sq_size;
    unsigned int hd_size;
    unsigned int bd_size;
    int hdbd_id;
    int sq_id;
    int track_id;
    int play_mode;
} sound_info_t;

#define __NO_ID (-1)

sound_info_t sound_info [3] = {
    {
	"host0:/usr/local/sce/data/sound/seq/overload.sq",
	"host0:/usr/local/sce/data/sound/wave/overload.hd",
	"host0:/usr/local/sce/data/sound/wave/overload.bd",
	0, 0, 0,		/* addr */
	0, 0, 0,		/* size */
	__NO_ID, __NO_ID, __NO_ID,
	SCESK_MIDI
    },
    {
	"host0:/usr/local/sce/data/sound/seq/sesample2.sq",
	"host0:/usr/local/sce/data/sound/wave/sesample2.hd",
	"host0:/usr/local/sce/data/sound/wave/sesample2.bd",
	0, 0, 0,		/* addr */
	0, 0, 0,		/* size */
	__NO_ID, __NO_ID, __NO_ID,
	SCESK_SESQ
    },
    {
	"host0:/usr/local/sce/data/sound/seq/overload.sq",
	"host0:/usr/local/sce/data/sound/wave/overload.hd",
	"host0:/usr/local/sce/data/sound/wave/overload.bd",
	0, 0, 0,		/* addr */
	0, 0, 0,		/* size */
	__NO_ID, __NO_ID, __NO_ID,
	SCESK_MSIN
    }
};

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
 * HD/BD ファイルを読み込む。
 */
static int
read_hdbd (int idx)
{
    int ret;

    /*
     * HD データの読み込み
     */
    if (sendtoIOP (sound_info [idx].hd,
		   &(sound_info [idx].hd_addr),
		   &(sound_info [idx].hd_size)) < 0) {
	return (-1);
    }

    PRINTF (("HD: (%08x) %d\n",
	     sound_info [idx].hd_addr, sound_info [idx].hd_size));
	
    /*
     * BD データの読み込み ... IOP メモリからさらに SPU2 ローカルメモリへ転送
     *                     ... bd_addr: SPU2 ローカルメモリ内のアドレス
     */
    if (sendtoSPU (sound_info [idx].bd,
		   eebuf, EEBUF_SIZE,
		   &(sound_info [idx].bd_addr),
		   &(sound_info [idx].bd_size)) < 0) {
	sceSifFreeSysMemory ((void *)sound_info [idx].hd_addr);
	sound_info [idx].hd_addr = 0;
	sound_info [idx].hd_size = 0;
	return (-2);
    }

    PRINTF (("BD: (%08x) %d\n",
	     sound_info [idx].bd_addr, sound_info [idx].bd_size));

    return 0;
}

/*
 * 各サウンドファイルを読み込む。
 */
static int
read_sound_file (int idx)
{
    if (read_hdbd (idx) < 0) {
	return (-1);
    }

    switch (sound_info [idx].play_mode) {
    case SCESK_MIDI:
    case SCESK_SONG:
    case SCESK_SESQ:
	/* SQ ファイルを IOP メモリ内に転送 */
	if (sendtoIOP (sound_info [idx].sq,
		       &(sound_info [idx].sq_addr),
		       &(sound_info [idx].sq_size)) < 0) {
	    sceSifFreeSysMemory ((void *)sound_info [idx].hd_addr);
	    sceSpu2MemFree (sound_info [idx].bd_addr);
	    sound_info [idx].hd_addr = 0;
	    sound_info [idx].bd_addr = 0;
	    sound_info [idx].hd_size = 0;
	    sound_info [idx].bd_size = 0;
	    return (-2);
	}
	PRINTF (("SQ: (%08x) %d\n",
		 sound_info [idx].sq_addr, sound_info [idx].sq_size));
	break;
    }

    return 0;
}

/*
 * 標準キット / 各サウンドファイルを bind する。
 */
static int
bind_sound_data (int idx)
{
    int ret;

    /*
     * HD/BD データの登録
     *		→ HD/BD id が返る
     */
    sound_info [idx].hdbd_id = sceSkSsBindHDBD (SCESK_AUTOASSIGNMENT,
						sound_info [idx].hd_addr,
						sound_info [idx].hd_size,
						sound_info [idx].bd_addr,
						sound_info [idx].bd_size);

    PRINTF (("BIND HDBD / return: %d\n", sound_info [idx].hdbd_id));

    /*
     * HD/BD データ ID を、トラック (ID: 自動割り当て) に登録
     * 		→ トラック id が返る
     */
    sound_info [idx].track_id = sceSkSsBindTrackHDBD (SCESK_AUTOASSIGNMENT,
						      sound_info [idx].hdbd_id,
						      0); /* バンク番号: 0 */

    PRINTF (("BIND TRACK (HDBD) / return: %d\n", sound_info [idx].track_id));

    switch (sound_info [idx].play_mode) {
    case SCESK_MIDI:
    case SCESK_SONG:
	/*
	 * SQ データ (MIDI) の登録
	 *		→ SQ id が返る
	 */
	sound_info [idx].sq_id = sceSkSsBindSQ (MIDI, SCESK_AUTOASSIGNMENT,
						sound_info [idx].sq_addr,
						sound_info [idx].sq_size);

	PRINTF (("BIND SQ (MIDI) / return: %d\n", sound_info [idx].sq_id));

	/*
	 * SQ データ ID を、HD/BD データ ID を登録したトラックに登録
	 *		→ 登録したトラック id がそのまま返る
	 */
	ret = sceSkSsBindTrackSQ (MIDI,
				  sound_info [idx].track_id,
				  sound_info [idx].sq_id);

	PRINTF (("BIND TRACK (SQ (MIDI)) / return: %d\n", ret));
	break;

    case SCESK_SESQ:
	/*
	 * SQ データ (SESQ) の登録
	 *		→ SQ id が返る
	 */
	sound_info [idx].sq_id = sceSkSsBindSQ (SESQ, SCESK_AUTOASSIGNMENT,
						sound_info [idx].sq_addr,
						sound_info [idx].sq_size);

	PRINTF (("BIND SQ (SESQ) / return: %d\n", sound_info [idx].sq_id));

	/*
	 * SQ データ ID を、HD/BD データ ID を登録したトラックに登録
	 *		→ 登録したトラック id がそのまま返る
	 */
	ret = sceSkSsBindTrackSQ (SESQ,
				  sound_info [idx].track_id,
				  sound_info [idx].sq_id);

	PRINTF (("BIND TRACK (SQ (SESQ)) / return: %d\n", ret));
	break;

    case SCESK_MSIN:
	/*
	 * SQ データ (SESQ) の登録
	 *		→ SQ id が返る
	 */
	sound_info [idx].sq_id = sceSkSsBindSQ (MSIN, SCESK_AUTOASSIGNMENT, 0, 0);

	PRINTF (("BIND SQ (MSIN) / return: %d\n", sound_info [idx].sq_id));

	/*
	 * SQ データ ID を、HD/BD データ ID を登録したトラックに登録
	 *		→ 登録したトラック id がそのまま返る
	 */
	ret = sceSkSsBindTrackSQ (MSIN,
				  sound_info [idx].track_id,
				  sound_info [idx].sq_id);

	PRINTF (("BIND TRACK (SQ (MSIN)) / return: %d\n", ret));
	break;
    }

    return 0;
}

/*
 * 標準キット / 各サウンドを消音する。
 */
static void
all_sound_off (void)
{
    int i;
    int ret;

    for (i = 0; i < 3; i ++) {
	if (sound_info [i].track_id != __NO_ID) {
	    ret = sceSkSsAllSoundOff (sound_info [i].track_id);
	    PRINTF (("ALLSOUNDOFF TRACK (%d) / return: %d\n",
		     sound_info [i].track_id, ret));
	}
    }

    return;
}


/*
 * 標準キット / 各サウンドファイルを unbind する。
 */
static void
unbind_all_sound_data (void)
{
    int i;
    int ret;

    for (i = 0; i < 3; i ++) {
	/*
	 * トラックから HD/BD データ ID の割り当てを解除
	 */
	if (sound_info [i].hdbd_id  != __NO_ID &&
	    sound_info [i].track_id != __NO_ID) {
	    ret = sceSkSsUnbindTrackHDBD (sound_info [i].track_id);
	    PRINTF (("UNBIND TRACK (HDBD) / return: %d\n", ret));
	}
	
	/*
	 * トラックから SQ データ ID の割り当てを解除
	 */
	if (sound_info [i].sq_id    != __NO_ID &&
	    sound_info [i].track_id != __NO_ID) {
	    switch (sound_info [i].play_mode) {
	    case SCESK_MIDI:
	    case SCESK_SONG:
		ret = sceSkSsUnbindTrackSQ (MIDI, sound_info [i].track_id);
		PRINTF (("UNBIND TRACK (SQ (MIDI)) / return: %d\n", ret));
		break;
	    case SCESK_SESQ:
		ret = sceSkSsUnbindTrackSQ (SESQ, sound_info [i].track_id);
		PRINTF (("UNBIND TRACK (SQ (SESQ)) / return: %d\n", ret));
		break;
	    case SCESK_MSIN:
		ret = sceSkSsUnbindTrackSQ (MSIN, sound_info [i].track_id);
		PRINTF (("UNBIND TRACK (SQ (SESQ)) / return: %d\n", ret));
		break;
	    }
	}

	/*
	 * HD/BD データ ID を解除
	 */
	if (sound_info [i].hdbd_id != __NO_ID) {
	    ret = sceSkSsUnbindHDBD (sound_info [i].hdbd_id);
	    PRINTF (("UNBIND HDBD / return: %d\n", ret));
	}

	/*
	 * SQ データ ID を解除
	 */
	if (sound_info [i].sq_id != __NO_ID) {
	    switch (sound_info [i].play_mode) {
	    case SCESK_MIDI:
	    case SCESK_SONG:
		ret = sceSkSsUnbindSQ (MIDI, sound_info [i].sq_id);
		PRINTF (("UNBIND (SQ (MIDI)) / return: %d\n", ret));
		break;
	    case SCESK_SESQ:
		ret = sceSkSsUnbindSQ (SESQ, sound_info [i].sq_id);
		PRINTF (("UNBIND (SQ (SESQ)) / return: %d\n", ret));
		break;
	    case SCESK_MSIN:
		ret = sceSkSsUnbindSQ (MSIN, sound_info [i].sq_id);
		PRINTF (("UNBIND (SQ (MSIN)) / return: %d\n", ret));
		break;
	    }
	}
    }

    return;
}

/*
 * サウンド処理スレッド
 */
void
main_sound (void *argp)
{
    int ret, sesq_id;
    int isstop = 0;
    int note_id;

    sound_command = _IDLE;
    note_id = 0;

    while (isstop == 0) {
	WaitSema (sema_sound);

	sound_return = 0;
	ret = 0;
	switch (sound_command) {
	case _SEND:
	    ret = read_sound_file (sound_index);
	    if (ret >= 0) {
		ret = bind_sound_data (sound_index);
	    }
	    break;
	case _PLAY:
	    ret = sceSkSsPlayMIDI (sound_info [sound_index].track_id,
				   0); /* MIDI data block # */
	    break;
	case _PAUSE:
	    ret = sceSkSsPauseMIDI (sound_info [sound_index].track_id);
	    break;
	case _STOP:
	    ret = sceSkSsStopMIDI (sound_info [sound_index].track_id);
	    break;
	case _VOLUME:
	    ret = sceSkSsSetPlayVolumeMIDI (sound_info [sound_index].track_id,
					    sound_value);
	    break;
	case _TEMPO:
	    ret = sceSkSsSetPlayTempoMIDI (RELATIVE, sound_info [sound_index].track_id,
					   (sound_value * 0x100) / 100 ); /* 0x100 が 100 % */
	    break;
	case _PLAY2:
	    sesq_id = sceSkSsPlaySESQ (sound_info [sound_index].track_id,
				       1, 1); /* SE sequence set #, sequence # */
	    break;
	    
	case _STOP2:
#if 0
	    // 複数回、演奏可能…ここでは _PLAY2 時の全ての sesq_id を保持していない
	    ret = sceSkSsStopSESQ (sound_info [sound_index].track_id, sesq_id);
	    break;
#endif
	case _NOTEON:
	    ret = sceSkSsVoiceNoteOn (sound_info [sound_index].track_id,
				      note_id,	/* id */
				      0,	/* bank # */
				      4,	/* prog # */
				      0,	/* MIDI ch */
				      0x3c,	/* note # */
				      0x7f,	/* vel */
				      127);	/* pan */
	    if (ret >= 0) {
		ret = note_id;
	    }
	    note_id ++;
	    if (note_id > 126) note_id = 0;
	    break;
	case _NOTEOFF:
#if 0
	    // 複数回、演奏可能…ここでは _NOTEON 時の全ての note_id を保持していない
	    ret = sceSkSsVoiceNoteOff (sound_info [sound_index].track_id,
				       0x10,	/* id */
				       0,	/* bank # */
				       4,	/* prog # */
				       0,	/* MIDI ch */
				       0x3c);	/* note # */
#endif
	    break;

	case _PAN:
	    ret = sceSkSsVoiceSetPanpot (sound_info [sound_index].track_id,
					 sound_value >> 16,	/* id */
					 0,			/* MIDI ch */
					 0x3c,			/* note # */
					 sound_value & 0x7f);	/* pan */
	    break;

	case _PBEND:
	    ret = sceSkSsVoiceSetPitchbend (sound_info [sound_index].track_id,
					    sound_value >> 16,	/* id */
					    0,			/* MIDI ch */
					    0x3c,		/* note # */
					    sound_value & 0x3fff); /* pitchbend */
	    break;

	case _EXP:
	    ret = sceSkSsVoiceSetExpression (sound_info [sound_index].track_id,
					     sound_value >> 16,	/* id */
					     0,			/* MIDI ch */
					     0x3c,		/* note # */
					     sound_value & 0x7f); /* expression */
	    break;

	case _QUIT:
	    all_sound_off ();
	    unbind_all_sound_data ();
	    isstop ++;
	    break;
	}
	if (ret < 0) {
	    PRINTF (("something is wrong...\n"));
	}
	sound_return = ret;
	sound_command = _IDLE;
	sound_index   = -1;
	sound_value   = 0;
	if (isstop == 0) { // Not quiting ...
	    SignalSema (sema_sound_send);
	}
	SignalSema (sema_sound_complete);
    }

    return;
}

/*
 * サウンド処理スレッドへのコマンド送出用関数
 */
int
command_x (int idx, int cmd, int value)
{
    int ret;

    ret = WaitSema (sema_sound_send); // or PollSema()
    if (ret >= 0) {
	sound_index   = idx;
	sound_command = cmd;
	sound_value   = value;
	SignalSema (sema_sound);
	WaitSema (sema_sound_complete);
    } else {
	return (-1);
    }

    return (sound_return);
}

/*
 * コントローラによるサウンド指示
 */
#define _PAD_DATA (0xffff ^ ((rdata[2] << 8) | rdata[3]))
#define _L 0
#define _R 1
#define _X 2
#define DEFAULT_VOLUME 0x80
#define DEFAULT_TEMPO  100
#define DEFAULT_PANPOT 127
void
main_loop (void)
{
    int ret, c;
    int note_id, old_note_id;
    unsigned int paddata, oldpad;
    unsigned char rdata [32];
    /* 各種フラグ */
    int playing [3] = {_IDLE, _IDLE, _IDLE};
    int isstop  = 0;		/* スレッド終了フラグ */
    int vol [2] = { 0x80, 0x80 }; /* ボリューム */
    int tempo [2] = { 100, 100 }; /* テンポ */
    int pan = 127; /* パンポット */
    int pbend = 0x3fff;
    int express = 127;
    int pan_start = -1;

    scePadPortOpen (0, 0, pad_dma_buf);

    c = 0;
    oldpad = paddata = 0;
    note_id = old_note_id = -1;
    while (isstop == 0) {
	/* コントローラ情報の読み込み */
	if (scePadRead (0, 0, rdata) > 0) paddata = _PAD_DATA;
	else                              paddata = 0;

	if (paddata != oldpad) {
	    if (paddata & SCE_PADselect) {
		/* main_loop() の終了 */
		isstop ++;
	    }
	    /*
	     * SQ データ (MIDI) の制御
	     */
	    else if (paddata & SCE_PADLleft) {
		/* データの読み込み */
		if (playing [_L] == _IDLE) {
		    ret = command_x (_L, _SEND, _NOOP);
		    PRINTF (("Send: %d\n", ret));
		    playing [_L] = _SEND;
		}
	    }
	    else if (paddata & SCE_PADLright) {
		/* MIDI データ…演奏開始/ポーズ */
		if (playing [_L] == _SEND ||
		    playing [_L] == _PAUSE) {
		    /* 演奏開始・再開 */
		    ret = command_x (_L, _PLAY, _NOOP);
		    PRINTF (("Play (%d): %d\n", _L, ret));
		    playing [_L] = _PLAY;
		} else if (playing [_L] == _PLAY) {
		    /* ポーズ */
		    ret = command_x (_L, _PAUSE, _NOOP);
		    PRINTF (("Pause (%d): %d\n", _L, ret));
		    playing [_L] = _PAUSE;
		}
	    }
	    else if (paddata & SCE_PADLdown) {
		/* MIDI データ…停止 */
		if (playing [_L] == _PLAY ||
		    playing [_L] == _PAUSE) {
		    ret = command_x (_L, _STOP, _NOOP);
		    PRINTF (("Stop (%d): %d\n", _L, ret));
		        vol [_L] = DEFAULT_VOLUME;
		      tempo [_L] = DEFAULT_TEMPO;
		    playing [_L] = _SEND; /* データ転送状態に */
		    ret = command_x (_L, _VOLUME, DEFAULT_VOLUME);
		    PRINTF (("Volume (%d): %d\n", _L, ret));
		    ret = command_x (_L, _TEMPO,  DEFAULT_TEMPO);
		    PRINTF (("Tempo (%d): %d\n", _L, ret));
		}
	    }
	    else if (paddata & SCE_PADL1) {
		if (paddata & SCE_PADstart) {
		    /* テンポアップ */
		    if (playing [_L] == _PLAY) {
			tempo [_L] += 5;
			if (tempo [_L] > 200) tempo [_L] = 200;	/* 200% */
			ret = command_x (_L, _TEMPO, tempo [_L]);
			PRINTF (("Tempo (%d): %d\n", _L, ret));
		    }
		} else {
		    /* ボリュームアップ */
		    if (playing [_L] == _PLAY) {
			vol [_L] += 5;
			if (vol [_L] > DEFAULT_VOLUME) vol [_L] = DEFAULT_VOLUME;
			ret = command_x (_L, _VOLUME, vol [_L]);
			PRINTF (("Vol: %d\n", ret));
		    }
		}
	    }
	    else if (paddata & SCE_PADL2) {
		if (paddata & SCE_PADstart) {
		    /* テンポダウン */
		    if (playing [_L] == _PLAY) {
			tempo [_L] -= 5;
			if (tempo [_L] < 50) tempo [_L] = 50;	/* 50% */
			ret = command_x (_L, _TEMPO, tempo [_L]);
			PRINTF (("Tempo: %d\n", ret));
		    }
		} else {
		    /* volume down */
		    if (playing [_L] == _PLAY) {
			vol [_L] -= 5; if (vol [_L] < 0) vol [_L] = 0;
			ret = command_x (_L, _VOLUME, vol [_L]);
			PRINTF (("Vol: %d\n", ret));
		    }
		}
	    }
	    /*
	     * SQ データ (SESQ) の制御
	     */
	    else if (paddata & SCE_PADRleft) {
		/* データの読み込み */
		if (playing [_R] == _IDLE) {
		    ret = command_x (_R, _SEND, _NOOP);
		    PRINTF (("Send: %d\n", ret));
		    playing [_R] = _SEND;
		}
	    }
	    else if (paddata & SCE_PADRright) {
		/* SESQ データ…演奏開始 */
		/* 最大同時演奏数は 32 ... modsesq の制限 */
		if (playing [_R] == _SEND ||
		    playing [_R] == _PLAY) {
		    ret = command_x (_R, _PLAY2, _NOOP);
		    PRINTF (("Play: %d\n", ret));
		    playing [_R] = _PLAY;
		}
	    }
#if 0
	    else if (paddata & SCE_PADRdown) {
		/* SESQ データ…演奏停止 */
		/* ... 現在、発音の際に割り当てられた sesq_id をこの
		 *     サンプル内で保持していないため、STOP 処理は
		 *     行っていない */
		if (playing [_R] == _PLAY) {
		    ret = command_x (_R, _STOP, _NOOP);
		    PRINTF (("Stop: %d\n", ret));
		    playing [_R] = _SEND; /* データ転送状態に */
		}
	    }
#endif
	    /*
	     * 単発発音の制御
	     */
	    else if (paddata & SCE_PADLup) {
		/* データの読み込み */
		if (playing [_X] == _IDLE) {
		    ret = command_x (_X, _SEND, _NOOP);
		    PRINTF (("Send: %d\n", ret));
		    playing [_X] = _SEND;
		}
	    }
	    // 単発発音
	    else if (paddata & SCE_PADRup) {
		/* 単発発音…発音開始 */
		if (playing [_X] == _SEND ||
		    playing [_X] == _PLAY) {
		    ret = command_x (_X, _NOTEON, _NOOP);
		    PRINTF (("Play: %d\n", ret));
		    playing [_X] = _PLAY;
		    note_id = ret;
		}
	    }
	}
	c ++;
	sceGsSyncV (0);
	oldpad = paddata;
	// if (c % 60 == 0) scePrintf (".\n");

	/*
	 * 単発発音のパンポット・ピッチベンド・エクスプレッション制御
	 */
	if (note_id != -1) {
	    if (note_id != old_note_id) {
		pan = 127;
		pbend = 0x3fff;
		express = 127;
		old_note_id = note_id;
	    }
	    // ピッチベンド
	    ret = command_x (_X, _PBEND, (note_id << 16 | pbend));
	    pbend -= 32;
	    // エクスプレッション
	    ret = command_x (_X, _EXP,   (note_id << 16 | (express & 0x7f)));
	    express -= 1;
	    // パンポット
	    ret = command_x (_X, _PAN, (note_id << 16 | pan));
	    //PRINTF (("Pan: %d\n", pan));
	    pan --;
	    if (pan < 0) {
		pan = 127;
		pbend = 0x3fff;
		express = 0;
		note_id = -1;
		pan_start = -1;
	    }
	}
    }
    PRINTF (("c = %d\n", c));

    if (playing [_L] == _PLAY ||
	playing [_L] == _PAUSE) {
	ret = command_x (_L, _STOP, _NOOP);
	PRINTF (("Stop: %d\n", ret));
    }
#if 0
    if (playing [_R] == _PLAY) {
	ret = command_x (_R, _STOP, _NOOP);
	PRINTF (("Stop: %d\n", ret));
    }
#endif

    // サウンド処理スレッドを終了させる
    sound_command = _QUIT;
    SignalSema (sema_sound);
    WaitSema (sema_sound_complete);
}

/*
 * メイン処理スレッド
 */
void
main_main (void *argp)
{
    // 再生をコントロールする
    main_loop ();

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
 * 各種初期化処理
 */
int
initialize (void)
{
    struct SemaParam sp;

    // SIF 関連の初期化
    sceSifInitRpc (0);
    sceSifInitIopHeap ();

    // モジュールのロード
    LOAD_MODULE (_MOD_SIO2MAN);
    LOAD_MODULE (_MOD_PADMAN);
    LOAD_MODULE (_MOD_LIBSD);		/* 低レベルサウンドライブラリ */
    LOAD_MODULE (_MOD_SDRDRV);		/* 低レベルサウンドライブラリ (for EE) */
    LOAD_MODULE (_MOD_MODHSYN);		/* ハードウェアシンセサイザ */
    LOAD_MODULE (_MOD_MODMIDI);		/* MIDI シーケンサ */
    LOAD_MODULE (_MOD_MODMSIN);		/* MIDI ストリーム生成 */
    LOAD_MODULE (_MOD_MODSESQ);		/* 効果音シーケンサ */
    LOAD_MODULE (_MOD_SK_SOUND);	/* 標準キット: サウンドファンダメンタル */
    LOAD_MODULE (_MOD_SK_HSYNTH);	/* 標準キット: ハードウェアシンセサイザ */
    LOAD_MODULE (_MOD_SK_MIDI);		/* 標準キット: MIDI シーケンサ */
    LOAD_MODULE (_MOD_SK_MSIN);		/* 標準キット: MIDI ストリーム生成 */
    LOAD_MODULE (_MOD_SK_SESQ);		/* 標準キット: 効果音シーケンサ */

    // コントローラライブラリの初期化
    scePadInit (0);

    // 標準キット ... 初期化
    sceSkInit (0);		/* Standard Kit 全体の初期化 */
    sceSkSsInit (32);		/* SK/Sound System の初期化:
				 *   引数 ... sdr 割り込みハンドラスレッドのプライオリティ */

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
    sema_main = CreateSema (&sp); /* メイン処理スレッド終了用 */
    if (sema_main < 0) {
	PRINTF (("Can't create semaphore.\n"));
	return (-1);
    }
    sema_sound = CreateSema (&sp); /* サウンド処理スレッド用 */
    if (sema_sound < 0) {
	PRINTF (("Can't create semaphore.\n"));
	return (-1);
    }
    sema_sound_complete = CreateSema (&sp); /* サウンド処理コマンド用 */
    if (sema_sound_complete < 0) {
	PRINTF (("Can't create semaphore.\n"));
	return (-1);
    }

    sp.initCount = 1;		/* 最初に WaitSema() を行う */
    sema_sound_send = CreateSema (&sp); /* サウンド処理コマンド用 */
    if (sema_sound_send < 0) {
	PRINTF (("Can't create semaphore.\n"));
	return (-1);
    }
}

/*
 * 各種終了処理
 */
void
finalize (void)
{
    // SPU2 メモリ管理の終了
    sceSpu2MemQuit();

    DeleteSema (sema_sound);
    DeleteSema (sema_sound_send);
    DeleteSema (sema_sound_complete);
    DeleteSema (sema_main);
}

/*
 * スタートエントリ
 */
int
main (int argc, char *argv [])
{
    struct ThreadParam tp;

    initialize ();

    /* メイン処理スレッドの生成と起動 */
    tp.stackSize    = STACK_SIZE_MAIN;
    tp.gpReg 	    = &_gp;
    tp.entry 	    = main_main;
    tp.stack 	    = (void *) stack_main;
    tp.initPriority = 3;
    thid_main = CreateThread (&tp);
    if (thid_main < 0) {
	// Can't create thread
	return 0;
    }
    StartThread (thid_main, (void *)&sema_main);

    /* サウンド処理スレッドの生成と起動 */
    tp.stackSize    = STACK_SIZE_SOUND;
    tp.gpReg 	    = &_gp;
    tp.entry 	    = main_sound;
    tp.stack 	    = (void *) stack_sound;
    tp.initPriority = 4;
    thid_sound = CreateThread (&tp);
    if (thid_sound < 0) {
	// Can't create thread
	return 0;
    }
    StartThread (thid_sound, (void *)&sema_sound);

    WaitSema (sema_main);

    scePrintf ("Exit ...\n");

    if (TerminateThread (thid_sound) == thid_sound) {
	DeleteThread (thid_sound);
    }
    if (TerminateThread (thid_main) == thid_main) {
	DeleteThread (thid_main);
    }

    finalize ();

    scePrintf ("Fine ...\n");

    return 0;
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADD STUFF AFTER THIS */
