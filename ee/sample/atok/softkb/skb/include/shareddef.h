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

#ifndef __SHAREDDEF_H__
#define __SHAREDDEF_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#include <assert.h>
#include <libccc.h>
#include <libpkt.h>

#ifndef ATOK_NO_LINK
#include <libatok.h>
#else
#define DUMYSIZE (16)
typedef unsigned char sceAtokContext[DUMYSIZE] __attribute__((aligned(64)));
#endif

#include "extypes.h"

#define STG(_v_)			(((_v_) << 4) + 8)

#define SKB_ASSERT(_a_)     assert((int)_a_)
#define SKBSMPL_ASSERT(_a_) assert((int)_a_)

#define LOCALPATH_MAX		(256)
#if CDBOOT
#define LOCALPATH			"cdrom0:\\RESOURCE"
#else
#define LOCALPATH			"host0:skb/resource"
#endif

// �\�t�g�E�F�A�L�[�{�[�h�Ŏg���郊�\�[�X�ւ̃p�X
#if CDBOOT
#define SKB_JPKEY_PATH		LOCALPATH"\\KEY\\JP"		// ���{�����L�[�g�b�v�̃p�X
#define SKB_OTHERKEY_PATH	LOCALPATH"\\KEY\\OTHER"		// �C�O�����L�[�g�b�v�̃p�X
#define SKB_SKBTEX_PATH		LOCALPATH"\\TEX\\SKB"		// �e�N�X�`���̃p�X
#define SKB_HELPTEX_PATH	LOCALPATH"\\TEX\\HELP"		// �w���v�e�N�X�`���̃p�X
#else
#define SKB_JPKEY_PATH		LOCALPATH"/key/jp"		// ���{�����L�[�g�b�v�̃p�X
#define SKB_OTHERKEY_PATH	LOCALPATH"/key/other"	// �C�O�����L�[�g�b�v�̃p�X
#define SKB_SKBTEX_PATH		LOCALPATH"/tex/skb"		// �e�N�X�`���̃p�X
#define SKB_HELPTEX_PATH	LOCALPATH"/tex/help"	// �w���v�e�N�X�`���̃p�X
#endif

// �d�������
#define REGION_J	(0)
#define REGION_A	(1)
#define REGION_E	(2)

typedef enum
{
	// �L�[�{�[�h
	ENV_KEYBD_TYPE,				// �^�C�v
	ENV_KEYBD_REPEATW,			// ���s�[�g�E�F�C�g
	ENV_KEYBD_REPEATS,			// ���s�[�g�J�n

	// �}�E�X
	ENV_MOUSE_SPEED,			// �X�s�[�h
	ENV_MOUSE_DBLCLK,			// �_�u���N���b�N�Ԋu
	ENV_MOUSE_LR,				// LEFTHAND RIGHTHAND

	// ATOK
	ENV_ATOK_MODE,				// ���[�}�����́A��������
	ENV_ATOK_BIND,				// �o�C���h�^�C�v

	// �\�t�g�E�F�A�L�[�{�[�h
	ENV_SOFTKB_ONOFF,			// ON/OFF
	ENV_SOFTKB_QWERTY,			// ABC�z��orQWERTY���H

	ENV_MAX
} SYSenv_m;


// �L�[�o�C���h
#define KEYBIND_TYPE1	(0)
#define KEYBIND_TYPE2	(1)

// ��������FEP
#define FEPMODE_HIRAGANA	(0)
#define FEPMODE_KATAKANA	(1)
#define FEPMODE_ALPHABET	(2)

typedef IRECT		iRect_t;
typedef IPOINT		iPoint_t;

typedef struct
{
	int x __attribute__((aligned(16)));
	int y, z;
	u_short w, h;
} sprite_t __attribute__((aligned(16)));

typedef struct
{
	unsigned short u __attribute__((aligned(8)));
	unsigned short v, tw, th;
} uv_t __attribute__((aligned(8)));

typedef struct
{
	unsigned short spIndex;
	unsigned short uvIndex;
	unsigned int   rgba;
} paneChip_t;


typedef struct
{
	char		*pTim2Buffer;			// TIM2�o�b�t�@�|�C���^
	sceGsTex0	GsTex0;					// ���W�X�^�ݒ�
	u_int		nTexSizeW, nTexSizeH;	// �e�N�X�`�����A����

	u_char		bMipmapLv;				// TIM2�̍ő�~�b�v�}�b�v���x��(0�`6)
	u_long64	GsMiptbp1;				// TIM2��MIPTBP1���W�X�^�l
	u_long64	GsMiptbp2;				// TIM2��MIPTBP2���W�X�^�l
	u_int		nLod;					// MIPMAP�̂��߂�LOD�l
	u_int		nFilter;				// MIPMAP���̃t�B���^���[�h
	u_int		nClutNo;				// CLUT�ԍ�
} textureInfo_t;


#define TF_FRAME_ELEMENT	(9)
enum
{
	// �A���t�@�l
	TF_ALPHA_EDITORBG,		// �e�L�X�g���͕����̔w�i
	TF_ALPHA_FRONT,			// ����
	TF_ALPHA_BACK,			// �����w�i
	TF_ALPHA_CARET,			// �L�����b�g
	TF_ALPHA_CANDLISTBG,	// ���ꗗ�w�i

	TF_ALPHA_MAX
};

typedef struct
{
	int					iHide;					// 0: �\��/ 1: ��\��
	iRect_t				rcEdit16;				// �e�L�X�g�t�B�[���h�ʒu�A�T�C�Y

	textureInfo_t		tex;					// �e�N�X�`�����
	struct
	{
		sprite_t		aSP[TF_FRAME_ELEMENT];	// �X�v���C�g
		uv_t			aUV[TF_FRAME_ELEMENT];	// UV
		paneChip_t		aPN[TF_FRAME_ELEMENT];	// PANE
	} textField, candList;						// ----�e�L�X�g�t�B�[���h�p
												// ----���ꗗ�p

	u_char		alphaValue[TF_ALPHA_MAX];

	// �X�N���[��
	int			iScrollPosX16;		// X�l
	int			iScrollLX16;
	int			iScrollRX16;

	// �F
	u_char	ucConvCol[4][4];			// �ϊ���      ������
	u_char	ucMarkCol[4][4];			// ���ڕ���    ������
	u_char	ucMarkBGCandListCol[4][4];	// ���ꗗ���ڕ�����
	u_char	ucFontBGCol[4][4];			// �ʏ�t�H���g������
	u_int	uiCaretColor;				// �L�����b�g�F

	int		iCaretAnimSpeed;			// �L�����b�g�A�j���[�V�����X�s�[�h

	// �����֘A
	int		fontH;						// �t�H���g����
	int		dispMode;					// �����\�����[�h

} textField_t;

typedef struct
{
	u_int start;
	u_int end;
} charCodeBlock_t;








// �F������32�r�b�g�^�̒�`�ƁA����������}�N��
#define DEFAULT_RGBA			(0x80808080)
#define DEFAULT_DISABLE_RGBA	(0x40404040)


//============================================================================
typedef struct
{
	u_int	uiCharHeight;		// �t�H���g�̍���

	int  (*getToCaretPos)(void);	// �L�����b�g�̈ʒu
	// ������`��
	u_int *(*getStrings)(void);
	void (*drawStrings)(sceVif1Packet *pk, const u_int *pUCS4, const iRect_t *pClip, const iRect_t *rc16);
	int  (*drawStringBeforeChange)(sceVif1Packet *pk, const u_int *pUCS4, const iRect_t *pClip, const iRect_t *rc16);
	int  (*drawStringChanging)(sceVif1Packet *pk, const u_int *pUCS4, const iRect_t *pClip, const iRect_t *rc16,
							  int *scrollLX16, int *scrollRX16,
							  int frameW16, int frameH16, textField_t *pTf,
							  void drawCandListFrame(sceVif1Packet *pk, textField_t *pTf, iRect_t *rc16, int nCurrentCand));
	int  (*getCaretPos)(void);
	int  (*getStringWidth)(const u_int *str);	// ���͕�����̕����擾

	// ���ꗗ
	int   (*getAtokState)(void);				// ATOK�̓��͏�Ԃ��擾
	#if 1 //SKB_EX
	      //�L�����b�g���㏑�����[�h���ɕω������邽�߂ɒǉ�
	int   (*isInsertMode)(void);				// �}�����[�h������B
	#endif
} txtFImport_t;

typedef struct
{
	u_int	*puiUCS4;				// ������ւ̃|�C���^
	u_int	uiMaxTextLen;			// �ő啶����

	// �A�v������
	void (*atokON)(void);						// ATOK ON
	void (*atokOFF)(sceAtokContext *pAtok);		// ATOK OFF
	void (*ok)(void);							// ok
	void (*cancel)(void);						// �L�����Z��
	void (*changeMode)(int mode);				// ���̓��[�h�ύX
	int  (*changeText)(int changeType, const u_int *szChar, u_short ctrlCode);	// �����`�F�b�N
	u_char (*showHideSoftKB)(void);				// �\�t�g�E�F�A�L�[�{�[�h
												// 0: ��\��
												// 1: �\��
	void (*closeSoftKB)(void);					// �\�t�g�E�F�A�L�[�{�[�h����
	void (*openSoftKB)(char flag);				// �\�t�g�E�F�A�L�[�{�[�h�J��

	int (*getNumlockState)(void);				// ����NumLock��Ԃ��H
												// 0: �Ⴄ
												// 1: NumLock���

	// �d���Ǘ�
	int (*powerLock)(void);
	int (*powerUnlock)(void);

	int (*getStrWidthSJIS)(const char *sjis, float fontratio);		// �߂�l : 16�{���ꂽ�l

} fepImport_t;

typedef struct
{
	u_char	ucShowSKbd;							// �\�t�g�E�F�A�L�[�{�[�h
												// 0: ��\��
												// 1: �\��

	u_char	ucShowHelp;							// �w���v�\����
												// 0: ��\��
												// 1: �\��

	u_int	uiListFontW, uiListFontH;			// ���X�g�̕������A����

	void  (*changeInputMode)(u_char);			// ���̓��[�h�ύX
	void  (*abcToQwerty)(void);					// ABC <--> QWERTY
	int   (*getNowInputMode)(void);				// ���݂̓��̓��[�h���擾

	// �A�v��
	void (*exitOK)();														// ����I��
	void (*exitCancel)();													// �L�����Z���I��	�����setChars�ŕ₦��H
	void (*setChars)(const u_int *pUcs4, u_char Flag, u_short ctrlCode);	// �����ݒ�
																			// �X�y�[�X�L�[�A�����L�[�A�폜�L�[�܂�
	void (*showHelp)(u_char flag);		// �w���v�\��
	void (*keytopChange)(char keyNo);	// �L�[�g�b�v�ύX
	int (*getAtokInput)(void);			// ATOK�̃��[�h�擾(���͂����邩)
	u_int (*getAtokMode)(void);			// ATOK�̃��[�h�擾(�ϊ�����)
	void (*kakuteiAll)(void);			// �S���m��
	void (*kakuteiPart)(void);			// �����m��
	int  (*opening)(void);				// �I�[�v����
	int  (*closing)(void);				// �N���[�Y��

	// �����`��
	void (*setListFontEnv)(int x, int y);		// ���X�g���̃t�H���g�ݒ�
	void (*setListDrawString)(sceVif1Packet *pk,  const char *pUtf8, int x, int y);	// ���X�g���̕����`��
	void (*setListDrawStringF)(sceVif1Packet *pk, const char *pUtf8, int x, int y);	// ���X�g���̕����`��(�t�H�[�J�X����)
	int  (*getStringWidth)(const char *pUtf8);		// ������̕����擾

} skbImport_t;

typedef struct
{
	skbImport_t		skb;	// SOFTKB
	fepImport_t		fep;	// FEP
	txtFImport_t	txt;	// TEXT FIELD

	//----------------------
	// ���ʕ���
	u_char	ucUseAtok;							// ATOK
												// 0: �g��Ȃ�
												// 1: �g��

	u_int	iRegion;							// ���[�W����
												// 0: Japan
												// 1: Us
												// 2: Europe

	u_short	usLang;								// ����ݒ�

	u_short	usSystemIni[ENV_MAX];				// OSD�V�X�e���ݒ�
	u_int	uiScreenW, uiScreenH;				// �X�N���[���傫��

	int   (*thinkYHn)(int v);					// YH��ݒ�(1/1�ANTSC/PAL�l���AFRAME/FIELD�l��)
	int   (*thinkYH16)(int v);					// YH��ݒ�(16�{�ANTSC/PAL�l���AFRAME/FIELD�l��)
	int   (*thinkXW16)(int v);					// XW��ݒ�(16�{)
	int   (*thinkYH)(int v16);					// YH��ݒ�(1/16�ANTSC/PAL�l���AFRAME/FIELD�l��)
	int   (*thinkXW)(int v16);					// XW��ݒ�(1/16)
	void  (*dprintf)(const char *msg, ...);		// �f�o�b�O�pPRINTF
	u_int (*getNtscPal)(void);					// 0:NTSC/1:PAL
	u_int (*getFrameField)();					// 0:Field/1:Frame
	int  (*isConv)(void);						// �ϊ������H
												// 0: �Ⴄ
												// 1: �ϊ���

	// �������Ǘ�
	void* (*malign)(size_t align, size_t size);	// = memalign
	void  (*mfree)(void *p);					// = free

	// ��
	void (*se)(int soundNo);

	// �`��֘A
	int  (*readTex)(textureInfo_t *pTex, const char *filename);		// �e�N�X�`���ǂݍ���
	void (*deleteTex)(textureInfo_t *pTex);							// �e�N�X�`���j��
	void (*loadTex)(sceVif1Packet *pk, textureInfo_t *pTex);		// �e�N�X�`�����[�h
	void (*drawTex)(sceVif1Packet *pk, u_int col, sprite_t *pSp, uv_t *pUv, textureInfo_t *pTex);	// �e�N�X�`���`��
	void (*drawPane)(sceVif1Packet *pk, const IRECT *pRc, const RGBA32 c, int z);					// �`��
	void (*drawLine)(sceVif1Packet *pk, int x, int y, int cx, int cy, const RGBA32 c, int z);		// ���C���`��
	void (*drawFocus)(sceVif1Packet *pk, sprite_t *sp, u_int curPos, u_int col);					// �t�H�[�J�X�`��

} gImport_t;


//============================================================================
extern __inline__ u_int affectAlpha(u_int src, u_char alpha)
{
	u_char *pcol;
	u_char _alpha;
	u_int	col=0x80808080;

	pcol = (u_char*)&src;
	_alpha = (alpha * pcol[0])>>7;
	col = (u_int)_alpha;
	col <<= (6*4);
	col |= src & 0x00ffffff;

	return col;
}

#define ORIGINAL_PAL_RATIO	(1.19047619f)


#if 1//SKB_EX_Sound
enum
{
	// �L�[�g�b�v����
	SKBSE_ONKEY = 0,	// �L�[�g�b�v�̃L�[����
	SKBSE_BACKSPACE,	// BackSpace �i���{�^���������A�L�[�g�b�v���Backspace�������j
	SKBSE_ENTER,		// Enter         �i�L�[�g�b�v���Return�������j
	SKBSE_SHIFT,		// Shift          �iR1�������A�L�[�g�b�v���Shift�������j
	SKBSE_CARETMOVE,	// �L�����b�g�ړ��iR1L1�������j
	SKBSE_OK,		// ����          �iSTART�������j
	SKBSE_CANCEL,		// �߂�A�L�����Z���i�~�������j
	SKBSE_CHANGEKEYTOP,	// �L�[�g�b�v�ؑցiSELECT�������j
	SKBSE_CHANGEZENHAN,	// ���p�S�p�ؑ�
	SKBSE_ONHELP,		// �w���v�L�[������
	SKBSE_QWERTYABC,	// QWERTY<-->ABC�z��ύX�{�^��
	SKBSE_ONCAPSLOCK,	// CapsLock�L�[������
	SKBSE_ONSPACE,		// Space�L�[������

	// ���{��ϊ�����
	SKBSE_STARTTRANS,	// �ϊ��J�n�i���{�^���������j
	SKBSE_MOVECANDLIST,	// �ϊ����ꗗ���ړ��i�����{�^�������� �܂��́A���{�^���������j
	SKBSE_CANDDETECT,	// �ϊ���⌈��i���{�^���������j
	SKBSE_MOVECLS,		// �ϊ����߈ړ��i�����{�^���������j
	SKBSE_STRETCHCLS,	// �ϊ����ߐL�k�iR1�{�����{�^���������j
	SKBSE_DETECTEXIT,	// �m��I���iSTART�{�^���������j
					
	// �ȉ��A�ҏW����
	SKBSE_INHIBITINPUT,	// ���͋֎~���������͂��ꂽ��

	// IP�A�h���X���͎�
	SKBSE_ONKEY_IP,         // �L�[�g�b�v�̃L�[����
	SKBSE_BACKSPACE_IP,	// BackSpace �i���{�^���������A�L�[�g�b�v���Backspace�������j
	SKBSE_ENTER_IP,		// Enter         �i�L�[�g�b�v���Return�������j
	SKBSE_CARETMOVE_IP,	// �L�����b�g�ړ��iR2L2�������j
	SKBSE_OK_IP,		// ����          �iSTART�������j
	SKBSE_CANCEL_IP,	// �߂�A�L�����Z���i�~�������j
	SKBSE_ONSPACE_IP,	// Space�L�[������

	// IP�A�h���X���͌x����
	SKBSE_INHIBITINPUT_IP,	        // ���͋֎~���������͂��ꂽ��
	SKBSE_ONKEY2_IP,		// �������͎���256�ȏ�̏ꍇ,0��0���͂̏ꍇ
	SKBSE_ONKEY3_IP,		// DELETE�L�[����,��ɂ���ꍇ

	SKBSE_MAX
};
#endif


#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// !__SHAREDDEF_H__
