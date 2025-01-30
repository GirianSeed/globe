/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
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

#ifndef __LIBFEP_H__
#define __LIBFEP_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#include <libusbkb.h>

#include "mtfifo.h"
#include <shareddef.h>

#define	IS_SJIS_KANJI(_c)		((((_c)>=0x81) && ((_c)<=0x9F)) || (((_c)>=0xE0) && ((_c)<=0xFC)))

#define HENKAN_SIZE			(256)		// 変換中の文字列最大値
#define KAKUTEI_SIZE		(256)		// 確定文字列    最大値
#define CANDLIST_SIZE		(1024)		// 候補一覧文字列最大値

#define CONV_ON				(1)			// 変換中
#define CONV_OFF			(0)			// 変換中でない


#define FEP_EDIT_DELETE		(0)			// 文字削除
#define FEP_EDIT_ADD		(1)			// 文字追加
#define FEP_EDIT_INSERT		(2)			// 文字挿入

#define FEP_EDIT_NG			(0)			// 文字追加NG
#define FEP_EDIT_OK			(1)			// 文字追加OK
#define FEP_EDIT_OVER		(2)			// 指定したバイト数をオーバーした


// 入力モード
typedef enum
{
	// FEP ON
	FEPON_HIRA = 0,			// ひらがな
	FEPON_HAN_KATAKANA,		// カタカナ(半角)
	FEPON_ZEN_KATAKANA,		// カタカナ(全角)
	FEPON_HAN_ALPHABET,		// 無変換  (半角)
	FEPON_ZEN_ALPHABET,		// 無変換  (全角)

	// FEP OFF
	FEPOFF,

	#if 1//SKB_EX FixMode
	FEPON_HIRA_FIX        ,			// ひらがな
	FEPON_HAN_KATAKANA_FIX,		// カタカナ(半角)
	FEPON_ZEN_KATAKANA_FIX,		// カタカナ(全角)
	FEPON_HAN_ALPHABET_FIX,		// 無変換  (半角)
	FEPON_ZEN_ALPHABET_FIX,		// 無変換  (全角)
	#endif

	FEPMODE_MAX,

} fepAtokInputMode_m;


// キーバインドテーブルの定義に使われる値の定義
enum {
	RAW_CODE          = 0x4000, // 出力キューに生コードを返すときに使うフラグ

	EX_ATOK_EDIT_NULL = 0x8000, // 拡張用コードを、0x8000以上として定義
	                            // EX_ATOK_で始まるコマンドは、直接sceAtokEditConv()に渡されるコマンドではなく
	                            // いくつかのプリミティブを組み合わせた複合コマンドです。複合コマンドの番号の値
	                            // の範囲を0x8000以上にすることで、bit 15を使って区別できるようにしています。

	// sceAtokEditConv()関数1回の呼び出しで実現できない機能は、
	// 適当な名前をつけて適宜処理を実装します。
	EX_ATOK_EDIT_KANJIOFF,      // 漢字入力モードOFF
	EX_ATOK_EDIT_KANALOCK,      // カナロック切り替え

	// 入力文字の切り替え
	EX_ATOK_DSPHIRAGANA,        // ひらがな入力
	EX_ATOK_DSPZENKANA,         // 全角カタカナ入力
	EX_ATOK_DSPZENRAW,          // 全角無変換入力
	EX_ATOK_DSPHANKANA,         // 半角カタカナ入力
	EX_ATOK_DSPHANRAW,          // 半角無変換入力

	EX_ATOK_DSPSWEISU,          // 英数切り替え
	EX_ATOK_DSPSWKANA,          // かな切り替え
	                            // NOTICE: 半角カナになる可能性あり

	// 固定入力モードの設定
	EX_ATOK_FIXOFF,             // 固定入力モードOFF
	EX_ATOK_FIXHIRAGANA,        // 固定入力ひらがな
	EX_ATOK_FIXZENKANA,         // 固定入力全角カタカナ
	EX_ATOK_FIXZENRAW,          // 固定入力全角無変換
	EX_ATOK_FIXHANKANA,         // 固定入力半角カタカナ
	EX_ATOK_FIXHANRAW,          // 固定入力半角無変換

	// 
	EX_ATOK_FIX_ZENHIRA_HANRAW,          // 固定入力ひらがな<-->固定入力半角無変換
	EX_ATOK_FIX_ZENHIRA_ZENRAW,          // 固定入力ひらがな<-->固定入力全角無変換
	EX_ATOK_FIX_ZENHIRA_ZENKANA_HANKANA, // 固定入力ひらがな-->固定入力全角カタカナ-->固定入力半角カタカナ

	EX_ATOK_CHANGE_DSPKANA,     // ひらがな-->全角カタカナ-->半角カタカナ
	EX_ATOK_CHANGE_DSPRAW,      // 全角無変換-->半角無変換

	// 候補リストの選択
	EX_ATOK_EDIT_CANDNEXT,      // 次候補
	EX_ATOK_EDIT_CANDPREV,      // 前候補
	EX_ATOK_EDIT_CANDHEAD,      // 候補先頭
	EX_ATOK_EDIT_CANDTAIL,      // 候補末尾
	EX_ATOK_EDIT_CANDGRPNEXT,   // 次候補群
	EX_ATOK_EDIT_CANDGRPPREV,   // 前候補群

	EX_ATOK_EDIT_CLSEND_CAND,   // 最後候補
	EX_ATOK_EDIT_CLSTOP_CAND,   // 先頭候補

	// 文節単位のカーソル移動
	EX_ATOK_EDIT_REVCLSLEFT,    // 文節左循環
	EX_ATOK_EDIT_REVCLSRIGHT,   // 文節右循環
	EX_ATOK_EDIT_CLSLEFT,       // 文節左
	EX_ATOK_EDIT_CLSRIGHT,      // 文節右
	EX_ATOK_EDIT_CLSTOP,        // 文節先頭
	EX_ATOK_EDIT_CLSEND,        // 文節末尾
//	EX_ATOK_EDIT_FOCUSLEFTEX,   // フォーカス左
//	EX_ATOK_EDIT_FOCUSRIGHTEX,  // フォーカス右

	EX_ATOK_EDIT_POSTRAWHALF,   // 半角英数への復号後変換

	EX_ATOK_EDIT_KAKUTEISPACE,    // 確定後スペース挿入
	EX_ATOK_EDIT_KAKUTEIHANSPACE, // 確定後半角スペース挿入
	EX_ATOK_EDIT_CURSTAY,         // カーソル移動無し（ダミー）

	EX_ATOK_EDIT_CAND1,         // 候補一覧表示のとき、[0]～[9]のキーを使って
	EX_ATOK_EDIT_CAND2,         // ダイレクトに候補を選択した場合の動作定義
	EX_ATOK_EDIT_CAND3,         // 
	EX_ATOK_EDIT_CAND4,         // 
	EX_ATOK_EDIT_CAND5,         // 
	EX_ATOK_EDIT_CAND6,         // 
	EX_ATOK_EDIT_CAND7,         // 
	EX_ATOK_EDIT_CAND8,         // 
	EX_ATOK_EDIT_CAND9,         // 
	EX_ATOK_EDIT_CAND0,         // 

};



// シフト状態を指定するためにキーコードに付加するフラグの値です。
#define SHIFT (0x10000000U)
#define CTRL  (0x20000000U)
#define ALT   (0x40000000U)

// ひとつひとつのキーストロークに対する動作を定義するテーブルの構成要素です。
typedef struct {
	u_long  code;    // キーコード
	u_short func[6]; //  0: SCE_ATOK_ISTATE_BEFOREINPUT   未入力
	                 //  1: SCE_ATOK_ISTATE_BEFORECONVERT 変換前
	                 //  2: SCE_ATOK_ISTATE_CONVERTING    変換中
	                 //     SCE_ATOK_ISTATE_CANDEMPTY     候補切れ(候補なし)
	                 //  3: SCE_ATOK_ISTATE_POSTCONVKANA  候補一覧
	                 //  4: SCE_ATOK_ISTATE_MOVECLAUSEGAP 文節区切り直し
	                 //  5: SCE_ATOK_ISTATE_POSTCONVKANA  後変換(カタカナ)
	                 //     SCE_ATOK_ISTATE_POSTCONVHALF  後変換(半角)
	                 //     SCE_ATOK_ISTATE_POSTCONVRAW   後変換(無変換)
} FEP_KEYBIND;


// 変換された文字列
typedef struct {
	u_int   Char;     // 変換ずみ文字コード UCS4(UCS2)
	u_char  Flag;     // フラグ
	                  //  bit 0: 
	                  //  bit 1: 
	                  //  bit 2: 
	                  //  bit 3: 
	                  //  bit 4: 
	                  //  bit 5: 
	                  //  bit 6: 
	                  //  bit 7: 
	u_short RawCode;  // ロウUSBキーコード
} COOKED_CHAR;


// FEP
typedef struct
{
	u_char ucUseAtok;  // ATOK
	                   //  0: 使わない
	                   //  1: 使う

	char   *pAtokErx;  // ERX用バッファ

	int iInsertMode;   // 挿入モード
	                   //  0: INSERT
	                   //  1: ADD

	int nCaretPos;     // キャレットの存在するバイト位置
	int nTextLen;      // 入力済み文字列のバイト数

//	int szText[2048];       // 確定文字列
	int szInsertText[2048]; // 挿入文字列

	int mode;               // sceAtokGetKanjiState()の値
	                        // ATOKのON/OFF
	int state;              // sceAtokGetInputState()の値
	                        // ATOKエンジンの文字入力状態
	int stateCandLeftEnd;   // sceAtokGetInputState()の値
	                        // ATOKエンジンの文字入力状態
	                        // 先頭(末尾)文節にジャンプ＆候補一覧
	int led;          // 非同期ATOKレイヤが管理しているかなロック状態
	int KanDspType;   // ATOKエンジンに設定した入力文字種

	int Keybind;      // キーバインドの設定
	                  //  0: ATOK風
	                  //  1: IME風
	                  //  2: VJE風

	int InitialInput; // 初期入力モード
	                  //  0: ローマ字入力
	                  //  1: かな入力

	int SpaceInput;   // スペース入力モード
	                  //  0: 常に半角
	                  //  1: 常に全角
	                  //  2: 入力モードによる

	int TenkeyInput;  // テンキー入力モード
	                  //  0: 常に半角
	                  //  1: 常に全角
	                  //  2: 入力モードによる

	int Font1, Font2; // フォント設定
	FIFO qKeyInput;            // キー入力キュー
	u_int auiKinQueueBuf[512]; // キー入力キューのバッファ

	FIFO qFepOuput;            // ATOK出力キュー
	u_int qOutBuf[512];        // ATOK出力キューのバッファ

	int usePadChangeMode; // パッドが使われてモードが変わったかどうかのフラグ


	int FocusTop;      // カーソルがあるバイト位置
	int FocusCls;      // フォーカスがある文節の番号
	int nClusters;     // 変換中文字列の文節数

	int nCandidates;   // 変換候補数
	int nCandOffset;   // 現在の候補一覧の表示開始オフセット
	int nCurrentCand;  // 現在の変換候補カーソル位置

	char szConv[HENKAN_SIZE];           // 変換中文字列バッファ
	char CandBuf[CANDLIST_SIZE];        // 候補リストバッファ
	char aszClusters[110][HENKAN_SIZE]; // 変換時の分節バッファ

	int  cntF10;      // F10用状態カウンタ
	int  cntFix;      // 無変換
	int  cntFixShift; // 無変換+SHIFT
	int  cntFixCtrl;  // 無変換+CTRL

	int  nIsConv;     // 変換中か
	                  //  0: 変換中でない
	                  //  1: 変換中
} fep_t;

/*
typedef struct
{
	int		iRegion;				// リージョン
	int		iKeyBindType;			// キーバインドタイプ
	int		iKeybdType;				// キーボードタイプ
	u_int	*puiUCS4;				// 文字列へのポインタ
	u_int	uiMaxTextLen;			// 最大文字数

	// アプリ操作
	void (*atokON)(void);						// ATOK ON
	void (*atokOFF)(sceAtokContext *pAtok);		// ATOK OFF
	void (*ok)(void);							// ok
	void (*cancel)(void);						// キャンセル
	void (*changeMode)(int mode);				// 入力モード変更
	int  (*changeText)(int changeType, const u_int *szChar, u_short ctrlCode);	// 文字チェック
	u_char (*showHideSoftKB)(void);				// ソフトウェアキーボード
												// 0: 非表示
												// 1: 表示
	void (*closeSoftKB)(void);					// ソフトウェアキーボード閉じる
	void (*openSoftKB)(void);					// ソフトウェアキーボード開く

	// システム
	void (*dprintf)(const char *msg, ...);

	// 音
	void (*se)(int soundNo);

	// メモリ管理
	void* (*malign)(size_t align, size_t size);		// = memalign
	void  (*mfree)(void *p);						// = free

	// 電源管理
	void (*powerLock)(void);
	void (*powerUnlock)(void);

	int (*getStrWidthSJIS)(const char *sjis, float fontratio);		// 戻り値 : 16倍された値

	// 描画関連
	int  (*readTex)(textureInfo_t *pTex, const char *filename);		// テクスチャ読み込み
	void (*deleteTex)(textureInfo_t *pTex);							// テクスチャ破棄
	void (*loadTex)(sceVif1Packet *pk, textureInfo_t *pTex);		// テクスチャロード
	void (*drawTex)(sceVif1Packet *pk, u_int col, sprite_t *pSp, uv_t *pUv, textureInfo_t *pTex);	// テクスチャ描画

} fepImport_t;
*/




int  Fep_Init(gImport_t *pImport,
			 int iUseAtok, int iPriority,
			 const char *pszSysDic, const char *pszUserDic, const char *pszSysconfDir, const char *pszConfig);
int  Fep_Exit(void);
int  Fep_Run(void);
void Fep_PostSoftKey(const u_int *pUcs4, u_char Flag, u_short ctrlCode);
void Fep_PostKeys(USBKBDATA_t *pData);

void Fep_GetConvertChars(char *sjis);
int Fep_GetClusterNum(void);
int Fep_GetNowCluster(void);
char *Fep_GetClusterChars(int index);


u_int Fep_GetCandListMax(void);
u_int Fep_GetCandListOffset(void);
u_int Fep_GetCurrentCand(void);
u_int Fep_GetCandListMaxWidth(float mainChars, float subChars);
char *Fep_GetCandStart(void);
char *Fep_GetCandListItem(char *pStartPos, char *pItem, char *pSubItem);
const int Fep_CandListNum(void);
void Fep_SetText(const u_int *pUCS4);

void Fep_SetKakuteiPart(void);
void Fep_SetKakuteiAll(void);
int Fep_GetNowAtokInput(void);
int Fep_GetNowConv(void);
void Fep_SetKanjiCancel(void);
void Fep_ConvStrings(char *str_sjis);


u_int Fep_ShowCandList(void);
u_int Fep_Changing(void);
int  Fep_AtokGetInputState(void);
int  Fep_GetCaretPos(void);
int  Fep_IsIdle(void);
const u_int *Fep_GetInsertChars(void);
int  Fep_SetInputMode(fepAtokInputMode_m mode);
int  Fep_Cmd(sceAtokContext *pAtok, u_int f);
void Fep_SetData2Fifo(void);


#if 1//SKB_EX Insert 
void Fep_SetInsertMode(int Mode);  //  文字の挿入/上書モードを設定する
int  Fep_GetInsertMode(void);      //  文字の挿入/上書モードを獲得する
#endif

#if 1//SKB_EX Power
int  Fep_GetAtokSemaID(void);      // Atok動作資源のセマフォＩＤ取得
#endif

#if 1//SKB_EX_Sound 
void Fep_se_play(int sound_No);
#endif

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// !__LIBFEP_H__
