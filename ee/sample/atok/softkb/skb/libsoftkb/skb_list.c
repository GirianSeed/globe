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

//	kigo.bin ��[Capslock],[��/�S]�̃{�^���͏�ɖ����Ȃ̂Ńf�[�^�ɂ���Ė����ɂ���Ă��܂��B

#include <sys/types.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <eekernel.h>
#include <eetypes.h>
#include <sifdev.h>
#include <libpkt.h>
#include <libccc.h>
#include <libpad.h>

#include "libsoftkb.h"
#include "sharedskb.h"

// �`��
#define CHAR_STARTDRAW_POSX		(22)		// �����`��G���A����̕����`��J�nX�ʒu
#define CHAR_STARTDRAW_POSY		(2)			// �����`��G���A����̕����`��J�nY�ʒu
#define CHAR_TOPLEFT_POSX		(10)		// �\�t�g�E�F�A�L�[�{�[�h�̍��ォ���X�ʒu
#define CHAR_TOPLEFT_POSY		(7)			// �\�t�g�E�F�A�L�[�{�[�h�̍��ォ���Y�ʒu
#define TEXTPOS_ADDY			(8)			// ������

// �{�^��
#define BUTTON_CTRL_GLOBALPOSX	(16)		// �R���g���[�������̃{�^��
#define BUTTON_CTRL_GLOBALNUMV	(5)			// �R���g���[�������̃A�C�e����
#define BUTTON_HELP_GLOBALPOSX	(17)		// �w���v�����̃{�^��
#define BUTTON_HELP_GLOBALNUMV	(1)			// �w���v��̃A�C�e����
#define DEFAULT_FOCUS_X			(0)			// �f�t�H���g �t�H�[�J�XX�l
#define DEFAULT_FOCUS_Y			(0)			// �f�t�H���g �t�H�[�J�XY�l
#define FOCUS_MARGINX			(0)			// �t�H�[�J�X�}�[�W��X�l
#define FOCUS_MARGINY			(1)			// �t�H�[�J�X�}�[�W��Y�l

// SCROLL
#define REPEAT_START_TIME		(20)		// ���s�[�g�J�n����
#define LIST_SCROLL_SPEED4		(4)			// �X�N���[���X�s�[�h 4
#define LIST_SCROLL_SPEED2		(2)			//                    2
#define LIST_SCROLL_SPEED1		(1)			//                    1
#define LIST_SCROLL_START		(30)		// �X�N���[������܂ł̎���(�E�F�C�g)
#define LIST_ACCEL_MAX			(200)		// �A�N�Z���ő�l
#define LIST_ACCEL_50			(50)		// �A�N�Z��  50
#define LIST_ACCEL_100			(100)		// �A�N�Z�� 100
#define LIST_ACCEL_150			(150)		// �A�N�Z�� 150
#define LIST_ACCEL_200			(200)		// �A�N�Z�� 200

// �v���g�^�C�v�錾
extern gImport_t		*g_pSkbImport;		// ���C�����֐��Q�̃|�C���^
extern protectedSkb_t	g_protectSkb;		// 


// �g�O��
#define TOGGLE_SP_OFFX	(10)
#define TOGGLE_SP_OFFY	(-4)
#define TOGGLE_SP_W		(13)
#define TOGGLE_SP_H		(26)
uv_t		g_uvToggleSrc = {STG(169),	STG(208),	STG(13),	STG(26)};
sprite_t	g_spToggleSrc = {TOGGLE_SP_OFFX, TOGGLE_SP_OFFY, 0, TOGGLE_SP_W, TOGGLE_SP_H};


void setFocus(skey_t *pKey, int x, int y);
static void pushKey(skey_t *pKey);

static int readResource(skey_t *pKey, const char *filename, u_int keyTopNo, inhibitChar_t *inhibitChar, void *pCharCodeBlock);
static void makePackets_listBody(sceVif1Packet *pk, skey_t *pKey,
				 textureInfo_t *pTexBtn, textureInfo_t *pTexChar, textureInfo_t *pTexBg);
static void makePackets_cursor(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pTexCur);
static void makeChars(sceVif1Packet *pk, skey_t *pKey);
int setSpriteCtrl(skey_t *pKey, u_short ctrlCode, sprite_t *pSp);
static void makeToggle(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pTex);


static void ctrlPad(skey_t *pKey, u_short down, u_short up, u_short press, u_short repeat);
static void padDown(skey_t *pKey, u_short down, u_short press, u_short repeat);
static void padUp(skey_t *pKey, u_short up);
static void scrollCheck(skey_t *pKey);
static int  scrollChars(skey_t *pKey, u_short press);
static void moveCursorList(skey_t *pKey, int dx, int dy);
static int  addChar(skey_t *pKey);

static void initKeyTop(skey_t *pKey);

static void loadUtf8Cache(skey_t *pKey, int startLine, int lineNum);
static void shiftUtf8Cache(skey_t *pKey, int shift);
static UTF8Char_t getUtf8CharFromPos(skey_t *pKey, int x, int y);
static void getSJISCharFromPos(skey_t *pKey, int x, int y, char *pBufSjis);
static inline int checkIgnoreSJISCode(u_int code);

static void moveMouseCursor(skey_t *pKey, iPoint_t *pPoint);
static int searchListByPoint(skey_t *pKey, iPoint_t *pPt);
static void leftButtonDown_list(skey_t *pKey, iPoint_t *pPoint);
static void leftButtonUp_list(skey_t *pKey, iPoint_t *pPoint);
int searchDummyKeyByIndex(skey_t *pKey, int index);
static u_int searchFocus(skey_t *pKeyNext, sprite_t *pNowSp);

static void open(skey_t *pKey);
static void close(skey_t *pKey);

//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//  SkbList_Init											ref:  skb_core.h
//
//  �ʏ�\�t�g�E�F�A�L�[�{�[�h������
//  �����̒񋟂ł���֐���o�^
//
//  exportSkb_t *pExport : �񋟊֐���o�^����ׂ̃|�C���^
//
//  �߂�l �F void
void SkbList_Init(exportSkb_t *pExport)
{
	SKB_ASSERT(pExport);

	// �g����֐��|�C���^��ݒ�
	pExport->readResource       = readResource;
	pExport->makePacketsSkbBody = makePackets_listBody;
	pExport->makePacketsCursor  = makePackets_cursor;

	pExport->setPad   = ctrlPad;
	pExport->mouseMove        = moveMouseCursor;
	pExport->mouseLButtonDown = leftButtonDown_list;
	pExport->mouseLButtonUp   = leftButtonUp_list;

	pExport->initKeyTop      = initKeyTop;

	pExport->searchFocus = searchFocus;


	pExport->open  = open;
	pExport->close = close;
}


//============================================================================
// STATIC FUNCTIONS
//============================================================================
//============================================================================
//  readResourceFrame
//
//  �L�[�z���`���\�[�X�Ǎ���
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	const char *filename : �L�[�z���`���\�[�X�t�@�C����
//	u_int keyTopNo : �L�[�g�b�v�̔ԍ�
//	inhibitChar_t *inhibitChar : ���͋֎~������
//	void *pTmp : �\��������
//
//	�߂�l : int : 0:����/1:�G���[
static int readResource(skey_t *pKey, const char *filename, u_int keyTopNo, inhibitChar_t *inhibitChar, void *pCharCodeBlock)
{

	SKB_ASSERT(pKey);
	SKB_ASSERT(filename);
	SKB_ASSERT(inhibitChar);
	SKB_ASSERT(pCharCodeBlock);

	memset(&pKey->List, 0, sizeof(skblist_t));

	pKey->List.pListItem = (charCodeBlock_t*)pCharCodeBlock;
	pKey->List.iUtf8CacheTopLine = -1;

	pKey->uiCursorPos = 0;

	readResourceFrame(pKey, filename, keyTopNo, inhibitChar, pKey->List.pListItem);

	// �sNO->�����R�[�h�ɕύX
	{
		int i=0, j=0;
		int line = 0;
		while(pKey->List.pListItem[i].start != 0x0000 ||
			  pKey->List.pListItem[i].end != 0x0000)
		{
			j = 0;
			for(j=pKey->List.pListItem[i].start; j<pKey->List.pListItem[i].end; j+=VISIBLE_CHARNUM_H)
			{
				pKey->List.auiCodeTable[line] = j;
				line++;
			}

			i++;
		}
		pKey->List.iLineNum = line;
	}


	setFocus(pKey, 0, 0);

	return 0;
}


//============================================================================
//	makePackets_listBody
//
//  �\�t�g�E�F�A�L�[�{�[�h���X�g�`��p�P�b�g�쐬
//
//	sceVif1Packet *pk : VIF�p�P�b�g�i�[�p�o�b�t�@�ւ̃|�C���^
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	textureInfo_t *pTexBtn : �{�^���e�N�X�`���ւ̃|�C���^
//	textureInfo_t *pTexChar: �����e�N�X�`���ւ̃|�C���^
//	textureInfo_t *pTexBg  : �w�i�e�N�X�`���ւ̃|�C���^
static void makePackets_listBody(sceVif1Packet *pk, skey_t *pKey,
				 textureInfo_t *pTexBtn, textureInfo_t *pTexChar, textureInfo_t *pTexBg)
{
	SKB_ASSERT(pk);
	SKB_ASSERT(pKey);
	SKB_ASSERT(pTexBtn);
	SKB_ASSERT(pTexChar);
//	SKB_ASSERT(pTexBg);			// BG���v��Ȃ��ꍇ������


	makePacketsBody(pk, pKey, pTexBtn, pTexChar, pTexBg);	// �{�f�B�����`��
	makeToggle(pk, pKey, pTexBtn);
	makeChars(pk, pKey);									// �����`��
}


//============================================================================
//	makeToggle
//
//	�g�O���{�^���`��p�P�b�g�쐬
//
//	sceVif1Packet *pk : VIF�p�P�b�g�i�[�p�o�b�t�@�ւ̃|�C���^
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	textureInfo_t *pTexBtn : �g�O���e�N�X�`���ւ̃|�C���^
static void makeToggle(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pTex)
{
	int col;
	if(pKey->List.iGlobalCurPosX != BUTTON_CTRL_GLOBALPOSX &&
	   pKey->List.iGlobalCurPosX != BUTTON_HELP_GLOBALPOSX)
	{
		//�g�O���ړ�
		int scrlY  = pKey->List.iScrollPosY;
		int charH  = g_pSkbImport->skb.uiListFontH;
		int posY16 = (((g_pSkbImport->thinkYH16(charH)*VISIBLE_CHARNUM_V)*scrlY)/(pKey->List.iLineNum-VISIBLE_CHARNUM_V));

		pKey->List.iOldPosY16 = posY16;

		pKey->List.spToggle16.x = g_pSkbImport->thinkXW16(g_spToggleSrc.x) + pKey->uiKbdPosX;
		pKey->List.spToggle16.y = g_pSkbImport->thinkYH16(g_spToggleSrc.y) + pKey->List.iOldPosY16 + pKey->uiKbdPosY;
		pKey->List.spToggle16.z = 0;
		pKey->List.spToggle16.w = g_pSkbImport->thinkXW16(g_spToggleSrc.w);
		pKey->List.spToggle16.h = g_pSkbImport->thinkYH16(g_spToggleSrc.h);

		pKey->List.iOldPosY16 = posY16;
		g_protectSkb.uiTogglePos16 = pKey->List.spToggle16.y;		// ��U�g�O���̈ʒu��ޔ�
	}
	else
	{
		pKey->List.spToggle16.x = g_pSkbImport->thinkXW16(g_spToggleSrc.x) + pKey->uiKbdPosX;
		pKey->List.spToggle16.y = g_pSkbImport->thinkYH16(g_spToggleSrc.y) + pKey->List.iOldPosY16 + pKey->uiKbdPosY;
		pKey->List.spToggle16.z = 0;
		pKey->List.spToggle16.w = g_pSkbImport->thinkXW16(g_spToggleSrc.w);
		pKey->List.spToggle16.h = g_pSkbImport->thinkYH16(g_spToggleSrc.h);
	}

	col = affectAlpha(0x80808080, 0x80);
	g_pSkbImport->loadTex(pk, pTex);

	g_pSkbImport->drawTex(pk, col, &pKey->List.spToggle16, &g_uvToggleSrc, pTex);
}


//============================================================================
//	refreshUtf8Cache
//
//	UTF8�ɕϊ����ꂽ�����R�[�h�L���b�V�����X�V
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
static void refreshUtf8Cache(skey_t *pKey)
{
	int iTopLine = pKey->List.iScrollPosY - pKey->List.iUtf8CacheTopLine;

	if(pKey->List.iUtf8CacheTopLine == -1)
	{
		pKey->List.iUtf8CacheTopLine = pKey->List.iScrollPosY;
		loadUtf8Cache(pKey, pKey->List.iUtf8CacheTopLine, VISIBLE_CHARNUM_V);
	}
	else if(iTopLine > 0)
	{
		shiftUtf8Cache(pKey, iTopLine);
		pKey->List.iUtf8CacheTopLine = pKey->List.iScrollPosY;
		loadUtf8Cache(pKey, pKey->List.iUtf8CacheTopLine+VISIBLE_CHARNUM_V-iTopLine, iTopLine);
	}
	else if(iTopLine < 0)
	{
		shiftUtf8Cache(pKey, iTopLine);
		pKey->List.iUtf8CacheTopLine = pKey->List.iScrollPosY;
		loadUtf8Cache(pKey, pKey->List.iUtf8CacheTopLine, -iTopLine);
	}
}


//============================================================================
//	makeChars
//
//	������`��
//
//	sceVif1Packet *pk : VIF�p�P�b�g�i�[�p�o�b�t�@�ւ̃|�C���^
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
static void makeChars(sceVif1Packet *pk, skey_t *pKey)
{
	// UTF8�L���b�V�����X�V
	refreshUtf8Cache(pKey);

	// �����p�P�b�g�쐬
	{
		int x, y;
		int globalX, globalY;
		int charPacketLocalPosX, charPacketLocalPosY;
		int centeringOffsetX;
		int charW = g_pSkbImport->skb.uiListFontW;
		int charH = g_pSkbImport->skb.uiListFontH;
		int dx, dy;
		int cx, cy;
		UTF8Char_t utf8Char;

		cx = g_pSkbImport->thinkXW(pKey->uiKbdPosX) + CHAR_STARTDRAW_POSX + CHAR_TOPLEFT_POSX;							// ���������n��OFFSET X
		cy = g_pSkbImport->thinkYH(pKey->uiKbdPosY) + g_pSkbImport->thinkYH(CHAR_STARTDRAW_POSY + CHAR_TOPLEFT_POSY);	// ���������n��OFFSET Y
		g_pSkbImport->skb.setListFontEnv(cx, cy);		// �����̏����ݒ������

		for(y=0; y<VISIBLE_CHARNUM_V; y++)
		{
			for(x=0; x<VISIBLE_CHARNUM_H; x++)
			{
				globalX = x;
				globalY = y + pKey->List.iScrollPosY;
				charPacketLocalPosX = (x * charW) + cx;
				charPacketLocalPosY = (y * charH) + cy;

				utf8Char = pKey->List.utf8Cache[(y * VISIBLE_CHARNUM_H) + x].utf8Char;
				charPacketLocalPosY = g_pSkbImport->thinkYHn(charPacketLocalPosY);

				centeringOffsetX = (charW - pKey->List.utf8Cache[(y * VISIBLE_CHARNUM_H) + x].iFontWidth) >> 1;
				dx = charPacketLocalPosX + centeringOffsetX;
				dy = charPacketLocalPosY + g_pSkbImport->thinkYHn(TEXTPOS_ADDY);

				if(globalX == pKey->List.iGlobalCurPosX &&
				   globalY == pKey->List.iGlobalCurPosY)
				{
					sprite_t sp16;
					g_pSkbImport->skb.setListDrawStringF(pk, utf8Char.buf, dx, dy);

					// �t�H�[�J�X�X�v���C�g�ݒ�
					if(!strlen(utf8Char.buf))
					{
						// ����������������X�̈ʒu���������̔����̈ʒu�ɂȂ��Ă��܂��̂�
						// �������̔��������������l�ɂ���
						sp16.x = g_pSkbImport->thinkXW16(dx+FOCUS_MARGINX) - (g_pSkbImport->thinkXW16(charW)/2);
					}
					else
					{
						// ����́A���ʂɏ����ׁANTSC/PAL��FRAME/FIELD���l���Ȃ���΂Ȃ�Ȃ��B
						sp16.x = g_pSkbImport->thinkXW16(dx+FOCUS_MARGINX);
					}
					sp16.y = (dy<<4) + g_pSkbImport->thinkYH16(FOCUS_MARGINY);
					sp16.z = 0;
					sp16.w = g_pSkbImport->thinkXW16(charW);
					sp16.h = g_pSkbImport->thinkYH16(charH);
					makeCursorSprite(&sp16, pKey->spCursor, pKey->iCursorPos);
				}
				else
				{
					g_pSkbImport->skb.setListDrawString(pk, utf8Char.buf, dx, dy);
				}
			}
		}
	}
}


//============================================================================
//	loadUtf8Cache
//
//	UTF8�ɕϊ����ꂽ��������L���b�V���Ƀ��[�h
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	int startLine : ���[�h���n�߂̍s�ԍ�
//	int lineNum  : ���[�h����s��
static void loadUtf8Cache(skey_t *pKey, int startLine, int lineNum)
{
	int count = 0;
	int line, i;
	int topLine = pKey->List.iUtf8CacheTopLine;
	char *pBuf = NULL;

	for(line=startLine; line<(startLine+lineNum); line++)
	{
		for(i=0; i<VISIBLE_CHARNUM_H; i++)
		{
			pKey->List.utf8Cache[(line - topLine) * VISIBLE_CHARNUM_H + i].utf8Char = getUtf8CharFromPos(pKey, i, line);
			pBuf = pKey->List.utf8Cache[(line - topLine) * VISIBLE_CHARNUM_H + i].utf8Char.buf;
			pKey->List.utf8Cache[(line - topLine) * VISIBLE_CHARNUM_H + i].iFontWidth = g_pSkbImport->skb.getStringWidth(pBuf);
			count++;
		}
	}
}


//============================================================================
//	shiftUtf8Cache
//
//	UTF8�ɕϊ����ꂽ�����L���b�V�����ړ�
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	int shift : �ړ���
static void shiftUtf8Cache(skey_t *pKey, int shift)
{
	if(shift > 0)
	{
		int i, j;
		for(i=0; i<VISIBLE_CHARNUM_V-shift; i++)
		{
			for(j=0; j<VISIBLE_CHARNUM_H; j++)
			{
				pKey->List.utf8Cache[i * VISIBLE_CHARNUM_H + j] = pKey->List.utf8Cache[(i+shift) * VISIBLE_CHARNUM_H + j];
			}
		}
	}
	else if(shift < 0)
	{
		int i, j;
		for(i=VISIBLE_CHARNUM_V-1; i>=(-shift); i--)
		{
			for(j=0; j<VISIBLE_CHARNUM_H; j++)
			{
				pKey->List.utf8Cache[i * VISIBLE_CHARNUM_H + j] = pKey->List.utf8Cache[(i+shift) * VISIBLE_CHARNUM_H + j];
			}
		}
	}
}


//============================================================================
//	getUtf8CharFromPos
//
//	��ʏ��X,Y�l����UTF8�̕����R�[�h���擾
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	int x : X�l
//	int y : Y�l
//
//	�߂�l : UTF8Char_t : �������
static UTF8Char_t getUtf8CharFromPos(skey_t *pKey, int x, int y)
{
	UTF8Char_t ret;
	char acBufSjis[4];
	u_short code = pKey->List.auiCodeTable[y] + x;

	if(checkIgnoreSJISCode(code))
	{
		acBufSjis[0] = '\0';
	}
	else
	{
		if(code < 0x00ff)
		{
			// ���p����
			acBufSjis[0] = (char)code;
			acBufSjis[1] = '\0';
		}
		else
		{
			// �S�p����
			acBufSjis[0] = ((char*)&code)[1];
			acBufSjis[1] = ((char*)&code)[0];
			acBufSjis[2] = '\0';
		}
	}

	sceCccSJIStoUTF8(ret.buf, sizeof(ret.buf), acBufSjis);
	return ret;
}


//============================================================================
//	getSJISCharFromPos
//
//	�ʒu��񂩂�ASJIS�����R�[�h���擾
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	int x : X�l
//	int y : Y�l
//	char *pBufSjis : SJIS�����R�[�h�i�[�p�o�b�t�@
static void getSJISCharFromPos(skey_t *pKey, int x, int y, char *pBufSjis)
{
	u_short code = pKey->List.auiCodeTable[y] + x;
	if(checkIgnoreSJISCode(code))
	{
		pBufSjis[0] = '\0';
	}
	else
	{
		if(code < 0x00ff)
		{
			// ���p����
			pBufSjis[0] = (char)code;
			pBufSjis[1] = '\0';
		}
		else
		{
			// �S�p����
			pBufSjis[0] = ((char*)&code)[1];
			pBufSjis[1] = ((char*)&code)[0];
			pBufSjis[2] = '\0';
		}
	}
}


//============================================================================
//	checkIgnoreSJISCode
//
//	��ʓI�ȃt�H���g�ɓ����Ă�/���Ȃ��`�F�b�N
//
//	u_int code : �`�F�b�N�p�����R�[�h
//
//	�߂�l : int : 0:�����Ă��Ȃ�/1:�����Ă���
static inline int checkIgnoreSJISCode(u_int code)
{
	if( code >= 0x81ad  &&  code <= 0x81b7 ){ return 1; }
	if( code >= 0x81c0  &&  code <= 0x81c7 ){ return 1; }
	if( code >= 0x81cf  &&  code <= 0x81d9 ){ return 1; }
	if( code >= 0x81e9  &&  code <= 0x81ef ){ return 1; }
	if( code >= 0x81f8  &&  code <= 0x81fb ){ return 1; }
	if( code >= 0x81fd  &&  code <= 0x81ff ){ return 1; }

	if( code >= 0x8240  &&  code <= 0x824e ){ return 1; }
	if( code >= 0x8259  &&  code <= 0x825f ){ return 1; }
	if( code >= 0x827a  &&  code <= 0x8280 ){ return 1; }
	if( code >= 0x829b  &&  code <= 0x829e ){ return 1; }
	if( code >= 0x82f2  &&  code <= 0x82ff ){ return 1; }

	if( code >= 0x8397  &&  code <= 0x839e ){ return 1; }
	if( code >= 0x83b7  &&  code <= 0x83be ){ return 1; }
	if( code >= 0x83d7  &&  code <= 0x83ff ){ return 1; }

	if( code >= 0x8461  &&  code <= 0x846f ){ return 1; }
	if( code >= 0x8492  &&  code <= 0x849e ){ return 1; }
	if( code >= 0x84bf  &&  code <= 0x86ff ){ return 1; }

	if( code == 0x875e ){ return 1; }
	if( code >= 0x8776  &&  code <= 0x877d ){ return 1; }
	if( code >= 0x8780  &&  code <= 0x8781 ){ return 1; }
	if( code >= 0x8790  &&  code <= 0x8792 ){ return 1; }
	if( code >= 0x8794  &&  code <= 0x879f ){ return 1; }

	if( code >= 0x8890  &&  code <= 0x889e ){ return 1; }

	return 0;
}



//============================================================================
//  ctrlPad
//
//	�p�b�h����
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	u_short down : �{�^�������n�ߏ��
//	u_short up   : �{�^���������
//	u_short press : �{�^���̉������
//	u_short repeat: �{�^���̃��s�[�g���
static void ctrlPad(skey_t *pKey, u_short down, u_short up, u_short press, u_short repeat)
{
	SKB_ASSERT(pKey);

	padDown(pKey, down, press, repeat);
	padUp(pKey, up);
}


//============================================================================
//	padDown
//
//	�p�b�h��������
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	u_short down : �{�^�������n�ߏ��
//	u_short press : �{�^���̉������
//	u_short repeat: �{�^���̃��s�[�g���
static void padDown(skey_t *pKey, u_short down, u_short press, u_short repeat)
{
	SKB_ASSERT(pKey);

	if(g_pSkbImport->isConv())
	{
		// �ϊ����̎��́A����ł��Ȃ�
		return;
	}

	if(down & SCE_PADselect)
	{
		pKey->uiPush = 0;								// �{�^�������t���O��RELEASE�ɂ���
		g_pSkbImport->skb.keytopChange(SKBMODE_NOHIT);	// �L�[�g�b�v�ύX�p�R�[���o�b�N�Ăяo��
		return;
	}

	// �{�^�������n�ߏ��
	if(down & SCE_PADL1)
	{
	}
	if((down & SCE_PADL2) || (press & SCE_PADL2))
	{
		onStateCtrlCode(pKey, SKBCTRL_LEFT, KEY_PUSH);
	}

	if((down & SCE_PADR1) || (press & SCE_PADR1))
	{
		pushShift(pKey, 0);
	}

	if((down & SCE_PADR2) || (press & SCE_PADL2))
	{
		onStateCtrlCode(pKey, SKBCTRL_RIGHT, KEY_PUSH);
	}
	if((down & SCE_PADRup) || (press & SCE_PADRup))
	{
		u_int uiTmp[] = {0x00003000, 0x00000000};
		onStateKey(pKey, uiTmp, KEY_PUSH);
	}
	if((down & SCE_PADRdown) || (press & SCE_PADRdown)){}
	if((down & SCE_PADRleft) || (press & SCE_PADRleft))
	{
		onStateCtrlCode(pKey, SKBCTRL_BACKSPACE, KEY_PUSH);
	}
	if(down & SCE_PADstart)
	{
		#if 0//SKB_EX
		if(g_pSkbImport->skb.getAtokInput())
		#else
		if( g_pSkbImport->isConv() )
		#endif
		{
			// ATOK�œ��͂�����Ȃ�S���m���ݒ�
			g_pSkbImport->skb.kakuteiAll();
			#if 1//SKB_EX Sound
			SoftKB_se_play(SKBSE_DETECTEXIT);
			#endif
		}
		else
		{
			pushEnter(pKey);
		}
		onStateCtrlCode(pKey, SKBCTRL_ENTER, KEY_PUSH);
		g_pSkbImport->skb.exitOK();						// EXIT
		return;
	}
	// �{�^���̃��s�[�g���
	if(repeat & SCE_PADL1){}
	if(repeat & SCE_PADL2)
	{
		pushLeftArrow(pKey);	// ��
	}
	if(repeat & SCE_PADR1){}
	if(repeat & SCE_PADR2)
	{
		pushRightArrow(pKey);	// ��
	}
	if(repeat & SCE_PADRup)
	{
		pushSpace(pKey);		// SPACE
	}
	if(repeat & SCE_PADRdown)
	{
		pushEsc(pKey);			// ESC
	}
	if(repeat & SCE_PADRleft)
	{
		pushBackSpace(pKey);	// BACKSPACE
	}
	if(repeat & SCE_PADRright)
	{
		pushKey(pKey);
		pKey->uiPush = 1;		// �J�[�\���������t���O��ON
	}
	if(repeat & SCE_PADselect){}
	if(repeat & SCE_PADstart){}

	// �J�[�\���ړ�
	{
		int dx=0, dy=0;
		u_short updown=0;

		if(repeat & SCE_PADLup)
		{
			dy -= 1;
		}
		if(repeat & SCE_PADLdown)
		{
			dy += 1;
		}
		updown = scrollChars(pKey, press);

		// ���������Â������͂���ɑ����Ȃ�
		// 2��J�[�\��������
		if(pKey->List.iRepeatAccel >= LIST_ACCEL_150 &&
		   (pKey->List.iGlobalCurPosX != BUTTON_CTRL_GLOBALPOSX) &&
		   (pKey->List.iGlobalCurPosX != BUTTON_HELP_GLOBALPOSX))
		{
			if(updown & SCE_PADLup)
			{
				dy -= 1;
			}
			if(updown & SCE_PADLdown)
			{
				dy += 1;
			}
		}

		if(repeat & SCE_PADLleft)
		{
			dx -= 1;
		}
		if(repeat & SCE_PADLright)
		{
			dx += 1;
		}

		moveCursorList(pKey, dx, dy);
	}
}


//============================================================================
//	addChar
//
//	�������I�����ꂽ�̂ŕ����R�[�h�𑗂�
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//
//	�߂�l: int : 0:����/1:�G���[
static int addChar(skey_t *pKey)
{
	char bufSjis[4];
	getSJISCharFromPos(pKey, pKey->List.iGlobalCurPosX, pKey->List.iGlobalCurPosY, bufSjis);

	{
		// SJIS -> UTF8
		UTF8Char_t utf8Char = getUtf8CharFromPos(pKey, pKey->List.iGlobalCurPosX, pKey->List.iGlobalCurPosY);
		if(g_pSkbImport->skb.getStringWidth(utf8Char.buf) <= 0)
		{
			g_pSkbImport->dprintf("ERROR : addChar - invalid char\n");
			return 1;
		}
	}


	if(bufSjis[0] == 0x00)
	{
		// �������Ȃ�
		return 1;
	}

	// SJIS -> UCS4
	{
		sceCccJISCS jis;
		sceCccUCS4 chr=0;
		sceCccSJISTF const *pSjis = (sceCccSJISTF *)bufSjis;
		u_int iBuf[4];
		int cnt=0, i=0;

		while(*pSjis)
		{
			// SJIS -> UCS4
			jis = sceCccDecodeSJIS(&pSjis);
			iBuf[i] = sceCccJIStoUCS(jis, chr);
			cnt++;
		}

		g_pSkbImport->skb.setChars(iBuf, ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), 0);
	}

	return 0;
}


//============================================================================
//	makePackets_cursor
//
//	�J�[�\���`��p�P�b�g�쐬
//
//	sceVif1Packet *pk : VIF�p�P�b�g�i�[�p�|�C���^
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	textureInfo_t *pTexCur : �J�[�\���e�N�X�`���ւ̃|�C���^
static void makePackets_cursor(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pTexCur)
{
	sprite_t sp16;
	pKey->uiCursorPos = -1;
	if(pKey->List.iGlobalCurPosX == BUTTON_HELP_GLOBALPOSX)
	{
		setSpriteCtrl(pKey, SKBCTRL_HELP, &sp16);
		makeCursorSprite(&sp16, pKey->spCursor, pKey->iCursorPos);
		pKey->uiCursorPos = 1;
	}
	else if(pKey->List.iGlobalCurPosX == BUTTON_CTRL_GLOBALPOSX)
	{
		int iY = pKey->List.iGlobalCurPosY - pKey->List.iScrollPosY;
		switch(iY)
		{
		case 0:
			setSpriteCtrl(pKey, SKBCTRL_BACKSPACE, &sp16);
			pKey->uiCursorPos = 0;
			break;
		case 1:
			setSpriteCtrl(pKey, SKBCTRL_ENTER, &sp16);
			pKey->uiCursorPos = 2;
			break;
		case 3:
			if(setSpriteCtrl(pKey, SKBCTRL_SHIFT_L, &sp16))
			{
				setSpriteCtrl(pKey, SKBCTRL_SHIFT_R, &sp16);
			}
			pKey->uiCursorPos = 3;
			break;
		}
		makeCursorSprite(&sp16, pKey->spCursor, pKey->iCursorPos);
	}

	{
		int i;
		int col = affectAlpha(0x80808080, pKey->ucAlpha[SKBALPHA_CURSOR]);

		// �`��
		g_pSkbImport->loadTex(pk, pTexCur);
		for(i=0; i<4; i++)
		{
			g_pSkbImport->drawTex(pk, col, &pKey->spCursor[i], (uv_t*)&g_uvCursors[i], pTexCur);
		}
	}

}


//============================================================================
//	setSpriteCtrl
//
//	�R���g���[���R�[�h�p�̃X�v���C�g��ݒu
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	u_short ctrlCode : �R���g���[���R�[�h
//	sprite_t *pSp : �X�v���C�g�i�[�p�|�C���^
//
//	�߂�l : int : 0:����/1:�Y������R���g���[���R�[�h����������
int setSpriteCtrl(skey_t *pKey, u_short ctrlCode, sprite_t *pSp)
{
	int i, index = 0;
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	SKB_ASSERT(pKey);

	for(i=0; i<pKey->uiBtnNum; i++)
	{
		if(ctrlCode == pKey->pBtns[i].state[ucShift].ctrlCode)
		{
			index = pKey->pBtns[i].spBaseIndex;
			memcpy(pSp, &pKey->pSps[index], sizeof(sprite_t));
			return 0;
		}
	}

	return 1;
}


//============================================================================
//	pushKey
//
//	�L�[���������ꂽ�Ƃ��̏���
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
static void pushKey(skey_t *pKey)
{
	if(pKey->List.iGlobalCurPosX == BUTTON_HELP_GLOBALPOSX)
	{
		g_pSkbImport->skb.showHelp(1);
		g_pSkbImport->skb.ucShowHelp = 1;
		pKey->uiCursorPos = 1;
		onStateCtrlCode(pKey, SKBCTRL_HELP, KEY_PUSH);
		releaseShift(pKey, 1);
	}
	else if(pKey->List.iGlobalCurPosX == BUTTON_CTRL_GLOBALPOSX)
	{
		int iY = pKey->List.iGlobalCurPosY - pKey->List.iScrollPosY;
		switch(iY)
		{
		case 0:
			pushBackSpace(pKey);
			pKey->uiCursorPos = 0;
			break;
		case 1:
			pushEnter(pKey);
			pKey->uiCursorPos = 2;
			break;
		case 3:
			pushShift(pKey, 1);
			pKey->uiCursorPos = 3;
			break;
		}
	}
	else
	{
		// �����`��̈� ��
		// ��������
		addChar(pKey);
		releaseShift(pKey, 1);
	}
}


//============================================================================
//	releaseKey
//
//	�L�[���������ꂽ�Ƃ��̏���
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
static void releaseKey(skey_t *pKey)
{
	if(pKey->List.iGlobalCurPosX == BUTTON_HELP_GLOBALPOSX)
	{
		offStateCtrlCode(pKey, SKBCTRL_HELP, KEY_PUSH);
	}
	else if(pKey->List.iGlobalCurPosX == BUTTON_CTRL_GLOBALPOSX)
	{
		int iY = pKey->List.iGlobalCurPosY - pKey->List.iScrollPosY;
		switch(iY)
		{
		case 0:
			break;
		case 1:
			break;
		case 3:
			releaseShift(pKey, 0);
			break;
		}
	}

	pKey->uiCursorPos = -1;	// �J�[�\���𖳌��ɂ���
	pKey->uiPush = 0;		// �J�[�\���������t���O��OFF
}


//============================================================================
//	setFocus
//
//	�t�H�[�J�X(�J�[�\��)�ʒu�ݒ�
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//
//	int x : X�����̒l
//	int y : Y�����̒l
void setFocus(skey_t *pKey, int x, int y)
{
	pKey->List.iGlobalCurPosY = y;
	pKey->List.iGlobalCurPosX = x;

	if(pKey->List.iGlobalCurPosX > (VISIBLE_CHARNUM_H-1))
	{
		pKey->List.iGlobalCurPosX = (VISIBLE_CHARNUM_H-1);
	}
	if(pKey->List.iGlobalCurPosY > (VISIBLE_CHARNUM_V-1))
	{
		pKey->List.iGlobalCurPosY = (VISIBLE_CHARNUM_V-1);
	}
}


//============================================================================
//	scrollCheck
//
//	�X�N���[���ő�/�ŏ��`�F�b�N
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
static void scrollCheck(skey_t *pKey)
{
	int iLY = pKey->List.iGlobalCurPosY - pKey->List.iScrollPosY;

	if(iLY < 0)
	{
		pKey->List.iScrollPosY -= 0 - iLY;
	}
	if(iLY > (VISIBLE_CHARNUM_V-1))
	{
		pKey->List.iScrollPosY += iLY - (VISIBLE_CHARNUM_V-1);
	}
}


//============================================================================
//	moveCursorList
//
//	�J�[�\���ړ�
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	int dx : �ړ���X�l
//	int dy : �ړ���Y�l
static void moveCursorList(skey_t *pKey, int dx, int dy)
{
	int iY = 0;
	u_int maxX[] = {17, 16, 16, 16, 15, 15, 15, 15};	// maxX[4],maxX[5]�͏��DISABLE��ԂȂ̂ŃJ�E���g���Ȃ�

	if(dy)
	{
		// Y�����̒l��ύX
		if(dy > 0)
		{
			// �������̈ړ�
			if(pKey->List.iGlobalCurPosX == BUTTON_HELP_GLOBALPOSX)
			{
				// �ω�����
			}
			else if(pKey->List.iGlobalCurPosX == BUTTON_CTRL_GLOBALPOSX)
			{
				// �R���g���[���{�^��
				// �����́A�A�N�Z���Ƃ��֌W�Ȃ�
				pKey->List.iGlobalCurPosY += 1;
				iY = pKey->List.iGlobalCurPosY - pKey->List.iScrollPosY;
				if(iY == 2)
				{
					pKey->List.iGlobalCurPosY = pKey->List.iScrollPosY + 3;
				}
				else if(iY == 4)
				{
					pKey->List.iGlobalCurPosY = pKey->List.iScrollPosY;
				}
			}
			else
			{
				pKey->List.iGlobalCurPosY += dy;
			}
		}
		else
		{
			// �������̈ړ�
			if(pKey->List.iGlobalCurPosX == BUTTON_HELP_GLOBALPOSX)
			{
				// �ω�����
			}
			else if(pKey->List.iGlobalCurPosX == BUTTON_CTRL_GLOBALPOSX)
			{
				// �R���g���[���{�^��
				pKey->List.iGlobalCurPosY -= 1;
				iY = pKey->List.iGlobalCurPosY - pKey->List.iScrollPosY;
				if(iY == 2)
				{
					pKey->List.iGlobalCurPosY = pKey->List.iScrollPosY + 1;
				}
				else if(iY == -1)
				{
					pKey->List.iGlobalCurPosY = pKey->List.iScrollPosY + 3;
				}
			}
			else
			{
				pKey->List.iGlobalCurPosY += dy;
			}
		}

		if(pKey->List.iGlobalCurPosY < 0)
		{
			pKey->List.iGlobalCurPosY = 0;
			pKey->List.iScrollPosY = 0;
			pKey->List.iRepeatAccel = 0;
		}
		if(pKey->List.iGlobalCurPosY > (pKey->List.iLineNum-1))
		{
			pKey->List.iGlobalCurPosY = (pKey->List.iLineNum-1);
			pKey->List.iRepeatAccel = 0;
		}
		scrollCheck(pKey);
	}


	if(dx)
	{
		iY = pKey->List.iGlobalCurPosY - pKey->List.iScrollPosY;
		// X�����̒l��ύX
		if(dx > 0)
		{
			pKey->List.iGlobalCurPosX += 1;
			if(maxX[iY] < pKey->List.iGlobalCurPosX)
			{
				pKey->List.iGlobalCurPosX = 0;
			}
		}
		else
		{
			pKey->List.iGlobalCurPosX -= 1;
			if(pKey->List.iGlobalCurPosX < 0)
			{
				pKey->List.iGlobalCurPosX = maxX[iY];
			}
		}
	}

	iY = pKey->List.iGlobalCurPosY - pKey->List.iScrollPosY;
	if(iY == 2 && pKey->List.iGlobalCurPosX == BUTTON_CTRL_GLOBALPOSX)
	{
		pKey->List.iGlobalCurPosY = pKey->List.iScrollPosY + 1;
	}
}


//============================================================================
//	scrollChars
//
//	�����ꗗ�X�N���[������
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	u_short press : �������
//
//	�߂�l : int : �������
static int scrollChars(skey_t *pKey, u_short press)
{
	static u_short usOldPress = 0;
	static int iRepeatOn = 0;
	static int iCountDown = LIST_SCROLL_START;
	int iv=0;

	if(press && press  == usOldPress)
	{
		if(iCountDown > 0)
		{
			iCountDown--;
		}
		if(iCountDown <= 0)
		{
			if(iRepeatOn)
			{
				pKey->List.iRepeatAccel++;
				if(pKey->List.iRepeatAccel > LIST_ACCEL_MAX)
				{
					pKey->List.iRepeatAccel = LIST_ACCEL_MAX;
				}
			}
			else
			{
				iRepeatOn = 1;
			}
		}
	}
	else
	{
		iRepeatOn = 0;
		iCountDown = LIST_SCROLL_START;
		pKey->List.iRepeatAccel = 0;
	}

	if(iRepeatOn)
	{
		static int iRepeatCycle = LIST_SCROLL_SPEED4;
		if(--iRepeatCycle <= 0)
		{
			if(pKey->List.iRepeatAccel < LIST_ACCEL_50)		 { iRepeatCycle = LIST_SCROLL_SPEED4; }
			else if(pKey->List.iRepeatAccel < LIST_ACCEL_100){ iRepeatCycle = LIST_SCROLL_SPEED2; }
			else if(pKey->List.iRepeatAccel < LIST_ACCEL_150){ iRepeatCycle = LIST_SCROLL_SPEED1; }
			else if(pKey->List.iRepeatAccel < LIST_ACCEL_200){ iRepeatCycle = LIST_SCROLL_SPEED1; }
			iv |= press;
		}
	}

	usOldPress = press;
	return 	iv;
}


//============================================================================
//	padUp
//
//	�p�b�h����
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	u_short up : �������
static void padUp(skey_t *pKey, u_short up)
{
	if(up & SCE_PADL1){}
	if(up & SCE_PADL2)
	{
		offStateCtrlCode(pKey, SKBCTRL_LEFT, KEY_PUSH);
	}
	if(up & SCE_PADR1)
	{
		releaseShift(pKey, 0);
	}
	if(up & SCE_PADR2)
	{
		offStateCtrlCode(pKey, SKBCTRL_RIGHT, KEY_PUSH);
	}
	if(up & SCE_PADRup)
	{
		u_int uiTmp[] = {0x00003000, 0x00000000};
		offStateKey(pKey, uiTmp, KEY_PUSH);
	}
	if(up & SCE_PADRdown){}
	if(up & SCE_PADRleft)
	{
		offStateCtrlCode(pKey, SKBCTRL_BACKSPACE, KEY_PUSH);
	}
	if(up & SCE_PADRright)
	{
		releaseKey(pKey);
	}
	if(up & SCE_PADselect){}
	if(up & SCE_PADstart)
	{
		offStateCtrlCode(pKey, SKBCTRL_ENTER, KEY_PUSH);
	}
}


//============================================================================
//	initKeyTop
//
//	�L�[�g�b�v������(��)
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
static void initKeyTop(skey_t *pKey)
{
}


//============================================================================
//	moveMouseCursor
//
//	�}�E�X�J�[�\���ړ�
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	iPoint_t *pPoint : �J�[�\���ʒu�ւ̃|�C���^
static void moveMouseCursor(skey_t *pKey, iPoint_t *pPoint)
{
	int keyNo = SKBMOUSE_NULL;
	SKB_ASSERT(pKey);

	if((keyNo=searchKeyByPoint(pKey, pPoint)) != SKBMOUSE_NULL)
	{
		// �{�^���̕��Ō��������̂Ńt�H�[�J�X�ړ�
		pKey->uiCursorPos = keyNo;
	}
	else
	{
		int curPos = SKBMOUSE_NULL;
		// �{�^���̕��Ō�����Ȃ������̂ňꗗ����������
		keyNo = searchListByPoint(pKey, pPoint);
		if(keyNo == SKBMOUSE_NULL)
		{
			return;		// �G��ĂȂ�
		}

		// �ꗗ�̕��Ō�������
		// �ꗗ���̃J�[�\���ړ�
		pKey->List.iGlobalCurPosX = keyNo % VISIBLE_CHARNUM_H;
		pKey->List.iGlobalCurPosY = (keyNo / VISIBLE_CHARNUM_H) + pKey->List.iScrollPosY;

		// �\�t�g�E�F�A�L�[�{�[�h���̃J�[�\���ړ�
		curPos = searchDummyKeyByIndex(pKey, keyNo);
		if(curPos != SKBMOUSE_NULL)
		{
			pKey->uiCursorPos = curPos;
		}
	}
}


//============================================================================
//	searchDummyKeyByIndex
//
//	�ꗗ�̃C���f�b�N�X����_�~�[�L�[��{��
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	int index : �����ꗗ�̃��[�J���C���f�b�N�X
//
//	�߂�l : int : �J�[�\��Pos
int searchDummyKeyByIndex(skey_t *pKey, int index)
{
	int i, d;

	// ���ɑI������L�[�{�[�h�̃t�H�[�J�X�ʒu�����߂�
	for(i=0, d=0; i<pKey->uiBtnNum; i++)
	{
		if((pKey->pBtns[i].animFlag & KEY_DUMMY))
		{
			if(index == d)
			{
				return i;
			}
			d++;
		}
	}

	return SKBMOUSE_NULL;	// ������Ȃ�����
}


//============================================================================
//	searchDummyKeyByIndex
//
//	�ʒu��񂩂當���ꗗ�̂ǂ̃L�[��I�����Ă��邩����
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	iPoint_t *pPt : �ʒu���̃|�C���^
//
//	�߂�l : int : �����ꗗ�̃C���f�b�N�X/SKBMOUSE_NULL
static int searchListByPoint(skey_t *pKey, iPoint_t *pPt)
{
	// �����p�P�b�g�쐬
	int x, y;
	int globalX, globalY;
	int charPacketLocalPosX, charPacketLocalPosY;
	int centeringOffsetX;
	int charW = g_pSkbImport->skb.uiListFontW;
	int charH = g_pSkbImport->skb.uiListFontH;
	int charW16 = g_pSkbImport->thinkXW16(g_pSkbImport->skb.uiListFontW);
	int charH16 = g_pSkbImport->thinkYH16(g_pSkbImport->skb.uiListFontH);		// ����Y�l�́A�t�H���g���C�u�����ɓn���l�Ȃ̂ŁANTSC/PAL��FRAME/FIELD���l���Ȃ��ėǂ��B
	int dx, dy;
	int cx, cy;
	int px, py;
	int tx, ty;
	UTF8Char_t utf8Char;

	cx = g_pSkbImport->thinkXW(pKey->uiKbdPosX) + CHAR_STARTDRAW_POSX + CHAR_TOPLEFT_POSX;							// ���������n��OFFSET X
	cy = g_pSkbImport->thinkYH(pKey->uiKbdPosY) + g_pSkbImport->thinkYH(CHAR_STARTDRAW_POSY + CHAR_TOPLEFT_POSY);	// ���������n��OFFSET Y
	px = g_pSkbImport->thinkXW16(pPt->x);
	py = g_pSkbImport->thinkYH16(pPt->y);

	for(y=0; y<VISIBLE_CHARNUM_V; y++)
	{
		for(x=0; x<VISIBLE_CHARNUM_H; x++)
		{
			globalX = x;
			globalY = y + pKey->List.iScrollPosY;
			charPacketLocalPosX = (x * charW) + cx;			// ���[�J���ʒu���擾
			charPacketLocalPosY = (y * charH) + cy;

			utf8Char = pKey->List.utf8Cache[(y * VISIBLE_CHARNUM_H) + x].utf8Char;
			charPacketLocalPosY = g_pSkbImport->thinkYHn(charPacketLocalPosY);

			centeringOffsetX = (charW - pKey->List.utf8Cache[(y * VISIBLE_CHARNUM_H) + x].iFontWidth) >> 1;
			dx = charPacketLocalPosX + centeringOffsetX;
			dy = charPacketLocalPosY + g_pSkbImport->thinkYHn(TEXTPOS_ADDY);

			tx = g_pSkbImport->thinkXW16(dx+FOCUS_MARGINX);
			ty = (dy<<4) + g_pSkbImport->thinkYH16(FOCUS_MARGINY);

			if((tx <= px) && (ty <= py) &&
			   ((tx + charW16) > px) && ((ty + charH16) > py))
			{
				return ((y*VISIBLE_CHARNUM_H)+x);		// ���X�g�̃C���f�b�N�X��Ԃ�
			}
		}
	}

	return SKBMOUSE_NULL;
}



//============================================================================
//	leftButtonDown_list
//
//	�}�E�X���{�^������
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	iPoint_t *pPt : �ʒu���̃|�C���^
static void leftButtonDown_list(skey_t *pKey, iPoint_t *pPoint)
{
	int keyNo  = SKBMOUSE_NULL;
	int listNo = SKBMOUSE_NULL;
	int tgNo = SKBMOUSE_NULL;
	SKB_ASSERT(pKey);

	keyNo = searchKeyByPoint(pKey, pPoint);		// �{�^������

	// �ꗗ����������
	if(tgNo == SKBMOUSE_NULL)
	{
		listNo = searchListByPoint(pKey, pPoint);
	}

	if(keyNo  == SKBMOUSE_NULL &&
	   tgNo   == SKBMOUSE_NULL &&
	   listNo == SKBMOUSE_NULL)
	{
		pKey->uiPush = 0;				// �J�[�\���������t���O��OFF
		return;
	}


	if(!(g_protectSkb.uiDragState & SKBMOUSE_LBTNDOWN))
	{
		// ���{�^������
		// �L�[
		if(keyNo != SKBMOUSE_NULL)
		{
			pKey->uiCursorPos = keyNo;
			pKey->uiPush  = 1;					// �J�[�\���������t���O��ON

			g_protectSkb.uiOldCurPos[0] = pKey->uiCursorPos;	// �h���b�O�����܂ܑ��̃{�^���Ɉړ��������̌��o�t���O��ۑ�
			g_protectSkb.uiDragState |= SKBMOUSE_LBTNDOWN;
			return;
		}

		// �ꗗ
		if(listNo != SKBMOUSE_NULL)
		{
			g_protectSkb.uiOldCurPosList[0] = listNo % VISIBLE_CHARNUM_H;
			pKey->List.iGlobalCurPosX = listNo % VISIBLE_CHARNUM_H;
			g_protectSkb.uiOldCurPosList[1] = (listNo / VISIBLE_CHARNUM_H) + pKey->List.iScrollPosY;
			pKey->List.iGlobalCurPosY = (listNo / VISIBLE_CHARNUM_H) + pKey->List.iScrollPosY;
			pKey->uiPush  = 1;					// �J�[�\���������t���O��ON

			g_protectSkb.uiDragState |= SKBMOUSE_LBTNDOWN;
		}
	}
	else
	{
		// �h���b�O��
		// �L�[
		if(g_protectSkb.uiOldCurPos[0] != keyNo && keyNo != SKBMOUSE_NULL)
		{
			pKey->uiPush = 0;				// �J�[�\���������t���O��OFF
			g_protectSkb.uiOldCurPos[0] = SKBMOUSE_NULL;
			return;
		}

		// �ꗗ
		if(g_protectSkb.uiOldCurPosList[0] != (listNo % VISIBLE_CHARNUM_H) ||
		   g_protectSkb.uiOldCurPosList[1] != ((listNo / VISIBLE_CHARNUM_H) + pKey->List.iScrollPosY))
		{
			// �h���b�O�����܂ܑ��̃{�^���Ɉړ�����
			pKey->uiPush = 0;				// �J�[�\���������t���O��OFF
			g_protectSkb.uiOldCurPosList[0] = SKBMOUSE_NULL;
			g_protectSkb.uiOldCurPosList[1] = SKBMOUSE_NULL;
		}
	}
}


//============================================================================
//	leftButtonUp_list
//
//	�}�E�X���{�^������
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	iPoint_t *pPt : �ʒu���̃|�C���^
static void leftButtonUp_list(skey_t *pKey, iPoint_t *pPoint)
{
	int keyNo  = SKBMOUSE_NULL;
	int listNo = SKBMOUSE_NULL;
	SKB_ASSERT(pKey);

	// �}�E�X�|�C���^
	keyNo = searchKeyByPoint(pKey, pPoint);
	if(keyNo != SKBMOUSE_NULL)
	{
		// �{�^���̕��Ō��������̂Ńt�H�[�J�X�ړ�
		if(g_protectSkb.uiOldCurPos[0] == pKey->uiCursorPos)
		{
			pushKey(pKey);			// ������ݒ�
		}
	}
	else
	{
		// �{�^���̕��Ō�����Ȃ������̂ňꗗ����������
		listNo = searchListByPoint(pKey, pPoint);
		if(listNo == SKBMOUSE_NULL)
		{
			pKey->uiPush = 0;					// �J�[�\���������t���O��OFF
			g_protectSkb.uiOldCurPosList[0] = SKBMOUSE_NULL;
			g_protectSkb.uiOldCurPosList[1] = SKBMOUSE_NULL;
			g_protectSkb.uiOldCurPos[0] = SKBMOUSE_NULL;
			g_protectSkb.uiDragState &= ~SKBMOUSE_LBTNDOWN;		// Drag�t���O������
			return;		// �G��ĂȂ�
		}

		// ��������
		if(g_protectSkb.uiOldCurPosList[0] != SKBMOUSE_NULL &&
		   g_protectSkb.uiOldCurPosList[1] != SKBMOUSE_NULL)
		{
			addChar(pKey);
			releaseShift(pKey, 1);
		}
	}

	pKey->uiPush = 0;					// �J�[�\���������t���O��OFF
	g_protectSkb.uiOldCurPosList[0] = SKBMOUSE_NULL;
	g_protectSkb.uiOldCurPosList[1] = SKBMOUSE_NULL;
	g_protectSkb.uiOldCurPos[0] = SKBMOUSE_NULL;
	g_protectSkb.uiDragToggle = 0;
	g_protectSkb.uiDragState &= ~SKBMOUSE_LBTNDOWN;		// Drag�t���O������
}


//============================================================================
//	searchFocus
//
//	�ʒu����t�H�[�J�X�̏ꏊ�����߂�
//
//	skey_t *pKeyNext : ����I��\��̃\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	sprite_t *pNowSp : ���ݑI�𒆂̃\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//
//	�߂�l : u_int : 0:�����l/0�ȊO:����I��\��̃\�t�g�E�F�A�L�[�{�[�h�̃L�[�C���f�b�N�X
static u_int searchFocus(skey_t *pKeyNext, sprite_t *pNowSp)
{
	int keyNo=SKBMOUSE_NULL, listNo=SKBMOUSE_NULL;
	iPoint_t pt;
	sprite_t *pSp = pNowSp;

	// �t�H�[�J�X�̂������Ă���X�v���C�g�̒��S�ʒu�����߂�
	pt.x = g_pSkbImport->thinkXW(pSp->x + (pSp->w>>1));
	pt.y = g_pSkbImport->thinkYH(pSp->y + (pSp->h>>1));

	keyNo = searchKeyByPoint(pKeyNext, &pt);
	if(keyNo != SKBMOUSE_NULL)
	{
		// �{�^������
		return keyNo;
	}

	// �{�^�������Ō�����Ȃ������̂ŕ����ꗗ���ŒT��
	listNo = searchListByPoint(pKeyNext, &pt);
	if(listNo != SKBMOUSE_NULL)
	{
		// �I��
		u_int RR[] = {2, 0, 1, 1, 1, 1, 0, 0};
		u_int cnt, i, pp;

		pp = 0;
		cnt = listNo / VISIBLE_CHARNUM_H;
		for(i=0; i<cnt; i++)
		{
			pp += RR[i];
		}

		return (pp + listNo);
	}

	pKeyNext->List.iGlobalCurPosX = 0;
	pKeyNext->List.iGlobalCurPosY = 0;
	return 0;
}


//============================================================================
//	open
//
//	�\�t�g�E�F�A�L�[�{�[�h��OPEN�������̏���
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
static void open(skey_t *pKey)
{
//	�ȉ��̐ݒ�𖳎�����ƁA
//	���������̃J�[�\���ʒu�ƃL�[(�{�^��)�̃J�[�\���ʒu������Ȃ���
//	�\�����Ȃ���������܂��B
	u_int RR[] = {18, 16, 17, 17, 17, 17, 16, 16};
	u_int cntX, i;

	// ���X�g�����̏�����
	pKey->List.iScrollPosY = 0;
	pKey->List.iUtf8CacheTopLine = -1;

	cntX = 0;
	for(i=0; i<sizeof(RR)/sizeof(RR[0]); i++)
	{
		if((cntX + RR[i]) > (pKey->uiCursorPos))
		{
			break;
		}
		cntX += RR[i];
	}

	pKey->List.iGlobalCurPosX = (pKey->uiCursorPos) - cntX;
	pKey->List.iGlobalCurPosY = i;

	setFocus(pKey, pKey->List.iGlobalCurPosX, pKey->List.iGlobalCurPosY);
}


//============================================================================
//	close
//
//	�\�t�g�E�F�A�L�[�{�[�h��CLOSE�������̏����i��j
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
static void close(skey_t *pKey)
{
}
