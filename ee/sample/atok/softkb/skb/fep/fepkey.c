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

// �n�[�h�E�F�A�L�[�{�[�hFEP���C��
// ����fepkey.c�̊֐��́AATOK���[�J�X���b�h���Ŏ��s����܂�
// �L�[�R�[�h��ATOK�R�}���h�ւ̕ϊ�����v�ȖړI�ł��B
#include <sys/types.h>
#include <eekernel.h>
#include <libusbkb.h>
#include <stdio.h>
#include <libccc.h>

#include "libfep.h"
#include "fepkey.inc"

#ifndef ATOK_NO_LINK
#include "vje.inc"								// VJE ���L�[�o�C���h��`
#include "atok.inc"								// ATOK���L�[�o�C���h��`
#include "ime.inc"								// IME ���L�[�o�C���h��`
#endif

#define SUPPORT_OVERSEAUSBKB	(0)
#define SUPPORT_OVERSEALIBCCC	(0)


extern gImport_t		*g_pFepImport;
extern fep_t			g_Fep;

#if 1//SKB_EX Sound
extern void Fep_atok_sound(int f);
#endif

static void ascii2UCS4(u_int *ucs4, u_short keycode);
#if SUPPORT_OVERSEAUSBKB
static int thinkAltGr(int keybdType, u_short usCharCode, u_char mkey, u_short rawcode);
#endif

static void FepTreatKeyDirect(u_char led, u_char mkey, u_short rawcode);
#ifndef ATOK_NO_LINK
static int  FepTreatKeyAtok(sceAtokContext *pAtok, u_char led, u_char mkey, u_short rawcode);
static const FEP_KEYBIND *FepSearchBind(u_char led, u_char mkey, u_short rawcode);

// �L�[�o�C���h��`
static const struct
{
	const FEP_KEYBIND *pBind;
	int nKeyBinds;
} g_BindTbl[] =
{
	{ImeKeyTbl,		(sizeof(ImeKeyTbl) / sizeof(FEP_KEYBIND))},		// CHECK :
	{AtokKeyTbl,	(sizeof(AtokKeyTbl)/ sizeof(FEP_KEYBIND))},
};
#endif

static int idiomFlg = 0;		// �����L����FEP���[�`���ɓn�����t���O


//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//	FepTreatKey                                                ref: libfep.c
//
//	�e�L�[�R�[�h�ɂ���������
//
//	sceAtokContext *pAtok : ATOK�ւ̃R���e�L�X�g�|�C���^
//	u_char led  : LED�̏��
//	u_char mkey : ���f�B�t�@�C�A�L�[�̏��
//	u_short rawcode : USB�L�[�{�[�h������͂��ꂽRAW�R�[�h
void FepTreatKey(sceAtokContext *pAtok, u_char led, u_char mkey, u_short rawcode)
{
	#if 1//SKB_EX F12
	if(rawcode == USBKEYC_F12)
	{
		g_pFepImport->fep.changeMode(-1); // Select or FunctionKey�ɂ��AtokMode�̐؂芷��
		return;
	}
	#endif

	if(pAtok == NULL)
	{
		// ATOK �T�|�[�g���ĂȂ����̏���
		FepTreatKeyDirect(led, mkey, rawcode);
		return;
	}

	#ifndef ATOK_NO_LINK
	// �L�[�R�[�h����������
	if(sceAtokGetKanjiState(pAtok))
	{
		int i;
		// ATOK��ON�̂Ƃ�

		// FEP�\���̓��ɕۑ����ꂽ ���ȃ��[�h�t���O���g��
		if(g_Fep.led & USBKB_LED_KANA)
		{
			led |= USBKB_LED_KANA;
		}
		else
		{
			led &= ~USBKB_LED_KANA;
		}

		// ATOK�G���W���ɃR�}���h�𓊂���ꍇ
		i = FepTreatKeyAtok(pAtok, led, mkey, rawcode);
		if(i)
		{
			// �f�b�h�L�����N�^�Ƃ��Ĉ����ꍇ
			// �o��FIFO��USB�L�[�R�[�h�����̂܂܊i�[����
			COOKED_CHAR c;
			c.Char    = 0xFFFFFFFF;
			c.Flag    = mkey;
			c.RawCode = 0x8000 | rawcode;
			FifoSendData(&g_Fep.qFepOuput, &c, sizeof(COOKED_CHAR));
		}
	}
	else
	{
		// ATOK��OFF�̂Ƃ�
		// ATOK�G���W�����������̓��[�h�ɂ��邽�߂̃L�[�R�[�h���`�F�b�N����
		if((g_pFepImport->usSystemIni[ENV_KEYBD_TYPE] == USBKB_ARRANGEMENT_106 && rawcode==USBKEYC_106_KANJI )  // 106�̑S����101��`~ ��
			|| (g_pFepImport->usSystemIni[ENV_KEYBD_TYPE] == USBKB_ARRANGEMENT_101 && rawcode==USBKEYC_106_KANJI ))  // ����rawcode���Ԃ��Ă���݂���
		{
			// [SHIFT],[CTRL],[Windows]�L�[��������Ă��炸�A[ALT]+[���p/�S�p]�����͂��ꂽ�Ƃ�
			sceAtokKanjiOn(pAtok);                              // ATOK���������̓��[�h��
			sceAtokFlushConverted(pAtok);                       // �m�蕶����o�b�t�@�̃t���b�V�����s��

			g_pFepImport->fep.atokON();
			#if 0//SKB_EX Default   �f�t�H���g���A�Ђ炪�ȂƂ͌���Ȃ��̂ŁA�폜�BatokON���ŁA�Z�b�g����B
			Fep_Cmd(pAtok, EX_ATOK_FIXOFF);
			g_pFepImport->fep.changeMode(EX_ATOK_DSPHIRAGANA);  // �ŏ��o�[�Ɠ���������ׂɃR�[���o�b�N���Ă�
			Fep_SetInputMode(FEPON_HIRA);                       // �����I�ɂЂ炪�ȃ��[�h
			#endif
		}
		else
		{
			// ATOK�G���W���ɃR�}���h�𓊂����A���ړ��͂����ꍇ
			FepTreatKeyDirect(led, mkey, rawcode);
		}
	}
	#endif
	return;
}


//============================================================================
//	FepKeyInitIdiomFlag
//
//	�����L����FEP���[�`���ɓn�����t���O
//	���̊֐��́A�C�O�ŃL�[�{�[�h���g�p����ɂ�������
//	�L�[�{�[�h�^�C�v��؂�ւ��镔���ŕK�v�ƂȂ�܂��B
void FepKeyInitIdiomFlag(void)
{
	idiomFlg = 0;
}


//============================================================================
// STATIC FUNCTIONS
//============================================================================
//============================================================================
//	ascii2UCS4
//
//	�A�X�L�[�R�[�h����UCS2(UCS4)�ɕϊ�
//
//	u_int *ucs4 : �ϊ���̕����R�[�h�i�[�p�o�b�t�@
//	u_short keycode : �ϊ��O�̕����R�[�h
static void ascii2UCS4(u_int *ucs4, u_short keycode)
{
	sceCccUCS4 chr=0;
#if SUPPORT_OVERSEALIBCCC
	if(g_pFepImport->iRegion == REGION_J)
#endif
	{
		// ���{����
		sceCccJISCS  jis;
		sceCccSJISTF aSjis[2];
		sceCccSJISTF const* pSjis = aSjis;
		aSjis[0] = keycode;
		aSjis[1] = 0;
		jis = sceCccDecodeSJIS(&pSjis);
		*ucs4 = sceCccJIStoUCS(jis, chr);
	}
#if SUPPORT_OVERSEALIBCCC
	else
	{
		// �C�O��
		*ucs4 = sceCccLatin1toUCS((u_char)keycode, chr);
	}
#endif
}


#ifndef ATOK_NO_LINK
//============================================================================
//	FepSearchBind
//
//	�L�[�o�C���h���̌����T�u���[�`��
//
//	u_char led  : LED�̏��
//	u_char mkey : ���f�B�t�@�C�A�L�[�̏��
//	u_short rawcode : USB�L�[�{�[�h������͂��ꂽRAW�R�[�h
//
//	�߂�l : const FEP_KEYBIND * : �L�[�o�C���h�e�[�u���ւ̃|�C���^
static const FEP_KEYBIND *FepSearchBind(u_char led, u_char mkey, u_short rawcode)
{
	const FEP_KEYBIND *pBindList;
	int nKeyBinds;
	u_short usCharCode;
	u_int c1, c2, c3;
	int i;

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

	// LED���,���f�B�t�@�C�A�L�[��Ԃ�OFF�ɂ��ăL�[�R�[�h������
	switch(g_pFepImport->usSystemIni[ENV_KEYBD_TYPE])
	{
	case USBKB_ARRANGEMENT_101:
		usCharCode = sceUsbKbCnvRawCode(USBKB_ARRANGEMENT_101, mkey, (led & ~USBKB_LED_KANA), rawcode);
		break;
	case USBKB_ARRANGEMENT_106:
	default:
		usCharCode = sceUsbKbCnvRawCode(USBKB_ARRANGEMENT_106, mkey, (led & ~USBKB_LED_KANA), rawcode);
		break;
	}

	c1 = usCharCode;
	if(mkey & (USBKB_MKEY_L_SHIFT | USBKB_MKEY_R_SHIFT)) {
		c1 |= SHIFT;
	}
	if(mkey & (USBKB_MKEY_L_CTRL  | USBKB_MKEY_R_CTRL)) {
		c1 |= CTRL;
	}
	if(mkey & (USBKB_MKEY_L_ALT   | USBKB_MKEY_R_ALT)) {
		c1 |= ALT;
	}
	// [a]�`[z]��[A]�`[Z]�ɕϊ�����
	if(('a'<=(c1 & 0x0000BFFF) && (c1 & 0x0000BFFF)<='z')) {
		c2 = c1 & 0xFFFFFFDFUL;
	} else {
		c2 = c1;
	}
	for(i=0; i<nKeyBinds; i++) {
		c3 = pBindList[i].code;
		if((c3 & USBKB_KEYPAD) && (c3==c2)) {
			// �e���L�[���ł̂ݗL���ȃR�[�h�����������ꍇ
			return(&pBindList[i]);
		} else if(c3==(c2 & ~USBKB_KEYPAD)) {
			// �Y������L�[�R�[�h�����������ꍇ(�t���L�[,�e���L�[��킸)
			return(&pBindList[i]);
		}
	}
	return(NULL);
}
#endif


//============================================================================
//	FepTreatKeyDirect
//
//	ATOK��OFF�̏��(���ړ��͂��ꂽ��)�̏���
//
//	u_char led  : LED�̏��
//	u_char mkey : ���f�B�t�@�C�A�L�[�̏��
//	u_short rawcode : USB�L�[�{�[�h������͂��ꂽRAW�R�[�h
static void FepTreatKeyDirect(u_char led, u_char mkey, u_short rawcode)
{
	COOKED_CHAR c;
	u_short usCharCode = 0;
	u_char  ucMask = 0;
	u_int keybdType = USBKB_ARRANGEMENT_101;

	// �ʏ�̃L�[�R�[�h�ϊ�
	if(g_pFepImport->iRegion == REGION_J)
	{
		// ���{
		switch(g_pFepImport->usSystemIni[ENV_KEYBD_TYPE])
		{
		case USBKB_ARRANGEMENT_101:
			usCharCode = sceUsbKbCnvRawCode(USBKB_ARRANGEMENT_101, mkey, led, rawcode);
			break;
		case USBKB_ARRANGEMENT_106:
		default:
			if(led & USBKB_LED_KANA)
			{
				usCharCode = sceUsbKbCnvRawCode(USBKB_ARRANGEMENT_106_KANA, mkey, led, rawcode);
			}
			else
			{
				usCharCode = sceUsbKbCnvRawCode(USBKB_ARRANGEMENT_106, mkey, led, rawcode);
			}
			break;
		}
	}
	else
	{
		u_int type = g_pFepImport->usSystemIni[ENV_KEYBD_TYPE];
		if(type == USBKB_ARRANGEMENT_101)
		{
			// US
			keybdType = USBKB_ARRANGEMENT_101;
		}
#if SUPPORT_OVERSEAUSBKB
		else
		{
			// �C�O
			if(USBKB_ARRANGEMENT_FRENCH <= type)
			{
				keybdType = type+1;		// USB�L�[�{�[�h���C�u�����ƃL�[�{�[�h�ݒ��
										// USBKB_ARRANGEMENT_106_KANA������̂�+1�������
				if(keybdType > USBKB_ARRANGEMENT_FRENCH_CA)
				{
					keybdType = USBKB_ARRANGEMENT_101;
				}
			}
		}
#endif
		usCharCode = sceUsbKbCnvRawCode(keybdType, mkey, led, rawcode);
		#if 0 //SKB_EX Cut
		g_pFepImport->dprintf("RAWCODE --- %d ---, %x, %x, %x\n", __LINE__, usCharCode, rawcode, mkey);
		#endif
	}

	if((led & USBKB_LED_CAPS_LOCK) && (mkey & (USBKB_MKEY_L_SHIFT | USBKB_MKEY_R_SHIFT))
	   && !(mkey & (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL | USBKB_MKEY_L_ALT | USBKB_MKEY_R_ALT | USBKB_MKEY_L_WIN | USBKB_MKEY_R_WIN)))
	{
		// CAPSLOCK+SHIFT -> ������
		if(usCharCode>=0x41 && usCharCode<=0x5A)
		{
			usCharCode += 0x0020;
		}
	}

	if(((usCharCode & (USBKB_RAWDAT|USBKB_KEYPAD))==USBKB_KEYPAD)
	   && !(mkey & (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL | USBKB_MKEY_L_ALT | USBKB_MKEY_R_ALT | USBKB_MKEY_L_WIN | USBKB_MKEY_R_WIN)))
	{
		// �e���L�[������̓���
		c.Char    = usCharCode & ~(USBKB_RAWDAT|USBKB_KEYPAD);
		c.Flag    = mkey;
		c.RawCode = rawcode;
		// �L�[���͂�FEP�o��FIFO�ɒu��(WAIT����)
		FifoSendData(&g_Fep.qFepOuput, &c, sizeof(COOKED_CHAR));
		return;
	}

#if 0
	// ALT�{�A���t�@�x�b�g�ŃA���t�@�x�b�g�����͂ł��Ă��܂��̂��C�������o�[�W����
	if(keybdType == USBKB_ARRANGEMENT_101 || keybdType == USBKB_ARRANGEMENT_106)
	{
		// �A�����J�Ȃ̂ŉEALT�͗L��
		ucMask = (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL | USBKB_MKEY_L_ALT | USBKB_MKEY_R_ALT | USBKB_MKEY_L_WIN | USBKB_MKEY_R_WIN);
	}
#if SUPPORT_OVERSEAUSBKB
	else
	{
		// ���[���b�p�Ȃ̂ŉEALT��ALTGr�ɂȂ�
		ucMask = (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL | USBKB_MKEY_L_ALT | USBKB_MKEY_L_WIN | USBKB_MKEY_R_WIN);
	}
#endif
#else
	// ������Alt�L�[�����Ȃ���ł��A���t�@�x�b�g���o��
	ucMask = USBKB_MKEY_L_WIN | USBKB_MKEY_R_WIN;
#endif

	if(!(usCharCode & (USBKB_RAWDAT|USBKB_KEYPAD)) && !(mkey & ucMask))
	{
		u_int ucs4;
		ascii2UCS4(&ucs4, usCharCode);

		c.Char    = ucs4;		// usCharCode : ����́A��̔���ɕK�v�Ȃ̂ő��삵�Ȃ�
		c.Flag    = mkey;
		c.RawCode = rawcode;
	}
	else
	{
		usCharCode = 0;
		c.Char    = 0xFFFFFFFF;
		c.Flag    = mkey;
		c.RawCode = rawcode;
	}


	if(usCharCode==0) {
		// �f�b�h�L�����N�^
		c.Char = 0xFFFFFFFF;
		c.RawCode |= 0x8000;
	} else if(!(usCharCode & USBKB_RAWDAT)) {
		// ASCII�R�[�h�ɕϊ��ł���������������
		if(mkey & (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL)) {
			// [CTRL]�L�[��������Ă����Ƃ�
			if(usCharCode>=0x40 && usCharCode<=0x7F) {
				// [CTRL]+[@]/[`]��0x00�ɕϊ�
				// [CTRL]+[A]�`[Z]/[a]�`[z]��0x01�`0x1A�ɕϊ�
				// [CTRL]+[[]/[{]��0x1B�ɕϊ�
				// [CTRL]+[\]/[|]��0x1C�ɕϊ�
				// [CTRL]+[]]/[}]��0x1D�ɕϊ�
				// [CTRL]+[^]/[~]��0x1E�ɕϊ�
				// [CTRL]+[_]/[]��0x1F�ɕϊ�
				c.Char = usCharCode & 0x0000001F;
			} else {
				// �f�b�h�L�����N�^
				c.Char = 0xFFFFFFFF;
				c.RawCode |= 0x8000;
			}
		}
	} else {
		// ASCII�R�[�h�ɕϊ��ł��Ȃ���������
		#ifndef ATOK_NO_LINK
		if(g_Fep.ucUseAtok)
		{
			const FEP_KEYBIND *pBind = FepSearchBind(led, mkey, rawcode);
			int f;
			if(pBind==NULL) {
				// �L�[�o�C���h��������Ȃ������ꍇ
				// �f�b�h�L�����N�^
				c.Char = 0xFFFFFFFF;
				c.RawCode |= 0x8000;
			}
			else
			{
				// ���ړ��͎��̃t�@���N�V�������擾����
				f = pBind->func[0];
	
				if(f & RAW_CODE) {
					c.Char = f;
				} else if(f==0 || f>=0x100) {
					// �L�[�o�C���h����`�������ꍇ�A�f�b�h�L�����N�^
					c.Char = 0xFFFFFFFF;
					c.RawCode |= 0x8000;
				} else {
					// �ʏ�̃L�[�R�[�h���������Ƃ�
					c.Char = f;
				}
			}
		}
		else
		#endif
		{
			// �f�b�h�L�����N�^
			c.Char = 0xFFFFFFFF;
			c.RawCode |= 0x8000;
		}
	}


	if(keybdType == USBKB_ARRANGEMENT_101 || keybdType == USBKB_ARRANGEMENT_106)
	{
		// �A�����J�Ȃ̂ŉEALT�͗L��
		ucMask = (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL | USBKB_MKEY_L_ALT | USBKB_MKEY_R_ALT | USBKB_MKEY_L_WIN | USBKB_MKEY_R_WIN);
	}
#if SUPPORT_OVERSEAUSBKB
	else
	{
		// ���[���b�p�Ȃ̂ŉEALT��ALTGr�ɂȂ�
		ucMask = (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL | USBKB_MKEY_L_ALT | USBKB_MKEY_L_WIN | USBKB_MKEY_R_WIN);
	}

	// �C�O�ł̂݃`�F�b�N
	// �ȉ��̏�����AltGr�L�[�݂̂ɑΉ����܂��B
	if(keybdType >= USBKB_ARRANGEMENT_FRENCH  && usCharCode != 0 && !(mkey & ucMask))
	{
		thinkAltGr(keybdType, usCharCode, mkey, rawcode);
		return;
	}
#endif

	// �L�[���͂�FEP�o��FIFO�ɒu��(WAIT����)
	FifoSendData(&g_Fep.qFepOuput, &c, sizeof(COOKED_CHAR));
}


#if SUPPORT_OVERSEAUSBKB
//============================================================================
//	thinkAltGr
//
//	AltGr�L�[���������̏���
//
//	int keybdType : USB�L�[�{�[�h�̃^�C�v
//	u_short usCharCode : �����R�[�h
//	u_char mkey : ���f�B�t�@�C�A�L�[�̏��
//	u_short rawcode : USB�L�[�{�[�hRAW�R�[�h
static void thinkAltGr(int keybdType, u_short usCharCode, u_char mkey, u_short rawcode)
{
	COOKED_CHAR c;
	static int idiomIndex = 0;
	static 	COOKED_CHAR old_char = {0x0, 0x0, 0x0};
	u_int chLangTbl=keybdType-USBKB_ARRANGEMENT_FRENCH;
	int		i;

	if((keybdType == USBKB_ARRANGEMENT_FRENCH) && (rawcode ==0x0026))
	{
		// �t�����X��̎���[AltGr+9=^]��"^"�����̂܂܏o��
		// FIXME : ���̔����L��(ex : "~")�{[AltGr+9=^] = ~^ ���ƂȂ�
		FifoSendData(&g_Fep.qFepOuput, &c, sizeof(COOKED_CHAR));
		return;
	}
	if((keybdType == USBKB_ARRANGEMENT_ITALIAN) && (rawcode == 0x0026))
	{
		// �C�^���A��̎��ɂ�"^"�����̂܂܏o��
		FifoSendData(&g_Fep.qFepOuput, &c, sizeof(COOKED_CHAR));
		return;
	}

	// �����L����������FIFO�ɑ��炸���̓��͕��������Ĕ��f
	if(idiomFlg == 0)
	{
		for(i=0; i<6; i++)
		{
			if(usCharCode == g_idiomTbl[i].idiomCode && g_idiomTbl[i].keyNum[chLangTbl] != 0)
			{
				// �����L�����`�F�b�N
				// �����L�������͂���Ă������U�ޔ�
				old_char.Char    = usCharCode;
				old_char.Flag    = mkey;
				old_char.RawCode = rawcode;
				idiomFlg = 1;
				idiomIndex = i;
				return;
			}
		}
	}
	else
	{
		idiom_t *pTbl = &g_idiomTbl[idiomIndex];
		exchange_table_t *pExTbl = pTbl->key[chLangTbl];
		int chenged = 0;

		if(rawcode == 0x2cU)
		{
			// �X�y�[�X�������甭���L����FIFO�ɑ��M
			c = old_char;
			idiomFlg = 0;
		}
		else
		{
			// �ϊ��e�[�u���ɂĔ����L���{�A���t�@�x�b�g�ɕϊ�
			for(i=0; i<pTbl->keyNum[chLangTbl]; i++)
			{
				if(usCharCode == pExTbl[i].nowCode)
				{
					c.Char = pExTbl[i].outCode;
					c.RawCode = rawcode;
					c.Flag    = mkey;
					chenged = 1;				// �ϊ��ł���
				}
			}
			idiomFlg = 0;

			if(!chenged)
			{
				// �ϊ��ł��Ȃ������ꍇ�́A�����L���{�A���t�@�x�b�g��FIFO�ɑ��M
				FifoSendData(&g_Fep.qFepOuput, &old_char, sizeof(COOKED_CHAR));
			}
		}
	}
}
#endif


#ifndef ATOK_NO_LINK
//============================================================================
//	FepTreatKeyAtok
//
//	�e�L�[�R�[�h�ɂ���������
//	ATOK��ON�̂Ƃ�(�R�}���h�ɕϊ�)
//	�߂�l��1�Ƃ��邱�ƂŁA�f�b�h�L�����N�^�ł��邱�Ƃ�ʒm����
//
//	sceAtokContext *pAtok : ATOK�ւ̃R���e�L�X�g�|�C���^
//	u_char led  : LED�̏��
//	u_char mkey : ���f�B�t�@�C�A�L�[�̏��
//	u_short rawcode : USB�L�[�{�[�hRAW�R�[�h
//
//	�߂�l : int : 0:�R���g���[���R�[�h�Ƃ��Ĉ���/1:�f�b�h�L�����N�^�Ƃ��Ĉ���
static int FepTreatKeyAtok(sceAtokContext *pAtok, u_char led, u_char mkey, u_short rawcode)
{
	u_short usCharCode;
	int s, f = 0;
	// �ʏ�̃L�[�R�[�h�ϊ�
	switch(g_pFepImport->usSystemIni[ENV_KEYBD_TYPE])
	{
	case USBKB_ARRANGEMENT_101:
		usCharCode = sceUsbKbCnvRawCode(USBKB_ARRANGEMENT_101, mkey, led, rawcode);
		break;
	case USBKB_ARRANGEMENT_106:
	default:
		if(led & USBKB_LED_KANA)
		{
			usCharCode = sceUsbKbCnvRawCode(USBKB_ARRANGEMENT_106_KANA, mkey, led, rawcode);
		}
		else
		{
			usCharCode = sceUsbKbCnvRawCode(USBKB_ARRANGEMENT_106, mkey, led, rawcode);
		}
		break;
	}

	// ATOK�̓��̓X�e�[�g�𓾂�
	s = sceAtokGetInputState(pAtok);

	if(usCharCode==0)
	{
		if(s==SCE_ATOK_ISTATE_MOVECLAUSEGAP &&
		   !(mkey & (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL)) &&
		   0x21<=rawcode && rawcode>=0x7e)
		{
			// ���ߋ�؂�ύX���Ɏ��̃L�[�������ꍇ
			sceAtokEditConv(pAtok, SCE_ATOK_EDIT_KAKUTEIALL, 0);
			Fep_SetData2Fifo();
		}

		// �f�b�h�L�����N�^
		return(1);
	}

	// 101�L�[�̂Ƃ��� ATOK OFF ���A��O�I�ɏ���
	if((g_pFepImport->usSystemIni[ENV_KEYBD_TYPE] == USBKB_ARRANGEMENT_101) &&
	   (mkey & (USBKB_MKEY_L_ALT | USBKB_MKEY_R_ALT)) &&
	   (rawcode == 0x35))
	{
		Fep_Cmd( pAtok , EX_ATOK_EDIT_KANJIOFF);
		return 0;
	}
	else
	{
		if(s==SCE_ATOK_ISTATE_BEFOREINPUT && usCharCode==' ')
		{
			// �����͏�ԂŃX�y�[�X�������Ƃ�
			f = ' ';
			if(g_Fep.SpaceInput==0 || (g_Fep.SpaceInput==2 && g_Fep.KanDspType>=4))
			{
				// ��ɔ��p�A�܂��͔��p�J�^�J�i,���p�p��
				f |= RAW_CODE;
			}
		}
		else if(s==SCE_ATOK_ISTATE_BEFOREINPUT && (usCharCode & (USBKB_RAWDAT|USBKB_KEYPAD))==USBKB_KEYPAD)
		{
			// �����͏�ԂŃe���L�[�R���̃L�����N�^�������Ƃ�
			if(g_Fep.TenkeyInput==0 || (g_Fep.TenkeyInput==2 && g_Fep.KanDspType>=4))
			{
				// ��ɔ��p�A�܂��͔��p�J�^�J�i,���p�p��
				f = (usCharCode & 0xFF);
			}
		}
		else
		{
			if(!(mkey & (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL)) &&
				(((0x21<=(usCharCode & ~USBKB_KEYPAD) && (usCharCode & ~USBKB_KEYPAD)<=0x7E)) || ((0xA0<=usCharCode) && (usCharCode<=0xDF))) &&
				!('0'<=(usCharCode & ~USBKB_KEYPAD) && (usCharCode & ~USBKB_KEYPAD)<='9') &&
				!((0x402C<=usCharCode) && (usCharCode<=0x402F)))
			{
	
				// [CTRL]�L�[�͉�����Ă��Ȃ��A
				// ���ʕ���(0x21�`0x7E)�܂��̓e���L�[���̕���(USBKB_KEYPAD��OR����Ă���)
				// �������A[0]�`[9]�ƃe���L�[����[,][-][.][/]�͏��O���Ă���
				if(s==SCE_ATOK_ISTATE_MOVECLAUSEGAP) {
					// ���ߋ�؂�ύX���Ɏ��̃L�[�������ꍇ�A�m�肳����
					sceAtokEditConv(pAtok, SCE_ATOK_EDIT_KAKUTEIALL, 0);
					Fep_SetData2Fifo();
				}
				sceAtokEditConv(pAtok, (int)usCharCode, 0);
				return(0);
			}
			// �L�[�o�C���h���X�g����T�[�`
			f = 0;
		}
	}

	if(f==0)
	{
		// �܂��A�L�[�e�[�u������Y������L�[�R�[�h���Ȃ����e�[�u�����猟��
		const FEP_KEYBIND *pBind = FepSearchBind(led, mkey, rawcode);
		if(pBind==NULL)
		{
			// �Y������L�[�o�C���h��������Ȃ������ꍇ
			// �f�b�h�L�����N�^�Ƃ��Ĉ���
			g_pFepImport->dprintf("/Unknown code %d: %02X:%02X:%04X\n", __LINE__, led, mkey, usCharCode);
			return(1);
		}

		// ATOK�̓��̓X�e�[�g�ɉ�����ATOK�̃R�}���h�𓾂�
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
		if(f==0)
		{
			// �f�b�h�L�����N�^�Ƃ��Ĉ���
			return(1);
		}
	}

	if(s > SCE_ATOK_ISTATE_BEFOREINPUT &&
		(f >= SCE_ATOK_EDIT_CONVFORWARD && f <= SCE_ATOK_EDIT_POSTCONVRAW))
	{
		g_Fep.nIsConv = CONV_ON;			// �ϊ����t���O�ݒ�
	}

	if(f & RAW_CODE)
	{
		// �R���g���[���R�[�h�Ƃ��Ĉ���
		COOKED_CHAR c;
		c.Char    = f & ~RAW_CODE;
		c.Flag    = mkey;
		c.RawCode = rawcode;
		FifoSendData(&g_Fep.qFepOuput, &c, sizeof(COOKED_CHAR));
		return(0);
	}
	else
	{
		int res;
		#if 1//SKB_EX Sound
		Fep_atok_sound(f);
		#endif
		res = Fep_Cmd(pAtok ,f);
		if(res)
		{
			// �R�}���h�̒�`�G���[
			// CAUTION: �f�b�h�L�����N�^�Ƃ��Ă͈���Ȃ��B
			g_pFepImport->dprintf("Unknown function: %02X:%02X:%04X %d\n", led, mkey, usCharCode, f);
		}
	}
	return(0);
}
#endif
