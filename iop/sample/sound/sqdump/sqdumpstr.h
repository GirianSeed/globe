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
#ifndef _SQDUMPSTR_H_
#define _SQDUMPSTR_H_

/* エラーメッセージ. */
const char	*s_errmsgStr[] = {
	"OK.\n",

	/* 不正なSQ ファイルです. */
	"This File is bad SQ format. (%d)\n",

	/* 指定されたMIDI 番号は最大数より大きい. */
	"This Midi number is over max. (%d)\n",
	/* 指定されたMIDI 番号は不正です. */
	"This Midi number is badly. (%d)\n",
	/* 指定されたMIDI 番号は使用していません. */
	"This Midi number is nouse. (%d)\n",

	/* 指定されたSONG 番号は最大数より大きい. */
	"This Song number is over max. (%d)\n",
	/* 指定されたSONG 番号は不正です. */
	"This Song number is badly. (%d)\n",
	/* 指定されたSONG 番号は使用していません. */
	"This Song number is nouse. (%d)\n"
};

#ifdef _DEBUG /* { */

/* ASSERT メッセージ. */
enum
{
	e_sqdump_amsg_Param = 0,
	e_sqdump_amsg_Again,
	e_sqdump_amsg_memory,
	e_sqdump_amsg_File,
	e_sqdump_amsg_Sqform,
	e_sqdump_amsg_Create,
	e_sqdump_amsg_MidiMxRtn,
	e_sqdump_amsg_SongMxRtn,
	e_sqdump_amsg_MidiItRtn,
	e_sqdump_amsg_SongItRtn,
};

const char	*s_assertStr[] = {
	"Parameter error.",
	"Cannot call again.",
	"Cannot alloc memory.",
	"File not open.",
	"Invalid SQ Format.",
	"SqDumpCreate() was not called.",
	"Bad sceSdSqGetMaxMidiNumber() return code.",
	"Bad sceSdSqGetMaxSongNumber() return code.",
	"Bad sceSdSqInitMidiData() return code.",
	"Bad sceSdSqInitSongData() return code."
};
#endif /* } */

#endif /*_GETTICKSQSTR_H_ */
