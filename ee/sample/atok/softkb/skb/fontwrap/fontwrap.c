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
/*============================================================================

�ȉ��A���ӎ���

pFw->stat.FontRatio		: �傫�� �c���̔䗦�͏�Ɉ��
SetTvFontRatio			: FrameMode/FieldMode�ŕς��
							NTSC : 1.0f
							PAL  : 1.15f

maxHeight : �f�[�^�ɂ���ĈႤ�B

      BASE POINT X
          ��
���\�\�\�\���\�\�\�\�\�\�\�\�\�\�\���\��������������������������
�b        �� R BEARING            �b��                      ��
�b        �����\�\�\�\���b        �b�b                      ��
�b  L BEARING           �b        �b�b                      ��
�b�b���\����            �b        �b�b                      ��
�b���\�\�\���\�\�\�\���\���\      �b�b                      ��
�b�b      �b        ****�b��      �b�b                      ��
�b�b      �b      **    * �b      �b�b                      ��
�b�b      �b     ** �b  �b�b      �b�b                      ��
�b�b      �b**********  �bASCENT  �bTEXTURE 1 ���� ����     �� maxHeight
�b�b      �b  ***   �b  �b�b      �b�b                      ��
�b�b      �b ***    �b  �b�b      �b�b                      ��
�b�b      �b***     �b  �b��      �b�b                      ��
�b�b�Q�Q�Q ***�Q�Q�Q�b�Q�Q__�Q�Q�Q�b�Q�Q BASE POINT Y       ��
�b�b      **        �b  �b��      �b�b                      ��
�b *    **�b        �b  �bDESCENT �b�b                      ��
�b�b****  �b        �b  �b��      �b�b                      ��
�b���\�\�\���\�\�\�\���\���\      �b�b                      ��
�b        �b���\�\���b            �b�b                      ��
�b           WIDTH                �b��                      ��
���\�\�\�\�\�\�\�\�\�\�\�\�\�\�\�\������������������������������
�b���\�\ TEXTURE 1 ���� ��  �\�\���b
�b���\�\ maxWidth           �\�\���b


============================================================================*/
#include <eekernel.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include <sifdev.h>
#include <libpkt.h>
#include <libpfont.h>
#include <libgraph.h>

#include <assert.h>	
#include "fontwrap.h"

#define FIELD_MODE  0
#define PAL_MODE   0
/*---------------------------------------------------------*/
#if FIELD_MODE
	#define VRESOLUTION_ADJUST(_v_)		(_v_)
	#define VRESOLUTION_ADJUST_F(_v_)	(_v_)
#else
	#define VRESOLUTION_ADJUST(_v_)		((_v_)/2)
	#define VRESOLUTION_ADJUST_F(_v_)	((_v_)/2.0f)
#endif		// FIELD_MODE

#if PAL_MODE
	#define SCREEN_VHEIGHT	(512)
	#define SCREEN_HEIGHT	(VRESOLUTION_ADJUST(SCREEN_VHEIGHT))
#else
	#define SCREEN_VHEIGHT	(448)
	#define SCREEN_HEIGHT	(VRESOLUTION_ADJUST(SCREEN_VHEIGHT))
#endif

#define SCREEN_WIDTH	(640)
#define XOFFSET			(2048 - SCREEN_WIDTH / 2)
#define YOFFSET			(2048 - SCREEN_HEIGHT / 2)
#define FONT_GSMEMSIZE	(2048 * 1)
/*---------------------------------------------------------*/

#define FONT_DUMMY_CHAR		(0xd800+24)
#define DEF_FONT_RATIO		(1.0f)
#define DEF_FONT_PITCH		(0)
#define OFFSET_X			(24)
#define OFFSET_Y			(24)
#define FONT_SIZE_FX		(32.0f)
#define FONT_SIZE_FY		(40.0f)
#define FONTCTRL			('\a')

#define UNDERLINE_H		(1)
#define ULDOT_W			(4)

#define _SET_VECTOR(v, r, g, b, a)	(v[0] = r, v[1] = g, v[2] = b, v[3] = a)


typedef struct
{
	int	FontRetHeight;		// \n return �Ή��p�i�擪 X �ۑ��AY �����j
	int	FontPropSw;			// �������Ԏw��
	int	FontRatioSw;		// �Վ��T�C�Y�X�C�b�`
	int	FontPitch;			// �s�b�`

	int	FontAdjustY;		// Y���W1/16�s�N�Z���P�ʒ���
	int	Window_x;			// �E�C���h�E x
	int Window_w;			// �E�B���h�E w
	int effect;				// �w�i�F�A�����Ȃǂ̕�������
    int clip;				// ��ʃN���b�v ������

	float TvFontRatioY;		// �A�X�y�N�g��ݒ�p
	float FontRatio;		// ��g�嗦
	int pad0;

	sceVu0FVECTOR locate;	// �`��ʒu
	sceVu0FVECTOR color;	// �`��F

	sceVu0IVECTOR bgcolor[4];	// �w�i�F
	sceVu0IVECTOR ulcolor;		// �����F

} fontStatus_t;




typedef struct
{
	u_long128 *fontData;
	int  fd;
	int calc;

	int MaxWidth;
	int MaxAscent;
	int MaxHeight;
	int transmodified;


	float left;				// �`��P�ʖ��̍��[���W�ۑ�
	float top;				// �`��P�ʖ��̏�[���W�ۑ�
	float right;
	float bottom;


	fontStatus_t	stat;

	sceVu0FMATRIX screen;	// ���[�J�����X�N���[���}�g���N�X(�P�ʍs��)
	sceVu0FMATRIX trans;	// �ό`�}�g���N�X
	int bExit;
	u_long128 font_cache[64];	// 1k bytes


	u_int qwc;
	sceVif1Packet* pPacket;
	sceVif1Packet packet;

} fontWork_t;
static fontWork_t g_FontWork;
static fontWork_t *pFw = &g_FontWork;



static int  fontDrawBG(int left, int top, int right, int bottom);
static int  fontDrawUL(int left, int top, int right, int bottom);
static int  fontDrawULD(int left, int top, int right, int bottom);
static int  calcPacketSpace(void);
static void calcDrawArea(char const *str, int *width, int *height);
static void _memcpy128(u_long128* pDst, u_long128 const* pSrc, int qwc);
static int  gethex(int c);
static int  fontFilter(int fd, scePFontControl* pCtrl);
static int  fontFilterPutc(int fd, scePFontControl* pCtrl, int c);
static void updateTransMatrix(void);
static void updateScreenMatrix(void);


static union
{
	u_long d[2];
	u_long128 tag;
} const _gif_ad = {{ SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0xeUL }};


//============================================================================
// Color
static int const FontCol[][3] =
{
	{  96,  96,  96 },		// 0 WHITE
	{ 110, 110,   0 },		// 1 YELLOW
	{  47,  87, 127 },		// 2 BLUE
	{ 190, 128, 150 },		// 3 PINK
	{  44,  44,  44 },		// 4 GRAY
	{  80, 140, 140 },		// 5 CYAN
	{  98,  56,  56 },		// 6 ORANGE

	// original
	{  60,  60,  60 },		// 7 LGRAY		// BROWSER COPY�u���v
	{  90,  90,  90 },		// 8 BWHITE		// not used
	{  20,  90,  60 },		// 9 GREEN		// not used
};




//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//  Font_Init												ref : fontwrap.h
//
//  �t�H���g������
//
//  �߂�l : void
void Font_Init(void)
{
	assert(pFw);

	memset(pFw, 0x0, sizeof(fontWork_t));		// �t�H���g���[�J������
}


//============================================================================
//  Font_Load												ref : fontwrap.h
//
//  �t�H���g���[�h
//
//  int gsclutadr   : CLUT�A�h���X
//  int gstexadr    : TEXTURE�A�h���X
//  u_long128 *pFontData : �t�H���g�f�[�^�ւ̃|�C���^
//
//  �߂�l int : 0:����I��/ 1:�G���[
int Font_Load(int gsclutadr, int gstexadr, u_long128 *pFontData)
{
	{
		int csize = scePFontCalcCacheSize(2) - scePFontCalcCacheSize(1);
		int fsize = scePFontCalcCacheSize(1) - csize;
		int num = (sizeof(pFw->font_cache) - fsize) / csize;
		pFw->fd = scePFontInit(num, pFw->font_cache);
		//assert(sizeof(SPR.font_cache) >= scePFontCalcCacheSize(num));
		scePFontSetTexMem(pFw->fd, gstexadr, FONT_GSMEMSIZE, gsclutadr);
	}

	pFw->fontData = pFontData;						// �t�H���g�f�[�^�ݒ�
	if(0 != scePFontAttachData(pFw->fd, pFw->fontData)){
		printf("scePFontAttachData(pFw->fd, pFw->fontData) failed\n");
		return 1;
	}
	scePFontSetFilter(pFw->fd, fontFilter);

	{
		// �ʒu�ݒ菉����
		pFw->stat.locate[0] = 0.0f;
		pFw->stat.locate[1] = 0.0f;
		pFw->stat.locate[2] = 0.0f;
		pFw->stat.locate[3] = 0.0f;

		// �t�H���g�̍������L���b�V��
		pFw->MaxWidth  = scePFontGetFontInfo(pFw->fd)->max_width;
		pFw->MaxAscent = scePFontGetFontInfo(pFw->fd)->max_ascent;
		pFw->MaxHeight = pFw->MaxAscent - scePFontGetFontInfo(pFw->fd)->max_descent;

		pFw->transmodified = 1;
		updateTransMatrix();
		updateScreenMatrix();
	}

	// �f�t�H���g�ݒ�
	Font_SetLocate(0, 0);
	Font_SetPitch(DEF_FONT_PITCH);
	Font_SetRetHeight(0);
	Font_SetColor(128, 128, 128, 128);
	Font_SetProp(0);
	Font_SetRatio(DEF_FONT_RATIO);
	Font_SetTvFontRatio(1.0f);

	Font_SetULColor(255, 0, 0, 64);
	Font_SetBGColor(0, 0, 64,  64);
	Font_SetBGColor(0, 0, 64,  64);
	Font_SetBGColor(0, 0, 255, 64);
	Font_SetBGColor(0, 0, 255, 64);

	return 0;
}


//============================================================================
//  Font_Unload												ref : fontwrap.h
//
//  �t�H���g���폜
void Font_Unload(void)
{
//	if(pFw->fontData != NULL)
//	{
//		free(pFw->fontData);
//	}

	scePFontRelease(pFw->fd);
}


//============================================================================
//  Font_SetLocate											ref : fontwrap.h
//
//  �ʒu�ݒ�
//
//  int x : X���W(16�{���Ȃ��l)
//  int y : Y���W(16�{���Ȃ��l)
void Font_SetLocate(int x, int y)
{
	pFw->stat.locate[0] = (float)x;
	pFw->stat.locate[1] = (float)y;
}


//============================================================================
//  Font_SetLocate_f											ref : fontwrap.h
//
//  �ʒu�ݒ�
//
//  float x : X���W(16�{���Ȃ��l)
//  float y : Y���W(16�{���Ȃ��l)
void Font_SetLocate_f( float x, float y )
{
	pFw->stat.locate[0] = (float)x;
	pFw->stat.locate[1] = (float)y;
}


//============================================================================
//  Font_SetPitch											ref : fontwrap.h
//
//  �s�b�`�ݒ�
//
//  int pitch : �s�b�`
void Font_SetPitch(int pitch)
{
	pFw->stat.FontPitch = (float)(pitch - DEF_FONT_PITCH);
	pFw->transmodified = 1;
}


//============================================================================
//  Font_SetRetHeight										ref : fontwrap.h
//
//  \n(return)���� y �����ݒ�
//
//  int y : y ����
void Font_SetRetHeight(int y)
{
	pFw->stat.FontRetHeight = y;
}


//============================================================================
//  Font_SetProp											ref : fontwrap.h
//
//  �v���|�[�V�����ݒ�
//
//  int sw : �v���|�[�V�����l
void Font_SetProp(int sw)
{
	pFw->stat.FontPropSw = sw;
}


//============================================================================
//  Font_SetRatio											ref : fontwrap.h
//
//  �傫���ݒ�
//
//  float flRatio : �����傫��
void Font_SetRatio(float flRatio)
{
	pFw->stat.FontRatio = flRatio;
	pFw->transmodified = 1;
}


//============================================================================
//  Font_SetRatioPixel										ref : fontwrap.h
//
//  �傫���ݒ�
//
//  int size : �����傫��(�s�N�Z���Őݒ�)
void Font_SetRatioPixel(int size)
{
	float fsize = (float)size/FONT_SIZE_FY;

	Font_SetRatio(fsize);
}


//============================================================================
//  Font_SetTvFontRatio										ref : fontwrap.h
//
//  NTSC/PAL�A�X�y�N�g�ݒ�
//
//  float flRatio : �A�X�y�N�g��
void Font_SetTvFontRatio(float flRatio)
{
	pFw->stat.TvFontRatioY = flRatio;
	pFw->transmodified = 1;
}


//============================================================================
//  Font_SetColor											ref : fontwrap.h
//
//  �����F�ݒ�
//
//  int r : ��
//  int g : ��
//  int b : ��
//  int a : �A���t�@�l
void Font_SetColor(int r, int g, int b, int a)
{
	pFw->stat.color[0] = (float)r / 128.0f;
	pFw->stat.color[1] = (float)g / 128.0f;
	pFw->stat.color[2] = (float)b / 128.0f;
	pFw->stat.color[3] = (float)a / 128.0f;
}


//============================================================================
//  Font_SetBGColor											ref : fontwrap.h
//
//  �w�i�F�ݒ�(4���_���F)
//
//  int r : ��
//  int g : ��
//  int b : ��
//  int a : �A���t�@�l
void Font_SetBGColor(int r, int g, int b, int a)
{
	_SET_VECTOR(pFw->stat.bgcolor[0], r, g, b, a);
	_SET_VECTOR(pFw->stat.bgcolor[1], r, g, b, a);
	_SET_VECTOR(pFw->stat.bgcolor[2], r, g, b, a);
	_SET_VECTOR(pFw->stat.bgcolor[3], r, g, b, a);
}


//============================================================================
//  Font_SetBGColorEx										ref : fontwrap.h
//
//  �w�i�F�ݒ�(���_�F)
//
//  int no : ���_�̃C���f�b�N�X
//  int r : ��
//  int g : ��
//  int b : ��
//  int a : �A���t�@�l
void Font_SetBGColorEx(int no, int r, int g, int b, int a)
{
	_SET_VECTOR(pFw->stat.bgcolor[no], r, g, b, a);
}


//============================================================================
//  Font_SetULColor											ref : fontwrap.h
//
//  �A���_�[���C���F�ݒ�
//
//  int r : ��
//  int g : ��
//  int b : ��
//  int a : �A���t�@�l
void Font_SetULColor(int r, int g, int b, int a)
{
	_SET_VECTOR(pFw->stat.ulcolor, r, g, b, a);
}


//============================================================================
//  Font_GetStrLength										ref : fontwrap.h
//
//  �����̕`�敝�擾
//
//  const char *pszText : �����擾������������ւ̃|�C���^
//
//  �߂�l int : ������̕��i�s�N�Z���l�j
int Font_GetStrLength(const char *pszText)
{
	int width;
	calcDrawArea(pszText, &width, NULL);
	width += pFw->stat.FontPitch * pFw->stat.FontRatio;
	return width;
}


//============================================================================
//  Font_SetEffect											ref : fontwrap.h
//
//  �����̑�����ݒ�
//
//  int mode : ����NO
void Font_SetEffect(int mode)
{
	pFw->stat.effect = mode;
}


//============================================================================
//  Font_PutsPacket											ref : fontwrap.h
//
//  �����p�P�b�g���쐬
//
//  const char *str : �`�悵����������
//  sceVif1Packet* pPacket : �p�P�b�g�o�b�t�@�ւ̃|�C���^
//  u_int qwc : �p�P�b�g�o�b�t�@�̑傫��
//
//  �߂�l int : �ʏ�I��(1)
//               -1:�p�P�b�g������������ ??
int Font_PutsPacket(const char *str, sceVif1Packet* pPacket, u_int qwc)
{
	int result;

	pFw->pPacket = pPacket;
	pFw->qwc     = qwc;

	if(4 > calcPacketSpace())
	{
		return 1;
	}

	updateTransMatrix();

	scePFontSetFontMatrix(pFw->fd, (sceVu0FMATRIX const*)&pFw->trans);
	scePFontSetLocate(pFw->fd, (sceVu0FVECTOR const*)&pFw->stat.locate);
	scePFontSetColor(pFw->fd, (sceVu0FVECTOR const*)&pFw->stat.color);

	pFw->calc = 0;
	pFw->left   = pFw->stat.locate[0];
	pFw->top    = pFw->stat.locate[1];
	pFw->right  = pFw->left;
	pFw->bottom = pFw->top;

	{
		sceVif1PkCnt(pFw->pPacket, 0);
		sceVif1PkOpenDirectCode(pFw->pPacket, 0);
		sceVif1PkOpenGifTag(pFw->pPacket, _gif_ad.tag);
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_TEST_1, SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 0, 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA(0, 1, 0, 1, 0));
		sceVif1PkCloseGifTag(pFw->pPacket);
		sceVif1PkCloseDirectCode(pFw->pPacket);
	}

	result = scePFontPuts(pFw->fd, pFw->pPacket, (int)pFw->qwc, (u_char const*)str);
	if(-1 == result)
	{
		return result;
	}

	scePFontGetLocate(pFw->fd, &pFw->stat.locate);
	scePFontGetColor(pFw->fd, &pFw->stat.color);

	return 1;
}


//============================================================================
//  Font_SetClipOn											ref : fontwrap.h
//
//  �N���b�vON
//
void Font_SetClipOn(void)
{
	pFw->stat.clip = 1;
}


//============================================================================
//  Font_SetClipOn											ref : fontwrap.h
//
//  �N���b�vOFF
//
void Font_SetClipOff(void)
{
	pFw->stat.clip = 0;
}

//============================================================================
//  Font_GetHeight											ref : fontwrap.h
//
//  �����̍����𓾂� (�㉺�̗]������)
//
//  �߂�l �Fint : �����̍���
int Font_GetHeight(void)
{
	return pFw->MaxHeight * pFw->stat.FontRatio * pFw->stat.TvFontRatioY;
}


//============================================================================
//  Font_GetWidth											ref : fontwrap.h
//
//  �����̕��𓾂�
//
//  �߂�l �Fint : �����̍���
//int Font_GetWidth(void)
//{
//}



//============================================================================
//  Font_GetLocateX											ref : fontwrap.h
//
//  X�l���擾
//
//  �߂�l �Fint : X�l
int Font_GetLocateX(void)
{
	return (int)(pFw->stat.locate[0] * 16.0f);//FONT_SIZE_FX);
}


//============================================================================
//  Font_GetLocateY											ref : fontwrap.h
//
//  Y�l���擾
//
//  �߂�l �Fint : Y�l
int Font_GetLocateY(void)
{
	return (int)(pFw->stat.locate[1] * 16.0f);
}


//============================================================================
//  Font_GetLocate_f										ref : fontwrap.h
//
//  XY�l���擾
//
void Font_GetLocate_f( float *x, float *y )
{
	if(x)	*x = pFw->stat.locate[0];
	if(y)	*y = pFw->stat.locate[1];
}


//============================================================================
// STATIC FUNCTIONS
//============================================================================
//============================================================================
//	fontDrawBG
//
//	�t�H���g�̔w�i��`��
//
//	int left : ����X���W
//	int top : �㑤Y���W
//	int right : �E��X���W
//	int bottom : ����Y���W
//
//	�߂�l : int : 0:����/-1:�p�P�b�g�i�[�̈悪�s�����Ă���
static int fontDrawBG(int left, int top, int right, int bottom)
{
	if(11 > calcPacketSpace())
	{
		printf("ERROR : _FontDrawBG - packet space is low\n");
		return -1;
	}

	{
		sceVif1PkCnt(pFw->pPacket, 0);
		sceVif1PkOpenDirectCode(pFw->pPacket, 0);
		sceVif1PkOpenGifTag(pFw->pPacket, _gif_ad.tag);
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_PRIM, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 1, 0, 0, 1, 0, 0, 0, 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(pFw->stat.bgcolor[0][0], pFw->stat.bgcolor[0][1], pFw->stat.bgcolor[0][2], pFw->stat.bgcolor[0][3], 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_XYZ2, SCE_GS_SET_XYZ((XOFFSET + left) << 4, (YOFFSET + top) << 4, 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(pFw->stat.bgcolor[1][0], pFw->stat.bgcolor[1][1], pFw->stat.bgcolor[1][2], pFw->stat.bgcolor[1][3], 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_XYZ2, SCE_GS_SET_XYZ((XOFFSET + right) << 4, (YOFFSET + top) << 4, 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(pFw->stat.bgcolor[2][0], pFw->stat.bgcolor[2][1], pFw->stat.bgcolor[2][2], pFw->stat.bgcolor[2][3], 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_XYZ2, SCE_GS_SET_XYZ((XOFFSET + left) << 4, (YOFFSET + bottom) << 4, 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(pFw->stat.bgcolor[3][0], pFw->stat.bgcolor[3][1], pFw->stat.bgcolor[3][2], pFw->stat.bgcolor[3][3], 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_XYZ2, SCE_GS_SET_XYZ((XOFFSET + right) << 4, (YOFFSET + bottom) << 4, 0));
		sceVif1PkCloseGifTag(pFw->pPacket);
		sceVif1PkCloseDirectCode(pFw->pPacket);
	}

	return 0;
}


//============================================================================
//	fontDrawUL
//
//	������`��(����)
//
//	int left : ����X���W
//	int top : �㑤Y���W
//	int right : �E��X���W
//	int bottom : ����Y���W
//
//	�߂�l : int : 0:����/-1:�p�P�b�g�i�[�̈悪�s�����Ă���
static int fontDrawUL(int left, int top, int right, int bottom)
{

	if(6 > calcPacketSpace())
	{
		printf("ERROR : _FontDrawBG - packet space is low\n");
		return -1;
	}

	{
		sceVif1PkCnt(pFw->pPacket, 0);
		sceVif1PkOpenDirectCode(pFw->pPacket, 0);
		sceVif1PkOpenGifTag(pFw->pPacket, _gif_ad.tag);
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_PRIM, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 0, 0, 1, 0, 0, 0, 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(pFw->stat.ulcolor[0], pFw->stat.ulcolor[1], pFw->stat.ulcolor[2], pFw->stat.ulcolor[3], 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_XYZ2, SCE_GS_SET_XYZ((XOFFSET + left) << 4, (YOFFSET + bottom - UNDERLINE_H) << 4, 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_XYZ2, SCE_GS_SET_XYZ((XOFFSET + right) << 4, (YOFFSET + bottom) << 4, 0));
		sceVif1PkCloseGifTag(pFw->pPacket);
		sceVif1PkCloseDirectCode(pFw->pPacket);
	}

	return 0;
}


//============================================================================
//	fontDrawULD
//
//	������`��(�_��)
//
//	int left : ����X���W
//	int top : �㑤Y���W
//	int right : �E��X���W
//	int bottom : ����Y���W
//
//	�߂�l : int : 0:����/-1:�p�P�b�g�i�[�̈悪�s�����Ă���
static int fontDrawULD(int left, int top, int right, int bottom)
{
	int x0 = left - (left % ULDOT_W);
	int count = ((right - (right % ULDOT_W)) - x0) / ULDOT_W;

	if((4 + 2 * count) > calcPacketSpace())
	{
		printf("ERROR : _FontDrawBG - packet space is low\n");
		return -1;
	}

	{
		sceVif1PkCnt(pFw->pPacket, 0);
		sceVif1PkOpenDirectCode(pFw->pPacket, 0);
		sceVif1PkOpenGifTag(pFw->pPacket, _gif_ad.tag);
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_PRIM, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 0, 0, 1, 0, 0, 0, 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(pFw->stat.ulcolor[0], pFw->stat.ulcolor[1], pFw->stat.ulcolor[2], pFw->stat.ulcolor[3], 0));

		while(count--){
			sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_XYZ2, SCE_GS_SET_XYZ((XOFFSET + x0) << 4, (YOFFSET + bottom - UNDERLINE_H) << 4, 0));
			sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_XYZ2, SCE_GS_SET_XYZ((XOFFSET + x0 + ULDOT_W / 2) << 4, (YOFFSET + bottom) << 4, 0));
			x0 += ULDOT_W;
		}
		sceVif1PkCloseGifTag(pFw->pPacket);
		sceVif1PkCloseDirectCode(pFw->pPacket);
	}

	return 0;
}


//============================================================================
//	calcPacketSpace
//
//	�p�P�b�g�o�b�t�@�󂫗̈���v�Z
//
//	�߂�l : int : �󂫗̈�
static int calcPacketSpace(void)
{
	return pFw->qwc - 1 - (((u_int)pFw->pPacket->pCurrent - (u_int)pFw->pPacket->pBase)/sizeof(u_long128));

}


//============================================================================
//	calcDrawArea
//
//	�`��̈���v�Z
//	�����ł͂Ȃ��A�w�i�≺���A�_���Ɏg����
//
//	char const *str : �`�悵����������
//	int *width : �`��̈�̕�
//	int *height: �`��̈�̍���
static void calcDrawArea(char const *str, int *width, int *height)
{
	fontStatus_t back __attribute__ ((aligned(16)));
	sceVu0FVECTOR pos = { 0.0f, 0.0f, 0.0f, 0.0f };
	_memcpy128((u_long128*)&back, (u_long128*)&pFw->stat, sizeof(fontStatus_t) / sizeof(u_long128));

	updateTransMatrix();

	pFw->calc = 1;
	pFw->left = 0.0f;
	pFw->top  = 0.0f;
	pFw->right  = pFw->left;
	pFw->bottom = pFw->top;

	scePFontSetFontMatrix(pFw->fd, (sceVu0FMATRIX const*)&pFw->trans);
	scePFontSetLocate(pFw->fd, (sceVu0FVECTOR const*)&pos);

	scePFontCalcRect(pFw->fd, (u_char const*)str, NULL, NULL);

	if(NULL != width){
		*width = (int)pFw->right;
	}
	if(NULL != height){
		*height = (int)pFw->bottom;
	}

	_memcpy128((u_long128*)&pFw->stat, (u_long128*)&back, sizeof(fontStatus_t) / sizeof(u_long128));
}


//============================================================================
//	_memcpy128
//
//	memcpy�̍�����
//
//	u_long128* pDst : �R�s�[��
//	u_long128 const* pSrc : �R�s�[��
//	int qwc : �R�s�[�������T�C�Y
static void _memcpy128(u_long128* pDst, u_long128 const* pSrc, int qwc)
{
	int count0 = qwc % 4;
	int count1 = qwc / 4;

	while(count0--)
	{
		*pDst++ = *pSrc++;
	}
	while(count1--)
	{
		u_long128 d0 = *pSrc++;
		u_long128 d1 = *pSrc++;
		u_long128 d2 = *pSrc++;
		u_long128 d3 = *pSrc++;
		*pDst++ = d0;
		*pDst++ = d1;
		*pDst++ = d2;
		*pDst++ = d3;
	}
}


//============================================================================
//	gethex
//
//	������ŕ\�����ꂽ������16�i->10�i
//
//	int c : ������ŕ\�����ꂽ10�i��
//
//	�߂�l: int : 16�i��
static int gethex(int c)
{
	if(('0' <= c) && ('9' >= c)){
		return c - '0';
	}
	c |= 0x20;		// tolower ?
	if(('a' <= c) && ('f' >= c)){
		return (c - 'a') + 10;
	}
	return 0;
}


//============================================================================
//	fontFilter
//
//	�����������ʗp�̃t�B���^
//
//	int fd : 
//	scePFontControl* pCtrl : 
//
//	�߂�l: int : 
static int fontFilter(int fd, scePFontControl* pCtrl)
{
	int stat = -1;
	int c;
	while(0 < (c = pCtrl->Getc(fd)))
	{
		if(0x20 <= c)
		{
			if((0xFEFFU == c) || (0xFFFEU == c) || (0xFFFFU == c))
			{
			}
			else
			{
				if(0 > fontFilterPutc(pFw->fd, pCtrl, c))
				{
					pCtrl->Ungetc(pFw->fd);
					return stat;
				}
				stat = 1;
			}
		}
		else
		{
			switch(c)
			{
			case '\n':
				{
					sceVu0FVECTOR v;
					scePFontGetLocate(fd, &v);
					v[0] = FONT_SIZE_FX;
					v[1] += FONT_SIZE_FY;
					scePFontSetLocate(fd, (sceVu0FVECTOR const*)&v);
				}
				break;

			case '\t':
				{
					sceVu0FVECTOR v;
					scePFontGetLocate(fd, &v);
					v[0] += 32.0f;
					scePFontSetLocate(fd, (sceVu0FVECTOR const*)&v);
				}
				break;

			case FONTCTRL:
				c = pCtrl->Getc(fd);
				switch(c){
				case 'c':		// \acx x...Color code
					{
						int no = pCtrl->Getc(fd) - '0';
						sceVu0FVECTOR v;
						scePFontGetColor(fd, &v);
						v[0] = (float)FontCol[no][0] / 128.0f;
						v[1] = (float)FontCol[no][1] / 128.0f;
						v[2] = (float)FontCol[no][2] / 128.0f;
						scePFontSetColor(fd, (sceVu0FVECTOR const*)&v);
					}
					break;

				case 'p':		// \apxx xx...Propotional Width   \ap@c c�̕����ɍ��킹��
					{
						int c1 = pCtrl->Getc(fd);
						if('@' == c1){
							int c2 = pCtrl->Getc(fd);
							scePFontGlyph glyph;
							if(0 == scePFontGetGlyph(fd, c2, &glyph)){
								pFw->stat.FontPropSw = glyph.prop->width;
							}
						}else{
							int c2 = pCtrl->Getc(fd);
							pFw->stat.FontPropSw = (c1 - '0') * 10 + (c2 - '0');
						}
					}
					break;

				case 'r':		// \arx.xx x.xx...
					{
						int c1;
						int c2;
						int c3;
						int c4;
						c1 = pCtrl->Getc(fd);
						c2 = pCtrl->Getc(fd);
						c3 = pCtrl->Getc(fd);
						c4 = pCtrl->Getc(fd);
						pFw->stat.FontRatioSw = (int)(c1 - '0') * 100 + (int)(c3 - '0') * 10 + (int)(c4 - '0');
						pFw->transmodified = 1;
						updateTransMatrix();
						scePFontSetFontMatrix(fd, (sceVu0FMATRIX const*)&pFw->trans);
					}
					break;

				case 'y':		// \ay+xx \ay-xx xx:16�i�� Y���W��1/16�s�N�Z���P�ʂŒ���
					{
						int c1 = pCtrl->Getc(fd);
						int c2 = pCtrl->Getc(fd);
						int c3 = pCtrl->Getc(fd);
						sceVu0FVECTOR v;
						scePFontGetLocate(fd, &v);
						if(0 != (pFw->stat.FontAdjustY = ((gethex(c2) << 4) + gethex(c3)))){
							pFw->stat.FontAdjustY *= (c1 == '-') ? -1 : 1;
						}
						v[1] = pFw->top + (float)pFw->stat.FontAdjustY / 16.0f;
						scePFontSetLocate(fd, (sceVu0FVECTOR const*)&v);
					}
					break;

				case 'a':		// \aaxxx xxx...ambient
					{
						int c1 = pCtrl->Getc(fd);
						int c2 = pCtrl->Getc(fd);
						int c3 = pCtrl->Getc(fd);
						sceVu0FVECTOR v;
						scePFontGetColor(fd, &v);
						v[3] = (float)((c1 - '0') * 100 + (c2 - '0') * 10 + (c3 - '0')) / 255.0f;
						scePFontSetColor(fd, (sceVu0FVECTOR const*)&v);
					}
					break;

				case 's':		// \as zenkaku space
					{
						sceVu0FVECTOR v;
						scePFontGetLocate(fd, &v);
						v[0] += pFw->MaxWidth;
						scePFontSetLocate(fd, (sceVu0FVECTOR const*)&v);
					}
					break;
				default:
					// �s���ȃt�H���g����R�[�h
					break;
				}
			}
		}


		{
			sceVu0FVECTOR v;
			scePFontGetLocate(fd, &v);
			if(v[0] > pFw->right)
			{
				pFw->right = v[0];
			}
			if(v[0] > pFw->bottom)
			{
				pFw->bottom = v[0];
			}
		}

	}

	return 0;
}


//============================================================================
//	fontFilterPutc
//
//	�������������p�t�B���^
//
//	int fd : 
//	scePFontControl* pCtrl :
//	int c :
//
//	�߂�l: int : 
static int fontFilterPutc(int fd, scePFontControl* pCtrl, int c)
{
	static u_int _effectmsk = 0xffffffffU;
	u_int _effect = pFw->stat.effect & _effectmsk;

	sceVu0FVECTOR locate;
	float xl = 0.0f;
	float xr = 0.0f;

	if(0 != pFw->calc)
	{
		_effect = 0;
	}

	if((0 != pFw->stat.FontPropSw) || (0 != _effect) || pFw->stat.clip )
	{
		float xpitch = pFw->trans[3][0];
		float xratio = pFw->trans[0][0];

		scePFontGlyph glyph;

		if(0 > scePFontGetGlyph(fd, c, &glyph))
		{
#if defined(FONT_DUMMY_CHAR)
			if(0 > scePFontGetGlyph(fd, FONT_DUMMY_CHAR, &glyph))
			{
				return 0;
			}
#else
			return 0;
#endif
		}

		scePFontGetLocate(fd, &locate);
		xl = locate[0];
		xr = xl;

		if(0 != pFw->stat.FontPropSw){
			xr = xl + xpitch + (float)pFw->stat.FontPropSw * xratio;

		}else{
			xr = xl + xpitch + glyph.prop->width * xratio;
		}

		if(0 != _effect)
		{
			float ypitch = pFw->trans[3][1];
			float yratio = pFw->trans[1][1];
			float yt = locate[1] + (float)ypitch - (float)pFw->MaxAscent * yratio;
			float yb = yt + (float)pFw->MaxHeight * yratio;

			if(_effect & FONT_EFFECT_BG){
				if(-1 == fontDrawBG((int)xl, (int)yt, (int)xr, (int)yb)){
					return -1;
				}
				_effectmsk &= ~FONT_EFFECT_BG;
			}
			if(_effect & FONT_EFFECT_UL){
				if(-1 == fontDrawUL((int)xl, (int)yt, (int)xr, (int)yb)){
					return -1;
				}
				_effectmsk &= ~FONT_EFFECT_UL;
			}
			if(_effect & FONT_EFFECT_ULD){
				if(-1 == fontDrawULD((int)xl, (int)yt, (int)xr, (int)yb)){
					return -1;
				}
				_effectmsk &= ~FONT_EFFECT_ULD;
			}
		}

		if(0 != pFw->stat.FontPropSw){
			locate[0] = xl + (float)((pFw->stat.FontPropSw - glyph.prop->width) / 2) * xratio;
			scePFontSetLocate(fd, (sceVu0FVECTOR const*)&locate);
		}
	}

    if( pFw->stat.clip && ( xl > 640.0f || xr < 0.0f ) )
    {
        // ���̕����̓N���b�v���ꂽ
    }
    else
	{
		int result;
		result = pCtrl->Putc(fd, c);
#if defined(FONT_DUMMY_CHAR)
		if(-2 == result){
			// �O���t���Ȃ��� :D~
			result = pCtrl->Putc(fd, FONT_DUMMY_CHAR);
		}
#endif
		if(-1 == result){
			if(xl != xr){
				locate[0] = xl;
				scePFontSetLocate(fd, (sceVu0FVECTOR const*)&locate);
			}
			return -1;
		}
	}

	if(xl != xr){
		locate[0] = xr;
		scePFontSetLocate(fd, (sceVu0FVECTOR const*)&locate);
	}

	_effectmsk = 0xffffffffU;

	return 0;
}


//============================================================================
//	updateTransMatrix
//
//	�ʒu���W�X�V
static void updateTransMatrix(void)
{
	float ratio;

	ratio = pFw->stat.FontRatio;
	if(0 != pFw->stat.FontRatioSw)
	{
		ratio = ratio * (float)pFw->stat.FontRatioSw/100.0f;
	}

	if(0 != pFw->transmodified)
	{
		pFw->transmodified = 0;

		sceVu0UnitMatrix(pFw->trans);
		pFw->trans[0][0] = ratio;
		pFw->trans[1][1] = ratio * pFw->stat.TvFontRatioY;

		pFw->trans[3][0] = pFw->stat.FontPitch * pFw->stat.FontRatio;
		pFw->trans[3][1] = pFw->MaxAscent * pFw->stat.FontRatio * pFw->stat.TvFontRatioY;
	}

	scePFontSetPitch(pFw->fd, pFw->stat.FontPitch * ratio);
	scePFontSetFontMatrix(pFw->fd, (sceVu0FMATRIX const*)&pFw->trans);
}


//============================================================================
//	updateScreenMatrix
//
//	�X�N���[�����W�X�V
static void updateScreenMatrix(void)
{
	sceVu0UnitMatrix(pFw->screen);
	pFw->screen[3][0] = 2048.0f - (float)(SCREEN_WIDTH / 2);
	pFw->screen[3][1] = 2048.0f - (float)(SCREEN_HEIGHT / 2);
	scePFontSetScreenMatrix(pFw->fd, (sceVu0FMATRIX const*)&pFw->screen);
}
