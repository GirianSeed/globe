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

#ifndef __LIBSOFTKB_H__
#define __LIBSOFTKB_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#include <shareddef.h>
#include "sharedskb.h"

// ���̓��[�h
#define SKBMODE_ZENKAKU			(0x80)
#define SKBMODE_HIRAGANA		(SKBMODE_ZENKAKU | 0x00)
#define SKBMODE_HAN_KATAKANA	(0x01)
#define SKBMODE_ZEN_KATAKANA	(SKBMODE_ZENKAKU | 0x01)
#define SKBMODE_HAN_ALPHABET	(0x02)
#define SKBMODE_ZEN_ALPHABET	(SKBMODE_ZENKAKU | 0x02)

#define KEYTOP_NUM_MAX		(10)
#define SKBMODE_NOHIT		(KEYTOP_NUM_MAX + 5)

// �t�F�[�Y
enum
{
	SKBPHASE_OPENING,	// �I�[�v����
	SKBPHASE_OPENED,	// �I�[�v�����I�����
	SKBPHASE_CLOSING,	// �N���[�Y��
	SKBPHASE_CLOSED,	// �N���[�Y���I�����

	SKBPHASE_MAX,
};

// �A���t�@�l�z��̃C���f�b�N�X
enum
{
	SKBALPHA_BG,
	SKBALPHA_BASE,
	SKBALPHA_CHAR,
	SKBALPHA_CURSOR,

	SKBALPHA_MAX
};

// �L�[�g�b�v
enum
{
	// ���{��
	KEYTOP_HIRAGANA = 0,	// 50���z��Ђ炪��
	KEYTOP_KATAKANA,		// 50���z��J�^�J�i
	KEYTOP_ABC,				// ABC�z��
	KEYTOP_SIGN,			// �L��(����{�^������)
	KEYTOP_NUMJP,			// ����(���{�ŁF�S�p���p�{�^������)
	KEYTOP_PC_JAPAN,		// ���{
	// �C�O��
	KEYTOP_PC_US,			// �A�����J
	KEYTOP_PC_FRANCE,		// �t�����X
	KEYTOP_PC_SPAIN,		// �X�y�C��
	KEYTOP_PC_GERMAN,		// �h�C�c
	KEYTOP_PC_ITALIA,		// �C�^���A
	KEYTOP_PC_DUTCH,		// �I�����_
	KEYTOP_PC_PORTUGRESS,	// �|���g�K��
	KEYTOP_PC_UK,			// �C�M���X
	KEYTOP_PC_CANADAF,		// �J�i�f�B�A���t�����`
	KEYTOP_NUMF,			// ����(�C�O�ŁF�S�p���p�{�^������)
	KEYTOP_SIGNF,			// �L��(�ʉ�etc)
	KEYTOP_IDIOMCHAR,		// �����L���t������

	KEYTOP_MAX
};

// ���[�h�o�[
enum
{
	// ���{��
	MODEBAR_CHAR_HIRAGANA_J = 0,	// �Ђ炪��
	MODEBAR_CHAR_KATAKANA_J,		// �J�^�J�i
	MODEBAR_CHAR_ALPHABET_J,		// �A���t�@�x�b�g
	MODEBAR_CHAR_SIGN_J,			// �L��
	// �C�O��
	MODEBAR_CHAR_ALPHABET_F,		// �A���t�@�x�b�g
	MODEBAR_CHAR_UMLAUT_F,			// �����L�����A���t�@�x�b�g
	MODEBAR_CHAR_SIGN_F,			// �L��
	// ����
	MODEBAR_CHAR_NUM,				// ����

	MODEBAR_CHAR_MAX
};

// ���͋֎~���� �\����
struct inhibitChar_s
{
	u_int	*pCtrlKey;		// ���͋֎~�R���g���[���L�[�ւ̃|�C���^
	u_int	uiCKeyNum;		// ���͋֎~�R���g���[���L�[�̐�
	u_int	(*pChar)[10];	// ���͋֎~������ւ̃|�C���^(UCS4)
	u_int	uiCharNum;		// ���͋֎~������̐�
};



struct skey_s
{
	// ���̃L�[�{�[�h���g���֐�
	exportSkb_t		export;

	int				created;		// 0:�܂��쐬����Ă��Ȃ�/1:�쐬�ς�
	u_int			uiKeyTopNo;		// �L�[�g�b�v�ԍ�
	u_int			uiCursorPos;	// ���݂̃J�[�\���ʒu
	u_int			uiPush;			// ���{�^����������Ă��邩�t���O

	// ���
	u_char			ucInputMode;	// ���̓��[�h
									// 0: �Ђ炪��
									// 1: �J�^�J�i
									// 2: �A���t�@�x�b�g

	u_char			ucModebarType;	// ���[�h�o�[�^�C�v
									// 0: �Ђ炪��
									// 1: �J�^�J�i
									// 2: �A���t�@�x�b�g
									// 3: �L��
									// 4: �A���t�@�x�b�g
									// 5: �����L�����A���t�@�x�b�g
									// 6: �L��
									// 7: ����

	u_char			ucShift;		// �V�t�g���
									// bit 1: SHIFT    ON
									// bit 2: CAPSLOCK ON
									// bit 4: ZENKAKU  ON/ALTGR ON
									// bit 8: �J�[�\����SHIFT�������ꂽ�� ON

	inhibitChar_t	*pInhibitChar;	// �֎~�����ւ̃|�C���^

	// �`��p
	char			*pKeyBuff;		// �L�[���\�[�X�o�b�t�@
	u_int			uiSpNum;		// �X�v���C�g��
	u_int			uiUvNum;		// UV��
	u_int			uiChipNum;		// �w�i�p�`�b�v��
	u_int			uiBtnNum;		// �L�[��
	sprite_t		*pSps;			// �X�v���C�g���
	uv_t			*pUvs;			// UV���
	paneChip_t		*pChips;		// �w�i�p�`�b�v
	button_t		*pBtns;			// �L�[���
	skbInfo_t		*pSkbInfo;		// �\�t�g�E�F�A�L�[�{�[�h���

	u_char			ucAlpha[SKBALPHA_MAX];		// �A���t�@�l
												// 0:BG
												// 1:�L�[�{�^��
												// 2:����
												// 3:�J�[�\��

	int		uiKbdPosX, uiKbdPosY;	// �L�[�{�[�h�̈ʒu

	sprite_t		spCursor[5];	// �J�[�\���̃X�v���C�g
	int				iCursorPos;		// �J�[�\���̃t���t��

	int				iPhase;			// �t�F�[�Y

	skblist_t		List;			// ���X�g
};


int  SoftKB_Init(gImport_t *pImport);
void SoftKB_Destroy(void);

void SoftKB_Run(sceVif1Packet *pk, skey_t *pKey);
int  SoftKB_CheckShown(void);
void SoftKB_UseMouse(u_char flag);
int  SoftKB_Create(skey_t *pKey, iPoint_t *skbPoint, u_int keyTopNo, inhibitChar_t *inhibitChar, void *pTmp);
void SoftKB_Delete(skey_t *pKey);
void SoftKB_Open(skey_t *pKey);
void SoftKB_Close(skey_t *pKey);

void SoftKB_SetPadData(skey_t *pKey, u_long paddata);
void SoftKB_SetMouseData(skey_t *pKey, u_long128 *data);

void SoftKB_Move(skey_t *pKey, int x, int y);
void SoftKB_ChangeAlpha(skey_t *pKey, int index, u_char alpha);
u_char SoftKB_GetInputMode(skey_t *pKey);
void SoftKB_GetSize(skey_t *pKey, int *w, int *h);
void SoftKB_MoveModebar(int x, int y);
void SoftKB_SetModebarType(const u_int *pTypes, int maxNum);
void SoftKB_ClearModebarType(void);
void SoftKB_DrawFocus(sceVif1Packet *pk, sprite_t *sp, u_int curPos, u_int col);
u_int SoftKB_SearchFocus(skey_t *pKeyNow, skey_t *pKeyNext);
void SoftKB_SetStartCurPos(skey_t *pKey, u_int keyTopType);


const iPoint_t* SoftKB_GetMousePoint(void);
//const u_int SoftKB_GetMouseBtn(void);     // ������
//const u_int SoftKB_GetMouseWheel(void);   // ������

#if 1//SKB_EX Sound
void SoftKB_se_play(int sound);
#endif

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// !__LIBSOFTKB_H__
