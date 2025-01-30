/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                I/O Processor Library Sample Program
 *
 *                             - sqdump -
 *
 *                            Version 1.00
 *                              Shift-JIS
 *
 *      Copyright (C) 2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            Apr,24,2003     gens        create
 */
#ifndef _MAINSTR_H_
#define _MAINSTR_H_

#define d_Error_Param_SQ		"SQfile.sq"
#define d_Error_Param_OT		"-o output_file"

#define d_Check_Device			"host1:"
#define d_Check_Device_len		(sizeof(d_Check_Device) - 1)

enum
{
	e_MainMsg_Name = 0,
	e_MainMsg_MaxMidi,
	e_MainMsg_MaxSong,
	e_MainMsg_NothMidi,
	e_MainMsg_NothSong,
	e_MainMsg_Succeed,
	e_MainMsg_ErrCode,
	e_MainMsg_ErrSdSq,
	e_MainMsg_ErrMain,

	e_MainMsg_BasePth,
	e_MainMsg_OutFPth
};

static const char	*s_MainMsg[] = {
	"** This SQ file is\n     \"%s\". **\n\n",
	"** Max Midi number in SQ file. = %d\n",
	"** Max Song number in SQ file. = %d\n\n",
	"** Nothing Midi chunk in SQ file.\n",
	"** Nothing Song chunk in SQ file.\n\n",
	"** Command succeeded.\n",
	"** Error code = 0x%08x\n",
	"** Error: %s\n\n",
	"** Error: %s (%s)\n\n",

	"** The file which becomes a base = \n   %s\n",
	"** Keep a made file is specified = \n   %s\n"
};

enum
{
	e_ER_OK = 0,
	e_ER_Usage,
	e_ER_Param,
	e_ER_NotOpenFile,
	e_ER_UKDevice,

	e_ER_sqdumpERR = 0x2000
};

static const char*	c_ErrString[] = {
	/* e_ER_OK */
	"",

	/* e_ER_Usage */
	"\nUsage: %s -o output_file [OPTIONS] SQfile.sq\n"
	"  OPTIONS\n"

	/*  sqdump -o output_file [オプション] SQfile.sq
	*
	*     -o :　出力ファイルを設定する. (必須)
	*     -n :　Note On / Off の表示を制御する. (指定するとoff)
	*     -m :　ダンプするMIDI チャンクを指定する.
	*     -s :　ダンプするSONG チャンクを指定する.
	*     -D :　デバッグプリント.
	*     -u :　Usage を表示.
	*/
	/*  sqdump -o output_file [OPTIONS] SQfile.sq
	*/

	"    -o : Set output file.\n"
	"    -n : Control print Note On / Off.\n"
	"    -m : Dump MIDI chunk no.\n"
	"    -s : Dump SONG chunk no.\n"
	"    -D : Debug print.\n"
	"    -u : Usage is displayed.\n"
	"\n",

	/* e_ER_Param */
	"Parameter error !",

	/* e_ER_NotOpenFile */
	"Cannot file open !",

	/* e_ER_UKDevice */
	"Unknown device !"
};
#endif /* _MAINSTR_H_ */
