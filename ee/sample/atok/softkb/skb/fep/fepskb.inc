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

// fepskb.inc

#ifndef __SKBBIND_INC__
#define __SKBBIND_INC__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#ifndef ATOK_NO_LINK
//============================================================================
//
//  ソフトウェアキーボードバインドテーブル
//
#if 1
//============================================================================
// OSD風 : Atok風とIme風で差はありません。
//============================================================================
static const FEP_KEYBIND skbAtokKeyTbl[] =
{
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
	// ASCII制御文字に変換されたBSコード
	{USBKB_RAWDAT | USBKEYC_BS,						{0x08 | RAW_CODE,		SCE_ATOK_EDIT_BACKSPACE,	SCE_ATOK_EDIT_CONVCANCEL, 	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL}},	// [BS]
	{USBKB_RAWDAT | USBKEYC_BS | SHIFT,				{0x7F | RAW_CODE,		SCE_ATOK_EDIT_DELETE,		SCE_ATOK_EDIT_CONVCANCEL, 	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL}},	// [Shift]+[BS]

	// ASCII制御文字に変換されたLFコード
	{USBKB_RAWDAT | USBKEYC_ENTER,					{0x0A | RAW_CODE,		SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_KAKUTEIPART}},// [Enter]
	{USBKB_RAWDAT | USBKEYC_ENTER | SHIFT,			{0x0A | RAW_CODE,		SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_KAKUTEIPART}},// [Shift]+[Enter]

	// ASCII制御文字に変換されたスペースコード
	{' ',											{' ',					SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDNEXT,		SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD}},// [SPACE]
	{' ' | SHIFT,									{' ',					SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDNEXT,		SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD}},// [Shift]+[SPACE]

	// [ESC]のキーコード
	{USBKB_RAWDAT | USBKEYC_ESCAPE,					{0x1B | RAW_CODE,		SCE_ATOK_EDIT_DELETEALL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL}},	// [ESC]
	{USBKB_RAWDAT | USBKEYC_ESCAPE | SHIFT,			{0x1B | RAW_CODE,		SCE_ATOK_EDIT_DELETEALL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL}},	// [Shift]+[ESC]


	// [半角/全角 漢字]のキーコード
	{USBKB_RAWDAT | USBKEYC_106_KANJI,				{EX_ATOK_EDIT_KANJIOFF,	0x00,						0x00,						0x00,						0x00,						0x00}},// [漢字]
//	{USBKB_RAWDAT | USBKEYC_106_KANJI | ALT,		{EX_ATOK_EDIT_KANJIOFF,	EX_ATOK_EDIT_KANJIOFF,		EX_ATOK_EDIT_KANJIOFF,		EX_ATOK_EDIT_KANJIOFF,		EX_ATOK_EDIT_KANJIOFF,		EX_ATOK_EDIT_KANJIOFF}},						// [Alt]+[漢字]

	// [CapsLock]のキーコード
	{USBKB_RAWDAT | USBKEYC_CAPS_LOCK | CTRL,		{EX_ATOK_EDIT_KANALOCK,	0x00,						0x00,						0x00,						0x00,						0x00}},						// [Ctrl]+[CapsLock]

	// [DEL]のキーコード
//	{USBKB_RAWDAT | USBKEYC_DELETE,					{0x7F | RAW_CODE,		SCE_ATOK_EDIT_DELETE,		SCE_ATOK_EDIT_DELETE,		0x00,						0x00,						SCE_ATOK_EDIT_DELETE}},		// [DEL]

	// [←]のキーコード
	{USBKB_RAWDAT | USBKEYC_LEFT_ARROW,				{0x00,					SCE_ATOK_EDIT_CURLEFT,		EX_ATOK_EDIT_CLSLEFT,		EX_ATOK_EDIT_CLSLEFT,		EX_ATOK_EDIT_CLSLEFT,		EX_ATOK_EDIT_CLSLEFT}},		// [←]
	{USBKB_RAWDAT | USBKEYC_LEFT_ARROW | SHIFT,		{0x00,					SCE_ATOK_EDIT_CURLEFT,		SCE_ATOK_EDIT_FOCUSLEFT,	SCE_ATOK_EDIT_FOCUSLEFT,	SCE_ATOK_EDIT_FOCUSLEFT,	EX_ATOK_EDIT_CLSLEFT}},		// [Shift]+[←]

	// [→]のキーコード
	{USBKB_RAWDAT | USBKEYC_RIGHT_ARROW,			{0x00,					SCE_ATOK_EDIT_CURRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT}},	// [→]
	{USBKB_RAWDAT | USBKEYC_RIGHT_ARROW | SHIFT,	{0x00,					SCE_ATOK_EDIT_CURRIGHT,		SCE_ATOK_EDIT_FOCUSRIGHT,	SCE_ATOK_EDIT_FOCUSRIGHT,	SCE_ATOK_EDIT_FOCUSRIGHT,	EX_ATOK_EDIT_CLSRIGHT}},	// [Shift]+[→]

	// [↓]のキーコード
	{USBKB_RAWDAT | USBKEYC_DOWN_ARROW,				{0x00,					SCE_ATOK_EDIT_CUREND,		SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDNEXT,		SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD}},// [↓]
	{USBKB_RAWDAT | USBKEYC_DOWN_ARROW | SHIFT,		{0x00,					SCE_ATOK_EDIT_CUREND,		SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDGRPNEXT,	SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD}},// [Shift]+[↓]

	// [↑]のキーコード
	{USBKB_RAWDAT | USBKEYC_UP_ARROW,				{0x00,					SCE_ATOK_EDIT_CURTOP,		SCE_ATOK_EDIT_CONVBACKWARD,	EX_ATOK_EDIT_CANDPREV,		SCE_ATOK_EDIT_CONVBACKWARD,	SCE_ATOK_EDIT_CONVBACKWARD}},// [↑]
	{USBKB_RAWDAT | USBKEYC_UP_ARROW | SHIFT,		{0x00,					SCE_ATOK_EDIT_CURTOP,		SCE_ATOK_EDIT_CONVBACKWARD,	EX_ATOK_EDIT_CANDGRPPREV,	SCE_ATOK_EDIT_CONVBACKWARD,	SCE_ATOK_EDIT_CONVBACKWARD}},// [Shift]+[↑]
};



static const FEP_KEYBIND skbImeKeyTbl[] =
{
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
	// ASCII制御文字に変換されたBSコード
	{USBKB_RAWDAT | USBKEYC_BS,						{0x08 | RAW_CODE,		SCE_ATOK_EDIT_BACKSPACE,	SCE_ATOK_EDIT_CONVCANCEL, 	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL}},	// [BS]
	{USBKB_RAWDAT | USBKEYC_BS | SHIFT,				{0x7F | RAW_CODE,		SCE_ATOK_EDIT_DELETE,		SCE_ATOK_EDIT_CONVCANCEL, 	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL}},	// [Shift]+[BS]

	// ASCII制御文字に変換されたLFコード
	{USBKB_RAWDAT | USBKEYC_ENTER,					{0x0A | RAW_CODE,		SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_KAKUTEIPART}},// [Enter]
	{USBKB_RAWDAT | USBKEYC_ENTER | SHIFT,			{0x0A | RAW_CODE,		SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_KAKUTEIPART}},// [Shift]+[Enter]

	// ASCII制御文字に変換されたスペースコード
	{' ',											{' ',					SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDNEXT,		SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD}},// [SPACE]
	{' ' | SHIFT,									{' ',					SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDNEXT,		SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD}},// [Shift]+[SPACE]

	// [ESC]のキーコード
	{USBKB_RAWDAT | USBKEYC_ESCAPE,					{0x1B | RAW_CODE,		SCE_ATOK_EDIT_DELETEALL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL}},	// [ESC]
	{USBKB_RAWDAT | USBKEYC_ESCAPE | SHIFT,			{0x1B | RAW_CODE,		SCE_ATOK_EDIT_DELETEALL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL}},	// [Shift]+[ESC]


	// [半角/全角 漢字]のキーコード
	{USBKB_RAWDAT | USBKEYC_106_KANJI,				{EX_ATOK_EDIT_KANJIOFF,	0x00,						0x00,						0x00,						0x00,						0x00}},// [漢字]
//	{USBKB_RAWDAT | USBKEYC_106_KANJI | ALT,		{EX_ATOK_EDIT_KANJIOFF,	EX_ATOK_EDIT_KANJIOFF,		EX_ATOK_EDIT_KANJIOFF,		EX_ATOK_EDIT_KANJIOFF,		EX_ATOK_EDIT_KANJIOFF,		EX_ATOK_EDIT_KANJIOFF}},						// [Alt]+[漢字]

	// [CapsLock]のキーコード
	{USBKB_RAWDAT | USBKEYC_CAPS_LOCK | CTRL,		{EX_ATOK_EDIT_KANALOCK,	0x00,						0x00,						0x00,						0x00,						0x00}},						// [Ctrl]+[CapsLock]

	// [DEL]のキーコード
//	{USBKB_RAWDAT | USBKEYC_DELETE,					{0x7F | RAW_CODE,		SCE_ATOK_EDIT_DELETE,		SCE_ATOK_EDIT_DELETE,		0x00,						0x00,						SCE_ATOK_EDIT_DELETE}},		// [DEL]

	// [←]のキーコード
	{USBKB_RAWDAT | USBKEYC_LEFT_ARROW,				{0x00,					SCE_ATOK_EDIT_CURLEFT,		EX_ATOK_EDIT_CLSLEFT,		EX_ATOK_EDIT_CLSLEFT,		EX_ATOK_EDIT_CLSLEFT,		EX_ATOK_EDIT_CLSLEFT}},		// [←]
	{USBKB_RAWDAT | USBKEYC_LEFT_ARROW | SHIFT,		{0x00,					SCE_ATOK_EDIT_CURLEFT,		SCE_ATOK_EDIT_FOCUSLEFT,	SCE_ATOK_EDIT_FOCUSLEFT,	SCE_ATOK_EDIT_FOCUSLEFT,	EX_ATOK_EDIT_CLSLEFT}},		// [Shift]+[←]

	// [→]のキーコード
	{USBKB_RAWDAT | USBKEYC_RIGHT_ARROW,			{0x00,					SCE_ATOK_EDIT_CURRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT}},	// [→]
	{USBKB_RAWDAT | USBKEYC_RIGHT_ARROW | SHIFT,	{0x00,					SCE_ATOK_EDIT_CURRIGHT,		SCE_ATOK_EDIT_FOCUSRIGHT,	SCE_ATOK_EDIT_FOCUSRIGHT,	SCE_ATOK_EDIT_FOCUSRIGHT,	EX_ATOK_EDIT_CLSRIGHT}},	// [Shift]+[→]

	// [↓]のキーコード
	{USBKB_RAWDAT | USBKEYC_DOWN_ARROW,				{0x00,					SCE_ATOK_EDIT_CUREND,		SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDNEXT,		SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD}},// [↓]
	{USBKB_RAWDAT | USBKEYC_DOWN_ARROW | SHIFT,		{0x00,					SCE_ATOK_EDIT_CUREND,		SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDGRPNEXT,	SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD}},// [Shift]+[↓]

	// [↑]のキーコード
	{USBKB_RAWDAT | USBKEYC_UP_ARROW,				{0x00,					SCE_ATOK_EDIT_CURTOP,		SCE_ATOK_EDIT_CONVBACKWARD,	EX_ATOK_EDIT_CANDPREV,		SCE_ATOK_EDIT_CONVBACKWARD,	SCE_ATOK_EDIT_CONVBACKWARD}},// [↑]
	{USBKB_RAWDAT | USBKEYC_UP_ARROW | SHIFT,		{0x00,					SCE_ATOK_EDIT_CURTOP,		SCE_ATOK_EDIT_CONVBACKWARD,	EX_ATOK_EDIT_CANDGRPPREV,	SCE_ATOK_EDIT_CONVBACKWARD,	SCE_ATOK_EDIT_CONVBACKWARD}},// [Shift]+[↑]
};
#else
//============================================================================
// USBキーボード風
//============================================================================
static const FEP_KEYBIND skbAtokKeyTbl[] =
{
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
	// ASCII制御文字に変換されたBSコード
	{USBKB_RAWDAT | USBKEYC_BS,						{0x08 | RAW_CODE,		SCE_ATOK_EDIT_BACKSPACE,	SCE_ATOK_EDIT_CANCELLALL, 	SCE_ATOK_EDIT_CANCELLALL,	SCE_ATOK_EDIT_CANCELLALL,	SCE_ATOK_EDIT_CANCELLALL}},	// [BS]
	{USBKB_RAWDAT | USBKEYC_BS | SHIFT,				{0x08 | RAW_CODE,		SCE_ATOK_EDIT_BACKSPACE,	SCE_ATOK_EDIT_CONVCANCEL, 	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CANCELLALL}},	// [Shift]+[BS]

	// ASCII制御文字に変換されたLFコード
	{USBKB_RAWDAT | USBKEYC_ENTER,					{0x0A | RAW_CODE,		SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL}},	// [Enter]
	{USBKB_RAWDAT | USBKEYC_ENTER | SHIFT,			{0x0A | RAW_CODE,		SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL}},	// [Shift]+[Enter]

	// ASCII制御文字に変換されたスペースコード
	{' ',											{' ',					SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDNEXT,		SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD}},// [SPACE]
	{' ' | SHIFT,									{' ',					SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDPREV,		SCE_ATOK_EDIT_FOCUSRIGHT,	0x00}},						// [Shift]+[SPACE]

	// [ESC]のキーコード
	{USBKB_RAWDAT | USBKEYC_ESCAPE,					{0x1B | RAW_CODE,		SCE_ATOK_EDIT_DELETEALL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CANCELLALL,	SCE_ATOK_EDIT_CANCELLALL}},	// [ESC]
	{USBKB_RAWDAT | USBKEYC_ESCAPE | SHIFT,			{0x1B | RAW_CODE,		SCE_ATOK_EDIT_DELETEALL,	SCE_ATOK_EDIT_DELETEALL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_DELETEALL,	SCE_ATOK_EDIT_DELETEALL}},	// [Shift]+[ESC]


	// [半角/全角 漢字]のキーコード
	{USBKB_RAWDAT | USBKEYC_106_KANJI,				{EX_ATOK_EDIT_KANJIOFF,	0x00,						0x00,						0x00,						0x00,						0x00}},// [漢字]
//	{USBKB_RAWDAT | USBKEYC_106_KANJI | ALT,		{EX_ATOK_EDIT_KANJIOFF,	EX_ATOK_EDIT_KANJIOFF,		EX_ATOK_EDIT_KANJIOFF,		EX_ATOK_EDIT_KANJIOFF,		EX_ATOK_EDIT_KANJIOFF,		EX_ATOK_EDIT_KANJIOFF}},						// [Alt]+[漢字]

	// [CapsLock]のキーコード
	{USBKB_RAWDAT | USBKEYC_CAPS_LOCK | CTRL,		{EX_ATOK_EDIT_KANALOCK,	0x00,						0x00,						0x00,						0x00,						0x00}},						// [Ctrl]+[CapsLock]

	// [DEL]のキーコード
	{USBKB_RAWDAT | USBKEYC_DELETE,					{0x7F | RAW_CODE,		SCE_ATOK_EDIT_DELETE,		SCE_ATOK_EDIT_DELETE,		0x00,						0x00,						SCE_ATOK_EDIT_DELETE}},		// [DEL]

	// [←]のキーコード
	{USBKB_RAWDAT | USBKEYC_LEFT_ARROW,				{0x00,					SCE_ATOK_EDIT_CURLEFT,		SCE_ATOK_EDIT_FOCUSLEFT,	SCE_ATOK_EDIT_FOCUSLEFT,	SCE_ATOK_EDIT_FOCUSLEFT,	SCE_ATOK_EDIT_FOCUSLEFT}},	// [←]
	{USBKB_RAWDAT | USBKEYC_LEFT_ARROW | SHIFT,		{0x00,					SCE_ATOK_EDIT_CURLEFT,		EX_ATOK_EDIT_CLSLEFT,		EX_ATOK_EDIT_CLSLEFT,		EX_ATOK_EDIT_CLSLEFT,		EX_ATOK_EDIT_CLSLEFT}},		// [Shift]+[←]

	// [→]のキーコード
	{USBKB_RAWDAT | USBKEYC_RIGHT_ARROW,			{0x00,					SCE_ATOK_EDIT_CURRIGHT,		SCE_ATOK_EDIT_FOCUSRIGHT,	SCE_ATOK_EDIT_FOCUSRIGHT,	SCE_ATOK_EDIT_FOCUSRIGHT,	SCE_ATOK_EDIT_FOCUSRIGHT}},	// [→]
	{USBKB_RAWDAT | USBKEYC_RIGHT_ARROW | SHIFT,	{0x00,					SCE_ATOK_EDIT_CURRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT,		EX_ATOK_EDIT_CLSRIGHT}},	// [Shift]+[→]

	// [↓]のキーコード
	{USBKB_RAWDAT | USBKEYC_DOWN_ARROW,				{0x00,					SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_KAKUTEIPART}},						// [↓]
	{USBKB_RAWDAT | USBKEYC_DOWN_ARROW | SHIFT,		{0x00,					SCE_ATOK_EDIT_1ONKAKUTEI,	SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_KAKUTEIPART}},// [Shift]+[↓]

	// [↑]のキーコード
	{USBKB_RAWDAT | USBKEYC_UP_ARROW,				{0x00,					0x00,						SCE_ATOK_EDIT_CONVBACKWARD,	EX_ATOK_EDIT_CANDPREV,		0x00,						0x00}},						// [↑]
	{USBKB_RAWDAT | USBKEYC_UP_ARROW | SHIFT,		{0x00,					0x00,						SCE_ATOK_EDIT_CONVBACKWARD,	EX_ATOK_EDIT_CANDGRPPREV,	0x00,						0x00}},						// [Shift]+[↑]
};



static const FEP_KEYBIND skbImeKeyTbl[] =
{
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
	// ASCII制御文字に変換されたBSコード
	{USBKB_RAWDAT | USBKEYC_BS,						{0x08 | RAW_CODE,		SCE_ATOK_EDIT_BACKSPACE,	SCE_ATOK_EDIT_CONVCANCEL, 	SCE_ATOK_EDIT_CONVCANCEL,	0x00,						SCE_ATOK_EDIT_CONVCANCEL}},	// [BS]
	{USBKB_RAWDAT | USBKEYC_BS | SHIFT,				{0x08 | RAW_CODE,		SCE_ATOK_EDIT_BACKSPACE,	SCE_ATOK_EDIT_CONVCANCEL, 	SCE_ATOK_EDIT_CONVCANCEL,	0x00,						SCE_ATOK_EDIT_CONVCANCEL}},	// [Shift]+[BS]

	// ASCII制御文字に変換されたLFコード
	{USBKB_RAWDAT | USBKEYC_ENTER,					{0x0A | RAW_CODE,		SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL}},	// [Enter]
	{USBKB_RAWDAT | USBKEYC_ENTER | SHIFT,			{0x0A | RAW_CODE,		SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL,	SCE_ATOK_EDIT_KAKUTEIALL}},	// [Enter]

	// [ESC]のキーコード
	{USBKB_RAWDAT | USBKEYC_ESCAPE,					{0x1B | RAW_CODE,		SCE_ATOK_EDIT_DELETEALL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CONVCANCEL,	SCE_ATOK_EDIT_CANCELLALL,	SCE_ATOK_EDIT_CANCELLALL}},	// [ESC]
	{USBKB_RAWDAT | USBKEYC_ESCAPE | SHIFT,			{0x1B | RAW_CODE,		SCE_ATOK_EDIT_DELETEALL,	SCE_ATOK_EDIT_CANCELLALL,	SCE_ATOK_EDIT_CANCELLALL,	SCE_ATOK_EDIT_CANCELLALL,	SCE_ATOK_EDIT_CANCELLALL}},	// [Shift]+[ESC]

	// ASCII制御文字に変換されたスペースコード
	{' ',											{' ',					SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDNEXT,		SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVFORWARD}},// [SPACE]
	{' ' | SHIFT,									{' ',					SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVBACKWARD,	EX_ATOK_EDIT_CANDPREV,		SCE_ATOK_EDIT_CONVFORWARD,	SCE_ATOK_EDIT_CONVBACKWARD}},// [Shift]+[SPACE]

	// [半角/全角 漢字]のキーコード
	{USBKB_RAWDAT | USBKEYC_106_KANJI,				{EX_ATOK_EDIT_KANJIOFF,	EX_ATOK_EDIT_KANJIOFF,		EX_ATOK_EDIT_KANJIOFF,		EX_ATOK_EDIT_KANJIOFF,		EX_ATOK_EDIT_KANJIOFF,		EX_ATOK_EDIT_KANJIOFF}},// [漢字]
//	{USBKB_RAWDAT | USBKEYC_106_KANJI | ALT,		{EX_ATOK_EDIT_KANJIOFF,	EX_ATOK_EDIT_KANJIOFF,		EX_ATOK_EDIT_KANJIOFF,		EX_ATOK_EDIT_KANJIOFF,		EX_ATOK_EDIT_KANJIOFF,		EX_ATOK_EDIT_KANJIOFF}},	// [Alt]+[漢字]

	// [DEL]のキーコード
	{USBKB_RAWDAT | USBKEYC_DELETE,					{0x7F | RAW_CODE,		SCE_ATOK_EDIT_DELETE,		SCE_ATOK_EDIT_DELETE,		0x00,						0x00,						SCE_ATOK_EDIT_DELETE}},		// [DEL]

	// [←]のキーコード
	{USBKB_RAWDAT | USBKEYC_LEFT_ARROW,				{0x00,					SCE_ATOK_EDIT_CURLEFT,		EX_ATOK_EDIT_REVCLSLEFT,	EX_ATOK_EDIT_REVCLSLEFT,		EX_ATOK_EDIT_REVCLSLEFT,		EX_ATOK_EDIT_REVCLSLEFT}},		// [←]
	{USBKB_RAWDAT | USBKEYC_LEFT_ARROW | SHIFT,		{0x00,					SCE_ATOK_EDIT_CURLEFT,		SCE_ATOK_EDIT_FOCUSLEFT,	SCE_ATOK_EDIT_FOCUSLEFT,	SCE_ATOK_EDIT_FOCUSLEFT,	SCE_ATOK_EDIT_FOCUSLEFT}},	// [Shift]+[←]

	// [↑]のキーコード
	{USBKB_RAWDAT | USBKEYC_UP_ARROW,				{0x00,					SCE_ATOK_EDIT_CURTOP,		SCE_ATOK_EDIT_CONVBACKWARD,	EX_ATOK_EDIT_CANDPREV,		SCE_ATOK_EDIT_FOCUSLEFT,	SCE_ATOK_EDIT_CONVBACKWARD}},// [↑]
	{USBKB_RAWDAT | USBKEYC_UP_ARROW | SHIFT,		{0x00,					0x00,						SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDGRPPREV,	0x00,						0x00}},// [Shift]+[↑]

	// [→]のキーコード
	{USBKB_RAWDAT | USBKEYC_RIGHT_ARROW,			{0x00,					SCE_ATOK_EDIT_CURRIGHT,		EX_ATOK_EDIT_REVCLSRIGHT,	EX_ATOK_EDIT_REVCLSRIGHT,	EX_ATOK_EDIT_REVCLSRIGHT,	EX_ATOK_EDIT_REVCLSRIGHT}},	// [→]
	{USBKB_RAWDAT | USBKEYC_RIGHT_ARROW | SHIFT,	{0x00,					SCE_ATOK_EDIT_CURRIGHT,		SCE_ATOK_EDIT_FOCUSRIGHT,	SCE_ATOK_EDIT_FOCUSRIGHT,	SCE_ATOK_EDIT_FOCUSRIGHT,	SCE_ATOK_EDIT_FOCUSRIGHT}},	// [Shift]+[→]

	// [↓]のキーコード
	{USBKB_RAWDAT | USBKEYC_DOWN_ARROW,				{0x00,					SCE_ATOK_EDIT_CUREND,		SCE_ATOK_EDIT_CONVFORWARD,	EX_ATOK_EDIT_CANDNEXT,		SCE_ATOK_EDIT_FOCUSRIGHT,	SCE_ATOK_EDIT_CONVFORWARD}},// [↓]
	{USBKB_RAWDAT | USBKEYC_DOWN_ARROW | SHIFT,		{0x00,					SCE_ATOK_EDIT_1ONKAKUTEI,	SCE_ATOK_EDIT_KAKUTEIPART,	EX_ATOK_EDIT_CANDGRPNEXT,	/*先頭確定*/SCE_ATOK_EDIT_KAKUTEIPART,	SCE_ATOK_EDIT_KAKUTEIPART}},// [Shift]+[↓]
};
#endif
#endif


//============================================================================
//
//  後入力によって変化する文字の変換テーブル定義
//  ATOK OFF時
typedef struct
{
	u_int from;			// 変換前の文字
	u_int to;			// 変換後の文字
} dakuHandaku_t;

// 濁点 ひらがな
static const dakuHandaku_t g_DakutenHiraTbl[] =
{
	{0x00003045,	0x00003094},	// "う" -> "ぅ゛"	// サポートしてない?

	{0x0000304B,	0x0000304C},	// "か" -> "が"
	{0x0000304D,	0x0000304E},	// "き" -> "ぎ"
	{0x0000304F,	0x00003050},	// "く" -> "ぐ"
	{0x00003051,	0x00003052},	// "け" -> "げ"
	{0x00003053,	0x00003054},	// "こ" -> "ご"

	{0x00003055,	0x00003056},	// "さ" -> "ざ"
	{0x00003057,	0x00003058},	// "し" -> "じ"
	{0x00003059,	0x0000305A},	// "す" -> "ず"
	{0x0000305B,	0x0000305C},	// "せ" -> "ぜ"
	{0x0000305D,	0x0000305E},	// "そ" -> "ぞ"

	{0x0000305F,	0x00003060},	// "た" -> "だ"
	{0x00003061,	0x00003062},	// "ち" -> "ぢ"
	{0x00003064,	0x00003065},	// "つ" -> "づ"
	{0x00003066,	0x00003067},	// "て" -> "で"
	{0x00003068,	0x00003069},	// "と" -> "ど"

	{0x0000306F,	0x00003070},	// "は" -> "ば"
	{0x00003072,	0x00003073},	// "ひ" -> "び"
	{0x00003075,	0x00003076},	// "ふ" -> "ぶ"
	{0x00003078,	0x00003079},	// "へ" -> "べ"
	{0x0000307B,	0x0000307C},	// "ほ" -> "ぼ"
};

// 半濁点 ひらがな
static const dakuHandaku_t g_HandakutenHiraTbl[] =
{
	{0x0000306F,	0x00003071},	// "は" -> "ぱ"
	{0x00003072,	0x00003074},	// "ひ" -> "ぴ"
	{0x00003075,	0x00003077},	// "ふ" -> "ぷ"
	{0x00003078,	0x0000307A},	// "へ" -> "ぺ"
	{0x0000307B,	0x0000307D},	// "ほ" -> "ぽ"
};

// 濁点 全角カタカナ
static const dakuHandaku_t g_DakutenKanaTbl[] =
{
	{0x000030A6,	0x000030F4},	// "ウ" -> "ヴ"	// サポートしてない?

	{0x000030AB,	0x000030AC},	// "カ" -> "ガ"
	{0x000030AD,	0x000030AE},	// "キ" -> "ギ"
	{0x000030AF,	0x000030B0},	// "ク" -> "グ"
	{0x000030B1,	0x000030B2},	// "ケ" -> "ゲ"
	{0x000030B3,	0x000030B4},	// "コ" -> "ゴ"

	{0x000030B5,	0x000030B6},	// "サ" -> "ザ"
	{0x000030B7,	0x000030B8},	// "シ" -> "ジ"
	{0x000030B9,	0x000030BA},	// "ス" -> "ズ"
	{0x000030BB,	0x000030BC},	// "セ" -> "ゼ"
	{0x000030BD,	0x000030BE},	// "ソ" -> "ゾ"

	{0x000030BF,	0x000030C0},	// "タ" -> "ダ"
	{0x000030C1,	0x000030C2},	// "チ" -> "ヂ"
	{0x000030C4,	0x000030C5},	// "ツ" -> "ヅ"
	{0x000030C6,	0x000030C7},	// "テ" -> "デ"
	{0x000030C8,	0x000030C9},	// "ト" -> "ド"

	{0x000030CF,	0x000030D0},	// "ハ" -> "バ"
	{0x000030D2,	0x000030D3},	// "ヒ" -> "ビ"
	{0x000030D5,	0x000030D6},	// "フ" -> "ブ"
	{0x000030D8,	0x000030D9},	// "ヘ" -> "ベ"
	{0x000030DB,	0x000030DC},	// "ホ" -> "ボ"

	{0x000030EF,	0x000030F7},	// "ワ" -> "ワ゛" // サポートしてない?
};

// 半濁点 全角カタカナ
static const dakuHandaku_t g_HandakutenKanaTbl[] =
{
	{0x000030CF,	0x000030D1},	// "ハ" -> "パ"
	{0x000030D2,	0x000030D4},	// "ヒ" -> "ピ"
	{0x000030D5,	0x000030D7},	// "フ" -> "プ"
	{0x000030D8,	0x000030DA},	// "ヘ" -> "ペ"
	{0x000030DB,	0x000030DD},	// "ホ" -> "ポ"
};

typedef struct
{
	u_int				target;		// 変換対象文字
	const dakuHandaku_t	*pTbl;		// 変換テーブルへのポインタ

	int					tblNum;		// 変換テーブル内要素数
} skbChange_t;

static const skbChange_t g_DakuHandakuSkbTbl[] =
{
	// ひらがな
	{0x0000309B,	g_DakutenHiraTbl,		sizeof(g_DakutenHiraTbl)/sizeof(dakuHandaku_t)},		// 濁点
	{0x0000309C,	g_HandakutenHiraTbl,	sizeof(g_HandakutenHiraTbl)/sizeof(dakuHandaku_t)},		// 半濁点

	// カタカナ(全角)
	{0x0000309B,	g_DakutenKanaTbl,		sizeof(g_DakutenKanaTbl)/sizeof(dakuHandaku_t)},		// 濁点
	{0x0000309C,	g_HandakutenKanaTbl,	sizeof(g_HandakutenKanaTbl)/sizeof(dakuHandaku_t)},		// 半濁点
//	{0x0000FF9E,	g_DakutenKanaTbl,		sizeof(g_DakutenKanaTbl)/sizeof(dakuHandaku_t)},		// 濁点
//	{0x0000FF9F,	g_HandakutenKanaTbl,	sizeof(g_HandakutenKanaTbl)/sizeof(dakuHandaku_t)},		// 半濁点
};


#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// __SKBBIND_INC__
