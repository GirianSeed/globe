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
#include <stdio.h>
#include <stdlib.h>
#include <kernel.h>
#include <sys/file.h>

#include "mainstr.h"
#include "sqdump.h"

int start(int argc, char *argv[])
{
	int				fpR = 0;
	int				fpW = 0;

	int				w_Dflag = 0;
	int				w_Nflag = 0;
	int				w_MidiNo = -1;
	int				w_SongNo = -1;
	char			*w_pmakePath = NULL;
	char			*w_pbasePath = NULL;

	int				make_f = 0;
	int				midi_f = 0;
	int				song_f = 0;
	int				Maxmidi = 0;
	int				Maxsong = 0;
	int				iRtn = e_ER_OK;
	char			*w_errparm = "";
	int				i;

	if (2 == argc) {
		if (0 == strcmp("-u", argv[1])) {
			iRtn	= e_ER_Usage;
			goto EXIT;
		}
	}
	if (2 >= argc) {
		iRtn	= e_ER_Param;
		goto EXIT;
	}
	for (i = 1; i < argc; i++) {
		if ('-' == *argv[i]) {
			if (('o'  != *(argv[i] + 1)
				|| 'm'  != *(argv[i] + 1)
				|| 's'  != *(argv[i] + 1))
			&& (0x00 != *(argv[i] + 2))) {
				iRtn		= e_ER_Param;
				w_errparm	= argv[i];
				goto EXIT;
			}
			switch (*(argv[i] + 1)) {
			case 'o':
				if (0x00 == *(argv[i] + 2)) {
					make_f++;
				}
				else {
					w_pmakePath	= argv[i] + 2;
				}
				break;
			case 'n':
				w_Nflag++;
				break;
			case 'm':
				if (0x00 == *(argv[i] + 2)) {
					midi_f++;
				}
				else {
					w_MidiNo	= atoi(argv[i] + 2);
				}
				break;
			case 's':
				if (0x00 == *(argv[i] + 2)) {
					song_f++;
				}
				else {
					w_SongNo	= atoi(argv[i] + 2);
				}
				break;
			case 'D':
				w_Dflag++;
				break;
			case 'u':
			default:
				iRtn		= e_ER_Param;
				w_errparm	= argv[i];
				goto EXIT;
			}
		}
		else if (make_f) {
			w_pmakePath	= argv[i];
			make_f		= 0;
		}
		else if (midi_f) {
			w_MidiNo	= atoi(argv[i]);
			midi_f		= 0;
		}
		else if (song_f) {
			w_SongNo	= atoi(argv[i]);
			song_f		= 0;
		}
		else {
			w_pbasePath	= argv[i];
		}
	}
	if (NULL == w_pbasePath) {
		iRtn		= e_ER_Param;
		w_errparm	= d_Error_Param_SQ;
		goto EXIT;
	}
	if (NULL == w_pmakePath) {
		iRtn		= e_ER_Param;
		w_errparm	= d_Error_Param_OT;
		goto EXIT;
	}
	if (w_Dflag) {
		printf(s_MainMsg[e_MainMsg_BasePth], w_pbasePath);
		printf(s_MainMsg[e_MainMsg_OutFPth], w_pmakePath);
	}
	if (0 != memcmp(w_pbasePath, d_Check_Device, d_Check_Device_len)) {
		iRtn		= e_ER_UKDevice;
		w_errparm	= d_Error_Param_SQ;
		goto EXIT;
	}
	if (0 != memcmp(w_pmakePath, d_Check_Device, d_Check_Device_len)) {
		iRtn		= e_ER_UKDevice;
		w_errparm	= d_Error_Param_OT;
		goto EXIT;
	}
	/* 入力ファイルオープン */
	if (0 > (fpR = open(w_pbasePath, O_RDONLY))) {
		iRtn		= e_ER_NotOpenFile;
		w_errparm	= w_pbasePath;
		goto EXIT;
	}
	/* SQ ファイル読み込み & 初期化. */
	SqDumpCreate(fpR);

	/* SQ ファイルクローズ */
	close(fpR);

	/* 出力ファイルオープン */
	if (0 > (fpW = open(w_pmakePath, O_WRONLY | O_TRUNC | O_CREAT))) {
		iRtn	= e_ER_NotOpenFile;
		w_errparm	= w_pmakePath;
		goto EXIT;
	}
	fdprintf(fpW, s_MainMsg[e_MainMsg_Name], w_pbasePath);

	if (0 <= (Maxmidi = SqDumpMidiGetMaxNum())) {
		printf(s_MainMsg[e_MainMsg_MaxMidi], Maxmidi);
		fdprintf(fpW, s_MainMsg[e_MainMsg_MaxMidi], Maxmidi);
		Maxmidi++;
	}
	else if (e_sqdump_NoMidiChnk == -Maxmidi) {
		printf(s_MainMsg[e_MainMsg_NothMidi]);
		fdprintf(fpW, s_MainMsg[e_MainMsg_NothMidi]);
	}
	else {
		iRtn	= -Maxmidi | e_ER_sqdumpERR;
		goto EXIT;
	}
	if (0 <= (Maxsong = SqDumpSongGetMaxNum())) {
		printf(s_MainMsg[e_MainMsg_MaxSong], Maxsong);
		fdprintf(fpW, s_MainMsg[e_MainMsg_MaxSong], Maxsong);
		Maxsong++;
	}
	else if (e_sqdump_NoSongChnk == -Maxsong) {
		printf(s_MainMsg[e_MainMsg_NothSong]);
		fdprintf(fpW, s_MainMsg[e_MainMsg_NothSong]);
	}
	else {
		iRtn	= -Maxsong | e_ER_sqdumpERR;
		goto EXIT;
	}
	/* Note On / Off 表示フラグ. */
	SqDumpMidiPrintNoteOnOff(w_Nflag);

	/* MIDI 解析準備. */
	if (-1 == w_MidiNo) {
		w_MidiNo	= 0;
	}
	else {
		Maxmidi		= w_MidiNo + 1;
	}
	for (i = w_MidiNo; i < Maxmidi; i++) {
		if (e_sqdump_Ok != (iRtn = SqDumpMidiInit(i, fpW))) {
			if (e_sqdump_NoMidiNum == iRtn) {
				/* MIDI チャンクがエントリされていない. */
				iRtn	= e_sqdump_Ok;
				continue;
			}
			iRtn	|= e_ER_sqdumpERR;
			goto EXIT;
		}
		if (e_sqdump_Ok != (iRtn = SqDumpMidi())) {
			iRtn	|= e_ER_sqdumpERR;
			goto EXIT;
		}
	}
	/* SONG 解析準備. */
	if (-1 == w_SongNo) {
		w_SongNo	= 0;
	}
	else {
		Maxsong		= w_SongNo + 1;
	}
	for (i = w_SongNo; i < Maxsong; i++) {
		if (e_sqdump_Ok != (iRtn = SqDumpSongInit(i, fpW))) {
			if (e_sqdump_NoSongNum == iRtn) {
				/* SONG チャンクがエントリされていない. */
				iRtn	= e_sqdump_Ok;
				continue;
			}
			iRtn	|= e_ER_sqdumpERR;
			goto EXIT;
		}
		if (e_sqdump_Ok != (iRtn = SqDumpSong())) {
			iRtn	|= e_ER_sqdumpERR;
			goto EXIT;
		}
	}
EXIT:
	if (e_ER_OK == iRtn) {
		printf(s_MainMsg[e_MainMsg_Succeed]);
	}
	else {
		printf(c_ErrString[e_ER_Usage], argv[0]);

		/* エラー処理 */
		if (w_Dflag) {
			printf(s_MainMsg[e_MainMsg_ErrCode], iRtn);
		}
		if (e_ER_sqdumpERR & iRtn) {
			/* sqdump module エラー */
			printf(s_MainMsg[e_MainMsg_ErrSdSq], SqDumpGetErrorMessage());
		}
		else if (e_ER_Usage != iRtn) {
			printf(s_MainMsg[e_MainMsg_ErrMain], c_ErrString[iRtn], w_errparm);
		}
	}
	if (0 <= fpW) {
		close(fpW);
	}
	SqDumpVanish();

	/* 正常は 0, エラーは 1. */
	return (e_ER_OK != iRtn);
}
