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

#ifndef __SHAREDSKB_H__
#define __SHAREDSKB_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#include "shareddef.h"

// �L�[�{�[�h�̏��
#define LAYOUT_SHIFT		(0x01)		// SHIFT     �L�[ON
#define LAYOUT_CAPS			(0x02)		// CAPSLOCK  �L�[ON
#define LAYOUT_ZENKAKU		(0x04)		// ���p/�S�p �L�[ON
#define LAYOUT_ALTGR		(0x04)		// AltGr     �L�[ON(���̃L�[������̂̓��[���b�p����)
#define LAYOUT_FOCUSSHIFT	(0x80)		// �J�[�\�������킹��SHIFT��ON�ɂȂ����Ƃ�
// �L�[�̏��
#define KEY_PUSH			(1<<1)		// ������       �L�[
#define KEY_DISABLE			(1<<2)		// �I���ł��Ȃ� �L�[
#define KEY_FOCUSON			(1<<3)		// �J�[�\���ʒu �L�[
#define KEY_INVISIBLE		(1<<4)		// �����Ȃ�     �L�[
#define KEY_DUMMY			(1<<5)		// �_�~�[       �L�[
// �}�X�N
#define SKBMASK_LAYOUT(_l_)		(_l_ & ~(1<<7))		// �J�[�\��SHIFT�̃}�X�N
// �}�E�X
#define SKBMOUSE_NULL		(-1)		// NULL �t���O
#define SKBMOUSE_LBTNDOWN	(1<<0)		// ���{�^������
#define SKBMOUSE_RBTNDOWN	(1<<1)		// �E�{�^������
#define SKBMOUSE_WBTNDOWN	(1<<2)		// �z�C�[������
#define SKBMOUSE_DOWN		(1)			// �{�^������
#define SKBMOUSE_UP			(0)			// �{�^������


// �F
#define SKBDEF_DISABLE_ALPHA	(0x40)

#define SKBDEF_ATTENTION_ALPHA(_a_)	((((u_int)(0xff*_a_))>>7) << (6*4))
#define SKBDEF_ATTENTION_COL	(0x00ff7070)

enum
{
	SKBCTRL_NORMAL = 0,	// �m�[�}���L�[
	SKBCTRL_CAPS,		// CAPSLOCK
	SKBCTRL_SHIFT_L,	// SHIFT��
	SKBCTRL_SHIFT_R,	// SHIFT�E
	SKBCTRL_BACKSPACE,	// �o�b�N�X�y�[�X
	SKBCTRL_ENTER,		// ���s
	SKBCTRL_UP,			// ��
	SKBCTRL_DOWN,		// ��
	SKBCTRL_LEFT,		// ��
	SKBCTRL_RIGHT,		// ��
	SKBCTRL_HELP,		// �w���v
	SKBCTRL_KANJI,		// ����
	SKBCTRL_TAB,		// �^�u
	SKBCTRL_ABCQWERTY,	// ABC<->QWERTY

	SKBCTRL_MAX,
};



#define MODEBAR_HIRAGANA	(0)


typedef struct skey_s         skey_t;
typedef struct inhibitChar_s  inhibitChar_t;

typedef struct
{
	u_char ucUseAtok;
	int  (*readResource)  (skey_t *pKey, const char *filename, u_int keyTopNo, inhibitChar_t *inhibitChar, void *pTmp);
	void (*deleteResource)(skey_t *pKey);
	void (*makePacketsSkbBody)(sceVif1Packet *pk, skey_t *pKey,
								textureInfo_t *pTexBtn, textureInfo_t *pTexChar, textureInfo_t *pTexBg);
	void (*makePacketsCursor)(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pTexBtn);
	void (*setPad)        (skey_t *pKey, u_short down, u_short up, u_short press, u_short repeat);

	void (*open) (skey_t *pKey);
	void (*close)(skey_t *pKey);

	void (*mouseMove)			(skey_t *pKey, iPoint_t *pPoint);
	void (*mouseLButtonDown)	(skey_t *pKey, iPoint_t *pPoint);
	void (*mouseLButtonUp)		(skey_t *pKey, iPoint_t *pPoint);

	u_int (*searchFocus)  (skey_t *pKeyNext, sprite_t *pNowSp);


	// ����n
	void (*trans)         (skey_t *pKey, int x, int y);
	void (*changeAlpha)   (skey_t *pKey, int index, u_char alpha);
	void (*initKeyTop)    (skey_t *pKey);

	void (*initSpecialKey)(skey_t *pKey, u_char flag);

} exportSkb_t;


// SOFTKB�֐��Q������Q�Ƃł���\����
typedef struct
{
	u_char			ucUseMouse;			// �}�E�X�g�p
										// 0 : �g�p���Ȃ�
										// 1 : �g�p����

	u_int			uiDragState;		// �h���b�O����
										// 1bit : ���{�^��
										// 2bit : �E�{�^��
										// 4bit : �z�C�[��

	u_int			uiOldCurPos[3];		// �h���b�O�����܂ܑ��̃{�^���Ɉړ��������̌��o�p���[�N
										// [0] : ���{�^��
										// [1] : �E�{�^��
										// [2] : Wheel�{�^��

	u_int			uiOldCurPosList[2];	// �ꗗX,Y�l

	u_int			uiTogglePos16;		// �g�O��POS[��΍��W]
	u_int			uiDragToggle;
} protectedSkb_t;


// ���X�g�֘A
#define VISIBLE_CHARNUM_H		(16)		// ���������̌����镶����
#define VISIBLE_CHARNUM_V		(8)			// ���������̌����镶����

typedef struct
{
	u_char buf[4];

} UTF8Char_t;

typedef struct
{
	UTF8Char_t		utf8Char;
	int				iFontWidth;

} UTF8CharDrawCache_t;

typedef struct
{
	int		iScrollPosY;		// �X�N���[��Y�l
	int		iGlobalCurPosX;		
	int		iGlobalCurPosY;
	int		iRepeatAccel;
	int		iOldPosY16;			// �g�O��
	sprite_t spToggle16;

	u_int	auiCodeTable[600];
	int		iLineNum;

	int		iUtf8CacheTopLine;

	UTF8CharDrawCache_t	utf8Cache[VISIBLE_CHARNUM_H * VISIBLE_CHARNUM_V];

	// �ŗL
	charCodeBlock_t *pListItem;
} skblist_t;




//============================================================================
// �L�[���\�[�X�\����
typedef struct
{
	u_short	uvCharIndex[2];		// ����	UV���ւ̃I�t�Z�b�g
	u_short	ctrlCode;			// �R���g���[���R�[�h(ESC�ACAPSLOCK�ASHIFT�AENTER�A���������ATAB)
	u_int	key[10];			// �����R�[�h
} buttonState_t;


typedef struct
{
	buttonState_t	state[8];
	u_short			spBaseIndex;	// �L�[�x�[�X �X�v���C�g���ւ̃I�t�Z�b�g
	u_short			spCharIndex;	// �L�[�g�b�v �X�v���C�g���ւ̃I�t�Z�b�g

	u_short			uvBaseIndex[2];	// �L�[�x�[�X UV���ւ̃I�t�Z�b�g
									// 0: �ʏ�
									// 1: �����ꂽ��

	u_int			rgba;			// �X�v���C�g�F
	u_char			animFlag;		// �A�j���[�V�����p�t���O
									// 0x01 : �ʏ�
									// 0x02 : �����ꂽ
									// 0x04 : �I���ł��Ȃ�
									// 0x10 : ����(�_�~�[)

	u_short			direction[4];	// �����L�[�ɂ��J�[�\���̍s�����`
									// [0] : ��
									// [1] : ��
									// [2] : ��
									// [3] : ��

} button_t;


// �\�t�g�E�F�A�L�[�{�[�h�̏��
typedef struct
{
	unsigned int w, h;				// �\�t�g�E�F�A�L�[�{�[�h�̕��A����
} skbInfo_t;


// �L�[�z��e���ւ̃C���f�b�N�X
typedef struct
{
	int length;						// �f�[�^�̒���(�o�C�g�P��)
	int offset;						// �f�[�^�I�t�Z�b�g(�t�@�C���̐擪����)
} lump_t;


// �L�[���\�[�X�t�@�C�����̏��
enum
{
	LUMP_SPRITE,	// �X�v���C�g���̉�
	LUMP_UV,		// UV���̉�
	LUMP_KBD,		// �L�[���̉�
	LUMP_PANE,		// �w�i�`�b�v�̉�
	LUMP_SKB,		// �\�t�g�E�F�A�L�[�{�[�h�̏��

	LUMP_MAX
};





//============================================================================
// �J�[�\����`
#define SKBCURSOR_UV_X	STG(208)
#define SKBCURSOR_UV_Y	STG(156)
#define SKBCURSOR_UV_W	STG(26/2)
#define SKBCURSOR_UV_H	STG(26/2)
#define SKBCURSOR_SP_W	(26/2)
#define SKBCURSOR_SP_H	(26/2)

const static uv_t g_uvCursors[] =
{
	{	SKBCURSOR_UV_X,					SKBCURSOR_UV_Y,					SKBCURSOR_UV_W,		SKBCURSOR_UV_H	},	// ����"��"�e�N�X�`��
	{	SKBCURSOR_UV_X+SKBCURSOR_UV_W,	SKBCURSOR_UV_Y,					SKBCURSOR_UV_W,		SKBCURSOR_UV_H	},	// �E��"��"�e�N�X�`��
	{	SKBCURSOR_UV_X,					SKBCURSOR_UV_Y+SKBCURSOR_UV_H,	SKBCURSOR_UV_W,		SKBCURSOR_UV_H	},	// ����"��"�e�N�X�`��
	{	SKBCURSOR_UV_X+SKBCURSOR_UV_W,	SKBCURSOR_UV_Y+SKBCURSOR_UV_H,	SKBCURSOR_UV_W,		SKBCURSOR_UV_H	},	// �E��"��"�e�N�X�`��
	{	0,								0,								0,					0				},	// �_�~�[
};




//============================================================================
// �eSOFTKB�ł̋��ʊ֐�

int  readResourceFrame(skey_t *pKey, const char *filename, u_int keyTopNo, inhibitChar_t *inhibitChar, void *pTmp);
void makePacketsBody(sceVif1Packet *pk, skey_t *pKey,
						 textureInfo_t *pTexBtn, textureInfo_t *pTexChar, textureInfo_t *pTexBg);

void setLayout(skey_t *pKey);
void setInhibitChars(skey_t *pKey, inhibitChar_t *inhibitChar);
void setEnableChars(skey_t *pKey);
void setReleseChars(skey_t *pKey);

void pushEnter(skey_t *pKey);
void pushLeftArrow(skey_t *pKey);
void pushRightArrow(skey_t *pKey);
void pushUpArrow(skey_t *pKey);
void pushDownArrow(skey_t *pKey);
void pushSpace(skey_t *pKey);
void pushShift(skey_t *pKey, u_char flag);
void releaseShift(skey_t *pKey, u_char flag);
void pushCapsLock(skey_t *pKey);
void pushEsc(skey_t *pKey);
void pushBackSpace(skey_t *pKey);
void pushTab(skey_t *pKey);
void pushKanji(skey_t *pKey);

void moveCursor(skey_t *pKey, int dx, int dy);
void makeCursorSprite(sprite_t *pSpSrc, sprite_t *pSpDst, u_int floating);


int onStateCtrlCode(skey_t *pKey, u_int ctrlCode, u_char state);
int offStateCtrlCode(skey_t *pKey, u_int ctrlCode, u_char state);
int onStateKey(skey_t *pKey, u_int *key, u_char state);
int offStateKey(skey_t *pKey, u_int *key, u_char state);

int searchKeyByPoint(skey_t *pKey, iPoint_t *pPt);


#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// ! __SHAREDSKB_H__
