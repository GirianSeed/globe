/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.5
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
#ifndef __SKB_H__
#define __SKB_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

// �t�H���g�����F  tSkb_Ctl�\���̃����o  gaucFontCol[]�z��v�f
enum
{
	FONT_COL_NORMAL,        // �ʏ펞
	FONT_COL_CANDLIST_SUB,  // ���ꗗ�̊����ȊO�̕���
	FONT_COL_UNDERLINE,     // �A���_�[���C���F�ݒ�
	FONT_COL_FOCUS,         // �J�[�\�������鎞�̐F
	FONT_COL_LIST,          // ���X�g�̕����F

	FONT_COL_MAX,
};

// �t�H���g�w�i�F  tSkb_Ctl�\���̃����o  aucFontBGCol[]�z��v�f
enum
{
	FONT_BGCOL_NORMAL,       // aucFontBGCol[0] �ʏ펞
	FONT_BGCOL_BEFORECHANGE, // aucFontBGCol[1] �ϊ��O
	FONT_BGCOL_CHANGING,     // aucFontBGCol[2] �ϊ���
	FONT_BGCOL_CANDLIST,     // aucFontBGCol[3] ���ꗗ������

	FONT_BGCOL_MAX,
};

// �\�t�g�E�F�A�L�[�{�[�h tSkb_Ctl�\���̃����o KBD[] �z��v�f
enum
{
	SKBTYPE_HIRA = 0,  // �Ђ炪��
	SKBTYPE_KANA,      // ��������
	SKBTYPE_ABC,       // ABC�z��
	SKBTYPE_QWERTY,    // QWERTY�z��
	SKBTYPE_NUM,       // ����
	SKBTYPE_SIGN,      // �L��
	SKBTYPE_MAX,
};


#define MAX_CHAR_COUNT       (255)  // �ő���͕�����
#define MAX_CHAR_BUFF_COUNT  (MAX_CHAR_COUNT + 1)	// �ő���͕�����(Buffer�p)

typedef  struct
{
	// ������o�b�t�@
	u_int uiChars[MAX_CHAR_BUFF_COUNT];
	u_int uiCharNum;

	u_int iCoverView;  // �e�L�X�g�{�b�N�X�̕\���}�X�N
	                   // 0: �\��  1: �}�X�N
	// �t�H���g
	int fontSizeW, fontSizeH;

	// �t�H���g�F
	u_char aucFontCol[FONT_COL_MAX][4];        // �����F
	u_char aucFontBGCol[FONT_BGCOL_MAX][16];   // �F

	// �\�t�g�E�F�A�L�[�{�[�h
	skey_t KBD[SKBTYPE_MAX]; // �L�[�{�[�h�\����
	u_int  uiNowKeyType;     // KBD�̃C���f�b�N�X(SKBTYPE_xx)
	fepAtokInputMode_m inputMode[SKBTYPE_MAX]; // ���̓��[�h(FEP�p)
	u_int          auiNextKeyTop[SKBTYPE_MAX]; // uiNowKeyType���C���f�b�N�X�Ƃ���
	                                           // ���̃L�[�{�[�h�̃C���f�b�N�X�𓾂�ׂ̔z��
	iPoint_t              skbPos[SKBTYPE_MAX]; // �ʒu


	int               modeBarNum[SKBTYPE_MAX]; // ���[�h�o�[�N���G�C�g�ԍ��i�[
	int fepModeOnState;        // Fep On/Off ����Fep���[�h�L���ϐ�
	struct {
		int alphabetKeyType;   // 0 ABC�� 1 �o�b�L�[�{�[�h�z�� system.ini ����
		int inputDevice;       // 0 USBKB 1 PAD
	}Env;

	int iControlFlag;   // ���͏��̐�L��v�����邩  0:CLose�� 1:Open���`
	int iOpenFlag;      // �I�[�v�����Ă��邩          0:��Open  1:Open�����`
	int iUsbKeyFlag;    // USBKEY�g�p����              0:�g�p���Ă��Ȃ� 1:�g�p��
	int (*Message)( int type, void* text );	// �Ăяo�����֏����t�o

} tSkb_Ctl;

// �L�[�{�[�h�Z�b�g�̒�`
typedef struct{
	int startKeyboardType;          // �ŏ��̃L�[�{�[�h�̃C���f�b�N�X(SKBTYPE)
	struct{
		int next;                   // ���̃L�[�{�[�h�̃C���f�b�N�X(SKBTYPE)
		fepAtokInputMode_m fepMode; // �����t�F�v���[�h�A FEP_OFF�ȊO�͂��̌㎩���ݒ肳���
		int ucShift;                // ���� �S�p���p �L���v�X
		int* disableCtrl;           // �R���g���[���L�[�̗L����L�����
		iPoint_t Position;          // KBD      ��ʈʒu
	}Keyboard[SKBTYPE_MAX];
	struct{
		int modeCount;             // Fep�̓��̓��[�h��
		u_int* Mode;               // Fep�̓��̓��[�h�z��A�h���X
		iPoint_t Position;         // FepBar   ��ʈʒu
	}Fep;
	struct{
		int setCount;               // ModeBar �N���G�C�g���i�[
		u_int SetMode[SKBTYPE_MAX]; // Keyboard���Ƀ��[�h�o�[�̕\���O���t�B�b�N���i�[�A
		u_int ModeNum[SKBTYPE_MAX]; // SKBTYE�ʂ�Keyboard���ԍ����i�[
		iPoint_t Position;          // ModeBar   ��ʈʒu
	}Bar;
	struct{
		iRect_t  Rect;             // TextFiels ��ʈʒu
	}Text;
}KEYBOARD_SET;


void SKB_SetCallback( int(*func)(int skbmes,void* info) );
enum {
	SKBMES_RESULT    , // �e�L�X�g�ҏW����
	SKBMES_USBKEY_ON , // USB�L�[�{�[�h ON  ��
	SKBMES_USBKEY_OFF, // USB�L�[�{�[�h OFF ��
	SKBMES_HELP_SHOW , // help��� �\�� ON  ��
	SKBMES_HELP_HIDE , // help��� �\�� OFF ��
	SKBMES_FEP_ON ,    // �����ϊ� ON  ��
	SKBMES_FEP_OFF,    // �����ϊ� OFF ��
	SKBMES_SE     ,    // SoundEffect ��� ��
};

typedef struct{
	int region;          // ���{��
	struct{
		char*	system;
		char*	config;
		char*	sysDic;
		char*	usrDic;
	}AtokPath;
	u_int Gs_workAddress256;
	void*(*memAlign)(size_t, size_t);
	void (*memFree)(void *p);
}SKB_ARG;

extern int Init_SKBConfig(SKB_ARG*arg, char *fsName, short keyType, short repWait, short repSpeed );

extern int  SKB_Init(void);
extern void SKB_Exit(void);

void SKB_GetKbdConfig( int* kbType, int *repeatWait, int *repeatSpeed,int *region );

int  SKB_GetStatus();
int  SKB_GetZenkakuMode();
void SKB_Control( u_long ulPad );
void SKB_SetUsbKeyborad( int kbStat, USBKBDATA_t* ,int numLock );

void SKB_Open( KEYBOARD_SET* kbSet, int editType, u_int* usc4 );
void*SKB_MakeVif1CallPacket( int clipOffX, int clipOffY );
void SKB_Close( char* );

void SKB_SetInhibit_MaskChar( u_int (*ucs4)[10], int count );
void SKB_SetInhibit_Disable( u_int (*ucs4)[10], int count );
void SKB_SetInhibit_Enable( u_int (*ucs4)[10], int count );
int  SKB_IsDisableUcs4( u_int ucs4 );

void SKB_GetLastKey(void);

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif  //!__SKB_H__
