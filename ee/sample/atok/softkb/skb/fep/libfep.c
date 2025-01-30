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

#include <sys/types.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <eekernel.h>
#include <sifdev.h>
#include <libusbkb.h>
#include <libccc.h>

#include "mtfifo.h"
#include "libfep.h"

#define FEP_ASSERT(_a_)			assert((int)_a_);
#define FEP_STACK_SIZE			(16384*4)

#define INPUTTYPE_KEYBOARD		(0)
#define INPUTTYPE_SOFTKB		(1)
#define INPUTTYPE_APP			(2)



enum
{
	APP_INTERRUPT_KEY,
	APP_KAKUTEIPART,
	APP_KAKUTEIALL,
	APP_CONVCANCEL,
	APP_DELETEALL,

	APP_MAX
};


gImport_t		*g_pFepImport;
fep_t			g_Fep;


extern void FepTreatKey(sceAtokContext *pAtok, u_char led, u_char mkey, u_short rawcode);
extern void FepTreatSkb(sceAtokContext *pAtok, u_short ctrlCode, u_char flag, const u_int *pCharCode);



static sceAtokContext g_Atok;					// ATOKエンジンのためのコンテキスト
static int  g_AtokRsrcSema;						// ATOKエンジンへの再入を防ぐリソースセマフォ
static int  g_FepWorkerThread;					// ワーカスレッドのスレッドID
static u_char FepWokerStack[FEP_STACK_SIZE] __attribute__((aligned(16)));	// スタックエリアの定義

static void postKey(u_char led, u_char mkey, u_short keycode);
static void interruptFromKey(void);
static void FepTreatApp(sceAtokContext *pAtok, u_int cmd, u_int arg0, u_int arg1);
static void workerMain(int param __attribute__((unused)));
#ifndef ATOK_NO_LINK
static int  getContext(void);
#endif
static int  getChar(COOKED_CHAR *pCc, int fPeek);

static int  inputBackspace(int pos);
static void inputDelete(int pos);
static void inputAllClear(int *pos);
#ifndef ATOK_NO_LINK
static int FepSetConfig();
#endif

#if 1//SKB_EX Sound
void Fep_atok_sound(int f);
void Fep_se_play(int sound_No);
#endif

// 入力FIFOキューに投げられる要素の構造を示す共用体
typedef union {
	u_char type;

	struct
	{
		u_char type;
		u_char led;
		u_char mkey;
		u_int  keycode;
	} kb;

	struct
	{
		u_char type;
		u_short usCtrlCode;
		u_char  ucFlag;
		u_int const  *puiChar;
	} skb;

	struct {
		u_char  type;
		int cmd;
		int arg0;
		int arg1;
	} msg;

	u_char sz[8];						// サイズ調整
} INPUT_QUEUE_ELEM;





//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//  Fep_Init													ref:  libfep.h
//
//  FEP初期化
//
//  gImport_t *pImport        : インポート関数群
//  int iUseAtok              : ATOK使用フラグ(0:しない/1:する)
//  int iPriority             : ATOKワーカスレッドのプライオリティ
//  const char *pszSysDic     : システムディレクトリのパス へのポインタ
//  const char *pszUserDic    : ユーザー辞書のパス         へのポインタ
//  const char *pszSysconfDir : システムディレクトリパス   へのポインタ
int Fep_Init(gImport_t *pImport,
			 int iUseAtok, int iPriority,
			 const char *pszSysDic, const char *pszUserDic, const char *pszSysconfDir )
{
	int i;
	struct ThreadParam tp;

	memset(&g_Fep, 0x00, sizeof(g_Fep));	// ワークをゼロクリア
	g_pFepImport = pImport;					// アプリ側のコールバック設定

	// 個別の初期化
	g_Fep.iInsertMode = 1;		// 挿入モードで起動
	#if 1//SKB_EX Power
	g_AtokRsrcSema=-1;
	#endif

	// キー入力キューを作成
	i = FifoCreate(&g_Fep.qKeyInput, g_Fep.auiKinQueueBuf, sizeof(g_Fep.auiKinQueueBuf));
	if(i<0)
	{
		// FIFO生成に失敗
		return(i);
	}

	// ATOKエンジンの出力キューを作成
	i = FifoCreate(&g_Fep.qFepOuput, g_Fep.qOutBuf, sizeof(g_Fep.qOutBuf));
	if(i<0)
	{
		// FIFO生成に失敗
		FifoDelete(&g_Fep.qKeyInput);
		return(i);
	}

	// キューイングされたキー入力をATOKエンジンに渡すためのワーカスレッドを作る
	tp.entry          = (void(*)(void *))workerMain; // スレッドのエントリ関数
	tp.stack          = FepWokerStack;               // スタックエリアの先頭アドレス
	tp.stackSize      = sizeof(FepWokerStack);       // スタックエリアのサイズ
	tp.initPriority   = iPriority;                   // スレッドプライオリティ
	tp.gpReg          = &_gp;                        // GPレジスタ値
	g_FepWorkerThread = CreateThread(&tp);  // スレッドを生成
	if(g_FepWorkerThread<0)
	{
		// スレッド生成失敗エラー
		FifoDelete(&g_Fep.qKeyInput);
		FifoDelete(&g_Fep.qFepOuput);
		return(g_FepWorkerThread);
	}

	// FEP 初期化
	g_Fep.ucUseAtok = 0;
	g_Fep.mode     = 0;	// ATOK OFF
	g_Fep.state    = 0;	// 入力前

	i=0;
	#ifndef ATOK_NO_LINK
	if(iUseAtok == 1)
	{
		g_Fep.pAtokErx = g_pFepImport->malign(16, 512*1024);
		if(! g_Fep.pAtokErx )
		{
			g_pFepImport->dprintf("ERROR : Fep_Init - fail to get ERX buf\n");
			i=-100;
		}
		else{
			struct SemaParam sp;

			// ATOKエンジンのためのリソースセマフォを作成
			sp.maxCount    = 1;                 // セマフォ最大値
			sp.initCount   = 1;                 // セマフォ初期値
			sp.option      = 0;                 // セマフォ付加情報
			g_AtokRsrcSema = CreateSema(&sp);   // セマフォ生成
			if( g_AtokRsrcSema >= 0 ){
				// ATOKラッパライブラリ初期化   ATOK辞書ファイルあるディレクトリ名を指定します。
				i = sceAtokInit( &g_Atok, pszSysDic, pszUserDic, pszSysconfDir, g_Fep.pAtokErx );
				if( i < 0 ){
					DeleteSema(g_AtokRsrcSema);			// リソースセマフォの削除
					g_Fep.ucUseAtok = 0;
				}
				else{
					g_Fep.ucUseAtok = 1;

					// まず_sysconfパーティションの/atok/atok.iniに記録されている設定を読みこみます
					FepSetConfig();

					g_Fep.led        = g_pFepImport->usSystemIni[ENV_ATOK_MODE] ? USBKB_LED_KANA : 0;	// 初期入力モード設定
					g_Fep.KanDspType = 1;											// ひらがな入力
					sceAtokKanjiOn(&g_Atok);										// ATOKを漢字入力モードへ
				}
			}
			else i=-200;
		}
	}
	#endif

	// スレッドを起動
	StartThread(g_FepWorkerThread, 0);

	return(i);
}


//============================================================================
//  Fep_Exit													ref:  libfep.h
//
//  ワーカスレッドを安全に停止させる
//
//  戻り値 : 意味無い
int Fep_Exit(void)
{
	#ifndef ATOK_NO_LINK
	if(g_Fep.ucUseAtok)
	{
		struct SemaParam semaPara;

		if( ReferSemaStatus( g_AtokRsrcSema, &semaPara ) ){
			// Atok関係の処理が、始まっていたら、終了まで待つ。
			if( semaPara.currentCount == 0 ){
				#if 1	// Block関数。
				WaitSema(g_AtokRsrcSema);	// リソースセマフォの削除
				#else	// nonBlock関数。
				return -1;               
				#endif
			}
		}
	}
	#endif
	//強制スレッド削除。
	TerminateThread( g_FepWorkerThread );
	DeleteThread( g_FepWorkerThread );
	g_FepWorkerThread = -1;

	FifoDelete(&g_Fep.qKeyInput);		// 処理キューを削除
	FifoDelete(&g_Fep.qFepOuput);		// 処理キューを削除

	#ifndef ATOK_NO_LINK
	if(g_Fep.ucUseAtok)
	{
		// ATOKエンジンを停止(辞書ファイルがクローズされる)
		sceAtokExit(&g_Atok);
		DeleteSema(g_AtokRsrcSema);			// リソースセマフォの削除
		g_AtokRsrcSema = -1;
		if(g_Fep.pAtokErx)
		{
			g_pFepImport->mfree(g_Fep.pAtokErx);
			g_Fep.pAtokErx = 0;
		}
		g_Fep.ucUseAtok = 0;
	}
	#endif

	return 0;
}


//============================================================================
//  Fep_PostSoftKey												ref:  libfep.h
//
//  ソフトウェアキーボード入力をFIFOに渡す
//
//  const u_int *pUcs2 : 文字列へのポインタ(UCS2)
//  u_char Flag        : SHIFTが押されているかのフラグ
//  u_short ctrlCode   : コントロールコード
void Fep_PostSoftKey(const u_int *pUcs2, u_char Flag, u_short ctrlCode)
{
	INPUT_QUEUE_ELEM q;

	q.skb.type       = INPUTTYPE_SOFTKB;
	q.skb.usCtrlCode = ctrlCode;
	q.skb.ucFlag     = Flag;
	q.skb.puiChar    = pUcs2;
	FifoPostData(&g_Fep.qKeyInput, &q, sizeof(INPUT_QUEUE_ELEM));
}


//============================================================================
//  Fep_PostApp													ref:  libfep.h
//
//  メッセージをFIFOに渡す
//
//  int cmd  : コマンドタイプ
//  int arg0 : 引数0
//  int arg1 : 引数1
void Fep_PostApp(int cmd, int arg0, int arg1)
{
	INPUT_QUEUE_ELEM q;

	q.msg.type  = INPUTTYPE_APP;
	q.msg.cmd   = cmd;
	q.msg.arg0  = arg0;
	q.msg.arg1  = arg1;
	FifoPostData(&g_Fep.qKeyInput, &q, sizeof(INPUT_QUEUE_ELEM));
}


//============================================================================
//  Fep_PostKeys												ref:  libfep.h
//
//  キーボード入力をFIFOに渡す
//
//  USBKBDATA_t * pData : キーボードデータ格納構造体へのポインタ
void Fep_PostKeys(USBKBDATA_t * pData)
{
	int i;
	FEP_ASSERT(pData);

	if(pData == NULL)
	{
		return;
	}

	for(i=0; i<pData->len; i++)
	{

		if(g_pFepImport->fep.showHideSoftKB())
		{
			// ソフトウェアキーボードを非表示にする(今回のキー入力は無視する)
			Fep_PostApp(APP_INTERRUPT_KEY, 0, 0);
			return;
		}

		postKey(pData->led, pData->mkey, pData->keycode[i]);
	}
}




//============================================================================
//  Fep_Run													ref:  libfep.h
//
//  FEPメイン処理
//
//  戻り値 : int : 常に0
int Fep_Run(void)
{
	int i, j;
	COOKED_CHAR		c;
	int loopCnt = 20;	// 1フレームにloopCnt文字しか処理しない

	// FEPからの情報を引き取って、編集中の文字列に反映させる
	// 別スレッドでHDD上の辞書アクセスがおこなわれていて、ATOKエンジンの状態は非同期に変化している
	int pos;

	// まず非同期FEPレイヤから、変換済み(確定された)文字列を取得する
	#ifndef ATOK_NO_LINK
	getContext();
	#endif

	// 現在のキャレット位置を取得
	pos = g_Fep.nCaretPos;

	while(1)
	{
		// 1フレームにloopCnt文字しか処理しない
		loopCnt--;
		if(loopCnt <= 0)
		{
			break;
		}

		i = getChar(&c, 0);
		if(i == 0)
		{
			break;
		}

		memset((char*)g_Fep.szInsertText, 0x0, sizeof(g_Fep.szInsertText));		// 上書きされる文字列退避用バッファ 初期化

		// まず、文字列と制御コードに分離する
		if((c.Char<0x00000020 || c.Char==0x0000007F || c.Char==0xFFFFFFFF) && c.Char != 0x00000009)
		{
			// 制御コードだったとき
			// ATOKがOFFの状態で[Ctlr]+[A]～[Z]で発生
			g_pFepImport->dprintf("%08X %02X %04X %d %d\n", c.Char, c.Flag, c.RawCode, pos, g_Fep.nTextLen);

			switch(c.Char)
			{
			case 0x00000008:	// [CTRL]+[H],[BS]入力
				pos = inputBackspace(pos);		// バックスペース
				break;

			case 0x0000000A:	// [CTRL]+[M]入力
			case 0x0000000D:	// [CTRL]+[J],[CR]入力
				g_pFepImport->fep.ok();			// 終了
				#if 1//SKB_EX Sound
				Fep_se_play(SKBSE_ENTER);
				#endif
				break;

			case 0x00000015:	// [CTRL]+[U]入力
				inputAllClear(&pos);			// 全部削除
				pos = 0;
				break;

			case 0x0000007F:	// [DEL]入力
				inputDelete(pos);
				break;

			case 0x0000001B:	// [ESC]入力
				g_pFepImport->fep.cancel();
				#if 1//SKB_EX Sound
				Fep_se_play(SKBSE_CANCEL);
				#endif
				break;

			case 0xFFFFFFFF:
				// デッドキャラクタ
				// ATOKエンジンを通しても文字,制御コードに変換できなかったものがここに集められる
				// USBKB_RAWDAT,USBKB_KEYPADビットを落として比較
				switch(c.RawCode & 0x3FFFU)
				{
				case USBKEYC_RIGHT_ARROW:		// [→]キー
					if(pos < g_Fep.nTextLen)
					{
						pos++;
						#if 1//SKB_EX Sound
						Fep_se_play(SKBSE_CARETMOVE);
						#endif
					}
					#if 1//SKB_EX Sound
					else
					{
						Fep_se_play(SKBSE_INHIBITINPUT);
					}
					#endif
					break;

				case USBKEYC_LEFT_ARROW:		// [←]キー
					if(pos>0)
					{
						pos--;
						#if 1//SKB_EX Sound
						Fep_se_play(SKBSE_CARETMOVE);
						#endif
					}
					#if 1//SKB_EX Sound
					else
					{
						Fep_se_play(SKBSE_INHIBITINPUT);
					}
					#endif
					break;

				case USBKEYC_UP_ARROW:			// [↑]キー
					// FIXME : 今のところ機能無し
					break;

				case USBKEYC_DOWN_ARROW:		// [↓]キー
					// FIXME : 今のところ機能無し
					break;

				case USBKEYC_HOME:				// [HOME]キー
					pos = 0;
					#if 1//SKB_EX Sound
					if( pos==0 )  Fep_se_play(SKBSE_INHIBITINPUT);
					else          Fep_se_play(SKBSE_CARETMOVE);
					#endif
					break;

				case USBKEYC_END:				// [END]キー
					pos = g_Fep.nTextLen;
					#if 1//SKB_EX Sound
					if( pos==g_Fep.nTextLen )  Fep_se_play(SKBSE_INHIBITINPUT);
					else                       Fep_se_play(SKBSE_CARETMOVE);
					#endif
					break;

				case USBKEYC_INSERT:			// [INSERT]キー
					g_Fep.iInsertMode ^= 1;
					break;

				// -------↓ NumLock=OFFの場合 
				case USBKEYC_KPAD_0:
					if(!g_pFepImport->fep.getNumlockState())
					{
						g_Fep.iInsertMode ^= 1;	// [INSERT]キー
					}
					break;
				case USBKEYC_KPAD_PERIOD:
					if(!g_pFepImport->fep.getNumlockState())
					{
						inputDelete(pos);		// [DEL]入力
					}
					break;
				case USBKEYC_KPAD_1:
					if(!g_pFepImport->fep.getNumlockState())
					{
						pos = g_Fep.nTextLen;	// [END]キー
						#if 1//SKB_EX Sound
						if( pos==g_Fep.nTextLen )  Fep_se_play(SKBSE_INHIBITINPUT);
						else                       Fep_se_play(SKBSE_CARETMOVE);
						#endif
					}
					break;
				case USBKEYC_KPAD_2:
					if(!g_pFepImport->fep.getNumlockState())
					{
						// FIXME : 今のところ機能無し
						// [↓]キー
					}
					break;
				case USBKEYC_KPAD_3:
					if(!g_pFepImport->fep.getNumlockState())
					{
						// FIXME : 今のところ機能無し
						// [PAGEDOWN]キー
					}
					break;
				case USBKEYC_KPAD_4:
					if(!g_pFepImport->fep.getNumlockState())
					{
						// [←]キー
						if(pos>0)
						{
							pos--;
							#if 1//SKB_EX Sound
							Fep_se_play(SKBSE_CARETMOVE);
							#endif
						}
						#if 1//SKB_EX Sound
						else
						{
						     Fep_se_play(SKBSE_INHIBITINPUT);
						}
						#endif
					}
					break;
				case USBKEYC_KPAD_6:
					if(!g_pFepImport->fep.getNumlockState())
					{
						// [→]キー
						if(pos < g_Fep.nTextLen)
						{
							pos++;
							#if 1//SKB_EX Sound
							Fep_se_play(SKBSE_CARETMOVE);
							#endif
						}
						#if 1//SKB_EX Sound
						else
						{
						    Fep_se_play(SKBSE_INHIBITINPUT);
						}
						#endif
					}
					break;
				case USBKEYC_KPAD_7:
					if(!g_pFepImport->fep.getNumlockState())
					{
						pos = 0;		// [HOME]キー
						#if 1//SKB_EX Sound
						if( pos==0 )  Fep_se_play(SKBSE_INHIBITINPUT);
						else          Fep_se_play(SKBSE_CARETMOVE);
						#endif
					}
					break;
				case USBKEYC_KPAD_8:
					if(!g_pFepImport->fep.getNumlockState())
					{
						// FIXME : 今のところ機能無し
						// [↑]キー
					}
					break;
				case USBKEYC_KPAD_9:
					if(!g_pFepImport->fep.getNumlockState())
					{
						// FIXME : 今のところ機能無し
						// [PAGEUP]キー
					}
					break;
				// -------↑ NumLock=OFFの場合

				case USBKEYC_DELETE:			// [Delete]キー
					if( c.Flag != 0 
					#ifndef ATOK_NO_LINK
					 || ( g_Fep.ucUseAtok && sceAtokGetInputState(&g_Atok) != SCE_ATOK_ISTATE_BEFOREINPUT )
					#endif
					){
						break;
					}
					inputDelete(pos);
					break;

				case USBKEYC_ESCAPE:			// [ESC]キー
					if( c.Flag != 0 
					#ifndef ATOK_NO_LINK
					 || ( g_Fep.ucUseAtok && sceAtokGetInputState(&g_Atok) != SCE_ATOK_ISTATE_BEFOREINPUT )
					#endif
					){
						break;
					}
					g_pFepImport->fep.cancel();
					#if 1//SKB_EX Sound
					Fep_se_play(SKBSE_CANCEL);
					#endif
					break;

				case USBKEYC_BS:				// [BS]キー
					if( c.Flag != 0 
					#ifndef ATOK_NO_LINK
					 || ( g_Fep.ucUseAtok && sceAtokGetInputState(&g_Atok) != SCE_ATOK_ISTATE_BEFOREINPUT )
					#endif
					){
						break;
					}
					pos = inputBackspace(pos);
					break;

				case USBKEYC_ENTER:				// [ENTER]キー
					if( c.Flag != 0 
					#ifndef ATOK_NO_LINK
					 || ( g_Fep.ucUseAtok && sceAtokGetInputState(&g_Atok) != SCE_ATOK_ISTATE_BEFOREINPUT )
					#endif
					){
						break;
					}
					g_pFepImport->fep.ok();
					#if 1//SKB_EX Sound
					Fep_se_play(SKBSE_ENTER);
					#endif
					break;
				}
				break;
			}
		}
		else
		{
			int w = FEP_EDIT_ADD;	// 挿入

			switch(c.Char)
			{
			case 0x00000009:
				if(c.Flag != 0)
				{
					continue;
				}
				break;
			}

			// 制御コードでなかったとき、何バイトのリテラル入力があったか調べる
			// 挿入処理
			j = 0;
			g_pFepImport->dprintf("%08X %02X %04X %d %d\n", c.Char, c.Flag, c.RawCode, pos, g_Fep.nTextLen);
			if(g_Fep.iInsertMode)
			{
				if(g_pFepImport->fep.uiMaxTextLen > g_Fep.nTextLen)
				{
					i = g_Fep.nTextLen - pos;
					if(i > 0)
					{
						j = 1;
					}
				}
			}
			else
			{
				// INSERT
				j = 0;
			}

			if(g_Fep.iInsertMode == 0)
			{
				// 上書き
				if(g_Fep.nTextLen == pos)
				{
					// 挿入(キャレットが文字列の最後にある場合)
					w = FEP_EDIT_ADD;
				}
				else
				{
					// 上書き
					w = FEP_EDIT_INSERT;
					g_Fep.szInsertText[0] = g_pFepImport->fep.puiUCS4[pos];
					g_Fep.szInsertText[1] = 0;								// NOTICE : 文字1文字のみ
				}
			}

			// 文字入力処理
			{
				u_int cTmp[10];
				cTmp[0] = c.Char;
				cTmp[1] = 0;
				// チェック
				if(FEP_EDIT_OK == g_pFepImport->fep.changeText(w, cTmp, 0))
				{
					// 変数jで示されたバイト数だけ挿入処理を行う
					if(j)
					{
						// 領域がオーバーラップするので、memcpy()ではなくmemmove()を使う
						memmove((char*)&g_pFepImport->fep.puiUCS4[pos+j], (char*)&g_pFepImport->fep.puiUCS4[pos], (size_t)(i*sizeof(u_int)));
						g_Fep.nTextLen += j;
					}
					g_pFepImport->fep.puiUCS4[pos] = c.Char;
					pos++;
				}
				#if 1//SKB_EX Sound
				else
				{
				    Fep_se_play(SKBSE_INHIBITINPUT);
				}
				#endif
			}

			// 次のキャラクタをピーク(キューから取り出さずに、先頭のデータを盗み見る)
			i = getChar(&c, 1);
			if(i==0)
			{
				// キュー内に次のキャラクタが存在していなかった場合
				break;
			}
			else if(c.Char<0x00000020 || c.Char==0x0000007F || c.Char==0xFFFFFFFF)
			{
				// キューの次に位置しているキャラクタが制御系のコードだった場合いったん処理を打ちきり。
				// 制御系キャラクタの処理は次回行う。
				// 各文字がどの位置に描画されていたかを知る必要があるキャレット系の制御(カーソルを1行上にするとか)は、
				// 画面上での描画位置が決定されていないと処理ができないため。
				break;
			}
		}
	} // while

	g_Fep.nCaretPos = pos;
	if(pos > g_Fep.nTextLen)
	{
		g_Fep.nTextLen = pos;
	}
	g_pFepImport->fep.puiUCS4[g_Fep.nTextLen]    = 0x00;

	return 0;
}


//============================================================================
//  Fep_GetCandListMax											ref:  libfep.h
//
//  候補一覧 最大要素数取得
//
//  戻り値 : u_int : 候補一覧最大要素数
u_int Fep_GetCandListMax(void)
{
	return g_Fep.nCandidates;
}


//============================================================================
//  Fep_GetCandListOffset										ref:  libfep.h
//
//  候補一覧 オフセット取得
//
//  戻り値 : u_int : オフセットNO
u_int Fep_GetCandListOffset(void)
{
	return g_Fep.nCandOffset;						// 表示を始める候補リストの先頭番号を得る
}


//============================================================================
//  Fep_GetCurrentCand											ref:  libfep.h
//
//  候補一覧 現在候補NO取得
//
//  戻り値 : u_int : 現在候補NO
u_int Fep_GetCurrentCand(void)
{
	return g_Fep.nCurrentCand;
}


//============================================================================
//  Fep_GetCurrentCand											ref:  libfep.h
//
//  候補一覧 表示開始文字列へのポインタ取得
//
//  戻り値 : char* : 表示開始文字列へのポインタ
char *Fep_GetCandStart(void)
{
	char *pszConverting;							// 変換中の文字列(ひらがな)
	int l;											// 変換元文字列の長さ
	int ofs;										// 表示を始める候補リストの番号
	int i, n;
	char *cp;

	ofs = g_Fep.nCandOffset;						// 表示を始める候補リストの先頭番号を得る
	pszConverting = g_Fep.CandBuf;					// 候補リストのバッファを得る
	l  = strlen(pszConverting);						// 変換元文字列の文字列長を得る
	cp = g_Fep.CandBuf + l + 1;						// 変換元文字列をスキップ
	for(i=0; i<ofs; i++) {
		n = strlen(cp);
		cp += n + 2;
	}

	return cp;										// 表示する候補リストの先頭
}


//============================================================================
//  Fep_CandListNum												ref:  libfep.h
//
//  候補一覧 要素数取得
//
//  戻り値 : const int : 候補一覧要素数
const int Fep_CandListNum(void)
{
	int i;
	int iCandNum = 0;
	for(i=0; i<10; i++)
	{
		if((g_Fep.nCandOffset+i) >= g_Fep.nCandidates)
		{
			break;
		}
		iCandNum++;
	}

	return iCandNum;
}


//============================================================================
//  Fep_GetCandListItem											ref:  libfep.h
//
//  候補一覧 文字列へのポインタ取得
//
//  char *pStartPos : 候補一覧文字列開始位置
//  char *pItem     : 候補文字列へのポインタ
//  char *pSubItem  : 付属語文字列へのポインタ
//
//  戻り値 : char* : 文字列へのポインタ
char *Fep_GetCandListItem(char *pStartPos, char *pItem, char *pSubItem)
{
	const char *pszConverting;						// 変換中の文字列(ひらがな)
	char *cp = pStartPos;
	int n, len, o, l;

	pszConverting = g_Fep.CandBuf;					// 候補リストのバッファを得る
	l = strlen(pszConverting);						// 変換元文字列の文字列長を得る

	n = strlen(cp);
	memcpy(pItem, cp, n);
	pItem[n] = '\0';

	o = *(const u_char *)&cp[n + 1];				// 付属語のオフセット位置情報を得る
	if(o < l)
	{
		len = strlen(&pszConverting[o]);
		memcpy(pSubItem, &pszConverting[o], len);
		pSubItem[len] = '\0';
	}
	else
	{
		pSubItem[0] = '\0';
	}

	cp += n + 2;
	return cp;
}


//============================================================================
//  Fep_GetCandListMaxWidth										ref:  libfep.h
//
//  候補一覧 文字幅の最大値を求める(ピクセル)
//
//  float mainChars : 変換中文字
//  float subChars  : 付属  文字
//
//  戻り値 : u_int : 文字幅の最大値
u_int Fep_GetCandListMaxWidth(float mainChars, float subChars)
{
	const char *pszConverting;						// 変換中の文字列(ひらがな)
	const char *pCandStart;							// 候補リストデータの先頭
	int ofs;										// 表示を始める候補リストの番号
	int l;											// 変換元文字列の長さ
	int nCands;										// 表示する候補リスト数
	u_int iPixels = 0;								// 候補リストの横幅サイズ(GS座標系)

	const char *cp;
	int i, j, n, o;

	ofs = g_Fep.nCandOffset;						// 表示を始める候補リストの先頭番号を得る
	pszConverting = g_Fep.CandBuf;					// 候補リストのバッファを得る
	l  = strlen(pszConverting);						// 変換元文字列の文字列長を得る
	cp = g_Fep.CandBuf + l + 1;						// 変換元文字列をスキップ
	for(i=0; i<ofs; i++)
	{
		n = strlen(cp);
		cp += n + 2;
	}
	pCandStart = cp;								// 表示する候補リストの先頭


	// 候補リストの中から最も長い文字列を取り出す
	{
		// フォントサイズを設定
		iPixels = 0;
		nCands = 0;
		cp = pCandStart;
		for(i=0; i<10; i++)
		{
			if((ofs+i)>=g_Fep.nCandidates)
			{
				break;
			}

			// 候補文字列を表示した時のピクセル数を得る
			j = g_pFepImport->fep.getStrWidthSJIS(cp, mainChars);
			n = strlen(cp);							// 候補文字列の長さを得る
			o = *(const u_char *)&cp[n + 1];		// 付属語のオフセット位置情報を得る
			if(o < l)
			{
				// 付属語を表示した時のピクセル数を得る
				j += g_pFepImport->fep.getStrWidthSJIS(&pszConverting[o], subChars);
			}
			if(iPixels < j)
			{
				iPixels = j;
			}
			nCands++;
			cp += n+2;
		}
	}

	return iPixels;
}


//============================================================================
//  Fep_AtokGetInputState										ref:  libfep.h
//
//  ATOK 入力状態取得
//
//  戻り値 : int : 入力状態
int Fep_AtokGetInputState(void)
{
	#ifndef ATOK_NO_LINK
	if(!g_Fep.ucUseAtok)
	{
		return 0;
	}

	// 入力状態を取得
	return sceAtokGetInputState(&g_Atok);
	#else
	return 0;
	#endif
}


//============================================================================
//  Fep_ShowCandList											ref:  libfep.h
//
//  候補一覧を表示すべきか
//
//  戻り値 : u_int : 0 表示しない
//					 1 表示する
u_int Fep_ShowCandList(void)
{
	#ifndef ATOK_NO_LINK
	if(g_Fep.state == SCE_ATOK_ISTATE_CANDIDATES)
	{
		return 1;	// 表示する
	}
	#endif

	return 0;		// 表示しない
}


//============================================================================
//  Fep_Changing												ref:  libfep.h
//
//  変換中か？
//
//  戻り値 : u_int : 0 変換前
//					 1 変換中
u_int Fep_Changing(void)
{
	#ifndef ATOK_NO_LINK
	if(!g_Fep.ucUseAtok)
	{
		return 0;
	}

	{
		int s = sceAtokGetInputState(&g_Atok);

		if(s >= SCE_ATOK_ISTATE_CONVERTING)
		{
			return 1;	// 変換中
		}
		else
		{
			return 0;	// 変換前
		}
	}
	#else
	return 0;
	#endif
}


//============================================================================
//  Fep_GetCaretPos												ref:  libfep.h
//
//  キャレット位置取得
//
//  戻り値 : int : キャレット位置
int Fep_GetCaretPos(void)
{
	return g_Fep.nCaretPos;
}


//============================================================================
//  Fep_GetConvertChars											ref:  libfep.h
//
//  変換中 文字列取得
//
//  char *sjis : 文字列のポインタ(SJISの文字コードで入る)
void Fep_GetConvertChars(char *sjis)
{
	int len = strlen(g_Fep.szConv);
	if(len >= HENKAN_SIZE)
	{
		sjis[0] = '\0';
	}

	memcpy(sjis, g_Fep.szConv, HENKAN_SIZE);
	sjis[len] = '\0';
}


//============================================================================
//  Fep_GetClusterNum											ref:  libfep.h
//
//  文節数取得
//
//  戻り値 : int : 文節数
int Fep_GetClusterNum(void)
{
	return g_Fep.nClusters;
}


//============================================================================
//  Fep_GetNowCluster											ref:  libfep.h
//
//  注目文節のNO取得
//
//  戻り値 : int : 文節NO
int Fep_GetNowCluster(void)
{
	return g_Fep.FocusCls;
}


//============================================================================
//  Fep_GetClusterChars											ref:  libfep.h
//
//  文節 文字列取得
//
//  int index : 文節インデックス
//
//  戻り値 : char* : 文節文字列へのポインタ
char *Fep_GetClusterChars(int index)
{
	return &g_Fep.aszClusters[index][0];
}


void Fep_ClearText(void)
{
	g_Fep.nCaretPos = 0;									// キャレットの存在するバイト位置
	g_Fep.nTextLen  = 0;									// 入力済み文字列のバイト数
}


//============================================================================
//  Fep_SetText													ref:  libfep.h
//
//  テキスト初期設定
//
//  const u_int *pUCS4 : 設定したい文字列へのポインタ(UCS4)
void Fep_SetText(const u_int *pUCS4)
{
	int i;
	COOKED_CHAR cc;

	if(pUCS4 == NULL)
	{
		return;
	}
	Fep_ClearText();					// テキストクリア

	i=0;
	while(*(pUCS4+i))
	{
		cc.Char    = *(pUCS4+i);
		cc.Flag    = 0;
		cc.RawCode = 0;

		FifoPostData(&g_Fep.qFepOuput, &cc, sizeof(COOKED_CHAR));
		i++;
	}
}


//============================================================================
//  Fep_IsIdle													ref:  libfep.h
//
//  現在の瞬間、FEPワーカスレッドがアイドル状態であるかどうか返す
//
//  戻り値: int : 非アイドル状態(0)
//                アイドル状態  (1)
int Fep_IsIdle(void)
{
	if(FifoGetBytes(&g_Fep.qKeyInput) > 0) {
		return(0);
	}

	#ifndef ATOK_NO_LINK
	if(g_Fep.ucUseAtok)
	{
		struct SemaParam sp;
		ReferSemaStatus(g_AtokRsrcSema, &sp);
		if(sp.currentCount==0) {
			return(0);
		}
	}
	#endif

	if(FifoGetBytes(&g_Fep.qFepOuput) > 0) {
		return(0);
	}

	return(1);
}



//============================================================================
//  Fep_SetInputMode											ref:  libfep.h
//
//  入力モード変更
//
//  fepAtokInputMode_m mode : モード
//
//  戻り値: int : 正常終了(0)
//                エラー  (1)
int Fep_SetInputMode(fepAtokInputMode_m mode)
{
	if(mode > (FEPMODE_MAX-1))
	{
		// 予期しない値入力時
		return 1;
	}

	if(!g_Fep.ucUseAtok)
	{
		// ATOK 使ってない
		g_pFepImport->fep.changeMode(EX_ATOK_EDIT_KANJIOFF);	// 最小バーと同期させる為にコールバックを呼ぶ
		return 0;
	}
	#ifndef ATOK_NO_LINK

	// ATOK使っている時
	if(mode == FEPOFF)
	{
		if(sceAtokGetKanjiState(&g_Atok))
		{
			Fep_Cmd(&g_Atok, SCE_ATOK_EDIT_DELETEALL);		// ATOKに[削除]要求
		}

		// FEP OFF
		if(sceAtokGetKanjiState(&g_Atok))
		{
			Fep_Cmd(&g_Atok, EX_ATOK_EDIT_KANJIOFF);
		}
		return 0;
	}
	else
	{
		u_int f;
	
		if(!sceAtokGetKanjiState(&g_Atok))
		{
			sceAtokKanjiOn(&g_Atok);				// ATOKを漢字入力モードへ
			#if 0//SKB_EX Default 
			// ひらがなとは限らないので、カット。この後再設定されるので必要ない。
			Fep_Cmd(&g_Atok, EX_ATOK_FIXOFF);
			Fep_Cmd(&g_Atok, EX_ATOK_DSPHIRAGANA);
			#endif
			sceAtokFlushConverted(&g_Atok);			// 確定文字列バッファのフラッシュを行う
		}
		else
		{
			Fep_Cmd(&g_Atok, SCE_ATOK_EDIT_DELETEALL);		// ATOKに[削除]要求
		}


		Fep_Cmd(&g_Atok, EX_ATOK_FIXOFF);		// とりあえずFIXOFFを無効にする
		switch(mode)
		{
		case FEPON_HIRA:
			f = EX_ATOK_DSPHIRAGANA;	// 全角ひらがな入力モード
			break;
		case FEPON_HAN_KATAKANA:
			f = EX_ATOK_DSPHANKANA;		// 半角カタカナ入力モード
			break;
		case FEPON_ZEN_KATAKANA:
			f = EX_ATOK_DSPZENKANA;		// 全角カタカナ入力モード
			break;
		case FEPON_HAN_ALPHABET:
			f = EX_ATOK_DSPHANRAW;		// 半角無変換入力モード
			break;
		case FEPON_ZEN_ALPHABET:
			f = EX_ATOK_DSPZENRAW;		// 全角無変換入力モード
			break;
		#if 1//SKB_EX FixMode
		case FEPON_HIRA_FIX:
			f = EX_ATOK_FIXHIRAGANA;	// 固定入全角ひらがな入力モード
			break;
		case FEPON_HAN_KATAKANA_FIX:
			f = EX_ATOK_FIXHANKANA;		// 固定入半角カタカナ入力モード
			break;
		case FEPON_ZEN_KATAKANA_FIX:
			f = EX_ATOK_FIXZENKANA;		// 固定入全角カタカナ入力モード
			break;
		case FEPON_HAN_ALPHABET_FIX:
			f = EX_ATOK_FIXHANRAW;		// 固定入半角無変換入力モード
			break;
		case FEPON_ZEN_ALPHABET_FIX:
			f = EX_ATOK_FIXZENRAW;		// 固定入全角無変換入力モード
			break;
		#endif
		default:
			g_pFepImport->dprintf("ERROR : Fep_SetInputMode - unKnown.\n");
			return 1;
		}
		Fep_Cmd(&g_Atok, f);
	}
	#endif

	return 0;
}


//============================================================================
//  Fep_GetInsertChars											ref:  libfep.h
//
//  上書きされる文字列へのポインタを取得
//
//  戻り値: const u_int * : 上書きされる文字列へのポインタ
const u_int *Fep_GetInsertChars(void)
{
	return g_Fep.szInsertText;
}


#ifndef ATOK_NO_LINK
//============================================================================
//  Fep_Cmd														ref:  libfep.h
//
//  FEPコマンド
//
//  sceAtokContext *pAtok : ATOKへのコンテキストポインタ
//  u_int f               : 文字とか
//
//  戻り値 : int : 正常終了(0)
//                 コマンド定義エラー(1)
int Fep_Cmd(sceAtokContext *pAtok, u_int f)
{
	static int changeMuhenkan=0, changeMuhenkanRaw=0, lastMode=1;
	int s;

	if(g_Fep.ucUseAtok == 0)
	{
		return 0;
	}

	s = sceAtokGetInputState(pAtok);

	// [F10キーが押された時]
	// SCE_ATOK_EDIT_POSTCONVRAWを最初の一回やることによって
	// 「ABCDE」→「Abcde」→「abcde」になります。
	if(EX_ATOK_EDIT_POSTRAWHALF != f)
	{
		g_Fep.cntF10 = 0;
	}

	// ATOKエンジンにコマンドを送出
	if(!(f & EX_ATOK_EDIT_NULL))
	{
		// 定義済みファンクションだった場合
		// そのままテーブルから得た値を使ってコマンド送信
		if(s==SCE_ATOK_ISTATE_MOVECLAUSEGAP && (f & 0x200))
		{
			// 文節区切り変更中に次のキーがきた場合
			sceAtokEditConv(pAtok, SCE_ATOK_EDIT_KAKUTEIALL, 0);
			Fep_SetData2Fifo();
		}
		sceAtokEditConv(pAtok, f, 0);

	} else {
		// 複合ファンクションだった場合
		sceAtokConfig cfg;
		cfg.Size = sizeof(sceAtokConfig);

		if(f == EX_ATOK_EDIT_CURSTAY)
		{
			// ダミー
			return(0);
		}

		if(EX_ATOK_CHANGE_DSPKANA != f)
		{
			// 無変換フラグ初期化(カナ)
			changeMuhenkan = 0;
		}
		if(EX_ATOK_CHANGE_DSPRAW != f)
		{
			// 無変換フラグ初期化(アルファベット)
			changeMuhenkanRaw = 0;
		}

		switch(f) {
			case EX_ATOK_EDIT_KANJIOFF:
				// 変換中文字を全消去
				if(s == SCE_ATOK_ISTATE_BEFORECONVERT)
				{
					// 変換前
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_DELETEALL, 0);		// ATOKに[消去]を送る
				}
				else
				{
					// 変換中
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_CONVCANCEL, 0);		// ATOKに[変換キャンセル]を送る
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_DELETEALL, 0);		// ATOKに[消去]を送る
				}

				// 漢字入力モードOFF
				sceAtokKanjiOff(pAtok);
				g_pFepImport->fep.atokOFF(pAtok);
				g_pFepImport->fep.changeMode(EX_ATOK_EDIT_KANJIOFF);			// 最小バーと同期させる為にコールバックを呼ぶ
				break;

			case EX_ATOK_EDIT_KANALOCK:
				// カナロック切り替え
				g_Fep.led ^= USBKB_LED_KANA;
				break;

			case EX_ATOK_DSPHIRAGANA:		// ひらがな入力
			case EX_ATOK_DSPZENKANA:		// 全角カタカナ入力
			case EX_ATOK_DSPZENRAW:			// 全角無変換入力
			case EX_ATOK_DSPHANKANA:		// 半角カタカナ入力
			case EX_ATOK_DSPHANRAW:			// 半角無変換入力
				// 入力文字種設定
				// CAUTION: SCE_ATOK_ISTATE_BEFOREINPUTでしか文字種の切り替えをできない
				g_Fep.KanDspType = (f - EX_ATOK_DSPHIRAGANA) + 1;
				cfg.KanDspType = g_Fep.KanDspType;
				sceAtokSetConfig(pAtok, &cfg, SCE_ATOK_CONFIG_KANDSPTYPE);
				if(EX_ATOK_DSPZENRAW != f && EX_ATOK_DSPHANRAW!= f)
				{
					lastMode = g_Fep.KanDspType;
				}
				g_pFepImport->fep.changeMode(f);		// 最小バーと同期させる為にコールバックを呼ぶ
				break;

			case EX_ATOK_DSPSWEISU:			// 英数切り替え
				// CAUTION: SCE_ATOK_ISTATE_BEFOREINPUTでしか文字種の切り替えをできない
				if(g_Fep.KanDspType==3) {
					// すでに全角無変換入力モードだったとき、
					g_Fep.KanDspType = 5;		// 半角無変換入力
				} else {
					g_Fep.KanDspType = 3;		// 全角無変換入力
				}
				cfg.KanDspType = g_Fep.KanDspType;
				sceAtokSetConfig(pAtok, &cfg, SCE_ATOK_CONFIG_KANDSPTYPE);
				g_pFepImport->fep.changeMode(EX_ATOK_DSPHIRAGANA+g_Fep.KanDspType-1);							// 最小バーと同期させる為にコールバックを呼ぶ
				break;

			case EX_ATOK_DSPSWKANA:			// かな切り替え
				// CAUTION: SCE_ATOK_ISTATE_BEFOREINPUTでしか文字種の切り替えをできない
				if(g_Fep.KanDspType==1) {
					// 全角ひらがな入力モードだったとき
					g_Fep.KanDspType = 2;		// 全角カタカナ入力
				} else if(g_Fep.KanDspType==2) {
					// 全角かたかな入力モードだったとき、
					g_Fep.KanDspType = 4;		// 半角カタカナ入力
				} else {
					g_Fep.KanDspType = 1;		// 全角ひらがな入力
				}
				cfg.KanDspType = g_Fep.KanDspType;
				sceAtokSetConfig(pAtok, &cfg, SCE_ATOK_CONFIG_KANDSPTYPE);
				g_pFepImport->fep.changeMode(EX_ATOK_DSPHIRAGANA+g_Fep.KanDspType-1);							// 最小バーと同期させる為にコールバックを呼ぶ
				break;


			case EX_ATOK_FIX_ZENHIRA_HANRAW:
				// [あ][ア][ｱ]→[A]
				if(SCE_ATOK_ISTATE_BEFORECONVERT > s)
				{
					g_Fep.cntFix ^= 1;

					if(g_Fep.KanDspType == lastMode)
					{
						// 前の入力種類と現在の入力種類が一緒の場合は、強制的に無変換にさせる
						g_Fep.cntFix = 1;
					}

					if(g_Fep.cntFix && g_Fep.KanDspType != 5 && g_Fep.KanDspType != 3)
					{
						// ひらがな、半角カタカナ、全角カタカナだったらモード保存
						lastMode = g_Fep.KanDspType;
						// [A]にする
						g_Fep.KanDspType = 5;		// 半角無変換入力
					}
					else
					{
						g_Fep.KanDspType = lastMode;
					}
					cfg.FixMode = EX_ATOK_FIXOFF-EX_ATOK_FIXOFF;
					sceAtokSetConfig(pAtok, &cfg, SCE_ATOK_CONFIG_FIXMODE);					// FIXモードをOFFにする
					cfg.KanDspType = g_Fep.KanDspType;
					sceAtokSetConfig(pAtok, &cfg, SCE_ATOK_CONFIG_KANDSPTYPE);
					g_pFepImport->fep.changeMode(EX_ATOK_DSPHIRAGANA+g_Fep.KanDspType-1);							// 最小バーと同期させる為にコールバックを呼ぶ
				}
				break;

			case EX_ATOK_FIX_ZENHIRA_ZENRAW:
				// [あ][ア][ｱ]→[Ａ]
				if(SCE_ATOK_ISTATE_BEFORECONVERT > s)
				{
					g_Fep.cntFixShift ^= 1;

					if(g_Fep.KanDspType == lastMode)
					{
						// 前の入力種類と現在の入力種類が一緒の場合は、強制的に無変換にさせる
						g_Fep.cntFixShift = 1;
					}


					if(g_Fep.cntFixShift && g_Fep.KanDspType != 5 && g_Fep.KanDspType != 3)
					{
						// [ひらがな入力]、[半角カタカナ入力]、[全角カタカナ入力]だったらモード保存
						lastMode = g_Fep.KanDspType;

						g_Fep.KanDspType = 3;		// 全角無変換入力
					}
					else
					{
						g_Fep.KanDspType = lastMode;
					}
					cfg.FixMode = EX_ATOK_FIXOFF-EX_ATOK_FIXOFF;
					sceAtokSetConfig(pAtok, &cfg, SCE_ATOK_CONFIG_FIXMODE);					// FIXモードをOFFにする
					cfg.KanDspType = g_Fep.KanDspType;
					sceAtokSetConfig(pAtok, &cfg, SCE_ATOK_CONFIG_KANDSPTYPE);
					g_pFepImport->fep.changeMode(EX_ATOK_DSPHIRAGANA+g_Fep.KanDspType-1);							// 最小バーと同期させる為にコールバックを呼ぶ
				}
				break;

			case EX_ATOK_FIX_ZENHIRA_ZENKANA_HANKANA:
				if(SCE_ATOK_ISTATE_BEFORECONVERT > s)
				{
					if(g_Fep.KanDspType == 5 || g_Fep.KanDspType == 3)
					{
						// モードを固定させる為の作戦
						int chtbl[] = {0,4,1,0,2};
						g_Fep.KanDspType = chtbl[lastMode];
					}

					switch(g_Fep.KanDspType)
					{
					case 2:
						// 全角カタカナ入力だった場合
						g_Fep.KanDspType = 4;		// 半角カタカナ入力
						break;

					case 1:
						// ひらがな入力だった場合
						g_Fep.KanDspType = 2;		// 全角カタカナ入力
						break;

					case 4:
						// 半角カタカナ入力だった場合
					default:
						g_Fep.KanDspType = 1;		// 全角ひらがな入力
						break;
					}
					lastMode = cfg.KanDspType = g_Fep.KanDspType;
					sceAtokSetConfig(pAtok, &cfg, SCE_ATOK_CONFIG_KANDSPTYPE);
					g_pFepImport->fep.changeMode(EX_ATOK_DSPHIRAGANA+g_Fep.KanDspType-1);							// 最小バーと同期させる為にコールバックを呼ぶ
				}
				g_pFepImport->dprintf("--- %d ---, %d\n", __LINE__, g_Fep.KanDspType);
				break;

			case EX_ATOK_CHANGE_DSPKANA:

				if(changeMuhenkan == 0)
				{
					changeMuhenkan = 4;
				}

				switch(changeMuhenkan)
				{
				case 2:
					// 全角カタカナ入力
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVKANA, 0);	// 後変換カタカナ
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVHALF, 0);	// 後変換半角
					changeMuhenkan = 4;
					break;
				case 1:
					// 全角ひらがな入力
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVKANA, 0);	// 後変換カタカナ
					changeMuhenkan = 2;
					break;
				case 4:
					// 半角カタカナ入力
				default:
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVHIRA, 0);	// 後変換ひらがな
					changeMuhenkan = 1;
					break;
				}

				break;

			case EX_ATOK_CHANGE_DSPRAW:
				if(changeMuhenkanRaw == 0)
				{
					changeMuhenkanRaw = 5;
				}

				if(changeMuhenkanRaw == 3)
				{
					// 全角無変換入力
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVRAW, 0);	// 後変換無変換
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVHALF, 0);	// 後変換半角
					changeMuhenkanRaw = 5;
				}
				else
				{
					// 全角無変換入力
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVRAW, 0);	// 後変換無変換
					changeMuhenkanRaw = 3;
				}
				break;

			case EX_ATOK_FIXOFF:			// 固定入力モードOFF
			case EX_ATOK_FIXHIRAGANA:		// 固定入力ひらがな
			case EX_ATOK_FIXZENKANA:		// 固定入力全角カタカナ
			case EX_ATOK_FIXZENRAW:			// 固定入力全角無変換
			case EX_ATOK_FIXHANKANA:		// 固定入力半角カタカナ
			case EX_ATOK_FIXHANRAW:			// 固定入力半角無変換
				// 固定入力モードの設定
				cfg.FixMode = f - EX_ATOK_FIXOFF;
				sceAtokSetConfig(pAtok, &cfg, SCE_ATOK_CONFIG_FIXMODE);
				g_pFepImport->fep.changeMode(f);						// 最小バーと同期させる為にコールバックを呼ぶ

				break;

			case EX_ATOK_EDIT_CANDNEXT:
				// 次候補
				if(g_Fep.nCurrentCand < (g_Fep.nCandidates - 1)) {
					g_Fep.nCurrentCand++;
				} else {
					g_Fep.nCurrentCand = 0;
				}
				g_Fep.nCandOffset  = (g_Fep.nCurrentCand / 10) * 10;
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_SELECTCAND, (g_Fep.nCurrentCand + 1));
				#if 1//SKB_EX Sound
				Fep_se_play(SKBSE_MOVECANDLIST);
				#endif
				break;

			case EX_ATOK_EDIT_CANDPREV:
				// 前候補
				if(g_Fep.nCurrentCand) {
					g_Fep.nCurrentCand--;
				} else {
					g_Fep.nCurrentCand = g_Fep.nCandidates - 1;
				}
				g_Fep.nCandOffset  = (g_Fep.nCurrentCand / 10) * 10;
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_SELECTCAND, (g_Fep.nCurrentCand + 1));
				#if 1//SKB_EX Sound
				Fep_se_play(SKBSE_MOVECANDLIST);
				#endif
				break;


			case EX_ATOK_EDIT_CANDHEAD:
				// 候補先頭
				g_Fep.nCurrentCand = 0;
				g_Fep.nCandOffset  = 0;
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_SELECTCAND, (g_Fep.nCurrentCand + 1));
				break;

			case EX_ATOK_EDIT_CANDTAIL:
				// 候補末尾（表示中の）
				g_Fep.nCurrentCand = g_Fep.nCandidates - 1;
				g_Fep.nCandOffset  = (g_Fep.nCurrentCand / 10) * 10;
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_SELECTCAND, (g_Fep.nCurrentCand + 1));
				break;

			case EX_ATOK_EDIT_CANDGRPNEXT:
				// 次候補群
				if(((g_Fep.nCandOffset/10)*10 + 10) < g_Fep.nCandidates) {
					g_Fep.nCandOffset = (g_Fep.nCandOffset/10 + 1)*10;
				} else {
					g_Fep.nCandOffset = 0;
				}
				g_Fep.nCurrentCand = g_Fep.nCandOffset;
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_SELECTCAND, (g_Fep.nCurrentCand + 1));
				break;

			case EX_ATOK_EDIT_CANDGRPPREV:
				// 前候補群
				if((g_Fep.nCandOffset - 10) >= 0) {
					g_Fep.nCandOffset -= 10;
				} else {
					g_Fep.nCandOffset = ((g_Fep.nCandidates - 1)/10) * 10;
				}
				if((g_Fep.nCandOffset + 9) < g_Fep.nCandidates) {
					g_Fep.nCurrentCand = g_Fep.nCandOffset + 9;
				} else {
					g_Fep.nCurrentCand = g_Fep.nCandidates - 1;
				}
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_SELECTCAND, (g_Fep.nCurrentCand + 1));
				break;

			case EX_ATOK_EDIT_CLSEND_CAND:
				// 最後候補
				if(g_Fep.FocusCls == (g_Fep.nClusters-1))
				{
					break;
				}
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_MOVFOCUSCLAUS, 2);		// 文節末尾
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_CONVFORWARD  , 0);		// ATOKに[変換・順方向]を送る
				g_Fep.stateCandLeftEnd = EX_ATOK_EDIT_CLSEND_CAND;			// 候補一覧更新
				break;

			case EX_ATOK_EDIT_CLSTOP_CAND:
				// 先頭候補
				if(g_Fep.FocusCls == 0)
				{
					break;
				}
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_MOVFOCUSCLAUS, 3);		// 文節先頭
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_CONVFORWARD , 0);		// ATOKに[変換・順方向]を送る
				g_Fep.stateCandLeftEnd = EX_ATOK_EDIT_CLSTOP_CAND;			// 候補一覧更新
				break;

			case EX_ATOK_EDIT_CLSRIGHT:		// 文節右
			case EX_ATOK_EDIT_CLSLEFT:		// 文節左
			case EX_ATOK_EDIT_CLSEND:		// 文節末尾
			case EX_ATOK_EDIT_CLSTOP:		// 文節先頭
				// 文節区切り変更中には注目文節を移動できないので、
				// いったん後変換コマンドを発行し、SCE_ATOK_ISTATE_MOVECLAUSEGAP状態を脱出する
				if(s==SCE_ATOK_ISTATE_MOVECLAUSEGAP) {
					// どの文字種に後変換するか、いったん入力文字種を取得してしらべる。
					switch(g_Fep.KanDspType) {
						case 2:
							// 全角カタカナ入力
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVKANA, 0);	// 後変換カタカナ
							break;
						case 3:
							// 全角無変換入力
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVRAW,  0);	// 後変換無変換
							break;
						case 4:
							// 半角カタカナ入力
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVKANA, 0);	// 後変換カタカナ
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVHALF, 0);	// 後変換半角
							break;
						case 5:
							// 半角無変換
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVRAW,  0);	// 後変換無変換
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVHALF, 0);	// 後変換半角
							break;
						case 1:
							// 全角ひらがな入力
						default:
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVHIRA, 0);	// 後変換ひらがな
							break;
					}
				}
				// 文節の移動方法ごとに分岐
				switch(f) {
					case EX_ATOK_EDIT_CLSRIGHT:		// 文節右
						sceAtokEditConv(pAtok, SCE_ATOK_EDIT_MOVFOCUSCLAUS, 0);
						break;
					case EX_ATOK_EDIT_CLSLEFT:		// 文節左
						sceAtokEditConv(pAtok, SCE_ATOK_EDIT_MOVFOCUSCLAUS, 1);
						break;
					case EX_ATOK_EDIT_CLSEND:		// 文節末尾
						sceAtokEditConv(pAtok, SCE_ATOK_EDIT_MOVFOCUSCLAUS, 2);
						break;
					case EX_ATOK_EDIT_CLSTOP:		// 文節先頭
						sceAtokEditConv(pAtok, SCE_ATOK_EDIT_MOVFOCUSCLAUS, 3);
						break;
				}
				#if 1//SKB_EX Sound
				Fep_se_play(SKBSE_MOVECANDLIST);
				#endif
				break;

			case EX_ATOK_EDIT_REVCLSRIGHT:		// 文節右循環
			case EX_ATOK_EDIT_REVCLSLEFT:		// 文節左循環
				// 文節区切り変更中には注目文節を移動できないので、
				// いったん後変換コマンドを発行し、SCE_ATOK_ISTATE_MOVECLAUSEGAP状態を脱出する
				if(s==SCE_ATOK_ISTATE_MOVECLAUSEGAP) {
					// どの文字種に後変換するか、いったん入力文字種を取得してしらべる。
					switch(g_Fep.KanDspType) {
						case 2:
							// 全角カタカナ入力
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVKANA, 0);	// 後変換カタカナ
							break;
						case 3:
							// 全角無変換入力
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVRAW,  0);	// 後変換無変換
							break;
						case 4:
							// 半角カタカナ入力
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVKANA, 0);	// 後変換カタカナ
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVHALF, 0);	// 後変換半角
							break;
						case 5:
							// 半角無変換
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVRAW,  0);	// 後変換無変換
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVHALF, 0);	// 後変換半角
							break;
						case 1:
							// 全角ひらがな入力
						default:
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVHIRA, 0);	// 後変換ひらがな
							break;
					}
				}
				// 文節の移動方法ごとに分岐
				switch(f) {
					case EX_ATOK_EDIT_REVCLSRIGHT:		// 文節右循環
						if((g_Fep.nClusters-1) == g_Fep.FocusCls)
						{
							// 最右端なので文節先頭へ
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_MOVFOCUSCLAUS, 3);
						}
						else
						{
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_MOVFOCUSCLAUS, 0);
						}
						break;
					case EX_ATOK_EDIT_REVCLSLEFT:		// 文節左循環
						if(g_Fep.FocusCls == 0)
						{
							// 最左端なので文節末尾へ
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_MOVFOCUSCLAUS, 2);
						}
						else
						{
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_MOVFOCUSCLAUS, 1);
						}
						break;
				}
				#if 1//SKB_EX Sound
				Fep_se_play(SKBSE_MOVECLS);
				#endif
				break;
			case EX_ATOK_EDIT_POSTRAWHALF:
				// 半角英数への復号変換
				// まず無変換文字列に変換したあと、続けて半角文字列への変換
				// CAUTION: 半角カナになる可能性があります
				if(g_Fep.cntF10==0)
				{
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVRAW,  0);
					g_Fep.cntF10++;
				}
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVHALF, 0);
				break;

			case EX_ATOK_EDIT_KAKUTEISPACE:
				// 確定後スペース挿入
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_KAKUTEIALL, 0);
				Fep_SetData2Fifo();					// 確定文字をFIFOに設定
				sceAtokEditConv(pAtok, ' ', 0);
				#if 1//SKB_EX Sound
				Fep_se_play(SKBSE_ONSPACE);
				#endif
				break;
			case EX_ATOK_EDIT_KAKUTEIHANSPACE:
				// 確定後半角スペース挿入
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_KAKUTEIALL, 0);
				Fep_SetData2Fifo();					// 確定文字をFIFOに設定
				{
					// 半角スペース送る
					COOKED_CHAR c;
					c.Flag    = 0x0;
					c.RawCode = 0x0000;
					c.Char    = 0x00000020;
					FifoSendData(&g_Fep.qFepOuput, &c, sizeof(COOKED_CHAR));
				}
				#if 1//SKB_EX Sound
				Fep_se_play(SKBSE_ONSPACE);
				#endif
				break;

			case EX_ATOK_EDIT_CAND1:
			case EX_ATOK_EDIT_CAND2:
			case EX_ATOK_EDIT_CAND3:
			case EX_ATOK_EDIT_CAND4:
			case EX_ATOK_EDIT_CAND5:
			case EX_ATOK_EDIT_CAND6:
			case EX_ATOK_EDIT_CAND7:
			case EX_ATOK_EDIT_CAND8:
			case EX_ATOK_EDIT_CAND9:
			case EX_ATOK_EDIT_CAND0:
				{
					int no = 0;
					no = (g_Fep.nCandOffset + (f - EX_ATOK_EDIT_CAND1)+1);
					if(no > g_Fep.nCandidates)
					{
						// 入力した数字が候補数より大きかった場合
						return 0;
					}
					// テンキーの[0]～[9]が押されたとき、候補リストから部分確定
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_KAKUTEIPART, no);
				}
				break;

			default:
				// 定義エラー
				return(1);
		}
	}

	getContext();
	return(0);
}
#endif


//============================================================================
//  Fep_SetKakuteiPart											ref:  libfep.h
//
//  部分確定をATOKエンジンに送る
//
//  戻り値: void
void Fep_SetKakuteiPart(void)
{
	#ifndef ATOK_NO_LINK
	if(!g_Fep.ucUseAtok)
	{
		// ATOK 使っていない
		return;
	}

	if(g_Fep.nIsConv != CONV_OFF)
	{
		g_pFepImport->dprintf("ERROR : Fep_SetKakuteiPart - convert on\n");
		return;
	}

	// KAKUTEIPARTを投げる
	Fep_PostApp(APP_KAKUTEIPART, 0, 0);
	#endif
}


//============================================================================
//  Fep_SetKakuteiAll											ref:  libfep.h
//
//  全文確定をATOKエンジンに送る
//
//  戻り値: void
void Fep_SetKakuteiAll(void)
{
	#ifndef ATOK_NO_LINK
	if(!g_Fep.ucUseAtok)
	{
		// ATOK 使っていない
		return;
	}

	if(g_Fep.nIsConv != CONV_OFF)
	{
		g_pFepImport->dprintf("ERROR : Fep_SetKakuteiPart - convert on\n");
		return;
	}

	// KAKUTEIALLを投げる
	Fep_PostApp(APP_KAKUTEIALL, 0, 0);
	#endif
}


//============================================================================
//  Fep_GetNowConv												ref:  libfep.h
//
//  ATOKが現在変換中か？
//
//  戻り値: int : 0:変換中じゃない
//                1:変換中
int Fep_GetNowConv(void)
{
	#ifndef ATOK_NO_LINK
	if(!g_Fep.ucUseAtok)
	{
		// ATOK 使っていない
		return 0;
	}

	return g_Fep.nIsConv;
	#else
	return 0;
	#endif
}


//============================================================================
//  Fep_GetNowAtokInput											ref:  libfep.h
//
//  ATOK使って入力中か？
//
//  //戻り値: int : 0:非入力中
//  //              1:入力中
//  //SKB_EX 
//  戻り値: int : 0: 非変換入力中
//                1: ローマ字変換入力中
//                2: かな変換入力中
//                3: 半角スルー入力中
int Fep_GetNowAtokInput(void)
{
	#ifndef ATOK_NO_LINK
	if(!g_Fep.ucUseAtok)
	{
		// ATOK 使っていない
		return 0;
	}

	{
		#if 1//SKB_EX 
		sceAtokConfig cfg;
		cfg.Size = sizeof(sceAtokConfig);
		sceAtokGetConfig(&g_Atok,&cfg,SCE_ATOK_CONFIG_KANJIINPUT);

		return cfg.KanjiInput;
		#else
		int s = sceAtokGetInputState(&g_Atok);
		if(s == SCE_ATOK_ISTATE_BEFORECONVERT )
		{
			return 0;
		}
		else
		{
			return 1;
		}
		#endif
	}
	#else
	return 0;
	#endif
}


//============================================================================
//  Fep_SetKanjiCancel											ref:  libfep.h
//
//  キャンセルをATOKエンジンに送る
//
//  戻り値: void
void Fep_SetKanjiCancel(void)
{
	#ifndef ATOK_NO_LINK
	int s;
	if(!g_Fep.ucUseAtok)
	{
		// ATOK 使ってない
		return;
	}

	s = Fep_AtokGetInputState();
	if(s == SCE_ATOK_ISTATE_BEFOREINPUT)
	{
		g_pFepImport->fep.cancel();
	}
	else if(s == SCE_ATOK_ISTATE_BEFORECONVERT)
	{
		Fep_PostApp(APP_DELETEALL, 0, 0);
	}
	else
	{
		Fep_PostApp(APP_CONVCANCEL, 0, 0);
	}
	#endif
}


//============================================================================
//  Fep_ConvStrings												ref:  libfep.h
//
//  入力位置以前の文字列取得
//  # キャレット描画時に必要になる・・・・
//
//  char *str_sjis : 格納用文字列バッファ
//
//  void : 戻り値
void Fep_ConvStrings(char *str_sjis)
{
	memcpy(str_sjis, g_Fep.szConv, g_Fep.FocusTop);
	str_sjis[g_Fep.FocusTop] = '\0';
}






static void workerMain(int param __attribute__((unused)))
{
	sceAtokContext *pAtok = NULL;
	#ifndef ATOK_NO_LINK
	if(g_Fep.ucUseAtok)
	{
		pAtok = &g_Atok;
	}
	#endif

	while(1)
	{
		// FIFOから新しいキーコードを取得する(WAITあり)
		INPUT_QUEUE_ELEM q;

		FifoRecvData(&g_Fep.qKeyInput, &q, sizeof(INPUT_QUEUE_ELEM), 0);

		#ifndef ATOK_NO_LINK
		if(g_Fep.ucUseAtok)
		{
			WaitSema(g_AtokRsrcSema);				// ATOKエンジンの排他的使用権を確保
			// ATOKエンジンの動作中は電源マスク
			g_pFepImport->fep.powerLock();
		}
		#endif

		// キューの先頭にあった要素のメッセージ由来種別に応じて分岐
		switch(q.type)
		{
		case INPUTTYPE_KEYBOARD:
			// キーボード由来のメッセージだった場合
			FepTreatKey(pAtok, q.kb.led, q.kb.mkey, q.kb.keycode);
			break;
		case INPUTTYPE_SOFTKB:
			// パッド由来のメッセージだった場合
			FepTreatSkb(pAtok, q.skb.usCtrlCode, q.skb.ucFlag, q.skb.puiChar);
			break;

		case INPUTTYPE_APP:
			// アプリ由来のメッセージだった場合
			FepTreatApp(pAtok, q.msg.cmd, q.msg.arg0, q.msg.arg1);
			break;
		}

		#ifndef ATOK_NO_LINK
		if(g_Fep.ucUseAtok)
		{
			// 確定文字列をFIFOに設定
			Fep_SetData2Fifo();

			// 変換終了
			g_Fep.nIsConv = CONV_OFF;

			// 電源ロックを解除
			g_pFepImport->fep.powerUnlock();

			SignalSema(g_AtokRsrcSema);				// ATOKエンジンの使用権を解放する
		}
		#endif
	}
}

static void FepTreatApp(sceAtokContext *pAtok, u_int cmd, u_int arg0, u_int arg1)
{
	switch(cmd)
	{
	case APP_INTERRUPT_KEY:
		interruptFromKey();
		break;
	#ifndef ATOK_NO_LINK
	case APP_KAKUTEIPART:
		Fep_Cmd(&g_Atok, SCE_ATOK_EDIT_KAKUTEIPART);	// ATOKに[部分確定]を送る
		break;
	case APP_KAKUTEIALL:
		Fep_Cmd(&g_Atok, SCE_ATOK_EDIT_KAKUTEIALL);		// ATOKに[全文確定]を送る
		break;
	case APP_CONVCANCEL:
		Fep_Cmd(pAtok, SCE_ATOK_EDIT_CONVCANCEL);		// ATOKに[変換キャンセル]を送る
		break;
	case APP_DELETEALL:
		Fep_Cmd(&g_Atok, SCE_ATOK_EDIT_DELETEALL);		// ATOKに[削除]要求
		break;
	#endif
	}
}



#ifndef ATOK_NO_LINK
//============================================================================
//  Fep_SetData2Fifo											ref:  libfep.h
//
//  部分文字列をATOKエンジンから引き取り文字列バッファに設定
//  (SJIS -> UCS2)
//
//  戻り値: void
void Fep_SetData2Fifo(void)
{
	char buf[KAKUTEI_SIZE];
	COOKED_CHAR cc;
	int len;
	int i;
	sceCccJISCS jis;
	sceCccUCS4 chr=0;
	sceCccSJISTF const* pSjis;

	// 確定表記文字列は新しい入力がはじまると消えてしまう可能性があるので、毎回引き取らなくてはならない
	len = sceAtokGetConvertedStr(&g_Atok, buf, KAKUTEI_SIZE);
	if(len == 0)
	{
		return;
	}

	pSjis = (sceCccSJISTF *)buf;
	i=0;len=0;
	while(*pSjis)
	{
		// SJIS -> UCS4
		jis = sceCccDecodeSJIS(&pSjis);
		cc.Char    = sceCccJIStoUCS(jis, chr);
		cc.Flag    = 0;
		cc.RawCode = 0;
		FifoSendData(&g_Fep.qFepOuput, &cc, sizeof(COOKED_CHAR));

		len++;
	}

	// 確定文字列バッファのフラッシュを行う
	sceAtokFlushConverted(&g_Atok);
}
#endif



//============================================================================
// STATIC FUNCTIONS
//============================================================================
//============================================================================
//	postKey
//
//	FEPレイヤのもつ入力FIFOキューにキー情報を登録
//
//	u_char led : LED情報
//	u_char mkey: モディファイアキー情報
//	u_short keycode : USBキーコード
static void postKey(u_char led, u_char mkey, u_short keycode)
{
	INPUT_QUEUE_ELEM q;
	sceAtokContext *pAtok = NULL;
	if(g_Fep.ucUseAtok)
	{
		pAtok = &g_Atok;
	}

	q.kb.type    = INPUTTYPE_KEYBOARD;
	q.kb.led     = led;
	q.kb.mkey    = mkey;
	q.kb.keycode = keycode;
	FifoPostData(&g_Fep.qKeyInput, &q, sizeof(INPUT_QUEUE_ELEM));
	return;
}


//============================================================================
//	interruptFromKey
//
//	キーボードからの入力があった時
//	ソフトウェアキーボード消す。
static void interruptFromKey(void)
{
	#ifndef ATOK_NO_LINK
	sceAtokContext *pAtok = NULL;

	if(g_Fep.ucUseAtok)
	{
		pAtok = &g_Atok;
	}
	else
	#endif
	{
		// ATOK使ってない
		g_pFepImport->fep.closeSoftKB();		// ソフトウェアキーボードクローズ
		return;
	}

	#ifndef ATOK_NO_LINK
	if(!sceAtokGetKanjiState(pAtok))
	{
		// モードをひらがなにする(KBDとSOFTKBで同期させない)
		sceAtokKanjiOn(pAtok);						// ATOKを漢字入力モードへ
		sceAtokFlushConverted(pAtok);				// 確定文字列バッファのフラッシュを行う
	}
	else
	{
		// USBキーボード入力以外での入力文字があったら無効にする
		if(sceAtokGetInputState(pAtok) != SCE_ATOK_ISTATE_BEFOREINPUT)
		{
			Fep_Cmd(pAtok, SCE_ATOK_EDIT_DELETEALL);
		}
	}
	Fep_Cmd(pAtok, EX_ATOK_DSPHIRAGANA);
	g_pFepImport->fep.closeSoftKB();		// ソフトウェアキーボードクローズ
	#endif
}



#ifndef ATOK_NO_LINK
//============================================================================
//	getContext
//
//	sceAtokContextから、表示のために必要な情報を取得する
//	ただし、ATOKエンジンの排他的使用権が得られなかった場合にはなにもしない。
//
//	戻り値 : int : 0:排他的使用権が得られなかった/1:必要な情報が取得できた
static int getContext(void)
{
	int i;
	int state;
	int focus=-1;

	if(g_Fep.ucUseAtok)
	{
		// ATOKエンジンの排他的使用権を確保を試みる
		i = PollSema(g_AtokRsrcSema);
		if(i<0)
		{
			// 排他的使用権が得られなかったとき、g_Fepの中身には手をつけずに終了
			return(0);
		}

		// ATOKの現在のON/OFF状態を得る
		g_Fep.mode = sceAtokGetKanjiState(&g_Atok);

		sceAtokGetConvertingStr(&g_Atok, (char*)g_Fep.szConv, HENKAN_SIZE);	// 変換中文字列をバッファにコピー

		// カーソルのあるバイト位置を取得しておく
		g_Fep.FocusTop = sceAtokGetFocusClauseTop(&g_Atok);
		if(g_Fep.FocusTop < 0)
		{
			g_Fep.FocusTop = 0;
		}

		// 変換中の文字列を分節ごとに分解しておく
		// まず変換中文字列の文節数を得る
		g_Fep.nClusters = sceAtokGetConvertingClauseCount(&g_Atok);
		for(i=0; i<g_Fep.nClusters; i++)
		{
			sceAtokGetConvertingClause(&g_Atok, g_Fep.aszClusters[i], HENKAN_SIZE, i);
		}
		// フォーカスのある文節番号を取得しておく
		g_Fep.FocusCls = sceAtokGetFocusClauseIndex( &g_Atok );

		// 現在の入力ステートを取得します
		state = sceAtokGetInputState(&g_Atok);
		if(state==SCE_ATOK_ISTATE_CANDIDATES ||
		   (g_Fep.FocusCls != focus && state!=SCE_ATOK_ISTATE_MOVECLAUSEGAP))
		{
			// 現在の候補番号を得る
			g_Fep.nCurrentCand = sceAtokGetCurrentCandidateIndex(&g_Atok);

			if(g_Fep.state != SCE_ATOK_ISTATE_CANDIDATES ||
			   (g_Fep.FocusCls != focus && state!=SCE_ATOK_ISTATE_MOVECLAUSEGAP))
			{
				// 今回、候補選択モードになったとき、候補リストを得る
				g_Fep.nCandidates = sceAtokGetCandidateList(&g_Atok, g_Fep.CandBuf, CANDLIST_SIZE);
				g_Fep.nCandOffset  = (g_Fep.nCurrentCand / 10) * 10;
			}

			// 文節の先頭、最後にジャンプして候補一覧を出す時
			if(g_Fep.stateCandLeftEnd == EX_ATOK_EDIT_CLSEND_CAND || g_Fep.stateCandLeftEnd == EX_ATOK_EDIT_CLSTOP_CAND)
			{
				// 今回、候補選択モードになったとき、候補リストを得る
				g_Fep.nCandidates = sceAtokGetCandidateList(&g_Atok, g_Fep.CandBuf, CANDLIST_SIZE);
				g_Fep.nCandOffset  = (g_Fep.nCurrentCand / 10) * 10;
				g_Fep.stateCandLeftEnd = EX_ATOK_EDIT_NULL;
			}
		}
		else if(state == SCE_ATOK_ISTATE_BEFOREINPUT)
		{
			g_Fep.nCurrentCand = -1;
			focus = -1;
			// FIXME : g_Fep.FocusClsも初期化すべき
		}
		else
		{
			// 候補選択モードではない場合
			g_Fep.nCurrentCand = -1;
		}

		g_Fep.state = state;
		focus = g_Fep.FocusCls;

		SignalSema(g_AtokRsrcSema);				// ATOKエンジンの使用権を解放する
	}

	return 1;
}
#endif



//============================================================================
//	getChar
//
//	FEPレイヤのもつ出力FIFOキューから変換済みキャラクタ情報を取得
//
//	COOKED_CHAR *pCc : 文字列を取得する為のバッファ
//	int fPeek : 0:キューから取り出す/1:キューから取り出さずに、先頭のデータを盗み見る
//
//	戻り値 : int : 0:成功/1:エラー(キューが溢れそう)
static int getChar(COOKED_CHAR *pCc, int fPeek)
{
	// FIFO内に確定文字列があった場合、FIFOから引き取り(WAITなし)
	return (FifoPollData(&g_Fep.qFepOuput, pCc, sizeof(COOKED_CHAR), fPeek));
}


//============================================================================
//	inputBackspace
//
//	[BACKSPACE]キーをバッファに反映
//
//	int pos : 現在のキャレット（カーソル）の位置
//
//	戻り値 : int : キャレット位置
static int inputBackspace(int pos)
{
	int oldPos, i;
	if(pos <= 0)
	{
		#if 1//SKB_EX Sound
		Fep_se_play(SKBSE_INHIBITINPUT);
		#endif
		return pos;
	}

	oldPos = pos;
	pos--;
	#if 1//SKB_EX Sound
	Fep_se_play(SKBSE_BACKSPACE);
	#endif
	// キャレットより後の文字を前に詰める
	i=0;
	while(g_pFepImport->fep.puiUCS4[oldPos+i] != 0x00000000)
	{
		g_pFepImport->fep.puiUCS4[pos+i] = g_pFepImport->fep.puiUCS4[oldPos+i];
		i++;
	}
	g_pFepImport->fep.puiUCS4[pos+i] = 0x0;

	g_Fep.nTextLen = pos + i;
	return pos;
}


//============================================================================
//	inputDelete
//
//	[DELETE]キーをバッファに反映
//
//	int pos : 現在のキャレット（カーソル）の位置
static void inputDelete(int pos)
{
	int i, j;
	i = g_Fep.nTextLen - pos;
	if(i > 0)
	{
		j = 1;
		memcpy(&g_pFepImport->fep.puiUCS4[pos],    &g_pFepImport->fep.puiUCS4[pos + j],    (size_t)((i - j)*sizeof(u_int)));
		g_Fep.nTextLen -= j;
		#if 1//SKB_EX Sound
		Fep_se_play(SKBSE_BACKSPACE);
		#endif
	}
	#if 1//SKB_EX Sound
	else
	{
		Fep_se_play(SKBSE_INHIBITINPUT);
	}
	#endif
}


//============================================================================
//	inputAllClear
//
//	入力されているデータをすべてクリア
//
//	int *pos : 現在のキャレット（カーソル）の位置のポインタ
static void inputAllClear(int *pos)
{
	*pos = 0;
	g_Fep.nCaretPos = 0;
	g_Fep.nTextLen = 0;									// 文字列長を初期化
	memset(g_pFepImport->fep.puiUCS4, 0, (g_pFepImport->fep.uiMaxTextLen*sizeof(u_int)));	// 文字列バッファをクリア
}


#ifndef ATOK_NO_LINK
//============================================================================
//	FepSetConfig
//
//	ATOKの動作設定
static int FepSetConfig()
{
	sceAtokConfig cfg;

	// atok.iniに記録されていない設定を、アプリケーションが望む値で初期化します
	{
		cfg.KanjiInput   = 1;		// 漢字入力モード
									// 1= ローマ字漢字
									// 2= かな入力漢字
									// 3= 半角スルー

		cfg.KanDspType   = 1;		// 入力文字種
									// 1= ひらがな
									// 2= 全角カタカナ
									// 3= 全角無変換
									// 4= 半角カタカナ
									// 5= 半角無変換

		cfg.FixMode      = 0;		// 固定入力モード
									// 0= OFF
									// 1= 固定ひらがな
									// 2= 固定全角カタカナ
									// 3= 固定全角無変換
									// 4= 固定半角カタカナ
									// 5= 固定半角無変換

		cfg.HZCnvMode    = 0x0D;	// 半角全角変換
									// bit 3: 後変換単語への適用
									// bit 2: ユーザー登録単語への適用
									// bit 1: 入力中文字列への適用
									// bit 0: 半角全角変換

		// 文字種類ごとに半角,全角への変換可否を制御します
		// 例えばbit0を0にしておくことで確定文字列内に半角カタカナが入ることを防げます
		// しかしながら、確定前の変換中文字列の中に半角カタカナが現れることはありえます
		cfg.HZCnvDetail  = 0x77;	// 半角全角変換詳細
									// bit 7: 常に0
									// bit 6: 数字→全角
									// bit 5: 英字→全角
									// bit 4: カタカナ→全角
									// bit 3: 常に0
									// bit 2: 数字→半角
									// bit 1: 英字→半角
									// bit 0: カタカナ→半角


		// 半角変換する(全角化の禁止)記号文字列
		cfg.HankakuCnvStr = "";			// 全角化を禁止する文字なし

		// 全角変換する(半角化の禁止)記号文字列
		cfg.ZenkakuCnvStr = "｡｢｣､･";	// 0xA0～0xA5の範囲にある記号について、半角化を禁止
											// 0xA6～0xA7の範囲(半角カナ)についてはHZCnvTypeのbit 0で制御できます

		cfg.FuriganaMode = 0;		// ふりがな入力モード
									// 0= 終了
									// 1= 開始

		cfg.MaxInputChars = 30;		// 入力文字数 default 30
									// 0-30  負の値や30を超えた範囲外の指定では、default 30で動作します。

		cfg.PutCursorMode = 0;		// 未確定文字列末尾にカーソル文字を追加する/しない
									// 0= しない
									// 1= する

		// NOTICE: 先読み解析をONにすると、入力中に辞書アクセスが発生してしまうので、
		// その間ローマ時変換が止まってしまう。
		// 先読み解析をOFFにすれば、文字入力中の画面フィードバックまでのラグが短くなる効果がありますが、
		// 変換キーを押すまで辞書アクセスがおこなわれないため、トータルの変換時間は長くなってしまいます
		cfg.PreAnalyze    = 0;		// 入力時の辞書先読みする/しない
									// 0= しない
									// 1= する
	}


	// 以下の設定は、_sysconfパーティションの/atok/atok.iniに設定が記録されているので、
	// sceAtokLoadConfig()関数によって適当な値が設定されています。
	// アプリケーションでオーバーライドすることも可能です。
	{
		cfg.ConvMode     = 0;		// 変換モード
									// 0= 連文節変換
									// 1= 単文節変換
									// 2= 自動変換

		cfg.LearningMode = 1;		// 学習モード
									// 0= 学習しない
									// 1= 学習する

		cfg.KutouCharSet = 1;		// 句読点組み合わせパターン
									// 0= ",.[]/ｰ"	(カンマ/ ピリオド/ 中括弧/   スラッシュ/ 音引き)
									// 1= "､｡｢｣･ｰ"	(読点  / 句点    / かぎ括弧/ 中黒      / 音引き)

		cfg.OkuriMode    = 0;		// 送りがなモード
									// 0= 本則
									// 1= 省く
									// 2= 送る

		cfg.AutoRegKind  = 0x0F;	// 自動登録種別
									// bit 3: 未登録語
									// bit 2: 後変換
									// bit 1: 複合語
									// bit 0: 文節区切り

		cfg.LearningKind = 0x0F;	// 学習種別
									// bit 3: 未登録語
									// bit 2: 後変換語
									// bit 1: 複合語学習語
									// bit 0: 区切り直し学習語

		// NOTICE: ローマ字立ち直り処理とは、ローマ字として認められないシーケンスが現れたとき
		// 再びローマ字変換をしようとする処理である。この処理をOFFにすると、以降はアルファベットのままとなる
		// 例えば[H][D][D]と入力したとき、処理あり→「ＨっＤ」, 処理なし→「HDD」となる
		cfg.RomanRecover = 1;		// ローマ字立ち直りする/しない
									// 0= しない
									// 1= する

		cfg.AutoRecMode  = 0;		// 入力支援する/しない
									// 0= しない
									// 1= する

		cfg.AutoRecRoman = 0;		// 入力支援(ローマ字入力時)の詳細設定
									// bit 2: [N]の過不足
									// bit 1: 子音の超過
									// bit 0: 母音の過不足

		cfg.AutoRecKana  = 0;		// 入力支援(かな入力時)の詳細設定
									// bit 2: 「わ」→「を」
									// bit 1: 「つ」→「っ」(「つ」を拗音に)
									// bit 0: 「゜」→「゛」(半濁点を濁点に)

		cfg.AutoRecRep   = 0;			// 入力支援(置換時)の詳細設定
									// bit 3: 「。」→「．」(句点をピリオドに)
									// bit 2: 「、」→「，」(読点をカンマに)
									// bit 1: 「・」→「／」(中黒をスラッシュに)
									// bit 0: 「ー」→「－」(音引きをハイフンに)

		cfg.AppendAtoHen = 0x13;	// 後変換候補の候補リストへの追加をする/しない
									// bit 4: 半角無変換候補
									// bit 3: 全角無変換候補
									// bit 2: 半角カタカナ候補
									// bit 1: 全角カタカナ候補
									// bit 0: ひらがな候補

		cfg.KutouCnvMode   = 0; 	// 句読点変換
									// 0= しない
									// 1= する

		cfg.KutouCnvDetail = 0x03;// 句読点変換の詳細設定
									// bit 3: ！(感嘆符)で変換
									// bit 2: ？(疑問符)で変換
									// bit 1: 、(読点)で変換
									// bit 0: 。(句点)で変換

		cfg.RuledZenCnv    = 1; 	// 規則指定全角変換
									// bit 0: [~]→[～]に変換
	}


	// ATOKエンジンのコンフィギュレーションを設定
	cfg.Size = sizeof(sceAtokConfig);
	sceAtokSetConfig(&g_Atok, &cfg, SCE_ATOK_CONFIG_KANJIINPUT      |
									SCE_ATOK_CONFIG_KANDSPTYPE      |
									SCE_ATOK_CONFIG_FIXMODE         |
									SCE_ATOK_CONFIG_HZCNVMODE       |
									SCE_ATOK_CONFIG_HZCNVDETAIL     |
									SCE_ATOK_CONFIG_HANKAKUCNVSTR   |
									SCE_ATOK_CONFIG_ZENKAKUCNVSTR   |
									SCE_ATOK_CONFIG_FURIGANAMODE    |
									SCE_ATOK_CONFIG_MAXINPUTCHARS   |
									SCE_ATOK_CONFIG_PUTCURSORMODE   |
									SCE_ATOK_CONFIG_PREANALYZE      |
									SCE_ATOK_CONFIG_CONVMODE        |
									SCE_ATOK_CONFIG_LEARNINGMODE    |
									SCE_ATOK_CONFIG_KUTOUCHARSET    |
									SCE_ATOK_CONFIG_OKURIMODE       |
									SCE_ATOK_CONFIG_AUTOREGKIND     |
									SCE_ATOK_CONFIG_LEARNINGKIND    |
									SCE_ATOK_CONFIG_ROMANRECOVER    |
									SCE_ATOK_CONFIG_AUTORECMODE     |
									SCE_ATOK_CONFIG_AUTORECROMAN    |
									SCE_ATOK_CONFIG_AUTORECKANA     |
									SCE_ATOK_CONFIG_AUTORECREP      |
									SCE_ATOK_CONFIG_APPENDATOHEN    |
									SCE_ATOK_CONFIG_KUTOUCNVMODE    |
									SCE_ATOK_CONFIG_KUTOUCNVDETAIL  |
									SCE_ATOK_CONFIG_RULEDZENCNV);

	// システム設定を読んで書き換えてください。
	g_Fep.Keybind       = g_pFepImport->usSystemIni[ENV_ATOK_BIND]; // キーバインドの設定
																	// 0 : IME風
																	// 1 : ATOK風

    // システム設定を使う
	g_Fep.InitialInput  = g_pFepImport->usSystemIni[ENV_ATOK_MODE]; // 初期入力モード
																	// 0 : ローマ字入力
																	// 1 : かな入力

	g_Fep.SpaceInput    = 2;// スペース入力モード
							// 0 : 絶対半角
							// 1 : スペース禁止
							// 2 : 半角スペース/全角スペース
	g_Fep.TenkeyInput   = 2;// テンキー入力モード
							// 0 : 初入力がテンキーからの場合、直接半角入力
							// 1 : 初入力がテンキーでＡＴＯＫの入力モードが半角の場合、直接半角入力
							// 2 : 常にＡＴＯＫへ入力

	g_Fep.Font1         = 0;//
	g_Fep.Font2         = 0;//
	return(0);
}
#endif


#if 1//SKB_EX Sound 
//============================================================================
//  Fep_se_play												ref:  libfep.h
//
//  効果音のコールバック関数を呼び出す
//
//  戻り値    : なし
void Fep_se_play(int sound_No)
{
	if( g_pFepImport->se )
	{
		g_pFepImport->se(sound_No);
	}
}

//============================================================================
//  Fep_atok_sound											ref:  libfep.h
//
//  ＡＴＯＫコマンド別に効果音のコールバック関数を呼び出す。
//
//  戻り値    : なし
void Fep_atok_sound(int f)
{
	#ifndef ATOK_NO_LINK
	int sound;

	switch(f)
	{
	case SCE_ATOK_EDIT_BACKSPACE: // 左文字1文字削除(BACKSPACE)
	                              sound=SKBSE_BACKSPACE;   break;
	case SCE_ATOK_EDIT_DELETE:    // カーソル位置の1文字削除(DELETE)
	                              sound=SKBSE_BACKSPACE;   break;
	case SCE_ATOK_EDIT_CURRIGHT:  // カーソル移動(右)
	                              sound=SKBSE_CARETMOVE;   break;
	case SCE_ATOK_EDIT_CURLEFT:	// カーソル移動(左)
	                              sound=SKBSE_CARETMOVE;   break;
	case SCE_ATOK_EDIT_CUREND:	// カーソル移動(文末)（↓キー押下時）
	                              sound=SKBSE_CARETMOVE;   break;
	case SCE_ATOK_EDIT_CURTOP:	// カーソル移動(文頭)（↑キー押下時）
	                              sound=SKBSE_CARETMOVE;   break;
	case SCE_ATOK_EDIT_1ONKAKUTEI: // 1音確定
	case SCE_ATOK_EDIT_POSTCONVHIRA:// 後変換:ひらがな
	case SCE_ATOK_EDIT_POSTCONVKANA:// 後変換:カタカナ
	case SCE_ATOK_EDIT_POSTCONVHALF:// 後変換:半角
	case SCE_ATOK_EDIT_POSTCONVRAW: // 後変換:無変換
	case SCE_ATOK_EDIT_DICCONV: 	 // 辞書指定変換
	case SCE_ATOK_EDIT_CONVFORWARD:// 変換、次候補順次取得（△ボタン押下時または↓キー押下時）
	                              sound=SKBSE_STARTTRANS;  break;
	case SCE_ATOK_EDIT_CONVBACKWARD:// 変換、次候補順次取得（↑キー押下時）
	                              sound=SKBSE_STARTTRANS;  break;
	case SCE_ATOK_EDIT_KAKUTEIPART:// 部分確定（○ボタン押下時）
	case SCE_ATOK_EDIT_KAKUTEIREP: // 確定リピート
	case SCE_ATOK_EDIT_KAKUTEIUNDO:// 確定アンドゥ
	case SCE_ATOK_EDIT_KAKUTEIHEAD:// 文頭文字確定
	case SCE_ATOK_EDIT_KAKUTEITAIL:// 文末文字確定
	                              sound=SKBSE_CANDDETECT;  break;
	case SCE_ATOK_EDIT_KAKUTEIALL:// 全文確定（STARTボタン押下時）
	                              sound=SKBSE_DETECTEXIT;  break;
	case SCE_ATOK_EDIT_DELETEALL: // 全文字削除（×押下時）
	case SCE_ATOK_EDIT_CONVCANCEL:// 変換キャンセル(注目文節以降)（×押下時）
	case SCE_ATOK_EDIT_CANCELALL:// 全変換取り消し
	                              sound=SKBSE_CANCEL;      break;
	case SCE_ATOK_EDIT_FOCUSRIGHT:// 注目文節区切りを1文字のばす（R1＋→ボタン押下時）
	                              sound=SKBSE_STRETCHCLS;  break;
	case SCE_ATOK_EDIT_FOCUSLEFT: // 注目文節区切りを1文字縮める（R1＋←ボタン押下時）
	                              sound=SKBSE_STRETCHCLS;  break;
	case SCE_ATOK_EDIT_MOVFOCUSCLAUS:// 注目文節移動
	                              sound=SKBSE_MOVECLS;     break;
	default:                      sound=-999;              break;
	}
	if(sound!=-999)  Fep_se_play(sound);
	#endif
}
#endif

#if 1//SKB_EX Insert 
//============================================================================
//  Fep_SetInsertMode											ref:  libfep.h
//
//  文字の挿入/上書モードを設定する
//
//  int Mode  : 設定モード(ref:  libfep.h)
//      FEP_EDIT_ADD				// 文字追加
//      FEP_EDIT_INSERT				// 文字挿入
//              
//  戻り値    : なし
void  Fep_SetInsertMode(int Mode)
{
	if(Mode == FEP_EDIT_ADD)			// 上書Mode 
	{
		g_Fep.iInsertMode = 0;
	}else	// それ以外はあえて全部挿入Modeにする※Default 挿入Mode
	{
		g_Fep.iInsertMode = 1;
	}
}
//============================================================================
//  Fep_GetInsertMode											ref:  libfep.h
//
//  文字の挿入/上書モードを獲得する
//
//              
//  戻り値    : 設定モード(ref:  libfep.h)
//      FEP_EDIT_ADD				// 文字追加
//      FEP_EDIT_INSERT				// 文字挿入
int  Fep_GetInsertMode(void)
{
	return(g_Fep.iInsertMode ? FEP_EDIT_INSERT : FEP_EDIT_ADD);	
}
#endif

#if 1//SKB_EX PowerOff 
//============================================================================
//  Fep_GetAtokSemaID											ref:  libfep.h
//
//  Atok セマフォＩＤ取得
//
//  戻り値 : int : セマフォＩＤ
int Fep_GetAtokSemaID()
{
	return g_AtokRsrcSema;
}
#endif
