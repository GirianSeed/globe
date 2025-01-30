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

#ifndef __RESDEF_H__
#define __RESDEF_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

// Makefile define
//   SKBRES_LINK
//   REGION_J_ONLY
//   REGION_J_OTHER_ONLY
//   CDBOOT

typedef struct
{
	#ifdef SKBRES_LINK  
	char*  caKeyTopFilename;		// �t�@�C����
	#else
	char   caKeyTopFilename[LOCALPATH_MAX];		// �t�@�C����
	#endif
	u_char fepType;								// FEP�^�C�v
												// 		�Ђ炪�ȁA�J�^�J�i�A���ϊ�
	u_char modeType;							// ���[�h�^�C�v:
												// 		�Ђ炪�ȁA�J�^�J�i�A�A���t�@�x�b�g�A�����A�L��
} rsc_t;

#ifdef SKBRES_LINK
//--------------------------------------------------------
#include <skbres.h>
//---------------------------------------
// skbres.a�p
//---------------------------------------
static rsc_t g_RSC[] =
{
	// ���{��
	{	// 50���z��Ђ炪��
		0,
		0,
		MODEBAR_CHAR_HIRAGANA_J,
	},
	{	// 50���z��J�^�J�i
		0,
		1,
		MODEBAR_CHAR_KATAKANA_J,
	},
	{	// ABC�z��
		0,
		2,
		MODEBAR_CHAR_ALPHABET_J,
	},
	{	// �L��(����{�^������)
		0,
		2,
		MODEBAR_CHAR_SIGN_J,
	},
	{	// ����(���{�ŁF�S�p���p�{�^���t��)
		0,
		2,
		MODEBAR_CHAR_NUM,
	},
	{	// ���{106�L�[�{�[�h
		0,
		2,
		MODEBAR_CHAR_ALPHABET_J,
	},
	// �C�O��
	// QWERTY & AZERTY
	{	// �A�����J
		0,
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{	// �t�����X
		0,
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{	// �X�y�C��
		0,
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{	// �h�C�c
		0,
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{	// �C�^���A
		0,
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{	// �I�����_
		0,
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{	// �|���g�K��
		0,
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{	// �C�M���X
		0,
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{	// �J�i�f�B�A���t�����`
		0,
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{	// ����
		0,
		2,
		MODEBAR_CHAR_NUM,
	},
	{	// �L��(�ʉ�etc)
		0,
		2,
		MODEBAR_CHAR_SIGN_F,
	},
	{	// �����L���t������
		0,
		2,
		MODEBAR_CHAR_UMLAUT_F,
	},
};

// �e�N�X�`����`
static const char *caTexFilename[] =
{
	// �\�t�g�E�F�A�L�[�{�[�h
	skbBG_tm2,		// �w�i
	skbKey_tm2,		// �L�[�g�b�v����
	skbBtn_tm2,		// �L�[(�{�^��)
	skbMdbar_tm2,		// ���̓��[�h
	// �w���v
	skbHItem_tm2,	// �R���g���[��,�{�^���̊G
};

static const char *caTexLangFilename[] =
{
	// �w���v����
	#ifdef REGION_J_OTHER_ONLY
	0,
	#else
	skbHJp_tm2,	// ���{
	#endif
	skbHEng_tm2,	// �p��(���{�����p��o�[�W�������܂�)
	#ifdef REGION_J_ONLY
	0,
	0,
	0,
	0,
	0,
	0,
	#else
	skbHFch_tm2,	// �t�����X��
	skbHSp_tm2,	// �X�y�C����
	skbHGmn_tm2,	// �h�C�c��
	skbHIta_tm2,	// �C�^���A��
	skbHDch_tm2,	// �I�����_��
	skbHPtg_tm2,	// �|���g�K����
	#endif
};
//--------------------------------------------------------
#else

#if CDBOOT
//---------------------------------------
// CDBOOT�p
//---------------------------------------
static const rsc_t g_RSC[] =
{
	// ���{��
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"\\AIUHIRA.BIN;1",		// 50���z��Ђ炪��
		#endif
		0,
		MODEBAR_CHAR_HIRAGANA_J,
	},
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"\\AIUKANA.BIN;1",		// 50���z��J�^�J�i
		#endif
		1,
		MODEBAR_CHAR_KATAKANA_J,
	},
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"\\ABC.BIN;1",			// ABC�z��
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_J,
	},
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"\\KIGO.BIN;1",		// �L��(����{�^������)
		#endif
		2,
		MODEBAR_CHAR_SIGN_J,
	},
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"\\NUM.BIN;1",			// ����(���{�ŁF�S�p���p�{�^���t��)
		#endif
		2,
		MODEBAR_CHAR_NUM,
	},
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"\\QWERTY.BIN;1",		// ���{
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_J,
	},
	// �C�O��
	// QWERTY & AZERTY
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\USQ.BIN;1",			// �A�����J
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\FRENCHQ.BIN;1",		// �t�����X
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\SPANISHQ.BIN;1",	// �X�y�C��
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\GERMANQ.BIN;1",		// �h�C�c
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\ITALIANQ.BIN;1",	// �C�^���A
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\DUTCHQ.BIN;1",		// �I�����_
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\PORTUGQ.BIN;1",		// �|���g�K��
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\UKQ.BIN;1",			// �C�M���X
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\CFRENCHQ.BIN;1",	// �J�i�f�B�A���t�����`
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\NUMF.BIN;1",		// ����
		#endif
		2,
		MODEBAR_CHAR_NUM,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\SPSIGN.BIN;1",		// �L��(�ʉ�etc)
		#endif
		2,
		MODEBAR_CHAR_SIGN_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\IDIOMCHR.BIN;1",	// �����L���t������
		#endif
		2,
		MODEBAR_CHAR_UMLAUT_F,
	},
};

// �e�N�X�`����`
static const char *caTexFilename[] =
{
	// �\�t�g�E�F�A�L�[�{�[�h
	SKB_SKBTEX_PATH"\\SKBBG.TM2;1",		// �w�i
	SKB_SKBTEX_PATH"\\SKBKEY.TM2;1",	// �L�[�g�b�v����
	SKB_SKBTEX_PATH"\\SKBBTN.TM2;1",	// �L�[(�{�^��)
	SKB_SKBTEX_PATH"\\SKBMDBAR.TM2;1",	// ���̓��[�h
	// �w���v
	SKB_HELPTEX_PATH"\\SKBHITEM.TM2;1",	// �R���g���[��,�{�^���̊G
};

static const char *caTexLangFilename[] =
{
	// �w���v����
	#ifdef REGION_J_OTHER_ONLY
	0,
	#else
	SKB_HELPTEX_PATH"\\SKBHJP.TM2;1",	// ���{
	#endif

	SKB_HELPTEX_PATH"\\SKBHENG.TM2;1",	// �p��(���{�����p��o�[�W�������܂�)

	#ifdef REGION_J_ONLY
	0,
	0,
	0,
	0,
	0,
	0,
	#else
	SKB_HELPTEX_PATH"\\SKBHFCH.TM2;1",	// �t�����X��
	SKB_HELPTEX_PATH"\\SKBHSP.TM2;1",	// �X�y�C����
	SKB_HELPTEX_PATH"\\SKBHGMN.TM2;1",	// �h�C�c��
	SKB_HELPTEX_PATH"\\SKBHITA.TM2;1",	// �C�^���A��
	SKB_HELPTEX_PATH"\\SKBHDCH.TM2;1",	// �I�����_��
	SKB_HELPTEX_PATH"\\SKBHPTG.TM2;1",	// �|���g�K����
	#endif
};
//--------------------------------------------------------
#else //CDBOOT

//---------------------------------------
// HOST�p
//---------------------------------------
static const rsc_t g_RSC[] =
{
	// ���{��
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"/aiuHira.bin",		// 50���z��Ђ炪��
		#endif
		0,
		MODEBAR_CHAR_HIRAGANA_J,
	},
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"/aiuKana.bin",		// 50���z��J�^�J�i
		#endif
		1,
		MODEBAR_CHAR_KATAKANA_J,
	},
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"/abc.bin",			// ABC�z��
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_J,
	},
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"/kigo.bin",			// �L��(����{�^������)
		#endif
		2,
		MODEBAR_CHAR_SIGN_J,
	},
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"/num.bin",			// ����(���{�ŁF�S�p���p�{�^���t��)
		#endif
		2,
		MODEBAR_CHAR_NUM,
	},
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"/qwerty.bin",		// ���{
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_J,
	},
	// �C�O��
	// QWERTY & AZERTY
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/usQ.bin",			// �A�����J
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/frenchQ.bin",		// �t�����X
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/spanishQ.bin",		// �X�y�C��
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/germanQ.bin",		// �h�C�c
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/italianQ.bin",		// �C�^���A
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/dutchQ.bin",			// �I�����_
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/portugQ.bin",		// �|���g�K��
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/ukQ.bin",			// �C�M���X
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/cfrenchQ.bin",		// �J�i�f�B�A���t�����`
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/numF.bin",			// ����
		#endif
		2,
		MODEBAR_CHAR_NUM,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/spSign.bin",			// �L��(�ʉ�etc)
		#endif
		2,
		MODEBAR_CHAR_SIGN_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/idiomChr.bin",		// �����L���t������
		#endif
		2,
		MODEBAR_CHAR_UMLAUT_F,
	},
};

// �e�N�X�`����`
static const char *caTexFilename[] =
{
	// �\�t�g�E�F�A�L�[�{�[�h
	SKB_SKBTEX_PATH"/skbBG.tm2",		// �w�i
	SKB_SKBTEX_PATH"/skbKey.tm2",		// �L�[�g�b�v����
	SKB_SKBTEX_PATH"/skbBtn.tm2",		// �L�[(�{�^��)
	SKB_SKBTEX_PATH"/skbMdbar.tm2",		// ���̓��[�h
	// �w���v
	SKB_HELPTEX_PATH"/skbHItem.tm2",	// �R���g���[��,�{�^���̊G
};

static const char *caTexLangFilename[] =
{
	// �w���v����
	#ifdef REGION_J_OTHER_ONLY
	0,
	#else
	SKB_HELPTEX_PATH"/skbHJp.tm2",	// ���{
	#endif

	SKB_HELPTEX_PATH"/skbHEng.tm2",	// �p��(���{�����p��o�[�W�������܂�)

	#ifdef REGION_J_ONLY
	0,
	0,
	0,
	0,
	0,
	0,
	#else
	SKB_HELPTEX_PATH"/skbHFch.tm2",	// �t�����X��
	SKB_HELPTEX_PATH"/skbHSp.tm2",	// �X�y�C����
	SKB_HELPTEX_PATH"/skbHGmn.tm2",	// �h�C�c��
	SKB_HELPTEX_PATH"/skbHIta.tm2",	// �C�^���A��
	SKB_HELPTEX_PATH"/skbHDch.tm2",	// �I�����_��
	SKB_HELPTEX_PATH"/skbHPtg.tm2",	// �|���g�K����
	#endif
};
#endif //CDBOOT
#endif //SKBRES_LINK


#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// !__RESDEF_H__
