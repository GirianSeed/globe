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
#include <kernel.h>
#include <stdio.h>
#include <sys/file.h>

#include "myassert.h"
#include "sqdump.h"
#include "sqdumpstr.h"

/* SQ ダンプ構造体. */
/* 複数のSQ ファイルを同時に解析したい場合は構造体を外に持つ必要がある. */
static SSqDump		s_sqdump;
static SSqDump		*s_psqdump = NULL;

static char			s_ErrMessage[128];

/* 内部関数. */
static void analyzeMeta(const u_char *w_pmsg);
static void analyzeNoteOn(const u_char *w_pmsg, int *noteon);
static void analyzeNoteOff(const u_char *w_pmsg, int *noteon);
static void analyzeControl(const u_char *w_pmsg, int *status);
static void analyzeNRPNCom(const u_char *w_pmsg, int *status);
static void analyzeNRPNSub(const u_char *w_pmsg, int *status);
static void analyzeNRPNDtM(const u_char *w_pmsg, int *status);
static void analyzeNRPNDtL(const u_char *w_pmsg, int *status);

void SqDumpCreate(const int fpR)
{
	/* ファイルを新たに読み込みたい場合は、SqDumpVanish() を呼ぶこと. */
	my_assert(0 <= fpR, s_assertStr[e_sqdump_amsg_File]);
	my_assert(NULL == s_psqdump, s_assertStr[e_sqdump_amsg_Again]);

	s_psqdump	= &s_sqdump;
	s_psqdump->m_SqData		= NULL;
	s_psqdump->m_SqDLen		= 0;
	s_psqdump->m_fpW		= 0;
	s_psqdump->m_Datano		= 0;
	memset(&s_psqdump->m_Data, 0x00, sizeof(s_psqdump->m_Data));

	s_psqdump->m_Note_f	= 0;

	/* ファイルサイズを求める. */
	s_psqdump->m_SqDLen	= lseek(fpR, 0, SEEK_END);

	/* 先頭に戻す. */
	lseek(fpR, 0, SEEK_SET);

	/* ファイルをメモリに読み込む. */
	s_psqdump->m_SqData	=
		AllocSysMemory(SMEM_Low, s_psqdump->m_SqDLen, 0);
	read(fpR, s_psqdump->m_SqData, s_psqdump->m_SqDLen);

	my_assert(NULL != s_psqdump->m_SqData, s_assertStr[e_sqdump_amsg_memory]);
}

void SqDumpVanish(void)
{
	if (NULL != s_psqdump && s_psqdump->m_SqData) {
		FreeSysMemory(s_psqdump->m_SqData);
		s_psqdump->m_SqData	= NULL;
	}
	s_psqdump	= NULL;
}

int  SqDumpMidiGetMaxNum(void)
{
	int		w_rtn = 0;

	my_assert(s_psqdump != NULL, s_assertStr[e_sqdump_amsg_Create]);

	if (0 > (w_rtn = sceSdSqGetMaxMidiNumber(s_psqdump->m_SqData))) {
		int		iCode = SCE_ERROR_ERRNO(w_rtn);

		switch (iCode) {
		case EFAULT:					/* SQデータのアドレスが不正 */
												case EFORMAT:					/* SQデータの内容が不正 */
													w_rtn	= e_sqdump_BadSQ;
			sprintf(s_ErrMessage, s_errmsgStr[w_rtn], iCode);
			break;
		case SCE_ENO_MIDI:				/* Midiチャンクが存在しない */
													w_rtn	= e_sqdump_NoMidiChnk;
			break;
		default:
			printf(d_SqDumpErrorC, w_rtn);
			my_assert(0, s_assertStr[e_sqdump_amsg_MidiMxRtn]);
		}
		w_rtn	*= -1;
	}
	/* この関数はエラー時負数を返す. */
	return w_rtn;
}

void SqDumpMidiPrintNoteOnOff(const int noteonoff)
{
	my_assert(s_psqdump != NULL, s_assertStr[e_sqdump_amsg_Create]);

	s_psqdump->m_Note_f	= noteonoff;
}

int  SqDumpMidiInit(const int midino, const int fpW)
{
	int		iRtn  = e_sqdump_Ok;

	my_assert(s_psqdump != NULL, s_assertStr[e_sqdump_amsg_Create]);
	my_assert(0 <= fpW, s_assertStr[e_sqdump_amsg_File]);

	/* 最大Midi 数より指定値が大きい場合はエラー. */
	if (midino > SqDumpMidiGetMaxNum()) {
		iRtn		= e_sqdump_MaxMidiNum;
		sprintf(s_ErrMessage, s_errmsgStr[iRtn], midino);
		goto EXIT;
	}
	s_psqdump->m_fpW		= fpW;
	s_psqdump->m_Datano		= midino;
	s_psqdump->m_WernCnt	= 0;

	/* MIDI データ初期化. */
	memset(&s_psqdump->m_Data.Midi, 0x00, sizeof(SceSdSqMidiData));

	if (SCE_OK != (iRtn = sceSdSqInitMidiData(s_psqdump->m_SqData,
		s_psqdump->m_Datano, &s_psqdump->m_Data.Midi))) {
		int		iCode = SCE_ERROR_ERRNO(iRtn);

		switch (iCode) {
		case EFAULT:					/* SQデータのアドレスが不正 */
												case EFORMAT:					/* SQデータの内容が不正 */
													iRtn	= e_sqdump_BadSQ;
			sprintf(s_ErrMessage, s_errmsgStr[iRtn], iCode);
			break;
		case SCE_ENO_MIDI:				/* Midiチャンクが存在しない */
													iRtn	= e_sqdump_BadMidiNum;
			sprintf(s_ErrMessage, s_errmsgStr[iRtn], iCode);
			break;
		case SCE_ENO_MIDINUM:			/* MIDINumberは使用していない */
													iRtn	= e_sqdump_NoMidiNum;
			printf(s_errmsgStr[iRtn], midino);
			break;
		default:
			printf(d_SqDumpErrorC, iRtn);
			my_assert(0, s_assertStr[e_sqdump_amsg_MidiItRtn]);
		}
	}
EXIT:
	return iRtn;
}

int  SqDumpMidi(void)
{
	u_char		w_message[d_SqDumpMaxMsgMStr];
	u_char		*w_pmsg = NULL;
	int			w_msglen = 0;
	int			w_Tickcnt = 0;
	int			w_blockno = 0;
	int			w_noteon = 0;					/* ノートオン数. */
									
	int			w_status = e_SqDumpStIdle;		/* NRPN 状態. */
									
	u_int		i;
	int			iRtn  = e_sqdump_Ok;

	my_assert(s_psqdump != NULL, s_assertStr[e_sqdump_amsg_Create]);
	my_assert(0 <= s_psqdump->m_fpW, s_assertStr[e_sqdump_amsg_File]);

	printf(d_SqDumpMTit00, s_psqdump->m_Datano);
	fdprintf(s_psqdump->m_fpW, d_SqDumpMTit00, s_psqdump->m_Datano);
	fdprintf(s_psqdump->m_fpW, d_SqDumpMTit01);
	fdprintf(s_psqdump->m_fpW, d_SqDumpMTit02);

	while (1) {
		if (SCE_OK != (iRtn = sceSdSqReadMidiData(&s_psqdump->m_Data.Midi))) {
			int		iCode = SCE_ERROR_ERRNO(iRtn);
			iRtn	= e_sqdump_BadSQ;
			sprintf(s_ErrMessage, s_errmsgStr[iRtn], iCode);
			goto EXIT;
		}
		/* Tick カウントを計算する. */
		w_pmsg		= s_psqdump->m_Data.Midi.message;
		w_msglen	= s_psqdump->m_Data.Midi.messageLength;
		w_Tickcnt	+= s_psqdump->m_Data.Midi.deltaTime;
		w_blockno++;

		my_assert(w_msglen < SCESDSQ_MAXMIDIMESSAGELENGTH,
			s_assertStr[e_sqdump_amsg_Sqform]);

		fdprintf(s_psqdump->m_fpW, d_SqDumpMCont0,
			w_blockno, s_psqdump->m_Data.Midi.deltaTime, w_Tickcnt);

		for (i = 0; i < w_msglen; i++) {
			sprintf(&w_message[d_SqDumpMaxMsgLen * i],
				d_SqDumpMCont1, w_pmsg[i]);
		}
		for (i = d_SqDumpMaxMsgLen * i; i < d_SqDumpMaxMsgMStr - 1; i++) {
			w_message[i]	= ' ';
		}
		w_message[i]	= 0x00;

		/* 出力. */
		fdputs(w_message, s_psqdump->m_fpW);

		switch (w_pmsg[0] & 0xf0) {
		case 0x80:		/* ノートオフ. */
			analyzeNoteOff(w_pmsg, &w_noteon);
			break;
		case 0x90:		/* ノートオン. */
			analyzeNoteOn(w_pmsg, &w_noteon);
			break;
		case 0xa0:		/* ポリフォニックキープレッシャー. */
			fdputs(d_SqDumpMsgMPolyKeyPres, s_psqdump->m_fpW);
			break;
		case 0xb0:		/* コントロールチェンジ. */
			analyzeControl(w_pmsg, &w_status);
			break;
		case 0xd0:		/* チャンネルキープレッシャー. */
			fdputs(d_SqDumpMsgMChnlKeyPres, s_psqdump->m_fpW);
			break;
		case 0xf0:		/* メタイベント. */
			analyzeMeta(w_pmsg);
			break;
		default:
			fdputc('\n', s_psqdump->m_fpW);
		}
		if (SCESDSQ_READSTATUS_END == s_psqdump->m_Data.Midi.readStatus) {
			/* データ終了. */
			break;
		}
	}
	if (0 < s_psqdump->m_WernCnt) {
		printf(d_SqDumpMTit04, s_psqdump->m_WernCnt);
		fdprintf(s_psqdump->m_fpW, d_SqDumpMTit04, s_psqdump->m_WernCnt);
	}
	fdputs(d_SqDumpMTit03, s_psqdump->m_fpW);

EXIT:
	return iRtn;
}

static void analyzeNoteOn(const u_char *w_pmsg, int *noteon)
{
	if (0 == s_psqdump->m_Note_f) {
		if (0x00 != w_pmsg[2]) {
			/* ノートオン. */
			(*noteon)++;
			fdprintf(s_psqdump->m_fpW, d_SqDumpMsgMNoteOn, *noteon);
		}
		else {
			/* ノートオフ扱い. */
			analyzeNoteOff(w_pmsg, noteon);
		}
	}
	else {
		fdputc('\n', s_psqdump->m_fpW);
	}
}

static void analyzeNoteOff(const u_char *w_pmsg, int *noteon)
{
	if (0 == s_psqdump->m_Note_f) {
		if (0 < *noteon) {
			(*noteon)--;
		}
		fdprintf(s_psqdump->m_fpW, d_SqDumpMsgMNoteOff, *noteon);
	}
	else {
		fdputc('\n', s_psqdump->m_fpW);
	}
}

static void analyzeMeta(const u_char *w_pmsg)
{
	/* メタイベント. */
	if (0xff != w_pmsg[0]) {
		fdputc('\n', s_psqdump->m_fpW);
	}
	else if (0x2f == w_pmsg[1] && 0x00 == w_pmsg[2]) {
		fdputs(d_SqDumpMsgMEndTrak, s_psqdump->m_fpW);
	}
	else if (0x51 == w_pmsg[1] && 0x03 == w_pmsg[2]) {
		fdputs(d_SqDumpMsgMSetTempo, s_psqdump->m_fpW);
	}
}

static void analyzeControl(const u_char *w_pmsg, int *status)
{
	char	*w_msg = NULL;

	switch (w_pmsg[1]) {
	case 0x00:		/* Bank Select(MSB) */
		w_msg	= d_SqDumpMsgMBankSelM;
		break;
	case 0x20:		/* Bank Select(LSB) */
		w_msg	= d_SqDumpMsgMBankSelL;
		break;
	case 0x01:		/* Pitch Modulation Depth */
		w_msg	= d_SqDumpMsgMPitchModDep;
		break;
	case 0x02:		/* Amp Modulation Depth */
		w_msg	= d_SqDumpMsgMAmpModDep;
		break;
	case 0x07:		/* Channel Volume */
		w_msg	= d_SqDumpMsgMChnlVol;
		break;
	case 0x0a:		/* Pan */
		w_msg	= d_SqDumpMsgMPan;
		break;
	case 0x0b:		/* Expression */
		w_msg	= d_SqDumpMsgMExpression;
		break;
	case 0x40:		/* Damper Pedal */
		w_msg	= d_SqDumpMsgMDamperPedal;
		break;
	case 0x05:		/* Portament Time */
		w_msg	= d_SqDumpMsgMPortTime;
		break;
	case 0x41:		/* Portament On/Off */
		w_msg	= d_SqDumpMsgMPortOnOff;
		break;
	case 0x54:		/* Portament Controll */
		w_msg	= d_SqDumpMsgMPortCont;
		break;
	case 0x60:		/* NRPN Increment */
		w_msg	= d_SqDumpMsgMNRPNInc;
		break;
	case 0x61:		/* NRPN Decrement */
		w_msg	= d_SqDumpMsgMNRPNDec;
		break;
	case 0x06:		/* NRPN Data Entry (MSB) */
		analyzeNRPNDtM(w_pmsg, status);
		break;
	case 0x26:		/* NRPN Data Entry (LSB) */
		analyzeNRPNDtL(w_pmsg, status);
		break;
	case 0x62:		/* NRPN (LSB) */
		analyzeNRPNSub(w_pmsg, status);
		break;
	case 0x63:		/* NRPN (MSB) */
		analyzeNRPNCom(w_pmsg, status);
		break;
	}
	if (NULL != w_msg) {
		fdputs(w_msg, s_psqdump->m_fpW);
	}
}

static void analyzeNRPNCom(const u_char *w_pmsg, int *status)
{
	char	*w_msg = NULL;

	/* コマンド. */
	if (e_SqDumpStIdle == *status) {
		switch (w_pmsg[2]) {
		case 0x00:		/* ループスタート */
			*status	= e_SqDumpStloop_st;
			w_msg	= d_SqDumpMsgMLoopStart;
			break;
		case 0x01:		/* ループエンド */
			*status	= e_SqDumpStloop_ed;
			w_msg	= d_SqDumpMsgMLoopEnd;
			break;
		case 0x02:		/* リバーブタイプ */
			*status	= e_SqDumpStreverbe_ty;
			w_msg	= d_SqDumpMsgMReverbeTy;
			break;
		case 0x03:		/* リバーブデプス */
			*status	= e_SqDumpStreverbe_dp;
			w_msg	= d_SqDumpMsgMReverbeDp;
			break;
		case 0x10:		/* マークコールバック */
			*status	= e_SqDumpStmarkcb;
			w_msg	= d_SqDumpMsgMMarkCallbk;
			break;
		case 0x11:		/* マークコールバック MSB 7BIT */
			*status	= e_SqDumpStmarkcb_M7B;
			w_msg	= d_SqDumpMsgMMarkCbM7;
			break;
		case 0x12:		/* マークコールバック MSB 14BIT */
			*status	= e_SqDumpStmarkcb_M14BM;
			w_msg	= d_SqDumpMsgMMarkCbM14;
			break;
		default:
			*status	= e_SqDumpStIdle;
			w_msg	= d_SqDumpMsgUnknownCom;
			s_psqdump->m_WernCnt++;
			break;
		}
	}
	else {
		/* コマンドが終了していないのにコマンド発行. */
		*status	= e_SqDumpStIdle;
		w_msg	= d_SqDumpMsgBadFormat;
		s_psqdump->m_WernCnt++;
	}
	fdputs(w_msg, s_psqdump->m_fpW);
}

static void analyzeNRPNSub(const u_char *w_pmsg, int *status)
{
	char	*w_msg = NULL;

	/* サブ情報. */
	switch (*status) {
	case e_SqDumpStreverbe_ty:			/* リバーブタイプ */
									
		if (0x00 == w_pmsg[2] || 0x10 == w_pmsg[2]) {
			*status	= e_SqDumpStreverbe_cr;
			fdprintf(s_psqdump->m_fpW, d_SqDumpMsgMRevCore, w_pmsg[2] >> 8);
		}
		else {
			*status	= e_SqDumpStIdle;
			w_msg	= d_SqDumpMsgBadFormat;
			s_psqdump->m_WernCnt++;
		}
		break;
	case e_SqDumpStreverbe_dp:			/* リバーブデプス */
									
		switch (w_pmsg[2]) {
		case 0x00:
			w_msg	= d_SqDumpMsgMRevDpNrm0;
			break;
		case 0x01:
			w_msg	= d_SqDumpMsgMRevDpBck0;
			break;
		case 0x02:
			w_msg	= d_SqDumpMsgMRevDpDelay0;
			break;
		case 0x03:
			w_msg	= d_SqDumpMsgMRevDpFade0;
			break;
		case 0x10:
			w_msg	= d_SqDumpMsgMRevDpNrm1;
			break;
		case 0x11:
			w_msg	= d_SqDumpMsgMRevDpBck1;
			break;
		case 0x12:
			w_msg	= d_SqDumpMsgMRevDpDelay1;
			break;
		case 0x13:
			w_msg	= d_SqDumpMsgMRevDpFade1;
			break;
		}
		if (NULL != w_msg) {
			fdprintf(s_psqdump->m_fpW, d_SqDumpMsgMRevPlay, w_msg);
			*status	= e_SqDumpStreverbe_kd;
			w_msg	= NULL;
		}
		else {
			*status	= e_SqDumpStIdle;
			w_msg	= d_SqDumpMsgBadFormat;
			s_psqdump->m_WernCnt++;
		}
		break;
	case e_SqDumpStmarkcb:				/* マークコールバック */
												if (0x00 == w_pmsg[2]) {
			*status	= e_SqDumpStmarkcb_7Bdt;
			w_msg	= d_SqDumpMsgMMarkCb7B;
		}
		else if (0x01 == w_pmsg[2]) {
			*status	= e_SqDumpStmarkcb_14BdtM;
			w_msg	= d_SqDumpMsgMMarkCb14B;
		}
		else {
			*status	= e_SqDumpStIdle;
			w_msg	= d_SqDumpMsgBadFormat;
			s_psqdump->m_WernCnt++;
		}
		break;
	default:
		*status	= e_SqDumpStIdle;
		w_msg	= d_SqDumpMsgBadFormat;
		s_psqdump->m_WernCnt++;
	}
	if (NULL != w_msg) {
		fdputs(w_msg, s_psqdump->m_fpW);
	}
}

static void analyzeNRPNDtM(const u_char *w_pmsg, int *status)
{
	char	*w_msg = NULL;

	/* サブ情報. */
	switch (*status) {
	case e_SqDumpStloop_st:				/* ループスタート */
												*status	= e_SqDumpStIdle;
		w_msg	= d_SqDumpMsgMLoopStNo;
		break;
	case e_SqDumpStloop_ed:				/* ループエンド */
												*status	= e_SqDumpStloop_no;
		w_msg	= d_SqDumpMsgMLoopEdNo;
		break;
	case e_SqDumpStreverbe_cr:			/* Core */
												switch (w_pmsg[2]) {
		case 0x00:
			w_msg	= d_SqDumpMsgMRevTyOff;
			break;
		case 0x01:
			w_msg	= d_SqDumpMsgMRevTyRoom;
			break;
		case 0x02:
			w_msg	= d_SqDumpMsgMRevTyStudioA;
			break;
		case 0x03:
			w_msg	= d_SqDumpMsgMRevTyStudioB;
			break;
		case 0x04:
			w_msg	= d_SqDumpMsgMRevTyStudioC;
			break;
		case 0x05:
			w_msg	= d_SqDumpMsgMRevTyHall;
			break;
		case 0x06:
			w_msg	= d_SqDumpMsgMRevTySpace;
			break;
		case 0x07:
			w_msg	= d_SqDumpMsgMRevTyEcho;
			break;
		case 0x08:
			w_msg	= d_SqDumpMsgMRevTyDelay;
			break;
		case 0x09:
			w_msg	= d_SqDumpMsgMRevTyPipe;
			break;
		}
		*status	= e_SqDumpStIdle;

		if (NULL != w_msg) {
			fdprintf(s_psqdump->m_fpW, d_SqDumpMsgMRevName, w_msg);
			w_msg	= NULL;
		}
		else {
			w_msg	= d_SqDumpMsgBadFormat;
			s_psqdump->m_WernCnt++;
		}
		break;
	case e_SqDumpStreverbe_kd:			/* リバーブ方法 */
												*status	= e_SqDumpStIdle;
		w_msg	= d_SqDumpMsgMRevParam;
		break;
	case e_SqDumpStmarkcb_7Bdt:			/* マークコールバック 7BIT */
												*status	= e_SqDumpStIdle;
		w_msg	= d_SqDumpMsgMMarkCb7BDt;
		break;
	case e_SqDumpStmarkcb_14BdtM:		/* マークコールバック 14BIT MSB */
												*status	= e_SqDumpStmarkcb_14BdtL;
		w_msg	= d_SqDumpMsgMMarkCb14BDM;
		break;
	case e_SqDumpStmarkcb_M7B:			/* マークコールバック MSB 7BIT */
												*status	= e_SqDumpStIdle;
		w_msg	= d_SqDumpMsgMMarkCbM7Dt;
		break;
	case e_SqDumpStmarkcb_M14BM:		/* マークコールバック MSB 14BIT MSB */
												*status	= e_SqDumpStmarkcb_M14BML;
		w_msg	= d_SqDumpMsgMMarkCbM14DM;
		break;
	default:
		*status	= e_SqDumpStIdle;
		fdputs(d_SqDumpMsgBadFormat, s_psqdump->m_fpW);
		s_psqdump->m_WernCnt++;
	}
	if (NULL != w_msg) {
		fdprintf(s_psqdump->m_fpW, w_msg, w_pmsg[2]);
	}
}

static void analyzeNRPNDtL(const u_char *w_pmsg, int *status)
{
	char	*w_msg = NULL;

	/* サブ情報. */
	switch (*status) {
	case e_SqDumpStloop_no:				/* ループスタート番号 */
												if (0x00 == w_pmsg[2]) {
			fdputs(d_SqDumpMsgMLoopRepEv, s_psqdump->m_fpW);
		}
		else {
			w_msg	= d_SqDumpMsgMLoopRep;
		}
		break;
	case e_SqDumpStmarkcb_14BdtL:		/* マークコールバック 14BIT LSB */
												w_msg	= d_SqDumpMsgMMarkCb14BDL;
		break;
	case e_SqDumpStmarkcb_M14BML:		/* マークコールバック MSB 14BIT LSB */
												w_msg	= d_SqDumpMsgMMarkCbM14DL;
		break;
	default:
		fdputs(d_SqDumpMsgBadFormat, s_psqdump->m_fpW);
		s_psqdump->m_WernCnt++;
	}
	*status	= e_SqDumpStIdle;

	if (NULL != w_msg) {
		fdprintf(s_psqdump->m_fpW, w_msg, w_pmsg[2]);
	}
}

int  SqDumpSongGetMaxNum(void)
{
	int		w_rtn = 0;

	my_assert(s_psqdump != NULL, s_assertStr[e_sqdump_amsg_Create]);

	if (0 > (w_rtn = sceSdSqGetMaxSongNumber(s_psqdump->m_SqData))) {
		int		iCode = SCE_ERROR_ERRNO(w_rtn);

		switch (iCode) {
		case EFAULT:					/* SQデータのアドレスが不正 */
												case EFORMAT:					/* SQデータの内容が不正 */
													w_rtn	= e_sqdump_BadSQ;
			sprintf(s_ErrMessage, s_errmsgStr[w_rtn], iCode);
			break;
		case SCE_ENO_SONG:				/* Songチャンクが存在しない */
													w_rtn	= e_sqdump_NoSongChnk;
			break;
		default:
			printf(d_SqDumpErrorC, w_rtn);
			my_assert(0, s_assertStr[e_sqdump_amsg_SongMxRtn]);
		}
		w_rtn	*= -1;
	}
	/* この関数はエラー時負数を返す. */
	return w_rtn;
}

int  SqDumpSongInit(const int songno, const int fpW)
{
	int		iRtn = e_sqdump_Ok;

	my_assert(s_psqdump != NULL, s_assertStr[e_sqdump_amsg_Create]);
	my_assert(0 <= fpW, s_assertStr[e_sqdump_amsg_File]);

	/* 最大Song 数より指定値が大きい場合はエラー. */
	if (songno > SqDumpSongGetMaxNum()) {
		iRtn		= e_sqdump_MaxSongNum;
		sprintf(s_ErrMessage, s_errmsgStr[iRtn], songno);
		goto EXIT;
	}
	s_psqdump->m_fpW		= fpW;
	s_psqdump->m_Datano		= songno;
	s_psqdump->m_WernCnt	= 0;

	/* MIDI データ初期化. */
	memset(&s_psqdump->m_Data.Song, 0x00, sizeof(SceSdSqMidiData));

	if (SCE_OK != (iRtn = sceSdSqInitSongData(s_psqdump->m_SqData,
		s_psqdump->m_Datano, &s_psqdump->m_Data.Song))) {
		int		iCode = SCE_ERROR_ERRNO(iRtn);

		switch (iCode) {
		case EFAULT:					/* SQデータのアドレスが不正 */
												case EFORMAT:					/* SQデータの内容が不正 */
													iRtn	= e_sqdump_BadSQ;
			sprintf(s_ErrMessage, s_errmsgStr[iRtn], iCode);
			break;
		case SCE_ENO_SONG:				/* Songチャンクが存在しない */
													iRtn	= e_sqdump_BadSongNum;
			sprintf(s_ErrMessage, s_errmsgStr[iRtn], iCode);
			break;
		case SCE_ENO_SONGNUM:			/* songNumberは使用していない */
													iRtn	= e_sqdump_NoSongNum;
			printf(s_errmsgStr[iRtn], songno);
			break;
		default:
			printf(d_SqDumpErrorC, iRtn);
			my_assert(0, s_assertStr[e_sqdump_amsg_SongItRtn]);
		}
	}
EXIT:
	return iRtn;
}

int  SqDumpSong(void)
{
	u_char		w_message[d_SqDumpMaxMsgSStr];
	u_char		*w_pmsg = NULL;
	int			w_blockno = 0;
	u_int		i;
	int			iRtn  = e_sqdump_Ok;

	my_assert(s_psqdump != NULL, s_assertStr[e_sqdump_amsg_Create]);
	my_assert(0 <= s_psqdump->m_fpW, s_assertStr[e_sqdump_amsg_File]);

	printf(d_SqDumpSTit00, s_psqdump->m_Datano);
	fdprintf(s_psqdump->m_fpW, d_SqDumpSTit00, s_psqdump->m_Datano);
	fdprintf(s_psqdump->m_fpW, d_SqDumpSTit01);
	fdprintf(s_psqdump->m_fpW, d_SqDumpSTit02);

	while (1) {
		if (SCE_OK != (iRtn = sceSdSqReadSongData(&s_psqdump->m_Data.Song))) {
			int		iCode = SCE_ERROR_ERRNO(iRtn);
			iRtn	= e_sqdump_BadSQ;
			sprintf(s_ErrMessage, s_errmsgStr[iRtn], iCode);
			goto EXIT;
		}
		w_pmsg		= s_psqdump->m_Data.Song.message;
		w_blockno++;

		fdprintf(s_psqdump->m_fpW, d_SqDumpSCont0, w_blockno);

		for (i = 0; i < SCESDSQ_MAXSONGMESSAGELENGTH; i++) {
			sprintf(&w_message[d_SqDumpMaxMsgLen * i],
				d_SqDumpSCont1, w_pmsg[i]);
		}
		w_message[d_SqDumpMaxMsgLen * i]	= 0x00;

		/* 出力. */
		fdputs(w_message, s_psqdump->m_fpW);

		if (0xa0 == w_pmsg[0]) {
			switch (w_pmsg[1]) {
			case 0x00:		/* Play. */
				fdprintf(s_psqdump->m_fpW, d_SqDumpMsgSPlay, w_pmsg[2]);
				break;
			case 0x01:		/* Volume */
				fdputs(d_SqDumpMsgSVolume, s_psqdump->m_fpW);
				break;
			case 0x02:		/* Volume + */
				fdputs(d_SqDumpMsgSVolumePlus, s_psqdump->m_fpW);
				break;
			case 0x03:		/* Volume - */
				fdputs(d_SqDumpMsgSVolumeMina, s_psqdump->m_fpW);
				break;
			case 0x04:		/* Pan */
				fdputs(d_SqDumpMsgSPan, s_psqdump->m_fpW);
				break;
			case 0x05:		/* Pan + */
				fdputs(d_SqDumpMsgSPanPlus, s_psqdump->m_fpW);
				break;
			case 0x06:		/* Pan - */
				fdputs(d_SqDumpMsgSPanMina, s_psqdump->m_fpW);
				break;
			case 0x07:		/* Fade Time */
				fdputs(d_SqDumpMsgSFadeTime, s_psqdump->m_fpW);
				break;
			case 0x08:		/* Fade after volume */
				fdputs(d_SqDumpMsgSFadeVol, s_psqdump->m_fpW);
				break;
			case 0x11:		/* Repeat */
				if (0x00 == w_pmsg[2]) {
					fdputs(d_SqDumpMsgSRepeatEv, s_psqdump->m_fpW);
				}
				else {
					fdprintf(s_psqdump->m_fpW, d_SqDumpMsgSRepeat, w_pmsg[2]);
				}
				break;
			case 0x21:		/* Tempo */
				fdputs(d_SqDumpMsgSTempo, s_psqdump->m_fpW);
				break;
			case 0x22:		/* Tempo + */
				fdputs(d_SqDumpMsgSTempoPlus, s_psqdump->m_fpW);
				break;
			case 0x23:		/* Tempo - */
				fdputs(d_SqDumpMsgSTempoMina, s_psqdump->m_fpW);
				break;
			case 0x7f:
				if (0x00 == w_pmsg[2]) {
					fdputs(d_SqDumpMsgSAllClear, s_psqdump->m_fpW);
				}
				else if (0x01 == w_pmsg[2]) {
					fdputs(d_SqDumpMsgSClear, s_psqdump->m_fpW);
				}
				else if (0x7f == w_pmsg[2]) {
					fdputs(d_SqDumpMsgSEnd, s_psqdump->m_fpW);
				}
				else {
					fdputc('\n', s_psqdump->m_fpW);
				}
				break;
			}
		}
		else if (0xa1 == w_pmsg[0]) {
			fdputs(d_SqDumpMsgSRepeatOf, s_psqdump->m_fpW);
		}
		else {
			fdputc('\n', s_psqdump->m_fpW);
		}
		if (SCESDSQ_READSTATUS_END == s_psqdump->m_Data.Song.readStatus) {
			/* データ終了. */
			break;
		}
	}
	if (0 < s_psqdump->m_WernCnt) {
		printf(d_SqDumpSTit04, s_psqdump->m_WernCnt);
		fdprintf(s_psqdump->m_fpW, d_SqDumpSTit04, s_psqdump->m_WernCnt);
	}
	fdputs(d_SqDumpSTit03, s_psqdump->m_fpW);

EXIT:
	return iRtn;
}

char* SqDumpGetErrorMessage(void)
{
	return s_ErrMessage;
}
