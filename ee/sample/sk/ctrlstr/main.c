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
#include <sk/stream.h>

#define Xprintf scePrintf

// #define DEBUG
#ifdef DEBUG
#define PRINTF(x) Xprintf x
#define DBGPROC(x) x
#else
#define PRINTF(x)
#define DBGPROC(x)
#endif

/*
 * IOP 使用モジュール
 * ---------------------------------------------------------------- */

#define LOAD_MODULE(mod) \
while (sceSifLoadModule (_MOD_ROOT mod, 0, NULL) < 0) { \
    scePrintf ("  loading %s failed\n", mod); \
}

#define DATA_DIR     "host0:/usr/local/sce/data/sound/wave/"
#define _MOD_ROOT    "host0:/usr/local/sce/iop/modules/"

#define _MOD_SIO2MAN     "sio2man.irx"
#define _MOD_PADMAN      "padman.irx"
#define _MOD_LIBSD       "libsd.irx"
#define _MOD_STRADPCM    "stradpcm.irx"
#define _MOD_SDRDRV      "sdrdrv.irx"
#define _MOD_MODHSYN     "modhsyn.irx"
#define _MOD_MODMIDI     "modmidi.irx"
#define _MOD_SK_SOUND    "sksound.irx"
#define _MOD_SK_STRADPCM "sksadpcm.irx"

/*
 * 定数マクロ
 * ---------------------------------------------------------------- */

// ストリーム情報
#define N_STREAM	3 // BGM 2 channels + SE 1 channel

// バッファサイズ
#define SPU_BUF_SIZE	0x4000	/* SPU2 ローカルバッファ内 */
#define IOP_BUF_SIZE	(SPU_BUF_SIZE * 8) /* IOP メモリ内リングバッファ */
#define CPY_BUF_SIZE	(SPU_BUF_SIZE * 1) /*  */

#define EE_UNCACHE_MASK 0x20000000

// サウンドスレッド指示用コマンド
#define _IDLE     0
#define _PRELOAD  1
#define _PLAY     2
#define _STOP     3
#define _PAUSE    4
#define _MUTE     5
#define _PCHANGE  6
#define _QUIT     (-1)
#define _PLAY_ALL 1000

// ストリームチャンネル
#define _TR_CH_BGM_L 0
#define _TR_CH_BGM_R 1
#define _TR_CH_ONE   2

// ストリームチャンネル (ビット表現)
#define _TR_BGM		((1 << _TR_CH_BGM_L) | (1 << _TR_CH_BGM_R))
#define _TR_ONE		 (1 << _TR_CH_ONE)
#define _STREAM_ALL	 (_TR_BGM | _TR_ONE)

// ID 未割り当て
#define _NO_ID (-1)

// サンプル制御: ストリーム自動再生を有効にしてコンパイル & 実行
#define STREAM_AUTOPLAY 1
// サンプル制御: エフェクトを有効にしてコンパイル & 実行
#define STREAM_EFFECT   0

#if STREAM_AUTOPLAY != 0
#define STREAM_OPT_AUTOPLAY SCESK_STRADPCM_OPTION_AUTOPLAY
#else
#define STREAM_OPT_AUTOPLAY 0
#endif
#if STREAM_EFFECT != 0
#define STREAM_OPT_EFFECT SCESK_STRADPCM_OPTION_EFFECT
#else
#define STREAM_OPT_EFFECT 0
#endif

// サンプル制御: ストリームオプション
#define STREAM_OPTIONS (STREAM_OPT_AUTOPLAY | STREAM_OPT_EFFECT)

// サンプル制御: ポーズ指示でミュート処理
#define PAUSE_IS_MUTE 0

// ストリームボリューム: 10% 分抑えておく
#define VOLUME_L ((SCESK_STRADPCM_VOLUME_MAX * 90) / 100)
#define VOLUME_R ((SCESK_STRADPCM_VOLUME_MAX * 90) / 100)

// コントローラ操作
#define _PAD_PRELOAD_ONE SCE_PADRdown
#define _PAD_PRELOAD_BGM SCE_PADRright
#if STREAM_AUTOPLAY == 0
#define _PAD_PLAY_ONE    SCE_PADRleft
#define _PAD_PLAY_BGM    SCE_PADRup
#endif
#define _PAD_PAUSE_ONE   SCE_PADR1
#define _PAD_MUTE_ONE    SCE_PADL1
#define _PAD_MUTE_BGM    SCE_PADL2
#define _PAD_STOP_ONE    SCE_PADLdown
#define _PAD_STOP_BGM    SCE_PADLright
#define _PAD_PCHANGE_ONE SCE_PADLleft
#define _PAD_PCHANGE_BGM SCE_PADLup
#define _PAD_PLAY_ALL    SCE_PADstart
#define _PAD_QUIT        SCE_PADselect

// ADPCM ブロックの LOOP フラグを LOOP のみにする
#define ADPCM_FLAG_LOOP  0x02
#define ADPCM_SETFLAG_LOOP(a) { \
  *((unsigned char *)((a)+1)) = ADPCM_FLAG_LOOP; \
}

/*
 * 変数
 * ---------------------------------------------------------------- */

// コントローラ作業用バッファ
static u_long128 pad_dma_buf [scePadDmaBufferMax] __attribute__((aligned(64)));

// スレッド ID
static int thid_main;		// メイン処理スレッド
static int thid_sound;		// サウンド処理スレッド

// セマフォ
int sema_main;			// メイン処理スレッド終了用セマフォ
int sema_sound;			// サウンド処理スレッド用セマフォ
int sema_sound_send;		// サウンド処理コマンド用セマフォ
int sema_sound_complete;	// サウンド処理コマンド終了待ち受け用セマフォ

// メイン処理スレッドのスタック
#define STACK_SIZE_MAIN (1024 * 128)
static unsigned char stack_main [STACK_SIZE_MAIN] __attribute__((aligned(64)));

// サウンド処理スレッドのスタック
#define STACK_SIZE_SOUND (512 * 128)
static unsigned char stack_sound [STACK_SIZE_SOUND] __attribute__((aligned(64)));

// sceSpu2MemAllocate() により SPU2 ローカルメモリで 32 領域確保可能 
#define SPU2MEM_N_AREAS 32
static unsigned char spu2mem_table [SCESPU2MEM_TABLE_UNITSIZE * (SPU2MEM_N_AREAS + 1)];

// ストリーム環境構造体
SceSkSsStrAdpcmEnv strenv;

// ストリーム状態 (ビットパターン)
unsigned int stream_bit  = 0;	// 現在稼働しているストリーム
unsigned int stop_bit    = 0;	// 停止させるストリーム
unsigned int pause_bit   = 0;	// ポーズされているストリーム
unsigned int mute_bit    = 0;	// ミュートされているストリーム
unsigned int pchange_bit = 0;	// 音程が変更されているストリーム

// ストリーム ID, トラック ID
int stradpcm_id = _NO_ID;
int track_id    = _NO_ID;

// EE メモリ内データ読み込みバッファ
unsigned int xaddr [N_STREAM][CPY_BUF_SIZE] __attribute__((aligned(64)));

// サウンド処理スレッド・コマンド送信
static int sound_command = _IDLE;
static int sound_index;
static int sound_value;
static int sound_return;

typedef struct stream_sound_info {
    char *fname;	/* ファイル名		*/
    int   fd;		/* file descriptor	*/
    int   size;		/* ファイルサイズ	*/
    int   loop;		/* 1:ループ / 0:1 ショット */
    int   end;		/* 1:データ終了 / 0:読み込み中 (1 ショットデータのみ) */
} stream_sound_info_t;

// ストリーム波形データ情報
stream_sound_info_t stream_sound_info [N_STREAM] = {
    { DATA_DIR "tr1l_pad.vb", -1, 0, 1, 0 }, /* ループ */
    { DATA_DIR "tr1r_pad.vb", -1, 0, 1, 0 }, /* ループ */
    { DATA_DIR "ps_mono.vb",  -1, 0, 0, 0 }, /* 1 ショット */
};

/*
 * 関数
 * ---------------------------------------------------------------- */

/*
 *	ファイルのオープンとサイズ取得
 */
static void
open_file_ch (int ch)
{
    int fd, size;

    while ((fd = sceOpen (stream_sound_info [ch].fname, SCE_RDONLY)) < 0) {
	scePrintf ("[[ Error: bad filename %s ]]\n",
		   stream_sound_info [ch].fname);
    }
    while ((size = sceLseek (fd, 0, SCE_SEEK_END)) < 0) {
	scePrintf ("[[ Error: cannot seek %s ]]\n",
		   stream_sound_info [ch].fname);
    }
    while (sceLseek (fd, 0, SCE_SEEK_SET) < 0) {
	scePrintf ("[[ Error: cannot rewind %s ]]\n",
		   stream_sound_info [ch].fname);
    }
    stream_sound_info [ch].fd   = fd;
    stream_sound_info [ch].size = size;
}

static void
open_file (void)
{
    int ch;

    for (ch = 0; ch < N_STREAM; ch ++) {
	open_file_ch (ch);
    }
}

/*
 *	ファイルのクローズ
 */
static void
close_file_ch (int ch)
{
    sceClose (stream_sound_info [ch].fd);
}

static void
close_file (void)
{
    int ch;

    for (ch = 0; ch < N_STREAM; ch ++) {
	close_file_ch (ch);
    }
}

/*
 *	ストリームデータのプリロード
 */
static void
pre_read_file_ch (int ch)
{
    int rsize;

    stream_sound_info [ch].end = 0;
    while (sceLseek (stream_sound_info [ch].fd, 0, SCE_SEEK_SET) < 0) {
	scePrintf ("[[ Error: cannot seek %s ]]\n",
		   stream_sound_info [ch].fname);
    }
    // EE メモリ内データ読み込みバッファ分、読み込む
    rsize = sceRead (stream_sound_info [ch].fd,
		     (u_char *)strenv.buffer [ch].addr, CPY_BUF_SIZE);
    strenv.buffer [ch].validsize = CPY_BUF_SIZE;
}

static void
pre_read_file (unsigned int str_bit)
{
    int ch;

    for (ch = 0; ch < N_STREAM; ch ++) {
	if (str_bit & (1 << ch)) {
	    pre_read_file_ch (ch);
	}
    }
}

/*
 *	ストリームデータの読み込み位置を先頭に巻き戻す
 */
static void
rewind_file_ch (int ch)
{
    while (sceLseek (stream_sound_info [ch].fd, 0, SCE_SEEK_SET) < 0) {
	scePrintf ("[[ Error: cannot seek %s ]]\n",
		   stream_sound_info [ch].fname);
    }
}

static void
rewind_file (int str_bit)
{
    int ch;
    for (ch = 0; ch < N_STREAM; ch ++) {
	if (str_bit & (1 << ch)) {
	    rewind_file_ch (ch);
	}
    }
}

/*
 *	ストリームデータのロード
 */
int
fill_data_ch (int ch)
{
    int ret;

    ret = 0;
    if (strenv.buffer [ch].validsize == 0) {
	unsigned char *addr = (unsigned char *)strenv.buffer [ch].addr;
	unsigned int   size = CPY_BUF_SIZE;
	int rsize;

	if (stream_sound_info [ch].end == 0) {
	    // 1 ショット: 波形データが末尾まで到達していない
	    //     ループ: 常に .end は 0
	    rsize = sceRead (stream_sound_info [ch].fd, addr, size);
	    if (rsize < size) {
		if (stream_sound_info [ch].loop == 1) {
		    int xsize;
		    // ループ: データ先頭まで巻き戻す
		    if (rsize < 0) rsize = 0;
		    if (rsize >= 0x10) {
			// 最終ブロックの LOOP フラグを LOOP のみに
			ADPCM_SETFLAG_LOOP (addr + rsize - 0x10);
		    }
		    while (sceLseek (stream_sound_info [ch].fd, 0, SCE_SEEK_SET) < 0) {
			scePrintf ("[[ Error: cannot seek %s ]]\n",
				   stream_sound_info [ch].fname);
		    }
		    xsize = sceRead (stream_sound_info [ch].fd,
				     addr + rsize, size - rsize);
		    if (xsize >= 0x10) {
			// 先頭ブロックの LOOP フラグを LOOP のみに
			ADPCM_SETFLAG_LOOP (addr + rsize);	// 先頭ブロック
		    }
		    if (xsize >= 0x20) {
			// (先頭 + 1) ブロックの LOOP フラグを LOOP のみに
			ADPCM_SETFLAG_LOOP (addr + rsize + 0x10);
		    }
		} else {
		    // 1 ショット: 波形データ読み込みが末尾まで到達した
		    stream_sound_info [ch].end ++;
		}
	    }
	    strenv.buffer [ch].validsize = CPY_BUF_SIZE;
	    ret = 1;
	}
    }

    return (ret);
}

unsigned int
fill_data (unsigned int str_bit)
{
    int ch;
    unsigned int ret_bit;

    ret_bit = 0;
    for (ch = 0; ch < N_STREAM; ch ++) {
	if (str_bit & (1 << ch)) {
	    ret_bit |= fill_data_ch (ch) << ch;
	}
    }

    // データが読み込まれたストリームのビット列
    return (ret_bit);
}

/*
 *	SPU2 設定
 */
void
set_spu2 (void)
{
    /*
     * 光デジタル出力の設定
     */
    sceSkSsSetDigitalOut (SCESK_DOUT_CD_NORMAL);

    /*
     * エフェクトの設定
     */
    sceSkSsSetEffectMode (SCESK_EFFECT_MODE_HALL | SCESK_EFFECT_MODE_CLEAR_WA,
			  0, 0); /* delay, feedback */
    sceSkSsSetEffect (SCESK_EFFECT_ON); 
    sceSkSsSetEffectVolume (SCESK_EFFECT_VOLUME_MAX / 4,  /* L */
			    SCESK_EFFECT_VOLUME_MAX / 4); /* R */

    /*
     * マスターボリューム設定
     */
    sceSkSsSetMasterVolume (SCESK_MASTER_VOLUME_MAX,  /* L */
			    SCESK_MASTER_VOLUME_MAX); /* R */

    return;
}

/*
 *	ストリーム環境の各 ID 割り当て
 */
void
set_stream (void)
{
    int ret;

    /*
     * ADPCM stream データの登録
     *     → ADPCM stream データ ID が返る
     */
    stradpcm_id = sceSkSsBindSTRADPCM (SCESK_AUTOASSIGNMENT);
    PRINTF (("Bind stream %d\n", stradpcm_id));

    /*
     * ADPCM stream データ ID を、トラック (ID: 自動割り当て) に登録
     *     → トラック id が返る
     */
    track_id = sceSkSsBindTrackSTRADPCM (SCESK_AUTOASSIGNMENT, stradpcm_id);
    PRINTF (("Bind track %d\n", track_id));

    /*
     * 環境構造体 env の初期化
     */
    ret = sceSkSsInitEnvironmentSTRADPCM (track_id, &strenv);

    stream_bit = 0;
}

/*
 *	ストリーム環境の各 ID の割り当て解除
 */
void
unset_stream (void)
{
    int ret;

    ret = sceSkSsUnbindTrackSTRADPCM (track_id);
    PRINTF (("Unbind track %d\n", ret));

    ret = sceSkSsUnbindSTRADPCM (stradpcm_id);
    PRINTF (("Unbind stream %d\n", ret));
}

void
set_env_ch (int ch)
{
    int          spu2_addr;
    unsigned int iop_addr;

    /*
     * SPU2 ボイス情報
     */
    strenv.adpcm.channel [ch].voice.core    = SD_CORE_0;	// SPU2 core.
    strenv.adpcm.channel [ch].voice.no      = ch;		// SPU2 voice no.
    strenv.adpcm.channel [ch].voice.options = STREAM_OPTIONS;
    // SPU2 voice volume L/R
    switch (ch) {					
    case _TR_CH_BGM_L:
	strenv.adpcm.channel [ch].voice.voll = VOLUME_L;
	strenv.adpcm.channel [ch].voice.volr = 0;
	break;
    case _TR_CH_BGM_R:
	strenv.adpcm.channel [ch].voice.voll = 0;
	strenv.adpcm.channel [ch].voice.volr = VOLUME_R;
	break;
    case _TR_CH_ONE:
	strenv.adpcm.channel [ch].voice.voll = VOLUME_L;
	strenv.adpcm.channel [ch].voice.volr = VOLUME_R;
	break;
    }
    // SPU2 voice pitch
    strenv.adpcm.channel [ch].voice.pitch = SCESK_STRADPCM_PITCH_1X;
    // SPU2 voice envelope ... See common/include/sdmacro.h and `SPU2 overview'
    strenv.adpcm.channel [ch].voice.adsr1 = SD_ADSR1 (SD_ADSR_A_LINEAR,	/* attack curve */
						      0,		/* attack rate */
						      0xf,		/* decay rate */
						      0xf);		/* sustain level */
    strenv.adpcm.channel [ch].voice.adsr2 = SD_ADSR2 (SD_ADSR_S_EXP_DEC, /*sustain curve */
						      0x7f,		/* sustain rate */
						      SD_ADSR_R_LINEAR,	/* release curve */
						      0xa);		/* release rate */

    /*
     * バッファ情報
     */

    // SPU2 ローカルメモリ内
    while ((spu2_addr = sceSpu2MemAllocate (SPU_BUF_SIZE)) < 0) {
	scePrintf ("[[ No enough memory in SPU2! ]]\n");
    }
    strenv.adpcm.channel [ch].spu2.addr      = spu2_addr;
    strenv.adpcm.channel [ch].spu2.size      = SPU_BUF_SIZE;
    strenv.adpcm.channel [ch].spu2.validsize = 0;

    // IOP メモリ内リングバッファ
    while ((iop_addr = (unsigned int)sceSifAllocIopHeap (IOP_BUF_SIZE)) == (int)NULL) {
	scePrintf ("[[ No enough memory in IOP! ]]\n");
    }
    strenv.adpcm.channel [ch].iop.addr      = iop_addr;
    strenv.adpcm.channel [ch].iop.size      = IOP_BUF_SIZE;
    strenv.adpcm.channel [ch].iop.validsize = 0;

    // (IOP メモリ内) EE メモリ -> IOP メモリ転送バッファ
    while ((iop_addr = (unsigned int)sceSifAllocIopHeap (CPY_BUF_SIZE)) == (int)NULL) {
	scePrintf ("[[ No enough memory in IOP! ]]\n");
    }
    strenv.adpcm.channel [ch].buffer.addr      = iop_addr;
    strenv.adpcm.channel [ch].buffer.size      = CPY_BUF_SIZE;
    strenv.adpcm.channel [ch].buffer.validsize = 0;

    // EE メモリ内データ読み込みバッファ
    // ... アドレスをアンキャッシュ領域に変換
    strenv.buffer [ch].addr      = ((unsigned int)xaddr [ch]) | EE_UNCACHE_MASK;
    strenv.buffer [ch].size      = CPY_BUF_SIZE;
    strenv.buffer [ch].validsize = 0;
}

static void
set_env (void)
{
    int ch;

    strenv.adpcm.version  = 0;		/* バージョン: 現在必ず 0 */
    strenv.adpcm.trans_ch = SD_CORE_0;  /* SPU2 転送チャンネル */
    for (ch = 0; ch < N_STREAM; ch ++) {
	set_env_ch (ch);
    }

    // 設定を IOP 側に反映: 全ストリームチャンネル
    sceSkSsSetupSTRADPCM (track_id, &strenv, _STREAM_ALL);
}

static void
unset_env_ch (int ch)
{
    // SPU2 ローカルメモリ内
    sceSpu2MemFree (strenv.adpcm.channel [ch].spu2.addr);

    // (IOP メモリ内) EE メモリ -> IOP メモリ転送バッファ
    sceSifFreeIopHeap ((void *)strenv.adpcm.channel [ch].buffer.addr);
    // IOP メモリ内リングバッファ
    sceSifFreeIopHeap ((void *)strenv.adpcm.channel [ch].iop.addr);
}

void
unset_env (void)
{
    int ch;

    for (ch = 0; ch < N_STREAM; ch ++) {
	unset_env_ch (ch);
    }

    // track_id に関わる設定を無効に
    sceSkSsUnsetupSTRADPCM (track_id, &strenv);
}

/*
 *	サウンド処理スレッド
 */
void
main_sound (void *argp)
{
    int ret;
    int isstop = 0;
    int note_id;
    int ch;
    unsigned int bit;

    sound_command = _IDLE;
    note_id = 0;

    while (isstop == 0) {
	WaitSema (sema_sound);	/* コマンドを待ち受ける */
	
	sound_return = 0;
	ret = -1;
	switch (sound_command) {
	case _PRELOAD:
	    if ((stream_bit & sound_value) == 0) {
		pre_read_file (sound_value);
		ret = sceSkSsPreloadSTRADPCM (track_id, &strenv, sound_value);
		if (ret == SCESK_OK) {
		    stream_bit |= sound_value;
		}
	    }
	    break;
#if STREAM_AUTOPLAY == 0
	case _PLAY:
	    if ((stream_bit & sound_value) == sound_value) {
		ret = sceSkSsStartSTRADPCM (track_id, &strenv, sound_value);
	    }
	    break;
#endif
	case _STOP:
	    if ((stream_bit & sound_value) == sound_value) {
		ret = sceSkSsTerminateSTRADPCM (track_id, &strenv, sound_value);
		if (ret == SCESK_OK) {
		    rewind_file (sound_value);
		    stop_bit |= sound_value;
		}
	    }
	    break;

	case _PAUSE:
	    if ((stream_bit & sound_value) == sound_value) {
		bit = pause_bit & sound_value;
		if (bit == sound_value) {
		    // depause
		    ret = sceSkSsDepauseSTRADPCM (track_id, &strenv, sound_value);
		    pause_bit &= ~sound_value;
		} else {
		    // pause
		    ret = sceSkSsPauseSTRADPCM (track_id, &strenv, sound_value);
		    pause_bit |= sound_value;
		}
	    }
	    break;

	case _MUTE:
	    if ((stream_bit & sound_value) == sound_value) {
		bit = mute_bit & sound_value;
		if (bit == sound_value) {
		    // demute
		    ret = sceSkSsDemuteSTRADPCM (track_id, &strenv, sound_value);
		    mute_bit &= ~sound_value;
		} else {
		    // mute
		    ret = sceSkSsMuteSTRADPCM (track_id, &strenv, sound_value);
		    mute_bit |= sound_value;
		}
	    }
	    break;

	case _PCHANGE:
	    if ((stream_bit & sound_value) == sound_value) {
		bit = pchange_bit & sound_value;
		if (bit == sound_value) {
		    // pitch 1x
		    for (ch = 0; ch < N_STREAM; ch ++) {
			if (sound_value & (1 << ch)) {
			    strenv.adpcm.channel [ch].voice.mask  = SCESK_STRADPCM_ATTR_PITCH;
			    strenv.adpcm.channel [ch].voice.pitch = SCESK_STRADPCM_PITCH_1X;
			}
		    }
		    ret = sceSkSsChangeAttrSTRADPCM (track_id, &strenv, sound_value);
		    pchange_bit &= ~sound_value;
		} else {
		    // pitch 120% up
		    for (ch = 0; ch < N_STREAM; ch ++) {
			if (sound_value & (1 << ch)) {
			    strenv.adpcm.channel [ch].voice.mask  = SCESK_STRADPCM_ATTR_PITCH;
			    strenv.adpcm.channel [ch].voice.pitch = (SCESK_STRADPCM_PITCH_1X * 120) / 100;
			}
		    }
		    ret = sceSkSsChangeAttrSTRADPCM (track_id, &strenv, sound_value);
		    pchange_bit |= sound_value;
		}
	    }
	    break;

#if STREAM_AUTOPLAY != 0
	case _PLAY_ALL:
	    bit = stream_bit & _STREAM_ALL;
	    if (bit == _STREAM_ALL) {
		// terminate
		ret = sceSkSsTerminateSTRADPCM (track_id, &strenv, bit);
		if (ret == SCESK_OK) {
		    stream_bit &= ~bit;
		    rewind_file (bit);
		}
	    } else {
		bit = (~stream_bit) & _STREAM_ALL;
		pre_read_file (bit);
		ret = sceSkSsPreloadSTRADPCM (track_id, &strenv, bit);
		if (ret == SCESK_OK) {
		    stream_bit |= bit;
		}
	    }
	    break;
#endif

	case _QUIT:
	    ret = SCESK_OK;
	    isstop ++;
	    break;
	}
	if (ret < 0) {
	    PRINTF (("something is wrong...\n"));
	}
	sound_return = ret;
	sound_command = _IDLE;
	sound_value   = 0;
	if (isstop == 0) {	/* Not QUITing ... */
	    SignalSema (sema_sound_send);
	}
	SignalSema (sema_sound_complete);
    }

    return;
}

/*
 *	ストリームデータの IOP 転送とステータス更新
 */
static void
stream_update (void)
{
    int ret;

    if (stream_bit != 0) {
	ret = fill_data (stream_bit);
	DBGPROC (if (ret > 0) scePrintf ("Filled %x\n", ret));
	sceSkSsUpdateSTRADPCM (track_id, &strenv);
    }
}

/*
 *	ストリームデータの転送確認
 */
static void
stream_sync (void)
{
    int ret;

    if (stream_bit != 0) {
	ret = sceSkSsSyncSTRADPCM (track_id, &strenv);
	DBGPROC (if (ret > 0) scePrintf ("Sync %x\n", ret));
	if (stop_bit != 0) {
	    stream_bit &= ~stop_bit;
	    stop_bit = 0;
	}
    }
}

/*
 *	サウンド処理スレッドへのコマンド送出用関数
 *	... 注意: 大域変数を書き換えてコマンドとしているので、
 *		  multi-thread safe では無いことに注意。
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
 *	メイン処理
 */
void
main_loop (void)
{
    int ret, c;
    int note_id, old_note_id;
    unsigned int paddata, oldpad;
    unsigned char rdata [32];

    /* 各種フラグ */
    int isstop  = 0;		/* スレッド終了フラグ */

    scePadPortOpen (0, 0, pad_dma_buf);

    c = 0;
    oldpad = paddata = 0;
    note_id = old_note_id = -1;
    while (isstop == 0) {
	/* コントローラ情報の読み込み */
	if (scePadRead (0, 0, rdata) > 0) paddata = (0xffff ^ ((rdata[2] << 8) | rdata[3]));
	else                              paddata = 0;

	if (paddata != oldpad) {
	    if (paddata & _PAD_QUIT) { /* main_loop() の終了 */
		isstop ++;
	    }
	    else if (paddata & _PAD_PRELOAD_ONE) {
		ret = command_x (0, _PRELOAD, _TR_ONE);		PRINTF (("PRELOAD 1: %d\n", ret));
	    }
	    else if (paddata & _PAD_PRELOAD_BGM) {
		ret = command_x (0, _PRELOAD, _TR_BGM);		PRINTF (("PRELOAD 2: %d\n", ret));
	    }
#if STREAM_AUTOPLAY == 0
	    else if (paddata & _PAD_PLAY_ONE) {
		ret = command_x (0, _PLAY, _TR_ONE);		PRINTF (("PLAY 1: %d\n", ret));
	    }
	    else if (paddata & _PAD_PLAY_BGM) {
		ret = command_x (0, _PLAY, _TR_BGM);		PRINTF (("PLAY 2: %d\n", ret));
	    }
#endif
	    else if (paddata & _PAD_STOP_ONE) {
		ret = command_x (0, _STOP, _TR_ONE);		PRINTF (("STOP 1: %d\n", ret));
	    }
	    else if (paddata & _PAD_STOP_BGM) {
		ret = command_x (0, _STOP, _TR_BGM);		PRINTF (("STOP 2: %d\n", ret));
	    }
	    else if (paddata & _PAD_PAUSE_ONE) {
		ret = command_x (0, _PAUSE, _TR_ONE);		PRINTF (("PAUSE 1: %d\n", ret));
	    }
	    else if (paddata & _PAD_MUTE_ONE) {
		ret = command_x (0, _MUTE, _TR_ONE);		PRINTF (("MUTE 1: %d\n", ret));
	    }
	    else if (paddata & _PAD_MUTE_BGM) {
		ret = command_x (0, _MUTE, _TR_BGM);		PRINTF (("MUTE 2: %d\n", ret));
	    }
	    else if (paddata & _PAD_PCHANGE_ONE) {
		ret = command_x (0, _PCHANGE, _TR_ONE);		PRINTF (("PITCH CHANGE 1: %d\n", ret));
	    }
	    else if (paddata & _PAD_PCHANGE_BGM) {
		ret = command_x (0, _PCHANGE, _TR_BGM);		PRINTF (("PITCH CHANGE 2: %d\n", ret));
	    }
#if STREAM_AUTOPLAY != 0
	    else if (paddata & _PAD_PLAY_ALL) {
		ret = command_x (0, _PLAY_ALL, 0);		PRINTF (("PLAY ALL: %d\n", ret));
	    }
#endif
	}
	c ++;

	stream_update ();
	sceGsSyncV (0);
	stream_sync ();

	oldpad = paddata;
	DBGPROC(if (c % 60 == 0) scePrintf (".\n"));
    }

    PRINTF (("c = %d\n", c));

    // サウンド処理スレッドを終了させる
    sound_command = _QUIT;
    SignalSema (sema_sound);
    WaitSema (sema_sound_complete);
}

/*
 *	メイン処理スレッド
 */
static void
main_main (void *argp)
{
    // コントローラにより再生をコントロールする
    main_loop ();

    SignalSema (*(int *)argp);		// main() に戻る
    return;
}

/*
 *	各種初期化処理
 */
static void
initialize (void)
{
    struct SemaParam sp;

    /*
     * SIF 関連の初期化
     */
    sceSifInitRpc (0);
    sceSifInitIopHeap ();

    /*
     * モジュールのロード
     */
    LOAD_MODULE (_MOD_SIO2MAN);
    LOAD_MODULE (_MOD_PADMAN);
    LOAD_MODULE (_MOD_LIBSD);		/* 低レベルサウンドライブラリ */
    LOAD_MODULE (_MOD_SDRDRV);		/* 低レベルサウンドライブラリ (for EE) */
    LOAD_MODULE (_MOD_STRADPCM);	/* ADPCM ストリーミングライブラリ */
    LOAD_MODULE (_MOD_SK_SOUND);	/* 標準キット: サウンドファンダメンタル */
    LOAD_MODULE (_MOD_SK_STRADPCM);	/* 標準キット: ADPCM ストリーミング */

    /*
     * コントローラライブラリの初期化
     */
    scePadInit (0);

    /*
     * 標準キット + 標準キット/サウンドシステム ... 初期化
     */
    sceSkInit (0);	/* 標準キット全体の初期化: 引数 ... 必ず 0 */
    sceSkSsInit (32);	/* 標準キット/サウンドシステムの初期化:
			 *   引数 ... sdr 割り込みハンドラスレッドのプライオリティ */

    /*
     * 標準キットサウンドシステム ... ADPCM ストリーミング
     */
    sceSkSsInitSTRADPCM (); /* SK/Sound System ADPCM ストリーミングの初期化 */

    /*
     * SPU2 メモリ管理の初期化
     */
    sceSpu2MemInit (spu2mem_table, SPU2MEM_N_AREAS, /* 領域数: SPU2MEM_N_AREAS */
		    SCESPU2MEM_USE_EFFECT);	    /* エフェクトは使用する */

    /*
     * SPU2 の全般に関わる属性の設定
     */
    set_spu2 ();

    set_stream ();
    set_env ();
    open_file ();

    /*
     * 待ち受けセマフォの生成
     */
    sp.initCount = 0;
    sp.maxCount  = 1;
    sp.option    = 0;
    // メイン処理スレッド終了用セマフォ
    while ((sema_main = CreateSema (&sp)) < 0) {
	scePrintf ("[[ Error: can't create semaphore.\n]]");
    }
    // サウンド処理スレッド用セマフォ
    while ((sema_sound = CreateSema (&sp)) < 0) {
	scePrintf ("[[ Error: can't create semaphore.\n]]");
    }
    // サウンド処理コマンド終了待ち受け用セマフォ
    while ((sema_sound_complete = CreateSema (&sp)) < 0) {
	scePrintf ("[[ Error: can't create semaphore.\n]]");
    }
    sp.initCount = 1;		/* 先に WaitSema() を行う */
    // サウンド処理コマンド用セマフォ
    while ((sema_sound_send = CreateSema (&sp)) < 0) {
	scePrintf ("[[ Error: can't create semaphore.\n]]");
    }

    return;
}

/*
 *	各種終了処理
 */
static void
finalize (void)
{
    if (stream_bit != 0) {
	sceSkSsTerminateSTRADPCM (track_id, &strenv, stream_bit);
	stream_update ();
	stream_sync ();
    }
	
    close_file ();
    unset_env ();
    unset_stream ();

    // SPU2 メモリ管理の終了
    sceSpu2MemQuit();

    /*
     * 待ち受けセマフォの削除
     */
    DeleteSema (sema_sound);
    DeleteSema (sema_sound_send);
    DeleteSema (sema_sound_complete);
    DeleteSema (sema_main);

    sceSkSsQuitSTRADPCM (); /* SK/Sound System ADPCM ストリーミングの終了 */

    return;
}

/*
 *	スタートエントリ
 */
int
main (int argc, char *argv [])
{
    struct ThreadParam tp;

    /*
     *	初期化処理
     */
    initialize ();

    /*
     * メイン処理スレッドの生成と起動
     */
    tp.stackSize    = STACK_SIZE_MAIN;
    tp.gpReg 	    = &_gp;
    tp.entry 	    = main_main;
    tp.stack 	    = (void *)stack_main;
    tp.initPriority = 3;
    if ((thid_main = CreateThread (&tp)) < 0) {
	// Can't create thread
	return (-2);
    }
    StartThread (thid_main, (void *)&sema_main);

    /*
     * サウンド処理スレッドの生成と起動
     */
    tp.stackSize    = STACK_SIZE_SOUND;
    tp.gpReg 	    = &_gp;
    tp.entry 	    = main_sound;
    tp.stack 	    = (void *)stack_sound;
    tp.initPriority = 4;
    thid_sound = CreateThread (&tp);
    if (thid_sound < 0) {
	return 0;		// Can't create thread
    }
    StartThread (thid_sound, (void *)&sema_sound);

    /*
     * メイン処理スレッドの終了を待つ
     */
    WaitSema (sema_main);

    /*
     * リソースを解放
     */
    if (TerminateThread (thid_sound) == thid_sound) {
	DeleteThread (thid_sound);
    }
    if (TerminateThread (thid_main) == thid_main) {
	DeleteThread (thid_main);
    }

    /*
     *	終了処理
     */
    finalize ();

    // Fine ...
    PRINTF (("Fine ...\n"));

    return (0);
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADD STUFF AFTER THIS */
