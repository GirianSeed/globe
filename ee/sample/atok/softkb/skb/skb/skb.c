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
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdarg.h>

#include <eekernel.h>
#include <eetypes.h>
#include <sifdev.h>
#include <libpkt.h>
#include <libpad.h>

#include "env.h"
#include "shareddef.h"

#include "tim2wrap.h"
#include "libsoftkb.h"
#include "libfep.h"
#include "fontwrap.h"
#include "textField.h"
#include "fepbar.h"
#include "sysenv.h"

#include "skb.h"


//============================================================================
#define SOUND_ON    // ������

#define Printf   (0)? 0 : printf
#define PRINTF   (1)? 0 : printf
//============================================================================


typedef iRect_t RECT;

extern void SetClipOffset(int x,int y);
extern void HelpOffset( int x, int y );

//--------------------------------------------------------------------------------------
// libsoftkb.a fep.a textField �̂��߂́A�C���|�[�g�֐�
//--------------------------------------------------------------------------------------
extern void SetRectFill(sceVif1Packet *pk, const IRECT *pRc, const RGBA32 c, int z);
extern void SetClipArea(sceVif1Packet *pVif1Pk, const iRect_t *pRc);
extern void SetLine(sceVif1Packet *pk, int x, int y, int cx, int cy, const RGBA32 c, int z);
extern void SetRectFillEx(sceVif1Packet *pk, const IRECT *pRc, const RGBA32 *c, int z);

static void initColor(void);
static u_int getFrameField(void);
static void dprintf(const char *msg, ...);
static void se(int soundNo);
static u_int getNtscPal(void);
static int thinkYHn(int v);
static int thinkYH(int v16);
static int thinkXW(int v16);
static int thinkYH16(int v);
static int thinkXW16(int v);
static void ucsToUtf8(const u_int *ucs4, u_char *utf8);

static void setChars(const u_int *pUcs4, u_char Flag, u_short ctrlCode);
static void skb_exitOK(void);
static void showHelp(u_char flag);
static int  getNowInputMode(void);
static void openSoftKB(char flag);
static void somewhereFocus(sceVif1Packet *pk, sprite_t *sp, u_int curPos, u_int col);
static void changeInputMode(u_char uiMode);
static void keytopChange(char keyN);
static void exchangeABCtoQWERTY(void);
static void setSkbListFontEnv(int x, int y);
static void setSkbListDrawString(sceVif1Packet *pk, const char *pUtf8, int x, int y);
static void setSkbListDrawStringFocus(sceVif1Packet *pk, const char *pUtf8, int x, int y);
static int getStringWidthUtf8(const char *pUtf8);
u_char showHideSoftKB(void);
void closeSoftKB(void);
int opening(void);
int closing(void);


static u_int *getStrings(void);
static void drawStrings(sceVif1Packet *pk, const u_int *pUCS4, const iRect_t *pClip, const iRect_t *pRc);
static void setCharsNormalColor(void);
static void setSubCandListColor(void);
static void setUnderlineColor(void);
static void setNormalBGColor(void);
static void setBeforeChangBGColor(void);
static void setChangingBGColor(void);
static int drawStringBeforeChange(sceVif1Packet *pk, const u_int *pUCS4, const iRect_t *pClip, const iRect_t *pRc);
static int getStringWidthSJIS(const char *psjis, float ratio);
static int drawStringChanging(sceVif1Packet *pk, const u_int *pUCS4, const iRect_t *pClip, const iRect_t *pRc,
                              int *scrollLX16, int *scrollRX16, int frameW, int frameH, textField_t *pTf,
                              void drawCandListFrame(sceVif1Packet *pk, textField_t *pTf, iRect_t *rc16, int nCurrentCand));
static int getStringWidth(const u_int *pUCS4);

static int powerLock(void);
static int powerUnlock(void);
static void atokON(void);
static void atokOFF(sceAtokContext *pAtok);
static void exitOK();
static void cancel(void);
static void changeMode(int mode);
static int changeText(int changetype, const u_int *szChar, u_short ctrlCode);
int getToCaretPos(void);


//============================================================================

// �X���b�h
#define ATOK_THREAD_PRIORITY  (60)   // �񓯊�ATOK�G���W���̃X���b�h�v���C�I���e�B

// �p�P�b�g�o�b�t�@
#define PACKET_MAX  (1024*3*MAX_CHAR_BUFF_COUNT) // ������p�p�P�b�g�T�C�Y

// �����̑傫��
#define APP_FONT_BIG				(1.0f)
#define APP_FONT_DEF				(0.9f)
#define APP_FONT_MDL				(0.8f)
#define APP_FONT_SML				(0.7f)

#define FONT_W						(22)
#define FONT_H						(23)


//============================================================================
// �ϕϐ��Q
//============================================================================
tSkb_Ctl     g_Sample;     // �\�t�g�E�F�A�L�[�{�[�h����p�̈�̎���
textField_t  textField;    // �e�L�X�g�t�B�[���h

#define INHIBIT_CHAR_MAX    (500)                // ���͋֎~������
#define INHIBIT_SUB_CHAR_MAX (16)                // �L�[���͉̓��͋֎~����
// ����p�o�b�t�@
inhibitChar_t inhibitChar;                               // ���͋֎~����
u_int         inhibitUCS4[INHIBIT_CHAR_MAX][10];         // ���͋֎~�����p�o�b�t�@
inhibitChar_t inhibitSubChar;                            // �L�[���͉̓��͋֎~����
u_int         inhibitSubUCS4[INHIBIT_SUB_CHAR_MAX][10];  // �L�[���͉̓��͋֎~�����p�o�b�t�@

// ���[�N�o�b�t�@
inhibitChar_t EnableChar;                                // ���͋������֎~�����֕ϊ��O�̃��[�N
u_int         EnableUCS4[INHIBIT_CHAR_MAX][10];          // ���͋������o�b�t�@
inhibitChar_t inhibitMaskChar;                           // �L�[���͉̓��͋֎~���� �f�[�^�x�[�X
u_int         inhibitMaskUCS4[INHIBIT_SUB_CHAR_MAX][10]; // �L�[���͉̓��͋֎~�����p�o�b�t�@

//------------------ �C���^�[�t�F�[�X --------------------------------------------------
gImport_t g_import;     // �C���|�[�g�p�֐��|�C���^�p
gImport_t g_importFep;  // �C���|�[�g�p�֐��|�C���^�p(FEP��p)


//------------------ �`��p�P�b�g     --------------------------------------------------
static sceVif1Packet g_pkVif1[2];    // �p�P�b�g�̊Ǘ����
static u_int         g_pkVif1_ID=0;  // �p�P�b�g�̊Ǘ����
static u_char g_pkVif1Buff[2][PACKET_MAX] __attribute__((aligned(128)));	// �p�P�b�g�̊Ǘ����

//------------------ USB-KEYBOARD InterFace    --------------------------------------------------
int         KbStat;     // USB-KEYBOARD����p
USBKBDATA_t KbData;     // USB-KEYBOARD����p
int         KbNumLock;  // USB-KEYBOARD����p



// �L���ꗗ
static charCodeBlock_t g_ListItems[] =
{
    // ���p����
//    { 0x0020, 0x007f },
//    { 0x00a0, 0x00df },

    // �S�p����
    { 0x8140, 0x81ff },
    { 0x8240, 0x82ff },
    { 0x8340, 0x83df },
    { 0x8440, 0x84bf },

    { 0x8740, 0x879f },

    { 0x8890, 0x88ff },
    { 0x8940, 0x89ff },
    { 0x8a40, 0x8aff },
    { 0x8b40, 0x8bff },
    { 0x8c40, 0x8cff },
    { 0x8d40, 0x8dff },
    { 0x8e40, 0x8eff },
    { 0x8f40, 0x8fff },

    { 0x9040, 0x90ff },
    { 0x9140, 0x91ff },
    { 0x9240, 0x92ff },
    { 0x9340, 0x93ff },
    { 0x9440, 0x94ff },
    { 0x9540, 0x95ff },
    { 0x9640, 0x96ff },
    { 0x9740, 0x97ff },
    { 0x9840, 0x98ff },
    { 0x9940, 0x99ff },
    { 0x9a40, 0x9aff },
    { 0x9b40, 0x9bff },
    { 0x9c40, 0x9cff },
    { 0x9d40, 0x9dff },
    { 0x9e40, 0x9eff },
    { 0x9f40, 0x9fff },

    { 0xe040, 0xe0ff },
    { 0xe140, 0xe1ff },
    { 0xe240, 0xe2ff },
    { 0xe340, 0xe3ff },
    { 0xe440, 0xe4ff },
    { 0xe540, 0xe5ff },
    { 0xe640, 0xe6ff },
    { 0xe740, 0xe7ff },
    { 0xe840, 0xe8ff },
    { 0xe940, 0xe9ff },
    { 0xea40, 0xeaaf },

//    �t�H���g�ɓ����Ă��Ȃ�
//    { 0xed40, 0xedff },
//    { 0xee40, 0xeeff },
//
//    { 0xfa40, 0xfaff },
//    { 0xfb40, 0xfbff },
//    { 0xfc40, 0xfc4f },

	{ 0x0000, 0x0000 }		// END CODE
};


//======================================================================================
//
//     tSkb_Ctl g_Sample;  ����֐�
//
//======================================================================================

//--------- �F�ݒ� -------------------
u_char g_FontColSrc[][4] =        /* initColor() �ŁAg_Sample�ɁA�R�s�[����� */
{
	{0x80,	0x80,	0x80,	0x80},/* FONT_COL_NORMAL      */ // �ʏ� ����(��)
	{0x28,	0x28,	0x28,	0x80},/* FONT_COL_CANDLIST_SU */ // ���ꗗ�̊����ӊO�̕���
	{0x70,	0x70,	0xff,	0xff},/* FONT_COL_UNDERLINE   */ // �A���_�[���C���F�ݒ�
	{0x50,	0x50,	0xc8,	0x80},/* FONT_COL_FOCUS       */ // �\�t�g�E�F�A�L�[�{�[�h���X�g���̃J�[�\��������Ƃ��̐F
	{0x60,	0x60,	0x60,	0x80},/* FONT_COL_LIST        */ // �\�t�g�E�F�A�L�[�{�[�h���X�g���̒ʏ�F
};

u_char g_FontBGColSrc[][16] =
{
	{	// �ʏ펞 (����)
		0x80,	0x80,	0x80,	0x00,	0x80,	0x80,	0x80,	0x00,
		0x80,	0x80,	0x80,	0x00,	0x80,	0x80,	0x80,	0x00,
	},
	{	// �ϊ��O
		0x1f,	0x1f,	0x1f,	0x80,	0x1f,	0x1f,	0x1f,	0x80,
		0x50,	0x50,	0x78,	0x80,	0x50,	0x50,	0x78,	0x80,
	},
	{	// �ϊ����w�i
		0x1f,	0x1f,	0x1f,	0x80,	0x1f,	0x1f,	0x1f,	0x80,
		0x6c,	0xe8,	0x88,	0x80,	0x6c,	0xe8,	0x88,	0x80,
	},
	{	// ���ꗗ������
		0x1f,	0x1f,	0x1f,	0x80,	0x1f,	0x1f,	0x1f,	0x80,
		0x6c,	0xe8,	0x88,	0x80,	0x6c,	0xe8,	0x88,	0x80,
	},
};

// �F��񏉊���
static void initColor(void)
{
	int i;
	for(i=0; i<FONT_COL_MAX; i++)
	{
		memcpy(&g_Sample.aucFontCol[i][0],&g_FontColSrc[i][0], sizeof(u_char)*4);
	}

	for(i=0; i<FONT_BGCOL_MAX; i++)
	{
		memcpy(&g_Sample.aucFontBGCol[i][0],&g_FontBGColSrc[i][0], sizeof(u_char)*16);
	}
}

// ���� �ʏ�F �ݒ�
static void setCharsNormalColor(void)
{
	Font_SetColor(g_Sample.aucFontCol[FONT_COL_NORMAL][0], g_Sample.aucFontCol[FONT_COL_NORMAL][1], g_Sample.aucFontCol[FONT_COL_NORMAL][2], g_Sample.aucFontCol[FONT_COL_NORMAL][3]);
}

// ���ꗗ���̊����ӊO�̕����F�ݒ�
static void setSubCandListColor(void)
{
	Font_SetColor(g_Sample.aucFontCol[FONT_COL_CANDLIST_SUB][0], g_Sample.aucFontCol[FONT_COL_CANDLIST_SUB][1], g_Sample.aucFontCol[FONT_COL_CANDLIST_SUB][2], g_Sample.aucFontCol[FONT_COL_CANDLIST_SUB][3]);
}

// �����F�ݒ�
static void setUnderlineColor(void)
{
	Font_SetULColor(g_Sample.aucFontCol[FONT_COL_UNDERLINE][0], g_Sample.aucFontCol[FONT_COL_UNDERLINE][1], g_Sample.aucFontCol[FONT_COL_UNDERLINE][2], g_Sample.aucFontCol[FONT_COL_UNDERLINE][3]);
}


// �����w�i�F �ݒ�
static void setNormalBGColor(void)
{
	Font_SetBGColorEx(0, g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][0], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][1], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][2], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][3]);
	Font_SetBGColorEx(1, g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][4], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][5], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][6], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][7]);
	Font_SetBGColorEx(2, g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][8], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][9], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][10], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][11]);
	Font_SetBGColorEx(3, g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][12], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][13], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][14], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][15]);
}

// �����w�i �ϊ��O �ݒ�
static void setBeforeChangBGColor(void)
{
	Font_SetBGColorEx(0, g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][0], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][1], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][2], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][3]);
	Font_SetBGColorEx(1, g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][4], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][5], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][6], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][7]);
	Font_SetBGColorEx(2, g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][8], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][9], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][10], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][11]);
	Font_SetBGColorEx(3, g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][12], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][13], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][14], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][15]);
}

// �����w�i �ϊ��� �ݒ�
static void setChangingBGColor(void)
{
	Font_SetBGColorEx(0, g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][0], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][1], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][2], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][3]);
	Font_SetBGColorEx(1, g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][4], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][5], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][6], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][7]);
	Font_SetBGColorEx(2, g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][8], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][9], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][10], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][11]);
	Font_SetBGColorEx(3, g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][12], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][13], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][14], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][15]);
}

//--------- �L�[�{�[�h ABC�z��_QWERTY�z�� ����ւ� -------------------
static int swap_ABC_QWERTY( int notFoundSetType )
{
	int n;

	Printf("swap checking...\n");
	if( g_import.iRegion != REGION_J )	return 0;	// ���{�ł̂ݓ��삵�܂��B

	for( n=0; n<SKBTYPE_MAX; n++ ){
		if(g_Sample.auiNextKeyTop[n] == SKBTYPE_QWERTY ||
		   g_Sample.auiNextKeyTop[n] == SKBTYPE_ABC      )	break;
	}
	if( n == SKBTYPE_MAX )	return 0;               // ������Ȃ������̂œ���ւ�����

	if( g_Sample.modeBarNum[SKBTYPE_QWERTY] < 0 )	return 0; // ������Ȃ������̂œ���ւ�����
	if( g_Sample.modeBarNum[SKBTYPE_ABC   ] < 0 )	return 0; // ������Ȃ������̂œ���ւ�����

	if( g_Sample.uiNowKeyType == SKBTYPE_ABC ){
		g_Sample.uiNowKeyType = SKBTYPE_QWERTY;    // ���݂̃L�[�{�[�h������ւ��Ώۂ�������A�A�N�e�B�u�L�[�{�[�h�ύX
	}
	else if( g_Sample.uiNowKeyType == SKBTYPE_QWERTY ){
		g_Sample.uiNowKeyType = SKBTYPE_ABC;      // ���݂̃L�[�{�[�h������ւ��Ώۂ�������A�A�N�e�B�u�L�[�{�[�h�ύX 
	}
	else{ // ���݂̃L�[�{�[�h��ABC QWERTY �ǂ���ł��Ȃ������� �����ɏ]���B
		if( notFoundSetType >= 0 && notFoundSetType < SKBTYPE_MAX ){
			if( g_Sample.modeBarNum[notFoundSetType] < 0 )	return 0;	//�������s���������疳��

			g_Sample.uiNowKeyType = notFoundSetType;
			// ������ABC QWERTY �ǂ���ł��Ȃ�������A
			if( g_Sample.uiNowKeyType != SKBTYPE_ABC && g_Sample.uiNowKeyType != SKBTYPE_QWERTY ){
				return -1; //����ւ��͖����������A���̃L�[�{�[�h���ύX���ꂽ�B
			}
		}
	}

	Printf("swap SKBTYPE_QWERTY<->SKBTYPE_ABC \n");

	for( n=0; n<SKBTYPE_MAX; n++ ){
		if( g_Sample.auiNextKeyTop[n] == SKBTYPE_QWERTY ){
			g_Sample.auiNextKeyTop[n] = SKBTYPE_ABC;
		}
		else if( g_Sample.auiNextKeyTop[n] == SKBTYPE_ABC ){
			g_Sample.auiNextKeyTop[n] = SKBTYPE_QWERTY;
		}
	}

	return 1; //�L�[�{�[�h�����ւ����B
}

//-------- ���͋֎~�����̐ݒ� Open���ɗL����-------------------
static void inhibitChar_SetAllKeyborad()
{
	int i;
	skey_t *pKey;

	for(i=0; i < SKBTYPE_MAX; i++){
		pKey = &(g_Sample.KBD[i]);
		pKey->pInhibitChar = &inhibitChar;
		#if 0 // �I�[�v�����ɐݒ�B
		setEnableChars(pKey);                  // �S�ẴL�[����͉ɖ߂��B
		setInhibitChars( pKey, &inhibitChar ); // �֎~��������L�[�ɐݒ�
		#endif
	}
}

// usbkeyborad ���p�A�t�F�v���̓��[�h�ύX����
static void changeInputMode_usbkb()
{
	int mode   = SoftKB_GetInputMode( &g_Sample.KBD[g_Sample.uiNowKeyType] );
	int modekb = g_Sample.inputMode[ g_Sample.uiNowKeyType ];

	switch( mode ){
	case SKBMODE_HIRAGANA:
	                        if( modekb==FEPOFF ) mode = FEPON_HIRA_FIX;
	                        else                 mode = FEPON_HIRA;
	                        break;
	case SKBMODE_HAN_KATAKANA:
	                        if( modekb==FEPOFF ) mode = FEPON_HAN_KATAKANA_FIX;
	                        else                 mode = FEPON_HAN_KATAKANA;
	                        break;
	case SKBMODE_ZEN_KATAKANA:
	                        if( modekb==FEPOFF ) mode = FEPON_ZEN_KATAKANA_FIX;
	                        else                 mode = FEPON_ZEN_KATAKANA;
	                        break;
	case SKBMODE_HAN_ALPHABET:         // ALPHABET_FIX���[�h�́A�ϊ����J�i���ł�̂Ŏg�p���Ȃ�
	                        if( modekb==FEPOFF ) mode = FEPOFF;//mode = FEPON_HAN_ALPHABET_FIX;
	                        else                 mode = FEPON_HAN_ALPHABET;
	                        break;
	case SKBMODE_ZEN_ALPHABET:
	                        if( modekb==FEPOFF ) mode = FEPOFF;//mode = FEPON_ZEN_ALPHABET_FIX;
	                        else                 mode = FEPON_ZEN_ALPHABET;
	                        break;
	}

	Fep_SetInputMode(mode);
	Printf("changeInputMode_usbkb %x\n",mode);
}


//============================================================================
// �e�L�X�g
//============================================================================
// �����R�[�h��ύX(UCS2->UTF8)
static void ucsToUtf8(const u_int *ucs4, u_char *utf8)
{
	sceCccUTF8** dst = (sceCccUTF8**)&utf8;
	int i=0;

	SKBSMPL_ASSERT(ucs4);
	SKBSMPL_ASSERT(utf8);

	while(*(ucs4+i))
	{
		sceCccEncodeUTF8(dst, *(ucs4+i));
		i++;
	}
	*(*dst) = 0;
}



//============================================================================
// g_import ���ʕ� �o�^�֐�
//============================================================================
// �f�o�b�O�p�o��
static void dprintf(const char *msg, ...)
{
	#ifdef DEBUG
	va_list argptr;
	char    text[1024];

	va_start(argptr, msg);
	vsprintf(text, msg, argptr);
	va_end(argptr);

	printf("%s", text);
	#endif
}

// �t���[�����[�h/�t�B�[���h���[�h�擾
static u_int getFrameField(void)
{
	#if FIELD_MODE
	return 0;
	#else
	return 1;
	#endif
}

// NTSC/PAL�擾
static u_int getNtscPal(void)
{
	// NTSC
	return 0;
}

// ���ʉ�
static void se(int soundNo)
{
	#ifdef SOUND_ON
	if( g_Sample.Message ){
		g_Sample.Message( SKBMES_SE, (void*)soundNo );
	}
	#endif
}

//g_import.readTex   = Tim2_ReadTim2;      tim2wrap.c
//g_import.deleteTex = Tim2_DeleteTim2;    tim2wrap.c
//g_import.loadTex   = Tim2_SendTim2;      tim2wrap.c
//g_import.drawTex   = Tim2_DrawSprite;    tim2wrap.c
//g_import.drawPane  = SetRectFill;        draw.c
//g_import.drawLine  = SetLine;            draw.c

// Y�����̍��W�v�Z
static int thinkYHn(int v)
{
	return (int)(((float)v*VRESOLUTION_RATIO)*TV_RATIO);
}

// Y�����̍��W�v�Z
static int thinkYH(int v16)
{
	return (((int)(((float)v16/VRESOLUTION_RATIO)/TV_RATIO)) >> 4);
}

// X�����̍��W�v�Z
static int thinkXW(int v16)
{
	return v16 >> 4;
}

// Y�����̍��W�v�Z
static int thinkYH16(int v)
{
#if PAL_MODE
	return (((int)(((float)(v << 4))*(VRESOLUTION_RATIO*TV_RATIO))));
#else
	// NTSC
	#if FIELD_MODE
		return (v << 4);
	#else
		return (v << 3);
	#endif
#endif
}

// X�����̍��W�v�Z
static int thinkXW16(int v)
{
	return (v << 4);
}

// SOFTKB �t�H�[�J�X�ݒ�
static void somewhereFocus(sceVif1Packet *pk, sprite_t *sp, u_int curPos, u_int col)
{
	SoftKB_DrawFocus(pk, sp, curPos, col);
}

//g_import.isConv    = Fep_GetNowConv;    libFep.a



//============================================================================
// g_import.skb �\�t�g�E�F�A�L�[�{�[�h�� �o�^�֐�
//============================================================================

// SOFTKB�̃��[�h�ݒ肩��FEP���̃��[�h�ݒ�ɕύX����ׂ̊֐�
static void changeInputMode(u_char uiMode)
{
	fepAtokInputMode_m mode = FEPOFF;

	mode = g_Sample.inputMode[g_Sample.uiNowKeyType];
	if( mode != FEPOFF ){
		switch(uiMode)
		{
		case SKBMODE_HIRAGANA:
			mode = FEPON_HIRA;
			break;
		case SKBMODE_HAN_KATAKANA:
			mode = FEPON_HAN_KATAKANA;
			break;
		case SKBMODE_ZEN_KATAKANA:
			mode = FEPON_ZEN_KATAKANA;
			break;
		case SKBMODE_HAN_ALPHABET:
			mode = FEPOFF;
			break;
		case SKBMODE_ZEN_ALPHABET:
			mode = FEPOFF;
		break;
		}
	}
	if( g_Sample.Message){
		if( mode != FEPOFF ){
			g_Sample.Message( SKBMES_FEP_ON , (void*)0 );
		}
		else{
			g_Sample.Message( SKBMES_FEP_OFF, (void*)0 );
		}
	}

	// ���̓��[�h�ݒ�
	if(Fep_SetInputMode(mode))
	{
		printf("ERROR : changeInputMode - non support mode.");
	}
	Printf("MSG : changeInputMode uiMode[%d],mode[%d]\n",uiMode,mode);
}


// ABC�z���QWERTY�z������ւ���
static void exchangeABCtoQWERTY(void)
{
	u_char ucInputMode;
	skey_t *pCloseKey = &g_Sample.KBD[g_Sample.uiNowKeyType]; //exchange ����O�ɕۑ��B
	skey_t *pKey = NULL;

	if(! swap_ABC_QWERTY(SKBTYPE_QWERTY) )	return;

	// ����ւ�����AABC��QWERTY���̃��[�h�����ݒ� 
	g_Sample.Env.alphabetKeyType ^= 1;

	SoftKB_Close( pCloseKey );// ���݂̃L�[�{�[�hClose

	pKey = &g_Sample.KBD[g_Sample.uiNowKeyType];
	ucInputMode = SoftKB_GetInputMode(pKey);        // 
	changeInputMode(ucInputMode);                   // ���̓��[�h�ύX

	SoftKB_Open(&g_Sample.KBD[g_Sample.uiNowKeyType]); // �V�����L�[�{�[�hOpen
}


// ���̃R�[���o�b�N�́A�L�[�{�[�h��ނ�����킷���[�h�o�[��
// ���Ԗځi�N���G�C�g���j�ɂȂ��Ă��邩�Ԃ��悤�ɂ���B
static int getNowInputMode(void)
{
	return g_Sample.modeBarNum[ g_Sample.uiNowKeyType ];
}


// g_import.skb.getAtokInput= Fep_GetNowAtokInput;  libfep.a
// g_import.skb.getAtokMode = Fep_Changing;         libfep.a
// g_import.skb.kakuteiPart = Fep_SetKakuteiPart;   libfep.a
// g_import.skb.kakuteiAll  = Fep_SetKakuteiAll;    libfep.a


// �����R�[�h(UCS4)��SOFTKB->FEP�Ɉ��n���B
static void setChars(const u_int *pUcs4, u_char Flag, u_short ctrlCode)
{
	static u_int uiTmp[10];

	if(pUcs4 != NULL)
	{
		memcpy(uiTmp, pUcs4, sizeof(u_int)*10);
		Fep_PostSoftKey(uiTmp, Flag, ctrlCode); // FEP�ɕ����o�^
	}
	else if(ctrlCode != 0xFFFFFFFF)
	{
		memset(uiTmp, 0x0, sizeof(u_int)*10);
		Fep_PostSoftKey(uiTmp, Flag, ctrlCode); // FEP�ɕ����o�^
	}
}


// g_import.skb.exitOK =
static void skb_exitOK()
{
	Printf("MSG : skb_exitOK\n");
}


// �w���v�\��
// u_char flag : 0 : ��\��
//               1 : �\��
static void showHelp(u_char flag)
{
	g_Sample.iCoverView = flag;
	g_import.skb.ucShowHelp = flag;
	if( g_Sample.Message ){
		if(flag)	g_Sample.Message( SKBMES_HELP_SHOW,(void*)0 );
		else    	g_Sample.Message( SKBMES_HELP_HIDE,(void*)0 );
	}
	Printf("MSG : showHelp - %d\n", flag);
}


// �L�[�g�b�v�̕ύX
// char keyNo : -1 : ���̃L�[�g�b�v
//            : ���̑� : ���ڃL�[�g�b�v��ݒ�
static void keytopChange(char keyNo)
{
	u_char ucInputMode;
	skey_t *pKey = NULL,*pOldKey;

	if( g_Sample.auiNextKeyTop[g_Sample.uiNowKeyType] == g_Sample.uiNowKeyType ){
		return;
	}

	SoftKB_Close(&g_Sample.KBD[g_Sample.uiNowKeyType]);// ���݂̃L�[�{�[�hClose
	pOldKey = &g_Sample.KBD[g_Sample.uiNowKeyType];

	if(keyNo == SKBMODE_NOHIT)
	{
		// ���̃L�[�g�b�v
		g_Sample.uiNowKeyType = g_Sample.auiNextKeyTop[g_Sample.uiNowKeyType];
		PRINTF("%d, %d\n", g_Sample.uiNowKeyType, g_Sample.auiNextKeyTop[g_Sample.uiNowKeyType]);
	}
	else
	{
		int i;
		if(keyNo < 0)
		{
			keyNo += (SKBTYPE_MAX-1); // �������Ɉړ�
		}

		for(i=0; i<keyNo; i++)
		{
			g_Sample.uiNowKeyType = g_Sample.auiNextKeyTop[g_Sample.uiNowKeyType];	// �E�����Ɉړ�
		}
	}

	pKey = &g_Sample.KBD[g_Sample.uiNowKeyType];
	ucInputMode = SoftKB_GetInputMode(pKey);
	changeInputMode(ucInputMode);                // ���̓��[�h�ύX
	//pKey->uiCursorPos = SoftKB_SearchFocus(pOldKey, pKey);

	SoftKB_Open(&g_Sample.KBD[g_Sample.uiNowKeyType]); // �V�����L�[�{�[�hOpen
}

// �I�[�v�����̌Ă΂��֐��B�A�j���[�V�����}���ȂǂɎg�p�B
// g_import.skb.opening
int opening(void)
{
	Printf("skb.opening\n");
	return 1;
}

// �N���[�Y���ɌĂ΂��֐��B�A�j���[�V�����}���ȂǂɎg�p�B
// g_import.skb.close
int closing(void)
{
	Printf("skb.closing\n");
	return 1;
}

// �\�t�g�E�F�A�L�[�{�[�h���X�g
// �t�H���g���ݒ�
static void setSkbListFontEnv(int x, int y)
{
	Font_SetRatio(APP_FONT_MDL);    // �t�H���g�̑傫���ݒ�
	Font_SetLocate(x, y);           // �ʒu�ݒ�
	Font_SetEffect(FONT_EFFECT_BG); // ���ʐݒ�
	setCharsNormalColor();          // �ʏ핶���F�ݒ�
	setNormalBGColor();             // �ʏ�w�i�F�ݒ�
}

// �ʏ핶���`��
static void setSkbListDrawString(sceVif1Packet *pk, const char *pUtf8, int x, int y)
{
	Font_SetColor(g_Sample.aucFontCol[FONT_COL_LIST][0], g_Sample.aucFontCol[FONT_COL_LIST][1], g_Sample.aucFontCol[FONT_COL_LIST][2], g_Sample.aucFontCol[FONT_COL_LIST][3]);
	Font_SetLocate(x, y);                   // �ʒu�ݒ�
	Font_PutsPacket(pUtf8, pk, PACKET_MAX); // �p�P�b�g�쐬
}
// �J�[�\�����������Ă��鎞�̕����`��
static void setSkbListDrawStringFocus(sceVif1Packet *pk, const char *pUtf8, int x, int y)
{
	Font_SetColor(g_Sample.aucFontCol[FONT_COL_FOCUS][0], g_Sample.aucFontCol[FONT_COL_FOCUS][1], g_Sample.aucFontCol[FONT_COL_FOCUS][2], g_Sample.aucFontCol[FONT_COL_FOCUS][3]);
	Font_SetLocate(x, y);                   // �ʒu�ݒ�
	Font_PutsPacket(pUtf8, pk, PACKET_MAX); // �p�P�b�g�쐬
}

// ������̕���Ԃ�
static int getStringWidthUtf8(const char *pUtf8)
{
	Font_SetRatio(APP_FONT_DEF);
	return Font_GetStrLength(pUtf8);
}



//============================================================================
// g_import �e������ �o�^�֐�
//============================================================================
// ATOK��ON�ɂȂ���(�������̓��[�hON) �S�p���p�L�[��
static void atokON(void)
{
	if( g_Sample.fepModeOnState!=-1 ){
		Fep_SetInputMode( g_Sample.fepModeOnState );
		g_Sample.fepModeOnState=-1;
	}
	else Fep_SetInputMode( FEPOFF );

	Printf("MSG : atokON - fepModeOnState %d\n",g_Sample.fepModeOnState);
}

// ATOK��OFF�ɂȂ���(�������̓��[�hOFF)
static void atokOFF(sceAtokContext *pAtok)
{
	// fepbar��ҏW
	g_Sample.fepModeOnState = Fepbar_GetNowMode();

	Printf("MSG : atokOFF\n");
}

// ���[�h���ύX����
static void changeMode(int mode)
{
	if(mode == -1){
		Fepbar_NextMode();        // AtokModeChange
	}else{
		Fepbar_SetModeType(mode);
	}
	Printf("MSG : changeModex[0x%x]\n",mode);
}


static int changeText(int changetype, const u_int *szChar, u_short ctrlCode)
{
	int i=0, j=0;
	u_int num  = inhibitChar.uiCharNum; // �w�肳��Ă���֎~������
	u_int numc = inhibitChar.uiCKeyNum; // �w�肳��Ă���֎~�R���g���[����

	if(szChar == NULL && ctrlCode == 0)
	{
		// ���͖���������
		return FEP_EDIT_NG;
	}

	if(szChar != NULL)
	{
		// ���͋֎~�����`�F�b�N
		// �֎~�������P�ꕶ��
		for(i=0; i<num; i++)
		{
			j=0;
			#if 1//SKB_EX inhibitChar
			if( inhibitChar.pChar[i][1] && inhibitChar.pChar[i][1]!=0 )	continue;
			while(*(szChar+j))
			{
				// ���������T�|�[�g
				if(inhibitChar.pChar[i][0] == *(szChar+j))
				{
					return FEP_EDIT_NG; // ���͋֎~ �G���[
				}
				j++;
			}
			#else
			while(*(szChar+j))
			{
				// ���������T�|�[�g
				if(inhibitChar.pChar[i][0] == *(szChar+j))
				{
					return FEP_EDIT_NG; // ���͋֎~ �G���[
				}
				j++;
			}
			// ��������̂P�������`�F�b�N
			#endif
		}
		#if 1//SKB_EX inhibitSub
		for(i=0; i<inhibitSubChar.uiCharNum; i++){
			j=0;
			while(*(szChar+j))
			{
				if(inhibitSubChar.pChar[i][0] == *(szChar+j))
				{
					return FEP_EDIT_NG; // ���͋֎~ �G���[
				}
				j++;
			}
		}
		#endif
	}
	else
	{
		// ���͋֎~�R�[�h�`�F�b�N
		for(i=0; i<numc; i++)
		{
			// �R���g���[���R�[�h�Ń`�F�b�N
			if(ctrlCode == inhibitChar.pCtrlKey[i])
			{
				return FEP_EDIT_NG; // ���͋֎~ �G���[
			}
		}
	}

	// ���E�������`�F�b�N
	{
		char utf8Buf[1024];

		ucsToUtf8(g_Sample.uiChars, utf8Buf); // �m��ς݂̕����R�[�h�ϊ� UCS4 -> UTF8
		i = strlen(utf8Buf);
		if(changetype == FEP_EDIT_ADD)
		{
			// �}������
			ucsToUtf8(szChar, utf8Buf);       // �}�������R�[�h�ϊ� UCS4 -> UTF8
			i += strlen(utf8Buf);

			if(g_import.fep.uiMaxTextLen < i)
			{
				return FEP_EDIT_OVER; // �o�b�t�@�I�[�o�[�G���[
			}
		}
		else if(changetype == FEP_EDIT_INSERT)
		{
			// �㏑������
			char utf8BufOverWrite[1024];
			const u_int *pw = NULL;

			ucsToUtf8(szChar, utf8Buf);       // �㏑�������R�[�h�ϊ� UCS4 -> UTF8
			i += strlen(utf8Buf);             // ����㏑���������������擾

			pw = Fep_GetInsertChars();        // �㏑������镶����ւ̃|�C���^�擾
			ucsToUtf8(pw, utf8BufOverWrite);  // �㏑�������R�[�h�ϊ� UCS4 -> UTF8
			i -= strlen(utf8BufOverWrite);

			if(g_import.fep.uiMaxTextLen < i)
			{
				return FEP_EDIT_OVER; // �o�b�t�@�I�[�o�[�G���[
			}
		}
	}

	return FEP_EDIT_OK;
}


// �d�����b�N
static int powerLock(void)
{
	Printf("Power Lock\n");
	return 1;
}

// �d���A�����b�N
static int powerUnlock(void)
{
	Printf("Power UnLock\n");
	return 0;
}

// �I������
// pFep->ok
static void exitOK()
{
	Printf("MSG : exitOK\n");

	if( g_Sample.iOpenFlag > 0 ){  // 0:��Open 1:Open��
		g_Sample.iOpenFlag = -1;   // 0:��Open 1:Open��
		// �ҏW���ʂ��R�[���o�b�N�֐��œ`����B
		if( g_Sample.Message )	g_Sample.Message( SKBMES_RESULT, (void*)g_Sample.uiChars );
		return;
	}
}

// �L�����Z��
void cancel(void)
{
	Printf("MSG : cancel\n");

	if( g_Sample.iOpenFlag > 0 ){  // 0:��Open 1:Open��
		g_Sample.iOpenFlag = -1;   // 0:��Open 1:Open��
		// �ҏW���ʂ��R�[���o�b�N�֐��œ`����B(cancel�́A�e�L�X�g�͂m�t�k�k)
		if( g_Sample.Message )	g_Sample.Message( SKBMES_RESULT, (void*)0 );
	}
}

// USB�L�[�{�[�h�̂m�����k�������擾�B
int getNumlockState()
{
	return KbNumLock;
}

// �\�t�g�E�F�A�L�[�{�[�h�̕\��/��\�����擾
u_char showHideSoftKB(void)
{
	return g_import.skb.ucShowSKbd;
}

// �\�t�g�E�F�A�L�[�{�[�h�̔�\��
void closeSoftKB(void)
{
	Printf("closeSoftKB\n");

	g_import.skb.ucShowSKbd = 0;	// �\�t�g�E�F�A�L�[�{�[�h��\��
	g_import.skb.ucShowHelp = 0;	// �w���v��\��
}

// �\�t�g�E�F�A�L�[�{�[�h��\��
// char flag : 0 : �P�ɃI�[�v��
//             1 : �\�t�g�E�F�A�L�[�{�[�h�̑J�ڃT�C�N����������
static void openSoftKB(char flag)
{
	Printf("openSoftKB %d\n",flag);
	g_import.skb.ucShowSKbd = 1;

	#if 1//SKB_EX Insert 
	Fep_SetInsertMode(FEP_EDIT_INSERT); // �\�t�g�E�F�A�L�[�{�[�h�͑}�����[�h
	#endif

	if(flag)
	{
		skey_t *pKey, *pOldKey;
		u_char ucInputMode;

		pOldKey = &g_Sample.KBD[g_Sample.uiNowKeyType];
		pKey = &g_Sample.KBD[g_Sample.uiNowKeyType];

		ucInputMode = SoftKB_GetInputMode(pKey); // �S�p/���p���[�h���擾
		SoftKB_Open(pKey);                       // �V�����L�[�{�[�hOpen
		changeInputMode(ucInputMode);            // ���̓��[�h�ύX
		pKey->uiCursorPos = SoftKB_SearchFocus(pOldKey, pKey);
	}

	{
		fepAtokInputMode_m mode = g_Sample.inputMode[g_Sample.uiNowKeyType];

		//�e�����̓��̓��[�h�w��i�L�[�{�[�h�ɂ���ē��͓r���̕����񂪂�������I�������j
		Fep_SetInputMode(mode);	
		if( g_Sample.Message ){
			if( mode != FEPOFF )	g_Sample.Message( SKBMES_FEP_ON , (void*)0 );
			else                	g_Sample.Message( SKBMES_FEP_OFF, (void*)0 );
		}
	}

	g_Sample.iCoverView = 0; // text box �\��
}

// ������̕����擾
// SJIS�R�[�h�̕������UTF8�ɕϊ�����
static int getStringWidthSJIS(const char *psjis, float ratio)
{
	char buf[2048];

	Font_SetRatio(ratio);
	sceCccSJIStoUTF8((sceCccUTF8*)buf, HENKAN_SIZE, (sceCccSJISTF*)psjis);

	return g_import.thinkXW16(Font_GetStrLength(buf));
}



//============================================================================
// g_import.txt �e�L�X�g�t�B�[���h���A�o�^�֐�
//============================================================================

//g_import.txt.uiCharHeight = Font_GetHeight();  // fontwrap.a

// ������擾(UCS4)
static u_int *getStrings(void)
{
	return g_Sample.uiChars;
}

// ������`��
static void drawStrings(sceVif1Packet *pk, const u_int *pUCS4, const iRect_t *pClip, const iRect_t *pRc)
{
	char buf[2048];

	if(pUCS4 == NULL)
	{
		return;
	}

	ucsToUtf8(pUCS4, buf);       // UCS4->UTF8�ɕϊ�

	Font_SetRatio(APP_FONT_DEF);                       // �t�H���g�̑傫���ݒ�
	Font_SetLocate(pRc->x, g_import.thinkYHn(pRc->y)); // �ʒu�ݒ�
	Font_SetEffect(FONT_EFFECT_BG);                    // ���ʐݒ�
	setCharsNormalColor();       // �ʏ핶���F�ݒ�
	setNormalBGColor();          // �ʏ�w�i�F�ݒ�

	Font_SetClipOn();
	SetClipArea(pk, pClip);                // �N���b�v�ݒ�
	Font_PutsPacket(buf, pk, PACKET_MAX);  // �p�P�b�g�쐬
	SetClipArea(pk, NULL);                 // �N���b�v����
	Font_SetClipOff();
}

// �ϊ�������\��
static int drawStringBeforeChange(sceVif1Packet *pk, const u_int *pUCS4, const iRect_t *pClip, const iRect_t *pRc)
{
	char sjis[HENKAN_SIZE];
	char buf[2048];
	int  strWidth16 = 0;

	// �A���_�[���C��������,�w�i�F�ݒ�
	Font_SetEffect(FONT_EFFECT_BG | FONT_EFFECT_ULD);
	Font_SetRatio(APP_FONT_DEF);  // �t�H���g�̑傫���ݒ�
	setCharsNormalColor();        // �����F��ʏ�̐F�ɂ���
	setUnderlineColor();          // �����F��ݒ�
	setBeforeChangBGColor();      // �w�i�F��ݒ�

	// �ϊ��O�������ATOK����擾(SJIS)
	Fep_GetConvertChars(sjis);

	// �`��p��SJIS->UTF8�ɕϊ�
	sceCccSJIStoUTF8((sceCccUTF8*)buf, HENKAN_SIZE, (sceCccSJISTF*)sjis);
	strWidth16 = g_import.thinkXW16(Font_GetStrLength(buf)); // ���łɕ����񕝂����߂�

	// �`��
	Font_SetLocate(pRc->x, g_import.thinkYHn(pRc->y));       // �ʒu�ݒ�

	Font_SetClipOn();
	SetClipArea(pk, pClip);               // �N���b�v�ݒ�
	Font_PutsPacket(buf, pk, PACKET_MAX); // �p�P�b�g�쐬
	SetClipArea(pk, NULL);                // �N���b�v����
	Font_SetClipOff();

	return strWidth16;
}


// �ϊ���������\��
// ���߂��Ƃɕ`��
// ��⃊�X�g�̃T�C�Y���w��
static int drawStringChanging(sceVif1Packet *pk, const u_int *pUCS4, const iRect_t *pClip, const iRect_t *pRc,
                              int *scrollLX16, int *scrollRX16, int frameW, int frameH, textField_t *pTf,
                              void drawCandListFrame(sceVif1Packet *pk, textField_t *pTf, iRect_t *rc16, int nCurrentCand) )
{
	// �������`�悷��ۂ�Y�AH�̒l�́A�t�H���g���C�u�����̕��ŏ���Ɍv�Z�����̂�
	// �t�B�[���h���[�h�̍ۂ�Y�AH�̒l��OK�B
	int  i;
	int  maxClust, nowClust;
	char buf[2048];
	char *psjis = NULL;
	int  strWidth16 = 0;
	int  iCharStartPosX16 = 0;

	maxClust = Fep_GetClusterNum();
	nowClust = Fep_GetNowCluster();

	setCharsNormalColor();
	setUnderlineColor();

	Font_SetRatio(APP_FONT_DEF);                       // �t�H���g�̑傫���ݒ�
	Font_SetLocate(pRc->x, g_import.thinkYHn(pRc->y)); // �ʒu�ݒ�
	Font_SetClipOn();
	SetClipArea(pk, pClip); // �N���b�v�ݒ�
	for(i=0; i<maxClust; i++)
	{
		psjis = Fep_GetClusterChars(i);
		sceCccSJIStoUTF8((sceCccUTF8*)buf, HENKAN_SIZE, (sceCccSJISTF*)psjis);
		strWidth16 += g_import.thinkXW16(Font_GetStrLength(buf));

		if(i == nowClust)
		{
			// ���ڕ��� ---
			// ���ڕ��߂��[�܂�悤�ɃX�N���[��
			iCharStartPosX16 = *scrollLX16 = Font_GetLocateX();

			// �w�i�F�ݒ�
			Font_SetEffect(FONT_EFFECT_BG);
			setChangingBGColor();

			Font_PutsPacket(buf, pk, PACKET_MAX);	// �p�P�b�g�쐬

			// ���ڕ��߂��[�܂�悤�ɃX�N���[��
			*scrollRX16 = Font_GetLocateX();
		}
		else
		{
			// ���ڕ��� �ȊO---
			Font_SetEffect(FONT_EFFECT_BG | FONT_EFFECT_UL);
			setUnderlineColor();
			setBeforeChangBGColor();

			Font_PutsPacket(buf, pk, PACKET_MAX);	// �p�P�b�g�쐬
		}
	}
	SetClipArea(pk, NULL);  // �N���b�v����
	Font_SetClipOff();


	//--------------------------------------------------------
	if(!Fep_ShowCandList())
	{
		// ���ꗗ�K�v�Ȃ�
		return strWidth16;
	}

	// ���ꗗ��\��
	{
		char *pCandStart;                        // ��⃊�X�g�f�[�^�̐擪
		int iPixels;                             // ��⃊�X�g�̉����T�C�Y(GS���W�n)
		char buf[16];
		char sjis[16];
		char itemMain[HENKAN_SIZE];              // ���ꗗ�f�[�^
		char itemSub[HENKAN_SIZE];               // ���ꗗ�f�[�^ �Y����
		char itemTmp[HENKAN_SIZE*2];             // 

		int i;
		int nCands   = Fep_CandListNum();        // ���ꗗ�f�[�^��
		int nOffCand = Fep_GetCandListOffset();  // ���ꗗ�I�t�Z�b�g
		int nCurCand = Fep_GetCurrentCand();     // ���ꗗ���ݑI�𒆂�NO
		int nCandMax = Fep_GetCandListMax();     // ���ꗗ�ő�l
		int gx = 0, gy = 0;
		char *cp;
		iRect_t clip;                            // �N���b�s���O�̈�
		iRect_t rc;                              // 

		const int listHm = g_import.thinkYHn(2); //
		const int listHm16 = listHm << 4;

		int charH = 0, charH16 = 0;              // �����̍���

		pCandStart = Fep_GetCandStart();

		iPixels = Fep_GetCandListMaxWidth(APP_FONT_MDL, APP_FONT_MDL); // ���ꗗ�ŕ\������镶����̍ő啝�𓾂�
		iPixels += getStringWidthSJIS("0.", APP_FONT_MDL);             // ���ꗗ�C���f�b�N�X
		charH   = Font_GetHeight();     // �����̍����擾
		charH16 = charH << 4;
		Font_SetRatio(APP_FONT_MDL);    // �t�H���g�̑傫���ݒ�

		// �I��NO/�S�̂̐������̕��������傫�������ꍇ
		{
			int pix = 0;
			int currentCand = Fep_GetCurrentCand();
			int maxCand     = Fep_GetCandListMax();
			char cNowIndex[16];
			sprintf(cNowIndex, "%d/%d", currentCand, maxCand); // �I��NO/�S�̂̐�

			pix = getStringWidthSJIS(cNowIndex, APP_FONT_MDL);
			if(pix > iPixels)
			{
				iPixels = pix;
			}
		}

		// ���ꗗ
		rc.x = iCharStartPosX16;
		rc.y = g_import.thinkYH16(pRc->y) + charH16;
		rc.w = iPixels + g_import.thinkXW16(frameW<<1);
		rc.h = ((charH16+listHm16)*(nCands+1)) + g_import.thinkYH16(frameH<<1); // ((���{���Ԃ̋��) * ��␔) + �㉺�̃t���[���̍���

		// �g����ʂɔ[�܂肫��Ȃ��ꍇ�A�\���ʒu���C��
		{
			int w = (rc.x+rc.w) - ((SCREEN_WIDTH<<4) - (SAFESCREEN_X16<<1));
			if(w > 0)
			{
				rc.x -= w;
				if(rc.x < SAFESCREEN_X16)
				{
					rc.x = SAFESCREEN_X16; // ���ꗗ�`��ʒu�͈��S�t���[�����ɂ����߂�
				}
			}

			iCharStartPosX16 = rc.x;
		}
		drawCandListFrame(pk, pTf, &rc, (nCurCand-nOffCand)); // �g�`��

		// �N���b�v�̈�ݒ�
		clip.x = rc.x>>4;
		clip.y = rc.y>>4;
		clip.w = rc.w>>4;
		clip.h = rc.h>>4;
		Font_SetClipOn();
		SetClipArea(pk, &clip); // �N���b�v�ݒ�


		// �I�����J�[�T��`��
		{
			iRect_t curRc;

			curRc.x = iCharStartPosX16 + g_import.thinkXW16(frameW);
			curRc.y = (g_import.thinkYH16(pRc->y) + (charH16) + g_import.thinkYH16(frameH)) + ((charH16+listHm16)*(nCurCand-nOffCand));
			curRc.w = iPixels;
			curRc.h = charH16 + listHm16;

			SetRectFillEx(pk, &curRc, (RGBA32*)&g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][0], 0);
		}

		// �������`�� --------
		Font_SetEffect(FONT_EFFECT_BG);
		// sjis[0] = '1'   // �C���f�b�N�X
		sjis[1] = '.';
		sjis[2] = 0x00;
		cp = pCandStart;

		gx = g_import.thinkXW(iCharStartPosX16) + frameW;
		gy = g_import.thinkYHn(pRc->y + frameH) + charH;
		for(i=0; i<nCands; i++)
		{
			Font_SetRatio(APP_FONT_MDL); // �t�H���g�̑傫���ݒ�
			Font_SetLocate(gx, gy);

			// ���ԍ���`��
			if(i == 9)
			{
				sjis[0] = '0';
			}
			else
			{
				sjis[0] = '1' + i;
			}
			sceCccSJIStoUTF8((sceCccUTF8*)buf, HENKAN_SIZE, (sceCccSJISTF*)sjis);
			setCharsNormalColor();
			setNormalBGColor();
			Font_PutsPacket(buf, pk, PACKET_MAX);

			// ��╶�����`��
			memset(itemMain, 0x00, HENKAN_SIZE);
			memset(itemSub,  0x00, HENKAN_SIZE);
			cp = Fep_GetCandListItem(cp, itemMain, itemSub); // �����擾(����:itemMain, �Ђ炪��:itemSub)
			sceCccSJIStoUTF8((sceCccUTF8*)itemTmp, HENKAN_SIZE, (sceCccSJISTF*)itemMain);
			Font_PutsPacket(itemTmp, pk, PACKET_MAX); // �p�P�b�g�쐬

			// �t����̕`��
			setSubCandListColor();
			sceCccSJIStoUTF8((sceCccUTF8*)itemTmp, HENKAN_SIZE, (sceCccSJISTF*)itemSub);
			Font_PutsPacket(itemTmp, pk, PACKET_MAX); // �p�P�b�g�쐬

			gy += (charH) + listHm;  // ���̌��ׂ̈�Y�l�쐬
		}

		// ���̐��`��
		Font_SetRatio(APP_FONT_SML); // �t�H���g�̑傫���ݒ�
		setCharsNormalColor();
		setNormalBGColor();
		Font_SetLocate(gx, gy+2);
		sprintf(sjis, "%d/%d", nCurCand+1, nCandMax);
		sceCccSJIStoUTF8((sceCccUTF8*)buf, HENKAN_SIZE, (sceCccSJISTF*)sjis);
		Font_PutsPacket(buf, pk, PACKET_MAX);

		SetClipArea(pk, NULL);  // �N���b�v����
		Font_SetClipOff();
	}

	return strWidth16;
}

// ������(UCS2)�̕�(�s�N�Z��)�擾
static int getStringWidth(const u_int *pUCS4)
{
	char buf[2048];
	// UCS4->UTF8�ɕϊ�
	ucsToUtf8(pUCS4, buf);

	Font_SetRatio(APP_FONT_DEF);
	return Font_GetStrLength(buf) << 4;
}

//g_import.txt.getCaretPos    = Fep_GetCaretPos;
//g_import.txt.getAtokState   = Fep_AtokGetInputState;

#if 1 //SKB_EX Insert
int isInsertMode(void)  //�㏑�����[�h���ɃL�����b�g�̃O���t�B�b�N��ω������邽�߂ɒǉ��B
{
	if( Fep_GetInsertMode() == FEP_EDIT_ADD )   return 0;
	else                                        return 1;
}
#endif

// �L�����b�g�ʒu���擾
int getToCaretPos(void)
{
	char aSjis[1024];

	Fep_ConvStrings(aSjis);
	return getStringWidthSJIS(aSjis, APP_FONT_DEF);
}



//======================================================================================
//  �L�[�{�[�h�Z�b�g�A���擾����
//======================================================================================
int KeyBoardSet_Setup( KEYBOARD_SET* kbSet, tSkb_Ctl* skbCtl, int region  )
{
	int n;
	int alphabetKeyType=-1;

	int*next    = &skbCtl->auiNextKeyTop[0];
	int*modeBar = &skbCtl->modeBarNum[0];
	fepAtokInputMode_m* fepMode = &skbCtl->inputMode[0];

	// �L�[�{�[�h�̏��ԂƁA�g�p�t�F�v���[�h���A�L�[�{�[�h�Z�b�g��񂩂�R�s�[
	for(n=0;n<SKBTYPE_MAX;n++){
		next[n]    = kbSet->Keyboard[n].next;
		fepMode[n] = kbSet->Keyboard[n].fepMode;
		modeBar[n] = -1;
	}

	// ���[�h�o�[�ݒ�
	{
		int c = 0;
		#ifndef REGION_J_OTHER_ONLY
		static const u_int MODEBAR_SKBTYPE_GRAPH_J[SKBTYPE_MAX]={	//SKBTYPE_ �� ���[�h�o�[�O���b�t�B�b�N
			MODEBAR_CHAR_HIRAGANA_J,  // ��  �Ђ炪�� 
			MODEBAR_CHAR_KATAKANA_J,  // �A  �J�^�J�i 
			MODEBAR_CHAR_ALPHABET_J,  // �`  �A���t�@�x�b�g
			MODEBAR_CHAR_ALPHABET_J,  // �`  �A���t�@�x�b�g
			MODEBAR_CHAR_NUM,         // 123 ����
			MODEBAR_CHAR_SIGN_J,      // ��  �i�h�r
		};
		#endif
		#ifndef REGION_J_ONLY
		static const u_int MODEBAR_SKBTYPE_GRAPH_AE[SKBTYPE_MAX]={	//SKBTYPE_ ��
			-1,
			-1,
			MODEBAR_CHAR_UMLAUT_F,    // �`  �����L�����A���t�@�x�b�g 
			MODEBAR_CHAR_ALPHABET_F,  // �`  �A���t�@�x�b�g  
			MODEBAR_CHAR_NUM,         // 123 ����            
			MODEBAR_CHAR_SIGN_F,      // ��  �L��            
		};
		#endif
		const u_int *modeBarGRAPH;

		#ifdef REGION_J_ONLY
		modeBarGRAPH = MODEBAR_SKBTYPE_GRAPH_J;  // ���{�p
		#else
			#ifdef REGION_J_OTHER_ONLY
		modeBarGRAPH = MODEBAR_SKBTYPE_GRAPH_AE; // �C�O�p
			#else  //���\�[�X����������ꍇ�́A�����ɏ]���B
			if( region == REGION_J )  modeBarGRAPH = MODEBAR_SKBTYPE_GRAPH_J;
			else                      modeBarGRAPH = MODEBAR_SKBTYPE_GRAPH_AE;
			#endif
		#endif

		// �L�[�{�[�h�������A�P�����āA���[�h�o�[�����쐬�B
		n = skbCtl->uiNowKeyType = kbSet->startKeyboardType;
		while( n < SKBTYPE_MAX ){
			if( n >= SKBTYPE_MAX ){
				Printf("KEYBOARD_SET LinkError\n");
				break;
			}
			if( alphabetKeyType == -1 ){	//�L�[�{�[�h�Z�b�g���A�ǂ���^�C�v���g�p���Ă��邩����B
				if( n == SKBTYPE_QWERTY )	alphabetKeyType=1;	// ����l�́Asystem.ini ������
				else if( n == SKBTYPE_ABC )	alphabetKeyType=0;	// ����l�́Asystem.ini ������
			}

			modeBar[n] = c;
			kbSet->Bar.ModeNum[n] = c;
			kbSet->Bar.SetMode[c] = modeBarGRAPH[n];

			Printf("setModeBar %d\n",modeBar[n]);
			c++;
			n = kbSet->Keyboard[n].next;
			if( n == kbSet->startKeyboardType )	break;
			if( c >= SKBTYPE_MAX )	printf("kbset error\n");
		}
		kbSet->Bar.setCount = c;
		if( modeBar[SKBTYPE_QWERTY] == -1 )	modeBar[SKBTYPE_QWERTY] = modeBar[SKBTYPE_ABC   ];
		if( modeBar[SKBTYPE_ABC   ] == -1 )	modeBar[SKBTYPE_ABC   ] = modeBar[SKBTYPE_QWERTY];

		// ���[�h�o�[������
		Printf("CreateBar %d\n",c);
		SoftKB_ClearModebarType();
		SoftKB_SetModebarType( &kbSet->Bar.SetMode[0], kbSet->Bar.setCount );
	}

	// �e�������[�h������ 
	Fepbar_ClearModeType();
	Fepbar_CreateModeType( kbSet->Fep.Mode, kbSet->Fep.modeCount );

	return alphabetKeyType;
}

// �L�[�{�[�h�Z�b�g�̉�ʃ��C�A�E�g���W�ʒu���擾 (�L�[�{�[�h num �Ԗ�)
void KeyBoardSet_getKeybordLayout( KEYBOARD_SET* kbSet, int num, iPoint_t* point )
{
	*point = kbSet->Keyboard[num].Position;
}
// �L�[�{�[�h�Z�b�g�̉�ʃ��C�A�E�g���W�ʒu���擾 �i�e�L�X�g�t�B�[���h�j
void KeyBoardSet_getTextBoxLayout( KEYBOARD_SET* kbSet, iRect_t* rect )
{
	*rect = kbSet->Text.Rect;
}
// �L�[�{�[�h�Z�b�g�̉�ʃ��C�A�E�g���W�ʒu���擾 �i�e�����o�[�j
void KeyBoardSet_getFepBarLayout( KEYBOARD_SET* kbSet, iPoint_t* point )
{
	*point = kbSet->Fep.Position;
}
// �L�[�{�[�h�Z�b�g�̉�ʃ��C�A�E�g���W�ʒu���擾 �i���[�h�o�[�j
void KeyBoardSet_getModeBarLayout( KEYBOARD_SET* kbSet, iPoint_t* point )
{
	*point = kbSet->Bar.Position;
}

// �L�[�{�[�h������Ԃ̐ݒ�
void KeyBoardSet_InitKeyboad( KEYBOARD_SET* kbSet, int n, tSkb_Ctl* skbCtl )
{
	skey_t* skey = &skbCtl->KBD[n];
	u_int ucShift;
	u_int *ctrl;

	// �J�[�\���ʒu�̏������B
	SoftKB_SetStartCurPos(skey, skey->uiKeyTopNo);

	// �L�[���b�N�̐ݒ�
	ucShift = kbSet->Keyboard[n].ucShift;
	          // 1: SHIFT    ON
	          // 2: CAPSLOCK ON
	          // 4: ZENKAKU  ON/ALTGR ON

	//�S�p/���p���b�N�f�t�H���g�ݒ�
	if( skey->uiKeyTopNo == KEYTOP_NUMJP    // �S�p/���p�̂���L�[�{�[�h����
	 || skey->uiKeyTopNo == KEYTOP_PC_JAPAN 
	 || skey->uiKeyTopNo == KEYTOP_ABC      
	 || skey->uiKeyTopNo == KEYTOP_KATAKANA 
	 ){ 
		if( ucShift & LAYOUT_ZENKAKU ){
			skey->ucShift |= LAYOUT_ZENKAKU;                  // �S�p��ݒ�
			onStateCtrlCode( skey, SKBCTRL_KANJI, KEY_PUSH);  // ���p/�S�p/���� �S�p
		}
		else{
			skey->ucShift &= (~LAYOUT_ZENKAKU);               // ���p��ݒ�
			offStateCtrlCode( skey, SKBCTRL_KANJI, KEY_PUSH); // ���p/�S�p/���� ���p
		}
	}

	//�b���������b�N�f�t�H���g�ݒ�
	if( skey->uiKeyTopNo != KEYTOP_HIRAGANA   //�L���v�X�̖����L�[�{�[�h�����O 
	 && skey->uiKeyTopNo != KEYTOP_KATAKANA   
	 && skey->uiKeyTopNo != KEYTOP_IDIOMCHAR  
	 && skey->uiKeyTopNo != KEYTOP_NUMJP      
	 && skey->uiKeyTopNo != KEYTOP_NUMF       
	 && skey->uiKeyTopNo != KEYTOP_SIGNF      
	 && skey->uiKeyTopNo != KEYTOP_SIGN       
	 ){ 
		if( ucShift & LAYOUT_CAPS ){
			skey->ucShift |= LAYOUT_CAPS;                   
			onStateCtrlCode( skey, SKBCTRL_CAPS , KEY_PUSH);  // �b�`�o�r���b�N
		}
		else{
			skey->ucShift &= (~LAYOUT_CAPS);                
			offStateCtrlCode( skey, SKBCTRL_CAPS , KEY_PUSH);  // �b�`�o�r���b�NOFF
		}
	}

	// �S�ẴR���g���[���L�[���A���͉ɖ߂��B
	offStateCtrlCode( skey, SKBCTRL_CAPS     , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_SHIFT_L  , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_SHIFT_R  , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_BACKSPACE, KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_ENTER    , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_UP       , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_DOWN     , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_LEFT     , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_RIGHT    , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_HELP     , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_KANJI    , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_TAB      , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_ABCQWERTY, KEY_DISABLE );

	// ���͋֎~�L�[��ݒ肷��
	if( (ctrl = kbSet->Keyboard[n].disableCtrl) ){
		for(;;ctrl++){
			if( *ctrl == 0 )	break;
			onStateCtrlCode( skey, *ctrl, KEY_DISABLE );
		}
	}
}


//============================================================================
// ucs4 ������֐�
//============================================================================
static int ucs4cmp( u_int* ucs4str0, u_int* ucs4str1, int count )
{
	int n;

	for(n=0;n<count;n++){
		if( ucs4str0[n] != ucs4str1[n] ){
			return n+1;
		}
		if( ucs4str0[n] == 0 )	break;
	}
	return 0;
}
static int ucs4cpy( u_int* ucs4dst, u_int* ucs4src, int count )
{
	int n;

	for(n=0;n<count;n++){
		ucs4dst[n] = ucs4src[n];
		if( ucs4src[n] == 0 ){
			for(;n<count;n++)	ucs4dst[n]=0;
			return 0;
		}
	}
	n--;
	ucs4dst[n] = 0;

	return -n;
}
//--------------------------------------------------------------------------------------
// SKB �O���C���^�[�t�F�[�X
//--------------------------------------------------------------------------------------
int ThreadID_SKBWork=-1;            //�d�������p�̃X���b�h�̂h�c�i�[�ϐ�
static char stack[16*1024] __attribute__((aligned(128))); //���̃X�^�b�N

SKB_ARG SkbArg; //�������ۑ��B

// SKB�֐��S�������B
// �\�t�g�E�F�A�L�[�{�[�h�g�p�O�Ɋ��ݒ���s��
int Init_SKBConfig( SKB_ARG* skbArg, char *fsName, short keyType, short repWait, short repSpeed )
{
	SkbArg = *skbArg;
	memset(&g_import, 0x0, sizeof(gImport_t));

	g_import.iRegion = SkbArg.region;  // �n��w��
	#ifdef REGION_J_ONLY
	g_import.iRegion = REGION_J;       // ���{��
	#endif
	#ifdef REGION_J_OTHER_ONLY
	if( g_import.iRegion == REGION_J ){
		g_import.iRegion = REGION_A;   // �t�r�� english only
		//g_import.iRegion = REGION_E; // �d�t�� no Support
	}
	#endif

	g_import.malign = SkbArg.memAlign;              // Sysenv_ReadSystemIni() ���Ag_import���A
	g_import.mfree  = SkbArg.memFree;               // ���ڎQ�Ƃ��Ă���̂ŁA�ŏ��ɐݒ�

	g_import.usSystemIni[ENV_KEYBD_TYPE   ]=keyType;  // �f�t�H���g�̃L�[�{�[�h�ɁB
	g_import.usSystemIni[ENV_KEYBD_REPEATW]=repWait;  // �f�t�H���g�̃E�F�C�g��
	g_import.usSystemIni[ENV_KEYBD_REPEATS]=repSpeed; // �f�t�H���g�̃X�s�[�h��
	g_import.usSystemIni[ENV_SOFTKB_ONOFF ]=1;        // �f�t�H���g����ʃL�[�{�[�h�L��
	g_import.usSystemIni[ENV_SOFTKB_QWERTY]=0;        // �f�t�H���g���A���t�@�x�b�g�͂`�a�b��

	return Sysenv_ReadSystemIni(g_import.usSystemIni, fsName); // system.ini�ǂ�
}

// �\�t�g�E�F�A�L�[�{�[�h�֌W�̏�����
// �Ԃ�l �d���R���g���[�����ɁA�҂ׂ��Z�}�t�H�h�c��Ԃ�int(*)(void)�֐��|�C���^�B
//
SKB_GETSEMA_FUNC SKB_Init()
{
	Printf("SKB_Init [%s]\n",!getFrameField() ? "FieldMode" : "Etc Mode");
	#if CDBOOT
	Printf("CDBOOT!!\n");
	#endif 

	ThreadID_SKBWork=-1 ;	//�d�������p�̃X���b�h�̂h�c�i�[�ϐ�������
	//--------------------------------------------------
	// ����\���̏����� & ���̑�
	//--------------------------------------------------
	memset(&g_Sample, 0x0, sizeof(g_Sample));
	{

		// �t�H���g�F��` �ݒ�
		initColor();
		/*	�ȉ��֐��ŁA�g�p�����B
			setCharsNormalColor();  // �t�H���g�F           �ʏ�
			setSubCandListColor();  // �ϊ���⑗�肪�Ȃ̐F 
			setUnderlineColor();    // �A���_�[���C���F     
			setNormalBGColor();     // �t�H���g�w�i�F�ݒ�   
			setChangingBGColor();   // �t�H���g�w�i�F�i�����ϊ����j
		*/

		//--------------------------------------------------
		// SoftWareKeyBoad�`��̈�m��
		//--------------------------------------------------
		sceVif1PkInit(&g_pkVif1[0], (u_long128*)g_pkVif1Buff[0]);		// sceVif1Packet�\���̂�������
		sceVif1PkInit(&g_pkVif1[1], (u_long128*)g_pkVif1Buff[1]);		// sceVif1Packet�\���̂�������
	}
	//--------------------------------------------------
	// ���͋֎~�����̒�`�\���̂̏�����
	//--------------------------------------------------
	{
		int n;
		
		inhibitChar.pCtrlKey  = NULL;
		inhibitChar.uiCKeyNum = 0;
		inhibitChar.pChar     = inhibitUCS4;
		inhibitChar.uiCharNum = 0;
		
		for(n=0;n<SKBTYPE_MAX;n++){
			g_Sample.KBD[n].pInhibitChar=&inhibitChar;
		}
		#if 1//SKB_EX Inhibit
		// ���͋֎~�����ł��邪�A��ʃL�[�{�[�h�̃{�^���͉����镶�����X�g�i�[�o�b�t�@�B
		inhibitSubChar.uiCKeyNum = 0;
		inhibitSubChar.pCtrlKey  = NULL;
		inhibitSubChar.uiCharNum = 0;
		inhibitSubChar.pChar     = inhibitSubUCS4;
		#endif
	}

	// �V�X�e���ݒ���擾
	g_Sample.Env.inputDevice     = g_import.usSystemIni[ENV_SOFTKB_ONOFF];   // screen keyboard or usbkeyborad
	g_Sample.Env.alphabetKeyType = g_import.usSystemIni[ENV_SOFTKB_QWERTY];  // abc or pc key

	// ����Y�l��ݒ�
	Font_SetTvFontRatio(VRESOLUTION_ADJUST_F(TV_RATIO));

	//--------------------------------------------------
	// g_Import ���ʕ��ݒ�
	//--------------------------------------------------
	{
		if(g_import.iRegion == REGION_J){
			#if ATOK_USE == 0
			g_import.ucUseAtok  = 0;  // ATOK�g��Ȃ�
			#else
			g_import.ucUseAtok  = 1;  // ATOK�g��
			#endif
		}
		else{
			g_import.ucUseAtok  = 0;  // ATOK�g��Ȃ�
		}

		g_import.uiScreenW = SCREEN_WIDTH;
		g_import.uiScreenH = SCREEN_VHEIGHT;

		g_import.dprintf       = dprintf;
		g_import.getFrameField = getFrameField;
		g_import.getNtscPal    = getNtscPal;
		g_import.usLang        = Sysenv_GetLang();
		if(g_import.iRegion == REGION_J){
			if( g_import.usLang > 1  )	g_import.usLang = 1;
		}
		else{
			if( g_import.usLang == 0 )	g_import.usLang = 1;
		}

		g_import.se      = se;

		{
			u_int tbp,cbp;

			cbp = SkbArg.Gs_workAddress256;
			tbp = cbp + 4;

			Tim2_Init( SCREEN_WIDTH, SCREEN_HEIGHT, OFFX, OFFY, tbp, cbp,
			           g_import.malign,
			           g_import.mfree );

			g_import.readTex   = Tim2_ReadTim2;
			g_import.deleteTex = Tim2_DeleteTim2;
			g_import.loadTex   = Tim2_SendTim2;
			g_import.drawTex   = Tim2_DrawSprite;
			g_import.drawPane  = SetRectFill;
			g_import.drawLine  = SetLine;
		}

		g_import.thinkYHn  = thinkYHn;
		g_import.thinkYH   = thinkYH;
		g_import.thinkXW   = thinkXW;
		g_import.thinkYH16 = thinkYH16;
		g_import.thinkXW16 = thinkXW16;

		g_import.drawFocus = somewhereFocus;
		g_import.isConv    = Fep_GetNowConv;
	}

	//--------------------------------------------------
	// g_Import �e����������
	//--------------------------------------------------
	{
		// �e�ݒ�R�s�[
		int atokPrio;
		fepImport_t *pFep = &g_import.fep;

		pFep->puiUCS4      = g_Sample.uiChars;  // ������ւ̃|�C���^
		pFep->uiMaxTextLen = MAX_CHAR_COUNT;    // ���E������

		pFep->atokON  = atokON;
		pFep->atokOFF = atokOFF;
		pFep->changeMode = changeMode;
		pFep->changeText = changeText;
		pFep->powerLock   = powerLock;
		pFep->powerUnlock = powerUnlock;
		pFep->ok          = exitOK;
		pFep->cancel      = cancel;
		pFep->getNumlockState = getNumlockState;

		pFep->showHideSoftKB = showHideSoftKB;
		pFep->closeSoftKB    = closeSoftKB;
		pFep->openSoftKB     = openSoftKB;

		pFep->getStrWidthSJIS = getStringWidthSJIS;

		{
			struct ThreadParam t_info;

			ReferThreadStatus( GetThreadId(), &t_info );
			if( t_info.currentPriority < ATOK_THREAD_PRIORITY-2 ){
				atokPrio = ATOK_THREAD_PRIORITY;
			}
			else{
				atokPrio = t_info.currentPriority;
				if(atokPrio<127-2)      atokPrio+=2;
				else if(atokPrio<127-1) atokPrio+=1;
			}
			Printf("AtokPrio=%d\n",atokPrio);
		}
		Fep_Init(&g_import   , g_import.ucUseAtok, atokPrio, SkbArg.AtokPath.sysDic, SkbArg.AtokPath.usrDic, SkbArg.AtokPath.system );
	}
	//---------------------------------------------------------------------------
	// FEPBAR�������iUSB �L�[�{�[�h�g�p���ɏo��A�e�������̓��[�h�\�����s���o�[�j
	//---------------------------------------------------------------------------
	Fepbar_Init(&g_import);

	//--------------------------------------------------
	// g_Import �\�t�g�E�F�A�L�[�{�[�h���ݒ�
	//--------------------------------------------------
	{
		int j;
		skbImport_t *pSkb = &g_import.skb;

		pSkb->uiListFontW = FONT_W;
		pSkb->uiListFontH = FONT_H;
		pSkb->ucShowSKbd = g_import.usSystemIni[ENV_SOFTKB_ONOFF];
		pSkb->changeInputMode = changeInputMode;
		pSkb->abcToQwerty = exchangeABCtoQWERTY;
		pSkb->getNowInputMode = getNowInputMode;

		pSkb->getAtokInput= Fep_GetNowAtokInput;
		pSkb->getAtokMode = Fep_Changing;
		pSkb->kakuteiPart = Fep_SetKakuteiPart;
		pSkb->kakuteiAll  = Fep_SetKakuteiAll;

		pSkb->setChars = setChars;
		pSkb->exitOK   = skb_exitOK;
		pSkb->showHelp = showHelp;
		pSkb->keytopChange = keytopChange;
		pSkb->opening = opening;
		pSkb->closing = closing;

		pSkb->setListFontEnv     = setSkbListFontEnv;
		pSkb->setListDrawString  = setSkbListDrawString;
		pSkb->setListDrawStringF = setSkbListDrawStringFocus;
		pSkb->getStringWidth  = getStringWidthUtf8;


		SoftKB_Init(&g_import);


		//--------------------------------------------------
		// �\�t�g�E�F�A�L�[�{�[�h �쐬
		//--------------------------------------------------
		{
			int keyTop[SKBTYPE_MAX];

			#ifndef REGION_J_OTHER_ONLY
			if( g_import.iRegion == REGION_J ){
				keyTop[SKBTYPE_HIRA  ] = KEYTOP_HIRAGANA;
				keyTop[SKBTYPE_KANA  ] = KEYTOP_KATAKANA;
				keyTop[SKBTYPE_ABC   ] = KEYTOP_ABC;
				switch( g_import.usSystemIni[ENV_KEYBD_TYPE   ]){
				case USBKB_ARRANGEMENT_101       : keyTop[SKBTYPE_QWERTY] = KEYTOP_PC_US;    break; // US
				default:
				case USBKB_ARRANGEMENT_106       : keyTop[SKBTYPE_QWERTY] = KEYTOP_PC_JAPAN; break;
				case USBKB_ARRANGEMENT_106_KANA  : keyTop[SKBTYPE_QWERTY] = KEYTOP_PC_JAPAN; break;
				}
				keyTop[SKBTYPE_NUM   ] = KEYTOP_NUMJP;
				keyTop[SKBTYPE_SIGN  ] = KEYTOP_SIGN;
			}
			else
			#endif
			{
			#ifndef REGION_J_ONLY
				keyTop[SKBTYPE_HIRA  ] = -1;
				keyTop[SKBTYPE_KANA  ] = -1;
				keyTop[SKBTYPE_ABC   ] = KEYTOP_IDIOMCHAR;
				switch( g_import.usSystemIni[ENV_KEYBD_TYPE   ] ){
				default:
				case USBKB_ARRANGEMENT_101       : keyTop[SKBTYPE_QWERTY] = KEYTOP_PC_US;         break; // US
				}
				keyTop[SKBTYPE_NUM   ] = KEYTOP_NUMF;
				keyTop[SKBTYPE_SIGN  ] = KEYTOP_SIGNF;
			#endif 
			}

			g_Sample.Env.alphabetKeyType = g_import.usSystemIni[ENV_SOFTKB_QWERTY];

			for(j=0; j<SKBTYPE_MAX; j++)
			{
				iPoint_t skbPoint = {0, 0};
				void *listItem;

				if( keyTop[j] < 0 )	continue;
				if( keyTop[j] == KEYTOP_SIGN )	listItem = (void*)g_ListItems;
				else                          	listItem = NULL;
				
				if( SoftKB_Create( &g_Sample.KBD[j], &skbPoint, keyTop[j], &inhibitChar, listItem ) ){
					printf("ERROR %s, %d: SKB_Init - SoftKB_Create", __FILE__, __LINE__);
					return 0;
				}
				// �L�[�{�[�h�\���̔��������[�g�̐ݒ�B
				g_Sample.KBD[j].ucAlpha[0] = 0x80;
				g_Sample.KBD[j].ucAlpha[1] = 0x80;
				g_Sample.KBD[j].ucAlpha[2] = 0x80;
				g_Sample.KBD[j].ucAlpha[3] = 0x80;
			}
		}
	}
	//-----------------------------------------------------------
	// TEXTFIELD �������i�e�L�X�g�{�b�N�X�̍쐬�j
	//-----------------------------------------------------------
	{
		// �e�ݒ�R�s�[
		iRect_t rc = {30, 128, 500, 25};
		txtFImport_t *pTxt = &g_import.txt;

		//�e�L�X�g�t�B�[���h�̃t�H���g�T�C�Y�ɂ��Ă���A�t�H���g�̍������擾����B
		Font_SetRatio(APP_FONT_DEF);	

		pTxt->uiCharHeight = Font_GetHeight();

		pTxt->getStrings     = getStrings;
		pTxt->drawStrings    = drawStrings;
		pTxt->drawStringBeforeChange = drawStringBeforeChange;
		pTxt->drawStringChanging = drawStringChanging;
		pTxt->getStringWidth = getStringWidth;
		pTxt->getCaretPos    = Fep_GetCaretPos;
		pTxt->getAtokState   = Fep_AtokGetInputState;
		#if 1//SKB_EX Insert  
		pTxt->isInsertMode  = isInsertMode; //�㏑�����[�h���ɃL�����b�g��ω������邽�߂ɒǉ��B
		#endif
		pTxt->getToCaretPos = getToCaretPos;

		TextF_Init(&g_import);
		TextF_Create(&textField,  &rc);
		TextF_ChangeAlpha(&textField, TF_ALPHA_EDITORBG  , 0x80);
		TextF_ChangeAlpha(&textField, TF_ALPHA_CARET     , 0x80);
		TextF_ChangeAlpha(&textField, TF_ALPHA_CANDLISTBG, 0x80);
		textField.uiCaretColor = 0x80ff7070;     // �L�����b�g�F�ݒ�
	}

	// �d���n�e�e���ɁA
	// �`�s�n�j�����t�@�C���̘_���I����������邽��
	// �`�s�n�j�ɂ��g�c�c�A�N�Z�X���́A�d���n�e�e��҂��Ă��炤�K�v������B
	return Fep_GetAtokSemaID; //�Ăяo�����ɓn���B
}

// �N���[�Y���ɕҏW���ʕ�����̎󂯎��A�r�d�ȂǁA���b�Z�[�W���󂯎��֐���o�^����֐��B
void SKB_SetCallback( int(*func)(int,void*) )
{
	g_Sample.Message = func;
}


// 
// �L�[�{�[�h�́Asystem.ini  ���̎擾
// 
void SKB_GetKbdConfig( int* kbType, int *repeatWait, int *repeatSpeed,int *region  )
{
	*kbType      = g_import.usSystemIni[ENV_KEYBD_TYPE   ];
	*repeatWait  = g_import.usSystemIni[ENV_KEYBD_REPEATW];
	*repeatSpeed = g_import.usSystemIni[ENV_KEYBD_REPEATS];
}


//
// �Ԃ�l �\�t�g�L�[�{�[�h������ 0:�񓮍� 1:���쒆
//
int SKB_GetStatus()
{
	return g_Sample.iControlFlag;
}


//
// �Ԃ�l �S�p���͂�����  0:�S�p���� 1:���p����
//
int SKB_GetZenkakuMode()
{
	int mode = SoftKB_GetInputMode(&g_Sample.KBD[g_Sample.uiNowKeyType]);		// �S�p/���p���[�h���擾
	int ret=-1;

	switch(mode)
	{
	case SKBMODE_HIRAGANA    :
	case SKBMODE_ZEN_KATAKANA:
	case SKBMODE_ZEN_ALPHABET: ret = 1; break;
	case SKBMODE_HAN_KATAKANA:
	case SKBMODE_HAN_ALPHABET: ret = 0; break;
	}
	return ret;
}

// �����֐��@�\�t�g�E�F�A�L�[�{�[�h  �I�[�v��
void SKB_open();
//
// �p�b�h���͂̎󂯎��ƁA���쐧��
//
void SKB_Control( u_long ulPad )
{
	int inputKbdFlag=0;

	if( g_Sample.iControlFlag && g_Sample.iOpenFlag==0 ){
		// SKB_Open() �ŁA���͋֎~���쐬�҂��ŁA�J����Ă��Ȃ�������B
		if( ThreadID_SKBWork==-1 ){ // ������A�����łn�o�d�m
			SKB_open();             // �����ŁAiOpenFlag���P�ɂȂ�B
		}
	}
	else if( g_Sample.iOpenFlag <= 0 )	return;

	if( KbStat < 0 ) inputKbdFlag = 0;
	else             inputKbdFlag = KbStat;

	if( g_import.iRegion != REGION_J ) //���{�����łȂ���΁A���~������ꂩ����B
	{
		u_long pad_Rright = (((u_long)SCE_PADRright<<48)|((u_long)SCE_PADRright<<32)|((u_long)SCE_PADRright<<16)|((u_long)SCE_PADRright));
		u_long pad_Rdown  = (((u_long)SCE_PADRdown <<48)|((u_long)SCE_PADRdown <<32)|((u_long)SCE_PADRdown <<16)|((u_long)SCE_PADRdown ));
		                     // release                  triger                      press                        repeat
		u_long pad_Mask   = ~(pad_Rright|pad_Rdown);

		#if SCE_PADRright > SCE_PADRdown
		pad_Rright = (ulPad&pad_Rright)/(SCE_PADRright/SCE_PADRdown);
		pad_Rdown  = (ulPad&pad_Rdown )*(SCE_PADRright/SCE_PADRdown);
		#else
		pad_Rright = (ulPad&pad_Rright)*(SCE_PADRdown/SCE_PADRright);
		pad_Rdown  = (ulPad&pad_Rdown )/(SCE_PADRdown/SCE_PADRright);
		#endif
		ulPad = (( ulPad & pad_Mask) | pad_Rright | pad_Rdown );
	}

	if(! SoftKB_CheckShown() )      // �L�[�g�b�v���\������Ă��邩�H  USB�L�[�{�[�h���͎��́A�\������Ă��Ȃ��B
	{
		// �e�����o�[
		Fepbar_SetPadData(ulPad);   // �L�[�{�[�h�̑���ɁA�e�������̓��[�h�\���o�[������
		if( g_Sample.iUsbKeyFlag==0 ){
			g_Sample.iUsbKeyFlag = 1;
			if(g_Sample.Message)	g_Sample.Message( SKBMES_USBKEY_ON,(void*)0 );
			//USB�L�[�{�[�h�ɂȂ�ƁA�f�t�H���g�łe�������́A�Ђ炪�Ȃɐݒ肳���̂ŁA�Đݒ�B
			changeInputMode_usbkb();
		}
	}
	else
	{
		if( g_Sample.iUsbKeyFlag ){
			g_Sample.iUsbKeyFlag = 0;
			if(g_Sample.Message)	g_Sample.Message( SKBMES_USBKEY_OFF,(void*)0 );
		}
		// �L�[�{�[�h
		SoftKB_SetPadData(&g_Sample.KBD[g_Sample.uiNowKeyType], ulPad); // �p�b�h�̃f�[�^�ݒ�
	}

	// USB �L�[�{�[�h���͂ɂ�鐧��
	if(!g_Sample.iCoverView)  // �e�L�X�g�{�b�N�X���\���}�X�N����Ă��Ȃ����H  Help��ʕ\�����́A�}�X�N����Ă���B 
	{
		//�e�L�X�g�{�b�N�X�L����
		if( inputKbdFlag )    // �t�r�a�L�[�{�[�h�ɂ����͂��L��
		{
			Fep_PostKeys( &KbData );	// �L�[�{�[�h���͕�����POST
		}
	}
}

//
// USB�L�[�{�[�h���͎󂯎��
//
void SKB_SetUsbKeyborad( int stat, USBKBDATA_t *usbKb, int numLock )
{
	KbStat = stat;
	if(stat>0){
		KbData = *usbKb;
		KbNumLock = numLock;
	}
}

//
// �����֐��@�\�t�g�E�F�A�L�[�{�[�h  �I�[�v��
//
KEYBOARD_SET  KBSet;
u_int         KBText[256];
void SKB_open()
{
	KEYBOARD_SET* kbSet = &KBSet;
	u_int*         text = KBText;

	//-------- �\�t�g�E�F�A�L�[�{�[�h OPEN ---
	openSoftKB(1);
	{
		skey_t *pKey;
		int n;
		//�L�[�{�[�h�̐ݒ菉���� ��openSoftKB()�ŁA�f�t�H���g���ݒ肳���̂ŁAopenSoftKB()����ɍs��
		for(n=0;n<SKBTYPE_MAX;n++){
			KeyBoardSet_InitKeyboad( kbSet, n , &g_Sample );
		}
		// �L�[���b�N�̏�Ԃ�ύX�ɂƂ��Ȃ��A���͋֎~��Ԃ��Đݒ�B
		pKey = &g_Sample.KBD[g_Sample.uiNowKeyType];
		setEnableChars( pKey );                     // �S�ẴL�[��L����
		setInhibitChars( pKey, pKey->pInhibitChar );// ���͋֎~�L�[���Đݒ�
	}
	//�����e�L�X�g�̐ݒ�
	if( text ){
		// �����ݒ蕶������A�e�d�o�ɑ��M
		Fep_SetText( text );
	}

	// �f�t�H���g���̓f�o�C�X�̐ݒ�i�p�b�h�܂��́A�t�r�a�L�[�{�[�h�j  system.ini ������
	g_import.skb.ucShowSKbd = g_Sample.Env.inputDevice;
	// �t�r�a�L�[�{�[�h�ŃX�^�[�g��������A�t�r�a�L�[�{�[�h���͎��̂e�������[�h�Őݒ�B
	if( g_Sample.Env.inputDevice==0 ){
		changeInputMode_usbkb();
		g_Sample.Message( SKBMES_USBKEY_ON,(void*)0 );	//�g�p�A�v�����ɕ�
	}
	// fep On/Off ���̂e�������[�h�L���ϐ����A�������B
	g_Sample.fepModeOnState = -1;

	//�I�[�v����������p�ɁA�t���O�𗧂ĂĂ����B
	g_Sample.iOpenFlag = 1;
}


//
// �\�t�g�E�F�A�L�[�{�[�h  �I�[�v��
//
void SKB_Open( KEYBOARD_SET* kbSet, int editType, u_int* text, int inputMax )
{
	int n;
	int atype;

	Printf("SkbOpen \n" );
	g_Sample.iControlFlag = 1;            // SoftWareKeyBoard�\���J�n

	//--------  �e�L�X�g�ҏW�o�b�t�@�̏����� ------------------------------------------------
	g_import.fep.uiMaxTextLen = MAX_CHAR_COUNT; //�f�t�H���g�ő啶�����w��B�o�b�t�@�N���A�ɂ��g�p�����B
	g_Sample.uiChars[0] = 0;	// �e�L�X�g�o�b�t�@������
	g_Sample.uiCharNum  = 0;
	{
		static int clearCode[2] = { 0x00000015, 0 };
		Fep_SetText( clearCode );	// �����񑗏o(NULL�����܂ő��M�����)
	}
	//--------  �ő���͕������̐ݒ� =-------------------------------------------------------
	if( inputMax > 0 && inputMax <= MAX_CHAR_COUNT ){
		g_import.fep.uiMaxTextLen = inputMax;
	}
	//--------- �ҏW���[�h�ݒ� (�ʏ� / -1:PASSWORD)------------------------------------------
	if( editType == -1 ) TextF_SetDispMode( &textField, 1 );
	else                 TextF_SetDispMode( &textField, 0 );

	//======== �L�[�{�[�h�Z�b�g�̐ݒ�  ============================================================================
	atype = KeyBoardSet_Setup( kbSet, &g_Sample, g_import.iRegion );
	//  ���� ABC<->QWERTY �ݒ�����킹��
	if( atype >= 0 && atype != g_Sample.Env.alphabetKeyType ){
		swap_ABC_QWERTY(-1);	//
	}
	//--  ��ʏ�̃��C�A�E�g��ݒ�  ------
	{
		iRect_t  rect;
		iPoint_t point;

		for(n=0;n<SKBTYPE_MAX;n++){
			if( g_Sample.modeBarNum[n] < 0 )	continue;
			KeyBoardSet_getKeybordLayout( kbSet, n, &point );	SoftKB_Move( &g_Sample.KBD[n], point.x, point.y );
			g_Sample.skbPos[n] = point;
		}
		KeyBoardSet_getTextBoxLayout( kbSet, &rect );    TextF_Move( &textField,  &rect );
		KeyBoardSet_getFepBarLayout( kbSet, &point );    Fepbar_Move( point.x, point.y );
		KeyBoardSet_getModeBarLayout( kbSet, &point );   SoftKB_MoveModebar( point.x, point.y );
	}

	KBSet=*kbSet;
	for(n=0;n<256;n++){
		KBText[n]=text[n];
		if( text[n]==0 )	break;
	}

	if( ThreadID_SKBWork==-1 ){	//���͋֎~��񐶐����I����Ă����瑦�n�o�d�m
		SKB_open();
	}
}


//
// �\�t�g�E�F�A�L�[�{�[�h�̕`��p�P�b�g�쐬
//   ������ clipOffX clipOffY �́A�쐬�p�P�b�g�̕`��ʒu�ł͂Ȃ��A
//   �`��I�t�Z�b�g(GS�ɂ��`���̃I�t�Z�b�g)��ύX�����ꍇ�ɁA
//   �V�U�����O��`�ʒu�̕␳�p�I�t�Z�b�g�l��(�V�U�����O��GS�̕`��I�t�Z�b�g�̉e�����󂯂Ȃ�����)
//   �`��I�t�Z�b�g�ɉe�����󂯂����Ȃ��`��p�P�b�g(�S��ʕ\���̃w���v��ʂȂǂ�)�ɂ�
//   �t�Z�p�I�t�Z�b�g�l�Ƃ��āA�g�p����Ă��܂��B
void SKB_makePacket(sceVif1Packet *pkVif1, int clipOffX, int clipOffY )
{
	if( g_Sample.iOpenFlag == 0 )	return;

	if( g_Sample.iCoverView ){	// textbox ���}�X�N����Ă�����A�w���v�\����
		SetClipOffset( 0, 0 );	// �w���v�\�����́A�V�U�����O�͈͂����炳�Ȃ��B
		SetClipArea( pkVif1, NULL );
		HelpOffset( -clipOffX, -clipOffY );
	}
	else{
		SetClipOffset( clipOffX, clipOffY ); 
	}

	if(! SoftKB_CheckShown() ) // ��ʂɃL�[�{�[�h���o���Ă��Ȃ��Ȃ�
	{	// USB�L�[�{�[�h���g�p��
		Fepbar_Run(pkVif1);  // USB�L�[�{�[�h���ɁA�\�������e�������̓��[�h�o�[�̏���
	}
	else
	{	// ��ʂ̃L�[�{�[�h�g�p��
		SoftKB_Run(pkVif1, &g_Sample.KBD[g_Sample.uiNowKeyType]); // ��ʃL�[�{�[�h�̏���
	}

	if(!g_Sample.iCoverView) // �e�L�X�g�{�b�N�X�̕`��}�X�N����Ă��Ȃ����
	{
		//�ҏW������s���B
		Fep_Run();

		TextF_Run(pkVif1, &textField); // �e�L�X�g�{�b�N�X����
	}
}

//
// �\�t�g�E�F�A�L�[�{�[�h�̕`��p�P�b�g�쐬
//
void* SKB_MakeVif1CallPacket( int clipOffX, int clipOffY )
{
	sceVif1Packet *pkVif1 = &g_pkVif1[g_pkVif1_ID];

	g_pkVif1_ID^=1;    // �_�u���o�b�t�@����ւ�

	sceVif1PkReset(pkVif1);     // Reset

	// �p�P�b�g���� 
	SKB_makePacket(pkVif1, clipOffX, clipOffY );

	sceVif1PkRet(pkVif1, 0);    // ��sceVif1PkEnd�ł͂Ȃ�
	sceVif1PkTerminate(pkVif1); // 

	return((void *)pkVif1->pBase); //�쐬�����p�P�b�g�̃A�h���X��Ԃ�l��
}

//
// �\�t�g�E�F�A�L�[�{�[�h�̃N���[�Y
//
void SKB_Close( char* text )
{
	Printf("SKB_Close()\n");

	SoftKB_Close(&g_Sample.KBD[g_Sample.uiNowKeyType]); // �L�[�{�[�hClose

	g_Sample.iCoverView = 1;       // TextBox��\��
	g_Sample.iOpenFlag = 0;        // 0:Closed 1:Open��&Opened   -1:Close��
	g_Sample.iControlFlag = 0;     // 0:Closed 1:Open��&Opened&Close��

	g_Sample.Env.inputDevice = g_import.skb.ucShowSKbd; //�N���[�Y���ɏ����c���ipad/usb�j�B

	closeSoftKB();
}



// �֎~���������R�s�[��������֐�
static int SKB_inhibitCopy( inhibitChar_t *src , inhibitChar_t *dst ,int max )
{
	int n;

	dst->uiCharNum = 0;

	for(n=0;n<src->uiCharNum;n++){
		if( n >= max )	return -n;
		ucs4cpy( &dst->pChar[dst->uiCharNum][0], &src->pChar[n][0], 10);
		dst->uiCharNum++;
	}

	return n;
}

// �֎~�����񂩂�A��^�� http:// �Ȃǂ̒�^�������֎~�������g�p���Ă��邩�������A�֎~�L�[�ɐݒ�B
static void SKB_inhibitDisable_stringKey()
{
	int n,bn,sn,cn,dn;
	u_int *disableChar;
	u_int *keyChar;
	skey_t *pKey;
	inhibitChar_t *pInhibit;
	int inhibitMax;

	pInhibit   = &inhibitChar;      //���͋֎~�����o�^��
	inhibitMax = pInhibit->uiCharNum;
	for( n=0; n<SKBTYPE_MAX; n++ ){ 
		pKey = &g_Sample.KBD[n];
		if( pKey->uiKeyTopNo != KEYTOP_ABC )	continue;// ABC �z��ȊO�͊֌W�Ȃ�
		for( bn=0; bn<pKey->uiBtnNum ;bn++ ){ // �{�^���L�[����Â`�F�b�N
			for( sn=0; sn<8; sn++ ){          // Shift Ctrl Kanji �ȂǂłW�P�[�X���̏�񂪂���
				if( pKey->pBtns[bn].state[sn].ctrlCode ){ //�R���g���[���L�[�͖���
					continue;
				}
				keyChar = pKey->pBtns[bn].state[sn].key;

				if( keyChar[1]==0 )	continue; // ��^���̃L�[�ȊO�͖���

				for( cn=0; cn<10; cn++){
					if( keyChar[cn] == 0 ) break;

					for( dn=0; dn < inhibitMax ;dn++ ){
						disableChar = &pInhibit->pChar[dn][0];
						if( disableChar[1] )             continue;
						if( keyChar[cn] == disableChar[0] ) break;
					}
					if( dn != inhibitMax ){ //�֎~�������܂܂�Ă�����
						for( dn=0; dn< pInhibit->uiCharNum; dn++ ){ //�֎~�L�����N�^�̏d���`�F�b�N
							if( ucs4cmp( keyChar, &pInhibit->pChar[dn][0], 10 )==0 )	break;
						}
						if( pInhibit->uiCharNum==0 || dn==pInhibit->uiCharNum ){// �֎~�L�����N�^���d�����Ă��Ȃ�
							// �֎~�L�����N�^�ɒǉ�
							if( pInhibit->uiCharNum < INHIBIT_CHAR_MAX ){//�֎~������o�b�t�@�̍ő吔�𒴂��Ă��Ȃ����
								ucs4cpy( &pInhibit->pChar[pInhibit->uiCharNum][0], keyChar, 10 );
								pInhibit->uiCharNum ++; //�֎~������o�^�B
							}
						}
						break;
					}
				}
			}
		}
	}
}

// ���͋���������A�\�t�g�E�F�A�L�[�{�[�h�ƏƂ炵���킹�āA
// ���͋֎~���������A�쐬��������֐��B
// �i�t�r�a�L�[�{�[�h������͂ł��镶�����l�����Ă��Ȃ��j
// ���ɏd�������Ȃ̂ŁA��v���C�I���e�B�̃X���b�h�Ŏ��s���Ă���B
static void SKB_thread_InhibitEnableToDisable()
{
	int n,bn,sn,cn;
	u_int *enableChar;
	u_int *keyChar;
	skey_t *pKey;
	inhibitChar_t *pEnable = &EnableChar; 
	inhibitChar_t *pInhibit;
	int inhibitMax;

	inhibitChar.uiCKeyNum = 0;
	inhibitChar.pCtrlKey  = NULL;
	inhibitChar.uiCharNum = 0;
	inhibitChar.pChar     = inhibitUCS4;
	inhibitSubChar.uiCKeyNum = 0;
	inhibitSubChar.pCtrlKey  = NULL;
	inhibitSubChar.uiCharNum = 0;
	inhibitSubChar.pChar     = inhibitSubUCS4;

	Printf("enableChar count %d, %p \n",pEnable->uiCharNum,(void *)pEnable);

	for( n=0; n<SKBTYPE_MAX; n++ ){ //�S�ẴL�[�{�[�h������
		pKey = &g_Sample.KBD[n];
		if( pKey->uiKeyTopNo == KEYTOP_SIGN )	continue;// Jis�\���͔͂�T�|�[�g
		for( bn=0; bn<pKey->uiBtnNum ;bn++ ){ // �{�^���L�[����Â`�F�b�N
			for( sn=0; sn<8; sn++ ){          // Shift Ctrl Kanji �ȂǂłW�P�[�X���̏�񂪂���
				if( pKey->pBtns[bn].state[sn].ctrlCode ){ //�R���g���[���L�[�͖��������A
					if( pKey->pBtns[bn].state[sn].ctrlCode != SKBCTRL_TAB ){
						continue;
					}
					keyChar = (u_int *)"\t";              // �^�u�����́A���͐���ɏ]���B
				}
				else keyChar = pKey->pBtns[bn].state[sn].key;

				for( cn=0; cn < pEnable->uiCharNum ;cn++ ){
					enableChar = &pEnable->pChar[cn][0];
					if( ucs4cmp( keyChar, enableChar, 10) == 0 ){//�L�[�L�����N�^�͍ő�P�O������
						break;
					}
				}
				if( cn == pEnable->uiCharNum ){ // �L�[�L�����N�^���������񃊃X�g�Ɋ܂܂�Ă��Ȃ�������
					pInhibit   = &inhibitChar;    //���͋֎~�����o�^��
					inhibitMax = INHIBIT_CHAR_MAX;
					if( inhibitMaskChar.uiCharNum ){ // ���͋֎~�����A�L�[���͉͂̕����񃊃X�g������ꍇ
						for(cn=0;cn<inhibitMaskChar.uiCharNum;cn++){ // �i���_�Ȃǂ̕t�������p���X�g�j
							if( ucs4cmp( keyChar, &inhibitMaskChar.pChar[cn][0], 10)==0 ) break;
						}
						if( cn < inhibitMaskChar.uiCharNum ){ //�Y�������ꍇ�A�o�^���
							pInhibit = &inhibitSubChar;       //�L�[���͉̓��͋֎~�����o�b�t�@��
							inhibitMax = INHIBIT_SUB_CHAR_MAX;//�ύX����B
						}
					}

					for( cn=0; cn< pInhibit->uiCharNum; cn++ ){ //�֎~�L�����N�^�̏d���`�F�b�N
						if( ucs4cmp( keyChar, &pInhibit->pChar[cn][0], 10 )==0 )	break;
					}
					if( pInhibit->uiCharNum==0 || cn==pInhibit->uiCharNum ){// �֎~�L�����N�^���d�����Ă��Ȃ�
						// �֎~�L�����N�^�ɒǉ�
						if( pInhibit->uiCharNum < inhibitMax ){//�֎~������o�b�t�@�̍ő吔�𒴂��Ă��Ȃ����
							ucs4cpy( &pInhibit->pChar[pInhibit->uiCharNum][0], keyChar, 10 );
							pInhibit->uiCharNum ++; //�֎~������o�^�B
						}
					}
				}
			}
		}
	}

	// �쐬�����֎~�����񂩂�A��^�� http:// �Ȃǂ̃L�[���A�֎~�����g�p�L�[�͋֎~�ݒ�ɁB
	SKB_inhibitDisable_stringKey();

	Printf("inhibitChar count=%d *******************\n", inhibitChar.uiCharNum);

	n = ThreadID_SKBWork;
	ThreadID_SKBWork=-1;
	if( n!=-1 )	ExitDeleteThread(); //�X���b�h�Ăяo���̎��́A�X���b�h�폜�B
}

// �֎~������񂩂���͋֎~���������쐬��������֐�
static void SKB_inhibitSet_Disable( inhibitChar_t *pDisable )
{
	int n,cn;
	u_int *keyChar;
	inhibitChar_t *pInhibit;
	int inhibitMax;

	inhibitChar.uiCKeyNum = 0;
	inhibitChar.pCtrlKey  = NULL;
	inhibitChar.uiCharNum = 0;
	inhibitChar.pChar     = inhibitUCS4;
	inhibitSubChar.uiCKeyNum = 0;
	inhibitSubChar.pCtrlKey  = NULL;
	inhibitSubChar.uiCharNum = 0;
	inhibitSubChar.pChar     = inhibitSubUCS4;

	Printf("disableChar count %d, %p \n",pDisable->uiCharNum,(void *)pDisable);

	for(n=0;n<pDisable->uiCharNum;n++){
		keyChar = &pDisable->pChar[n][0];

		pInhibit   = &inhibitChar;    //���͋֎~�����o�^��
		inhibitMax = INHIBIT_CHAR_MAX;

		if( inhibitMaskChar.uiCharNum ){ // ���͋֎~�����A�L�[���͉͂̕����񃊃X�g������ꍇ
			for(cn=0;cn<inhibitMaskChar.uiCharNum;cn++){ // �i���_�Ȃǂ̕t�������p���X�g�j
				if( ucs4cmp( keyChar, &inhibitMaskChar.pChar[cn][0], 10)==0 ) break;
			}
			if( cn < inhibitMaskChar.uiCharNum ){ //�Y�������ꍇ�A�o�^���
				pInhibit = &inhibitSubChar;       //�L�[���͉̓��͋֎~�����o�b�t�@��
				inhibitMax = INHIBIT_SUB_CHAR_MAX;//�ύX����B
			}
		}

		for( cn=0; cn< pInhibit->uiCharNum; cn++ ){ //�֎~�L�����N�^�̏d���`�F�b�N
			if( ucs4cmp( keyChar, &pInhibit->pChar[cn][0], 10 )==0 )	break;
		}
		if( pInhibit->uiCharNum==0 || cn==pInhibit->uiCharNum ){// �֎~�L�����N�^���d�����Ă��Ȃ�
			// �֎~�L�����N�^�ɒǉ�
			if( pInhibit->uiCharNum < inhibitMax ){//�֎~������o�b�t�@�̍ő吔�𒴂��Ă��Ȃ����
				ucs4cpy( &pInhibit->pChar[pInhibit->uiCharNum][0], keyChar, 10 );
				pInhibit->uiCharNum ++; //�֎~������o�^�B
			}
		}
	}
	// �쐬�����֎~�����񂩂�A��^�� http:// �Ȃǂ̃L�[���A�֎~�����g�p�L�[�͋֎~�ݒ�ɁB
	SKB_inhibitDisable_stringKey();

	Printf("inhibitChar count=%d *******************\n", inhibitChar.uiCharNum);
}
// ��������񂩂���͋֎~���������쐬��������֐�
static void SKB_inhibitSet_EnableToDisable( inhibitChar_t *pEnable )
{
	EnableChar.uiCKeyNum = 0;
	EnableChar.pCtrlKey  = NULL;
	EnableChar.uiCharNum = 0;
	EnableChar.pChar     = EnableUCS4;

	if( !pEnable )	return;
	SKB_inhibitCopy( pEnable, &EnableChar, INHIBIT_CHAR_MAX );

	{
		struct ThreadParam t_info;
		struct ThreadParam tp;
		int prio;

		ReferThreadStatus( GetThreadId(), &t_info );
		prio=t_info.currentPriority;
		if( prio < 127-1 )  prio+=1;

		tp.entry		  = SKB_thread_InhibitEnableToDisable; // �X���b�h�̃G���g���֐�
		tp.stack		  = stack;                             // �X�^�b�N�G���A�̐擪�A�h���X
		tp.stackSize	  = sizeof(stack);                     // �X�^�b�N�G���A�̃T�C�Y
		tp.initPriority	  = prio;                              // �X���b�h�v���C�I���e�B
		tp.gpReg		  = &_gp;                              // GP���W�X�^�l

		ThreadID_SKBWork = CreateThread(&tp);    // �X���b�h�𐶐�
		if( ThreadID_SKBWork == -1 ){            // �X���b�h�쐬���s
			SKB_thread_InhibitEnableToDisable(); // �������Ȃ��̂ŁA���Ă�
			return;
		}
		StartThread( ThreadID_SKBWork, 0 );
	}
}


//  ���͋֎~�����ł͂��邪�A��ʃL�[�{�[�h������͂ł��镶�����ݒ�B�i���_�A�����_�ȂǕt�������p�j
void SKB_SetInhibit_MaskChar( u_int (*ucs4)[10], int count )
{
	inhibitChar_t inhibitWork;

	inhibitWork.uiCKeyNum = 0;
	inhibitWork.pCtrlKey  = NULL;
	inhibitWork.uiCharNum = count;
	inhibitWork.pChar     = ucs4;

	inhibitMaskChar.uiCKeyNum = 0;
	inhibitMaskChar.pCtrlKey  = NULL;
	inhibitMaskChar.uiCharNum = 0;
	inhibitMaskChar.pChar     = inhibitMaskUCS4;

	SKB_inhibitCopy( &inhibitWork, &inhibitMaskChar, INHIBIT_SUB_CHAR_MAX );
}

//  �֎~�������̐ݒ�
void SKB_SetInhibit_Disable( u_int (*ucs4)[10], int count )
{
	inhibitChar_t inhibitWork;

	inhibitWork.uiCKeyNum = 0;
	inhibitWork.pCtrlKey  = NULL;
	inhibitWork.uiCharNum = count;
	inhibitWork.pChar     = ucs4;

	// �����������֎~���o�b�t�@�ɍ쐬�B
	SKB_inhibitSet_Disable( &inhibitWork );
	inhibitChar_SetAllKeyborad();  // �L�[�{�[�h�S���ɋ֎~���������ݒ�
}

//  �֎~�������̐ݒ�i���́A���������Łj
void SKB_SetInhibit_Enable( u_int (*ucs4)[10], int count )
{
	inhibitChar_t inhibitWork;

	inhibitWork.uiCKeyNum = 0;
	inhibitWork.pCtrlKey  = NULL;
	inhibitWork.uiCharNum = count;
	inhibitWork.pChar     = ucs4;

	// ��������񂩂�֎~���������֎~���o�b�t�@�ɍ쐬�B
	SKB_inhibitSet_EnableToDisable( &inhibitWork );
	inhibitChar_SetAllKeyborad();  // �L�[�{�[�h�S���ɋ֎~���������ݒ�
}


// ���̕����R�[�h���A�֎~�����ł��邩�A����
//  �Ԃ�l   0:������   1:�֎~����
int SKB_IsDisableUcs4( u_int ucs4 )
{
	static int testCode[2];
	int n;

	testCode[0] = ucs4;
	for( n=0; n < inhibitChar.uiCharNum ;n++ ){
		if( ucs4cmp( testCode, inhibitChar.pChar[n] ,10 )==0 ){
			Printf("out %c\n", inhibitChar.pChar[n][0]);
			return 1;
		}
	}
	for( n=0; n < inhibitSubChar.uiCharNum ;n++ ){
		if( ucs4cmp( testCode, inhibitSubChar.pChar[n] ,10 )==0 ){
			Printf("out %c\n", inhibitSubChar.pChar[n][0]);
			return 1;
		}
	}

	return 0;
}

// �I��
void SKB_Exit(void)
{
	int i;

	if( ThreadID_SKBWork != -1 ){        // �X���b�h���c���Ă�����폜
		TerminateThread( ThreadID_SKBWork );
		DeleteThread( ThreadID_SKBWork );
		ThreadID_SKBWork=-1;
	}

	// �\�t�g�E�F�A�L�[�{�[�h�֌W�I������
	for(i=0; i<SKBTYPE_MAX; i++)
	{
		SoftKB_Close(&g_Sample.KBD[i]);  // �L�[�{�[�hClose
		SoftKB_Delete(&g_Sample.KBD[i]);
	}
	SoftKB_Destroy();

	TextF_Delete(&textField);
	TextF_Destroy();

	Fep_Exit();
	Fepbar_Destroy();
}

//--------------------------------------------------------------------------------------
