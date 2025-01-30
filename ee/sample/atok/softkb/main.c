/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                      ATOK Library Sample
 *
 *                         Version 0.10
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.10
 */
/* standard C include file */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

/* sce include file */
#include <eekernel.h>
#include <eeregs.h>
#include <devvif1.h>
#include <devvu1.h>
#include <libdma.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <libgraph.h>
#include <libvu0.h>
#include <libcdvd.h>
#include <libdev.h>
#include <libpkt.h>
#include <sif.h>
#include <sifcmd.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libpad.h>
#include <libmc.h>
#include <libcdvd.h>
#include <libmrpc.h>
#include <libsdr.h>
#include <libscf.h>

#include "pad.h"
#include "kbd.h"
#include <libccc.h>
#include <fontwrap.h>

#include <libfep.h>
#include <libsoftkb.h>
#include <skb.h>


//--------------------------- 
#define USE_USBKB           1 /* USB キーボードを使用する */
//---------------------------


#if BOOT_FROM_CD
//--------------------------------------------------------------------------------
// モジュールをＣＤから読む場合のパス
//--------------------------------------------------------------------------------
#define SIO2MAN_IRX     "cdrom0:\\MODULES\\SIO2MAN.IRX;1"
#define PADMAN_IRX      "cdrom0:\\MODULES\\PADMAN.IRX;1"
#define MCMAN_IRX       "cdrom0:\\MODULES\\MCMAN.IRX;1"
#define MCSERV_IRX      "cdrom0:\\MODULES\\MCSERV.IRX;1"
#define DEV9_IRX        "cdrom0:\\MODULES\\DEV9.IRX;1"
#define ATAD_IRX        "cdrom0:\\MODULES\\ATAD.IRX;1"
#define HDD_IRX         "cdrom0:\\MODULES\\HDD.IRX;1"
#define PFS_IRX         "cdrom0:\\MODULES\\PFS.IRX;1"

#define USBD_IRX        "cdrom0:\\MODULES\\USBD.IRX;1"
#define USBKB_IRX       "cdrom0:\\MODULES\\USBKB.IRX;1"


#define JIS2UCS         "cdrom0:\\DATA\\JIS2UCS.BIN;1"
#define UCS2JIS         "cdrom0:\\DATA\\UCS2JIS.BIN;1"
#define PFONT_DAT_PATH  "cdrom0:\\DATA\\SAMPLE.PF;1"


#else /* BOOT_FROM_CD */
//--------------------------------------------------------------------------------
// モジュールをhostから読む場合のパス
//--------------------------------------------------------------------------------
#define TOP_DIR         "host0:/usr/local/sce/"
#define IOP_MOD_DIR     TOP_DIR"iop/modules/"

#define SIO2MAN_IRX     IOP_MOD_DIR"sio2man.irx"
#define PADMAN_IRX      IOP_MOD_DIR"padman.irx"
#define MCMAN_IRX       IOP_MOD_DIR"mcman.irx"
#define MCSERV_IRX      IOP_MOD_DIR"mcserv.irx"
#define DEV9_IRX        IOP_MOD_DIR"dev9.irx"
#define ATAD_IRX        IOP_MOD_DIR"atad.irx"
#define HDD_IRX         IOP_MOD_DIR"hdd.irx"
#define PFS_IRX         IOP_MOD_DIR"pfs.irx"

#define USBD_IRX        IOP_MOD_DIR"usbd.irx"
#define USBKB_IRX       IOP_MOD_DIR"usbkb.irx"


#define JIS2UCS         "host0:utf8/libccc/lib/jis2ucs.bin"
#define UCS2JIS         "host0:utf8/libccc/lib/ucs2jis.bin"
#define PFONT_DAT_PATH  "host0:libpfont/sample/basic/sample.pf"

//--------------------------------------------------------------------------------
#endif

//--------------------------------------------------------------------------------
// ＡＴＯＫのパス、システム環境ファイルのパス。
// ＨＤＤ付属ＤＩＳＣ、システムソフトウェアのインストールで、インストールされます。
//--------------------------------------------------------------------------------
#define SYSCONF_PARTITION "hdd0:__sysconf"  // Atokインストール済みパーティション
#define SYSCONF_DEVICE    "pfs0:"           // (system.ini のパーティションも同じ)
#define ATOK_DEVICE       SYSCONF_DEVICE    // 
#define SYSTEM_INI_DEVICE SYSCONF_DEVICE    // 
#define ATOK_SYSDIC_PATH  ATOK_DEVICE"/atok/atokp.dic"    // ATOKシステム辞書のパス
#define ATOK_USERDIC_PATH ATOK_DEVICE"/atok/atokpyou.dic" // ATOKユーザー辞書のパス
//#define ATOK_USERDIC_PATH "host0:AtokUser.dic" // デバッグ用 ATOKユーザー辞書のパス



//--------------------------------------------------------------------------------
// Gs WorkAddress
//--------------------------------------------------------------------------------
#define SAMPLE_FREE_GSMEM (8960) // 0-8960を画面表示に使用するので、以降が空き領域
                                // 単位64ワード（256バイト）
// 空き領域を、pfont と ソフトウェアキーボードで使用する
// libpfontで使用するアドレスを直指定。こちらは単位がword
#define CLUT_GSMEM       (SAMPLE_FREE_GSMEM*256/4)     // libpfont用Clut転送アドレス
#define CLUT_GSMEMSIZE   (64*4)                        // 64ワード*4
#define FONT_GSMEM       (CLUT_GSMEM + CLUT_GSMEMSIZE) // その後ろをTex転送アドレスに
#define FONT_GSMEMSIZE   (64*64)                       // 64ワード*64 fontsizeMax 64ｘ64
// 続けてソフトウェアキーボードで使用するアドレスを直指定。単位は64ワード（256バイト）
#define FONT_GSMEM_END   ((FONT_GSMEM+FONT_GSMEMSIZE)/(256/4)) // 単位を戻す。
#define SKB_GSMEM        FONT_GSMEM_END  // ソフトウェアキーボードの転送ワークアドレス
//--------------------------------------------------------------------------------

// VSync割り込み関係
int VSync_HandleID;
volatile int VSync_SemaID;

// コントローラ ＤＭＡバッファ
u_long128 pad_dma_buf[scePadDmaBufferMax]  __attribute__((aligned (64)));
u_long ulPad=0;

// USB キーボード 
int UsbKB_InitFlag=0;
USBKBDATA_t UsbKbData;

// libpccc ライブラリ 文字コード変換テーブル用バッファ
static sceCccJISCS JisUcs2Tbl[65536];   // jis->ucs 変換テーブル用バッファ
static sceCccUCS2  Ucs2JisTbl[65536];   // ucs->jis 変換テーブル用バッファ

// libpfontライブラリ フォントデータバッファ
#define FONT_DATASIZE  (3 * 1024 * 1024) // 余裕みて3MB、配列でとってしまっている。
static u_long128 pfont_data[(FONT_DATASIZE+sizeof(u_long128)-1)/sizeof(u_long128)] __attribute__((aligned (128)));

static sceGsDBuff g_db;         // ダブルバッファ
static sceVif1Packet PkVif1[2]; // パケットの管理情報
static int db_id=0;

char TextBufSjis[1024];
int  TextBufUcs[1024];

//--------------------------------------------------------------------------------
//  irx モジュール、ロードオプション
//--------------------------------------------------------------------------------
static const char hdd_arg[] = "-o" "\0" "3" "\0" "-n" "\0" "3";
                             // -o 同時オープンバーティション数(default 10)
                             // -n   ブロックバッファ数(default 3)
static const char pfs_arg[] = "-m" "\0" "3" "\0" "-o" "\0" "10" "\0" "-n" "\0" "10";
                             // -m 同時マウント数(default 1)
                             // -o キャッシュバッファ数(default 8)
                             // -n 同時オープンファイル数(1オープンにキャッシュバッファ使用1-2)



//--------------------------------------------------------------------------------
//   キーボードのセットのレイアウトを定義
//--------------------------------------------------------------------------------
#define SKB_OFFSET_X (40)
#define SKB_OFFSET_Y (54)
//描画構成位置
#define TEXT_BOX_X ( 60 - SKB_OFFSET_X )
#define TEXT_BOX_Y ( 62 - SKB_OFFSET_Y )
#define TEXT_BOX_W (500)
#define TEXT_BOX_H ( 50)
#define KEYBORD_X  (TEXT_BOX_X)
#define KEYBORD_Y  (TEXT_BOX_Y+TEXT_BOX_H-2 )
#define FEP_BAR_X  (TEXT_BOX_X+TEXT_BOX_W-80)
#define FEP_BAR_Y  (TEXT_BOX_Y+TEXT_BOX_H-2 )
#define MODE_BAR_RX (TEXT_BOX_X+TEXT_BOX_W-2-12)
#define MODE_BAR_Y  (TEXT_BOX_Y+264)

//ＦＥＰの入力モード
static u_int FepMode_Kanji[] =
{
	FEPON_HIRA,            // ひらがな、漢字変換
	FEPON_ZEN_KATAKANA,    // カタカナ、漢字変換
	FEPON_HAN_KATAKANA,    // 半角カタカナ変換あり
	FEPON_ZEN_ALPHABET,    // 全角英文字  変換あり
	FEPON_HAN_ALPHABET,    // 半角英文字  変換あり
	FEPOFF                 // 変換無し
};

static u_int FepMode_HiKanji[] =
{
	FEPON_HIRA_FIX,           // ひらがな、漢字変換無し
	FEPON_ZEN_KATAKANA_FIX,   // カタカナ、漢字変換無し
	FEPOFF                    // 変換無し
};

static u_int FepMode_Off[] =
{
	FEPOFF                    // 変換無し
};

//入力禁止キー定義（コントロールキー）
u_int CtrlKeyDisabele_HanCaps[]={ SKBCTRL_KANJI, SKBCTRL_CAPS, SKBCTRL_DOWN, SKBCTRL_UP, SKBCTRL_TAB, 0, };
u_int CtrlKeyDisabele_Zenkaku[]={ SKBCTRL_KANJI,               SKBCTRL_DOWN, SKBCTRL_UP, SKBCTRL_TAB, 0, };
u_int CtrlKeyDisabele_Caps[]   ={                SKBCTRL_CAPS, SKBCTRL_DOWN, SKBCTRL_UP, SKBCTRL_TAB, 0, };
u_int CtrlKeyDisabele_UpDown[] ={                              SKBCTRL_DOWN, SKBCTRL_UP, SKBCTRL_TAB, 0, };


// < サンプル キーボード定義 説明 >
//   ソフトウェアキーボードは、セレクトボタンで、次のキーボードに代わり、繰り返すことでループします。
//   KEYBOARD_SET構造体は、キーボードセットとしてそれらを定義する構造体です。
//
//   ソフトウェアキーボード。セレクトボタンで、次のキーボードに代わり、繰り返すことでループします。
//     デフォルトのキーボード : 最初に表示されるキーボードです。
//     次のキーボード         : セレクトボタンで代わるキーボードを指し、使用するキーボードでループさせるデータとします。
//     Fepの入力モード        : Fepの入力モードはキーボードごとに固定されてますが、FEPOFF指定ができます。また、
//                              オープン直後はこの指定を使用していますので、キーボードとロック状態に合わて指定します。
//     ロックの初期状態の指定 : 全角キー、キャプスキーそれらのキーのないキーボードは指定出来ません。
//     使用不可コントロールキーの指定： SKBCTRL_xxx で指定。null ターミネートu_int 配列ポインタ。
//     ソフトウェアキーボード表示位置： キーボードの表示位置。
//   FEPバー用定義。（ＵＳＢキーボードでの入力時に表示される。）
//     FEPの入力モード : 使用するＦＥＰの入力モードを定義。セレクトボタン、F12で、この順番でモードループ。
//     FEPバー表示位置 : ｘｙ
//   モードバー用定義。オープンキーボード種の一覧表示バー。ソフトウェアキーボードと同期する。
//     モードバーの定義：指定不要。ソフトウェアキーボードの定義から自動生成。
//     モードバーの位置：ｘｙ
//   テキストボックス用定義。
//     テキストボックスの位置とサイズ：テキストフィールドの位置とサイズ。

// 非漢字用キーボード定義
KEYBOARD_SET KeyBoardSet_HiKanji = {
	SKBTYPE_HIRA,// ひらがなキーボードをデフォルトとする。
	{
	/*              次のキーボード      Fepの入力モード   初期ロック状態  使用不可コントロールキー  表示座標 */
	/* ひらがな   */{ SKBTYPE_KANA  , FEPOFF            , LAYOUT_ZENKAKU, CtrlKeyDisabele_HanCaps, { KEYBORD_X,KEYBORD_Y } },
	/* カタカナ   */{ SKBTYPE_QWERTY, FEPOFF            , LAYOUT_ZENKAKU, CtrlKeyDisabele_HanCaps, { KEYBORD_X,KEYBORD_Y } },
	/* ＡＢＣ順   */{ SKBTYPE_NUM   , FEPOFF            , 0             , CtrlKeyDisabele_Zenkaku, { KEYBORD_X,KEYBORD_Y } },
	/* キーボード */{ SKBTYPE_NUM   , FEPOFF            , 0             , CtrlKeyDisabele_Zenkaku, { KEYBORD_X,KEYBORD_Y } },
	/* 数字       */{ SKBTYPE_HIRA  , FEPOFF            , 0             , CtrlKeyDisabele_Zenkaku, { KEYBORD_X,KEYBORD_Y } },
	/* 記号       */{ SKBTYPE_HIRA  , FEPOFF            , 0             , 0                      , { KEYBORD_X,KEYBORD_Y } },
	// 記号は使用していない。
	},
	/*           Fepの入力モード数             Fepの入力モード        表示座標              */
	{ sizeof( FepMode_HiKanji )/sizeof(u_int), FepMode_HiKanji,  { FEP_BAR_X, FEP_BAR_Y }},
	{ 0,{0},{0},    { MODE_BAR_RX-24*4, MODE_BAR_Y }},
	{ { TEXT_BOX_X, TEXT_BOX_Y, TEXT_BOX_W, TEXT_BOX_H } },
};


// アルファベットと数字キーボード用の定義
KEYBOARD_SET KeyBoardSet_ABC123 = {
	SKBTYPE_QWERTY, // キーボード配列をデフォルトとする。
	{
	/*              次のキーボード      Fepの入力モード   初期ロック状態  使用不可コントロールキー  表示座標 */
	/* ひらがな   */{ SKBTYPE_QWERTY, FEPOFF            , LAYOUT_ZENKAKU, CtrlKeyDisabele_HanCaps, { KEYBORD_X,KEYBORD_Y } },
	/* カタカナ   */{ SKBTYPE_QWERTY, FEPOFF            , LAYOUT_ZENKAKU, CtrlKeyDisabele_HanCaps, { KEYBORD_X,KEYBORD_Y } },
	/* ＡＢＣ順   */{ SKBTYPE_NUM   , FEPOFF            , 0             , CtrlKeyDisabele_Zenkaku, { KEYBORD_X,KEYBORD_Y } },
	/* キーボード */{ SKBTYPE_NUM   , FEPOFF            , 0             , CtrlKeyDisabele_Zenkaku, { KEYBORD_X,KEYBORD_Y } },
	/* 数字       */{ SKBTYPE_QWERTY, FEPOFF            , 0             , CtrlKeyDisabele_Zenkaku, { KEYBORD_X,KEYBORD_Y } },
	/* 記号       */{ SKBTYPE_QWERTY, FEPOFF            , 0             , 0                      , { KEYBORD_X,KEYBORD_Y } },
	// ひらがな カタカナ 記号は、使用していない。
	},
	/*     Fepの入力モード数                   Fepの入力モード        表示座標              */
	{ sizeof( FepMode_Off )/sizeof(u_int),        FepMode_Off, { FEP_BAR_X, FEP_BAR_Y }},
	{ 0,{0},{0},    { MODE_BAR_RX-24*2, MODE_BAR_Y }},
	{ { TEXT_BOX_X, TEXT_BOX_Y, TEXT_BOX_W, TEXT_BOX_H } },
};


// 数字キーボードのみ用の定義
KEYBOARD_SET KeyBoardSet_123 = {
	SKBTYPE_NUM, // 数字キーボードをデフォルトとする。
	{
	/*              次のキーボード      Fepの入力モード   初期ロック状態  使用不可コントロールキー  表示座標 */
	/* ひらがな   */{ SKBTYPE_NUM   , FEPOFF            , LAYOUT_ZENKAKU, CtrlKeyDisabele_HanCaps, { KEYBORD_X,KEYBORD_Y } },
	/* カタカナ   */{ SKBTYPE_NUM   , FEPOFF            , LAYOUT_ZENKAKU, CtrlKeyDisabele_HanCaps, { KEYBORD_X,KEYBORD_Y } },
	/* ＡＢＣ順   */{ SKBTYPE_NUM   , FEPOFF            , 0             , CtrlKeyDisabele_Zenkaku, { KEYBORD_X,KEYBORD_Y } },
	/* キーボード */{ SKBTYPE_NUM   , FEPOFF            , 0             , CtrlKeyDisabele_Zenkaku, { KEYBORD_X,KEYBORD_Y } },
	/* 数字       */{ SKBTYPE_NUM   , FEPOFF            , 0             , CtrlKeyDisabele_Zenkaku, { KEYBORD_X,KEYBORD_Y } },
	/* 記号       */{ SKBTYPE_NUM   , FEPOFF            , 0             , 0                      , { KEYBORD_X,KEYBORD_Y } },
	// 数字以外は、使用していない。
	},
	/*     Fepの入力モード数                   Fepの入力モード        表示座標              */
	{ sizeof( FepMode_Off )/sizeof(u_int),        FepMode_Off , { FEP_BAR_X, FEP_BAR_Y }},
	{ 0,{0},{0},    { MODE_BAR_RX-24*1, MODE_BAR_Y }},
	/*  テキストボックス  ＸＹ  ＷＨ                      */
	{ { TEXT_BOX_X, TEXT_BOX_Y, TEXT_BOX_W, TEXT_BOX_H } },
};

#ifdef SAMPLE_REGION_J
// ずべてのキーボード用定義（漢字入力も可、ﾃｽﾄ用）
KEYBOARD_SET KeyBoardSet_All = {
	SKBTYPE_HIRA,// ひらがなキーボードをデフォルトとする。
	{
	/*              次のキーボード    Fepの入力モード     初期状態        使用不可コントロールキー  表示座標 */
	/* ひらがな   */{ SKBTYPE_KANA  , FEPON_HIRA        , LAYOUT_ZENKAKU, CtrlKeyDisabele_HanCaps, { KEYBORD_X,KEYBORD_Y } },
	/* カタカナ   */{ SKBTYPE_QWERTY, FEPON_ZEN_KATAKANA, LAYOUT_ZENKAKU, CtrlKeyDisabele_Caps   , { KEYBORD_X,KEYBORD_Y } },
	/* ＡＢＣ順   */{ SKBTYPE_NUM   , FEPOFF            , 0             , CtrlKeyDisabele_UpDown , { KEYBORD_X,KEYBORD_Y } },
	/* キーボード */{ SKBTYPE_NUM   , FEPOFF            , 0             , CtrlKeyDisabele_UpDown , { KEYBORD_X,KEYBORD_Y } },
	/* 数字       */{ SKBTYPE_SIGN  , FEPOFF            , 0             , CtrlKeyDisabele_UpDown , { KEYBORD_X,KEYBORD_Y } },
	/* 記号       */{ SKBTYPE_HIRA  , FEPOFF            , 0             , 0                      , { KEYBORD_X,KEYBORD_Y } },
	},
	/*           Fepの入力モード数                Fepの入力モード        表示座標            */
	{ sizeof( FepMode_Kanji )/sizeof(u_int),      FepMode_Kanji,  { FEP_BAR_X, FEP_BAR_Y }},
	{ 0,{0},{0},    { MODE_BAR_RX-24*5, MODE_BAR_Y }},
	{ { TEXT_BOX_X, TEXT_BOX_Y, TEXT_BOX_W, TEXT_BOX_H } },
};
#else
// ずべてのキーボード用定義（ラテン文字アルファベット可（要フォントサポート）ﾃｽﾄ用）
KEYBOARD_SET KeyBoardSet_All_AE = {
	SKBTYPE_QWERTY,// キーボード配列をデフォルトとする。
	{
	/*              次のキーボード      Fepの入力モード       初期状態    使用不可コントロールキー  表示座標 */
	/* なし       */{ SKBTYPE_ABC   , FEPOFF            , LAYOUT_ZENKAKU, CtrlKeyDisabele_UpDown , { KEYBORD_X,KEYBORD_Y } },
	/* なし       */{ SKBTYPE_ABC   , FEPOFF            , LAYOUT_ZENKAKU, CtrlKeyDisabele_UpDown , { KEYBORD_X,KEYBORD_Y } },
	/* Latin英字  */{ SKBTYPE_NUM   , FEPOFF            , 0             , CtrlKeyDisabele_UpDown , { KEYBORD_X-10,KEYBORD_Y } },
	/* キーボード */{ SKBTYPE_ABC   , FEPOFF            , 0             , CtrlKeyDisabele_UpDown , { KEYBORD_X,KEYBORD_Y } },
	/* 数字       */{ SKBTYPE_SIGN  , FEPOFF            , 0             , CtrlKeyDisabele_UpDown , { KEYBORD_X,KEYBORD_Y } },
	/* Latin記号  */{ SKBTYPE_QWERTY, FEPOFF            , 0             , CtrlKeyDisabele_UpDown , { KEYBORD_X,KEYBORD_Y } }, 
	// ひらがな、カタカナ、使用していない。
	},
	/*           Fepの入力モード数                Fepの入力モード        表示座標            */
	{ sizeof( FepMode_Off )/sizeof(u_int),     FepMode_Off,  { FEP_BAR_X, FEP_BAR_Y }},
	{ 0,{0},{0},    { MODE_BAR_RX-24*4, MODE_BAR_Y }},
	{ { TEXT_BOX_X, TEXT_BOX_Y, TEXT_BOX_W, TEXT_BOX_H } },
};
#endif

//--------------------------------------------------------------------------------
// 文字コード変換
//--------------------------------------------------------------------------------
// Sjis -> Ucs 文字列への変換
int SjisToUcs4(const char *pSjis, u_int* ucs4, u_int buffMax )
{
	int i = 0;
	sceCccJISCS	jis;
	sceCccUCS4 chr=0;
	sceCccSJISTF *pPtr = (sceCccSJISTF *)pSjis;

	if( (!ucs4) || buffMax==0 )	return -1;

	while( *pPtr )
	{
		if( i < buffMax-1 ){
			jis = sceCccDecodeSJIS((sceCccSJISTF const **)&pPtr); // SJIS -> JIS
			ucs4[i] = sceCccJIStoUCS(jis, chr);                   // JIS  -> UCS
			i++;
		}
		else{
			ucs4[i] = 0;
			return -i;
		}
	}
	ucs4[i] = 0;

	return i;
}

// Ucs -> Sjis 文字列への変換
static void UcsToSjis( u_int* ucs4, unsigned char *sjis, u_int buffMax )
{
	u_char utf8char[12] = "";
	sceCccUTF8 *pUtf8;       // UTF8-Buffer先頭(Pointer格納用)
	sceCccUTF8**ppUtf8 = &pUtf8; // 元変換先頭位置
	int len;

	if( *ucs4 == 0 ){
		if( buffMax )	*sjis = 0;
		return;
	}

	while( *ucs4 && buffMax >= 0 ){
		pUtf8  = utf8char; // UTF8-Buffer先頭(Pointer格納用)
		sceCccEncodeUTF8( ppUtf8, *ucs4 ); // UCS4 -> UTF8変換
		ucs4++;                            
		**ppUtf8 = 0;

		sceCccUTF8toSJIS( sjis, buffMax, utf8char );
		len = strlen(sjis);
		buffMax-=len;
		sjis   +=len;
	}
}


//--------------------------------------------------------------------------------
// VBlankStart 割り込みハンドラ
//--------------------------------------------------------------------------------
static int InterlaceField=0;
static int VBlankStartHandler(int ch)
{
	struct SemaParam sp;

	InterlaceField^=1;
	iReferSemaStatus( VSync_SemaID, &sp );

	/* カウントが0のときのみシグナルする */
	if( sp.currentCount == 0 ){
	    iSignalSema(VSync_SemaID);
	}
	PadVSyncCount();

	ExitHandler(); /* 割り込みハンドラ終了               */
	return -1;     /* 以後の割り込みハンドラは起動しない */
}

//--------------------------------------------------------------------------------
// 電源コントロール
//--------------------------------------------------------------------------------
#define STACKSZ (512 * 16)
int PowerOff_ThreadID;
int PowerOff_SemaID;
int (*GetWaitPowerOff)(void) = ( int(*)(void) )0;
int PowerOff_Start=0;
int PowerOff_Device=0;
static u_char stack[STACKSZ] __attribute__ ((aligned(16)));

static void PowerOffThread(void *arg)
{
	int sid = (int)arg;
	int stat = 0;
	int waitPowerOff_SemaID=-1;

	WaitSema(sid); /* 起動後、すぐに待ちに入る */

	printf("power off request has come.\n");
	sceCdPOffCallback( 0, (void *)0 );  /* 電源落とすので、２度呼びさせない。*/

	PowerOff_Start=1;

	if( GetWaitPowerOff )	waitPowerOff_SemaID = GetWaitPowerOff();
	else                 	waitPowerOff_SemaID = -1;
	if( waitPowerOff_SemaID >= 0 )
		WaitSema(waitPowerOff_SemaID);  /*  ソフトウェア側で終了処理がある場合は待つ */

	/* トレイが開いたまま、電源が落ちないように、ＣＤトレイクローズ。*/
	sceCdTrayReq(SCECdTrayClose, &stat);

	/* dev9 power off, need to power off PS2 */
	if( PowerOff_Device ){
		/* close all files */
		sceDevctl("pfs:", PDIOC_CLOSEALL, NULL, 0, NULL, 0);
	}
	/* ライブラリバージョン 2.53 から、dev9x の電源ＯＦＦで、ＨＤＤの電源もＯＦＦされます。*/
	while( sceDevctl("dev9x:", DDIOC_OFF    , NULL, 0, NULL, 0) < 0 );

	/* 念のため、最後にＣＤトレイクローズ。*/
	sceCdTrayReq(SCECdTrayClose, &stat);

	/* PS2 power off */
	while(!sceCdPowerOff(&stat)||stat);
	printf("power off ps2\n");

	for(;;); /* 他のスレッドに処理を渡さない */

	return;
}

static void PowerOffHandler(void *arg)
{
	int sid = (int)arg;

	iSignalSema(sid); // PowerOffThreadの待ちを解除
}

static void CreatePowerOffThread( int prio )
{
	struct ThreadParam tparam;
	struct SemaParam   sparam;

	sparam.initCount = 0;
	sparam.maxCount  = 1;
	sparam.option    = 0;
	PowerOff_SemaID = CreateSema( &sparam );

	tparam.stackSize = STACKSZ;
	tparam.gpReg = &_gp;
	tparam.entry = PowerOffThread;
	tparam.stack = (void *)stack;
	tparam.initPriority = prio;
	PowerOff_ThreadID = CreateThread(&tparam);

	StartThread( PowerOff_ThreadID, (void *)PowerOff_SemaID );

	sceCdPOffCallback( PowerOffHandler, (void *)PowerOff_SemaID );
}

//--------------------------------------------------------------------------------
// irx モジュールロード関数
//--------------------------------------------------------------------------------
// ロード成功まで、無限ループ
int sifLoadModule( const char *module, int args, const char *argp )
{
	int ret;

	for(;;){
		ret = sceSifLoadModule( module, args , argp );

		if( ret < 0 ){
			printf( "Can't load module [ %s ]:%d\n",module,-ret );
			continue;
		}
		break;
	}
	return ret;
}

// ロード失敗時、または、 irx が、常駐しないで終了した場合は、
// printf出力して、マイナス値を返す。
int _sifLoadStartModule( const char *module, int args, const char *argp )
{
	int ret,result;

	ret = sceSifLoadStartModule( module, args, argp, &result );
	if( ret < 0 ){
		printf( "Can't load module [ %s ]:%d\n",module,-ret );
		return ret;
	}
	if( result == NO_RESIDENT_END ){
		printf( "module [ %s ]:NO_RESIDENT_END\n",module );
		return -0x10000000; // 負の値として、判別可能な値を返している。
	}
	return ret;
}


static int skbStep=0;
#define STEP_SKB_NONE   ( 0)
#define STEP_SKB_INIT   ( 1)
#define STEP_SKB_OPENED ( 2)
#define STEP_SKB_CLOSE  (-1)
#define STEP_SKB_EXIT   (-2)

// サンプルソフトウェアキーボード、コールバック関数。
static int MessageFromSKB( int messType, void * p )
{
	/* メッセージ別に 処理 */
	switch( messType ){
	/* --- 編集結果を、取得してクローズ -------------------------------------*/
	case SKBMES_RESULT   :/* テキスト編集結果報告 */
	                        if( p ){
	                        	UcsToSjis( (u_int*)p, TextBufSjis, 1024 );
	                        }
	                        skbStep = STEP_SKB_CLOSE;
	                        break;
	/* --- 画面下部の操作ガイドの変更 ---------------------------------------*/
	case SKBMES_USBKEY_ON:/* USBKEYBORDに切り替え報告 */
	                        break;

	case SKBMES_HELP_SHOW:/* Help画面表示報告 */
	                        break;
	case SKBMES_FEP_ON   :/* 変換有効報告     */
	                        break;

	case SKBMES_USBKEY_OFF:/* 画面KEYBORDに切り替え報告 */
	case SKBMES_HELP_HIDE :/* Help画面表示解除報告 */
	case SKBMES_FEP_OFF   :/* FEP_Off報告 */
	                        break;
	/* -------- 効果音 --------------------------------------------- */
	case SKBMES_SE        :/* 効果音＆発音タイミング報告 */
	                        break;
	}
	return 0;
}


//--------------------------------------------------------------------------------
//  メイン
//--------------------------------------------------------------------------------
int main()
{
	static u_int use_hdd = 0;
	int hddDeviceFlag=0;
	int ret = 0;
	int i;

	sceSifInitRpc(0);

	#if BOOT_FROM_CD
	sceCdInit(SCECdINIT);
	while( sceSifRebootIop( "cdrom0:\\MODULES\\"IOP_IMAGE_FILE";1" ) == 0 );
	#else
	while( sceSifRebootIop( IOP_MOD_DIR IOP_IMAGE_file ) == 0 );
	#endif /* BOOT_FROM_CD */

	while( sceSifSyncIop() == 0 );
	sceSifInitRpc(0);
	sceSifLoadFileReset();

	#if BOOT_FROM_CD
	sceCdInit(SCECdINIT);
	sceCdMmode(SCECdCD);
	#endif /* BOOT_FROM_CD */
	sceFsReset();
	sceSifInitIopHeap();

	/* iopモジュールのロード ----------------------------------------*/
	/* コントローラ、メモリーカード関係 */
	sifLoadModule( SIO2MAN_IRX , 0                   , NULL         );
	sifLoadModule( PADMAN_IRX  , 0                   , NULL         );
	sifLoadModule( MCMAN_IRX   , 0                   , NULL         );
	sifLoadModule( MCSERV_IRX  , 0                   , NULL         );

	/* ハードディスクドライブ関係 */
	ret = _sifLoadStartModule( DEV9_IRX, 0 , NULL );
	if( ret >= 0 ){
		if( _sifLoadStartModule( ATAD_IRX, 0 , NULL ) >= 0 ){
			if( _sifLoadStartModule( HDD_IRX , sizeof(hdd_arg) , hdd_arg ) >= 0 ){
				hddDeviceFlag = 1; // ハードディスクOK
				if( _sifLoadStartModule( PFS_IRX , sizeof(pfs_arg) , pfs_arg ) >= 0 ){
					use_hdd = 1;
				}
			}
		}
	}

	sifLoadModule( USBD_IRX    , 0                   , NULL         );
	#if USE_USBKB
    sifLoadModule( USBKB_IRX   , 0                   , NULL         );
	#endif
	/*----------------------------------------------------------------*/


	sceDmaReset(1);

	// BCOND0設定
	{
		sceDmaEnv env;
		
		sceDmaGetEnv(&env);							// DMA共通環境を取得
		env.notify = 1<<SCE_DMA_VIF1;				// VIFチャネルのみ通知
		sceDmaPutEnv(&env);							// DMA共通環境を設定
		((volatile tD_CHCR *)D1_CHCR)->TTE = 1;		// VIF1に対してDMAタグも送出
	}

	sceGsResetPath();

	/* コントローラ、初期化 */
	scePadInit(0);
	scePadPortOpen(0, 0, pad_dma_buf);


	/* Libccc初期化 -------------------------------------------------*/
	{
		int fd;

		for(;;){
			fd = sceOpen( JIS2UCS, SCE_RDONLY ); // jis->ucs 変換テーブルロード
			if(0 > fd){
				printf("sceOpen(%s, SCE_RDONLY) failed\n", JIS2UCS);
				continue;
			}
			sceRead(fd,  JisUcs2Tbl, sizeof( JisUcs2Tbl ));
			sceClose(fd);
			break;
		}

		for(;;){
			fd = sceOpen( UCS2JIS, SCE_RDONLY ); // ucs->jis 変換テーブルロード
			if(0 > fd){
				printf("sceOpen(%s, SCE_RDONLY) failed\n", UCS2JIS);
				continue;
			}
			sceRead(fd,  Ucs2JisTbl, sizeof(Ucs2JisTbl) );
			sceClose(fd);
			break;
		}
	}
	sceCccSetTable( JisUcs2Tbl, Ucs2JisTbl );
	/*----------------------------------------------------------------*/


	/* フォントデータ読みこみ＆LibPFont 初期化 -----------------------*/
	{
		int fd;

		for(;;){
			fd = sceOpen( PFONT_DAT_PATH, SCE_RDONLY );
			if(0 > fd){
				printf("sceOpen(FONTPATH, SCE_RDONLY) failed\n");
				continue;
			}
			sceRead(fd, pfont_data, FONT_DATASIZE);
			sceClose(fd);
			break;
		}

		Font_Init();                                    // フォント初期化
		Font_Load( CLUT_GSMEM, FONT_GSMEM, pfont_data ); // フォントセットアップ
	}
	/*---------------------------------------------------------------*/

	if( hddDeviceFlag )
	{
		int res = sceDevctl("hdd0:", HDIOC_STATUS, NULL, 0, NULL, 0);

		switch( res ){
		case 0: printf("HDD ok.\n");                  break; // 正常終了。
		case 1: printf("HDD driver unformat.\n");     break; // HDDドライブがアンフォーマットだった(→UDで初期化してね)。
		case 2: printf("HDD reserved.\n");            break; // （予約)
		case 3: printf("HDD not connected.\n");       break; // HDDドライブが接続されていなかった。
		case -19: printf(" 'hdd0:' device error\n");  break; // 指定されたデバイスが存在しない。
		case -24: printf(" cannnot open.\n");         break; // オープンできるディスクリプタの最大数に達している。
		}

		if( res != 3 )	PowerOff_Device=1;	// 電源ＯＦＦ時に、HDDの電源を切る必要あり。
		if( res == 0 ){
			// __sysconfパーティションをマウントする
			res = sceMount( SYSCONF_DEVICE, SYSCONF_PARTITION, 0, NULL, 0);
			if(res<0) {
				printf("mount error: %d\n", res);
			}
			{// HDDを アイドルさせない設定に。
				u_char standbytimer = 0x00;
				sceDevctl("hdd0:", HDIOC_IDLE, &standbytimer, sizeof(char), NULL, 0);
			}
		}
	}

	ChangeThreadPriority(GetThreadId(), 4);	//電源制御スレッドのスタンバイまで、プライオリティを一時下げる。
	CreatePowerOffThread( 3 ); // 電源遮断スレッド動作中も、描画更新を行うため、
	                           // メインスレッドより、低いプライオリティに設定。
	                           // 電源遮断スレッド動作中は、ＣＤ、ＨＤＤアクセスしないようにします。
	ChangeThreadPriority(GetThreadId(), 1);

	// ダブルバッファの設定
	sceGsSetDefDBuff(&g_db, SCE_GS_PSMCT32, 640, 224,
							SCE_GS_ZGEQUAL, SCE_GS_PSMZ24, SCE_GS_CLEAR);

	// ダブルバッファのクリアカラーを設定
	*(u_long *)(void *)&g_db.clear0.rgbaq = SCE_GS_SET_RGBAQ(0x00, 0x20, 0x40, 0x00, 0);
	*(u_long *)(void *)&g_db.clear1.rgbaq = SCE_GS_SET_RGBAQ(0x00, 0x20, 0x40, 0x00, 0);


    // VIF1パケットバッファを確保
	// VIF1パケットの初期化
	for(i=0; i<2; i++) {
		u_long128 *p;
		p = (u_long128 *)memalign(128, 128*20); // プリミティブバッファを確保
		sceVif1PkInit(&PkVif1[i], p);           // sceVif1Packet構造体を初期化
		sceVif1PkReset(&PkVif1[i]);             // sceVif1Packet構造体をリセット
		sceVif1PkEnd(&PkVif1[i], 0);            // 初期プリミティブとしてDMAendタグを追加
		sceVif1PkTerminate(&PkVif1[i]);         // DMAendタグを終端
	}


	/*---------------------------------------------------------------*/
	{
		SKB_ARG skbArg;
		int kbType,repWait,repSpeed,region;

		#ifdef SAMPLE_REGION_J
		skbArg.region = REGION_J; // 日本版PS2
		#else
			#ifdef SAMPLE_REGION_E
		skbArg.region = REGION_E; // 海外PS2(未対応)
			#else
		skbArg.region = REGION_A; // 海外PS2
			#endif
		#endif
		skbArg.AtokPath.system = ATOK_DEVICE;
		skbArg.AtokPath.sysDic = ATOK_SYSDIC_PATH;
		skbArg.AtokPath.usrDic = ATOK_USERDIC_PATH;

		skbArg.Gs_workAddress256 = SKB_GSMEM;
		skbArg.memAlign = memalign;
		skbArg.memFree  = free;

		// ソフトウェアキーボードの環境初期化。システム環境ファイルのあるデバイスと
		//                                ファイルが無かった場合のデフォルトが引数。
		Init_SKBConfig( &skbArg, SYSTEM_INI_DEVICE, 1, KEYREPW_MIDDLE, KEYREPS_MIDDLE );
		// システム環境の取得。
		SKB_GetKbdConfig( &kbType, &repWait, &repSpeed, &region );
		
		// USBキーボードライブラリの初期化
		Kbd_Init( kbType, repWait, repSpeed, region );
		Kbd_SetConfig();    // キーボード設定
		UsbKB_InitFlag = 1; // UsbKB周りの初期化を済ませたことを、記録しておく。
	}

	// ソフトウェアキーボード初期化
	GetWaitPowerOff = SKB_Init();  // 返り値は、ＨＤＤアクセス中確保されるセマフォＩＤを返す、関数。
	                               // 電源コントロール処理で、使用。
	/* ソフトウェアキーボードからのメッセージを受け取る関数を設定しておく */
	SKB_SetCallback( MessageFromSKB ); /* ＳＥリクエストや、編集結果の受け取り用 */
	skbStep = STEP_SKB_INIT;

	strcpy( TextBufSjis, "1234");

	/*---------------------------------------------------------------*/
	InterlaceField = sceGsSyncV(0)^1;

	/* v-blank 開始を待つセマフォを設定 */
	{
		struct SemaParam sp;

		sp.initCount = 0;
		sp.maxCount = 1;
		VSync_SemaID = CreateSema(&sp);
	}
	/* v-blank 開始の割り込みハンドラ登録 */
	VSync_HandleID = AddIntcHandler(INTC_VBLANK_S, VBlankStartHandler, 0 );
	/* v-blank 割り込み許可 */
	EnableIntc(INTC_VBLANK_S);

	/*---------------------------------------------------------------*/

	while(1){
		sceVif1Packet* pkVif1;
		u_long128*     pkSkb;
		int padOn, kbStat;

		/* 入力情報取得 */
		ulPad = PadGetKey(0);           // パッド情報取得

		padOn = ((ulPad>>32)&0xFFFFU);  // ボタン On トリガ
		if( UsbKB_InitFlag ) kbStat = Kbd_Main(&UsbKbData); // キーボード情報取得
		else                 kbStat = -1;

		pkSkb = (u_long128*)0;
		/* パワーＯＦＦ処理を、メインスレッドより、プライオリティの低いスレッドにしているので */
		/* パワーＯＦＦスレッド動作中に、デバイスアクセスなどを起こさせないようにする         */
		if(! PowerOff_Start){
			switch(skbStep){
			case STEP_SKB_CLOSE:
			        SKB_Close(0);
			        skbStep = STEP_SKB_EXIT;	//Exitまでやる
			        //skbStep = STEP_SKB_INIT;	//Closeまで
			        break;
			case STEP_SKB_EXIT:
			        SKB_Exit();
			        GetWaitPowerOff = ( int(*)(void) )0;
			        skbStep = STEP_SKB_NONE;
			        break;
			case STEP_SKB_NONE :
			case STEP_SKB_INIT :
			        if( padOn & SCE_PADRright ){
			        	#define MAX_INHIBIT_UCS4        (100)       // 入力許可禁止文字情報最大数
			        	u_int inhibitWorkBuf[MAX_INHIBIT_UCS4][10]; // 入力禁止文字用バッファ
			        	u_int inhibitMaskBuf[2][10];                // 入力禁止文字用バッファ
			        	KEYBOARD_SET* kbSet;
		
			        	if( skbStep == STEP_SKB_NONE ){
			        		printf("SKB_Init\n");
			        		GetWaitPowerOff = SKB_Init();  // 返り値は、ＨＤＤアクセス中確保されるセマフォＩＤを返す、関数。
			        		                               // 電源コントロール処理で、使用。
			        		SKB_SetCallback( MessageFromSKB ); /* ＳＥリクエストや、編集結果の受け取り用 */
			        		skbStep = STEP_SKB_INIT;
			        	}

			        	inhibitMaskBuf[0][0]=0x309b;	inhibitMaskBuf[0][1]=0x0000;	//濁点
			        	inhibitMaskBuf[1][0]=0x309c;	inhibitMaskBuf[1][1]=0x0000;	//半濁点
			        	inhibitWorkBuf[0][0]=0x309b;	inhibitWorkBuf[0][1]=0x0000;	//濁点
			        	inhibitWorkBuf[1][0]=0x309c;	inhibitWorkBuf[1][1]=0x0000;	//半濁点

			        	SKB_SetInhibit_MaskChar( inhibitMaskBuf, 2 ); //入力禁止でも、ボタンは押せる文字。
			        	SKB_SetInhibit_Disable( inhibitWorkBuf , 2 ); //入力、禁止文字なし。
			        	SjisToUcs4( TextBufSjis, TextBufUcs, 1024 );

			        	kbSet = &KeyBoardSet_All;
			        	      // キーボードセット、編集動作モード、編集初期テキストを指定、0最大入力文字数
			        	SKB_Open( kbSet, 0, TextBufUcs, 0 ); 
			        	skbStep = STEP_SKB_OPENED;
			        }
			        break;
			case STEP_SKB_OPENED:
			        if( SKB_GetStatus() ){
			        	SKB_SetUsbKeyborad( kbStat, &UsbKbData, KbdGetNumLockState());
			        	SKB_Control( ulPad );      // ソフトウェアキーボード側に入力情報を渡して制御する。
			        	pkSkb = SKB_MakeVif1CallPacket( 0, 0 );
			        }
			        break;
			}
		}

		/* 描画パケット作成 */
		pkVif1 = &PkVif1[db_id];
		sceVif1PkReset( pkVif1 );
		if( pkSkb ){
			sceVif1PkCall( pkVif1, pkSkb, 0 );
		}
		sceVif1PkEnd(pkVif1, 0);
		sceVif1PkTerminate( pkVif1 );
		/* VSync待ち、ダブルバッファ切り替え */
		FlushCache(WRITEBACK_DCACHE);			// Dキャッシュの掃き出し

		// toVIF1チャネルを起動して、作成ずみのパケットを描画
		sceGsSyncPath(0, 0);          // データ転送終了まで待機

		// Atokライブラリは、ＨＤＤアクセスをともなうので、スリープ時間が長い。
		// そのため、サンプルでは、ロープライオリティスレッドで動作させている。
		// 次のＶまで、メイン処理は、WaitSemaして、ＡＴＯＫに処理を譲る。
		WaitSema( VSync_SemaID );

		if( db_id ) {
			sceGsSetHalfOffset(&g_db.draw1, 2048, 2048, InterlaceField );
		} else {
			sceGsSetHalfOffset(&g_db.draw0, 2048, 2048, InterlaceField );
		}
		sceGsSwapDBuff( &g_db, db_id );  // ダブルバッファ切り替え
		sceDmaSend( sceDmaGetChan(SCE_DMA_VIF1), pkVif1->pBase );
		db_id^=1;
	}
	/*---------------------------------------------------------------*/
	sceGsSyncPath(0, 0);


	/* v-blank 開始の割り込みの禁止 */
	DisableIntc( INTC_VBLANK_S );
	/* v-blank 開始の割り込みハンドラ登録抹消 */
	RemoveIntcHandler(INTC_VBLANK_S,VSync_HandleID);
	/* v-blank 開始を待つセマフォを削除 */
	DeleteSema( VSync_SemaID );

	/* コントローラ、終了処理 */
	scePadPortClose( 0, 0 );
	scePadEnd();

	/* 電源コントロールの解除 */
	sceCdPOffCallback( 0, (void *)0);
	TerminateThread( PowerOff_ThreadID );
	DeleteThread( PowerOff_ThreadID );
	DeleteSema( PowerOff_SemaID );

	sceCdInit( SCECdEXIT );

	sceSifExitCmd();

	for(;;);

	return 0;
}

