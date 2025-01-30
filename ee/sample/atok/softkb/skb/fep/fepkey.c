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

// ハードウェアキーボードFEPレイヤ
// このfepkey.cの関数は、ATOKワーカスレッド内で実行されます
// キーコードのATOKコマンドへの変換が主要な目的です。
#include <sys/types.h>
#include <eekernel.h>
#include <libusbkb.h>
#include <stdio.h>
#include <libccc.h>

#include "libfep.h"
#include "fepkey.inc"

#ifndef ATOK_NO_LINK
#include "vje.inc"								// VJE 風キーバインド定義
#include "atok.inc"								// ATOK風キーバインド定義
#include "ime.inc"								// IME 風キーバインド定義
#endif

#define SUPPORT_OVERSEAUSBKB	(0)
#define SUPPORT_OVERSEALIBCCC	(0)


extern gImport_t		*g_pFepImport;
extern fep_t			g_Fep;

#if 1//SKB_EX Sound
extern void Fep_atok_sound(int f);
#endif

static void ascii2UCS4(u_int *ucs4, u_short keycode);
#if SUPPORT_OVERSEAUSBKB
static int thinkAltGr(int keybdType, u_short usCharCode, u_char mkey, u_short rawcode);
#endif

static void FepTreatKeyDirect(u_char led, u_char mkey, u_short rawcode);
#ifndef ATOK_NO_LINK
static int  FepTreatKeyAtok(sceAtokContext *pAtok, u_char led, u_char mkey, u_short rawcode);
static const FEP_KEYBIND *FepSearchBind(u_char led, u_char mkey, u_short rawcode);

// キーバインド定義
static const struct
{
	const FEP_KEYBIND *pBind;
	int nKeyBinds;
} g_BindTbl[] =
{
	{ImeKeyTbl,		(sizeof(ImeKeyTbl) / sizeof(FEP_KEYBIND))},		// CHECK :
	{AtokKeyTbl,	(sizeof(AtokKeyTbl)/ sizeof(FEP_KEYBIND))},
};
#endif

static int idiomFlg = 0;		// 発音記号をFEPルーチンに渡すかフラグ


//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//	FepTreatKey                                                ref: libfep.c
//
//	各キーコードにそった処理
//
//	sceAtokContext *pAtok : ATOKへのコンテキストポインタ
//	u_char led  : LEDの状態
//	u_char mkey : モディファイアキーの状態
//	u_short rawcode : USBキーボードから入力されたRAWコード
void FepTreatKey(sceAtokContext *pAtok, u_char led, u_char mkey, u_short rawcode)
{
	#if 1//SKB_EX F12
	if(rawcode == USBKEYC_F12)
	{
		g_pFepImport->fep.changeMode(-1); // Select or FunctionKeyによるAtokModeの切り換え
		return;
	}
	#endif

	if(pAtok == NULL)
	{
		// ATOK サポートしてない時の処理
		FepTreatKeyDirect(led, mkey, rawcode);
		return;
	}

	#ifndef ATOK_NO_LINK
	// キーコードを処理する
	if(sceAtokGetKanjiState(pAtok))
	{
		int i;
		// ATOKがONのとき

		// FEP構造体内に保存された かなモードフラグを使う
		if(g_Fep.led & USBKB_LED_KANA)
		{
			led |= USBKB_LED_KANA;
		}
		else
		{
			led &= ~USBKB_LED_KANA;
		}

		// ATOKエンジンにコマンドを投げる場合
		i = FepTreatKeyAtok(pAtok, led, mkey, rawcode);
		if(i)
		{
			// デッドキャラクタとして扱う場合
			// 出力FIFOにUSBキーコードをそのまま格納する
			COOKED_CHAR c;
			c.Char    = 0xFFFFFFFF;
			c.Flag    = mkey;
			c.RawCode = 0x8000 | rawcode;
			FifoSendData(&g_Fep.qFepOuput, &c, sizeof(COOKED_CHAR));
		}
	}
	else
	{
		// ATOKがOFFのとき
		// ATOKエンジンを漢字入力モードにするためのキーコードをチェックする
		if((g_pFepImport->usSystemIni[ENV_KEYBD_TYPE] == USBKB_ARRANGEMENT_106 && rawcode==USBKEYC_106_KANJI )  // 106の全半と101の`~ は
			|| (g_pFepImport->usSystemIni[ENV_KEYBD_TYPE] == USBKB_ARRANGEMENT_101 && rawcode==USBKEYC_106_KANJI ))  // 同じrawcodeが返ってくるみたい
		{
			// [SHIFT],[CTRL],[Windows]キーが押されておらず、[ALT]+[半角/全角]が入力されたとき
			sceAtokKanjiOn(pAtok);                              // ATOKを漢字入力モードへ
			sceAtokFlushConverted(pAtok);                       // 確定文字列バッファのフラッシュを行う

			g_pFepImport->fep.atokON();
			#if 0//SKB_EX Default   デフォルトが、ひらがなとは限らないので、削除。atokON内で、セットする。
			Fep_Cmd(pAtok, EX_ATOK_FIXOFF);
			g_pFepImport->fep.changeMode(EX_ATOK_DSPHIRAGANA);  // 最小バーと同期させる為にコールバックを呼ぶ
			Fep_SetInputMode(FEPON_HIRA);                       // 強制的にひらがなモード
			#endif
		}
		else
		{
			// ATOKエンジンにコマンドを投げず、直接入力した場合
			FepTreatKeyDirect(led, mkey, rawcode);
		}
	}
	#endif
	return;
}


//============================================================================
//	FepKeyInitIdiomFlag
//
//	発音記号をFEPルーチンに渡すかフラグ
//	この関数は、海外版キーボードを使用するにあたって
//	キーボードタイプを切り替える部分で必要となります。
void FepKeyInitIdiomFlag(void)
{
	idiomFlg = 0;
}


//============================================================================
// STATIC FUNCTIONS
//============================================================================
//============================================================================
//	ascii2UCS4
//
//	アスキーコードからUCS2(UCS4)に変換
//
//	u_int *ucs4 : 変換後の文字コード格納用バッファ
//	u_short keycode : 変換前の文字コード
static void ascii2UCS4(u_int *ucs4, u_short keycode)
{
	sceCccUCS4 chr=0;
#if SUPPORT_OVERSEALIBCCC
	if(g_pFepImport->iRegion == REGION_J)
#endif
	{
		// 日本向け
		sceCccJISCS  jis;
		sceCccSJISTF aSjis[2];
		sceCccSJISTF const* pSjis = aSjis;
		aSjis[0] = keycode;
		aSjis[1] = 0;
		jis = sceCccDecodeSJIS(&pSjis);
		*ucs4 = sceCccJIStoUCS(jis, chr);
	}
#if SUPPORT_OVERSEALIBCCC
	else
	{
		// 海外版
		*ucs4 = sceCccLatin1toUCS((u_char)keycode, chr);
	}
#endif
}


#ifndef ATOK_NO_LINK
//============================================================================
//	FepSearchBind
//
//	キーバインド情報の検索サブルーチン
//
//	u_char led  : LEDの状態
//	u_char mkey : モディファイアキーの状態
//	u_short rawcode : USBキーボードから入力されたRAWコード
//
//	戻り値 : const FEP_KEYBIND * : キーバインドテーブルへのポインタ
static const FEP_KEYBIND *FepSearchBind(u_char led, u_char mkey, u_short rawcode)
{
	const FEP_KEYBIND *pBindList;
	int nKeyBinds;
	u_short usCharCode;
	u_int c1, c2, c3;
	int i;

	switch(g_pFepImport->usSystemIni[ENV_ATOK_BIND])
	{
	case KEYBIND_TYPE1:
		pBindList = g_BindTbl[0].pBind;
		nKeyBinds = g_BindTbl[0].nKeyBinds;
		break;

	case KEYBIND_TYPE2:
	default:
		pBindList = g_BindTbl[1].pBind;
		nKeyBinds = g_BindTbl[1].nKeyBinds;
		break;
	}

	// LED状態,モディファイアキー状態をOFFにしてキーコードを検索
	switch(g_pFepImport->usSystemIni[ENV_KEYBD_TYPE])
	{
	case USBKB_ARRANGEMENT_101:
		usCharCode = sceUsbKbCnvRawCode(USBKB_ARRANGEMENT_101, mkey, (led & ~USBKB_LED_KANA), rawcode);
		break;
	case USBKB_ARRANGEMENT_106:
	default:
		usCharCode = sceUsbKbCnvRawCode(USBKB_ARRANGEMENT_106, mkey, (led & ~USBKB_LED_KANA), rawcode);
		break;
	}

	c1 = usCharCode;
	if(mkey & (USBKB_MKEY_L_SHIFT | USBKB_MKEY_R_SHIFT)) {
		c1 |= SHIFT;
	}
	if(mkey & (USBKB_MKEY_L_CTRL  | USBKB_MKEY_R_CTRL)) {
		c1 |= CTRL;
	}
	if(mkey & (USBKB_MKEY_L_ALT   | USBKB_MKEY_R_ALT)) {
		c1 |= ALT;
	}
	// [a]～[z]を[A]～[Z]に変換する
	if(('a'<=(c1 & 0x0000BFFF) && (c1 & 0x0000BFFF)<='z')) {
		c2 = c1 & 0xFFFFFFDFUL;
	} else {
		c2 = c1;
	}
	for(i=0; i<nKeyBinds; i++) {
		c3 = pBindList[i].code;
		if((c3 & USBKB_KEYPAD) && (c3==c2)) {
			// テンキー側でのみ有効なコードが見つかった場合
			return(&pBindList[i]);
		} else if(c3==(c2 & ~USBKB_KEYPAD)) {
			// 該当するキーコードが見つかった場合(フルキー,テンキー問わず)
			return(&pBindList[i]);
		}
	}
	return(NULL);
}
#endif


//============================================================================
//	FepTreatKeyDirect
//
//	ATOKがOFFの状態(直接入力された時)の処理
//
//	u_char led  : LEDの状態
//	u_char mkey : モディファイアキーの状態
//	u_short rawcode : USBキーボードから入力されたRAWコード
static void FepTreatKeyDirect(u_char led, u_char mkey, u_short rawcode)
{
	COOKED_CHAR c;
	u_short usCharCode = 0;
	u_char  ucMask = 0;
	u_int keybdType = USBKB_ARRANGEMENT_101;

	// 通常のキーコード変換
	if(g_pFepImport->iRegion == REGION_J)
	{
		// 日本
		switch(g_pFepImport->usSystemIni[ENV_KEYBD_TYPE])
		{
		case USBKB_ARRANGEMENT_101:
			usCharCode = sceUsbKbCnvRawCode(USBKB_ARRANGEMENT_101, mkey, led, rawcode);
			break;
		case USBKB_ARRANGEMENT_106:
		default:
			if(led & USBKB_LED_KANA)
			{
				usCharCode = sceUsbKbCnvRawCode(USBKB_ARRANGEMENT_106_KANA, mkey, led, rawcode);
			}
			else
			{
				usCharCode = sceUsbKbCnvRawCode(USBKB_ARRANGEMENT_106, mkey, led, rawcode);
			}
			break;
		}
	}
	else
	{
		u_int type = g_pFepImport->usSystemIni[ENV_KEYBD_TYPE];
		if(type == USBKB_ARRANGEMENT_101)
		{
			// US
			keybdType = USBKB_ARRANGEMENT_101;
		}
#if SUPPORT_OVERSEAUSBKB
		else
		{
			// 海外
			if(USBKB_ARRANGEMENT_FRENCH <= type)
			{
				keybdType = type+1;		// USBキーボードライブラリとキーボード設定は
										// USBKB_ARRANGEMENT_106_KANAがあるので+1ずつずれる
				if(keybdType > USBKB_ARRANGEMENT_FRENCH_CA)
				{
					keybdType = USBKB_ARRANGEMENT_101;
				}
			}
		}
#endif
		usCharCode = sceUsbKbCnvRawCode(keybdType, mkey, led, rawcode);
		#if 0 //SKB_EX Cut
		g_pFepImport->dprintf("RAWCODE --- %d ---, %x, %x, %x\n", __LINE__, usCharCode, rawcode, mkey);
		#endif
	}

	if((led & USBKB_LED_CAPS_LOCK) && (mkey & (USBKB_MKEY_L_SHIFT | USBKB_MKEY_R_SHIFT))
	   && !(mkey & (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL | USBKB_MKEY_L_ALT | USBKB_MKEY_R_ALT | USBKB_MKEY_L_WIN | USBKB_MKEY_R_WIN)))
	{
		// CAPSLOCK+SHIFT -> 小文字
		if(usCharCode>=0x41 && usCharCode<=0x5A)
		{
			usCharCode += 0x0020;
		}
	}

	if(((usCharCode & (USBKB_RAWDAT|USBKB_KEYPAD))==USBKB_KEYPAD)
	   && !(mkey & (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL | USBKB_MKEY_L_ALT | USBKB_MKEY_R_ALT | USBKB_MKEY_L_WIN | USBKB_MKEY_R_WIN)))
	{
		// テンキー側からの入力
		c.Char    = usCharCode & ~(USBKB_RAWDAT|USBKB_KEYPAD);
		c.Flag    = mkey;
		c.RawCode = rawcode;
		// キー入力をFEP出力FIFOに置く(WAITあり)
		FifoSendData(&g_Fep.qFepOuput, &c, sizeof(COOKED_CHAR));
		return;
	}

#if 0
	// ALT＋アルファベットでアルファベットが入力できてしまうのを修正したバージョン
	if(keybdType == USBKB_ARRANGEMENT_101 || keybdType == USBKB_ARRANGEMENT_106)
	{
		// アメリカなので右ALTは有効
		ucMask = (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL | USBKB_MKEY_L_ALT | USBKB_MKEY_R_ALT | USBKB_MKEY_L_WIN | USBKB_MKEY_R_WIN);
	}
#if SUPPORT_OVERSEAUSBKB
	else
	{
		// ヨーロッパなので右ALTはALTGrになる
		ucMask = (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL | USBKB_MKEY_L_ALT | USBKB_MKEY_L_WIN | USBKB_MKEY_R_WIN);
	}
#endif
#else
	// 両方のAltキー押しながらでもアルファベットが出る
	ucMask = USBKB_MKEY_L_WIN | USBKB_MKEY_R_WIN;
#endif

	if(!(usCharCode & (USBKB_RAWDAT|USBKB_KEYPAD)) && !(mkey & ucMask))
	{
		u_int ucs4;
		ascii2UCS4(&ucs4, usCharCode);

		c.Char    = ucs4;		// usCharCode : これは、後の判定に必要なので操作しない
		c.Flag    = mkey;
		c.RawCode = rawcode;
	}
	else
	{
		usCharCode = 0;
		c.Char    = 0xFFFFFFFF;
		c.Flag    = mkey;
		c.RawCode = rawcode;
	}


	if(usCharCode==0) {
		// デッドキャラクタ
		c.Char = 0xFFFFFFFF;
		c.RawCode |= 0x8000;
	} else if(!(usCharCode & USBKB_RAWDAT)) {
		// ASCIIコードに変換できた文字だけ処理
		if(mkey & (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL)) {
			// [CTRL]キーが押されていたとき
			if(usCharCode>=0x40 && usCharCode<=0x7F) {
				// [CTRL]+[@]/[`]を0x00に変換
				// [CTRL]+[A]～[Z]/[a]～[z]を0x01～0x1Aに変換
				// [CTRL]+[[]/[{]を0x1Bに変換
				// [CTRL]+[\]/[|]を0x1Cに変換
				// [CTRL]+[]]/[}]を0x1Dに変換
				// [CTRL]+[^]/[~]を0x1Eに変換
				// [CTRL]+[_]/[]を0x1Fに変換
				c.Char = usCharCode & 0x0000001F;
			} else {
				// デッドキャラクタ
				c.Char = 0xFFFFFFFF;
				c.RawCode |= 0x8000;
			}
		}
	} else {
		// ASCIIコードに変換できなかった文字
		#ifndef ATOK_NO_LINK
		if(g_Fep.ucUseAtok)
		{
			const FEP_KEYBIND *pBind = FepSearchBind(led, mkey, rawcode);
			int f;
			if(pBind==NULL) {
				// キーバインドが見つからなかった場合
				// デッドキャラクタ
				c.Char = 0xFFFFFFFF;
				c.RawCode |= 0x8000;
			}
			else
			{
				// 直接入力時のファンクションを取得する
				f = pBind->func[0];
	
				if(f & RAW_CODE) {
					c.Char = f;
				} else if(f==0 || f>=0x100) {
					// キーバインド未定義だった場合、デッドキャラクタ
					c.Char = 0xFFFFFFFF;
					c.RawCode |= 0x8000;
				} else {
					// 通常のキーコードがあったとき
					c.Char = f;
				}
			}
		}
		else
		#endif
		{
			// デッドキャラクタ
			c.Char = 0xFFFFFFFF;
			c.RawCode |= 0x8000;
		}
	}


	if(keybdType == USBKB_ARRANGEMENT_101 || keybdType == USBKB_ARRANGEMENT_106)
	{
		// アメリカなので右ALTは有効
		ucMask = (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL | USBKB_MKEY_L_ALT | USBKB_MKEY_R_ALT | USBKB_MKEY_L_WIN | USBKB_MKEY_R_WIN);
	}
#if SUPPORT_OVERSEAUSBKB
	else
	{
		// ヨーロッパなので右ALTはALTGrになる
		ucMask = (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL | USBKB_MKEY_L_ALT | USBKB_MKEY_L_WIN | USBKB_MKEY_R_WIN);
	}

	// 海外版のみチェック
	// 以下の処理はAltGrキーのみに対応します。
	if(keybdType >= USBKB_ARRANGEMENT_FRENCH  && usCharCode != 0 && !(mkey & ucMask))
	{
		thinkAltGr(keybdType, usCharCode, mkey, rawcode);
		return;
	}
#endif

	// キー入力をFEP出力FIFOに置く(WAITあり)
	FifoSendData(&g_Fep.qFepOuput, &c, sizeof(COOKED_CHAR));
}


#if SUPPORT_OVERSEAUSBKB
//============================================================================
//	thinkAltGr
//
//	AltGrキーがきた時の処理
//
//	int keybdType : USBキーボードのタイプ
//	u_short usCharCode : 文字コード
//	u_char mkey : モディファイアキーの状態
//	u_short rawcode : USBキーボードRAWコード
static void thinkAltGr(int keybdType, u_short usCharCode, u_char mkey, u_short rawcode)
{
	COOKED_CHAR c;
	static int idiomIndex = 0;
	static 	COOKED_CHAR old_char = {0x0, 0x0, 0x0};
	u_int chLangTbl=keybdType-USBKB_ARRANGEMENT_FRENCH;
	int		i;

	if((keybdType == USBKB_ARRANGEMENT_FRENCH) && (rawcode ==0x0026))
	{
		// フランス語の時に[AltGr+9=^]は"^"がそのまま出る
		// FIXME : 他の発音記号(ex : "~")＋[AltGr+9=^] = ~^ ←となる
		FifoSendData(&g_Fep.qFepOuput, &c, sizeof(COOKED_CHAR));
		return;
	}
	if((keybdType == USBKB_ARRANGEMENT_ITALIAN) && (rawcode == 0x0026))
	{
		// イタリア語の時には"^"がそのまま出る
		FifoSendData(&g_Fep.qFepOuput, &c, sizeof(COOKED_CHAR));
		return;
	}

	// 発音記号が来たらFIFOに送らず次の入力文字を見て判断
	if(idiomFlg == 0)
	{
		for(i=0; i<6; i++)
		{
			if(usCharCode == g_idiomTbl[i].idiomCode && g_idiomTbl[i].keyNum[chLangTbl] != 0)
			{
				// 発音記号をチェック
				// 発音記号が入力されていたら一旦退避
				old_char.Char    = usCharCode;
				old_char.Flag    = mkey;
				old_char.RawCode = rawcode;
				idiomFlg = 1;
				idiomIndex = i;
				return;
			}
		}
	}
	else
	{
		idiom_t *pTbl = &g_idiomTbl[idiomIndex];
		exchange_table_t *pExTbl = pTbl->key[chLangTbl];
		int chenged = 0;

		if(rawcode == 0x2cU)
		{
			// スペースが来たら発音記号をFIFOに送信
			c = old_char;
			idiomFlg = 0;
		}
		else
		{
			// 変換テーブルにて発音記号＋アルファベットに変換
			for(i=0; i<pTbl->keyNum[chLangTbl]; i++)
			{
				if(usCharCode == pExTbl[i].nowCode)
				{
					c.Char = pExTbl[i].outCode;
					c.RawCode = rawcode;
					c.Flag    = mkey;
					chenged = 1;				// 変換できた
				}
			}
			idiomFlg = 0;

			if(!chenged)
			{
				// 変換できなかった場合は、発音記号＋アルファベットをFIFOに送信
				FifoSendData(&g_Fep.qFepOuput, &old_char, sizeof(COOKED_CHAR));
			}
		}
	}
}
#endif


#ifndef ATOK_NO_LINK
//============================================================================
//	FepTreatKeyAtok
//
//	各キーコードにそった処理
//	ATOKがONのとき(コマンドに変換)
//	戻り値を1とすることで、デッドキャラクタであることを通知する
//
//	sceAtokContext *pAtok : ATOKへのコンテキストポインタ
//	u_char led  : LEDの状態
//	u_char mkey : モディファイアキーの状態
//	u_short rawcode : USBキーボードRAWコード
//
//	戻り値 : int : 0:コントロールコードとして扱う/1:デッドキャラクタとして扱う
static int FepTreatKeyAtok(sceAtokContext *pAtok, u_char led, u_char mkey, u_short rawcode)
{
	u_short usCharCode;
	int s, f = 0;
	// 通常のキーコード変換
	switch(g_pFepImport->usSystemIni[ENV_KEYBD_TYPE])
	{
	case USBKB_ARRANGEMENT_101:
		usCharCode = sceUsbKbCnvRawCode(USBKB_ARRANGEMENT_101, mkey, led, rawcode);
		break;
	case USBKB_ARRANGEMENT_106:
	default:
		if(led & USBKB_LED_KANA)
		{
			usCharCode = sceUsbKbCnvRawCode(USBKB_ARRANGEMENT_106_KANA, mkey, led, rawcode);
		}
		else
		{
			usCharCode = sceUsbKbCnvRawCode(USBKB_ARRANGEMENT_106, mkey, led, rawcode);
		}
		break;
	}

	// ATOKの入力ステートを得る
	s = sceAtokGetInputState(pAtok);

	if(usCharCode==0)
	{
		if(s==SCE_ATOK_ISTATE_MOVECLAUSEGAP &&
		   !(mkey & (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL)) &&
		   0x21<=rawcode && rawcode>=0x7e)
		{
			// 文節区切り変更中に次のキーがきた場合
			sceAtokEditConv(pAtok, SCE_ATOK_EDIT_KAKUTEIALL, 0);
			Fep_SetData2Fifo();
		}

		// デッドキャラクタ
		return(1);
	}

	// 101キーのときの ATOK OFF を、例外的に処理
	if((g_pFepImport->usSystemIni[ENV_KEYBD_TYPE] == USBKB_ARRANGEMENT_101) &&
	   (mkey & (USBKB_MKEY_L_ALT | USBKB_MKEY_R_ALT)) &&
	   (rawcode == 0x35))
	{
		Fep_Cmd( pAtok , EX_ATOK_EDIT_KANJIOFF);
		return 0;
	}
	else
	{
		if(s==SCE_ATOK_ISTATE_BEFOREINPUT && usCharCode==' ')
		{
			// 未入力状態でスペースが来たとき
			f = ' ';
			if(g_Fep.SpaceInput==0 || (g_Fep.SpaceInput==2 && g_Fep.KanDspType>=4))
			{
				// 常に半角、または半角カタカナ,半角英数
				f |= RAW_CODE;
			}
		}
		else if(s==SCE_ATOK_ISTATE_BEFOREINPUT && (usCharCode & (USBKB_RAWDAT|USBKB_KEYPAD))==USBKB_KEYPAD)
		{
			// 未入力状態でテンキー由来のキャラクタが来たとき
			if(g_Fep.TenkeyInput==0 || (g_Fep.TenkeyInput==2 && g_Fep.KanDspType>=4))
			{
				// 常に半角、または半角カタカナ,半角英数
				f = (usCharCode & 0xFF);
			}
		}
		else
		{
			if(!(mkey & (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL)) &&
				(((0x21<=(usCharCode & ~USBKB_KEYPAD) && (usCharCode & ~USBKB_KEYPAD)<=0x7E)) || ((0xA0<=usCharCode) && (usCharCode<=0xDF))) &&
				!('0'<=(usCharCode & ~USBKB_KEYPAD) && (usCharCode & ~USBKB_KEYPAD)<='9') &&
				!((0x402C<=usCharCode) && (usCharCode<=0x402F)))
			{
	
				// [CTRL]キーは押されていない、
				// 普通文字(0x21～0x7E)またはテンキー側の文字(USBKB_KEYPADがORされている)
				// ただし、[0]～[9]とテンキー側の[,][-][.][/]は除外してある
				if(s==SCE_ATOK_ISTATE_MOVECLAUSEGAP) {
					// 文節区切り変更中に次のキーがきた場合、確定させる
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_KAKUTEIALL, 0);
					Fep_SetData2Fifo();
				}
				sceAtokEditConv(pAtok, (int)usCharCode, 0);
				return(0);
			}
			// キーバインドリストからサーチ
			f = 0;
		}
	}

	if(f==0)
	{
		// まず、キーテーブルから該当するキーコードがないかテーブルから検索
		const FEP_KEYBIND *pBind = FepSearchBind(led, mkey, rawcode);
		if(pBind==NULL)
		{
			// 該当するキーバインドが見つからなかった場合
			// デッドキャラクタとして扱う
			g_pFepImport->dprintf("/Unknown code %d: %02X:%02X:%04X\n", __LINE__, led, mkey, usCharCode);
			return(1);
		}

		// ATOKの入力ステートに応じたATOKのコマンドを得る
		switch(s)
		{
		case SCE_ATOK_ISTATE_BEFOREINPUT:		// 未入力
			f = pBind->func[0];
			break;

		case SCE_ATOK_ISTATE_BEFORECONVERT:		// 変換前
			f = pBind->func[1];
			break;

		case SCE_ATOK_ISTATE_CONVERTING:		// 変換中
		case SCE_ATOK_ISTATE_CANDEMPTY:			// 候補切れ(候補なし)
			f = pBind->func[2];
			break;

		case SCE_ATOK_ISTATE_CANDIDATES:		// 候補一覧
			f = pBind->func[3];
			break;

		case SCE_ATOK_ISTATE_MOVECLAUSEGAP:		// 文節区切り直し
			f = pBind->func[4];
			break;

		case SCE_ATOK_ISTATE_POSTCONVKANA:		// 後変換(カタカナ)
		case SCE_ATOK_ISTATE_POSTCONVHALF:		// 後変換(半角)
		case SCE_ATOK_ISTATE_POSTCONVRAW:		// 後変換(無変換)
			f = pBind->func[5];
			break;

		default:
			// 不明なステート
			f = 0;
			break;
		}
		if(f==0)
		{
			// デッドキャラクタとして扱う
			return(1);
		}
	}

	if(s > SCE_ATOK_ISTATE_BEFOREINPUT &&
		(f >= SCE_ATOK_EDIT_CONVFORWARD && f <= SCE_ATOK_EDIT_POSTCONVRAW))
	{
		g_Fep.nIsConv = CONV_ON;			// 変換中フラグ設定
	}

	if(f & RAW_CODE)
	{
		// コントロールコードとして扱う
		COOKED_CHAR c;
		c.Char    = f & ~RAW_CODE;
		c.Flag    = mkey;
		c.RawCode = rawcode;
		FifoSendData(&g_Fep.qFepOuput, &c, sizeof(COOKED_CHAR));
		return(0);
	}
	else
	{
		int res;
		#if 1//SKB_EX Sound
		Fep_atok_sound(f);
		#endif
		res = Fep_Cmd(pAtok ,f);
		if(res)
		{
			// コマンドの定義エラー
			// CAUTION: デッドキャラクタとしては扱わない。
			g_pFepImport->dprintf("Unknown function: %02X:%02X:%04X %d\n", led, mkey, usCharCode, f);
		}
	}
	return(0);
}
#endif
