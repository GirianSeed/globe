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

// ソフトウェアキーボードFEPレイヤ
// このfepskb.cの関数は、ATOKワーカスレッド内で実行されます
// キーコードのATOKコマンドへの変換が主要な目的です。
#include <sys/types.h>
#include <eekernel.h>
#include <stdio.h>
#include <string.h>

#include "libfep.h"
#include "fepskb.inc"

extern gImport_t		*g_pFepImport;

extern fep_t			g_Fep;

#ifndef ATOK_NO_LINK
static const struct
{
	const FEP_KEYBIND *pBind;
	int   nKeyBinds;
} g_BindTbl[] =
{
	{skbImeKeyTbl,		(sizeof(skbImeKeyTbl)  / sizeof(FEP_KEYBIND))},
	{skbAtokKeyTbl,		(sizeof(skbAtokKeyTbl) / sizeof(FEP_KEYBIND))},
};
#endif

extern unsigned int CharZenToHan(unsigned int sjis);

#ifndef ATOK_NO_LINK
static const FEP_KEYBIND *FepSearchBindSkb(u_char mkey, u_short ctrlCode);
static void sendKeyAtok(sceAtokContext *pAtok, const u_int *p);
static int  FepTreatSkbAtok(sceAtokContext *pAtok, u_short ctrlCode, u_char flag, const u_int *pCharCode);
#endif
static int  FepTreatKeyDirectSkb(u_short ctrlCode, u_char flag, const u_int *pCharCode);
static int thinkDakuHandaku(const u_int *D);
#if 1//SKB_EX Sound
extern void Fep_atok_sound(int f);
#endif

typedef struct
{
	u_char  from[16];
	u_char to[16];
} specialSign_t;

// 全角を半角にする(ATOKエンジンに入る前にUCS4からS-JISに変換される為このままコードを書く)
specialSign_t g_SpecialSign [] =
{
	{"「", "｢"},
	{"」", "｣"},
	{"（", "("},
	{"）", ")"},
	{"゛", "ﾞ"},
	{"゜", "ﾟ"},
	{"・", "･"},
	{"：", ":"},
	{"～", "~"},
	{"ー", "ｰ"},
	
	{"？", "?"},
	{"！", "!"},
	{"、", "､"},
	{"。", "｡"},

	{"　", " "},
};


//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//	FepTreatSkb                                                ref: libfep.c
//
//	ソフトウェアキーボードコア処理
//
//	sceAtokContext *pAtok : ATOKへのコンテキストポインタ
//	u_short ctrlCode : 特殊キー(USBキーコードが入る)
//	u_char flag : SHIFTが押されているか？
//	const u_int *pCharCode : 文字列へのポインタ
void FepTreatSkb(sceAtokContext *pAtok, u_short ctrlCode, u_char flag, const u_int *pCharCode)
{
	// ATOK サポートしてない時の処理
	if(pAtok == NULL)
	{
		FepTreatKeyDirectSkb(ctrlCode, flag, pCharCode);
		return;
	}

	#ifndef ATOK_NO_LINK
	if(sceAtokGetKanjiState(pAtok))
	{
		// ATOKがONのとき || ATOK サポートしている時の処理
		// ATOKの入力ステートを得る
		int i=0;
		if((Fep_AtokGetInputState() == SCE_ATOK_ISTATE_BEFOREINPUT) && pCharCode[0] == 0x00000020)
		{
			// スペースは特別
			#if 1//SKB_EX Sound
			Fep_se_play(SKBSE_STARTTRANS);  
			#endif
			FepTreatKeyDirectSkb(ctrlCode, flag, pCharCode);
		}
		#if 1//SKB_EX Sound
		if((Fep_AtokGetInputState() == 0) && pCharCode[0] == 0x00003000)
		{
			// スペース入力
			Fep_se_play(SKBSE_ONSPACE);  
		}
		#endif

		i = FepTreatSkbAtok(pAtok, ctrlCode, flag, pCharCode);
		if(i)
		{
			// デッドキャラクタとして扱う場合
			// 出力FIFOにUSBキーコードをそのまま格納する
			COOKED_CHAR c;
			c.Char    = 0xFFFFFFFF;
			c.Flag    = flag;
			c.RawCode = 0x8000 | ctrlCode;
			FifoSendData(&g_Fep.qFepOuput, &c, sizeof(COOKED_CHAR));
		}
	}
	else
	{
		// ATOKがOFFのとき
		FepTreatKeyDirectSkb(ctrlCode, flag, pCharCode);
	}
	#endif
}


//============================================================================
// STATIC FUNCTIONS
//============================================================================
//============================================================================
//	FepTreatKeyDirectSkb										ref: libfep.c
//
//	u_short ctrlCode : 特殊キー(USBキーコードが入る)
//	u_char flag : SHIFTが押されているか？
//	const u_int *pCharCode : 文字列へのポインタ
//
//	戻り値 : int : 常に0
static int FepTreatKeyDirectSkb(u_short ctrlCode, u_char flag, const u_int *pCharCode)
{
	COOKED_CHAR c;

	// そのままキーコードをポスト
	if(ctrlCode & 0x8000)
	{
		c.Char    = 0xFFFFFFFF;
		c.Flag    = 0;
		c.RawCode = 0x8000 | ctrlCode;
		FifoSendData(&g_Fep.qFepOuput, &c, sizeof(COOKED_CHAR));
	}
	else
	{
		#if 1//SKB_EX Sound
		if( g_pFepImport->fep.changeText( Fep_GetInsertMode(), pCharCode ,0 ) == FEP_EDIT_OK )
		{
			Fep_se_play(SKBSE_ONKEY);
		}
		#endif
		// TODO:
		// 現状、海外版の発音記号付き文字等は考慮されていない
		if(thinkDakuHandaku(pCharCode))
		{
			return 0;
		}

		while(*pCharCode)
		{
			c.Flag    = 0x00;
			c.RawCode = 0x0000;
			c.Char    = *pCharCode++;
			FifoSendData(&g_Fep.qFepOuput, &c, sizeof(COOKED_CHAR));
		}
	}

	return 0;
}


#ifndef ATOK_NO_LINK
//============================================================================
//	FepSearchBindSkb
//
//	u_char  mkey : モディファイアキー
//	u_short ctrlCode : 特殊キー(USBキーコードが入る)
//
//	戻り値 : const FEP_KEYBIND * : キーバインドへのポインタ
static const FEP_KEYBIND *FepSearchBindSkb(u_char mkey, u_short ctrlCode)
{
	const FEP_KEYBIND *pBindList;
	int nKeyBinds;
	u_int c1, c2;
	int i;

	// HDD : __SYSCONF/conf/system.ini の設定を読む
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

	c1 = ctrlCode;
	if(mkey & 0x01)
	{
		c1 |= SHIFT;
	}

	for(i=0; i<nKeyBinds; i++)
	{
		c2 = pBindList[i].code;
		if(c1 == (c2 & ~USBKB_KEYPAD))
		{
			return (&pBindList[i]);
		}
	}

	return NULL;
}
#endif




//============================================================================
//	thinkDakuHandaku
//
//	「た」+「゛」→ 「だ」に変換する処理。
//	ソフトウェアキーボード固有の処理。
//	u_int *D : 入力文字
//
//	戻り値 : int : 0:エラー(文字が見つからない/該当しない)/1:変換できた。
static int thinkDakuHandaku(const u_int *D)
{
	u_int *pUCS4 = g_pFepImport->fep.puiUCS4;
	int iPos     = g_Fep.nCaretPos;
	const skbChange_t *pTbl = g_DakuHandakuSkbTbl;
	int iTblNum = sizeof(g_DakuHandakuSkbTbl)/sizeof(skbChange_t);
	int i, j;

	if(iPos == 0)
	{
		return 0;
	}

	// 最後の文字のインデックス取得
	for(i=0; i<iTblNum; i++)
	{
		pTbl = &g_DakuHandakuSkbTbl[i];

		if(*D != pTbl->target)
		{
			continue;
		}

		for(j=0; j<pTbl->tblNum; j++)
		{
			if(pTbl->pTbl[j].from == pUCS4[iPos-1])
			{
				pUCS4[iPos-1] = pTbl->pTbl[j].to;
				return 1;
			}
		}
	}

	return 0;
}


#ifndef ATOK_NO_LINK
//============================================================================
//	sendKeyAtok
//
//	ATOK QUEUEに文字送信
//
//	sceAtokContext *pAtok : ATOKへのコンテキストポインタ
//	const u_int *p : 文字コード(UCS4)
static void sendKeyAtok(sceAtokContext *pAtok, const u_int *p)
{
	int i=0;
	sceCccJISCS chr=0;
	sceCccJISCS jis=0;
	sceCccSJISTF sjisTmp[32];
	sceCccSJISTF *pSjisTmp   = sjisTmp;
	sceCccSJISTF** ppSjisTmp = &pSjisTmp;

	// S-JISに文字コード変更
	while(*(p+i))
	{
		jis = sceCccUCStoJIS(*(p+i), chr);		// UCS4 -> JIS
		sceCccEncodeSJIS(ppSjisTmp, jis);		// JIS  -> S-JIS
		i++;
	}
	*(*ppSjisTmp) = '\0';


	{
		// 全角文字が来たら半角に変換
		int len = 0;
		int num = sizeof(g_SpecialSign)/sizeof(specialSign_t);
		for(i=0; i<num; i++)
		{
			len = strlen(g_SpecialSign[i].from);
			if(strncmp(g_SpecialSign[i].from, sjisTmp, len) == 0)
			{
				len = strlen(g_SpecialSign[i].to);
				memcpy(sjisTmp, g_SpecialSign[i].to, len);
				sjisTmp[len] = '\0';
			}
		}
	}


	i=0;
	while(sjisTmp[i])
	{
		if(!IS_SJIS_KANJI(sjisTmp[i]))
		{
			// 1バイト文字の場合
			switch(sjisTmp[i])
			{
			case ',':
			case '.':
			case '/':
			case '-':
				// 句読点キャラクタセット設定の影響を受けるキャラクタについては、
				// bit 9を1にしてコマンドを送ることにより設定を無効にする
				Fep_Cmd(pAtok, sjisTmp[i] | 0x200);
				break;

			default:
				Fep_Cmd(pAtok, sjisTmp[i]);
				break;
			}

			i++;
		}
		else
		{
			// 2バイト文字の場合
			u_int sjis, han;

			sjis = (sjisTmp[i]<<8) | sjisTmp[i+1];
			han = CharZenToHan(sjis);

			if(han < 0x100)
			{
				Fep_Cmd(pAtok, han);
			}
			else
			{
				Fep_Cmd(pAtok, ((han>>8) & 0xFF));
				Fep_Cmd(pAtok, ((han   ) & 0xFF));
			}
			i += 2;
		}
	}
}


//============================================================================
//	FepTreatSkbAtok
//
//	各キーコードに沿った処理。
//	戻り値を1とすることで、デッドキャラクタであることを通知する
//
//	sceAtokContext *pAtok : ATOKへのコンテキストポインタ
//	u_short ctrlCode : 特殊キー(USBキーコードが入る)
//	u_char flag : SHIFTが押されているか？
//	const u_int *pCharCode : 文字列へのポインタ
static int FepTreatSkbAtok(sceAtokContext *pAtok, u_short ctrlCode, u_char flag, const u_int *pCharCode)
{
	COOKED_CHAR c;
	int f = 0;
	int res;

	if(ctrlCode == 0)
	{
		// 文字コードをPOST
		#if 1//SKB_EX Sound
		Fep_se_play(SKBSE_ONKEY);  
		#endif
		sendKeyAtok(pAtok, pCharCode);
	}
	else
	{
		// コントロールコードを処理
		const FEP_KEYBIND *pBind = FepSearchBindSkb(flag, ctrlCode);
		int s = sceAtokGetInputState(pAtok);

		if(pBind == NULL)
		{
			// 該当するキーバインドが見つからなかった場合
			// デッドキャラクタとして扱う
			g_pFepImport->dprintf("Unknown code[%d]: %02X:%04X\n", __LINE__, flag, ctrlCode);
			return 1;
		}

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

		if(s > SCE_ATOK_ISTATE_BEFOREINPUT &&
			(f >= SCE_ATOK_EDIT_CONVFORWARD && f <= SCE_ATOK_EDIT_POSTCONVRAW))
		{
			g_Fep.nIsConv = CONV_ON;			// 変換中フラグ設定
		}

		if(f == 0)
		{
			// デッドキャラクタとして扱う
			return 1;
		}

		if(f & RAW_CODE)
		{
			// デッドキャラクタ
			c.Char    = f & ~(USBKB_KEYPAD | USBKB_RAWDAT);
			c.Flag    = 0;
			c.RawCode = 0;
			FifoSendData(&g_Fep.qFepOuput, &c, sizeof(COOKED_CHAR));
		}
		else
		{
			#if 1//SKB_EX Sound
			Fep_atok_sound(f);
			#endif
			res = Fep_Cmd(pAtok ,f);
			if(res)
			{
				// コマンドの定義エラー?
				// CAUTION: デッドキャラクタとしては扱わない。
				g_pFepImport->dprintf("Unknown code[%d]: %02X:%04X\n", __LINE__, flag, ctrlCode);
			}
		}
	}

	return 0;
}
#endif

