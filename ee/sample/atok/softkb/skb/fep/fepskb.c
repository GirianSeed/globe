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

// �\�t�g�E�F�A�L�[�{�[�hFEP���C��
// ����fepskb.c�̊֐��́AATOK���[�J�X���b�h���Ŏ��s����܂�
// �L�[�R�[�h��ATOK�R�}���h�ւ̕ϊ�����v�ȖړI�ł��B
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

// �S�p�𔼊p�ɂ���(ATOK�G���W���ɓ���O��UCS4����S-JIS�ɕϊ������ׂ��̂܂܃R�[�h������)
specialSign_t g_SpecialSign [] =
{
	{"�u", "�"},
	{"�v", "�"},
	{"�i", "("},
	{"�j", ")"},
	{"�J", "�"},
	{"�K", "�"},
	{"�E", "�"},
	{"�F", ":"},
	{"�`", "~"},
	{"�[", "�"},
	
	{"�H", "?"},
	{"�I", "!"},
	{"�A", "�"},
	{"�B", "�"},

	{"�@", " "},
};


//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//	FepTreatSkb                                                ref: libfep.c
//
//	�\�t�g�E�F�A�L�[�{�[�h�R�A����
//
//	sceAtokContext *pAtok : ATOK�ւ̃R���e�L�X�g�|�C���^
//	u_short ctrlCode : ����L�[(USB�L�[�R�[�h������)
//	u_char flag : SHIFT��������Ă��邩�H
//	const u_int *pCharCode : ������ւ̃|�C���^
void FepTreatSkb(sceAtokContext *pAtok, u_short ctrlCode, u_char flag, const u_int *pCharCode)
{
	// ATOK �T�|�[�g���ĂȂ����̏���
	if(pAtok == NULL)
	{
		FepTreatKeyDirectSkb(ctrlCode, flag, pCharCode);
		return;
	}

	#ifndef ATOK_NO_LINK
	if(sceAtokGetKanjiState(pAtok))
	{
		// ATOK��ON�̂Ƃ� || ATOK �T�|�[�g���Ă��鎞�̏���
		// ATOK�̓��̓X�e�[�g�𓾂�
		int i=0;
		if((Fep_AtokGetInputState() == SCE_ATOK_ISTATE_BEFOREINPUT) && pCharCode[0] == 0x00000020)
		{
			// �X�y�[�X�͓���
			#if 1//SKB_EX Sound
			Fep_se_play(SKBSE_STARTTRANS);  
			#endif
			FepTreatKeyDirectSkb(ctrlCode, flag, pCharCode);
		}
		#if 1//SKB_EX Sound
		if((Fep_AtokGetInputState() == 0) && pCharCode[0] == 0x00003000)
		{
			// �X�y�[�X����
			Fep_se_play(SKBSE_ONSPACE);  
		}
		#endif

		i = FepTreatSkbAtok(pAtok, ctrlCode, flag, pCharCode);
		if(i)
		{
			// �f�b�h�L�����N�^�Ƃ��Ĉ����ꍇ
			// �o��FIFO��USB�L�[�R�[�h�����̂܂܊i�[����
			COOKED_CHAR c;
			c.Char    = 0xFFFFFFFF;
			c.Flag    = flag;
			c.RawCode = 0x8000 | ctrlCode;
			FifoSendData(&g_Fep.qFepOuput, &c, sizeof(COOKED_CHAR));
		}
	}
	else
	{
		// ATOK��OFF�̂Ƃ�
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
//	u_short ctrlCode : ����L�[(USB�L�[�R�[�h������)
//	u_char flag : SHIFT��������Ă��邩�H
//	const u_int *pCharCode : ������ւ̃|�C���^
//
//	�߂�l : int : ���0
static int FepTreatKeyDirectSkb(u_short ctrlCode, u_char flag, const u_int *pCharCode)
{
	COOKED_CHAR c;

	// ���̂܂܃L�[�R�[�h���|�X�g
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
		// ����A�C�O�ł̔����L���t���������͍l������Ă��Ȃ�
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
//	u_char  mkey : ���f�B�t�@�C�A�L�[
//	u_short ctrlCode : ����L�[(USB�L�[�R�[�h������)
//
//	�߂�l : const FEP_KEYBIND * : �L�[�o�C���h�ւ̃|�C���^
static const FEP_KEYBIND *FepSearchBindSkb(u_char mkey, u_short ctrlCode)
{
	const FEP_KEYBIND *pBindList;
	int nKeyBinds;
	u_int c1, c2;
	int i;

	// HDD : __SYSCONF/conf/system.ini �̐ݒ��ǂ�
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
//	�u���v+�u�J�v�� �u���v�ɕϊ����鏈���B
//	�\�t�g�E�F�A�L�[�{�[�h�ŗL�̏����B
//	u_int *D : ���͕���
//
//	�߂�l : int : 0:�G���[(������������Ȃ�/�Y�����Ȃ�)/1:�ϊ��ł����B
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

	// �Ō�̕����̃C���f�b�N�X�擾
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
//	ATOK QUEUE�ɕ������M
//
//	sceAtokContext *pAtok : ATOK�ւ̃R���e�L�X�g�|�C���^
//	const u_int *p : �����R�[�h(UCS4)
static void sendKeyAtok(sceAtokContext *pAtok, const u_int *p)
{
	int i=0;
	sceCccJISCS chr=0;
	sceCccJISCS jis=0;
	sceCccSJISTF sjisTmp[32];
	sceCccSJISTF *pSjisTmp   = sjisTmp;
	sceCccSJISTF** ppSjisTmp = &pSjisTmp;

	// S-JIS�ɕ����R�[�h�ύX
	while(*(p+i))
	{
		jis = sceCccUCStoJIS(*(p+i), chr);		// UCS4 -> JIS
		sceCccEncodeSJIS(ppSjisTmp, jis);		// JIS  -> S-JIS
		i++;
	}
	*(*ppSjisTmp) = '\0';


	{
		// �S�p�����������甼�p�ɕϊ�
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
			// 1�o�C�g�����̏ꍇ
			switch(sjisTmp[i])
			{
			case ',':
			case '.':
			case '/':
			case '-':
				// ��Ǔ_�L�����N�^�Z�b�g�ݒ�̉e�����󂯂�L�����N�^�ɂ��ẮA
				// bit 9��1�ɂ��ăR�}���h�𑗂邱�Ƃɂ��ݒ�𖳌��ɂ���
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
			// 2�o�C�g�����̏ꍇ
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
//	�e�L�[�R�[�h�ɉ����������B
//	�߂�l��1�Ƃ��邱�ƂŁA�f�b�h�L�����N�^�ł��邱�Ƃ�ʒm����
//
//	sceAtokContext *pAtok : ATOK�ւ̃R���e�L�X�g�|�C���^
//	u_short ctrlCode : ����L�[(USB�L�[�R�[�h������)
//	u_char flag : SHIFT��������Ă��邩�H
//	const u_int *pCharCode : ������ւ̃|�C���^
static int FepTreatSkbAtok(sceAtokContext *pAtok, u_short ctrlCode, u_char flag, const u_int *pCharCode)
{
	COOKED_CHAR c;
	int f = 0;
	int res;

	if(ctrlCode == 0)
	{
		// �����R�[�h��POST
		#if 1//SKB_EX Sound
		Fep_se_play(SKBSE_ONKEY);  
		#endif
		sendKeyAtok(pAtok, pCharCode);
	}
	else
	{
		// �R���g���[���R�[�h������
		const FEP_KEYBIND *pBind = FepSearchBindSkb(flag, ctrlCode);
		int s = sceAtokGetInputState(pAtok);

		if(pBind == NULL)
		{
			// �Y������L�[�o�C���h��������Ȃ������ꍇ
			// �f�b�h�L�����N�^�Ƃ��Ĉ���
			g_pFepImport->dprintf("Unknown code[%d]: %02X:%04X\n", __LINE__, flag, ctrlCode);
			return 1;
		}

		switch(s)
		{
		case SCE_ATOK_ISTATE_BEFOREINPUT:		// ������
			f = pBind->func[0];
			break;

		case SCE_ATOK_ISTATE_BEFORECONVERT:		// �ϊ��O
			f = pBind->func[1];
			break;

		case SCE_ATOK_ISTATE_CONVERTING:		// �ϊ���
		case SCE_ATOK_ISTATE_CANDEMPTY:			// ���؂�(���Ȃ�)
			f = pBind->func[2];
			break;

		case SCE_ATOK_ISTATE_CANDIDATES:		// ���ꗗ
			f = pBind->func[3];
			break;

		case SCE_ATOK_ISTATE_MOVECLAUSEGAP:		// ���ߋ�؂蒼��
			f = pBind->func[4];
			break;

		case SCE_ATOK_ISTATE_POSTCONVKANA:		// ��ϊ�(�J�^�J�i)
		case SCE_ATOK_ISTATE_POSTCONVHALF:		// ��ϊ�(���p)
		case SCE_ATOK_ISTATE_POSTCONVRAW:		// ��ϊ�(���ϊ�)
			f = pBind->func[5];
			break;

		default:
			// �s���ȃX�e�[�g
			f = 0;
			break;
		}

		if(s > SCE_ATOK_ISTATE_BEFOREINPUT &&
			(f >= SCE_ATOK_EDIT_CONVFORWARD && f <= SCE_ATOK_EDIT_POSTCONVRAW))
		{
			g_Fep.nIsConv = CONV_ON;			// �ϊ����t���O�ݒ�
		}

		if(f == 0)
		{
			// �f�b�h�L�����N�^�Ƃ��Ĉ���
			return 1;
		}

		if(f & RAW_CODE)
		{
			// �f�b�h�L�����N�^
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
				// �R�}���h�̒�`�G���[?
				// CAUTION: �f�b�h�L�����N�^�Ƃ��Ă͈���Ȃ��B
				g_pFepImport->dprintf("Unknown code[%d]: %02X:%04X\n", __LINE__, flag, ctrlCode);
			}
		}
	}

	return 0;
}
#endif

