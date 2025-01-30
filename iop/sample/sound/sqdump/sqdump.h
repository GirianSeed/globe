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
#include <sdsq.h>				/* sdsq ���C�u�����w�b�_ */
								

/* �֐��G���g��. */
void SqDumpCreate(const int fpR);
void SqDumpVanish(void);

void SqDumpMidiPrintNoteOnOff(const int noteonoff);
int  SqDumpMidiInit(const int midino, const int fpW);
int  SqDumpMidi(void);

int  SqDumpSongInit(const int songno, const int fpW);
int  SqDumpSong(void);

/* ���̊֐��̓G���[��������Ԃ�. */
int  SqDumpMidiGetMaxNum(void);
int  SqDumpSongGetMaxNum(void);

char* SqDumpGetErrorMessage(void);


/* �G���[�R�[�h. */
enum
{
	e_sqdump_Ok = 0,

	e_sqdump_BadSQ,				/* �s����SQ �t�@�C���ł�. */
								
	e_sqdump_MaxMidiNum,		/* �w�肳�ꂽMIDI �ԍ��͍ő吔���傫��. */
								
	e_sqdump_BadMidiNum,		/* �w�肳�ꂽMIDI �ԍ��͕s���ł�. */
								
	e_sqdump_NoMidiNum,			/* �w�肳�ꂽMIDI �ԍ��͎g�p���Ă��܂���. */
								
	e_sqdump_MaxSongNum,		/* �w�肳�ꂽSONG �ԍ��͍ő吔���傫��. */
								
	e_sqdump_BadSongNum,		/* �w�肳�ꂽSONG �ԍ��͕s���ł�. */
								
	e_sqdump_NoSongNum,			/* �w�肳�ꂽSONG �ԍ��͎g�p���Ă��܂���. */
								

	/* ��������̓G���[���b�Z�[�W�ƘA�����Ă���. */
	e_sqdump_NoMidiChnk,		/* MIDI �`�����N������܂���. */
								
	e_sqdump_NoSongChnk			/* SONG �`�����N������܂���. */
								

};

/* �����p�\����. */
typedef struct SSqDump
{
	u_char*				m_SqData;			/* SQ �f�[�^ */
											int					m_SqDLen;			/* SQ �f�[�^�� */
											
	int					m_fpW;				/* �o�̓t�@�C���f�B�X�N���v�^ */
											
	union
	{
		SceSdSqMidiData		Midi;			/* MIDI �f�[�^ */
												SceSdSqSongData		Song;			/* SONG �f�[�^ */
											} m_Data;

	u_char				m_Datano;			/* MIDI / SONG �ԍ� */
											int					m_Note_f;			/* Note on / off �t���O */
											u_char				m_WernCnt;			/* �t�H�[�}�b�g�x���� */
											
} SSqDump;


/* ���������p�X�e�[�^�X. */
enum
{
	e_SqDumpStIdle = 0,
	e_SqDumpStloop_st,				/* ���[�v�X�^�[�g */
									
	e_SqDumpStloop_ed,				/* ���[�v�G���h */
									
	e_SqDumpStloop_no,				/* ���[�v�X�^�[�g�ԍ� */
									
	e_SqDumpStreverbe_ty,			/* ���o�[�u�^�C�v */
									
	e_SqDumpStreverbe_cr,			/* Core */
									
	e_SqDumpStreverbe_dp,			/* ���o�[�u�f�v�X */
									
	e_SqDumpStreverbe_kd,			/* ���o�[�u���@ */
									
	e_SqDumpStmarkcb,				/* �}�[�N�R�[���o�b�N */
									
	e_SqDumpStmarkcb_7Bdt,			/* �}�[�N�R�[���o�b�N 7BIT */
									
	e_SqDumpStmarkcb_14BdtM,		/* �}�[�N�R�[���o�b�N 14BIT MSB */
									
	e_SqDumpStmarkcb_14BdtL,		/* �}�[�N�R�[���o�b�N 14BIT LSB */
									
	e_SqDumpStmarkcb_M7B,			/* �}�[�N�R�[���o�b�N MSB 7BIT */
									
	e_SqDumpStmarkcb_M14BM,			/* �}�[�N�R�[���o�b�N MSB 14BIT MSB */
									
	e_SqDumpStmarkcb_M14BML			/* �}�[�N�R�[���o�b�N MSB 14BIT LSB */
									
};


/* Midi Message. */
#define d_SqDumpMaxMsgLen		3
#define d_SqDumpMaxMsgMStr	(SCESDSQ_MAXMIDIMESSAGELENGTH*d_SqDumpMaxMsgLen+1)
#define d_SqDumpMaxMsgSStr	(SCESDSQ_MAXSONGMESSAGELENGTH*d_SqDumpMaxMsgLen+1)

/* ���b�Z�[�W�i���s��Y�ꂸ�Ɂj. */
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

/* ���o�[�u�^�C�v. */
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

/* ���o�[�u�f�v�X. */
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
