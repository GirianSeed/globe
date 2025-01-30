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
#ifndef _SQDUMP_H_
#define _SQDUMP_H_

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include <sceerrno.h>
#include <sdsq.h>				/* sdsq ライブラリヘッダ */
								

/* 関数エントリ. */
void SqDumpCreate(const int fpR);
void SqDumpVanish(void);

void SqDumpMidiPrintNoteOnOff(const int noteonoff);
int  SqDumpMidiInit(const int midino, const int fpW);
int  SqDumpMidi(void);

int  SqDumpSongInit(const int songno, const int fpW);
int  SqDumpSong(void);

/* この関数はエラー時負数を返す. */
int  SqDumpMidiGetMaxNum(void);
int  SqDumpSongGetMaxNum(void);

char* SqDumpGetErrorMessage(void);


/* エラーコード. */
enum
{
	e_sqdump_Ok = 0,

	e_sqdump_BadSQ,				/* 不正なSQ ファイルです. */
								
	e_sqdump_MaxMidiNum,		/* 指定されたMIDI 番号は最大数より大きい. */
								
	e_sqdump_BadMidiNum,		/* 指定されたMIDI 番号は不正です. */
								
	e_sqdump_NoMidiNum,			/* 指定されたMIDI 番号は使用していません. */
								
	e_sqdump_MaxSongNum,		/* 指定されたSONG 番号は最大数より大きい. */
								
	e_sqdump_BadSongNum,		/* 指定されたSONG 番号は不正です. */
								
	e_sqdump_NoSongNum,			/* 指定されたSONG 番号は使用していません. */
								

	/* ここより上はエラーメッセージと連動している. */
	e_sqdump_NoMidiChnk,		/* MIDI チャンクがありません. */
								
	e_sqdump_NoSongChnk			/* SONG チャンクがありません. */
								

};

/* 処理用構造体. */
typedef struct SSqDump
{
	u_char*				m_SqData;			/* SQ データ */
											int					m_SqDLen;			/* SQ データ長 */
											
	int					m_fpW;				/* 出力ファイルディスクリプタ */
											
	union
	{
		SceSdSqMidiData		Midi;			/* MIDI データ */
												SceSdSqSongData		Song;			/* SONG データ */
											} m_Data;

	u_char				m_Datano;			/* MIDI / SONG 番号 */
											int					m_Note_f;			/* Note on / off フラグ */
											u_char				m_WernCnt;			/* フォーマット警告数 */
											
} SSqDump;


/* 内部処理用ステータス. */
enum
{
	e_SqDumpStIdle = 0,
	e_SqDumpStloop_st,				/* ループスタート */
									
	e_SqDumpStloop_ed,				/* ループエンド */
									
	e_SqDumpStloop_no,				/* ループスタート番号 */
									
	e_SqDumpStreverbe_ty,			/* リバーブタイプ */
									
	e_SqDumpStreverbe_cr,			/* Core */
									
	e_SqDumpStreverbe_dp,			/* リバーブデプス */
									
	e_SqDumpStreverbe_kd,			/* リバーブ方法 */
									
	e_SqDumpStmarkcb,				/* マークコールバック */
									
	e_SqDumpStmarkcb_7Bdt,			/* マークコールバック 7BIT */
									
	e_SqDumpStmarkcb_14BdtM,		/* マークコールバック 14BIT MSB */
									
	e_SqDumpStmarkcb_14BdtL,		/* マークコールバック 14BIT LSB */
									
	e_SqDumpStmarkcb_M7B,			/* マークコールバック MSB 7BIT */
									
	e_SqDumpStmarkcb_M14BM,			/* マークコールバック MSB 14BIT MSB */
									
	e_SqDumpStmarkcb_M14BML			/* マークコールバック MSB 14BIT LSB */
									
};


/* Midi Message. */
#define d_SqDumpMaxMsgLen		3
#define d_SqDumpMaxMsgMStr	(SCESDSQ_MAXMIDIMESSAGELENGTH*d_SqDumpMaxMsgLen+1)
#define d_SqDumpMaxMsgSStr	(SCESDSQ_MAXSONGMESSAGELENGTH*d_SqDumpMaxMsgLen+1)

/* メッセージ（改行を忘れずに）. */
/* 0x80, 0x90 */
#define d_SqDumpMsgMNoteOff		" - Note Off (%2d)\n"
#define d_SqDumpMsgMNoteOn		" - Note On  (%2d)\n"

/* 0xb0 */
#define d_SqDumpMsgMBankSelM	" - Bank Select (MSB)\n"			/* 0x00 */
#define d_SqDumpMsgMBankSelL	" - Bank Select (LSB)\n"			/* 0x20 */
#define d_SqDumpMsgMPitchModDep	" - Pitch Modulation Depth\n"		/* 0x01 */
#define d_SqDumpMsgMAmpModDep	" - Amp Modulation Depth\n"			/* 0x02 */
#define d_SqDumpMsgMChnlVol		" - Channel Volume\n"				/* 0x07 */
#define d_SqDumpMsgMPan			" - Pan\n"							/* 0x0a */
#define d_SqDumpMsgMExpression	" - Expression\n"					/* 0x0b */
#define d_SqDumpMsgMDamperPedal	" - Damper Pedal\n"					/* 0x40 */
#define d_SqDumpMsgMPortTime	" - Portament Time\n"				/* 0x05 */
#define d_SqDumpMsgMPortOnOff	" - Portament On / Off\n"			/* 0x41 */
#define d_SqDumpMsgMPortCont	" - Portament Controll\n"			/* 0x54 */
#define d_SqDumpMsgMNRPNInc		" - NRPN Increment\n"				/* 0x60 */
#define d_SqDumpMsgMNRPNDec		" - NRPN Decrement\n"				/* 0x61 */

/* 0xa0, 0xd0 */
#define d_SqDumpMsgMPolyKeyPres	" - Polyphonic Key Pressure\n"
#define d_SqDumpMsgMChnlKeyPres	" - Channel Key Pressure\n"

/* 0xb0 */
#define d_SqDumpMsgMLoopStart	" - Loop Start\n"		/* $Bn,$63,$00 */
#define d_SqDumpMsgMLoopStNo	"      + No. %2d\n"		/* $Bn,$06,$nn */
#define d_SqDumpMsgMLoopEnd		" - Loop End\n"			/* $Bn,$06,$nn */
#define d_SqDumpMsgMLoopEdNo	"      + No. %2d\n"		/* $Bn,$06,$nn */
#define d_SqDumpMsgMLoopRep		"      + Repeat %2d\n"	/* $Bn,$26,$nn */
#define d_SqDumpMsgMLoopRepEv	"      + Repeat Ever\n"	/* $Bn,$26,$00 */
#define d_SqDumpMsgMReverbeTy	" - Reverbe Type\n"		/* $Bn,$63,$02 */
#define d_SqDumpMsgMRevCore		"      + Core%d\n"		/* $Bn,$62,$nn */
#define d_SqDumpMsgMRevName		"      + %s\n"			/* $Bn,$06,$mm */
#define d_SqDumpMsgMReverbeDp	" - Reverbe Depth\n"	/* $Bn,$63,$03 */
#define d_SqDumpMsgMRevPlay		"      + %s\n"			/* $Bn,$62,$mm */
#define d_SqDumpMsgMRevParam	"      + %d\n"			/* $Bn,$06,$nn */
#define d_SqDumpMsgMMarkCallbk	" - Mark Callback\n"	/* $Bn,$63,$10 */
#define d_SqDumpMsgMMarkCb7B	"      + 7BIT\n"		/* $Bn,$62,$00 */
#define d_SqDumpMsgMMarkCb14B	"      + 14BIT\n"		/* $Bn,$62,$01 */
#define d_SqDumpMsgMMarkCb7BDt	"      + %d\n"			/* $Bn,$06,$dd */
#define d_SqDumpMsgMMarkCb14BDM	"      + MSB %d\n"		/* $Bn,$06,$dd */
#define d_SqDumpMsgMMarkCb14BDL	"      + LSB %d\n"		/* $Bn,$26,$dd */
#define d_SqDumpMsgMMarkCbM7	" - Mark Callback MSB 7BIT\n"
														/* $Bn,$63,$11 */
#define d_SqDumpMsgMMarkCbM7Dt	"      + %d\n"			/* $Bn,$06,$dd */
#define d_SqDumpMsgMMarkCbM14	" - Mark Callback MSB 14BIT\n"
														/* $Bn,$63,$12 */
#define d_SqDumpMsgMMarkCbM14DM	"      + %d MSB\n"		/* $Bn,$06,$dd */
#define d_SqDumpMsgMMarkCbM14DL	"      + %d LSB\n"		/* $Bn,$26,$dd */

/* リバーブタイプ. */
#define d_SqDumpMsgMRevTyOff		"Reverbe Off"
#define d_SqDumpMsgMRevTyRoom		"Room"
#define d_SqDumpMsgMRevTyStudioA	"StudioA"
#define d_SqDumpMsgMRevTyStudioB	"StudioB"
#define d_SqDumpMsgMRevTyStudioC	"StudioC"
#define d_SqDumpMsgMRevTyHall		"Hall"
#define d_SqDumpMsgMRevTySpace		"Space"
#define d_SqDumpMsgMRevTyEcho		"Echo"
#define d_SqDumpMsgMRevTyDelay		"Delay"
#define d_SqDumpMsgMRevTyPipe		"Pipe"

/* リバーブデプス. */
#define d_SqDumpMsgMRevDpNrm0		"Depth Normal (Core0)"
#define d_SqDumpMsgMRevDpBck0		"Depth Back (Core0)"
#define d_SqDumpMsgMRevDpDelay0		"Delay (Core0)"
#define d_SqDumpMsgMRevDpFade0		"Fade (Core0)"
#define d_SqDumpMsgMRevDpNrm1		"Depth Normal (Core1)"
#define d_SqDumpMsgMRevDpBck1		"Depth Back (Core1)"
#define d_SqDumpMsgMRevDpDelay1		"Delay (Core1)"
#define d_SqDumpMsgMRevDpFade1		"Fade (Core1)"

/* 0xff */
#define d_SqDumpMsgMEndTrak		" - End of Track\n"
#define d_SqDumpMsgMSetTempo	" - Set Tempo\n"


/* Song Message. */
#define d_SqDumpMsgSPlay		" - Play Midi data block %3d\n"
#define d_SqDumpMsgSVolume		" - Volume Set\n"
#define d_SqDumpMsgSVolumePlus	" - Volume +\n"
#define d_SqDumpMsgSVolumeMina	" - Volume -\n"
#define d_SqDumpMsgSPan			" - Pan Set\n"
#define d_SqDumpMsgSPanPlus		" - Pan +\n"
#define d_SqDumpMsgSPanMina		" - Pan -\n"
#define d_SqDumpMsgSFadeTime	" - Fade Time\n"
#define d_SqDumpMsgSFadeVol		" - Fade after Volume\n"
#define d_SqDumpMsgSRepeat		" - Repeat %2d\n"
#define d_SqDumpMsgSRepeatEv	" - Repeat Ever\n"
#define d_SqDumpMsgSRepeatOf	" - Repeat Offset\n"
#define d_SqDumpMsgSTempo		" - Tempo Set\n"
#define d_SqDumpMsgSTempoPlus	" - Tempo +\n"
#define d_SqDumpMsgSTempoMina	" - Tempo -\n"
#define d_SqDumpMsgSAllClear	" - All Clear\n"
#define d_SqDumpMsgSClear		" - Clear\n"
#define d_SqDumpMsgSEnd			" - End\n"

/* Common Message. */
#define d_SqDumpMsgBadFormat	" **** Bad Format ! ****\n"
#define d_SqDumpMsgUnknownCom	" **** Unknown Command ! ****\n"

/* Output Message. */
#define d_SqDumpMTit00	"**** SQ Midi no = %d. ****\n"
#define d_SqDumpMTit01	"No. delta time. tick count. message\n"
#define d_SqDumpMTit02	"------------------------------------------------\n"
#define d_SqDumpMTit03	"**** SQ Midi End. ****\n\n"
#define d_SqDumpMTit04	"****    Bad Format = %d. ****\n"

#define d_SqDumpMCont0	"%5d. %5d. %8d. "
#define d_SqDumpMCont1	" %02X"

#define d_SqDumpSTit00	"**** SQ Song no = %d. ****\n"
#define d_SqDumpSTit01	"No.     message\n"
#define d_SqDumpSTit02	"-----------------\n"
#define d_SqDumpSTit03	"**** SQ Song End. ****\n\n"
#define d_SqDumpSTit04	"****    Bad Format = %d. ****\n"

#define d_SqDumpSCont0	"%5d. "
#define d_SqDumpSCont1	" %02X"

#define d_SqDumpErrorC	"** Error code = %d\n"

#endif
