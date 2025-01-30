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


// VJE風キーバインドのための設定ファイル

#ifndef __VJE_INC__
#define __VJE_INC__


#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

// まずここで、キーコードを検索
static const FEP_KEYBIND VjeKeyTbl[] = {
/*
USB HUTのキーコード値
	   |                                        未入力(直接入力)
	   |                                              |               変換前(入力文字のみ)
	   |                                              |                       |                   変換中,候補切れ(候補なし)
	   |                                              |                       |                           |                        候補一覧
	   |                                              |                       |                           |                           |                     文節区切り直し
	   |                                              |                       |                           |                           |                           |                 後変換(カタカナ/半角,無変換)
	 __+__                                          __+_                    __+_                        __+_                        __+_                        __+_                        __+_
	{0x????,                                        {0x00,                  0x00,                       0x00,                       0x00,                       0x00,                       0x00}},
*/
	// ASCII文字に変換された[,][-][.][/]のコード(テンキー側)
	// テンキー側のキーを使って入力された文字を、句読点セットで変換せずに直接入力させるための特殊処理
	// EditTransのParam1のbit 9を1にしたコードを送信すれば、'、。ー・'に変換する機能が抑制されます。
	{',' | USBKB_KEYPAD,							{','|0x200,				','|0x200,					','|0x200,			 		','|0x200,					','|0x200,					','|0x200}},				// [,]
	{'-' | USBKB_KEYPAD,							{'-'|0x200,				'-'|0x200,					'-'|0x200,			 		'-'|0x200,					'-'|0x200,					'-'|0x200}},				// [-]
	{'.' | USBKB_KEYPAD,							{'.'|0x200,				'.'|0x200,					'.'|0x200,			 		'.'|0x200,					'.'|0x200,					'.'|0x200}},				// [.]
	{'/' | USBKB_KEYPAD,							{'/'|0x200,				'/'|0x200,					'/'|0x200,			 		'/'|0x200,					'/'|0x200,					'/'|0x200}},				// [/]

	// ASCII文字に変換された[0]～[9]のコード
	{'0',											{'0',					'0',						'0',					 	EX_ATOK_EDIT_CAND0,			'0',						'0'}},						// [0]
	{'1',											{'1',					'1',						'1',					 	EX_ATOK_EDIT_CAND1,			'1',						'1'}},						// [1]
	{'2',											{'2',					'2',						'2',					 	EX_ATOK_EDIT_CAND2,			'2',						'2'}},						// [2]
	{'3',											{'3',					'3',						'3',					 	EX_ATOK_EDIT_CAND3,			'3',						'3'}},						// [3]
	{'4',											{'4',					'4',						'4',					 	EX_ATOK_EDIT_CAND4,			'4',						'4'}},						// [4]
	{'5',											{'5',					'5',						'5',					 	EX_ATOK_EDIT_CAND5,			'5',						'5'}},						// [5]
	{'6',											{'6',					'6',						'6',					 	EX_ATOK_EDIT_CAND6,			'6',						'6'}},						// [6]
	{'7',											{'7',					'7',						'7',					 	EX_ATOK_EDIT_CAND7,			'7',						'7'}},						// [7]
	{'8',											{'8',					'8',						'8',					 	EX_ATOK_EDIT_CAND8,			'8',						'8'}},						// [8]
	{'9',											{'9',					'9',						'9',					 	EX_ATOK_EDIT_CAND9,			'9',						'9'}},						// [9]

	// ASCII制御文字に変換されたBSコード
	{0x0008,										{0x08 | RAW_CODE,		SCE_ATOK_EDIT_BACKSPACE,	SCE_ATOK_EDIT_CONVCANCEL, 	SCE_ATOK_EDIT_CONVCANCEL,	0x00,						SCE_ATOK_EDIT_CONVCANCEL}},	// [BS]
	{0x0008 | SHIFT,								{0x08 | RAW_CODE,		SCE_ATOK_EDIT_DELETEALL,	SCE_ATOK_EDIT_DELETEALL, 	SCE_ATOK_EDIT_DELETEALL,	SCE_ATOK_EDIT_DELETEALL,	SCE_ATOK_EDIT_DELETEALL}},	// [Shift]+[BS]
/*
	// [BS]のキーコード
	{USBKB_RAWDAT | 0x2A,							{0x00,					SCE_ATOK_EDIT_BACKSPACE,	SCE_ATOK_EDIT_CONVCANCEL, 	SCE_ATOK_EDIT_CONVCANCEL,	0x00,						SCE_ATOK_EDIT_CONVCANCEL}},	// [BS]
	{USBKB_RAWDAT | 0x2A | SHIFT,					{0x00,					SCE_ATOK_EDIT_DELETEALL,	SCE_ATOK_EDIT_DELETEALL, 	SCE_ATOK_EDIT_DELETEALL,	SCE_ATOK_EDIT_DELETEALL,	SCE_ATOK_EDIT_DELETEALL}},	// [Shift]+[BS]
*/

	// ASCII制御文字に変換されたTABコード
	{0x0009,										{0x09 | RAW_CODE,		0x00,						EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		0x00}},						// [TAB]
	{0x0009 | SHIFT,								{0x09 | RAW_CODE,		0x00,						EX_ATOK_EDIT_CLSLEFT,		EX_ATOK_EDIT_CLSLEFT,		EX_ATOK_EDIT_CLSLEFT,		0x00}},						// [Shift]+[TAB]

	// ASCII制御文字に変換されたLFコード
	{0x000A,										{0x0A | RAW_CODE,		SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL}},	// [Enter]
	{0x000A | CTRL,									{0x0A | RAW_CODE,		SCE_ATOK_EDIT_KAKUTEIALL,	0x00,						0x00,						0x00,						0x00}},						// [Ctrl]+[Enter]
	{0x000A | SHIFT,								{0x0A | RAW_CODE,		SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CLSRIGHT}},	// [Shift]+[Enter]
/*
	// [Enter]のキーコード
	{USBKB_RAWDAT | 0x28,							{0x0A | RAW_CODE,		SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL}},	// [Enter]
	{USBKB_RAWDAT | 0x28 | CTRL,					{0x0A | RAW_CODE,		SCE_ATOK_EDIT_KAKUTEIALL,	0x00,						0x00,						0x00,						0x00}},						// [Ctrl]+[Enter]
	{USBKB_RAWDAT | 0x28 | SHIFT,					{0x0A | RAW_CODE,		SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CLSRIGHT}},	// [Shift]+[Enter]
*/

	// ASCII制御文字に変換されたスペースコード
	{' ',											{' ',					SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDNEXT,		SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD}},// [SPACE]
//	{' ' | CTRL,									{0x00,					空白,						EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		0x00}},						// [Ctrl]+[SPACE]
//	{' ' | SHIFT,									{全角空白,				SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_FOCUSRIGHT,	EX_ATOK_EDIT_CANDPREV,		SCE_ATOK_EDIT_FOCUSRIGHT,	0x00}},						// [Shift]+[SPACE]

	// [ESC]のキーコード
	{USBKB_RAWDAT | USBKEYC_ESCAPE,					{0x1B | RAW_CODE,		SCE_ATOK_EDIT_DELETEALL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CANCELALL,	SCE_ATOK_EDIT_CANCELALL}},	// [ESC]
	{USBKB_RAWDAT | USBKEYC_ESCAPE | SHIFT,			{0x1B | RAW_CODE,		SCE_ATOK_EDIT_DELETEALL,	SCE_ATOK_EDIT_DELETEALL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_DELETEALL,	SCE_ATOK_EDIT_DELETEALL}},	// [Shift]+[ESC]

	// [半角/全角 漢字]のキーコード
	{USBKB_RAWDAT | USBKEYC_106_KANJI,				{0x00,					SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDNEXT,		SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD}},// [漢字]
	{USBKB_RAWDAT | USBKEYC_106_KANJI | ALT,		{EX_ATOK_EDIT_KANJIOFF,	0x00,						0x00,						0x00,						0x00,						0x00}},						// [Alt]+[漢字]

	// [CapsLock]のキーコード
	{USBKB_RAWDAT | USBKEYC_CAPS_LOCK | CTRL,		{EX_ATOK_EDIT_KANALOCK,	0x00,						0x00,						0x00,						0x00,						0x00}},						// [Ctrl]+[CapsLock]

	// [F1]～[F12]のキーコード
	{USBKB_RAWDAT | USBKEYC_F1,						{0x00,					0x00,						0x00,						0x00,						0x00,						0x00}},						// [F1]
	{USBKB_RAWDAT | USBKEYC_F2,						{0x00,					0x00,						0x00,						0x00,						0x00,						0x00}},						// [F2]
	{USBKB_RAWDAT | USBKEYC_F3,						{0x00,					0x00,						0x00,						0x00,						0x00,						0x00}},						// [F3]
	{USBKB_RAWDAT | USBKEYC_F4,						{0x00,					0x00,						0x00,						0x00,						0x00,						0x00}},						// [F4]
	{USBKB_RAWDAT | USBKEYC_F5,						{0x00,					0x00,						0x00,						0x00,						0x00,						0x00}},						// [F5]
	{USBKB_RAWDAT | USBKEYC_F6,						{0x00,					SCE_ATOK_EDIT_POSTCONVHIRA,	SCE_ATOK_EDIT_POSTCONVHIRA,	SCE_ATOK_EDIT_POSTCONVHIRA,	SCE_ATOK_EDIT_POSTCONVHIRA,	SCE_ATOK_EDIT_POSTCONVHIRA}},// [F6]
	{USBKB_RAWDAT | USBKEYC_F7,						{0x00,					SCE_ATOK_EDIT_POSTCONVKANA,	SCE_ATOK_EDIT_POSTCONVKANA,	SCE_ATOK_EDIT_POSTCONVKANA,	SCE_ATOK_EDIT_POSTCONVKANA,	SCE_ATOK_EDIT_POSTCONVKANA}},// [F7]
	{USBKB_RAWDAT | USBKEYC_F8,						{0x00,					SCE_ATOK_EDIT_POSTCONVRAW,	SCE_ATOK_EDIT_POSTCONVRAW,	SCE_ATOK_EDIT_POSTCONVRAW,	SCE_ATOK_EDIT_POSTCONVRAW,	SCE_ATOK_EDIT_POSTCONVRAW}},	// [F8]
	{USBKB_RAWDAT | USBKEYC_F9,						{0x00,					SCE_ATOK_EDIT_POSTCONVHALF,	SCE_ATOK_EDIT_POSTCONVHALF,	SCE_ATOK_EDIT_POSTCONVHALF,	SCE_ATOK_EDIT_POSTCONVHALF,	SCE_ATOK_EDIT_POSTCONVHALF}},// [F9]
	{USBKB_RAWDAT | USBKEYC_F10,					{0x00,					EX_ATOK_EDIT_POSTRAWHALF,	EX_ATOK_EDIT_POSTRAWHALF,	EX_ATOK_EDIT_POSTRAWHALF,	EX_ATOK_EDIT_POSTRAWHALF,	EX_ATOK_EDIT_POSTRAWHALF}},	// [F10]
	{USBKB_RAWDAT | USBKEYC_F11,					{0x00,					0x00,						0x00,						0x00,						0x00,						0x00}},						// [F11]
	{USBKB_RAWDAT | USBKEYC_F12,					{0x00,					0x00,						0x00,						0x00,						0x00,						0x00}},						// [F12]

	// [Home]のキーコード
	{USBKB_RAWDAT | USBKEYC_HOME,					{0x00,					SCE_ATOK_EDIT_CURTOP,		EX_ATOK_EDIT_CLSTOP,		EX_ATOK_EDIT_CANDHEAD,		0x00,						EX_ATOK_EDIT_CLSTOP}},		// [Home]

	// [End]のキーコード
	{USBKB_RAWDAT | USBKEYC_END,					{0x00,					SCE_ATOK_EDIT_CUREND,		EX_ATOK_EDIT_CLSEND,		EX_ATOK_EDIT_CANDTAIL,		0x00,						EX_ATOK_EDIT_CLSEND}},		// [End]

	// [PageUp]のキーコード
	{USBKB_RAWDAT | USBKEYC_PAGE_UP,				{0x00,					SCE_ATOK_EDIT_CURTOP,		SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDGRPPREV,	0x00,						EX_ATOK_EDIT_CLSTOP}},		// [PageUp]

	// [PageDown]のキーコード
	{USBKB_RAWDAT | USBKEYC_PAGE_DOWN,				{0x00,					SCE_ATOK_EDIT_CUREND,		SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDGRPNEXT,	0x00,						EX_ATOK_EDIT_CLSEND}},		// [PageDown]

	// [DEL]のキーコード
	{USBKB_RAWDAT | USBKEYC_DELETE,					{0x7F | RAW_CODE,		SCE_ATOK_EDIT_DELETE,		SCE_ATOK_EDIT_DELETE,		0x00,						0x00,						SCE_ATOK_EDIT_DELETE}},		// [DEL]

	// [→]のキーコード
	{USBKB_RAWDAT | USBKEYC_RIGHT_ARROW,			{0x00,					SCE_ATOK_EDIT_CURRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT}},	// [→]
	{USBKB_RAWDAT | USBKEYC_RIGHT_ARROW | CTRL,		{0x00,					SCE_ATOK_EDIT_CUREND,		EX_ATOK_EDIT_CLSEND,		EX_ATOK_EDIT_CANDNEXT,		EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSEND}},		// [Ctrl]+[→]
	{USBKB_RAWDAT | USBKEYC_RIGHT_ARROW | SHIFT,	{0x00,					SCE_ATOK_EDIT_CURRIGHT,		SCE_ATOK_EDIT_FOCUSRIGHT,	SCE_ATOK_EDIT_FOCUSRIGHT,	SCE_ATOK_EDIT_FOCUSRIGHT,	SCE_ATOK_EDIT_FOCUSRIGHT}},	// [Shift]+[→]

	// [←]のキーコード
	{USBKB_RAWDAT | USBKEYC_LEFT_ARROW,				{0x00,					SCE_ATOK_EDIT_CURLEFT,		EX_ATOK_EDIT_CLSLEFT,		EX_ATOK_EDIT_CLSLEFT,		EX_ATOK_EDIT_CLSLEFT,		EX_ATOK_EDIT_CLSLEFT}},		// [←]
	{USBKB_RAWDAT | USBKEYC_LEFT_ARROW | CTRL,		{0x00,					SCE_ATOK_EDIT_CURTOP,		EX_ATOK_EDIT_CLSTOP,		EX_ATOK_EDIT_CANDPREV,		EX_ATOK_EDIT_CLSLEFT,		EX_ATOK_EDIT_CLSTOP}},		// [Ctrl]+[←]
	{USBKB_RAWDAT | USBKEYC_LEFT_ARROW | SHIFT,		{0x00,					SCE_ATOK_EDIT_CURLEFT,		SCE_ATOK_EDIT_FOCUSLEFT,	SCE_ATOK_EDIT_FOCUSLEFT,	SCE_ATOK_EDIT_FOCUSLEFT,	SCE_ATOK_EDIT_FOCUSLEFT}},	// [Shift]+[←]

	// [↓]のキーコード
	{USBKB_RAWDAT | USBKEYC_DOWN_ARROW,				{0x00,					SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDNEXT,		SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD}},// [↓]
	{USBKB_RAWDAT | USBKEYC_DOWN_ARROW | CTRL,		{0x00,					SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_FOCUSRIGHT,	SCE_ATOK_EDIT_CONVFORWARD}},// [Ctrl]+[↓]
	{USBKB_RAWDAT | USBKEYC_DOWN_ARROW | SHIFT,		{0x00,					SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDGRPNEXT,	SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD}},// [Shift]+[↓]

	// [↑]のキーコード
	{USBKB_RAWDAT | USBKEYC_UP_ARROW,				{0x00,					SCE_ATOK_EDIT_CONVBACKWARD,	SCE_ATOK_EDIT_CONVBACKWARD,	EX_ATOK_EDIT_CANDPREV,		SCE_ATOK_EDIT_CONVBACKWARD,	SCE_ATOK_EDIT_CONVBACKWARD}},// [↑]
	{USBKB_RAWDAT | USBKEYC_UP_ARROW | CTRL,		{0x00,					SCE_ATOK_EDIT_CONVBACKWARD,	SCE_ATOK_EDIT_FOCUSLEFT,	SCE_ATOK_EDIT_FOCUSLEFT,	SCE_ATOK_EDIT_FOCUSLEFT,	SCE_ATOK_EDIT_CONVBACKWARD}},// [Ctrl]+[↑]
	{USBKB_RAWDAT | USBKEYC_UP_ARROW | SHIFT,		{0x00,					SCE_ATOK_EDIT_CONVBACKWARD,	SCE_ATOK_EDIT_CONVBACKWARD,	EX_ATOK_EDIT_CANDGRPPREV,	SCE_ATOK_EDIT_CONVBACKWARD,	SCE_ATOK_EDIT_CONVBACKWARD}},// [Shift]+[↑]

	// [Application]のキーコード
//	{USBKB_RAWDAT | USBKEYC_APPLICATION,			{0x00,					0x00,						0x00,						0x00,						0x00,						0x00}},						// [Application]

	// [ひらがな/カタカナ/ローマ字]キー
	{USBKB_RAWDAT | USBKEYC_KANA,					{EX_ATOK_DSPHIRAGANA,	0x00,						0x00,						0x00,						0x00,						0x00}},						// [ひらがな/カタカナ/ローマ字]
	{USBKB_RAWDAT | USBKEYC_KANA | SHIFT,			{EX_ATOK_DSPZENKANA,	0x00,						0x00,						0x00,						0x00,						0x00}},						// [Shift]+[ひらがな/カタカナ/ローマ字]
	{USBKB_RAWDAT | USBKEYC_KANA | ALT,				{EX_ATOK_EDIT_KANALOCK,	0x00,						0x00,						0x00,						0x00,						0x00}},						// [Alt]+[ひらがな/カタカナ/ローマ字]

	// [変換]のキーコード
	{USBKB_RAWDAT | USBKEYC_HENKAN,					{' ',					SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDNEXT,		SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD}},// [変換]
	{USBKB_RAWDAT | USBKEYC_HENKAN | SHIFT,			{' ',					SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVBACKWARD,	EX_ATOK_EDIT_CANDPREV,		SCE_ATOK_EDIT_CONVBACKWARD,	SCE_ATOK_EDIT_CONVBACKWARD}},// [Shift]+[変換]

	// [無変換]のキーコード
	{USBKB_RAWDAT | USBKEYC_MUHENKAN,				{0x00,					SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT}},	// [無変換]
	{USBKB_RAWDAT | USBKEYC_MUHENKAN | CTRL,		{EX_ATOK_DSPSWEISU,		0x00,						0x00,						0x00,						0x00,						0x00}}				,		// [Ctrl]+[無変換]
	{USBKB_RAWDAT | USBKEYC_MUHENKAN | SHIFT,		{EX_ATOK_DSPSWKANA,		0x00,						0x00,						0x00,						EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT}},	// [Shift]+[無変換]

	// [CTRL]+[A]～[Z]のキーコード
	{'A' | CTRL,									{0x01 | RAW_CODE,		SCE_ATOK_EDIT_CURTOP,		EX_ATOK_EDIT_CLSTOP,		EX_ATOK_EDIT_CANDHEAD,		0x00,						EX_ATOK_EDIT_CLSTOP}},		// [Ctrl]+[A]
	{'B' | CTRL,									{0x02 | RAW_CODE,		0x00,						0x00,						0x00,						0x00,						0x00}},						// [Ctrl]+[B]
	{'C' | CTRL,									{0x03 | RAW_CODE,		SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDNEXT,		SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD}},// [Ctrl]+[C]
	{'D' | CTRL,									{0x04 | RAW_CODE,		SCE_ATOK_EDIT_CURRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT}},	// [Ctrl]+[D]
	{'E' | CTRL,									{0x05 | RAW_CODE,		SCE_ATOK_EDIT_CURTOP,		SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDGRPPREV,	0x00,						EX_ATOK_EDIT_CLSTOP}},		// [Ctrl]+[E]
//	{'F' | CTRL,									{0x06 | RAW_CODE,		SCE_ATOK_EDIT_CUREND,		EX_ATOK_EDIT_CLSEND,		EX_ATOK_EDIT_CANDTAIL,		0x00,						EX_ATOK_EDIT_CLSEND}},		// [Ctrl]+[F]
	{'G' | CTRL,									{0x07 | RAW_CODE,		SCE_ATOK_EDIT_DELETE,		SCE_ATOK_EDIT_DELETE,		0x00,						0x00,						SCE_ATOK_EDIT_DELETE}},		// [Ctrl]+[G]
	{'H' | CTRL,									{0x08 | RAW_CODE,		SCE_ATOK_EDIT_BACKSPACE,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL,	0x00,						SCE_ATOK_EDIT_CONVCANCEL}},	// [Ctrl]+[H]
	{'I' | CTRL,									{0x09 | RAW_CODE,		0x00,						EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		0x00}},						// [Ctrl]+[I]
	{'J' | CTRL,									{0x0A | RAW_CODE,		SCE_ATOK_EDIT_POSTCONVHIRA,	SCE_ATOK_EDIT_POSTCONVHIRA,	SCE_ATOK_EDIT_POSTCONVHIRA,	SCE_ATOK_EDIT_POSTCONVHIRA,	SCE_ATOK_EDIT_POSTCONVHIRA}},// [Ctrl]+[J]
	{'K' | CTRL,									{0x0B | RAW_CODE,		SCE_ATOK_EDIT_POSTCONVKANA,	SCE_ATOK_EDIT_POSTCONVKANA,	SCE_ATOK_EDIT_POSTCONVKANA,	SCE_ATOK_EDIT_POSTCONVKANA,	SCE_ATOK_EDIT_POSTCONVKANA}},// [Ctrl]+[K]
	{'L' | CTRL,									{0x0C | RAW_CODE,		SCE_ATOK_EDIT_POSTCONVRAW,	SCE_ATOK_EDIT_POSTCONVRAW,	SCE_ATOK_EDIT_POSTCONVRAW,	SCE_ATOK_EDIT_POSTCONVRAW,	SCE_ATOK_EDIT_POSTCONVRAW}},	// [Ctrl]+[L]
	{'M' | CTRL,									{0x0D | RAW_CODE,		SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL}},	// [Ctrl]+[M]
	{'N' | CTRL,									{0x0E | RAW_CODE,		0x00,						0x00,						0x00,						0x00,						0x00}},						// [Ctrl]+[N]
	{'O' | CTRL,									{0x0F | RAW_CODE,		SCE_ATOK_EDIT_POSTCONVHALF,	SCE_ATOK_EDIT_POSTCONVHALF,	SCE_ATOK_EDIT_POSTCONVHALF,	SCE_ATOK_EDIT_POSTCONVHALF,	SCE_ATOK_EDIT_POSTCONVHALF}},// [Ctrl]+[O]
	{'P' | CTRL,									{0x10 | RAW_CODE,		0x00,						0x00,						0x00,						0x00,						0x00}},						// [Ctrl]+[P]
	{'Q' | CTRL,									{0x11 | RAW_CODE,		SCE_ATOK_EDIT_CURLEFT,		SCE_ATOK_EDIT_FOCUSLEFT,	SCE_ATOK_EDIT_FOCUSLEFT,	SCE_ATOK_EDIT_FOCUSLEFT,	SCE_ATOK_EDIT_FOCUSLEFT}},	// [Ctrl]+[Q]
	{'R' | CTRL,									{0x12 | RAW_CODE,		SCE_ATOK_EDIT_CURTOP,		EX_ATOK_EDIT_CLSTOP,		EX_ATOK_EDIT_CANDPREV,		EX_ATOK_EDIT_CLSTOP,		EX_ATOK_EDIT_CLSTOP}},		// [Ctrl]+[R]
	{'S' | CTRL,									{0x13 | RAW_CODE,		SCE_ATOK_EDIT_CURLEFT,		EX_ATOK_EDIT_CLSLEFT,		EX_ATOK_EDIT_CLSLEFT,		EX_ATOK_EDIT_CLSLEFT,		EX_ATOK_EDIT_CLSLEFT}},		// [Ctrl]+[S]
	{'T' | CTRL,									{0x14 | RAW_CODE,		0x00,						0x00,						0x00,						0x00,						0x00}},						// [Ctrl]+[T]
	{'U' | CTRL,									{0x15 | RAW_CODE,		0x00,						0x00,						0x00,						0x00,						0x00}},						// [Ctrl]+[U]
	{'V' | CTRL,									{0x16 | RAW_CODE,		0x00,						0x00,						0x00,						0x00,						0x00}},						// [Ctrl]+[V]
	{'W' | CTRL,									{0x17 | RAW_CODE,		SCE_ATOK_EDIT_CURRIGHT,		SCE_ATOK_EDIT_FOCUSRIGHT,	SCE_ATOK_EDIT_FOCUSRIGHT,	SCE_ATOK_EDIT_FOCUSRIGHT,	SCE_ATOK_EDIT_FOCUSRIGHT}},	// [Ctrl]+[W]
	{'X' | CTRL,									{0x18 | RAW_CODE,		SCE_ATOK_EDIT_CUREND,		SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDGRPNEXT,	0x00,						EX_ATOK_EDIT_CLSEND}},		// [Ctrl]+[X]
	{'Y' | CTRL,									{0x19 | RAW_CODE,		0x00,						0x00,						0x00,						0x00,						0x00}},						// [Ctrl]+[Y]
	{'Z' | CTRL,									{0x1A | RAW_CODE,		SCE_ATOK_EDIT_CONVBACKWARD,	SCE_ATOK_EDIT_CONVBACKWARD,	EX_ATOK_EDIT_CANDPREV,		SCE_ATOK_EDIT_CONVBACKWARD,	SCE_ATOK_EDIT_CONVBACKWARD}},// [Ctrl]+[Z]
};

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// __VJE_INC__

