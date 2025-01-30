/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.5
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
#ifndef __SKB_H__
#define __SKB_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

// フォント文字色  tSkb_Ctl構造体メンバ  gaucFontCol[]配列要素
enum
{
	FONT_COL_NORMAL,        // 通常時
	FONT_COL_CANDLIST_SUB,  // 候補一覧の漢字以外の文字
	FONT_COL_UNDERLINE,     // アンダーライン色設定
	FONT_COL_FOCUS,         // カーソルがある時の色
	FONT_COL_LIST,          // リストの文字色

	FONT_COL_MAX,
};

// フォント背景色  tSkb_Ctl構造体メンバ  aucFontBGCol[]配列要素
enum
{
	FONT_BGCOL_NORMAL,       // aucFontBGCol[0] 通常時
	FONT_BGCOL_BEFORECHANGE, // aucFontBGCol[1] 変換前
	FONT_BGCOL_CHANGING,     // aucFontBGCol[2] 変換中
	FONT_BGCOL_CANDLIST,     // aucFontBGCol[3] 候補一覧中注目

	FONT_BGCOL_MAX,
};

// ソフトウェアキーボード tSkb_Ctl構造体メンバ KBD[] 配列要素
enum
{
	SKBTYPE_HIRA = 0,  // ひらがな
	SKBTYPE_KANA,      // かたかな
	SKBTYPE_ABC,       // ABC配列
	SKBTYPE_QWERTY,    // QWERTY配列
	SKBTYPE_NUM,       // 数字
	SKBTYPE_SIGN,      // 記号
	SKBTYPE_MAX,
};


#define MAX_CHAR_COUNT       (255)  // 最大入力文字数
#define MAX_CHAR_BUFF_COUNT  (MAX_CHAR_COUNT + 1)	// 最大入力文字数(Buffer用)

typedef  struct
{
	// 文字列バッファ
	u_int uiChars[MAX_CHAR_BUFF_COUNT];
	u_int uiCharNum;

	u_int iCoverView;  // テキストボックスの表示マスク
	                   // 0: 表示  1: マスク
	// フォント
	int fontSizeW, fontSizeH;

	// フォント色
	u_char aucFontCol[FONT_COL_MAX][4];        // 文字色
	u_char aucFontBGCol[FONT_BGCOL_MAX][16];   // 色

	// ソフトウェアキーボード
	skey_t KBD[SKBTYPE_MAX]; // キーボード構造体
	u_int  uiNowKeyType;     // KBDのインデックス(SKBTYPE_xx)
	fepAtokInputMode_m inputMode[SKBTYPE_MAX]; // 入力モード(FEP用)
	u_int          auiNextKeyTop[SKBTYPE_MAX]; // uiNowKeyTypeをインデックスとして
	                                           // 次のキーボードのインデックスを得る為の配列
	iPoint_t              skbPos[SKBTYPE_MAX]; // 位置


	int               modeBarNum[SKBTYPE_MAX]; // モードバークリエイト番号格納
	int fepModeOnState;        // Fep On/Off 時のFepモード記憶変数
	struct {
		int alphabetKeyType;   // 0 ABC順 1 ＰＣキーボード配列 system.ini 準拠
		int inputDevice;       // 0 USBKB 1 PAD
	}Env;

	int iControlFlag;   // 入力情報の占有を要求するか  0:CLose完 1:Open中～
	int iOpenFlag;      // オープンしているか          0:未Open  1:Open完了～
	int iUsbKeyFlag;    // USBKEY使用中か              0:使用していない 1:使用中
	int (*Message)( int type, void* text );	// 呼び出し側へ情報をＵＰ

} tSkb_Ctl;

// キーボードセットの定義
typedef struct{
	int startKeyboardType;          // 最初のキーボードのインデックス(SKBTYPE)
	struct{
		int next;                   // 次のキーボードのインデックス(SKBTYPE)
		fepAtokInputMode_m fepMode; // 初期フェプモード、 FEP_OFF以外はその後自動設定される
		int ucShift;                // 初期 全角半角 キャプス
		int* disableCtrl;           // コントロールキーの有効非有効情報
		iPoint_t Position;          // KBD      画面位置
	}Keyboard[SKBTYPE_MAX];
	struct{
		int modeCount;             // Fepの入力モード数
		u_int* Mode;               // Fepの入力モード配列アドレス
		iPoint_t Position;         // FepBar   画面位置
	}Fep;
	struct{
		int setCount;               // ModeBar クリエイト数格納
		u_int SetMode[SKBTYPE_MAX]; // Keyboard順にモードバーの表示グラフィック情報格納、
		u_int ModeNum[SKBTYPE_MAX]; // SKBTYE別にKeyboard順番号を格納
		iPoint_t Position;          // ModeBar   画面位置
	}Bar;
	struct{
		iRect_t  Rect;             // TextFiels 画面位置
	}Text;
}KEYBOARD_SET;


void SKB_SetCallback( int(*func)(int skbmes,void* info) );
enum {
	SKBMES_RESULT    , // テキスト編集結果
	SKBMES_USBKEY_ON , // USBキーボード ON  報告
	SKBMES_USBKEY_OFF, // USBキーボード OFF 報告
	SKBMES_HELP_SHOW , // help画面 表示 ON  報告
	SKBMES_HELP_HIDE , // help画面 表示 OFF 報告
	SKBMES_FEP_ON ,    // 漢字変換 ON  報告
	SKBMES_FEP_OFF,    // 漢字変換 OFF 報告
	SKBMES_SE     ,    // SoundEffect 種別 報告
};

typedef struct{
	int region;          // 日本版
	struct{
		char*	system;
		char*	config;
		char*	sysDic;
		char*	usrDic;
	}AtokPath;
	u_int Gs_workAddress256;
	void*(*memAlign)(size_t, size_t);
	void (*memFree)(void *p);
}SKB_ARG;

extern int Init_SKBConfig(SKB_ARG*arg, char *fsName, short keyType, short repWait, short repSpeed );

extern int  SKB_Init(void);
extern void SKB_Exit(void);

void SKB_GetKbdConfig( int* kbType, int *repeatWait, int *repeatSpeed,int *region );

int  SKB_GetStatus();
int  SKB_GetZenkakuMode();
void SKB_Control( u_long ulPad );
void SKB_SetUsbKeyborad( int kbStat, USBKBDATA_t* ,int numLock );

void SKB_Open( KEYBOARD_SET* kbSet, int editType, u_int* usc4 );
void*SKB_MakeVif1CallPacket( int clipOffX, int clipOffY );
void SKB_Close( char* );

void SKB_SetInhibit_MaskChar( u_int (*ucs4)[10], int count );
void SKB_SetInhibit_Disable( u_int (*ucs4)[10], int count );
void SKB_SetInhibit_Enable( u_int (*ucs4)[10], int count );
int  SKB_IsDisableUcs4( u_int ucs4 );

void SKB_GetLastKey(void);

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif  //!__SKB_H__
