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

#include <sys/types.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <eekernel.h>
#include <sifdev.h>
#include <libusbkb.h>
#include <libccc.h>

#include "mtfifo.h"
#include "libfep.h"

#define FEP_ASSERT(_a_)			assert((int)_a_);
#define FEP_STACK_SIZE			(16384*4)

#define INPUTTYPE_KEYBOARD		(0)
#define INPUTTYPE_SOFTKB		(1)
#define INPUTTYPE_APP			(2)



enum
{
	APP_INTERRUPT_KEY,
	APP_KAKUTEIPART,
	APP_KAKUTEIALL,
	APP_CONVCANCEL,
	APP_DELETEALL,

	APP_MAX
};


gImport_t		*g_pFepImport;
fep_t			g_Fep;


extern void FepTreatKey(sceAtokContext *pAtok, u_char led, u_char mkey, u_short rawcode);
extern void FepTreatSkb(sceAtokContext *pAtok, u_short ctrlCode, u_char flag, const u_int *pCharCode);



static sceAtokContext g_Atok;					// ATOK�G���W���̂��߂̃R���e�L�X�g
static int  g_AtokRsrcSema;						// ATOK�G���W���ւ̍ē���h�����\�[�X�Z�}�t�H
static int  g_FepWorkerThread;					// ���[�J�X���b�h�̃X���b�hID
static u_char FepWokerStack[FEP_STACK_SIZE] __attribute__((aligned(16)));	// �X�^�b�N�G���A�̒�`

static void postKey(u_char led, u_char mkey, u_short keycode);
static void interruptFromKey(void);
static void FepTreatApp(sceAtokContext *pAtok, u_int cmd, u_int arg0, u_int arg1);
static void workerMain(int param __attribute__((unused)));
#ifndef ATOK_NO_LINK
static int  getContext(void);
#endif
static int  getChar(COOKED_CHAR *pCc, int fPeek);

static int  inputBackspace(int pos);
static void inputDelete(int pos);
static void inputAllClear(int *pos);
#ifndef ATOK_NO_LINK
static int FepSetConfig();
#endif

#if 1//SKB_EX Sound
void Fep_atok_sound(int f);
void Fep_se_play(int sound_No);
#endif

// ����FIFO�L���[�ɓ�������v�f�̍\�����������p��
typedef union {
	u_char type;

	struct
	{
		u_char type;
		u_char led;
		u_char mkey;
		u_int  keycode;
	} kb;

	struct
	{
		u_char type;
		u_short usCtrlCode;
		u_char  ucFlag;
		u_int const  *puiChar;
	} skb;

	struct {
		u_char  type;
		int cmd;
		int arg0;
		int arg1;
	} msg;

	u_char sz[8];						// �T�C�Y����
} INPUT_QUEUE_ELEM;





//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//  Fep_Init													ref:  libfep.h
//
//  FEP������
//
//  gImport_t *pImport        : �C���|�[�g�֐��Q
//  int iUseAtok              : ATOK�g�p�t���O(0:���Ȃ�/1:����)
//  int iPriority             : ATOK���[�J�X���b�h�̃v���C�I���e�B
//  const char *pszSysDic     : �V�X�e���f�B���N�g���̃p�X �ւ̃|�C���^
//  const char *pszUserDic    : ���[�U�[�����̃p�X         �ւ̃|�C���^
//  const char *pszSysconfDir : �V�X�e���f�B���N�g���p�X   �ւ̃|�C���^
int Fep_Init(gImport_t *pImport,
			 int iUseAtok, int iPriority,
			 const char *pszSysDic, const char *pszUserDic, const char *pszSysconfDir )
{
	int i;
	struct ThreadParam tp;

	memset(&g_Fep, 0x00, sizeof(g_Fep));	// ���[�N���[���N���A
	g_pFepImport = pImport;					// �A�v�����̃R�[���o�b�N�ݒ�

	// �ʂ̏�����
	g_Fep.iInsertMode = 1;		// �}�����[�h�ŋN��
	#if 1//SKB_EX Power
	g_AtokRsrcSema=-1;
	#endif

	// �L�[���̓L���[���쐬
	i = FifoCreate(&g_Fep.qKeyInput, g_Fep.auiKinQueueBuf, sizeof(g_Fep.auiKinQueueBuf));
	if(i<0)
	{
		// FIFO�����Ɏ��s
		return(i);
	}

	// ATOK�G���W���̏o�̓L���[���쐬
	i = FifoCreate(&g_Fep.qFepOuput, g_Fep.qOutBuf, sizeof(g_Fep.qOutBuf));
	if(i<0)
	{
		// FIFO�����Ɏ��s
		FifoDelete(&g_Fep.qKeyInput);
		return(i);
	}

	// �L���[�C���O���ꂽ�L�[���͂�ATOK�G���W���ɓn�����߂̃��[�J�X���b�h�����
	tp.entry          = (void(*)(void *))workerMain; // �X���b�h�̃G���g���֐�
	tp.stack          = FepWokerStack;               // �X�^�b�N�G���A�̐擪�A�h���X
	tp.stackSize      = sizeof(FepWokerStack);       // �X�^�b�N�G���A�̃T�C�Y
	tp.initPriority   = iPriority;                   // �X���b�h�v���C�I���e�B
	tp.gpReg          = &_gp;                        // GP���W�X�^�l
	g_FepWorkerThread = CreateThread(&tp);  // �X���b�h�𐶐�
	if(g_FepWorkerThread<0)
	{
		// �X���b�h�������s�G���[
		FifoDelete(&g_Fep.qKeyInput);
		FifoDelete(&g_Fep.qFepOuput);
		return(g_FepWorkerThread);
	}

	// FEP ������
	g_Fep.ucUseAtok = 0;
	g_Fep.mode     = 0;	// ATOK OFF
	g_Fep.state    = 0;	// ���͑O

	i=0;
	#ifndef ATOK_NO_LINK
	if(iUseAtok == 1)
	{
		g_Fep.pAtokErx = g_pFepImport->malign(16, 512*1024);
		if(! g_Fep.pAtokErx )
		{
			g_pFepImport->dprintf("ERROR : Fep_Init - fail to get ERX buf\n");
			i=-100;
		}
		else{
			struct SemaParam sp;

			// ATOK�G���W���̂��߂̃��\�[�X�Z�}�t�H���쐬
			sp.maxCount    = 1;                 // �Z�}�t�H�ő�l
			sp.initCount   = 1;                 // �Z�}�t�H�����l
			sp.option      = 0;                 // �Z�}�t�H�t�����
			g_AtokRsrcSema = CreateSema(&sp);   // �Z�}�t�H����
			if( g_AtokRsrcSema >= 0 ){
				// ATOK���b�p���C�u����������   ATOK�����t�@�C������f�B���N�g�������w�肵�܂��B
				i = sceAtokInit( &g_Atok, pszSysDic, pszUserDic, pszSysconfDir, g_Fep.pAtokErx );
				if( i < 0 ){
					DeleteSema(g_AtokRsrcSema);			// ���\�[�X�Z�}�t�H�̍폜
					g_Fep.ucUseAtok = 0;
				}
				else{
					g_Fep.ucUseAtok = 1;

					// �܂�_sysconf�p�[�e�B�V������/atok/atok.ini�ɋL�^����Ă���ݒ��ǂ݂��݂܂�
					FepSetConfig();

					g_Fep.led        = g_pFepImport->usSystemIni[ENV_ATOK_MODE] ? USBKB_LED_KANA : 0;	// �������̓��[�h�ݒ�
					g_Fep.KanDspType = 1;											// �Ђ炪�ȓ���
					sceAtokKanjiOn(&g_Atok);										// ATOK���������̓��[�h��
				}
			}
			else i=-200;
		}
	}
	#endif

	// �X���b�h���N��
	StartThread(g_FepWorkerThread, 0);

	return(i);
}


//============================================================================
//  Fep_Exit													ref:  libfep.h
//
//  ���[�J�X���b�h�����S�ɒ�~������
//
//  �߂�l : �Ӗ�����
int Fep_Exit(void)
{
	#ifndef ATOK_NO_LINK
	if(g_Fep.ucUseAtok)
	{
		struct SemaParam semaPara;

		if( ReferSemaStatus( g_AtokRsrcSema, &semaPara ) ){
			// Atok�֌W�̏������A�n�܂��Ă�����A�I���܂ő҂B
			if( semaPara.currentCount == 0 ){
				#if 1	// Block�֐��B
				WaitSema(g_AtokRsrcSema);	// ���\�[�X�Z�}�t�H�̍폜
				#else	// nonBlock�֐��B
				return -1;               
				#endif
			}
		}
	}
	#endif
	//�����X���b�h�폜�B
	TerminateThread( g_FepWorkerThread );
	DeleteThread( g_FepWorkerThread );
	g_FepWorkerThread = -1;

	FifoDelete(&g_Fep.qKeyInput);		// �����L���[���폜
	FifoDelete(&g_Fep.qFepOuput);		// �����L���[���폜

	#ifndef ATOK_NO_LINK
	if(g_Fep.ucUseAtok)
	{
		// ATOK�G���W�����~(�����t�@�C�����N���[�Y�����)
		sceAtokExit(&g_Atok);
		DeleteSema(g_AtokRsrcSema);			// ���\�[�X�Z�}�t�H�̍폜
		g_AtokRsrcSema = -1;
		if(g_Fep.pAtokErx)
		{
			g_pFepImport->mfree(g_Fep.pAtokErx);
			g_Fep.pAtokErx = 0;
		}
		g_Fep.ucUseAtok = 0;
	}
	#endif

	return 0;
}


//============================================================================
//  Fep_PostSoftKey												ref:  libfep.h
//
//  �\�t�g�E�F�A�L�[�{�[�h���͂�FIFO�ɓn��
//
//  const u_int *pUcs2 : ������ւ̃|�C���^(UCS2)
//  u_char Flag        : SHIFT��������Ă��邩�̃t���O
//  u_short ctrlCode   : �R���g���[���R�[�h
void Fep_PostSoftKey(const u_int *pUcs2, u_char Flag, u_short ctrlCode)
{
	INPUT_QUEUE_ELEM q;

	q.skb.type       = INPUTTYPE_SOFTKB;
	q.skb.usCtrlCode = ctrlCode;
	q.skb.ucFlag     = Flag;
	q.skb.puiChar    = pUcs2;
	FifoPostData(&g_Fep.qKeyInput, &q, sizeof(INPUT_QUEUE_ELEM));
}


//============================================================================
//  Fep_PostApp													ref:  libfep.h
//
//  ���b�Z�[�W��FIFO�ɓn��
//
//  int cmd  : �R�}���h�^�C�v
//  int arg0 : ����0
//  int arg1 : ����1
void Fep_PostApp(int cmd, int arg0, int arg1)
{
	INPUT_QUEUE_ELEM q;

	q.msg.type  = INPUTTYPE_APP;
	q.msg.cmd   = cmd;
	q.msg.arg0  = arg0;
	q.msg.arg1  = arg1;
	FifoPostData(&g_Fep.qKeyInput, &q, sizeof(INPUT_QUEUE_ELEM));
}


//============================================================================
//  Fep_PostKeys												ref:  libfep.h
//
//  �L�[�{�[�h���͂�FIFO�ɓn��
//
//  USBKBDATA_t * pData : �L�[�{�[�h�f�[�^�i�[�\���̂ւ̃|�C���^
void Fep_PostKeys(USBKBDATA_t * pData)
{
	int i;
	FEP_ASSERT(pData);

	if(pData == NULL)
	{
		return;
	}

	for(i=0; i<pData->len; i++)
	{

		if(g_pFepImport->fep.showHideSoftKB())
		{
			// �\�t�g�E�F�A�L�[�{�[�h���\���ɂ���(����̃L�[���͖͂�������)
			Fep_PostApp(APP_INTERRUPT_KEY, 0, 0);
			return;
		}

		postKey(pData->led, pData->mkey, pData->keycode[i]);
	}
}




//============================================================================
//  Fep_Run													ref:  libfep.h
//
//  FEP���C������
//
//  �߂�l : int : ���0
int Fep_Run(void)
{
	int i, j;
	COOKED_CHAR		c;
	int loopCnt = 20;	// 1�t���[����loopCnt���������������Ȃ�

	// FEP����̏�����������āA�ҏW���̕�����ɔ��f������
	// �ʃX���b�h��HDD��̎����A�N�Z�X�������Ȃ��Ă��āAATOK�G���W���̏�Ԃ͔񓯊��ɕω����Ă���
	int pos;

	// �܂��񓯊�FEP���C������A�ϊ��ς�(�m�肳�ꂽ)��������擾����
	#ifndef ATOK_NO_LINK
	getContext();
	#endif

	// ���݂̃L�����b�g�ʒu���擾
	pos = g_Fep.nCaretPos;

	while(1)
	{
		// 1�t���[����loopCnt���������������Ȃ�
		loopCnt--;
		if(loopCnt <= 0)
		{
			break;
		}

		i = getChar(&c, 0);
		if(i == 0)
		{
			break;
		}

		memset((char*)g_Fep.szInsertText, 0x0, sizeof(g_Fep.szInsertText));		// �㏑������镶����ޔ�p�o�b�t�@ ������

		// �܂��A������Ɛ���R�[�h�ɕ�������
		if((c.Char<0x00000020 || c.Char==0x0000007F || c.Char==0xFFFFFFFF) && c.Char != 0x00000009)
		{
			// ����R�[�h�������Ƃ�
			// ATOK��OFF�̏�Ԃ�[Ctlr]+[A]�`[Z]�Ŕ���
			g_pFepImport->dprintf("%08X %02X %04X %d %d\n", c.Char, c.Flag, c.RawCode, pos, g_Fep.nTextLen);

			switch(c.Char)
			{
			case 0x00000008:	// [CTRL]+[H],[BS]����
				pos = inputBackspace(pos);		// �o�b�N�X�y�[�X
				break;

			case 0x0000000A:	// [CTRL]+[M]����
			case 0x0000000D:	// [CTRL]+[J],[CR]����
				g_pFepImport->fep.ok();			// �I��
				#if 1//SKB_EX Sound
				Fep_se_play(SKBSE_ENTER);
				#endif
				break;

			case 0x00000015:	// [CTRL]+[U]����
				inputAllClear(&pos);			// �S���폜
				pos = 0;
				break;

			case 0x0000007F:	// [DEL]����
				inputDelete(pos);
				break;

			case 0x0000001B:	// [ESC]����
				g_pFepImport->fep.cancel();
				#if 1//SKB_EX Sound
				Fep_se_play(SKBSE_CANCEL);
				#endif
				break;

			case 0xFFFFFFFF:
				// �f�b�h�L�����N�^
				// ATOK�G���W����ʂ��Ă�����,����R�[�h�ɕϊ��ł��Ȃ��������̂������ɏW�߂���
				// USBKB_RAWDAT,USBKB_KEYPAD�r�b�g�𗎂Ƃ��Ĕ�r
				switch(c.RawCode & 0x3FFFU)
				{
				case USBKEYC_RIGHT_ARROW:		// [��]�L�[
					if(pos < g_Fep.nTextLen)
					{
						pos++;
						#if 1//SKB_EX Sound
						Fep_se_play(SKBSE_CARETMOVE);
						#endif
					}
					#if 1//SKB_EX Sound
					else
					{
						Fep_se_play(SKBSE_INHIBITINPUT);
					}
					#endif
					break;

				case USBKEYC_LEFT_ARROW:		// [��]�L�[
					if(pos>0)
					{
						pos--;
						#if 1//SKB_EX Sound
						Fep_se_play(SKBSE_CARETMOVE);
						#endif
					}
					#if 1//SKB_EX Sound
					else
					{
						Fep_se_play(SKBSE_INHIBITINPUT);
					}
					#endif
					break;

				case USBKEYC_UP_ARROW:			// [��]�L�[
					// FIXME : ���̂Ƃ���@�\����
					break;

				case USBKEYC_DOWN_ARROW:		// [��]�L�[
					// FIXME : ���̂Ƃ���@�\����
					break;

				case USBKEYC_HOME:				// [HOME]�L�[
					pos = 0;
					#if 1//SKB_EX Sound
					if( pos==0 )  Fep_se_play(SKBSE_INHIBITINPUT);
					else          Fep_se_play(SKBSE_CARETMOVE);
					#endif
					break;

				case USBKEYC_END:				// [END]�L�[
					pos = g_Fep.nTextLen;
					#if 1//SKB_EX Sound
					if( pos==g_Fep.nTextLen )  Fep_se_play(SKBSE_INHIBITINPUT);
					else                       Fep_se_play(SKBSE_CARETMOVE);
					#endif
					break;

				case USBKEYC_INSERT:			// [INSERT]�L�[
					g_Fep.iInsertMode ^= 1;
					break;

				// -------�� NumLock=OFF�̏ꍇ 
				case USBKEYC_KPAD_0:
					if(!g_pFepImport->fep.getNumlockState())
					{
						g_Fep.iInsertMode ^= 1;	// [INSERT]�L�[
					}
					break;
				case USBKEYC_KPAD_PERIOD:
					if(!g_pFepImport->fep.getNumlockState())
					{
						inputDelete(pos);		// [DEL]����
					}
					break;
				case USBKEYC_KPAD_1:
					if(!g_pFepImport->fep.getNumlockState())
					{
						pos = g_Fep.nTextLen;	// [END]�L�[
						#if 1//SKB_EX Sound
						if( pos==g_Fep.nTextLen )  Fep_se_play(SKBSE_INHIBITINPUT);
						else                       Fep_se_play(SKBSE_CARETMOVE);
						#endif
					}
					break;
				case USBKEYC_KPAD_2:
					if(!g_pFepImport->fep.getNumlockState())
					{
						// FIXME : ���̂Ƃ���@�\����
						// [��]�L�[
					}
					break;
				case USBKEYC_KPAD_3:
					if(!g_pFepImport->fep.getNumlockState())
					{
						// FIXME : ���̂Ƃ���@�\����
						// [PAGEDOWN]�L�[
					}
					break;
				case USBKEYC_KPAD_4:
					if(!g_pFepImport->fep.getNumlockState())
					{
						// [��]�L�[
						if(pos>0)
						{
							pos--;
							#if 1//SKB_EX Sound
							Fep_se_play(SKBSE_CARETMOVE);
							#endif
						}
						#if 1//SKB_EX Sound
						else
						{
						     Fep_se_play(SKBSE_INHIBITINPUT);
						}
						#endif
					}
					break;
				case USBKEYC_KPAD_6:
					if(!g_pFepImport->fep.getNumlockState())
					{
						// [��]�L�[
						if(pos < g_Fep.nTextLen)
						{
							pos++;
							#if 1//SKB_EX Sound
							Fep_se_play(SKBSE_CARETMOVE);
							#endif
						}
						#if 1//SKB_EX Sound
						else
						{
						    Fep_se_play(SKBSE_INHIBITINPUT);
						}
						#endif
					}
					break;
				case USBKEYC_KPAD_7:
					if(!g_pFepImport->fep.getNumlockState())
					{
						pos = 0;		// [HOME]�L�[
						#if 1//SKB_EX Sound
						if( pos==0 )  Fep_se_play(SKBSE_INHIBITINPUT);
						else          Fep_se_play(SKBSE_CARETMOVE);
						#endif
					}
					break;
				case USBKEYC_KPAD_8:
					if(!g_pFepImport->fep.getNumlockState())
					{
						// FIXME : ���̂Ƃ���@�\����
						// [��]�L�[
					}
					break;
				case USBKEYC_KPAD_9:
					if(!g_pFepImport->fep.getNumlockState())
					{
						// FIXME : ���̂Ƃ���@�\����
						// [PAGEUP]�L�[
					}
					break;
				// -------�� NumLock=OFF�̏ꍇ

				case USBKEYC_DELETE:			// [Delete]�L�[
					if( c.Flag != 0 
					#ifndef ATOK_NO_LINK
					 || ( g_Fep.ucUseAtok && sceAtokGetInputState(&g_Atok) != SCE_ATOK_ISTATE_BEFOREINPUT )
					#endif
					){
						break;
					}
					inputDelete(pos);
					break;

				case USBKEYC_ESCAPE:			// [ESC]�L�[
					if( c.Flag != 0 
					#ifndef ATOK_NO_LINK
					 || ( g_Fep.ucUseAtok && sceAtokGetInputState(&g_Atok) != SCE_ATOK_ISTATE_BEFOREINPUT )
					#endif
					){
						break;
					}
					g_pFepImport->fep.cancel();
					#if 1//SKB_EX Sound
					Fep_se_play(SKBSE_CANCEL);
					#endif
					break;

				case USBKEYC_BS:				// [BS]�L�[
					if( c.Flag != 0 
					#ifndef ATOK_NO_LINK
					 || ( g_Fep.ucUseAtok && sceAtokGetInputState(&g_Atok) != SCE_ATOK_ISTATE_BEFOREINPUT )
					#endif
					){
						break;
					}
					pos = inputBackspace(pos);
					break;

				case USBKEYC_ENTER:				// [ENTER]�L�[
					if( c.Flag != 0 
					#ifndef ATOK_NO_LINK
					 || ( g_Fep.ucUseAtok && sceAtokGetInputState(&g_Atok) != SCE_ATOK_ISTATE_BEFOREINPUT )
					#endif
					){
						break;
					}
					g_pFepImport->fep.ok();
					#if 1//SKB_EX Sound
					Fep_se_play(SKBSE_ENTER);
					#endif
					break;
				}
				break;
			}
		}
		else
		{
			int w = FEP_EDIT_ADD;	// �}��

			switch(c.Char)
			{
			case 0x00000009:
				if(c.Flag != 0)
				{
					continue;
				}
				break;
			}

			// ����R�[�h�łȂ������Ƃ��A���o�C�g�̃��e�������͂������������ׂ�
			// �}������
			j = 0;
			g_pFepImport->dprintf("%08X %02X %04X %d %d\n", c.Char, c.Flag, c.RawCode, pos, g_Fep.nTextLen);
			if(g_Fep.iInsertMode)
			{
				if(g_pFepImport->fep.uiMaxTextLen > g_Fep.nTextLen)
				{
					i = g_Fep.nTextLen - pos;
					if(i > 0)
					{
						j = 1;
					}
				}
			}
			else
			{
				// INSERT
				j = 0;
			}

			if(g_Fep.iInsertMode == 0)
			{
				// �㏑��
				if(g_Fep.nTextLen == pos)
				{
					// �}��(�L�����b�g��������̍Ō�ɂ���ꍇ)
					w = FEP_EDIT_ADD;
				}
				else
				{
					// �㏑��
					w = FEP_EDIT_INSERT;
					g_Fep.szInsertText[0] = g_pFepImport->fep.puiUCS4[pos];
					g_Fep.szInsertText[1] = 0;								// NOTICE : ����1�����̂�
				}
			}

			// �������͏���
			{
				u_int cTmp[10];
				cTmp[0] = c.Char;
				cTmp[1] = 0;
				// �`�F�b�N
				if(FEP_EDIT_OK == g_pFepImport->fep.changeText(w, cTmp, 0))
				{
					// �ϐ�j�Ŏ����ꂽ�o�C�g�������}���������s��
					if(j)
					{
						// �̈悪�I�[�o�[���b�v����̂ŁAmemcpy()�ł͂Ȃ�memmove()���g��
						memmove((char*)&g_pFepImport->fep.puiUCS4[pos+j], (char*)&g_pFepImport->fep.puiUCS4[pos], (size_t)(i*sizeof(u_int)));
						g_Fep.nTextLen += j;
					}
					g_pFepImport->fep.puiUCS4[pos] = c.Char;
					pos++;
				}
				#if 1//SKB_EX Sound
				else
				{
				    Fep_se_play(SKBSE_INHIBITINPUT);
				}
				#endif
			}

			// ���̃L�����N�^���s�[�N(�L���[������o�����ɁA�擪�̃f�[�^�𓐂݌���)
			i = getChar(&c, 1);
			if(i==0)
			{
				// �L���[���Ɏ��̃L�����N�^�����݂��Ă��Ȃ������ꍇ
				break;
			}
			else if(c.Char<0x00000020 || c.Char==0x0000007F || c.Char==0xFFFFFFFF)
			{
				// �L���[�̎��Ɉʒu���Ă���L�����N�^������n�̃R�[�h�������ꍇ�������񏈗���ł�����B
				// ����n�L�����N�^�̏����͎���s���B
				// �e�������ǂ̈ʒu�ɕ`�悳��Ă�������m��K�v������L�����b�g�n�̐���(�J�[�\����1�s��ɂ���Ƃ�)�́A
				// ��ʏ�ł̕`��ʒu�����肳��Ă��Ȃ��Ə������ł��Ȃ����߁B
				break;
			}
		}
	} // while

	g_Fep.nCaretPos = pos;
	if(pos > g_Fep.nTextLen)
	{
		g_Fep.nTextLen = pos;
	}
	g_pFepImport->fep.puiUCS4[g_Fep.nTextLen]    = 0x00;

	return 0;
}


//============================================================================
//  Fep_GetCandListMax											ref:  libfep.h
//
//  ���ꗗ �ő�v�f���擾
//
//  �߂�l : u_int : ���ꗗ�ő�v�f��
u_int Fep_GetCandListMax(void)
{
	return g_Fep.nCandidates;
}


//============================================================================
//  Fep_GetCandListOffset										ref:  libfep.h
//
//  ���ꗗ �I�t�Z�b�g�擾
//
//  �߂�l : u_int : �I�t�Z�b�gNO
u_int Fep_GetCandListOffset(void)
{
	return g_Fep.nCandOffset;						// �\�����n�߂��⃊�X�g�̐擪�ԍ��𓾂�
}


//============================================================================
//  Fep_GetCurrentCand											ref:  libfep.h
//
//  ���ꗗ ���݌��NO�擾
//
//  �߂�l : u_int : ���݌��NO
u_int Fep_GetCurrentCand(void)
{
	return g_Fep.nCurrentCand;
}


//============================================================================
//  Fep_GetCurrentCand											ref:  libfep.h
//
//  ���ꗗ �\���J�n������ւ̃|�C���^�擾
//
//  �߂�l : char* : �\���J�n������ւ̃|�C���^
char *Fep_GetCandStart(void)
{
	char *pszConverting;							// �ϊ����̕�����(�Ђ炪��)
	int l;											// �ϊ���������̒���
	int ofs;										// �\�����n�߂��⃊�X�g�̔ԍ�
	int i, n;
	char *cp;

	ofs = g_Fep.nCandOffset;						// �\�����n�߂��⃊�X�g�̐擪�ԍ��𓾂�
	pszConverting = g_Fep.CandBuf;					// ��⃊�X�g�̃o�b�t�@�𓾂�
	l  = strlen(pszConverting);						// �ϊ���������̕����񒷂𓾂�
	cp = g_Fep.CandBuf + l + 1;						// �ϊ�����������X�L�b�v
	for(i=0; i<ofs; i++) {
		n = strlen(cp);
		cp += n + 2;
	}

	return cp;										// �\�������⃊�X�g�̐擪
}


//============================================================================
//  Fep_CandListNum												ref:  libfep.h
//
//  ���ꗗ �v�f���擾
//
//  �߂�l : const int : ���ꗗ�v�f��
const int Fep_CandListNum(void)
{
	int i;
	int iCandNum = 0;
	for(i=0; i<10; i++)
	{
		if((g_Fep.nCandOffset+i) >= g_Fep.nCandidates)
		{
			break;
		}
		iCandNum++;
	}

	return iCandNum;
}


//============================================================================
//  Fep_GetCandListItem											ref:  libfep.h
//
//  ���ꗗ ������ւ̃|�C���^�擾
//
//  char *pStartPos : ���ꗗ������J�n�ʒu
//  char *pItem     : ��╶����ւ̃|�C���^
//  char *pSubItem  : �t���ꕶ����ւ̃|�C���^
//
//  �߂�l : char* : ������ւ̃|�C���^
char *Fep_GetCandListItem(char *pStartPos, char *pItem, char *pSubItem)
{
	const char *pszConverting;						// �ϊ����̕�����(�Ђ炪��)
	char *cp = pStartPos;
	int n, len, o, l;

	pszConverting = g_Fep.CandBuf;					// ��⃊�X�g�̃o�b�t�@�𓾂�
	l = strlen(pszConverting);						// �ϊ���������̕����񒷂𓾂�

	n = strlen(cp);
	memcpy(pItem, cp, n);
	pItem[n] = '\0';

	o = *(const u_char *)&cp[n + 1];				// �t����̃I�t�Z�b�g�ʒu���𓾂�
	if(o < l)
	{
		len = strlen(&pszConverting[o]);
		memcpy(pSubItem, &pszConverting[o], len);
		pSubItem[len] = '\0';
	}
	else
	{
		pSubItem[0] = '\0';
	}

	cp += n + 2;
	return cp;
}


//============================================================================
//  Fep_GetCandListMaxWidth										ref:  libfep.h
//
//  ���ꗗ �������̍ő�l�����߂�(�s�N�Z��)
//
//  float mainChars : �ϊ�������
//  float subChars  : �t��  ����
//
//  �߂�l : u_int : �������̍ő�l
u_int Fep_GetCandListMaxWidth(float mainChars, float subChars)
{
	const char *pszConverting;						// �ϊ����̕�����(�Ђ炪��)
	const char *pCandStart;							// ��⃊�X�g�f�[�^�̐擪
	int ofs;										// �\�����n�߂��⃊�X�g�̔ԍ�
	int l;											// �ϊ���������̒���
	int nCands;										// �\�������⃊�X�g��
	u_int iPixels = 0;								// ��⃊�X�g�̉����T�C�Y(GS���W�n)

	const char *cp;
	int i, j, n, o;

	ofs = g_Fep.nCandOffset;						// �\�����n�߂��⃊�X�g�̐擪�ԍ��𓾂�
	pszConverting = g_Fep.CandBuf;					// ��⃊�X�g�̃o�b�t�@�𓾂�
	l  = strlen(pszConverting);						// �ϊ���������̕����񒷂𓾂�
	cp = g_Fep.CandBuf + l + 1;						// �ϊ�����������X�L�b�v
	for(i=0; i<ofs; i++)
	{
		n = strlen(cp);
		cp += n + 2;
	}
	pCandStart = cp;								// �\�������⃊�X�g�̐擪


	// ��⃊�X�g�̒�����ł���������������o��
	{
		// �t�H���g�T�C�Y��ݒ�
		iPixels = 0;
		nCands = 0;
		cp = pCandStart;
		for(i=0; i<10; i++)
		{
			if((ofs+i)>=g_Fep.nCandidates)
			{
				break;
			}

			// ��╶�����\���������̃s�N�Z�����𓾂�
			j = g_pFepImport->fep.getStrWidthSJIS(cp, mainChars);
			n = strlen(cp);							// ��╶����̒����𓾂�
			o = *(const u_char *)&cp[n + 1];		// �t����̃I�t�Z�b�g�ʒu���𓾂�
			if(o < l)
			{
				// �t�����\���������̃s�N�Z�����𓾂�
				j += g_pFepImport->fep.getStrWidthSJIS(&pszConverting[o], subChars);
			}
			if(iPixels < j)
			{
				iPixels = j;
			}
			nCands++;
			cp += n+2;
		}
	}

	return iPixels;
}


//============================================================================
//  Fep_AtokGetInputState										ref:  libfep.h
//
//  ATOK ���͏�Ԏ擾
//
//  �߂�l : int : ���͏��
int Fep_AtokGetInputState(void)
{
	#ifndef ATOK_NO_LINK
	if(!g_Fep.ucUseAtok)
	{
		return 0;
	}

	// ���͏�Ԃ��擾
	return sceAtokGetInputState(&g_Atok);
	#else
	return 0;
	#endif
}


//============================================================================
//  Fep_ShowCandList											ref:  libfep.h
//
//  ���ꗗ��\�����ׂ���
//
//  �߂�l : u_int : 0 �\�����Ȃ�
//					 1 �\������
u_int Fep_ShowCandList(void)
{
	#ifndef ATOK_NO_LINK
	if(g_Fep.state == SCE_ATOK_ISTATE_CANDIDATES)
	{
		return 1;	// �\������
	}
	#endif

	return 0;		// �\�����Ȃ�
}


//============================================================================
//  Fep_Changing												ref:  libfep.h
//
//  �ϊ������H
//
//  �߂�l : u_int : 0 �ϊ��O
//					 1 �ϊ���
u_int Fep_Changing(void)
{
	#ifndef ATOK_NO_LINK
	if(!g_Fep.ucUseAtok)
	{
		return 0;
	}

	{
		int s = sceAtokGetInputState(&g_Atok);

		if(s >= SCE_ATOK_ISTATE_CONVERTING)
		{
			return 1;	// �ϊ���
		}
		else
		{
			return 0;	// �ϊ��O
		}
	}
	#else
	return 0;
	#endif
}


//============================================================================
//  Fep_GetCaretPos												ref:  libfep.h
//
//  �L�����b�g�ʒu�擾
//
//  �߂�l : int : �L�����b�g�ʒu
int Fep_GetCaretPos(void)
{
	return g_Fep.nCaretPos;
}


//============================================================================
//  Fep_GetConvertChars											ref:  libfep.h
//
//  �ϊ��� ������擾
//
//  char *sjis : ������̃|�C���^(SJIS�̕����R�[�h�œ���)
void Fep_GetConvertChars(char *sjis)
{
	int len = strlen(g_Fep.szConv);
	if(len >= HENKAN_SIZE)
	{
		sjis[0] = '\0';
	}

	memcpy(sjis, g_Fep.szConv, HENKAN_SIZE);
	sjis[len] = '\0';
}


//============================================================================
//  Fep_GetClusterNum											ref:  libfep.h
//
//  ���ߐ��擾
//
//  �߂�l : int : ���ߐ�
int Fep_GetClusterNum(void)
{
	return g_Fep.nClusters;
}


//============================================================================
//  Fep_GetNowCluster											ref:  libfep.h
//
//  ���ڕ��߂�NO�擾
//
//  �߂�l : int : ����NO
int Fep_GetNowCluster(void)
{
	return g_Fep.FocusCls;
}


//============================================================================
//  Fep_GetClusterChars											ref:  libfep.h
//
//  ���� ������擾
//
//  int index : ���߃C���f�b�N�X
//
//  �߂�l : char* : ���ߕ�����ւ̃|�C���^
char *Fep_GetClusterChars(int index)
{
	return &g_Fep.aszClusters[index][0];
}


void Fep_ClearText(void)
{
	g_Fep.nCaretPos = 0;									// �L�����b�g�̑��݂���o�C�g�ʒu
	g_Fep.nTextLen  = 0;									// ���͍ςݕ�����̃o�C�g��
}


//============================================================================
//  Fep_SetText													ref:  libfep.h
//
//  �e�L�X�g�����ݒ�
//
//  const u_int *pUCS4 : �ݒ肵����������ւ̃|�C���^(UCS4)
void Fep_SetText(const u_int *pUCS4)
{
	int i;
	COOKED_CHAR cc;

	if(pUCS4 == NULL)
	{
		return;
	}
	Fep_ClearText();					// �e�L�X�g�N���A

	i=0;
	while(*(pUCS4+i))
	{
		cc.Char    = *(pUCS4+i);
		cc.Flag    = 0;
		cc.RawCode = 0;

		FifoPostData(&g_Fep.qFepOuput, &cc, sizeof(COOKED_CHAR));
		i++;
	}
}


//============================================================================
//  Fep_IsIdle													ref:  libfep.h
//
//  ���݂̏u�ԁAFEP���[�J�X���b�h���A�C�h����Ԃł��邩�ǂ����Ԃ�
//
//  �߂�l: int : ��A�C�h�����(0)
//                �A�C�h�����  (1)
int Fep_IsIdle(void)
{
	if(FifoGetBytes(&g_Fep.qKeyInput) > 0) {
		return(0);
	}

	#ifndef ATOK_NO_LINK
	if(g_Fep.ucUseAtok)
	{
		struct SemaParam sp;
		ReferSemaStatus(g_AtokRsrcSema, &sp);
		if(sp.currentCount==0) {
			return(0);
		}
	}
	#endif

	if(FifoGetBytes(&g_Fep.qFepOuput) > 0) {
		return(0);
	}

	return(1);
}



//============================================================================
//  Fep_SetInputMode											ref:  libfep.h
//
//  ���̓��[�h�ύX
//
//  fepAtokInputMode_m mode : ���[�h
//
//  �߂�l: int : ����I��(0)
//                �G���[  (1)
int Fep_SetInputMode(fepAtokInputMode_m mode)
{
	if(mode > (FEPMODE_MAX-1))
	{
		// �\�����Ȃ��l���͎�
		return 1;
	}

	if(!g_Fep.ucUseAtok)
	{
		// ATOK �g���ĂȂ�
		g_pFepImport->fep.changeMode(EX_ATOK_EDIT_KANJIOFF);	// �ŏ��o�[�Ɠ���������ׂɃR�[���o�b�N���Ă�
		return 0;
	}
	#ifndef ATOK_NO_LINK

	// ATOK�g���Ă��鎞
	if(mode == FEPOFF)
	{
		if(sceAtokGetKanjiState(&g_Atok))
		{
			Fep_Cmd(&g_Atok, SCE_ATOK_EDIT_DELETEALL);		// ATOK��[�폜]�v��
		}

		// FEP OFF
		if(sceAtokGetKanjiState(&g_Atok))
		{
			Fep_Cmd(&g_Atok, EX_ATOK_EDIT_KANJIOFF);
		}
		return 0;
	}
	else
	{
		u_int f;
	
		if(!sceAtokGetKanjiState(&g_Atok))
		{
			sceAtokKanjiOn(&g_Atok);				// ATOK���������̓��[�h��
			#if 0//SKB_EX Default 
			// �Ђ炪�ȂƂ͌���Ȃ��̂ŁA�J�b�g�B���̌�Đݒ肳���̂ŕK�v�Ȃ��B
			Fep_Cmd(&g_Atok, EX_ATOK_FIXOFF);
			Fep_Cmd(&g_Atok, EX_ATOK_DSPHIRAGANA);
			#endif
			sceAtokFlushConverted(&g_Atok);			// �m�蕶����o�b�t�@�̃t���b�V�����s��
		}
		else
		{
			Fep_Cmd(&g_Atok, SCE_ATOK_EDIT_DELETEALL);		// ATOK��[�폜]�v��
		}


		Fep_Cmd(&g_Atok, EX_ATOK_FIXOFF);		// �Ƃ肠����FIXOFF�𖳌��ɂ���
		switch(mode)
		{
		case FEPON_HIRA:
			f = EX_ATOK_DSPHIRAGANA;	// �S�p�Ђ炪�ȓ��̓��[�h
			break;
		case FEPON_HAN_KATAKANA:
			f = EX_ATOK_DSPHANKANA;		// ���p�J�^�J�i���̓��[�h
			break;
		case FEPON_ZEN_KATAKANA:
			f = EX_ATOK_DSPZENKANA;		// �S�p�J�^�J�i���̓��[�h
			break;
		case FEPON_HAN_ALPHABET:
			f = EX_ATOK_DSPHANRAW;		// ���p���ϊ����̓��[�h
			break;
		case FEPON_ZEN_ALPHABET:
			f = EX_ATOK_DSPZENRAW;		// �S�p���ϊ����̓��[�h
			break;
		#if 1//SKB_EX FixMode
		case FEPON_HIRA_FIX:
			f = EX_ATOK_FIXHIRAGANA;	// �Œ���S�p�Ђ炪�ȓ��̓��[�h
			break;
		case FEPON_HAN_KATAKANA_FIX:
			f = EX_ATOK_FIXHANKANA;		// �Œ�����p�J�^�J�i���̓��[�h
			break;
		case FEPON_ZEN_KATAKANA_FIX:
			f = EX_ATOK_FIXZENKANA;		// �Œ���S�p�J�^�J�i���̓��[�h
			break;
		case FEPON_HAN_ALPHABET_FIX:
			f = EX_ATOK_FIXHANRAW;		// �Œ�����p���ϊ����̓��[�h
			break;
		case FEPON_ZEN_ALPHABET_FIX:
			f = EX_ATOK_FIXZENRAW;		// �Œ���S�p���ϊ����̓��[�h
			break;
		#endif
		default:
			g_pFepImport->dprintf("ERROR : Fep_SetInputMode - unKnown.\n");
			return 1;
		}
		Fep_Cmd(&g_Atok, f);
	}
	#endif

	return 0;
}


//============================================================================
//  Fep_GetInsertChars											ref:  libfep.h
//
//  �㏑������镶����ւ̃|�C���^���擾
//
//  �߂�l: const u_int * : �㏑������镶����ւ̃|�C���^
const u_int *Fep_GetInsertChars(void)
{
	return g_Fep.szInsertText;
}


#ifndef ATOK_NO_LINK
//============================================================================
//  Fep_Cmd														ref:  libfep.h
//
//  FEP�R�}���h
//
//  sceAtokContext *pAtok : ATOK�ւ̃R���e�L�X�g�|�C���^
//  u_int f               : �����Ƃ�
//
//  �߂�l : int : ����I��(0)
//                 �R�}���h��`�G���[(1)
int Fep_Cmd(sceAtokContext *pAtok, u_int f)
{
	static int changeMuhenkan=0, changeMuhenkanRaw=0, lastMode=1;
	int s;

	if(g_Fep.ucUseAtok == 0)
	{
		return 0;
	}

	s = sceAtokGetInputState(pAtok);

	// [F10�L�[�������ꂽ��]
	// SCE_ATOK_EDIT_POSTCONVRAW���ŏ��̈���邱�Ƃɂ����
	// �uABCDE�v���uAbcde�v���uabcde�v�ɂȂ�܂��B
	if(EX_ATOK_EDIT_POSTRAWHALF != f)
	{
		g_Fep.cntF10 = 0;
	}

	// ATOK�G���W���ɃR�}���h�𑗏o
	if(!(f & EX_ATOK_EDIT_NULL))
	{
		// ��`�ς݃t�@���N�V�����������ꍇ
		// ���̂܂܃e�[�u�����瓾���l���g���ăR�}���h���M
		if(s==SCE_ATOK_ISTATE_MOVECLAUSEGAP && (f & 0x200))
		{
			// ���ߋ�؂�ύX���Ɏ��̃L�[�������ꍇ
			sceAtokEditConv(pAtok, SCE_ATOK_EDIT_KAKUTEIALL, 0);
			Fep_SetData2Fifo();
		}
		sceAtokEditConv(pAtok, f, 0);

	} else {
		// �����t�@���N�V�����������ꍇ
		sceAtokConfig cfg;
		cfg.Size = sizeof(sceAtokConfig);

		if(f == EX_ATOK_EDIT_CURSTAY)
		{
			// �_�~�[
			return(0);
		}

		if(EX_ATOK_CHANGE_DSPKANA != f)
		{
			// ���ϊ��t���O������(�J�i)
			changeMuhenkan = 0;
		}
		if(EX_ATOK_CHANGE_DSPRAW != f)
		{
			// ���ϊ��t���O������(�A���t�@�x�b�g)
			changeMuhenkanRaw = 0;
		}

		switch(f) {
			case EX_ATOK_EDIT_KANJIOFF:
				// �ϊ���������S����
				if(s == SCE_ATOK_ISTATE_BEFORECONVERT)
				{
					// �ϊ��O
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_DELETEALL, 0);		// ATOK��[����]�𑗂�
				}
				else
				{
					// �ϊ���
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_CONVCANCEL, 0);		// ATOK��[�ϊ��L�����Z��]�𑗂�
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_DELETEALL, 0);		// ATOK��[����]�𑗂�
				}

				// �������̓��[�hOFF
				sceAtokKanjiOff(pAtok);
				g_pFepImport->fep.atokOFF(pAtok);
				g_pFepImport->fep.changeMode(EX_ATOK_EDIT_KANJIOFF);			// �ŏ��o�[�Ɠ���������ׂɃR�[���o�b�N���Ă�
				break;

			case EX_ATOK_EDIT_KANALOCK:
				// �J�i���b�N�؂�ւ�
				g_Fep.led ^= USBKB_LED_KANA;
				break;

			case EX_ATOK_DSPHIRAGANA:		// �Ђ炪�ȓ���
			case EX_ATOK_DSPZENKANA:		// �S�p�J�^�J�i����
			case EX_ATOK_DSPZENRAW:			// �S�p���ϊ�����
			case EX_ATOK_DSPHANKANA:		// ���p�J�^�J�i����
			case EX_ATOK_DSPHANRAW:			// ���p���ϊ�����
				// ���͕�����ݒ�
				// CAUTION: SCE_ATOK_ISTATE_BEFOREINPUT�ł���������̐؂�ւ����ł��Ȃ�
				g_Fep.KanDspType = (f - EX_ATOK_DSPHIRAGANA) + 1;
				cfg.KanDspType = g_Fep.KanDspType;
				sceAtokSetConfig(pAtok, &cfg, SCE_ATOK_CONFIG_KANDSPTYPE);
				if(EX_ATOK_DSPZENRAW != f && EX_ATOK_DSPHANRAW!= f)
				{
					lastMode = g_Fep.KanDspType;
				}
				g_pFepImport->fep.changeMode(f);		// �ŏ��o�[�Ɠ���������ׂɃR�[���o�b�N���Ă�
				break;

			case EX_ATOK_DSPSWEISU:			// �p���؂�ւ�
				// CAUTION: SCE_ATOK_ISTATE_BEFOREINPUT�ł���������̐؂�ւ����ł��Ȃ�
				if(g_Fep.KanDspType==3) {
					// ���łɑS�p���ϊ����̓��[�h�������Ƃ��A
					g_Fep.KanDspType = 5;		// ���p���ϊ�����
				} else {
					g_Fep.KanDspType = 3;		// �S�p���ϊ�����
				}
				cfg.KanDspType = g_Fep.KanDspType;
				sceAtokSetConfig(pAtok, &cfg, SCE_ATOK_CONFIG_KANDSPTYPE);
				g_pFepImport->fep.changeMode(EX_ATOK_DSPHIRAGANA+g_Fep.KanDspType-1);							// �ŏ��o�[�Ɠ���������ׂɃR�[���o�b�N���Ă�
				break;

			case EX_ATOK_DSPSWKANA:			// ���Ȑ؂�ւ�
				// CAUTION: SCE_ATOK_ISTATE_BEFOREINPUT�ł���������̐؂�ւ����ł��Ȃ�
				if(g_Fep.KanDspType==1) {
					// �S�p�Ђ炪�ȓ��̓��[�h�������Ƃ�
					g_Fep.KanDspType = 2;		// �S�p�J�^�J�i����
				} else if(g_Fep.KanDspType==2) {
					// �S�p�������ȓ��̓��[�h�������Ƃ��A
					g_Fep.KanDspType = 4;		// ���p�J�^�J�i����
				} else {
					g_Fep.KanDspType = 1;		// �S�p�Ђ炪�ȓ���
				}
				cfg.KanDspType = g_Fep.KanDspType;
				sceAtokSetConfig(pAtok, &cfg, SCE_ATOK_CONFIG_KANDSPTYPE);
				g_pFepImport->fep.changeMode(EX_ATOK_DSPHIRAGANA+g_Fep.KanDspType-1);							// �ŏ��o�[�Ɠ���������ׂɃR�[���o�b�N���Ă�
				break;


			case EX_ATOK_FIX_ZENHIRA_HANRAW:
				// [��][�A][�]��[A]
				if(SCE_ATOK_ISTATE_BEFORECONVERT > s)
				{
					g_Fep.cntFix ^= 1;

					if(g_Fep.KanDspType == lastMode)
					{
						// �O�̓��͎�ނƌ��݂̓��͎�ނ��ꏏ�̏ꍇ�́A�����I�ɖ��ϊ��ɂ�����
						g_Fep.cntFix = 1;
					}

					if(g_Fep.cntFix && g_Fep.KanDspType != 5 && g_Fep.KanDspType != 3)
					{
						// �Ђ炪�ȁA���p�J�^�J�i�A�S�p�J�^�J�i�������烂�[�h�ۑ�
						lastMode = g_Fep.KanDspType;
						// [A]�ɂ���
						g_Fep.KanDspType = 5;		// ���p���ϊ�����
					}
					else
					{
						g_Fep.KanDspType = lastMode;
					}
					cfg.FixMode = EX_ATOK_FIXOFF-EX_ATOK_FIXOFF;
					sceAtokSetConfig(pAtok, &cfg, SCE_ATOK_CONFIG_FIXMODE);					// FIX���[�h��OFF�ɂ���
					cfg.KanDspType = g_Fep.KanDspType;
					sceAtokSetConfig(pAtok, &cfg, SCE_ATOK_CONFIG_KANDSPTYPE);
					g_pFepImport->fep.changeMode(EX_ATOK_DSPHIRAGANA+g_Fep.KanDspType-1);							// �ŏ��o�[�Ɠ���������ׂɃR�[���o�b�N���Ă�
				}
				break;

			case EX_ATOK_FIX_ZENHIRA_ZENRAW:
				// [��][�A][�]��[�`]
				if(SCE_ATOK_ISTATE_BEFORECONVERT > s)
				{
					g_Fep.cntFixShift ^= 1;

					if(g_Fep.KanDspType == lastMode)
					{
						// �O�̓��͎�ނƌ��݂̓��͎�ނ��ꏏ�̏ꍇ�́A�����I�ɖ��ϊ��ɂ�����
						g_Fep.cntFixShift = 1;
					}


					if(g_Fep.cntFixShift && g_Fep.KanDspType != 5 && g_Fep.KanDspType != 3)
					{
						// [�Ђ炪�ȓ���]�A[���p�J�^�J�i����]�A[�S�p�J�^�J�i����]�������烂�[�h�ۑ�
						lastMode = g_Fep.KanDspType;

						g_Fep.KanDspType = 3;		// �S�p���ϊ�����
					}
					else
					{
						g_Fep.KanDspType = lastMode;
					}
					cfg.FixMode = EX_ATOK_FIXOFF-EX_ATOK_FIXOFF;
					sceAtokSetConfig(pAtok, &cfg, SCE_ATOK_CONFIG_FIXMODE);					// FIX���[�h��OFF�ɂ���
					cfg.KanDspType = g_Fep.KanDspType;
					sceAtokSetConfig(pAtok, &cfg, SCE_ATOK_CONFIG_KANDSPTYPE);
					g_pFepImport->fep.changeMode(EX_ATOK_DSPHIRAGANA+g_Fep.KanDspType-1);							// �ŏ��o�[�Ɠ���������ׂɃR�[���o�b�N���Ă�
				}
				break;

			case EX_ATOK_FIX_ZENHIRA_ZENKANA_HANKANA:
				if(SCE_ATOK_ISTATE_BEFORECONVERT > s)
				{
					if(g_Fep.KanDspType == 5 || g_Fep.KanDspType == 3)
					{
						// ���[�h���Œ肳����ׂ̍��
						int chtbl[] = {0,4,1,0,2};
						g_Fep.KanDspType = chtbl[lastMode];
					}

					switch(g_Fep.KanDspType)
					{
					case 2:
						// �S�p�J�^�J�i���͂������ꍇ
						g_Fep.KanDspType = 4;		// ���p�J�^�J�i����
						break;

					case 1:
						// �Ђ炪�ȓ��͂������ꍇ
						g_Fep.KanDspType = 2;		// �S�p�J�^�J�i����
						break;

					case 4:
						// ���p�J�^�J�i���͂������ꍇ
					default:
						g_Fep.KanDspType = 1;		// �S�p�Ђ炪�ȓ���
						break;
					}
					lastMode = cfg.KanDspType = g_Fep.KanDspType;
					sceAtokSetConfig(pAtok, &cfg, SCE_ATOK_CONFIG_KANDSPTYPE);
					g_pFepImport->fep.changeMode(EX_ATOK_DSPHIRAGANA+g_Fep.KanDspType-1);							// �ŏ��o�[�Ɠ���������ׂɃR�[���o�b�N���Ă�
				}
				g_pFepImport->dprintf("--- %d ---, %d\n", __LINE__, g_Fep.KanDspType);
				break;

			case EX_ATOK_CHANGE_DSPKANA:

				if(changeMuhenkan == 0)
				{
					changeMuhenkan = 4;
				}

				switch(changeMuhenkan)
				{
				case 2:
					// �S�p�J�^�J�i����
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVKANA, 0);	// ��ϊ��J�^�J�i
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVHALF, 0);	// ��ϊ����p
					changeMuhenkan = 4;
					break;
				case 1:
					// �S�p�Ђ炪�ȓ���
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVKANA, 0);	// ��ϊ��J�^�J�i
					changeMuhenkan = 2;
					break;
				case 4:
					// ���p�J�^�J�i����
				default:
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVHIRA, 0);	// ��ϊ��Ђ炪��
					changeMuhenkan = 1;
					break;
				}

				break;

			case EX_ATOK_CHANGE_DSPRAW:
				if(changeMuhenkanRaw == 0)
				{
					changeMuhenkanRaw = 5;
				}

				if(changeMuhenkanRaw == 3)
				{
					// �S�p���ϊ�����
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVRAW, 0);	// ��ϊ����ϊ�
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVHALF, 0);	// ��ϊ����p
					changeMuhenkanRaw = 5;
				}
				else
				{
					// �S�p���ϊ�����
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVRAW, 0);	// ��ϊ����ϊ�
					changeMuhenkanRaw = 3;
				}
				break;

			case EX_ATOK_FIXOFF:			// �Œ���̓��[�hOFF
			case EX_ATOK_FIXHIRAGANA:		// �Œ���͂Ђ炪��
			case EX_ATOK_FIXZENKANA:		// �Œ���͑S�p�J�^�J�i
			case EX_ATOK_FIXZENRAW:			// �Œ���͑S�p���ϊ�
			case EX_ATOK_FIXHANKANA:		// �Œ���͔��p�J�^�J�i
			case EX_ATOK_FIXHANRAW:			// �Œ���͔��p���ϊ�
				// �Œ���̓��[�h�̐ݒ�
				cfg.FixMode = f - EX_ATOK_FIXOFF;
				sceAtokSetConfig(pAtok, &cfg, SCE_ATOK_CONFIG_FIXMODE);
				g_pFepImport->fep.changeMode(f);						// �ŏ��o�[�Ɠ���������ׂɃR�[���o�b�N���Ă�

				break;

			case EX_ATOK_EDIT_CANDNEXT:
				// �����
				if(g_Fep.nCurrentCand < (g_Fep.nCandidates - 1)) {
					g_Fep.nCurrentCand++;
				} else {
					g_Fep.nCurrentCand = 0;
				}
				g_Fep.nCandOffset  = (g_Fep.nCurrentCand / 10) * 10;
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_SELECTCAND, (g_Fep.nCurrentCand + 1));
				#if 1//SKB_EX Sound
				Fep_se_play(SKBSE_MOVECANDLIST);
				#endif
				break;

			case EX_ATOK_EDIT_CANDPREV:
				// �O���
				if(g_Fep.nCurrentCand) {
					g_Fep.nCurrentCand--;
				} else {
					g_Fep.nCurrentCand = g_Fep.nCandidates - 1;
				}
				g_Fep.nCandOffset  = (g_Fep.nCurrentCand / 10) * 10;
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_SELECTCAND, (g_Fep.nCurrentCand + 1));
				#if 1//SKB_EX Sound
				Fep_se_play(SKBSE_MOVECANDLIST);
				#endif
				break;


			case EX_ATOK_EDIT_CANDHEAD:
				// ���擪
				g_Fep.nCurrentCand = 0;
				g_Fep.nCandOffset  = 0;
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_SELECTCAND, (g_Fep.nCurrentCand + 1));
				break;

			case EX_ATOK_EDIT_CANDTAIL:
				// ��▖���i�\�����́j
				g_Fep.nCurrentCand = g_Fep.nCandidates - 1;
				g_Fep.nCandOffset  = (g_Fep.nCurrentCand / 10) * 10;
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_SELECTCAND, (g_Fep.nCurrentCand + 1));
				break;

			case EX_ATOK_EDIT_CANDGRPNEXT:
				// �����Q
				if(((g_Fep.nCandOffset/10)*10 + 10) < g_Fep.nCandidates) {
					g_Fep.nCandOffset = (g_Fep.nCandOffset/10 + 1)*10;
				} else {
					g_Fep.nCandOffset = 0;
				}
				g_Fep.nCurrentCand = g_Fep.nCandOffset;
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_SELECTCAND, (g_Fep.nCurrentCand + 1));
				break;

			case EX_ATOK_EDIT_CANDGRPPREV:
				// �O���Q
				if((g_Fep.nCandOffset - 10) >= 0) {
					g_Fep.nCandOffset -= 10;
				} else {
					g_Fep.nCandOffset = ((g_Fep.nCandidates - 1)/10) * 10;
				}
				if((g_Fep.nCandOffset + 9) < g_Fep.nCandidates) {
					g_Fep.nCurrentCand = g_Fep.nCandOffset + 9;
				} else {
					g_Fep.nCurrentCand = g_Fep.nCandidates - 1;
				}
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_SELECTCAND, (g_Fep.nCurrentCand + 1));
				break;

			case EX_ATOK_EDIT_CLSEND_CAND:
				// �Ō���
				if(g_Fep.FocusCls == (g_Fep.nClusters-1))
				{
					break;
				}
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_MOVFOCUSCLAUS, 2);		// ���ߖ���
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_CONVFORWARD  , 0);		// ATOK��[�ϊ��E������]�𑗂�
				g_Fep.stateCandLeftEnd = EX_ATOK_EDIT_CLSEND_CAND;			// ���ꗗ�X�V
				break;

			case EX_ATOK_EDIT_CLSTOP_CAND:
				// �擪���
				if(g_Fep.FocusCls == 0)
				{
					break;
				}
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_MOVFOCUSCLAUS, 3);		// ���ߐ擪
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_CONVFORWARD , 0);		// ATOK��[�ϊ��E������]�𑗂�
				g_Fep.stateCandLeftEnd = EX_ATOK_EDIT_CLSTOP_CAND;			// ���ꗗ�X�V
				break;

			case EX_ATOK_EDIT_CLSRIGHT:		// ���߉E
			case EX_ATOK_EDIT_CLSLEFT:		// ���ߍ�
			case EX_ATOK_EDIT_CLSEND:		// ���ߖ���
			case EX_ATOK_EDIT_CLSTOP:		// ���ߐ擪
				// ���ߋ�؂�ύX���ɂ͒��ڕ��߂��ړ��ł��Ȃ��̂ŁA
				// ���������ϊ��R�}���h�𔭍s���ASCE_ATOK_ISTATE_MOVECLAUSEGAP��Ԃ�E�o����
				if(s==SCE_ATOK_ISTATE_MOVECLAUSEGAP) {
					// �ǂ̕�����Ɍ�ϊ����邩�A����������͕�������擾���Ă���ׂ�B
					switch(g_Fep.KanDspType) {
						case 2:
							// �S�p�J�^�J�i����
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVKANA, 0);	// ��ϊ��J�^�J�i
							break;
						case 3:
							// �S�p���ϊ�����
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVRAW,  0);	// ��ϊ����ϊ�
							break;
						case 4:
							// ���p�J�^�J�i����
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVKANA, 0);	// ��ϊ��J�^�J�i
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVHALF, 0);	// ��ϊ����p
							break;
						case 5:
							// ���p���ϊ�
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVRAW,  0);	// ��ϊ����ϊ�
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVHALF, 0);	// ��ϊ����p
							break;
						case 1:
							// �S�p�Ђ炪�ȓ���
						default:
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVHIRA, 0);	// ��ϊ��Ђ炪��
							break;
					}
				}
				// ���߂̈ړ����@���Ƃɕ���
				switch(f) {
					case EX_ATOK_EDIT_CLSRIGHT:		// ���߉E
						sceAtokEditConv(pAtok, SCE_ATOK_EDIT_MOVFOCUSCLAUS, 0);
						break;
					case EX_ATOK_EDIT_CLSLEFT:		// ���ߍ�
						sceAtokEditConv(pAtok, SCE_ATOK_EDIT_MOVFOCUSCLAUS, 1);
						break;
					case EX_ATOK_EDIT_CLSEND:		// ���ߖ���
						sceAtokEditConv(pAtok, SCE_ATOK_EDIT_MOVFOCUSCLAUS, 2);
						break;
					case EX_ATOK_EDIT_CLSTOP:		// ���ߐ擪
						sceAtokEditConv(pAtok, SCE_ATOK_EDIT_MOVFOCUSCLAUS, 3);
						break;
				}
				#if 1//SKB_EX Sound
				Fep_se_play(SKBSE_MOVECANDLIST);
				#endif
				break;

			case EX_ATOK_EDIT_REVCLSRIGHT:		// ���߉E�z��
			case EX_ATOK_EDIT_REVCLSLEFT:		// ���ߍ��z��
				// ���ߋ�؂�ύX���ɂ͒��ڕ��߂��ړ��ł��Ȃ��̂ŁA
				// ���������ϊ��R�}���h�𔭍s���ASCE_ATOK_ISTATE_MOVECLAUSEGAP��Ԃ�E�o����
				if(s==SCE_ATOK_ISTATE_MOVECLAUSEGAP) {
					// �ǂ̕�����Ɍ�ϊ����邩�A����������͕�������擾���Ă���ׂ�B
					switch(g_Fep.KanDspType) {
						case 2:
							// �S�p�J�^�J�i����
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVKANA, 0);	// ��ϊ��J�^�J�i
							break;
						case 3:
							// �S�p���ϊ�����
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVRAW,  0);	// ��ϊ����ϊ�
							break;
						case 4:
							// ���p�J�^�J�i����
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVKANA, 0);	// ��ϊ��J�^�J�i
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVHALF, 0);	// ��ϊ����p
							break;
						case 5:
							// ���p���ϊ�
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVRAW,  0);	// ��ϊ����ϊ�
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVHALF, 0);	// ��ϊ����p
							break;
						case 1:
							// �S�p�Ђ炪�ȓ���
						default:
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVHIRA, 0);	// ��ϊ��Ђ炪��
							break;
					}
				}
				// ���߂̈ړ����@���Ƃɕ���
				switch(f) {
					case EX_ATOK_EDIT_REVCLSRIGHT:		// ���߉E�z��
						if((g_Fep.nClusters-1) == g_Fep.FocusCls)
						{
							// �ŉE�[�Ȃ̂ŕ��ߐ擪��
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_MOVFOCUSCLAUS, 3);
						}
						else
						{
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_MOVFOCUSCLAUS, 0);
						}
						break;
					case EX_ATOK_EDIT_REVCLSLEFT:		// ���ߍ��z��
						if(g_Fep.FocusCls == 0)
						{
							// �ō��[�Ȃ̂ŕ��ߖ�����
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_MOVFOCUSCLAUS, 2);
						}
						else
						{
							sceAtokEditConv(pAtok, SCE_ATOK_EDIT_MOVFOCUSCLAUS, 1);
						}
						break;
				}
				#if 1//SKB_EX Sound
				Fep_se_play(SKBSE_MOVECLS);
				#endif
				break;
			case EX_ATOK_EDIT_POSTRAWHALF:
				// ���p�p���ւ̕����ϊ�
				// �܂����ϊ�������ɕϊ��������ƁA�����Ĕ��p������ւ̕ϊ�
				// CAUTION: ���p�J�i�ɂȂ�\��������܂�
				if(g_Fep.cntF10==0)
				{
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVRAW,  0);
					g_Fep.cntF10++;
				}
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_POSTCONVHALF, 0);
				break;

			case EX_ATOK_EDIT_KAKUTEISPACE:
				// �m���X�y�[�X�}��
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_KAKUTEIALL, 0);
				Fep_SetData2Fifo();					// �m�蕶����FIFO�ɐݒ�
				sceAtokEditConv(pAtok, ' ', 0);
				#if 1//SKB_EX Sound
				Fep_se_play(SKBSE_ONSPACE);
				#endif
				break;
			case EX_ATOK_EDIT_KAKUTEIHANSPACE:
				// �m��㔼�p�X�y�[�X�}��
				sceAtokEditConv(pAtok, SCE_ATOK_EDIT_KAKUTEIALL, 0);
				Fep_SetData2Fifo();					// �m�蕶����FIFO�ɐݒ�
				{
					// ���p�X�y�[�X����
					COOKED_CHAR c;
					c.Flag    = 0x0;
					c.RawCode = 0x0000;
					c.Char    = 0x00000020;
					FifoSendData(&g_Fep.qFepOuput, &c, sizeof(COOKED_CHAR));
				}
				#if 1//SKB_EX Sound
				Fep_se_play(SKBSE_ONSPACE);
				#endif
				break;

			case EX_ATOK_EDIT_CAND1:
			case EX_ATOK_EDIT_CAND2:
			case EX_ATOK_EDIT_CAND3:
			case EX_ATOK_EDIT_CAND4:
			case EX_ATOK_EDIT_CAND5:
			case EX_ATOK_EDIT_CAND6:
			case EX_ATOK_EDIT_CAND7:
			case EX_ATOK_EDIT_CAND8:
			case EX_ATOK_EDIT_CAND9:
			case EX_ATOK_EDIT_CAND0:
				{
					int no = 0;
					no = (g_Fep.nCandOffset + (f - EX_ATOK_EDIT_CAND1)+1);
					if(no > g_Fep.nCandidates)
					{
						// ���͂�����������␔���傫�������ꍇ
						return 0;
					}
					// �e���L�[��[0]�`[9]�������ꂽ�Ƃ��A��⃊�X�g���畔���m��
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_KAKUTEIPART, no);
				}
				break;

			default:
				// ��`�G���[
				return(1);
		}
	}

	getContext();
	return(0);
}
#endif


//============================================================================
//  Fep_SetKakuteiPart											ref:  libfep.h
//
//  �����m���ATOK�G���W���ɑ���
//
//  �߂�l: void
void Fep_SetKakuteiPart(void)
{
	#ifndef ATOK_NO_LINK
	if(!g_Fep.ucUseAtok)
	{
		// ATOK �g���Ă��Ȃ�
		return;
	}

	if(g_Fep.nIsConv != CONV_OFF)
	{
		g_pFepImport->dprintf("ERROR : Fep_SetKakuteiPart - convert on\n");
		return;
	}

	// KAKUTEIPART�𓊂���
	Fep_PostApp(APP_KAKUTEIPART, 0, 0);
	#endif
}


//============================================================================
//  Fep_SetKakuteiAll											ref:  libfep.h
//
//  �S���m���ATOK�G���W���ɑ���
//
//  �߂�l: void
void Fep_SetKakuteiAll(void)
{
	#ifndef ATOK_NO_LINK
	if(!g_Fep.ucUseAtok)
	{
		// ATOK �g���Ă��Ȃ�
		return;
	}

	if(g_Fep.nIsConv != CONV_OFF)
	{
		g_pFepImport->dprintf("ERROR : Fep_SetKakuteiPart - convert on\n");
		return;
	}

	// KAKUTEIALL�𓊂���
	Fep_PostApp(APP_KAKUTEIALL, 0, 0);
	#endif
}


//============================================================================
//  Fep_GetNowConv												ref:  libfep.h
//
//  ATOK�����ݕϊ������H
//
//  �߂�l: int : 0:�ϊ�������Ȃ�
//                1:�ϊ���
int Fep_GetNowConv(void)
{
	#ifndef ATOK_NO_LINK
	if(!g_Fep.ucUseAtok)
	{
		// ATOK �g���Ă��Ȃ�
		return 0;
	}

	return g_Fep.nIsConv;
	#else
	return 0;
	#endif
}


//============================================================================
//  Fep_GetNowAtokInput											ref:  libfep.h
//
//  ATOK�g���ē��͒����H
//
//  //�߂�l: int : 0:����͒�
//  //              1:���͒�
//  //SKB_EX 
//  �߂�l: int : 0: ��ϊ����͒�
//                1: ���[�}���ϊ����͒�
//                2: ���ȕϊ����͒�
//                3: ���p�X���[���͒�
int Fep_GetNowAtokInput(void)
{
	#ifndef ATOK_NO_LINK
	if(!g_Fep.ucUseAtok)
	{
		// ATOK �g���Ă��Ȃ�
		return 0;
	}

	{
		#if 1//SKB_EX 
		sceAtokConfig cfg;
		cfg.Size = sizeof(sceAtokConfig);
		sceAtokGetConfig(&g_Atok,&cfg,SCE_ATOK_CONFIG_KANJIINPUT);

		return cfg.KanjiInput;
		#else
		int s = sceAtokGetInputState(&g_Atok);
		if(s == SCE_ATOK_ISTATE_BEFORECONVERT )
		{
			return 0;
		}
		else
		{
			return 1;
		}
		#endif
	}
	#else
	return 0;
	#endif
}


//============================================================================
//  Fep_SetKanjiCancel											ref:  libfep.h
//
//  �L�����Z����ATOK�G���W���ɑ���
//
//  �߂�l: void
void Fep_SetKanjiCancel(void)
{
	#ifndef ATOK_NO_LINK
	int s;
	if(!g_Fep.ucUseAtok)
	{
		// ATOK �g���ĂȂ�
		return;
	}

	s = Fep_AtokGetInputState();
	if(s == SCE_ATOK_ISTATE_BEFOREINPUT)
	{
		g_pFepImport->fep.cancel();
	}
	else if(s == SCE_ATOK_ISTATE_BEFORECONVERT)
	{
		Fep_PostApp(APP_DELETEALL, 0, 0);
	}
	else
	{
		Fep_PostApp(APP_CONVCANCEL, 0, 0);
	}
	#endif
}


//============================================================================
//  Fep_ConvStrings												ref:  libfep.h
//
//  ���͈ʒu�ȑO�̕�����擾
//  # �L�����b�g�`�掞�ɕK�v�ɂȂ�E�E�E�E
//
//  char *str_sjis : �i�[�p������o�b�t�@
//
//  void : �߂�l
void Fep_ConvStrings(char *str_sjis)
{
	memcpy(str_sjis, g_Fep.szConv, g_Fep.FocusTop);
	str_sjis[g_Fep.FocusTop] = '\0';
}






static void workerMain(int param __attribute__((unused)))
{
	sceAtokContext *pAtok = NULL;
	#ifndef ATOK_NO_LINK
	if(g_Fep.ucUseAtok)
	{
		pAtok = &g_Atok;
	}
	#endif

	while(1)
	{
		// FIFO����V�����L�[�R�[�h���擾����(WAIT����)
		INPUT_QUEUE_ELEM q;

		FifoRecvData(&g_Fep.qKeyInput, &q, sizeof(INPUT_QUEUE_ELEM), 0);

		#ifndef ATOK_NO_LINK
		if(g_Fep.ucUseAtok)
		{
			WaitSema(g_AtokRsrcSema);				// ATOK�G���W���̔r���I�g�p�����m��
			// ATOK�G���W���̓��쒆�͓d���}�X�N
			g_pFepImport->fep.powerLock();
		}
		#endif

		// �L���[�̐擪�ɂ������v�f�̃��b�Z�[�W�R����ʂɉ����ĕ���
		switch(q.type)
		{
		case INPUTTYPE_KEYBOARD:
			// �L�[�{�[�h�R���̃��b�Z�[�W�������ꍇ
			FepTreatKey(pAtok, q.kb.led, q.kb.mkey, q.kb.keycode);
			break;
		case INPUTTYPE_SOFTKB:
			// �p�b�h�R���̃��b�Z�[�W�������ꍇ
			FepTreatSkb(pAtok, q.skb.usCtrlCode, q.skb.ucFlag, q.skb.puiChar);
			break;

		case INPUTTYPE_APP:
			// �A�v���R���̃��b�Z�[�W�������ꍇ
			FepTreatApp(pAtok, q.msg.cmd, q.msg.arg0, q.msg.arg1);
			break;
		}

		#ifndef ATOK_NO_LINK
		if(g_Fep.ucUseAtok)
		{
			// �m�蕶�����FIFO�ɐݒ�
			Fep_SetData2Fifo();

			// �ϊ��I��
			g_Fep.nIsConv = CONV_OFF;

			// �d�����b�N������
			g_pFepImport->fep.powerUnlock();

			SignalSema(g_AtokRsrcSema);				// ATOK�G���W���̎g�p�����������
		}
		#endif
	}
}

static void FepTreatApp(sceAtokContext *pAtok, u_int cmd, u_int arg0, u_int arg1)
{
	switch(cmd)
	{
	case APP_INTERRUPT_KEY:
		interruptFromKey();
		break;
	#ifndef ATOK_NO_LINK
	case APP_KAKUTEIPART:
		Fep_Cmd(&g_Atok, SCE_ATOK_EDIT_KAKUTEIPART);	// ATOK��[�����m��]�𑗂�
		break;
	case APP_KAKUTEIALL:
		Fep_Cmd(&g_Atok, SCE_ATOK_EDIT_KAKUTEIALL);		// ATOK��[�S���m��]�𑗂�
		break;
	case APP_CONVCANCEL:
		Fep_Cmd(pAtok, SCE_ATOK_EDIT_CONVCANCEL);		// ATOK��[�ϊ��L�����Z��]�𑗂�
		break;
	case APP_DELETEALL:
		Fep_Cmd(&g_Atok, SCE_ATOK_EDIT_DELETEALL);		// ATOK��[�폜]�v��
		break;
	#endif
	}
}



#ifndef ATOK_NO_LINK
//============================================================================
//  Fep_SetData2Fifo											ref:  libfep.h
//
//  �����������ATOK�G���W�����������蕶����o�b�t�@�ɐݒ�
//  (SJIS -> UCS2)
//
//  �߂�l: void
void Fep_SetData2Fifo(void)
{
	char buf[KAKUTEI_SIZE];
	COOKED_CHAR cc;
	int len;
	int i;
	sceCccJISCS jis;
	sceCccUCS4 chr=0;
	sceCccSJISTF const* pSjis;

	// �m��\�L������͐V�������͂��͂��܂�Ə����Ă��܂��\��������̂ŁA����������Ȃ��Ă͂Ȃ�Ȃ�
	len = sceAtokGetConvertedStr(&g_Atok, buf, KAKUTEI_SIZE);
	if(len == 0)
	{
		return;
	}

	pSjis = (sceCccSJISTF *)buf;
	i=0;len=0;
	while(*pSjis)
	{
		// SJIS -> UCS4
		jis = sceCccDecodeSJIS(&pSjis);
		cc.Char    = sceCccJIStoUCS(jis, chr);
		cc.Flag    = 0;
		cc.RawCode = 0;
		FifoSendData(&g_Fep.qFepOuput, &cc, sizeof(COOKED_CHAR));

		len++;
	}

	// �m�蕶����o�b�t�@�̃t���b�V�����s��
	sceAtokFlushConverted(&g_Atok);
}
#endif



//============================================================================
// STATIC FUNCTIONS
//============================================================================
//============================================================================
//	postKey
//
//	FEP���C���̂�����FIFO�L���[�ɃL�[����o�^
//
//	u_char led : LED���
//	u_char mkey: ���f�B�t�@�C�A�L�[���
//	u_short keycode : USB�L�[�R�[�h
static void postKey(u_char led, u_char mkey, u_short keycode)
{
	INPUT_QUEUE_ELEM q;
	sceAtokContext *pAtok = NULL;
	if(g_Fep.ucUseAtok)
	{
		pAtok = &g_Atok;
	}

	q.kb.type    = INPUTTYPE_KEYBOARD;
	q.kb.led     = led;
	q.kb.mkey    = mkey;
	q.kb.keycode = keycode;
	FifoPostData(&g_Fep.qKeyInput, &q, sizeof(INPUT_QUEUE_ELEM));
	return;
}


//============================================================================
//	interruptFromKey
//
//	�L�[�{�[�h����̓��͂���������
//	�\�t�g�E�F�A�L�[�{�[�h�����B
static void interruptFromKey(void)
{
	#ifndef ATOK_NO_LINK
	sceAtokContext *pAtok = NULL;

	if(g_Fep.ucUseAtok)
	{
		pAtok = &g_Atok;
	}
	else
	#endif
	{
		// ATOK�g���ĂȂ�
		g_pFepImport->fep.closeSoftKB();		// �\�t�g�E�F�A�L�[�{�[�h�N���[�Y
		return;
	}

	#ifndef ATOK_NO_LINK
	if(!sceAtokGetKanjiState(pAtok))
	{
		// ���[�h���Ђ炪�Ȃɂ���(KBD��SOFTKB�œ��������Ȃ�)
		sceAtokKanjiOn(pAtok);						// ATOK���������̓��[�h��
		sceAtokFlushConverted(pAtok);				// �m�蕶����o�b�t�@�̃t���b�V�����s��
	}
	else
	{
		// USB�L�[�{�[�h���͈ȊO�ł̓��͕������������疳���ɂ���
		if(sceAtokGetInputState(pAtok) != SCE_ATOK_ISTATE_BEFOREINPUT)
		{
			Fep_Cmd(pAtok, SCE_ATOK_EDIT_DELETEALL);
		}
	}
	Fep_Cmd(pAtok, EX_ATOK_DSPHIRAGANA);
	g_pFepImport->fep.closeSoftKB();		// �\�t�g�E�F�A�L�[�{�[�h�N���[�Y
	#endif
}



#ifndef ATOK_NO_LINK
//============================================================================
//	getContext
//
//	sceAtokContext����A�\���̂��߂ɕK�v�ȏ����擾����
//	�������AATOK�G���W���̔r���I�g�p���������Ȃ������ꍇ�ɂ͂Ȃɂ����Ȃ��B
//
//	�߂�l : int : 0:�r���I�g�p���������Ȃ�����/1:�K�v�ȏ�񂪎擾�ł���
static int getContext(void)
{
	int i;
	int state;
	int focus=-1;

	if(g_Fep.ucUseAtok)
	{
		// ATOK�G���W���̔r���I�g�p�����m�ۂ����݂�
		i = PollSema(g_AtokRsrcSema);
		if(i<0)
		{
			// �r���I�g�p���������Ȃ������Ƃ��Ag_Fep�̒��g�ɂ͎�������ɏI��
			return(0);
		}

		// ATOK�̌��݂�ON/OFF��Ԃ𓾂�
		g_Fep.mode = sceAtokGetKanjiState(&g_Atok);

		sceAtokGetConvertingStr(&g_Atok, (char*)g_Fep.szConv, HENKAN_SIZE);	// �ϊ�����������o�b�t�@�ɃR�s�[

		// �J�[�\���̂���o�C�g�ʒu���擾���Ă���
		g_Fep.FocusTop = sceAtokGetFocusClauseTop(&g_Atok);
		if(g_Fep.FocusTop < 0)
		{
			g_Fep.FocusTop = 0;
		}

		// �ϊ����̕�����𕪐߂��Ƃɕ������Ă���
		// �܂��ϊ���������̕��ߐ��𓾂�
		g_Fep.nClusters = sceAtokGetConvertingClauseCount(&g_Atok);
		for(i=0; i<g_Fep.nClusters; i++)
		{
			sceAtokGetConvertingClause(&g_Atok, g_Fep.aszClusters[i], HENKAN_SIZE, i);
		}
		// �t�H�[�J�X�̂��镶�ߔԍ����擾���Ă���
		g_Fep.FocusCls = sceAtokGetFocusClauseIndex( &g_Atok );

		// ���݂̓��̓X�e�[�g���擾���܂�
		state = sceAtokGetInputState(&g_Atok);
		if(state==SCE_ATOK_ISTATE_CANDIDATES ||
		   (g_Fep.FocusCls != focus && state!=SCE_ATOK_ISTATE_MOVECLAUSEGAP))
		{
			// ���݂̌��ԍ��𓾂�
			g_Fep.nCurrentCand = sceAtokGetCurrentCandidateIndex(&g_Atok);

			if(g_Fep.state != SCE_ATOK_ISTATE_CANDIDATES ||
			   (g_Fep.FocusCls != focus && state!=SCE_ATOK_ISTATE_MOVECLAUSEGAP))
			{
				// ����A���I�����[�h�ɂȂ����Ƃ��A��⃊�X�g�𓾂�
				g_Fep.nCandidates = sceAtokGetCandidateList(&g_Atok, g_Fep.CandBuf, CANDLIST_SIZE);
				g_Fep.nCandOffset  = (g_Fep.nCurrentCand / 10) * 10;
			}

			// ���߂̐擪�A�Ō�ɃW�����v���Č��ꗗ���o����
			if(g_Fep.stateCandLeftEnd == EX_ATOK_EDIT_CLSEND_CAND || g_Fep.stateCandLeftEnd == EX_ATOK_EDIT_CLSTOP_CAND)
			{
				// ����A���I�����[�h�ɂȂ����Ƃ��A��⃊�X�g�𓾂�
				g_Fep.nCandidates = sceAtokGetCandidateList(&g_Atok, g_Fep.CandBuf, CANDLIST_SIZE);
				g_Fep.nCandOffset  = (g_Fep.nCurrentCand / 10) * 10;
				g_Fep.stateCandLeftEnd = EX_ATOK_EDIT_NULL;
			}
		}
		else if(state == SCE_ATOK_ISTATE_BEFOREINPUT)
		{
			g_Fep.nCurrentCand = -1;
			focus = -1;
			// FIXME : g_Fep.FocusCls�����������ׂ�
		}
		else
		{
			// ���I�����[�h�ł͂Ȃ��ꍇ
			g_Fep.nCurrentCand = -1;
		}

		g_Fep.state = state;
		focus = g_Fep.FocusCls;

		SignalSema(g_AtokRsrcSema);				// ATOK�G���W���̎g�p�����������
	}

	return 1;
}
#endif



//============================================================================
//	getChar
//
//	FEP���C���̂��o��FIFO�L���[����ϊ��ς݃L�����N�^�����擾
//
//	COOKED_CHAR *pCc : ��������擾����ׂ̃o�b�t�@
//	int fPeek : 0:�L���[������o��/1:�L���[������o�����ɁA�擪�̃f�[�^�𓐂݌���
//
//	�߂�l : int : 0:����/1:�G���[(�L���[����ꂻ��)
static int getChar(COOKED_CHAR *pCc, int fPeek)
{
	// FIFO���Ɋm�蕶���񂪂������ꍇ�AFIFO����������(WAIT�Ȃ�)
	return (FifoPollData(&g_Fep.qFepOuput, pCc, sizeof(COOKED_CHAR), fPeek));
}


//============================================================================
//	inputBackspace
//
//	[BACKSPACE]�L�[���o�b�t�@�ɔ��f
//
//	int pos : ���݂̃L�����b�g�i�J�[�\���j�̈ʒu
//
//	�߂�l : int : �L�����b�g�ʒu
static int inputBackspace(int pos)
{
	int oldPos, i;
	if(pos <= 0)
	{
		#if 1//SKB_EX Sound
		Fep_se_play(SKBSE_INHIBITINPUT);
		#endif
		return pos;
	}

	oldPos = pos;
	pos--;
	#if 1//SKB_EX Sound
	Fep_se_play(SKBSE_BACKSPACE);
	#endif
	// �L�����b�g����̕�����O�ɋl�߂�
	i=0;
	while(g_pFepImport->fep.puiUCS4[oldPos+i] != 0x00000000)
	{
		g_pFepImport->fep.puiUCS4[pos+i] = g_pFepImport->fep.puiUCS4[oldPos+i];
		i++;
	}
	g_pFepImport->fep.puiUCS4[pos+i] = 0x0;

	g_Fep.nTextLen = pos + i;
	return pos;
}


//============================================================================
//	inputDelete
//
//	[DELETE]�L�[���o�b�t�@�ɔ��f
//
//	int pos : ���݂̃L�����b�g�i�J�[�\���j�̈ʒu
static void inputDelete(int pos)
{
	int i, j;
	i = g_Fep.nTextLen - pos;
	if(i > 0)
	{
		j = 1;
		memcpy(&g_pFepImport->fep.puiUCS4[pos],    &g_pFepImport->fep.puiUCS4[pos + j],    (size_t)((i - j)*sizeof(u_int)));
		g_Fep.nTextLen -= j;
		#if 1//SKB_EX Sound
		Fep_se_play(SKBSE_BACKSPACE);
		#endif
	}
	#if 1//SKB_EX Sound
	else
	{
		Fep_se_play(SKBSE_INHIBITINPUT);
	}
	#endif
}


//============================================================================
//	inputAllClear
//
//	���͂���Ă���f�[�^�����ׂăN���A
//
//	int *pos : ���݂̃L�����b�g�i�J�[�\���j�̈ʒu�̃|�C���^
static void inputAllClear(int *pos)
{
	*pos = 0;
	g_Fep.nCaretPos = 0;
	g_Fep.nTextLen = 0;									// �����񒷂�������
	memset(g_pFepImport->fep.puiUCS4, 0, (g_pFepImport->fep.uiMaxTextLen*sizeof(u_int)));	// ������o�b�t�@���N���A
}


#ifndef ATOK_NO_LINK
//============================================================================
//	FepSetConfig
//
//	ATOK�̓���ݒ�
static int FepSetConfig()
{
	sceAtokConfig cfg;

	// atok.ini�ɋL�^����Ă��Ȃ��ݒ���A�A�v���P�[�V�������]�ޒl�ŏ��������܂�
	{
		cfg.KanjiInput   = 1;		// �������̓��[�h
									// 1= ���[�}������
									// 2= ���ȓ��͊���
									// 3= ���p�X���[

		cfg.KanDspType   = 1;		// ���͕�����
									// 1= �Ђ炪��
									// 2= �S�p�J�^�J�i
									// 3= �S�p���ϊ�
									// 4= ���p�J�^�J�i
									// 5= ���p���ϊ�

		cfg.FixMode      = 0;		// �Œ���̓��[�h
									// 0= OFF
									// 1= �Œ�Ђ炪��
									// 2= �Œ�S�p�J�^�J�i
									// 3= �Œ�S�p���ϊ�
									// 4= �Œ蔼�p�J�^�J�i
									// 5= �Œ蔼�p���ϊ�

		cfg.HZCnvMode    = 0x0D;	// ���p�S�p�ϊ�
									// bit 3: ��ϊ��P��ւ̓K�p
									// bit 2: ���[�U�[�o�^�P��ւ̓K�p
									// bit 1: ���͒�������ւ̓K�p
									// bit 0: ���p�S�p�ϊ�

		// ������ނ��Ƃɔ��p,�S�p�ւ̕ϊ��ۂ𐧌䂵�܂�
		// �Ⴆ��bit0��0�ɂ��Ă������ƂŊm�蕶������ɔ��p�J�^�J�i�����邱�Ƃ�h���܂�
		// �������Ȃ���A�m��O�̕ϊ���������̒��ɔ��p�J�^�J�i������邱�Ƃ͂��肦�܂�
		cfg.HZCnvDetail  = 0x77;	// ���p�S�p�ϊ��ڍ�
									// bit 7: ���0
									// bit 6: �������S�p
									// bit 5: �p�����S�p
									// bit 4: �J�^�J�i���S�p
									// bit 3: ���0
									// bit 2: ���������p
									// bit 1: �p�������p
									// bit 0: �J�^�J�i�����p


		// ���p�ϊ�����(�S�p���̋֎~)�L��������
		cfg.HankakuCnvStr = "";			// �S�p�����֎~���镶���Ȃ�

		// �S�p�ϊ�����(���p���̋֎~)�L��������
		cfg.ZenkakuCnvStr = "������";	// 0xA0�`0xA5�͈̔͂ɂ���L���ɂ��āA���p�����֎~
											// 0xA6�`0xA7�͈̔�(���p�J�i)�ɂ��Ă�HZCnvType��bit 0�Ő���ł��܂�

		cfg.FuriganaMode = 0;		// �ӂ肪�ȓ��̓��[�h
									// 0= �I��
									// 1= �J�n

		cfg.MaxInputChars = 30;		// ���͕����� default 30
									// 0-30  ���̒l��30�𒴂����͈͊O�̎w��ł́Adefault 30�œ��삵�܂��B

		cfg.PutCursorMode = 0;		// ���m�蕶���񖖔��ɃJ�[�\��������ǉ�����/���Ȃ�
									// 0= ���Ȃ�
									// 1= ����

		// NOTICE: ��ǂ݉�͂�ON�ɂ���ƁA���͒��Ɏ����A�N�Z�X���������Ă��܂��̂ŁA
		// ���̊ԃ��[�}���ϊ����~�܂��Ă��܂��B
		// ��ǂ݉�͂�OFF�ɂ���΁A�������͒��̉�ʃt�B�[�h�o�b�N�܂ł̃��O���Z���Ȃ���ʂ�����܂����A
		// �ϊ��L�[�������܂Ŏ����A�N�Z�X�������Ȃ��Ȃ����߁A�g�[�^���̕ϊ����Ԃ͒����Ȃ��Ă��܂��܂�
		cfg.PreAnalyze    = 0;		// ���͎��̎�����ǂ݂���/���Ȃ�
									// 0= ���Ȃ�
									// 1= ����
	}


	// �ȉ��̐ݒ�́A_sysconf�p�[�e�B�V������/atok/atok.ini�ɐݒ肪�L�^����Ă���̂ŁA
	// sceAtokLoadConfig()�֐��ɂ���ēK���Ȓl���ݒ肳��Ă��܂��B
	// �A�v���P�[�V�����ŃI�[�o�[���C�h���邱�Ƃ��\�ł��B
	{
		cfg.ConvMode     = 0;		// �ϊ����[�h
									// 0= �A���ߕϊ�
									// 1= �P���ߕϊ�
									// 2= �����ϊ�

		cfg.LearningMode = 1;		// �w�K���[�h
									// 0= �w�K���Ȃ�
									// 1= �w�K����

		cfg.KutouCharSet = 1;		// ��Ǔ_�g�ݍ��킹�p�^�[��
									// 0= ",.[]/�"	(�J���}/ �s���I�h/ ������/   �X���b�V��/ ������)
									// 1= "������"	(�Ǔ_  / ��_    / ��������/ ����      / ������)

		cfg.OkuriMode    = 0;		// ���肪�ȃ��[�h
									// 0= �{��
									// 1= �Ȃ�
									// 2= ����

		cfg.AutoRegKind  = 0x0F;	// �����o�^���
									// bit 3: ���o�^��
									// bit 2: ��ϊ�
									// bit 1: ������
									// bit 0: ���ߋ�؂�

		cfg.LearningKind = 0x0F;	// �w�K���
									// bit 3: ���o�^��
									// bit 2: ��ϊ���
									// bit 1: ������w�K��
									// bit 0: ��؂蒼���w�K��

		// NOTICE: ���[�}���������菈���Ƃ́A���[�}���Ƃ��ĔF�߂��Ȃ��V�[�P���X�����ꂽ�Ƃ�
		// �Ăу��[�}���ϊ������悤�Ƃ��鏈���ł���B���̏�����OFF�ɂ���ƁA�ȍ~�̓A���t�@�x�b�g�̂܂܂ƂȂ�
		// �Ⴆ��[H][D][D]�Ɠ��͂����Ƃ��A�������聨�u�g���c�v, �����Ȃ����uHDD�v�ƂȂ�
		cfg.RomanRecover = 1;		// ���[�}���������肷��/���Ȃ�
									// 0= ���Ȃ�
									// 1= ����

		cfg.AutoRecMode  = 0;		// ���͎x������/���Ȃ�
									// 0= ���Ȃ�
									// 1= ����

		cfg.AutoRecRoman = 0;		// ���͎x��(���[�}�����͎�)�̏ڍאݒ�
									// bit 2: [N]�̉ߕs��
									// bit 1: �q���̒���
									// bit 0: �ꉹ�̉ߕs��

		cfg.AutoRecKana  = 0;		// ���͎x��(���ȓ��͎�)�̏ڍאݒ�
									// bit 2: �u��v���u���v
									// bit 1: �u�v���u���v(�u�v��X����)
									// bit 0: �u�K�v���u�J�v(�����_����_��)

		cfg.AutoRecRep   = 0;			// ���͎x��(�u����)�̏ڍאݒ�
									// bit 3: �u�B�v���u�D�v(��_���s���I�h��)
									// bit 2: �u�A�v���u�C�v(�Ǔ_���J���}��)
									// bit 1: �u�E�v���u�^�v(�������X���b�V����)
									// bit 0: �u�[�v���u�|�v(���������n�C�t����)

		cfg.AppendAtoHen = 0x13;	// ��ϊ����̌�⃊�X�g�ւ̒ǉ�������/���Ȃ�
									// bit 4: ���p���ϊ����
									// bit 3: �S�p���ϊ����
									// bit 2: ���p�J�^�J�i���
									// bit 1: �S�p�J�^�J�i���
									// bit 0: �Ђ炪�Ȍ��

		cfg.KutouCnvMode   = 0; 	// ��Ǔ_�ϊ�
									// 0= ���Ȃ�
									// 1= ����

		cfg.KutouCnvDetail = 0x03;// ��Ǔ_�ϊ��̏ڍאݒ�
									// bit 3: �I(���Q��)�ŕϊ�
									// bit 2: �H(�^�╄)�ŕϊ�
									// bit 1: �A(�Ǔ_)�ŕϊ�
									// bit 0: �B(��_)�ŕϊ�

		cfg.RuledZenCnv    = 1; 	// �K���w��S�p�ϊ�
									// bit 0: [~]��[�`]�ɕϊ�
	}


	// ATOK�G���W���̃R���t�B�M�����[�V������ݒ�
	cfg.Size = sizeof(sceAtokConfig);
	sceAtokSetConfig(&g_Atok, &cfg, SCE_ATOK_CONFIG_KANJIINPUT      |
									SCE_ATOK_CONFIG_KANDSPTYPE      |
									SCE_ATOK_CONFIG_FIXMODE         |
									SCE_ATOK_CONFIG_HZCNVMODE       |
									SCE_ATOK_CONFIG_HZCNVDETAIL     |
									SCE_ATOK_CONFIG_HANKAKUCNVSTR   |
									SCE_ATOK_CONFIG_ZENKAKUCNVSTR   |
									SCE_ATOK_CONFIG_FURIGANAMODE    |
									SCE_ATOK_CONFIG_MAXINPUTCHARS   |
									SCE_ATOK_CONFIG_PUTCURSORMODE   |
									SCE_ATOK_CONFIG_PREANALYZE      |
									SCE_ATOK_CONFIG_CONVMODE        |
									SCE_ATOK_CONFIG_LEARNINGMODE    |
									SCE_ATOK_CONFIG_KUTOUCHARSET    |
									SCE_ATOK_CONFIG_OKURIMODE       |
									SCE_ATOK_CONFIG_AUTOREGKIND     |
									SCE_ATOK_CONFIG_LEARNINGKIND    |
									SCE_ATOK_CONFIG_ROMANRECOVER    |
									SCE_ATOK_CONFIG_AUTORECMODE     |
									SCE_ATOK_CONFIG_AUTORECROMAN    |
									SCE_ATOK_CONFIG_AUTORECKANA     |
									SCE_ATOK_CONFIG_AUTORECREP      |
									SCE_ATOK_CONFIG_APPENDATOHEN    |
									SCE_ATOK_CONFIG_KUTOUCNVMODE    |
									SCE_ATOK_CONFIG_KUTOUCNVDETAIL  |
									SCE_ATOK_CONFIG_RULEDZENCNV);

	// �V�X�e���ݒ��ǂ�ŏ��������Ă��������B
	g_Fep.Keybind       = g_pFepImport->usSystemIni[ENV_ATOK_BIND]; // �L�[�o�C���h�̐ݒ�
																	// 0 : IME��
																	// 1 : ATOK��

    // �V�X�e���ݒ���g��
	g_Fep.InitialInput  = g_pFepImport->usSystemIni[ENV_ATOK_MODE]; // �������̓��[�h
																	// 0 : ���[�}������
																	// 1 : ���ȓ���

	g_Fep.SpaceInput    = 2;// �X�y�[�X���̓��[�h
							// 0 : ��Δ��p
							// 1 : �X�y�[�X�֎~
							// 2 : ���p�X�y�[�X/�S�p�X�y�[�X
	g_Fep.TenkeyInput   = 2;// �e���L�[���̓��[�h
							// 0 : �����͂��e���L�[����̏ꍇ�A���ڔ��p����
							// 1 : �����͂��e���L�[�ł`�s�n�j�̓��̓��[�h�����p�̏ꍇ�A���ڔ��p����
							// 2 : ��ɂ`�s�n�j�֓���

	g_Fep.Font1         = 0;//
	g_Fep.Font2         = 0;//
	return(0);
}
#endif


#if 1//SKB_EX Sound 
//============================================================================
//  Fep_se_play												ref:  libfep.h
//
//  ���ʉ��̃R�[���o�b�N�֐����Ăяo��
//
//  �߂�l    : �Ȃ�
void Fep_se_play(int sound_No)
{
	if( g_pFepImport->se )
	{
		g_pFepImport->se(sound_No);
	}
}

//============================================================================
//  Fep_atok_sound											ref:  libfep.h
//
//  �`�s�n�j�R�}���h�ʂɌ��ʉ��̃R�[���o�b�N�֐����Ăяo���B
//
//  �߂�l    : �Ȃ�
void Fep_atok_sound(int f)
{
	#ifndef ATOK_NO_LINK
	int sound;

	switch(f)
	{
	case SCE_ATOK_EDIT_BACKSPACE: // ������1�����폜(BACKSPACE)
	                              sound=SKBSE_BACKSPACE;   break;
	case SCE_ATOK_EDIT_DELETE:    // �J�[�\���ʒu��1�����폜(DELETE)
	                              sound=SKBSE_BACKSPACE;   break;
	case SCE_ATOK_EDIT_CURRIGHT:  // �J�[�\���ړ�(�E)
	                              sound=SKBSE_CARETMOVE;   break;
	case SCE_ATOK_EDIT_CURLEFT:	// �J�[�\���ړ�(��)
	                              sound=SKBSE_CARETMOVE;   break;
	case SCE_ATOK_EDIT_CUREND:	// �J�[�\���ړ�(����)�i���L�[�������j
	                              sound=SKBSE_CARETMOVE;   break;
	case SCE_ATOK_EDIT_CURTOP:	// �J�[�\���ړ�(����)�i���L�[�������j
	                              sound=SKBSE_CARETMOVE;   break;
	case SCE_ATOK_EDIT_1ONKAKUTEI: // 1���m��
	case SCE_ATOK_EDIT_POSTCONVHIRA:// ��ϊ�:�Ђ炪��
	case SCE_ATOK_EDIT_POSTCONVKANA:// ��ϊ�:�J�^�J�i
	case SCE_ATOK_EDIT_POSTCONVHALF:// ��ϊ�:���p
	case SCE_ATOK_EDIT_POSTCONVRAW: // ��ϊ�:���ϊ�
	case SCE_ATOK_EDIT_DICCONV: 	 // �����w��ϊ�
	case SCE_ATOK_EDIT_CONVFORWARD:// �ϊ��A����⏇���擾�i���{�^���������܂��́��L�[�������j
	                              sound=SKBSE_STARTTRANS;  break;
	case SCE_ATOK_EDIT_CONVBACKWARD:// �ϊ��A����⏇���擾�i���L�[�������j
	                              sound=SKBSE_STARTTRANS;  break;
	case SCE_ATOK_EDIT_KAKUTEIPART:// �����m��i���{�^���������j
	case SCE_ATOK_EDIT_KAKUTEIREP: // �m�胊�s�[�g
	case SCE_ATOK_EDIT_KAKUTEIUNDO:// �m��A���h�D
	case SCE_ATOK_EDIT_KAKUTEIHEAD:// ���������m��
	case SCE_ATOK_EDIT_KAKUTEITAIL:// ���������m��
	                              sound=SKBSE_CANDDETECT;  break;
	case SCE_ATOK_EDIT_KAKUTEIALL:// �S���m��iSTART�{�^���������j
	                              sound=SKBSE_DETECTEXIT;  break;
	case SCE_ATOK_EDIT_DELETEALL: // �S�����폜�i�~�������j
	case SCE_ATOK_EDIT_CONVCANCEL:// �ϊ��L�����Z��(���ڕ��߈ȍ~)�i�~�������j
	case SCE_ATOK_EDIT_CANCELALL:// �S�ϊ�������
	                              sound=SKBSE_CANCEL;      break;
	case SCE_ATOK_EDIT_FOCUSRIGHT:// ���ڕ��ߋ�؂��1�����̂΂��iR1�{���{�^���������j
	                              sound=SKBSE_STRETCHCLS;  break;
	case SCE_ATOK_EDIT_FOCUSLEFT: // ���ڕ��ߋ�؂��1�����k�߂�iR1�{���{�^���������j
	                              sound=SKBSE_STRETCHCLS;  break;
	case SCE_ATOK_EDIT_MOVFOCUSCLAUS:// ���ڕ��߈ړ�
	                              sound=SKBSE_MOVECLS;     break;
	default:                      sound=-999;              break;
	}
	if(sound!=-999)  Fep_se_play(sound);
	#endif
}
#endif

#if 1//SKB_EX Insert 
//============================================================================
//  Fep_SetInsertMode											ref:  libfep.h
//
//  �����̑}��/�㏑���[�h��ݒ肷��
//
//  int Mode  : �ݒ胂�[�h(ref:  libfep.h)
//      FEP_EDIT_ADD				// �����ǉ�
//      FEP_EDIT_INSERT				// �����}��
//              
//  �߂�l    : �Ȃ�
void  Fep_SetInsertMode(int Mode)
{
	if(Mode == FEP_EDIT_ADD)			// �㏑Mode 
	{
		g_Fep.iInsertMode = 0;
	}else	// ����ȊO�͂����đS���}��Mode�ɂ��遦Default �}��Mode
	{
		g_Fep.iInsertMode = 1;
	}
}
//============================================================================
//  Fep_GetInsertMode											ref:  libfep.h
//
//  �����̑}��/�㏑���[�h���l������
//
//              
//  �߂�l    : �ݒ胂�[�h(ref:  libfep.h)
//      FEP_EDIT_ADD				// �����ǉ�
//      FEP_EDIT_INSERT				// �����}��
int  Fep_GetInsertMode(void)
{
	return(g_Fep.iInsertMode ? FEP_EDIT_INSERT : FEP_EDIT_ADD);	
}
#endif

#if 1//SKB_EX PowerOff 
//============================================================================
//  Fep_GetAtokSemaID											ref:  libfep.h
//
//  Atok �Z�}�t�H�h�c�擾
//
//  �߂�l : int : �Z�}�t�H�h�c
int Fep_GetAtokSemaID()
{
	return g_AtokRsrcSema;
}
#endif
