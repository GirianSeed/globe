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
#include <string.h>

#include <eekernel.h>
#include <eetypes.h>
#include <sifdev.h>
#include <libpkt.h>
#include <libpad.h>
#include <libusbkb.h>

#include "libsoftkb.h"

// �v���g�^�C�v�錾
extern gImport_t		*g_pSkbImport;			// ���C�����֐��Q�̃|�C���^
extern protectedSkb_t	g_protectSkb;			// 



static void deleteResource(skey_t *pKey);
static void makePacketsCursor(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pTexCur);

static void ctrlPad(skey_t *pKey, u_short down, u_short up, u_short press, u_short repeat);
static void padUp(skey_t *pKey, u_short up);
static void padDown(skey_t *pKey, u_short down, u_short press, u_short repeat);
static void pushKey(skey_t *pKey);
static void releaseKey(skey_t *pKey);

static void atokCursor(skey_t *pKey, int dx, int dy);

static void moveMouseCursor(skey_t *pKey, iPoint_t *pPoint);
static void leftButtonDown(skey_t *pKey, iPoint_t *pPoint);
static void leftButtonUp(skey_t *pKey, iPoint_t *pPoint);

static u_int searchFocus(skey_t *pKeyNext, sprite_t *pNowSp);
static void moveKeybd(skey_t *pKey, int x, int y);
static void changeAlpha(skey_t *pKey, int index, u_char alpha);

void initKeyTop(skey_t *pKey);
static void InitSpecialKey(skey_t *pKey, u_char flag);

static void open(skey_t *pKey);
static void close(skey_t *pKey);


//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//  SkbCore_Init											ref:  skb_core.h
//
//  �\�t�g�E�F�A�L�[�{�[�h������
//
//  exportSkb_t *pExport : �O���Ƃ̂��Ƃ�p�|�C���^
//
//  �߂�l �F void
void SkbCore_Init(exportSkb_t *pExport)
{
	SKB_ASSERT(pExport);

	// �g����֐��|�C���^��ݒ�
	pExport->readResource   = readResourceFrame;
	pExport->deleteResource = deleteResource;

	pExport->makePacketsSkbBody = makePacketsBody;
	pExport->makePacketsCursor  = makePacketsCursor;
	pExport->searchFocus    = searchFocus;


	pExport->setPad         = ctrlPad;
	pExport->mouseMove        = moveMouseCursor;
	pExport->mouseLButtonDown = leftButtonDown;
	pExport->mouseLButtonUp   = leftButtonUp;

	pExport->trans           = moveKeybd;
	pExport->changeAlpha     = changeAlpha;
	pExport->initKeyTop      = initKeyTop;

	pExport->searchFocus     = searchFocus;
	pExport->initSpecialKey  = InitSpecialKey;

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
//	void *pTmp : �e�\�t�g�E�F�A�L�[�{�[�h�^�C�v�Ŏ��R�Ɏg����|�C���^(���̃\�t�g�E�F�A�L�[�{�[�h�^�C�v�ł́A�g���Ă��Ȃ�)
//
//	�߂�l : int : 0:����/1:�G���[
int readResourceFrame(skey_t *pKey, const char *filename, u_int keyTopNo, inhibitChar_t *inhibitChar, void *pTmp)
{
	#ifndef SKBRES_LINK  //Makefile define
	int fd, ret, size, i;
	SKB_ASSERT(pKey);
	SKB_ASSERT(filename);
	SKB_ASSERT(inhibitChar);

	// ���Ƀ��\�[�X�p���������m�ۂ���Ă������ɂ͊J������
	if(pKey->pKeyBuff)
	{
		g_pSkbImport->mfree(pKey->pKeyBuff);
		pKey->pKeyBuff = NULL;
	}

	// �t�@�C���Ǎ���
	fd = sceOpen(filename, SCE_RDONLY);
	if(fd < 0)
	{
		g_pSkbImport->dprintf("ERROR %s, %d: readResourceFrame - open(%s)\n", __FILE__, __LINE__, filename);
		return 1;
	}
	size = sceLseek(fd, 0, SCE_SEEK_END);
	sceLseek(fd, 0, SCE_SEEK_SET);
	if(size <= 0)
	{
		g_pSkbImport->dprintf("ERROR %s, %d: readResourceFrame - size(%d)\n", __FILE__, __LINE__, size);
		return 1;
	}
	pKey->pKeyBuff = (char*)g_pSkbImport->malign(128, size);
	if(pKey->pKeyBuff == NULL)
	{
		g_pSkbImport->dprintf("ERROR %s, %d: readResourceFrame - malloc failed(%d)\n", __FILE__, __LINE__, size);
		return 1;
	}
	ret = sceRead(fd, (char*)pKey->pKeyBuff, size);
	sceClose(fd);
	#else
	int i;
	SKB_ASSERT(pKey);
	SKB_ASSERT(inhibitChar);
	pKey->pKeyBuff=(char*)filename;	//skbres.a �������N�����ꍇ�́A�A�h���X��n�������B
	#endif

	// �f�[�^�ݒ�
	{
		lump_t	*pLump = (lump_t*)pKey->pKeyBuff;

		if(pLump->length != 0)
		{
			// �X�v���C�g���ݒ�
			pKey->pSps    = (sprite_t*)(pKey->pKeyBuff + pLump->offset);
			pKey->uiSpNum = pLump->length / sizeof(sprite_t);
			g_pSkbImport->dprintf("MSG : readResourceFrame\n");
			g_pSkbImport->dprintf("    : 		sprite_num = %d, %d, %d\n", pKey->uiSpNum, pLump->offset, pLump->length);
		}

		pLump++;
		if(pLump->length != 0)
		{
			// UV���ݒ�
			pKey->pUvs    = (uv_t*)(pKey->pKeyBuff + pLump->offset);
			pKey->uiUvNum = pLump->length / sizeof(uv_t);
			g_pSkbImport->dprintf("    : 		uv_num = %d, %d, %d\n", pKey->uiUvNum, pLump->offset, pLump->length);
		}

		pLump++;
		if(pLump->length != 0)
		{
			// �L�[�{�[�h���ݒ�
			pKey->pBtns    = (button_t*)(pKey->pKeyBuff + pLump->offset);
			pKey->uiBtnNum = pLump->length / sizeof(button_t);
			g_pSkbImport->dprintf("    : 		button_num = %d, %d, %d\n", pKey->uiBtnNum, pLump->offset, pLump->length);
		}

		pLump++;
		if(pLump->length != 0)
		{
			// �w�i�`�b�v���ݒ�
			pKey->pChips    = (paneChip_t*)(pKey->pKeyBuff + pLump->offset);
			pKey->uiChipNum = pLump->length / sizeof(paneChip_t);
			g_pSkbImport->dprintf("    : 		paneChip_num = %d, %d, %d\n", pKey->uiChipNum, pLump->offset, pLump->length);
		}

		//
		pLump++;
		if(pLump->length != 0)
		{
			// �\�t�g�E�F�A�L�[�{�[�h���ݒ�
			pKey->pSkbInfo = (skbInfo_t*)(pKey->pKeyBuff + pLump->offset);
			g_pSkbImport->dprintf("    : 		skbInfo = %d, %d\n", pLump->offset, pLump->length);
		}
	}

	{
		sprite_t *pSp = pKey->pSps;
		uv_t     *pUv = pKey->pUvs;

		// �X�v���C�g��16�{����
		for(i=0; i<pKey->uiSpNum; i++)
		{
			pSp[i].x = g_pSkbImport->thinkXW16(pSp[i].x);
			pSp[i].y = g_pSkbImport->thinkYH16(pSp[i].y);
			pSp[i].z = g_pSkbImport->thinkXW16(pSp[i].z);
			pSp[i].w = g_pSkbImport->thinkXW16(pSp[i].w);
			pSp[i].h = g_pSkbImport->thinkYH16(pSp[i].h);
		}
		// UV��16�{����
		for(i=0; i<pKey->uiUvNum; i++)
		{
			pUv[i].u  = STG(pUv[i].u);
			pUv[i].v  = STG(pUv[i].v);
			pUv[i].tw = STG(pUv[i].tw-1);	// UV�g�p�ׁ̈F����256���Ƃ�����A
											// �e�N�Z���̕���0�`255�Ȃ̂�16�{����O��-1���āA
											// 16�{������ɔ��s�N�Z�����炷�ׂ�+8����
			pUv[i].th = STG(pUv[i].th-1);	// UV�g�p�ׁ̈F������256���Ƃ�����
											// �e�N�Z���̍�����0�`255�Ȃ̂�16�{����O��-1���āA
											// 16�{������ɔ��s�N�Z�����炷�ׂ�+8����
		}
	}

	// ���͋֎~�������w��
	pKey->pInhibitChar = inhibitChar;
	setInhibitChars(pKey, pKey->pInhibitChar);


	// �L�[�{�[�h�����ݒ�
	pKey->uiKeyTopNo = keyTopNo;
	setLayout(pKey);

	return 0;
}



//============================================================================
//  readResourceFrame
//
//  �L�[��`���\�[�X�j��
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
static void deleteResource(skey_t *pKey)
{
	SKB_ASSERT(pKey);

	if(pKey->pKeyBuff)
	{
		#ifndef SKBRES_LINK  //Makefile define
		g_pSkbImport->mfree(pKey->pKeyBuff);
		#else
		moveKeybd( pKey, 0, 0 );
		{
			int i;
			sprite_t *pSp = pKey->pSps;
			uv_t     *pUv = pKey->pUvs;

			// �X�v���C�g���W�����ɖ߂��B
			for(i=0; i<pKey->uiSpNum; i++)
			{
				pSp[i].x = pSp[i].x/g_pSkbImport->thinkXW16(1);
				pSp[i].y = pSp[i].y/g_pSkbImport->thinkYH16(1);
				pSp[i].z = pSp[i].z/g_pSkbImport->thinkXW16(1);
				pSp[i].w = pSp[i].w/g_pSkbImport->thinkXW16(1);
				pSp[i].h = pSp[i].h/g_pSkbImport->thinkYH16(1);
			}
			// UV�����ɖ߂��B
			for(i=0; i<pKey->uiUvNum; i++)
			{
				pUv[i].u  = (pUv[i].u -8)/16  ;
				pUv[i].v  = (pUv[i].v   )/16  ;
				pUv[i].tw = (pUv[i].tw-8)/16+1;
				pUv[i].th = (pUv[i].th-8)/16+1;
			}
		}
		#endif
		pKey->pKeyBuff = NULL;
	}
}


//============================================================================
//	makePacketsBody
//
//  �\�t�g�E�F�A�L�[�{�[�h�`��p�P�b�g�쐬
//
//	sceVif1Packet *pk : VIF�p�P�b�g�i�[�p�o�b�t�@�ւ̃|�C���^
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	textureInfo_t *pTexBtn : �{�^���e�N�X�`���ւ̃|�C���^
//	textureInfo_t *pTexChar: �����e�N�X�`���ւ̃|�C���^
//	textureInfo_t *pTexBg  : �w�i�e�N�X�`���ւ̃|�C���^
void makePacketsBody(sceVif1Packet *pk, skey_t *pKey,
						 textureInfo_t *pTexBtn, textureInfo_t *pTexChar, textureInfo_t *pTexBg)
{
	int i;
	u_char alpha;
	u_int col=0x80808080;
	button_t		*pBtn   = NULL;
	buttonState_t	*pState = NULL;
	paneChip_t		*pPane  = NULL;
	u_char			s       = SKBMASK_LAYOUT(pKey->ucShift);
	u_int			spIndex, uvIndex;

	SKB_ASSERT(pk);
	SKB_ASSERT(pKey);
	SKB_ASSERT(pTexBtn);
	SKB_ASSERT(pTexChar);
//	SKB_ASSERT(pTexBg);		// BG���v��Ȃ��ꍇ������

	if(pTexBg != NULL)
	{
		// �w�i�`��
		g_pSkbImport->loadTex(pk, pTexBg);
		for(i=0; i<pKey->uiChipNum; i++)
		{
			pPane = (pKey->pChips+i);

			spIndex = pPane->spIndex;
			uvIndex = pPane->uvIndex;

			// �F�ݒ�
			col = affectAlpha(pPane->rgba, pKey->ucAlpha[SKBALPHA_BG]);

			// �p�P�b�g�쐬
			g_pSkbImport->drawTex(pk, col, (pKey->pSps+spIndex), (pKey->pUvs+uvIndex), pTexBg);
		}
	}

	// �L�[�x�[�X�`��
	g_pSkbImport->loadTex(pk, pTexBtn);
	{
		for(i=0; i<pKey->uiBtnNum; i++)
		{
			pBtn = (pKey->pBtns+i);
			if(pBtn->animFlag & KEY_INVISIBLE || pBtn->animFlag & KEY_DUMMY)
			{
				// �_�~�[�{�^���͕`�悵�Ȃ�
				continue;
			}

			// �X�v���C�g�C���f�b�N�X�擾
			spIndex = pBtn->spBaseIndex;

			alpha = pKey->ucAlpha[SKBALPHA_BASE];
			// �F�ݒ�
			if(g_pSkbImport->skb.getAtokMode() && (g_protectSkb.ucUseMouse == 0))
			{
				// �ϊ����̎��́A�A���t�@�l��ݒ�
				alpha = SKBDEF_DISABLE_ALPHA;
			}

			if(pBtn->animFlag & KEY_DISABLE)
			{
				// ���͋֎~�����̃L�[��������
				uvIndex = pBtn->uvBaseIndex[0];		// �ʏ�e�N�X�`��
				// �F�ݒ�
				col = affectAlpha(DEFAULT_RGBA, alpha);
			}
			else
			{
				// �L�[�x�[�X��UV�C���f�b�N�X���擾
				uvIndex = pBtn->uvBaseIndex[0];		// �ʏ�e�N�X�`��
				if(((pKey->uiPush == 1) && (pKey->uiCursorPos == i)) || (pBtn->animFlag & KEY_PUSH))
				{
					uvIndex = pBtn->uvBaseIndex[1];		// ������Ԃ̃e�N�X�`��
				}

				if(((pBtn->state[s].ctrlCode == SKBCTRL_SHIFT_L) ||
				   (pBtn->state[s].ctrlCode == SKBCTRL_SHIFT_R) ||
					(pBtn->state[s].ctrlCode == SKBCTRL_CAPS) ||
					(pBtn->state[s].ctrlCode == SKBCTRL_KANJI)))
				{
					uvIndex = pBtn->uvBaseIndex[0];		// �ʏ�e�N�X�`��
					if(pBtn->animFlag & KEY_PUSH)
					{
						uvIndex = pBtn->uvBaseIndex[1];		// ������Ԃ̃e�N�X�`��
					}
				}

				col = affectAlpha(pBtn->rgba, alpha);
			}

			// �p�P�b�g�쐬
			g_pSkbImport->drawTex(pk, col, (pKey->pSps+spIndex), (pKey->pUvs+uvIndex), pTexBtn);
		}
	}

	// �L�[�g�b�v�`��
	g_pSkbImport->loadTex(pk, pTexChar);
	{
		for(i=0; i<pKey->uiBtnNum; i++)
		{
			pBtn   = (pKey->pBtns+i);
			pState = &pBtn->state[s];
			if(pBtn->animFlag & KEY_INVISIBLE || pBtn->animFlag & KEY_DUMMY)
			{
				// �_�~�[�{�^���͕`�悵�Ȃ�
				continue;
			}

			// �F�ݒ�
			if(pBtn->animFlag & KEY_DISABLE)
			{
				col = DEFAULT_DISABLE_RGBA;
			}
			else
			{
				col = pBtn->rgba;
			}

			// �A���t�@�ݒ�
			alpha = pKey->ucAlpha[SKBALPHA_BASE];
			if(g_pSkbImport->skb.getAtokMode() && (g_protectSkb.ucUseMouse == 0))
			{
				// �ϊ����̎��́A�A���t�@�l��ݒ�
				alpha = SKBDEF_DISABLE_ALPHA;
			}

			col = affectAlpha(col, alpha);

			spIndex = pBtn->spCharIndex;
			uvIndex = pState->uvCharIndex[0];

			// SHIFT��������Ă�����[Backspace]��[Delete]�ɂȂ�
			if((pState->ctrlCode == SKBCTRL_BACKSPACE) && (s & LAYOUT_SHIFT))
			{
				uvIndex = pState->uvCharIndex[1];
			}
			// �S�p��������[��/�S]�{�^����ؑ�
			if((pState->ctrlCode == SKBCTRL_KANJI) && (s & LAYOUT_ZENKAKU))
			{
				uvIndex = pState->uvCharIndex[1];
			}

			// �t�H�[�J�X�F�ݒ�
			//  �J�[�\���ԍ��ƈ�v�ADISABLE�L�[����Ȃ��AATOK�̏�Ԃ��ϊ����łȂ�
			if((pKey->uiCursorPos == i) && !(pBtn->animFlag & KEY_DISABLE) &&
			   !g_pSkbImport->skb.getAtokMode() && (g_protectSkb.ucUseMouse == 0))
			{
				col = SKBDEF_ATTENTION_COL | SKBDEF_ATTENTION_ALPHA(pKey->ucAlpha[SKBALPHA_CURSOR]);
			}

			// �p�P�b�g�쐬
			g_pSkbImport->drawTex(pk, col, (pKey->pSps+spIndex), (pKey->pUvs+uvIndex), pTexChar);
		}
	}
}


//============================================================================
//  makePacketsCursor
//
//	�\�t�g�E�F�A�L�[�{�[�h�J�[�\���p�P�b�g�쐬
//
//	sceVif1Packet *pk : VIF�p�P�b�g�i�[�p�o�b�t�@�ւ̃|�C���^
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	textureInfo_t *pTexCur : �J�[�\���e�N�X�`���ւ̃|�C���^
static void makePacketsCursor(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pTexCur)
{
	SKB_ASSERT(pk);
	SKB_ASSERT(pKey);
	SKB_ASSERT(pTexCur);

	if(g_pSkbImport->skb.getAtokMode())
	{
		// ATOK��ON�̎��́A�\�����Ȃ�
		return;
	}

	{
		int i;
		int col = affectAlpha(0x80808080, pKey->ucAlpha[SKBALPHA_CURSOR]);
		static u_int cur = 500000;
		static u_int posX = 0, posY = 0;
		u_int  nowCur  = pKey->uiCursorPos;
		u_int  spIndex = pKey->pBtns[nowCur].spBaseIndex;

		// �J�[�\���X�v���C�g�Čv�Z
		makeCursorSprite(&pKey->pSps[spIndex], pKey->spCursor, pKey->iCursorPos);	// �X�v���C�g�쐬

		cur  = pKey->uiCursorPos;
		posX = pKey->uiKbdPosX;
		posY = pKey->uiKbdPosY;

		// �`��
		g_pSkbImport->loadTex(pk, pTexCur);
		for(i=0; i<4; i++)
		{
			g_pSkbImport->drawTex(pk, col, &pKey->spCursor[i], (uv_t*)&g_uvCursors[i], pTexCur);
		}
	}
}


//============================================================================
//  makeCursorSprite
//
//	�\�t�g�E�F�A�L�[�{�[�h�J�[�\���X�v���C�g�쐬
//
//	sprite_t *pSpSrc : �X�v���C�g���f�[�^�ւ̃|�C���^
//	sprite_t *pSpDst : �X�v���C�g�ۑ���̃|�C���^
//	u_int floating : �A�j���[�V�����̓x����
void makeCursorSprite(sprite_t *pSpSrc, sprite_t *pSpDst, u_int floating)
{
	int    w, h;

	w = g_pSkbImport->thinkXW16(SKBCURSOR_SP_W);
	h = g_pSkbImport->thinkYH16(SKBCURSOR_SP_H);

	// ����
	pSpDst[0].x = pSpSrc->x - floating;
	pSpDst[0].y = pSpSrc->y - floating;
	pSpDst[0].z = pSpSrc->z;
	pSpDst[0].w = w;
	pSpDst[0].h = h;

	// �E��
	pSpDst[1].x = pSpSrc->x + pSpSrc->w - w + floating;
	pSpDst[1].y = pSpSrc->y - floating;
	pSpDst[1].z = pSpSrc->z;
	pSpDst[1].w = w;
	pSpDst[1].h = h;

	// ����
	pSpDst[2].x = pSpSrc->x - floating;
	pSpDst[2].y = pSpSrc->y + pSpSrc->h - h + floating;
	pSpDst[2].z = pSpSrc->z;
	pSpDst[2].w = w;
	pSpDst[2].h = h;

	// �E��
	pSpDst[3].x = pSpSrc->x + pSpSrc->w - w + floating;
	pSpDst[3].y = pSpSrc->y + pSpSrc->h - h + floating;
	pSpDst[3].z = pSpSrc->z;
	pSpDst[3].w = w;
	pSpDst[3].h = h;

}


//============================================================================
//  moveKeybd
//
//	�\�t�g�E�F�A�L�[�{�[�h�ړ�
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	int x : X�ʒu
//	int y : Y�ʒu
static void moveKeybd(skey_t *pKey, int x, int y)
{
	int i;
	int _x, _y;
	sprite_t *pSp = pKey->pSps;
	SKB_ASSERT(pKey);

	x = g_pSkbImport->thinkXW16(x);
	y = g_pSkbImport->thinkYH16(y);

	// �O��Ɠ����ꏊ�Ȃ�ړ������Ȃ�
	if(x == pKey->uiKbdPosX && y == pKey->uiKbdPosY)
	{
		return;
	}

	_x = x - pKey->uiKbdPosX;
	_y = y - pKey->uiKbdPosY;

	for( i=0; i<pKey->uiSpNum; i++ )
	{
		(pSp+i)->x += _x;
		(pSp+i)->y += _y;
	}

	pKey->uiKbdPosX = x;
	pKey->uiKbdPosY = y;
}


//============================================================================
//  changeAlpha
//
//	�A���t�@�l�ݒ�
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	int index : �{�^���̃C���f�b�N�X
//	u_char alpha : ���f���������A���t�@�l
static void changeAlpha(skey_t *pKey, int index, u_char alpha)
{
	if(index >= SKBALPHA_MAX)
	{
		return;
	}

	pKey->ucAlpha[index] = alpha;
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
	static u_short _down   = 0;
	static u_short _up     = 0;
	static u_short _press  = 0;
	static u_short _repeat = 0;

	SKB_ASSERT(pKey);
	if(down == _down && up == _up && press == _press && repeat == _repeat )
	{
		return;
	}

	padDown(pKey, down, press, repeat);
	padUp(pKey, up);

	_up     = up;
	_down   = down;
	_press  = press;
	_repeat = repeat;
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
	int dx=0, dy=0;
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
		#if 1//SKB_EX Sound
		SoftKB_se_play(SKBSE_CHANGEKEYTOP);
		#endif
		return;
	}

	// �����ꂽ�u�Ԃ̃L�[���D�悳���
	// �����ꂽ��
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
		#if 1//SKB_EX Sound
		if(down & SCE_PADR1) SoftKB_se_play(SKBSE_SHIFT);
		#endif
	}


	if((down & SCE_PADR2) || (press & SCE_PADR2))
	{
		onStateCtrlCode(pKey, SKBCTRL_RIGHT, KEY_PUSH);
	}
	if((down & SCE_PADRup) || (press & SCE_PADRup))
	{
		int index = 0;
		u_int uiTmp[][2] =
		{
			{0x00000020, 0x00000000},
			{0x00003000, 0x00000000},
		};
		if(pKey->ucShift & LAYOUT_ZENKAKU)
		{
			index = 1;
		}

		onStateKey(pKey, &uiTmp[index][0], KEY_PUSH);
	}
	if((down & SCE_PADRdown) || (press & SCE_PADRdown))
	{
									// ESC�{�^���́A�\�t�g�E�F�A�L�[�{�[�h��ɂȂ�
	}
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
			// �ϊ����Ȃ�S���m���ݒ�
			g_pSkbImport->skb.kakuteiAll();
			#if 1//SKB_EX Sound
			SoftKB_se_play(SKBSE_DETECTEXIT);
			#endif
		}
		else
		{
			pushEnter(pKey);								// �m��
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
		if(g_pSkbImport->skb.getAtokMode())
		{
			g_pSkbImport->skb.kakuteiPart();
			#if 1//SKB_EX Sound
			SoftKB_se_play(SKBSE_CANDDETECT);
			#endif
		}
		else
		{
			pushKey(pKey);			// �����Ԃ�
			pKey->uiPush = 1;		// �J�[�\���������t���O��ON
		}
	}
	if(repeat & SCE_PADselect){}
	if(repeat & SCE_PADstart){}

	// �J�[�\���ړ�
	{
		if(repeat & SCE_PADLup)
		{
			dy = -1;	// [��]
		}
		else if(repeat & SCE_PADLdown)
		{
			dy = +1;	// [��]
		}
	
		if(repeat & SCE_PADLleft)
		{
			dx = -1;	// [��]
		}
		else if(repeat & SCE_PADLright)
		{
			dx = 1;		// [��]
		}
		if(g_pSkbImport->skb.getAtokMode())
		{
			// ATOK/ON�� && �ϊ���
			atokCursor(pKey, dx, dy);
		}
		else
		{
			// ATOK/OFF��
			moveCursor(pKey, dx, dy);
		}
	}
}


//============================================================================
//	padUp
//
//	�p�b�h��������
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	u_short up : �{�^���������
static void padUp(skey_t *pKey, u_short up)
{
	SKB_ASSERT(pKey);
	if(up & SCE_PADL1){}
	if(up & SCE_PADR1)
	{
		releaseShift(pKey, 0);
	}
	if(up & SCE_PADL2)
	{
		offStateCtrlCode(pKey, SKBCTRL_LEFT, KEY_PUSH);
	}
	if(up & SCE_PADR2)
	{
		offStateCtrlCode(pKey, SKBCTRL_RIGHT, KEY_PUSH);
	}
	if(up & SCE_PADRup)
	{
		int index = 0;
		u_int uiTmp[][2] =
		{
			{0x00000020, 0x00000000},
			{0x00003000, 0x00000000},
		};
		if(pKey->ucShift & LAYOUT_ZENKAKU)
		{
			index = 1;
		}

		offStateKey(pKey, &uiTmp[index][0], KEY_PUSH);
	}
	if(up & SCE_PADRdown)
	{
	}
	if(up & SCE_PADRleft)
	{
		offStateCtrlCode(pKey, SKBCTRL_BACKSPACE, KEY_PUSH);
	}
	if(up & SCE_PADRright)
	{
		releaseKey(pKey);
		pKey->uiPush = 0;		// �J�[�\���������t���O��OFF
	}
	if(up & SCE_PADselect){}
	if(up & SCE_PADstart)
	{
		offStateCtrlCode(pKey, SKBCTRL_ENTER, KEY_PUSH);
	}
}


//============================================================================
//	pushEnter
//
//	Enter�Z�b�g
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
void pushEnter(skey_t *pKey)
{
	g_pSkbImport->skb.setChars(NULL, ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), USBKEYC_ENTER|USBKB_RAWDAT);
}


//============================================================================
//	pushLeftArrow
//
//	�������Z�b�g
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
void pushLeftArrow(skey_t *pKey)
{
	g_pSkbImport->skb.setChars(NULL, ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), USBKEYC_LEFT_ARROW|USBKB_RAWDAT);
}


//============================================================================
//	pushRightArrow
//
//	�������Z�b�g
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
void pushRightArrow(skey_t *pKey)
{
	g_pSkbImport->skb.setChars(NULL, ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), USBKEYC_RIGHT_ARROW|USBKB_RAWDAT);
}


//============================================================================
//	pushUpArrow
//
//	�������Z�b�g
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
void pushUpArrow(skey_t *pKey)
{
	if(pKey->ucShift & LAYOUT_FOCUSSHIFT)
	{
		// �t�H�[�J�X���킹�ăV�t�g���������́A�V�t�g��Ԃ𖳎��B
		g_pSkbImport->skb.setChars(NULL, 0, USBKEYC_UP_ARROW|USBKB_RAWDAT);
		return;
	}

	g_pSkbImport->skb.setChars(NULL, ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), USBKEYC_UP_ARROW|USBKB_RAWDAT);
}


//============================================================================
//	pushDownArrow
//
//	�������Z�b�g
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
void pushDownArrow(skey_t *pKey)
{
	// @
	if(pKey->ucShift & LAYOUT_FOCUSSHIFT)
	{
		// �t�H�[�J�X���킹�ăV�t�g���������́A�V�t�g��Ԃ𖳎��B
		g_pSkbImport->skb.setChars(NULL, 0, USBKEYC_DOWN_ARROW|USBKB_RAWDAT);
		return;
	}

	g_pSkbImport->skb.setChars(NULL, ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), USBKEYC_DOWN_ARROW|USBKB_RAWDAT);
}


//============================================================================
//	pushSpace
//
//	�X�y�[�X�����Z�b�g
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
void pushSpace(skey_t *pKey)
{
	int index = 0;
	u_int uiTmp[][2] =
	{
		{0x00000020, 0x00000000},	// ���p
		{0x00003000, 0x00000000},	// �S�p
	};

	if(pKey->ucShift & LAYOUT_ZENKAKU)
	{
		index = 1;
	}

	g_pSkbImport->skb.setChars(&uiTmp[index][0], ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), ' ');
}


//============================================================================
//	setLayout
//
//	�V�t�g���L�[�g�b�v�ɂ���Đݒ�
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
void setLayout(skey_t *pKey)
{
	SKB_ASSERT(pKey);

	// �L�[�g�b�v�ɂ���ČŗL�̐ݒ�𔽉f������
	switch(pKey->uiKeyTopNo)
	{
	case KEYTOP_HIRAGANA:
		// �Ђ炪��
		pKey->ucShift |= LAYOUT_ZENKAKU | LAYOUT_CAPS;		// �S�p��ݒ�
		onStateCtrlCode(pKey, SKBCTRL_CAPS, KEY_DISABLE);	// CapsLock�𖳌��ɂ���
		break;

	case KEYTOP_KATAKANA:
		// �J�^�J�i
		pKey->ucShift |= LAYOUT_ZENKAKU;					// �S�p��ݒ�
		onStateCtrlCode(pKey, SKBCTRL_KANJI, KEY_PUSH);		// ���p/�S�p/���� ��������Ԃɂ���
		break;

	default:
		pKey->ucShift = 0;
	}

	{
		int ucInputMode = 0;
		ucInputMode = SoftKB_GetInputMode(pKey);		// �S�p/���p���[�h���擾
		g_pSkbImport->skb.changeInputMode(ucInputMode);	// ���̓��[�h�ύX
	}
}


//============================================================================
//	pushShift
//
//	SHIFT �����A�V���[�g�J�b�g�Ŏw��
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	u_char flag : 0:�V���[�g�J�b�g�iR1�j/1:�J�[�\�������킹�ĉ����ꂽ
void pushShift(skey_t *pKey, u_char flag)
{
	if(flag == 0)
	{
		// �V���[�g�J�b�g�ł����ꂽ
		pKey->ucShift |= LAYOUT_SHIFT;
		pKey->ucShift &= ~LAYOUT_FOCUSSHIFT;
	}
	else
	{
		// SHIFT�{�^���������ꂽ
		pKey->ucShift ^= (LAYOUT_SHIFT|LAYOUT_FOCUSSHIFT);
	}

	setEnableChars(pKey);						// �S�ẴL�[��������
	setInhibitChars(pKey, pKey->pInhibitChar);	// ���͋֎~�L�[���Đݒ�

	if(pKey->ucShift & LAYOUT_SHIFT)
	{
		onStateCtrlCode(pKey, SKBCTRL_SHIFT_L, KEY_PUSH);
		onStateCtrlCode(pKey, SKBCTRL_SHIFT_R, KEY_PUSH);
	}
	else
	{
		offStateCtrlCode(pKey, SKBCTRL_SHIFT_L, KEY_PUSH);
		offStateCtrlCode(pKey, SKBCTRL_SHIFT_R, KEY_PUSH);
	}
}


//============================================================================
//	releaseShift
//
//	SHIFT ����
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	u_char flag : 0:�V���[�g�J�b�g�iR1�j/1:�J�[�\�������킹�ĉ����ꂽ
void releaseShift(skey_t *pKey, u_char flag)
{
	if(flag == 0)
	{
		// SHIFT�{�^���������[�X���ꂽ
		if(pKey->ucShift & LAYOUT_FOCUSSHIFT)
		{
		}
		else
		{
			pKey->ucShift &= ~(LAYOUT_SHIFT|LAYOUT_FOCUSSHIFT);
		}
	}
	else
	{
		// ���̑��̕����ɑ΂��ă{�^�������[�X���ꂽ
		if(pKey->ucShift & LAYOUT_FOCUSSHIFT)
		{
			pKey->ucShift &= ~(LAYOUT_SHIFT|LAYOUT_FOCUSSHIFT);
		}
	}


	if(!(pKey->ucShift & LAYOUT_SHIFT))
	{
		offStateCtrlCode(pKey, SKBCTRL_SHIFT_L, KEY_PUSH);
		offStateCtrlCode(pKey, SKBCTRL_SHIFT_R, KEY_PUSH);
	}

	setEnableChars(pKey);							// �S�ẴL�[��������
	setInhibitChars(pKey, pKey->pInhibitChar);		// ���͋֎~�L�[���Đݒ�
}


//============================================================================
//	pushCapsLock
//
//	CAPSLOCK���������ꂽ�Ƃ��̏���
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
void pushCapsLock(skey_t *pKey)
{
	pKey->ucShift ^= LAYOUT_CAPS;
	setEnableChars(pKey);							// �S�ẴL�[��������
	setInhibitChars(pKey, pKey->pInhibitChar);	// ���͋֎~�L�[���Đݒ�
	if(pKey->ucShift & LAYOUT_CAPS)
	{
		onStateCtrlCode(pKey, SKBCTRL_CAPS, KEY_PUSH);
	}
	else
	{
		offStateCtrlCode(pKey, SKBCTRL_CAPS, KEY_PUSH);
	}
	#if 1//SKB_EX Sound
	SoftKB_se_play(SKBSE_ONCAPSLOCK);
	#endif
}


//============================================================================
//	pushTab
//
//	TAB�����Z�b�g
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
void pushTab(skey_t *pKey)
{
	// SHIFT��������Ă����
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	u_int  uiUCS4[2];
	if(ucShift & LAYOUT_SHIFT)
	{
		// TODO : TAB�������������灩�����ɃL�����b�g�ړ�
		//		  ���̍s�̂�
	}
	else
	{
		uiUCS4[0] = 0x00000009;
		uiUCS4[1] = 0x0;
		g_pSkbImport->skb.setChars(uiUCS4, 0, 0);
		#if 1//SKB_EX Sound
		SoftKB_se_play(SKBSE_ONKEY);
		#endif
	}
}


//============================================================================
//	pushKanji
//
//	���p/�S�p/����
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
void pushKanji(skey_t *pKey)
{
	u_char mode;
	// �L�[�g�b�v�ɂ���ẮA���p���T�|�[�g���Ă��Ȃ���������
	if(pKey->uiKeyTopNo == KEYTOP_HIRAGANA)
	{
		// �u�Ђ炪�ȁv�̎��͑S�p�̂�
		//  DISABLE�Ȃ��Γ����Ă��Ȃ��B
		return;
	}

	pKey->ucShift ^= LAYOUT_ZENKAKU;
	if(pKey->ucShift & LAYOUT_ZENKAKU)
	{
		// �S�p
		onStateCtrlCode(pKey, SKBCTRL_KANJI, KEY_PUSH);
	}
	else
	{
		// ���p
		offStateCtrlCode(pKey, SKBCTRL_KANJI, KEY_PUSH);
	}

	// ���̓��[�h�ύX
	mode = SoftKB_GetInputMode(pKey);
	g_pSkbImport->skb.changeInputMode(mode);

	setEnableChars(pKey);						// �S�ẴL�[��������
	setInhibitChars(pKey, pKey->pInhibitChar);	// ���͋֎~�L�[���Đݒ�

	#if 1//SKB_EX Sound
	SoftKB_se_play(SKBSE_CHANGEZENHAN);
	#endif
}


//============================================================================
//	pushEsc
//
//	ESC�����Z�b�g
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
void pushEsc(skey_t *pKey)
{
	// SHIFT�ɑ΂���
	g_pSkbImport->skb.setChars(NULL, ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), USBKEYC_ESCAPE|USBKB_RAWDAT);
}


//============================================================================
//	pushBackSpace
//
//	BACKSPACE�����Z�b�g
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
void pushBackSpace(skey_t *pKey)
{
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	if(ucShift & LAYOUT_SHIFT)
	{
		u_short usFlag = USBKEYC_DELETE|USBKB_RAWDAT;
		if(g_pSkbImport->ucUseAtok &&
		   #if 1//SKB_EX BS<->DEL
		   g_pSkbImport->skb.getAtokInput() )
		   #else
		   (pKey->ucModebarType == MODEBAR_CHAR_HIRAGANA_J || pKey->ucModebarType == MODEBAR_CHAR_KATAKANA_J))
		   #endif
		{
			usFlag = USBKEYC_BS|USBKB_RAWDAT;
		}
		// SHIFT��������Ă��鎞�ɂ�DELETE�ɒu�������
		g_pSkbImport->skb.setChars(NULL, ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), usFlag);
	}
	else
	{
		g_pSkbImport->skb.setChars(NULL, ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), USBKEYC_BS|USBKB_RAWDAT);
	}
}


//============================================================================
//	onStateCtrlCode
//
//	�R���g���[���L�[���w�肳�ꂽ��Ԃɂ���
//	����́A�p�b�h����̒��ړ��͂Ŏg���܂��B
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	u_int ctrlCode : �R���g���[���L�[
//	u_char state : ���(KEY_PUSH/KEY_DISABLE/KEY_INVISIBLE��)
//
//	�߂�l : int : 0:����/1:�w�肳�ꂽ�R���g���[���L�[��������Ȃ�����
int onStateCtrlCode(skey_t *pKey, u_int ctrlCode, u_char state)
{
	int i;
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	SKB_ASSERT(pKey);

	for(i=0; i<pKey->uiBtnNum; i++)
	{
		if(ctrlCode == pKey->pBtns[i].state[ucShift].ctrlCode)
		{
			pKey->pBtns[i].animFlag |= state;
			return 0;
		}
	}

	return 1;
}


//============================================================================
//	offStateCtrlCode
//
//	�R���g���[���L�[���w�肳�ꂽ��Ԃ̂�OFF�ɂ���B
//	����́A�p�b�h����̒��ړ��͂Ŏg���܂��B
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	u_int ctrlCode : �R���g���[���L�[
//	u_char state : ���(KEY_PUSH/KEY_DISABLE/KEY_INVISIBLE��)
//
//	�߂�l : int : 0:����/1:�w�肳�ꂽ�R���g���[���L�[��������Ȃ�����
int offStateCtrlCode(skey_t *pKey, u_int ctrlCode, u_char state)
{
	int i;
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	SKB_ASSERT(pKey);

	for(i=0; i<pKey->uiBtnNum; i++)
	{
		if(ctrlCode == pKey->pBtns[i].state[ucShift].ctrlCode)
		{
			pKey->pBtns[i].animFlag &= ~state;
			return 0;
		}
	}

	return 1;
}


//============================================================================
//	onStateKey
//
//	���ʂ̃L�[���w�肳�ꂽ��Ԃ̂�ON�ɂ���B
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	u_int *key : ������ւ̃|�C���^
//	u_char state : ���(KEY_PUSH/KEY_DISABLE/KEY_INVISIBLE��)
//
//	�߂�l : int : 0:����/1:�w�肳�ꂽ�R���g���[���L�[��������Ȃ�����
int onStateKey(skey_t *pKey, u_int *key, u_char state)
{
	int i;
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	SKB_ASSERT(pKey);

	for(i=0; i<pKey->uiBtnNum; i++)
	{
		if((strcmp((char*)key, (char*)pKey->pBtns[i].state[ucShift].key) == 0) &&
		   !(pKey->pBtns[i].animFlag & (KEY_DUMMY|KEY_INVISIBLE|KEY_DISABLE)))
		{
			pKey->pBtns[i].animFlag |= state;
			return 0;
		}
	}

	return 1;
}


//============================================================================
//	offStateKey
//
//	�ʏ�L�[���w�肳�ꂽ��Ԃ̂�OFF�ɂ���B
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	u_int *key : ������ւ̃|�C���^
//	u_char state : ���(KEY_PUSH/KEY_DISABLE/KEY_INVISIBLE��)
//
//	�߂�l : int : 0:����/1:�w�肳�ꂽ�R���g���[���L�[��������Ȃ�����
int offStateKey(skey_t *pKey, u_int *key, u_char state)
{
	int i;
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	SKB_ASSERT(pKey);

	for(i=0; i<pKey->uiBtnNum; i++)
	{
		if((strcmp((char*)key, (char*)pKey->pBtns[i].state[ucShift].key) == 0) &&
		   !(pKey->pBtns[i].animFlag & (KEY_DUMMY|KEY_INVISIBLE|KEY_DISABLE)))
		{
			pKey->pBtns[i].animFlag &= ~state;
			return 0;
		}
	}

	return 1;
}


//============================================================================
//	pushKey
//
//	�����ݒ� �L�[�Ɋ��蓖�Ă�ꂽ�������R�[���o�b�N�ŕԂ�
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
static void pushKey(skey_t *pKey)
{
	u_int  uiNowCur = pKey->uiCursorPos;
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	SKB_ASSERT(pKey);

	// bin�t�@�C���ɂ́A�R���g���[���R�[�h�������Ă��Ȃ���A�L�[�R�[�h�������Ă���ꍇ������B
	if((pKey->pBtns[uiNowCur].state[ucShift].ctrlCode != 0))
	{
		u_int code = pKey->pBtns[uiNowCur].state[ucShift].ctrlCode;
		switch(code)
		{
		case SKBCTRL_CAPS:		pushCapsLock(pKey);		return;
		case SKBCTRL_SHIFT_L:
		case SKBCTRL_SHIFT_R:	pushShift(pKey, 1);
								#if 1//SKB_EX Sound
								SoftKB_se_play(SKBSE_SHIFT);
								#endif
								return;
		case SKBCTRL_BACKSPACE:	pushBackSpace(pKey);	break;
		case SKBCTRL_ENTER:		pushEnter(pKey);		break;
		case SKBCTRL_UP:		pushUpArrow(pKey);		break;
		case SKBCTRL_DOWN:		pushDownArrow(pKey);	break;
		case SKBCTRL_LEFT:		pushLeftArrow(pKey);	break;
		case SKBCTRL_RIGHT:		pushRightArrow(pKey);	break;
		case SKBCTRL_HELP:
			if(g_pSkbImport->isConv())
			{
				// �ϊ����̎��́A�L�[�g�b�v�̕ύX���ł��Ȃ�
				return;
			}

			g_pSkbImport->skb.showHelp(1);
			g_pSkbImport->skb.ucShowHelp = 1;
			releaseShift(pKey, 1);
			#if 1//SKB_EX Sound
			SoftKB_se_play(SKBSE_ONHELP);
			#endif
			break;
		case SKBCTRL_KANJI:		pushKanji(pKey);		return;
		case SKBCTRL_TAB:		pushTab(pKey);			break;
		case SKBCTRL_ABCQWERTY:
			g_pSkbImport->skb.abcToQwerty();
			g_pSkbImport->dprintf("MSG : push abc <--> qwerty\n");
			#if 1//SKB_EX Sound
			SoftKB_se_play(SKBSE_QWERTYABC);
			#endif
			break;
		default:
			g_pSkbImport->dprintf("ERROR : pushKey - non supported CTRL(%x)\n", code);
		}
	}
	else
	{
		if(g_protectSkb.ucUseMouse ||
		   pKey->pBtns[uiNowCur].state[ucShift].key[0] == 0x00000020 ||
		   pKey->pBtns[uiNowCur].state[ucShift].key[0] == 0x00003000)
		{
			// �}�E�X���g���ꍇ�́A�X�y�[�X�L�[�̓R���g���[���ɂ���
			g_pSkbImport->skb.setChars(pKey->pBtns[uiNowCur].state[ucShift].key, ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), ' ');
		}
		else
		{
			g_pSkbImport->skb.setChars(pKey->pBtns[uiNowCur].state[ucShift].key, 0,
									   pKey->pBtns[uiNowCur].state[ucShift].ctrlCode);
			releaseShift(pKey, 1);
		}
	}
}


//============================================================================
//	releaseKey
//
//	�{�^���������̃A�j���[�V�������̌㏈��
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
static void releaseKey(skey_t *pKey)
{
	u_int  uiNowCur = pKey->uiCursorPos;
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	SKB_ASSERT(pKey);

	if(pKey->pBtns[uiNowCur].state[ucShift].ctrlCode != 0)
	{
		u_int code = pKey->pBtns[uiNowCur].state[ucShift].ctrlCode;
		switch(code)
		{
		case SKBCTRL_CAPS:		//offStateCtrlCode(pKey, SKBCTRL_CAPS, KEY_PUSH);	break
			pKey->uiPush = 0;
			return;
		case SKBCTRL_SHIFT_L:
		case SKBCTRL_SHIFT_R:
			releaseShift(pKey, 0);
			pKey->uiPush = 0;
			return;
		case SKBCTRL_BACKSPACE:	offStateCtrlCode(pKey, SKBCTRL_BACKSPACE, KEY_PUSH);break;
		case SKBCTRL_ENTER:		offStateCtrlCode(pKey, SKBCTRL_ENTER, KEY_PUSH);break;
		case SKBCTRL_UP:		offStateCtrlCode(pKey, SKBCTRL_UP, KEY_PUSH);break;
		case SKBCTRL_DOWN:		offStateCtrlCode(pKey, SKBCTRL_DOWN, KEY_PUSH);break;
		case SKBCTRL_LEFT:		offStateCtrlCode(pKey, SKBCTRL_LEFT, KEY_PUSH);break;
		case SKBCTRL_RIGHT:		offStateCtrlCode(pKey, SKBCTRL_RIGHT, KEY_PUSH);break;
		case SKBCTRL_HELP:		offStateCtrlCode(pKey, SKBCTRL_HELP, KEY_PUSH);break;
		case SKBCTRL_KANJI:
			pKey->uiPush = 0;
			return;
		case SKBCTRL_TAB:		offStateCtrlCode(pKey, SKBCTRL_TAB, KEY_PUSH);	break;
		case SKBCTRL_ABCQWERTY:
			g_pSkbImport->dprintf("MSG : push abc <--> qwerty\n");
			break;
		}
	}
	else
	{
		releaseShift(pKey, 1);
	}
}


//============================================================================
//	atokCursor
//
//	ATOK�g�p���ɂ͕����L�[�ɃA�T�C�������
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	int dx : �R���g���[���̍��E���L�[
//	int dy : �R���g���[���̏㉺���L�[
static void atokCursor(skey_t *pKey, int dx, int dy)
{
	if(dx == 0 && dy == 0)
	{
		return;
	}

	if(dy)
	{
		if(dy > 0)
		{
			pushDownArrow(pKey);	// �������L�[
		}
		else
		{
			pushUpArrow(pKey);		// �������L�[
		}
	}

	if(dx)
	{
		if(dx > 0)
		{
			pushRightArrow(pKey);	// �������L�[
		}
		else
		{
			pushLeftArrow(pKey);	// �������L�[
		}
	}
}


//============================================================================
//	moveCursor
//
//	�J�[�\���ړ�
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	int dx : �ړ���(X�l)
//	int dy : �ړ���(Y�l)
void moveCursor(skey_t *pKey, int dx, int dy)
{
	enum{ _up, _down, _left, _right };
	int d = _up;
	u_int nowCursor = pKey->uiCursorPos;
	#if 1//SKB_EX Cursor
	u_int backupCursor = pKey->uiCursorPos;
	u_int searchCount;
	#endif
	SKB_ASSERT(pKey);

	if(dx == 0 && dy == 0)
	{
		return;
	}

	if(dy)
	{
		int y;
		if(dy > 0)
		{
			for(y=0; y<dy; y++)
			{
				nowCursor = pKey->uiCursorPos;
				pKey->uiCursorPos = pKey->pBtns[nowCursor].direction[_down];
			}
			d = _down;
		}
		else
		{
			for(y=dy; y<0; y++)
			{
				nowCursor = pKey->uiCursorPos;
				pKey->uiCursorPos = pKey->pBtns[nowCursor].direction[_up];
			}
			d = _up;
		}

		// DISABLE, INVISIBLE �𖳎�
		nowCursor = pKey->uiCursorPos;
		#if 1//SKB_EX Cursor
		searchCount = 32;
		#endif
		while(1)
		{
			if(!(pKey->pBtns[nowCursor].animFlag & KEY_INVISIBLE) &&
			   !(pKey->pBtns[nowCursor].animFlag & KEY_DISABLE))
			{
				pKey->uiCursorPos = nowCursor;
				break;
			}
			nowCursor = pKey->pBtns[nowCursor].direction[d];
			#if 1//SKB_EX Cursor
			searchCount--;
			if(searchCount==0){
				pKey->uiCursorPos=backupCursor;
				break;
			}
			#endif
		}
	}

	if(dx)
	{
		int x, d;
		if(dx > 0)
		{
			for(x=0; x<dx; x++)
			{
				nowCursor = pKey->uiCursorPos;
				pKey->uiCursorPos = pKey->pBtns[nowCursor].direction[_right];
			}
			d = _right;
		}
		else
		{
			for(x=dx; x<0; x++)
			{
				nowCursor = pKey->uiCursorPos;
				pKey->uiCursorPos = pKey->pBtns[nowCursor].direction[_left];
			}
			d = _left;
		}

		// DISABLE, INVISIBLE �𖳎�
		nowCursor = pKey->uiCursorPos;
		#if 1//SKB_EX Cursor
		searchCount = 32;
		#endif
		while(1)
		{
			if(!(pKey->pBtns[nowCursor].animFlag & KEY_INVISIBLE) &&
			   !(pKey->pBtns[nowCursor].animFlag & KEY_DISABLE))
			{
				pKey->uiCursorPos = nowCursor;
				break;
			}
			nowCursor = pKey->pBtns[nowCursor].direction[d];
			#if 1//SKB_EX Cursor
			searchCount--;
			if(searchCount==0){
				pKey->uiCursorPos=backupCursor;
				break;
			}
			#endif
		}
	}

}


//============================================================================
//	searchFocus
//
//	���݂̃J�[�\���ʒu���玟��I�������\�t�g�E�F�A�L�[�{�[�h�̃J�[�\���̏ꏊ�����߂�
//
//	skey_t *pKeyNext : ����I��\��̃\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	sprite_t *pNowSp : ���ݑI�𒆂̃\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//
//	�߂�l : u_int : 0:�����l/0�ȊO:����I��\��̃\�t�g�E�F�A�L�[�{�[�h�̃L�[�C���f�b�N�X
static u_int searchFocus(skey_t *pKeyNext, sprite_t *pNowSp)
{
	int cx =0, cy =0;
	int i;
	u_int spIndex;

	sprite_t *pSp = pNowSp;
	cx = pSp->x + (pSp->w>>1);
	cy = pSp->y + (pSp->h>>1);


	// ���ɑI������L�[�{�[�h�̃t�H�[�J�X�ʒu�����߂�
	{
		button_t *pBtn=NULL;
		for(i=0; i<pKeyNext->uiBtnNum; i++)
		{
			pBtn = &pKeyNext->pBtns[i];
			spIndex = pBtn->spBaseIndex;
			pSp = &pKeyNext->pSps[spIndex];

			if((pSp->x <= cx) && (pSp->y <= cy) &&
			   ((pSp->x+pSp->w) > cx) && ((pSp->y+pSp->h) > cy) &&
			   !(pKeyNext->pBtns[i].animFlag & (KEY_INVISIBLE|KEY_DISABLE|KEY_DUMMY)))
			{
				return i;
			}
		}

		// �S��̂͂�߂�B
		return 0;
	}
}


//============================================================================
//	moveMouseCursor
//
//	�}�E�X�J�[�\���ړ�
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	iPoint_t *pPoint : �}�E�X�J�[�\���̈ʒu
static void moveMouseCursor(skey_t *pKey, iPoint_t *pPoint)
{
	int keyNo = SKBMOUSE_NULL;
	SKB_ASSERT(pKey);

	if((keyNo=searchKeyByPoint(pKey, pPoint)) != -1)
	{
		// ���������̂Ńt�H�[�J�X�ړ�
		pKey->uiCursorPos = keyNo;
	}
}


//============================================================================
//	leftButtonDown
//
//	�}�E�X���{�^������
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	iPoint_t *pPoint : �}�E�X�J�[�\���̈ʒu
static void leftButtonDown(skey_t *pKey, iPoint_t *pPoint)
{
	int keyNo = SKBMOUSE_NULL;
	SKB_ASSERT(pKey);

	// �L�[�ʒu����
	keyNo = searchKeyByPoint(pKey, pPoint);
	if(keyNo == SKBMOUSE_NULL)
	{
		// HIT���Ă���L�[����������
		if(g_protectSkb.uiOldCurPos[0] != SKBMOUSE_NULL)
		{
			pKey->uiPush = 0;				// �J�[�\���������t���O��OFF
			g_protectSkb.uiOldCurPos[0] = SKBMOUSE_NULL;
		}
		return;
	}

	if(!(g_protectSkb.uiDragState & SKBMOUSE_LBTNDOWN))
	{
		// ���{�^����������
		pKey->uiCursorPos = keyNo;
		pKey->uiPush  = 1;					// �J�[�\���������t���O��ON

		g_protectSkb.uiOldCurPos[0] = pKey->uiCursorPos;	// �h���b�O�����܂ܑ��̃{�^���Ɉړ��������̌��o�t���O��ۑ�
		g_protectSkb.uiDragState |= SKBMOUSE_LBTNDOWN;
	}
	else
	{
		if(g_protectSkb.uiOldCurPos[0] != keyNo)
		{
			// �h���b�O�����܂ܑ��̃{�^���Ɉړ�����
			pKey->uiPush = 0;				// �J�[�\���������t���O��OFF
			g_protectSkb.uiOldCurPos[0] = SKBMOUSE_NULL;
		}
	}
}


//============================================================================
//	leftButtonUp
//
//	�}�E�X���{�^������
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	iPoint_t *pPoint : �}�E�X�J�[�\���̈ʒu
static void leftButtonUp(skey_t *pKey, iPoint_t *pPoint)
{
	int keyNo = SKBMOUSE_NULL;
	SKB_ASSERT(pKey);

	// �}�E�X�|�C���^
	keyNo = searchKeyByPoint(pKey, pPoint);
	if(g_protectSkb.uiOldCurPos[0] == pKey->uiCursorPos)
	{
		pushKey(pKey);			// ������ݒ�
	}

	pKey->uiPush = 0;			// �J�[�\���������t���O��OFF
	g_protectSkb.uiOldCurPos[0] = SKBMOUSE_NULL;
	g_protectSkb.uiDragState &= ~SKBMOUSE_LBTNDOWN;		// Drag�t���O������
}


//============================================================================
//	searchKeyByPoint
//
//	�}�E�X���W�ʒu����HIT���Ă���L�[�ԍ���T��
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	iPoint_t *pPoint : �}�E�X�J�[�\���̈ʒu
int searchKeyByPoint(skey_t *pKey, iPoint_t *pPt)
{
	int spIndex;
	int cx = g_pSkbImport->thinkXW16(pPt->x);
	int cy = g_pSkbImport->thinkYH16(pPt->y);
	int i;
	sprite_t *pSp = NULL;

	// ���ɑI������L�[�{�[�h�̃t�H�[�J�X�ʒu�����߂�
	button_t *pBtn=NULL;
	for(i=0; i<pKey->uiBtnNum; i++)
	{
		pBtn = &pKey->pBtns[i];
		spIndex = pBtn->spBaseIndex;
		pSp = &pKey->pSps[spIndex];

		if((pSp->x <= cx) && (pSp->y <= cy) &&
		   ((pSp->x+pSp->w) > cx) && ((pSp->y+pSp->h) > cy) &&
		   !(pKey->pBtns[i].animFlag & KEY_INVISIBLE) &&
		   !(pKey->pBtns[i].animFlag & KEY_DISABLE) &&
		   !(pKey->pBtns[i].animFlag & KEY_DUMMY))
		{
			return i;
		}
	}

	return -1;	// ������Ȃ�����
}


//============================================================================
//	setInhibitChars
//
//	���͋֎~�����𔽉f
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	inhibitChar_t *inhibitChar : ���͋֎~�����\���̂ւ̃|�C���^
void setInhibitChars(skey_t *pKey, inhibitChar_t *inhibitChar)
{
	u_int i, s;
	u_int charNum = inhibitChar->uiCharNum;
	u_int ctrlNum = inhibitChar->uiCKeyNum;
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);

	SKB_ASSERT(pKey);
	SKB_ASSERT(inhibitChar);

	for(i=0; i<pKey->uiBtnNum; i++)
	{
		// �R���g���[���R�[�h�͑��삵�Ȃ�
		if(pKey->pBtns[i].state[ucShift].ctrlCode != 0)
		{
			continue;
		}

		for(s=0; s<charNum; s++)
		{
			if(memcmp((char*)pKey->pBtns[i].state[ucShift].key, (char*)inhibitChar->pChar[s], 10) == 0)
			{
				// ���͋֎~�����t���O���Ă�
				pKey->pBtns[i].animFlag |= KEY_DISABLE;
				goto NEXT;
			}
		}

		for(s=0; s<ctrlNum; s++)
		{
			if(pKey->pBtns[i].state[ucShift].ctrlCode == inhibitChar->pCtrlKey[s])
			{
				// ���͋֎~�R���g���[���L�[
				pKey->pBtns[i].animFlag |= KEY_DISABLE;
				break;
			}
		}

	NEXT:;
	}
}


//============================================================================
//	setEnableChars
//
//	�_�~�[�iKEY_INVISIBLE�j�L�[�ȊO�̃L�[�S�Ă�
//	�L���ɂ���B
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
void setEnableChars(skey_t *pKey)
{
	u_int i;
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	SKB_ASSERT(pKey);

	for(i=0; i<pKey->uiBtnNum; i++)
	{
		// �R���g���[���R�[�h�͑��삵�Ȃ�
		if(pKey->pBtns[i].state[ucShift].ctrlCode != 0)
		{
			continue;
		}

		if(!(pKey->pBtns[i].animFlag & KEY_INVISIBLE))
		{
			pKey->pBtns[i].animFlag &= ~KEY_DISABLE;
		}
	}
}


//============================================================================
//	setReleseChars
//
//	�S�ẴL�[��KEY_PUSH��Ԃ�����
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
void setReleseChars(skey_t *pKey)
{
	u_int i;
	SKB_ASSERT(pKey);

	for(i=0; i<pKey->uiBtnNum; i++)
	{
		pKey->pBtns[i].animFlag &= ~KEY_PUSH;
	}
}


//============================================================================
//	initKeyTop
//
//	�\�t�g�E�F�A�L�[�{�[�h�̃L�[�g�b�v������
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
void initKeyTop(skey_t *pKey)
{
	setReleseChars(pKey);						// �S�ẴL�[��PUSH��Ԃ�����
	setEnableChars(pKey);						// �S�ẴL�[��L����
	setInhibitChars(pKey, pKey->pInhibitChar);	// ���͋֎~�L�[���Đݒ�

	setLayout(pKey);							// ���[�h�ɂ���ăL�[�g�b�v�̏�Ԃ�ݒ�

}


//============================================================================
//	InitSpecialKey
//
//	���ꂫ�[������
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	u_char flag : 0:LAYOUT_FOCUSSHIFT�͖���/1:LAYOUT_FOCUSSHIFT���l��
static void InitSpecialKey(skey_t *pKey, u_char flag)
{
	u_int i;
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	SKB_ASSERT(pKey);

	for(i=0; i<pKey->uiBtnNum; i++)
	{
		// �ʏ핶���R�[�h�͑��삵�Ȃ�
		if(pKey->pBtns[i].state[ucShift].ctrlCode == 0xFFFFFFFF)
		{
			continue;
		}

		if(pKey->pBtns[i].state[ucShift].ctrlCode == SKBCTRL_SHIFT_L ||
		   pKey->pBtns[i].state[ucShift].ctrlCode == SKBCTRL_SHIFT_R)
		{
			if(flag)
			{
				if(!(pKey->ucShift & LAYOUT_FOCUSSHIFT))
				{
					pKey->ucShift &= ~LAYOUT_SHIFT;
					pKey->pBtns[i].animFlag &= ~KEY_PUSH;
				}
			}
			else
			{
				pKey->ucShift &= ~(LAYOUT_SHIFT|LAYOUT_FOCUSSHIFT);
				pKey->pBtns[i].animFlag &= ~KEY_PUSH;
			}
			continue;
		}
		else if(pKey->pBtns[i].state[ucShift].ctrlCode != SKBCTRL_CAPS &&
		   pKey->pBtns[i].state[ucShift].ctrlCode != SKBCTRL_KANJI)
		{
			pKey->pBtns[i].animFlag &= ~KEY_PUSH;
		}
	}
	setEnableChars( pKey );                     // �S�ẴL�[��L����
	setInhibitChars( pKey, pKey->pInhibitChar );// ���͋֎~�L�[���Đݒ�
}


//============================================================================
//	open
//
//	�\�t�g�E�F�A�L�[�{�[�h��OPEN�������̏����i��j
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
static void open(skey_t *pKey)
{
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


