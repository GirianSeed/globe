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

#include <sys/types.h>
#include <eekernel.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <libpkt.h>

#include <shareddef.h>
#include "textField.h"



#define TEXF_ASSERT(_a_)  assert((int)_a_)
#define TF_MAX_NUM			(1024)

#define CARET_W16			(2<<4)


static gImport_t		*g_pTxtFImport;


static struct
{
	textureInfo_t		tex;	// �e�N�X�`��

} g_TF;



// �e�L�X�g�t�B�[���h�g�����̃T�C�Y
#define TF_MARGIN			(16)			// �g�����̃}�[�W��
#define TF_MARGIN16			(TF_MARGIN<<4)	// �g�����̃}�[�W��x16
#define TF_CLIP_W			(13)			// �N���b�v�g�`��ʒu����̑��Έʒu
#define TF_CLIP_H			(3)				// �N���b�v�g�`��ʒu����̑��Έʒu
//#define TF_CLIP_SIZE		(13)


// �e�L�X�g�t�B�[���h �X�v���C�g
const static sprite_t	g_spSrc[] =
{
	{0,		0,		0,		16,		16},
	{0,		16,		0,		16,		16},
	{0,		32,		0,		16,		16},
	{16,	0,		0,		16,		16},
	{16,	16,		0,		16,		16},
	{16,	32,		0,		16,		16},
	{32,	0,		0,		16,		16},
	{32,	16,		0,		16,		16},
	{32,	32,		0,		16,		16},
};
// �e�L�X�g�t�B�[���h ���
const static paneChip_t	g_pnSrc[] =
{
	{0,		0,		0x80808080},
	{1,		1,		0x80808080},
	{2,		2,		0x80808080},
	{3,		3,		0x80808080},
	{4,		4,		0x80808080},
	{5,		5,		0x80808080},
	{6,		6,		0x80808080},
	{7,		7,		0x80808080},
	{8,		8,		0x80808080},
};

// �e�L�X�g�t�B�[���h UV���W
const static uv_t		g_uvTFSrc[] =
{
	{0,		0,		16,		16},
	{0,		16,		16,		16},
	{0,		32,		16,		16},
	{16,	0,		16,		16},
	{16,	16,		16,		16},
	{16,	32,		16,		16},
	{32,	0,		16,		16},
	{32,	16,		16,		16},
	{32,	32,		16,		16},
};
// ���ꗗ UV���W
const static uv_t		g_uvCLSrc[] =
{
	{48,	0,		16,		16},
	{48,	16,		16,		16},
	{48,	32,		16,		16},
	{64,	0,		16,		16},
	{64,	16,		16,		16},
	{64,	32,		16,		16},
	{80,	0,		16,		16},
	{80,	16,		16,		16},
	{80,	32,		16,		16},
};

#ifdef SKBRES_LINK  //Makefile define
//--------------------------------------------------------
#include <skbres.h>
#define TF_TEXTURE_NAME		tField_tm2
//--------------------------------------------------------
#else
//--------------------------------------------------------
#if CDBOOT
#define TF_TEXTURE_NAME		TF_TEXTURE_PATH"\\TFIELD.TM2;1"
#else
#define TF_TEXTURE_NAME		TF_TEXTURE_PATH"/tField.tm2"
#endif
//--------------------------------------------------------
#endif


static void drawText(sceVif1Packet *pk, textField_t *pTf);
static void drawCaret(sceVif1Packet *pk, textField_t *pTf, int x16, int y16);
static void moveTF(textField_t *pTf, int x16, int y16);
static void sizeTF(textField_t *pTf, int w16, int h16);
static void stretchRect(sprite_t *sp0, int w, int h);
static void setResource(textField_t *pTf);
#ifndef ATOK_NO_LINK
static void drawCandListFrame(sceVif1Packet *pk, textField_t *pTf, iRect_t *rc16, int nCurrentCand);
#endif

//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//  TextF_Init												ref:  textField.h
//
//  �e�L�X�g�t�B�[���h����������
//
//  gImport_t *pImport    : �O���R�[���o�b�N�֐��Q�|�C���^
//  iRect_t *rc           : �e�L�X�g�t�B�[���h�̈ʒu�A�T�C�Y
void TextF_Init(gImport_t *pImport)
{
	TEXF_ASSERT(pImport);
	g_pTxtFImport = pImport;

	// �e�N�X�`���ǂݍ���
	g_pTxtFImport->readTex(&g_TF.tex, TF_TEXTURE_NAME);
}


//============================================================================
//  TextF_Destroy											ref:  textField.h
//
//  �e�L�X�g�t�B�[���h�폜����
//
void TextF_Destroy(void)
{
	g_pTxtFImport->deleteTex(&g_TF.tex);

}

//============================================================================
//  TexF_Create												ref:  textField.h
//
//  �e�L�X�g�t�B�[���h�쐬
//
//  textField_t *pTf : �e�L�X�g�t�B�[���h�\���̂ւ̃|�C���^
//  iRect_t *rc      : �e�L�X�g�t�B�[���h�̈ʒu�A�T�C�Y
void TextF_Create(textField_t *pTf, iRect_t *rc)
{
	TEXF_ASSERT(pTf);
	TEXF_ASSERT(rc);
	memset(pTf, 0x0, sizeof(textField_t));

	// ���\�[�X�ݒ�
	setResource(pTf);

	// rc�̈ʒu�A�T�C�Y�͉�ʏ�Ɏʂ�͈͂ƕۏ؂���Ă���
	// �ʒu�A�T�C�Y�ݒ�
	TextF_Move(pTf, rc);
}


//============================================================================
//  TextF_Delete											ref:  textField.h
void TextF_Delete(textField_t *pTf)
{
}


extern void SetLine(sceVif1Packet *pk, int x, int y, int cx, int cy, const RGBA32 c, int z);
//============================================================================
//  TextF_Run												ref:  textField.h
//
//  �ʏ폈��
//
//  sceVif1Packet *pk : �p�P�b�g�ւ̃|�C���^
//  textField_t *pTf  : �e�L�X�g�t�B�[���h�\���̂ւ̃|�C���^
void TextF_Run(sceVif1Packet *pk, textField_t *pTf)
{
	int i;
	TEXF_ASSERT(pk);
	TEXF_ASSERT(pTf);

	if(pTf->iHide)
	{
		// �`�悵�Ȃ�
		return;
	}

	// �e�L�X�g�t�B�[���h�`��
	{
		sprite_t   *pSp = pTf->textField.aSP;
		uv_t       *pUv = pTf->textField.aUV;
		paneChip_t *pPn = pTf->textField.aPN;
		int spIndex, uvIndex;
		u_int col;

		g_pTxtFImport->loadTex(pk, &g_TF.tex);											// �e�N�X�`�����[�h
		for(i=0; i<TF_FRAME_ELEMENT; i++, pPn++)
		{
			spIndex = pPn->spIndex;
			uvIndex = pPn->uvIndex;
			col = affectAlpha(pPn->rgba, pTf->alphaValue[TF_ALPHA_EDITORBG]);

			g_pTxtFImport->drawTex(pk, col, (pSp+spIndex), (pUv+uvIndex), &g_TF.tex);	// �e�N�X�`���`��
		}
	}

	drawText(pk, pTf);
}


//============================================================================
//  TextF_Move												ref:  textField.h
//
//  �e�L�X�g�t�B�[���h�ړ�
//
//  textField_t *pTf : �e�L�X�g�t�B�[���h�\���̂ւ̃|�C���^
//  iRect_t *rc      : ��`�̃|�C���^
//					   FRAME/FIELD ���[�h���l�����Ȃ��l�ł���NTSC����ɍ쐬�B
void TextF_Move(textField_t *pTf, iRect_t *rc)
{
	int x16, y16, w16, h16;
	TEXF_ASSERT(pTf);
	TEXF_ASSERT(rc);

	// ���E�l��ݒ�
	if(rc->x < 0)
	{
		rc->x = 0;
	}
	if(rc->y < 0)
	{
		rc->y = 0;
	}
	if(rc->w < 0)
	{
		rc->w = TF_MARGIN*2;
	}
	if(rc->h < 0)
	{
		rc->h = 0;//(TF_MARGIN_H*2)>>4;
	}

	x16 = g_pTxtFImport->thinkXW16(rc->x);
	y16 = g_pTxtFImport->thinkYH16(rc->y);
	w16 = g_pTxtFImport->thinkXW16(rc->w);
	h16 = g_pTxtFImport->thinkYH16(rc->h);

	if((x16 == pTf->rcEdit16.x) && (y16 == pTf->rcEdit16.y) &&
	   (w16 == pTf->rcEdit16.w) && (h16 == pTf->rcEdit16.h))
	{
		return;
	}

	moveTF(pTf, x16, y16);
	sizeTF(pTf, w16, h16);
}


//============================================================================
//  TextF_ChangeAlpha										ref:  textField.h
//
//  �e�L�X�g�t�B�[���h�A���t�@�l�ύX
//
//  textField_t *pTf : �e�L�X�g�t�B�[���h�\���̂ւ̃|�C���^
//  int index        : �A���t�@�z��ւ̃C���f�b�N�X
//  u_char alpha     : �ݒ肵�����A���t�@�l
void TextF_ChangeAlpha(textField_t *pTf, int index, u_char alpha)
{
	TEXF_ASSERT(pTf);

	pTf->alphaValue[index] = alpha;
}

//============================================================================
//  TextF_SetDispMode										ref:  textField.h
//
//  �e�L�X�g�t�B�[���h�̕\�����[�h�ݒ�
//
//  textField_t *pTf : �e�L�X�g�t�B�[���h�\���̂ւ̃|�C���^
//  int mode         : ���[�h
void TextF_SetDispMode( textField_t *pTf, int mode )
{
	TEXF_ASSERT(pTf);

	pTf->dispMode = mode;
}

//============================================================================
// STATIC FUNCTIONS
//============================================================================
//============================================================================
//	drawText
//
//	������`��
//
//	sceVif1Packet *pk : VIF1�p�P�b�g�i�[�p�o�b�t�@�ւ̃|�C���^
//	textField_t *pTf : �e�L�X�g�t�B�[���h�\���̂ւ̃|�C���^
static void drawText(sceVif1Packet *pk, textField_t *pTf)
{
	int strWidth16 = 0;
	int x16 = pTf->rcEdit16.x + g_pTxtFImport->thinkXW16(TF_MARGIN);	// �e�L�X�g�t�B�[���h�̘g�̕�  ���E���ɕ`��
	int y16 = pTf->rcEdit16.y + g_pTxtFImport->thinkYH16(TF_MARGIN);	// �e�L�X�g�t�B�[���h�̘g�̍����������ɕ`��
	int w16 = pTf->rcEdit16.w - g_pTxtFImport->thinkXW16(TF_MARGIN<<1);	// �e�L�X�g�t�B�[���h�̘g�̕�  ������������
	int h16 = pTf->rcEdit16.h - g_pTxtFImport->thinkYH16(TF_MARGIN<<1);	// �e�L�X�g�t�B�[���h�̘g�̍���������������
	int caretPos = g_pTxtFImport->txt.getCaretPos();					// �L�����b�g�ʒu�擾
	int cx=0 ,cy=0, cw=0, ch=0;
	int clipX = x16>>4;
	int clipY = y16>>4;
	int clipW = w16>>4;
	int clipH = (h16>>4) + 10;
	u_int *pUCS4 = g_pTxtFImport->txt.getStrings();						// �m�蕶����擾
	u_int *pDispText;
	iRect_t rc   = {x16, y16, w16, h16};								// �g
	iRect_t clip = {clipX, clipY, clipW, clipH};						// �N���b�v�̈�
	u_int uiBuf[TF_MAX_NUM];
	u_int dispText[TF_MAX_NUM];

	int scrx16, scry16;

	cx = g_pTxtFImport->thinkXW(x16);
	cy = g_pTxtFImport->thinkYH(y16);
	cw = g_pTxtFImport->thinkXW(w16);
	ch = g_pTxtFImport->thinkYH(h16);

	// �X�N���[��
	if(pTf->iScrollLX16 < (x16+(TF_MARGIN16*2)))
	{
		// �������ɂ͂ݏo����
		pTf->iScrollPosX16 -= 2000;
		if(pTf->iScrollPosX16 < 0)
		{
			pTf->iScrollPosX16 = 0;
		}
	}
	else if(pTf->iScrollRX16 > (x16+w16-(TF_MARGIN16*2)))
	{
		// �E�����ɂ͂ݏo����
		if(!((pTf->iScrollLX16-2000) < x16))
		{
			pTf->iScrollPosX16 += 2000;
		}
	}

	// �m�蕶���� �`��
	rc.x = cx - g_pTxtFImport->thinkXW(pTf->iScrollPosX16);
	rc.y = cy;
	rc.w = cw;
	rc.h = ch;
	if( pTf->dispMode ){ // password �p
		int n;

		for(n=0;;n++){
			if(*(pUCS4+n)==0)	break;
			dispText[n]='*';
		}
		dispText[n]=0;
		g_pTxtFImport->txt.drawStrings(pk, dispText, &clip, &rc);	// �����`��
		pDispText=dispText;
	}
	else{
		g_pTxtFImport->txt.drawStrings(pk, pUCS4, &clip, &rc);	// �����`��
		pDispText=pUCS4;
	}

	// �L�����b�g
	memcpy((char*)uiBuf, (char*)pDispText, caretPos*sizeof(u_int));
	uiBuf[caretPos] = '\0';
	strWidth16 = g_pTxtFImport->txt.getStringWidth(uiBuf);	// ������̐擪����L�����b�g�ʒu�܂ł̕������߂�

	// �L�����b�g�����݂��Ă���o�C�g�ʒu�̕������`�悳�ꂽGS���[�J�����W�𓾂�
	scrx16 = x16 + strWidth16;
	scry16 = y16;

	#ifndef ATOK_NO_LINK
	if(g_pTxtFImport->ucUseAtok)
	{
		// ATOK �g�p
		int atokState = g_pTxtFImport->txt.getAtokState();	// ���̓X�e�[�g���擾

		if(atokState <= SCE_ATOK_ISTATE_BEFOREINPUT)
		{
			// �������͑O�̎�
			drawCaret(pk, pTf, scrx16 - pTf->iScrollPosX16, scry16);
			pTf->iScrollLX16 = scrx16 - pTf->iScrollPosX16;
			pTf->iScrollRX16 = scrx16 - pTf->iScrollPosX16;
		}
		else if(atokState == SCE_ATOK_ISTATE_BEFORECONVERT)
		{
			// �ϊ��O
			rc.x = g_pTxtFImport->thinkXW(scrx16 - pTf->iScrollPosX16);
			g_pTxtFImport->txt.drawStringBeforeChange(pk, pUCS4, &clip, &rc);		// �ϊ��O������`��

			strWidth16 = g_pTxtFImport->txt.getToCaretPos();
			scrx16 += strWidth16;	// �L�����b�g�p

			drawCaret(pk, pTf, scrx16 - pTf->iScrollPosX16, scry16);
			pTf->iScrollLX16 = scrx16 - pTf->iScrollPosX16;
			pTf->iScrollRX16 = scrx16 - pTf->iScrollPosX16;
		}
		else
		{
			// �ϊ���
			rc.x = g_pTxtFImport->thinkXW(scrx16 - pTf->iScrollPosX16);
			g_pTxtFImport->txt.drawStringChanging(pk, pUCS4, &clip, &rc,
												   &pTf->iScrollLX16, &pTf->iScrollRX16,
												   TF_MARGIN, TF_MARGIN, pTf, drawCandListFrame);		// �ϊ��O������`��
			strWidth16 = g_pTxtFImport->txt.getToCaretPos();
			scrx16 += strWidth16;	// �L�����b�g�p
			pTf->iScrollLX16 = scrx16 - pTf->iScrollPosX16;
			pTf->iScrollRX16 = scrx16 - pTf->iScrollPosX16;
		}
	}
	else
	#endif
	{
		// ATOK ���g�p
		drawCaret(pk, pTf, scrx16 - pTf->iScrollPosX16, scry16);
		pTf->iScrollLX16 = scrx16 - pTf->iScrollPosX16;
		pTf->iScrollRX16 = scrx16 - pTf->iScrollPosX16;
	}
}


//============================================================================
//	drawText
//
//	�L�����b�g�`��
//
//	sceVif1Packet *pk : VIF1�p�P�b�g�i�[�p�o�b�t�@�ւ̃|�C���^
//	textField_t *pTf : �e�L�X�g�t�B�[���h�\���̂ւ̃|�C���^
//	int x16 : �L�����b�g�`��X�ʒu
//	int y16 : �L�����b�g�`��Y�ʒu
static void drawCaret(sceVif1Packet *pk, textField_t *pTf, int x16, int y16)
{
	int ex16 = pTf->rcEdit16.x + g_pTxtFImport->thinkXW16(TF_MARGIN);
	int ew16 = pTf->rcEdit16.w - g_pTxtFImport->thinkXW16(TF_MARGIN<<1);
	iRect_t rc = {x16, y16, CARET_W16, 0};
	u_int col = affectAlpha(pTf->uiCaretColor, pTf->alphaValue[TF_ALPHA_CARET]);

	if(rc.x < ex16)
	{
		rc.x = ex16;
	}
	if(rc.x > (ex16+ew16))
	{
		rc.x = ex16+ew16;
	}
	rc.h = g_pTxtFImport->txt.uiCharHeight<<4;
	#if 1 //SKB_EX Insert
	if( ( !g_pTxtFImport->txt.isInsertMode() ) 
	  #ifndef ATOK_NO_LINK
	  && g_pTxtFImport->txt.getAtokState() != SCE_ATOK_ISTATE_BEFORECONVERT
	  #endif
	){
		//�㏑�����쎞�́A�L�����b�g�𑾂点�Ă���B
		rc.w +=  (1<<4);
	}
	#endif
	g_pTxtFImport->drawPane(pk, &rc, col, 0);
}


//============================================================================
//	moveTF
//
//	TextField�̈ʒu�ύX
//
//	textField_t *pTf : �e�L�X�g�t�B�[���h�\���̂ւ̃|�C���^
//	int x16 : �ړ�X���Έʒu
//	int y16 : �ړ�Y���Έʒu
static void moveTF(textField_t *pTf, int x16, int y16)
{
	int i;
	pTf->rcEdit16.x += (x16 - pTf->rcEdit16.x);
	pTf->rcEdit16.y += (y16 - pTf->rcEdit16.y);

	memcpy(pTf->textField.aSP, g_spSrc, sizeof(sprite_t)*TF_FRAME_ELEMENT);

	// ���W���C��
	for(i=0; i<TF_FRAME_ELEMENT; i++)
	{
		pTf->textField.aSP[i].x = g_pTxtFImport->thinkXW16(pTf->textField.aSP[i].x) + x16;
		pTf->textField.aSP[i].y = g_pTxtFImport->thinkYH16(pTf->textField.aSP[i].y) + y16;
		pTf->textField.aSP[i].z = g_pTxtFImport->thinkXW16(pTf->textField.aSP[i].z);
		pTf->textField.aSP[i].w = g_pTxtFImport->thinkXW16(pTf->textField.aSP[i].w);
		pTf->textField.aSP[i].h = g_pTxtFImport->thinkYH16(pTf->textField.aSP[i].h);
	}
}


//============================================================================
//	sizeTF
//
//	TextField�̃T�C�Y�ύX
//
//	textField_t *pTf : �e�L�X�g�t�B�[���h�\���̂ւ̃|�C���^
//	int w16 : �`��T�C�Y��
//	int h16 : �`��T�C�Y����
static void sizeTF(textField_t *pTf, int w16, int h16)
{
	pTf->rcEdit16.w = w16;
	pTf->rcEdit16.h = h16;
	stretchRect(pTf->textField.aSP, w16, h16);
}


//============================================================================
//	stretchRect
//
//	TextField��`��L�΂�
//
//	textField_t *pTf : �e�L�X�g�t�B�[���h�\���̂ւ̃|�C���^
//	int w : �L�����b�g�`��X�ʒu
//	int h : �L�����b�g�`��Y�ʒu
static void stretchRect(sprite_t *sp0, int w, int h)
{
//	 <---  W  ---->
//	����������������<---
//	��0 �� 3  ��6 ��   |
//	����������������   |
//	��  ��    ��  ��   |
//	��1 �� 4  ��7 ��   | H
//	��  ��    ��  ��   |
//	����������������   |
//	��2 �� 5  ��8 ��   |
//	����������������<---
//
//  0,1,2,6,7,8 = �g����
//  4 = �ϕ���
	sprite_t *pSp = sp0+1;
	sprite_t *pSpPrev;
	int rs = 0;

	// 1, 4, 7�̍����A4�̕����v�Z
	if(g_pTxtFImport->getFrameField())
	{
		// FRAME : Y�l�A������1/2�ɂ���
		rs = 1;
	}
	w -= g_pTxtFImport->thinkXW16(TF_MARGIN<<1);	// �ϕ����̕�  (���E2��)
	h -= g_pTxtFImport->thinkYH16(TF_MARGIN<<1);//(TF_MARGIN_H16<<1);	// �ϕ����̍���(�㉺2��)
	if(w < 0)
	{
		w = 0;
	}

	if(h < 0)
	{
		h = 0;
	}

	pSp->h = h;							// Pane[1]�̍���
	pSpPrev = (sp0+1);
	pSp = (sp0+2);
	pSp->y = h + pSpPrev->y;			// Pane[2]��Y�l

	pSpPrev = (sp0+2);
	pSp = (sp0+3);
	pSp->w = w;							// Pane[3]�̕�

	pSpPrev = (sp0+3);
	pSp = (sp0+4);
	pSp->h = h;							// Pane[4]�̍���
	pSp->w = w;							// Pane[4]�̕�

	pSpPrev = (sp0+4);
	pSp = (sp0+5);
	pSp->w = w;							// Pane[5]�̕�
	pSp->y = h + pSpPrev->y;			// Pane[5]��Y�l

	pSpPrev = (sp0+5);
	pSp = (sp0+6);
	pSp->x = pSpPrev->x + pSpPrev->w;	// Pane[6]��X�l

	pSpPrev = (sp0+6);
	pSp = (sp0+7);
	pSp->x = pSpPrev->x;				// Pane[7]��X�l
	pSp->h = h;							// Pane[7]�̍���

	pSpPrev = (sp0+7);
	pSp = (sp0+8);
	pSp->y = pSpPrev->h + pSpPrev->y;	// Pane[8]��Y�l
	pSp->x = pSpPrev->x;				// Pane[8]��X�l
}


//============================================================================
//	setResource
//
//	��`�̌��f�[�^��ݒ�
//
//	textField_t *pTf : �e�L�X�g�t�B�[���h�\���̂ւ̃|�C���^
static void setResource(textField_t *pTf)
{
	int i;
	// �e�L�X�g�t�B�[���h
	memcpy(pTf->textField.aSP, g_spSrc,   sizeof(sprite_t)*TF_FRAME_ELEMENT);
	memcpy(pTf->textField.aUV, g_uvTFSrc, sizeof(uv_t)*TF_FRAME_ELEMENT);
	memcpy(pTf->textField.aPN, g_pnSrc,   sizeof(paneChip_t)*TF_FRAME_ELEMENT);

	// ���ꗗ
	memcpy(pTf->candList.aSP, g_spSrc,   sizeof(sprite_t)*TF_FRAME_ELEMENT);
	memcpy(pTf->candList.aUV, g_uvCLSrc, sizeof(uv_t)*TF_FRAME_ELEMENT);
	memcpy(pTf->candList.aPN, g_pnSrc,   sizeof(paneChip_t)*TF_FRAME_ELEMENT);

	for(i=0; i<TF_FRAME_ELEMENT; i++)
	{
		pTf->textField.aSP[i].x  = g_pTxtFImport->thinkXW16(pTf->textField.aSP[i].x);
		pTf->textField.aSP[i].y  = g_pTxtFImport->thinkYH16(pTf->textField.aSP[i].y);
		pTf->textField.aSP[i].z  = g_pTxtFImport->thinkXW16(pTf->textField.aSP[i].z);
		pTf->textField.aSP[i].w  = g_pTxtFImport->thinkXW16(pTf->textField.aSP[i].w);
		pTf->textField.aSP[i].h  = g_pTxtFImport->thinkYH16(pTf->textField.aSP[i].h);

		pTf->textField.aUV[i].u  = STG(pTf->textField.aUV[i].u);
		pTf->textField.aUV[i].v  = STG(pTf->textField.aUV[i].v);
		pTf->textField.aUV[i].tw = STG(pTf->textField.aUV[i].tw);
		pTf->textField.aUV[i].th = STG(pTf->textField.aUV[i].th);

		pTf->candList.aSP[i].x  = g_pTxtFImport->thinkXW16(pTf->candList.aSP[i].x);
		pTf->candList.aSP[i].y  = g_pTxtFImport->thinkYH16(pTf->candList.aSP[i].y);
		pTf->candList.aSP[i].z  = g_pTxtFImport->thinkXW16(pTf->candList.aSP[i].z);
		pTf->candList.aSP[i].w  = g_pTxtFImport->thinkXW16(pTf->candList.aSP[i].w);
		pTf->candList.aSP[i].h  = g_pTxtFImport->thinkYH16(pTf->candList.aSP[i].h);

		pTf->candList.aUV[i].u  = STG(pTf->candList.aUV[i].u);
		pTf->candList.aUV[i].v  = STG(pTf->candList.aUV[i].v);
		pTf->candList.aUV[i].tw = STG(pTf->candList.aUV[i].tw);
		pTf->candList.aUV[i].th = STG(pTf->candList.aUV[i].th);
	}
}


#ifndef ATOK_NO_LINK
//============================================================================
//	drawCandListFrame
//
//	���ꗗ�̃t���[���`��p�p�P�b�g�쐬
//
//	sceVif1Packet *pk : VIF1�p�P�b�g�i�[�p�o�b�t�@�ւ̃|�C���^
//	textField_t *pTf : �e�L�X�g�t�B�[���h�\���̂ւ̃|�C���^
//	iRect_t *rc16 : ��`���\������o�b�t�@�ւ̃|�C���^
//	int nCurrentCand : �I�𒆌��ԍ�
static void drawCandListFrame(sceVif1Packet *pk, textField_t *pTf, iRect_t *rc16, int nCurrentCand)
{
	int i;
	int x16 = rc16->x;
	int y16 = rc16->y;

	memcpy(pTf->candList.aSP, g_spSrc,  sizeof(sprite_t)*TF_FRAME_ELEMENT);

	// ���W���C��
	for(i=0; i<TF_FRAME_ELEMENT; i++)
	{
		pTf->candList.aSP[i].x = g_pTxtFImport->thinkXW16(pTf->candList.aSP[i].x) + x16;
		pTf->candList.aSP[i].y = g_pTxtFImport->thinkYH16(pTf->candList.aSP[i].y) + y16;
		pTf->candList.aSP[i].z = g_pTxtFImport->thinkXW16(pTf->candList.aSP[i].z);
		pTf->candList.aSP[i].w = g_pTxtFImport->thinkXW16(pTf->candList.aSP[i].w);
		pTf->candList.aSP[i].h = g_pTxtFImport->thinkYH16(pTf->candList.aSP[i].h);
	}

	// �g�̕��A�����ύX
	stretchRect(pTf->candList.aSP, rc16->w, rc16->h);

	// �g�`��
	{
		sprite_t   *pSp = pTf->candList.aSP;
		uv_t       *pUv = pTf->candList.aUV;
		paneChip_t *pPn = pTf->candList.aPN;
		int spIndex, uvIndex;
		u_int col;

		g_pTxtFImport->loadTex(pk, &g_TF.tex);											// �e�N�X�`�����[�h
		for(i=0; i<TF_FRAME_ELEMENT; i++, pPn++)
		{
			spIndex = pPn->spIndex;
			uvIndex = pPn->uvIndex;
			col = affectAlpha(pPn->rgba, pTf->alphaValue[TF_ALPHA_CANDLISTBG]);
			g_pTxtFImport->drawTex(pk, col, (pSp+spIndex), (pUv+uvIndex), &g_TF.tex);	// �e�N�X�`���`��
		}
	}
}
#endif
