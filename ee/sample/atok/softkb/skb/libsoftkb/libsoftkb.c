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
// �\�t�g�E�F�A�L�[�{�[�h�����̕����R�[�h��UCS2(int�^�łƂ��Ă���܂���)�ł��B

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <eekernel.h>
#include <eetypes.h>
#include <sifdev.h>
#include <libscf.h>
#include <libpkt.h>
#include <libpad.h>

#include "libsoftkb.h"
#include "help.inc"
#include "modebar.inc"
#include "resdef.h"			// ���\�[�X�t�@�C�������`

// �v���g�^�C�v�錾
gImport_t		*g_pSkbImport = NULL;		// ���C�����֐��Q�̃|�C���^
protectedSkb_t	g_protectSkb;				// SOFTKB�֐��Q������Q�Ƃł���\����

extern void SkbCore_Init(exportSkb_t *pExport);
extern void SkbList_Init(exportSkb_t *pExport);


// �e�N�X�`���o�b�t�@������`
enum
{
	TEX_BG,
	TEX_KEYTOP,
	TEX_BTN,
	TEX_MODE,
	TEX_HELPITEMS,

	TEX_HELPCHARS,

	TEX_MAX
};



static struct
{
	u_int			uiModebarItemNum;	// ���[�h�o�[�v�f��
	u_int			uiCreateNum;		// �쐬��

	// ��ʎ���
	textureInfo_t	tex[TEX_MAX];		// �e�N�X�`��
	sprite_t		spCursor[4+1];		// �J�[�\�� �X�v���C�g
	uv_t			uvCursor[4+1];		// �J�[�\�� UV

	// ���[�h�o�[
	u_int			auiModebarType[KEYTOP_NUM_MAX];	// ���[�h�o�[�^�C�v��
													// MAX : 10��

	int				modebarPos16[2];				// ���[�h�o�[�ʒu
	sprite_t		modebarBgSp[KEYTOP_NUM_MAX];	// ���[�h�o�[�X�v���C�g�w�i
	sprite_t		modebarCharSp[KEYTOP_NUM_MAX];	// ���[�h�o�[�X�v���C�g����

	// pad
	u_short			up;
	u_short			down;
	u_short			press;
	u_short			repeat;
	u_int			padCnt[16];			// �����Ɉ�̃{�^�������������Ȃ��ׂ̃o�b�t�@

	// �w���v
	sprite_t		SpHelpItems[10];	// �w���v �{�^�����A�C�e�� ���[�N
	sprite_t		SpHelpChars[10];	// �w���v ���� ���[�N
	sprite_t		SpHelpTitle;		// "��{������" ���[�N
	sprite_t		SpAtokVer;			// ATOK�o�[�W����
	sprite_t		SpAtokLogo;			// ATOK���S
	sprite_t		SpFrame[9][9];		// �t���[��(9�X�v���C�g/1�t���[��)
	iRect_t			rcFrame[9];

	// �}�E�X
	u_int			uiBtn;				// �{�^�����
										// 1bit : ���{�^��
										// 2bit : �E�{�^��
										// 4bit : �z�C�[��
	iPoint_t		mousePos;			// �}�E�X�ʒu���
	u_int			uiWheel;			// �z�C�[�����


} g_libskb;


static void initHelp(void);
static void setFrame(sprite_t *sp0, iRect_t *rc, int x, int y);
static void drawHelp(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pChars, textureInfo_t *pItems);
static void makeModebars(void);
static void makePackets_modebars(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pTexBtn, textureInfo_t *pTexChar);
static void openingAnim(skey_t *pKey);
static void closingAnim(skey_t *pKey);
static int searchModebarByPoint(skey_t *pKey, iPoint_t *pt);

//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//  SoftKB_Init												ref:  libsoftkb.h
//
//  ������
//
//  gImport_t *pImport : �R�[���o�b�N�֐��Q�|�C���^
//  �߂�l : int : 0:����������/1:���������s
int SoftKB_Init(gImport_t *pImport)
{
	int i, typeNum, lang;
	SKB_ASSERT(pImport);

	memset(&g_libskb, 0x0, sizeof(g_libskb));			// ���[�N������
	memset(&g_protectSkb, 0x0, sizeof(protectedSkb_t));	// ���[�N������
	g_pSkbImport = pImport;						// ���C�����֐��Q�̃|�C���^

	#if 1//SKB_EX BinLink
	//init_g_RSC();
		#ifndef REGION_J_OTHER_ONLY
	g_RSC[ 0].caKeyTopFilename = aiuHira_bin; // 50���z��Ђ炪��
	g_RSC[ 1].caKeyTopFilename = aiuKana_bin; // 50���z��J�^�J�i
	g_RSC[ 2].caKeyTopFilename = abc_bin;     // ABC�z��
	g_RSC[ 3].caKeyTopFilename = kigo_bin;    // �L��(����{�^������)
	g_RSC[ 4].caKeyTopFilename = num_bin;     // ����(���{�ŁF�S�p���p�{�^���t��)
	g_RSC[ 5].caKeyTopFilename = qwerty_bin;  // ���{
		#endif
		#ifndef REGION_J_ONLY
	g_RSC[ 6].caKeyTopFilename = usQ_bin;			// �A�����J
	g_RSC[ 7].caKeyTopFilename = ukQ_bin;			// �C�M���X
	g_RSC[ 8].caKeyTopFilename = frenchQ_bin;		// �t�����X
	g_RSC[ 9].caKeyTopFilename = spanishQ_bin;		// �X�y�C��
	g_RSC[10].caKeyTopFilename = germanQ_bin;		// �h�C�c
	g_RSC[11].caKeyTopFilename = italianQ_bin;		// �C�^���A
	g_RSC[12].caKeyTopFilename = dutchQ_bin;		// �I�����_
	g_RSC[13].caKeyTopFilename = portugQ_bin;		// �|���g�K��
	g_RSC[14].caKeyTopFilename = cfrenchQ_bin;		// �J�i�f�B�A���t�����`
	g_RSC[15].caKeyTopFilename = numF_bin;			// ����
	g_RSC[16].caKeyTopFilename = spSign_bin;		// �L��(�ʉ�etc)
	g_RSC[17].caKeyTopFilename = idiomChr_bin;		// �����L���t������
		#endif
	#endif
	typeNum = (sizeof(g_RSC)/sizeof(rsc_t));
	if(typeNum != KEYTOP_MAX)
	{
		// �L�[�z�u���\�[�X�̐���Define�ƈ�v���Ă��Ȃ�������G���[
		g_pSkbImport->dprintf("ERROR %s, %d: SoftKB_Init - resource num wrong.\n", __FILE__, __LINE__);
		return 1;
	}

	// �e�N�X�`����S�����[�h(�w���v����������)
	for(i=0; i<TEX_HELPCHARS; i++)
	{
		g_pSkbImport->readTex(&g_libskb.tex[i], caTexFilename[i]);
	}

	// �e�N�X�`���w���v�����̂݃��[�h
	lang = g_pSkbImport->usLang;
	g_pSkbImport->readTex(&g_libskb.tex[TEX_HELPCHARS], &caTexLangFilename[lang][0]);
	initHelp();

	return 0;
}



//============================================================================
//  SoftKB_Destroy											ref:  libsoftkb.h
//
//  �I������
//
//  �߂�l : void
void SoftKB_Destroy(void)
{
	int i;

	// �e�N�X�`����S���j��
	for(i=0; i<TEX_MAX; i++)
	{
		g_pSkbImport->deleteTex(&g_libskb.tex[i]);
	}
}


//============================================================================
// SoftKB_UseMouse											ref:  libsoftkb.h
//
// �\�t�g�E�F�A�L�[�{�[�h�Ń}�E�X���g��
//
void SoftKB_UseMouse(u_char flag)
{
	if(flag)
	{
		flag = 1;	// �g��
	}

	g_protectSkb.ucUseMouse = flag;
}


//============================================================================
// SoftKB_CheckShown										ref:  libsoftkb.h
//
// �\�t�g�E�F�A�L�[�{�[�h���o�������H
//
//  �߂�l : int
int SoftKB_CheckShown(void)
{
	return g_pSkbImport->skb.ucShowSKbd;
}


//============================================================================
//  SoftKB_Create											ref:  libsoftkb.h
//
//  �\�t�g�E�F�A�L�[�{�[�h�쐬
//
//  skey_t *pKey      : �L�[�\���̂ւ̃|�C���^
//  iPoint_t *skbPoint: �\�t�g�E�F�A�L�[�{�[�h�̈ʒu�ւ̃|�C���^
//  u_int keyTopNo    : �L�[�g�b�v�ԍ�
//  inhibitChar_t *inhibitChar : ���͋֎~�����ւ̃|�C���^
//  void *pTmp        : �e�L�[�{�[�h�Ŏ��R�Ɏg����
//
//  �߂�l : int 0(����)/1(���s)
int SoftKB_Create(skey_t *pKey, iPoint_t *skbPoint, u_int keyTopNo, inhibitChar_t *inhibitChar, void *pTmp)
{
	int ret;
	SKB_ASSERT(pKey);
	SKB_ASSERT(skbPoint);
	SKB_ASSERT(inhibitChar);

	if(keyTopNo > KEYTOP_IDIOMCHAR)
	{
		// �L�[�g�b�v�̔ԍ�������
		g_pSkbImport->dprintf("ERROR %s, %d: SoftKB_Create - non support keyNo.\n", __FILE__, __LINE__);
		return 1;
	}

	// �\�t�g�E�F�A�L�[�{�[�h�\���̏�����
	memset(pKey, 0x0, sizeof(skey_t));

	SkbCore_Init(&pKey->export);
	if(keyTopNo == KEYTOP_SIGN)
	{
		// �����ꗗ�̎��́A
		// �ʏ�L�[�{�[�h�ƈႤ������ݒ�
		SkbList_Init(&pKey->export);
	}

	// ���\�[�X�t�@�C���Ǎ���
	ret = pKey->export.readResource(pKey, &g_RSC[keyTopNo].caKeyTopFilename[0], keyTopNo, inhibitChar, pTmp);
	if(ret)
	{
		g_pSkbImport->dprintf("ERROR %s, %d: SoftKB_Create - failed to read resource.\n", __FILE__, __LINE__);
		return 1;
	}
	pKey->uiKeyTopNo  = keyTopNo;
	pKey->ucInputMode = g_RSC[keyTopNo].fepType;	// ���\�[�X�ɂ����̓��[�h��ݒ�
	pKey->ucModebarType = g_RSC[keyTopNo].modeType;	// ���[�h�^�C�v��ݒ�

	g_libskb.uiCreateNum++;
	g_pSkbImport->dprintf("MSG : SoftKB_Create - created keyboard(%d).\n", g_libskb.uiCreateNum);
	return 0;
}


//============================================================================
//  SoftKB_Delete											ref:  libsoftkb.h
//
// �\�t�g�E�F�A�L�[�{�[�h �폜
//
//  �߂�l : void
void SoftKB_Delete(skey_t *pKey)
{
	// ���\�[�X�t�@�C���폜
	pKey->export.deleteResource(pKey);

	--g_libskb.uiCreateNum;
}


//============================================================================
//  SoftKB_SetModebarType									ref:  libsoftkb.h
//
//  ���[�h�o�[�ݒ�
//
//  const u_int *pTypes : �^�C�v���̓������|�C���^
//  int typeNum         : �^�C�v���̐�
//
//  �߂�l : void
void SoftKB_SetModebarType(const u_int *pTypes, int typeNum)
{
	if(typeNum >= KEYTOP_NUM_MAX)
	{
		g_pSkbImport->dprintf("ERROR : SoftKB_SetModebarType - over %d\n", typeNum);
		return;
	}

	memcpy(g_libskb.auiModebarType, pTypes, typeNum*sizeof(int));	// ���[�h�o�[�^�C�v�ݒ�

	g_libskb.uiModebarItemNum = typeNum;							// ���[�h�o�[�v�f���ݒ�
	makeModebars();
}


//============================================================================
//  SoftKB_ClearModebarType									ref:  libsoftkb.h
//
//  ���[�h�o�[��������
//
//  �߂�l : void
void SoftKB_ClearModebarType(void)
{
	g_libskb.uiModebarItemNum = 0;
}


//============================================================================
//  SoftKB_Open												ref:  libsoftkb.h
//
//  �\�t�g�E�F�A�L�[�{�[�h �I�[�v������
//
//  skey_t *pKey      : �L�[�\���̂ւ̃|�C���^
//
//  �߂�l : void
void SoftKB_Open(skey_t *pKey)
{
	pKey->export.initKeyTop(pKey);		// �L�[�g�b�v�ύX

	pKey->iPhase = SKBPHASE_OPENING;	// �t�F�[�Y�ݒ�

	pKey->export.open(pKey);
	pKey->export.initSpecialKey(pKey, 0);

	g_pSkbImport->dprintf("MSG : SoftKB_Open\n");
}


//============================================================================
//  SoftKB_Close											ref:  libsoftkb.h
//
//  �\�t�g�E�F�A�L�[�{�[�h �N���[�Y����
//
//  skey_t *pKey      : �L�[�\���̂ւ̃|�C���^
//
//  �߂�l : void
void SoftKB_Close(skey_t *pKey)
{
	pKey->iPhase = SKBPHASE_CLOSING;		// �t�F�[�Y�ݒ�

	pKey->export.close(pKey);

	g_pSkbImport->dprintf("MSG : SoftKB_Close\n");
}


//============================================================================
//  SoftKB_Run												ref:  libsoftkb.h
//
//  �\�t�g�E�F�A�L�[�{�[�h ���t���[������
//
//  sceVif1Packet *pk : �p�P�b�g�o�b�t�@�ւ̃|�C���^
//  skey_t *pKey      : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//
//  �߂�l : void
void SoftKB_Run(sceVif1Packet *pk, skey_t *pKey)
{
	SKB_ASSERT(pk);
	SKB_ASSERT(pKey);

	if(!g_pSkbImport->skb.ucShowSKbd && !g_pSkbImport->skb.ucShowHelp)
	{
		// �\�t�g�E�F�A�L�[�{�[�h����\�� ���� �w���v���\������Ă��Ȃ���
		return;
	}

	switch(pKey->iPhase)
	{
	case SKBPHASE_OPENING:
		openingAnim(pKey);
		break;

	case SKBPHASE_CLOSING:
		closingAnim(pKey);
		break;

	case SKBPHASE_CLOSED:
		break;

	case SKBPHASE_OPENED:
	default:
		// �ʏ폈��
		if(g_pSkbImport->skb.ucShowHelp)
		{
			// �w���v�`��
			drawHelp(pk, pKey, &g_libskb.tex[TEX_HELPCHARS], &g_libskb.tex[TEX_HELPITEMS]);
		}
		else
		{
			// �p�P�b�g�쐬
			pKey->export.makePacketsSkbBody(pk, pKey, &g_libskb.tex[TEX_BTN], &g_libskb.tex[TEX_KEYTOP], &g_libskb.tex[TEX_BG]);
			pKey->export.makePacketsCursor (pk, pKey, &g_libskb.tex[TEX_BTN]);

			makePackets_modebars(pk, pKey, &g_libskb.tex[TEX_BTN], &g_libskb.tex[TEX_KEYTOP]);
		}
		break;
	}
}


//============================================================================
//  SoftKB_SetPadData										ref:  libsoftkb.h
//
//  �p�b�h���ݒ�
//
//  u_long paddata : �p�b�h���
//
//  �߂�l : void
void SoftKB_SetPadData(skey_t *pKey, u_long paddata)
{
	u_short down, press, repeat, up;
	SKB_ASSERT(pKey);

	// �I�[�v�����A�N���[�Y���A�N���[�Y��
	if(pKey->iPhase == SKBPHASE_OPENING || pKey->iPhase == SKBPHASE_CLOSING || pKey->iPhase == SKBPHASE_CLOSED)
	{
		return;
	}

	// �I�[�v����
	up     = (paddata>>48)	& 0xFFFFU;
	down   = (paddata>>32)	& 0xFFFFU;
	press  = (paddata>>16)	& 0xFFFFU;
	repeat = (paddata)		& 0xFFFFU;

	if(!g_pSkbImport->skb.ucShowSKbd && !g_pSkbImport->skb.ucShowHelp)
	{
		// �\�t�g�E�F�A�L�[�{�[�h����\�� ����
		// �w���v���\������Ă���ꍇ�́APAD����n���Ȃ�
		return;
	}


	// ���~�����{�^���������ꂽ��A�����L�[�͖����B
	if( press & ( SCE_PADRup | SCE_PADRdown | SCE_PADRleft | SCE_PADRright ) ){
		press &=  ~( SCE_PADLup | SCE_PADLdown | SCE_PADLleft | SCE_PADLright );
		repeat&=  ~( SCE_PADLup | SCE_PADLdown | SCE_PADLleft | SCE_PADLright );
		down  &=  ~( SCE_PADLup | SCE_PADLdown | SCE_PADLleft | SCE_PADLright );
	}


	if(g_pSkbImport->skb.ucShowHelp)
	{
		// �w���v�\����
		if(down & SCE_PADRdown)
		{
			// �w���v����
			g_pSkbImport->skb.ucShowHelp = 0;
			g_pSkbImport->skb.showHelp(0);

			pKey->export.initSpecialKey(pKey, 1);
			pKey->uiPush = 0;
			#if 1//SKB_EX Sound
			SoftKB_se_play(SKBSE_CANCEL);
			#endif
		}
	}
	else
	{
		// �p�b�h����
		pKey->export.setPad(pKey, down, up, press, repeat);
	}
}


//============================================================================
//  SoftKB_SetMouseData										ref:  libsoftkb.h
//
//  �}�E�X���ݒ�
//
//  u_long128 data : �}�E�X���
//
//  �߂�l : void
void SoftKB_SetMouseData(skey_t *pKey, u_long128 *pData)
{
	static int lbtnState = SKBMODE_NOHIT;
	static int dragState = 0;
	int hitNo = SKBMODE_NOHIT;
	int _x, _y, _w, _btn;
	char *cPtr = NULL;
	SKB_ASSERT(pKey);
	SKB_ASSERT(pData);

	if(!g_protectSkb.ucUseMouse)
	{
		// �}�E�X�͎g��Ȃ�
		return;
	}

	// �I�[�v�����A�N���[�Y���A�N���[�Y��
	if(pKey->iPhase == SKBPHASE_OPENING || pKey->iPhase == SKBPHASE_CLOSING || pKey->iPhase == SKBPHASE_CLOSED)
	{
		return;
	}
	if(!g_pSkbImport->skb.ucShowSKbd && !g_pSkbImport->skb.ucShowHelp)
	{
		// �\�t�g�E�F�A�L�[�{�[�h����\�� ���� �w���v���\������Ă��Ȃ���
		return;
	}

	cPtr = (u_char *)pData;
	if(cPtr[0] == 0)
	{
		return;		// �}�E�X���̍X�V���Ȃ��ꍇ�́A������
	}

	_btn = cPtr[1];		// �{�^��
	_x   = cPtr[2];		// X����
	_y   = cPtr[3];		// Y����
	_w   = cPtr[4];		// �z�C�[��

	// �}�E�X�̈ړ��͈͂𐧌�
	g_libskb.mousePos.x += _x;
	g_libskb.mousePos.y += _y;
	if(g_libskb.mousePos.x < 0)
	{
		g_libskb.mousePos.x = 0;
	}
	if(g_libskb.mousePos.y < 0)
	{
		g_libskb.mousePos.y = 0;
	}
	if(g_libskb.mousePos.x > g_pSkbImport->uiScreenW)
	{
		g_libskb.mousePos.x = g_pSkbImport->uiScreenW;
	}
	if(g_libskb.mousePos.y > g_pSkbImport->uiScreenH)
	{
		g_libskb.mousePos.y = g_pSkbImport->uiScreenH;
	}

	if(g_pSkbImport->skb.ucShowHelp)
	{
		if(SKBMOUSE_LBTNDOWN & _btn)
		{
			// �w���v����
			g_pSkbImport->skb.ucShowHelp = 0;
			g_pSkbImport->skb.showHelp(0);

			pKey->export.initSpecialKey(pKey, 1);
			return;
		}
	}

	// CAUTION : �}�E�X���́A�X�V���ꂽ���̂ݑ��M�����
	// �N���b�N ////////////
	// ���{�^��
	if(SKBMOUSE_LBTNDOWN & _btn)
	{
		// ����
		hitNo = searchModebarByPoint(pKey, &g_libskb.mousePos);
		if(hitNo == SKBMODE_NOHIT)
		{
			// �L�[
			pKey->export.mouseLButtonDown(pKey, &g_libskb.mousePos);
		}
		else
		{
			// ���[�h�o�[
			if(!dragState)
			{
				// ��������
				lbtnState = hitNo;		// ���[�h�o�[�������ꂽ
				dragState = 0;			// �h���b�O�t���OON
			}
			else
			{
				// �h���b�O
				if(lbtnState != hitNo)
				{
					lbtnState = SKBMODE_NOHIT;			// ���[�h�o�[�������ꂽ
				}
			}
		}
		g_libskb.uiBtn |= SKBMOUSE_LBTNDOWN;
	}
	else if(!(SKBMOUSE_LBTNDOWN & _btn) && (SKBMOUSE_LBTNDOWN & g_libskb.uiBtn))
	{
		// ����
		hitNo = searchModebarByPoint(pKey, &g_libskb.mousePos);
		if(hitNo == SKBMODE_NOHIT)
		{
			// �����ݒ�
			pKey->export.mouseLButtonUp(pKey, &g_libskb.mousePos);
		}
		else
		{
			// ���[�h�o�[
			if(lbtnState == hitNo)
			{
				g_pSkbImport->skb.keytopChange(hitNo);	// �L�[�g�b�v�ύX�p�R�[���o�b�N�Ăяo��
			}
			dragState = 0;								// �h���b�O�t���OOFF
		}
		g_libskb.uiBtn &= ~SKBMOUSE_LBTNDOWN;
	}

#if 0
	// �E�{�^��
	if(SKBMOUSE_RBTNDOWN & _btn)
	{
		// ����
	}
	else if(!(SKBMOUSE_LBTNDOWN & _btn) && (SKBMOUSE_LBTNDOWN & g_libskb.uiBtn))
	{
		// ����
	}

	// �z�C�[��
	if(SKBMOUSE_WBTNDOWN & _btn)
	{
		// ����
	}
	else if(!(SKBMOUSE_WBTNDOWN & _btn) && (SKBMOUSE_WBTNDOWN & g_libskb.uiBtn))
	{
		// ����
	}
#endif

	pKey->export.mouseMove(pKey, &g_libskb.mousePos);	// �}�E�X�ړ�
}


//============================================================================
//  SoftKB_Move												ref:  libsoftkb.h
//
//  �\�t�g�E�F�A�L�[�{�[�h�ړ�
//
//  skey_t *pKey : �L�[�{�[�h�\���̂ւ̃|�C���^
//  int x        : �ړI�ʒuX
//  int y        : �ړI�ʒuY
//
//  �߂�l : void
void SoftKB_Move(skey_t *pKey, int x, int y)
{
	pKey->export.trans(pKey, x, y);	// �ړ�
}


//============================================================================
//  SoftKB_MoveModebar										ref:  libsoftkb.h
//
//  ���[�h�o�[�ړ�
//
//  int x        : �ړI�ʒuX
//  int y        : �ړI�ʒuY
//
//  �߂�l : void
void SoftKB_MoveModebar(int x, int y)
{
	g_libskb.modebarPos16[0] = g_pSkbImport->thinkXW16(x);
	g_libskb.modebarPos16[1] = g_pSkbImport->thinkYH16(y);

	makeModebars();		// ���[�h�o�[�쐬
}


//============================================================================
//  SoftKB_ChangeAlpha										ref:  libsoftkb.h
//
//  �A���t�@���ύX
//
//  skey_t *pKey : �L�[�{�[�h�\���̂ւ̃|�C���^
//  int index    : �A���t�@�l�z��̃C���f�b�N�X(SKBALPHA_BG etc)
//  u_char alpha : �A���t�@�l
//
//  �߂�l : void
void SoftKB_ChangeAlpha(skey_t *pKey, int index, u_char alpha)
{
	if(index < 0 || index > SKBALPHA_MAX)
	{
		// �\������Ȃ��C���f�b�N�X
		g_pSkbImport->dprintf("ERROR : SoftKB_ChangeAlpha - non support alpha(%d)\n", index);
		return;
	}

	pKey->export.changeAlpha(pKey, index, alpha);	// �A���t�@�l�ύX
}


//============================================================================
//  SoftKB_GetInputMode										ref:  libsoftkb.h
//
//  ���̓��[�h�擾
//
//  skey_t *pKey : �L�[�{�[�h�\���̂ւ̃|�C���^
//
//  �߂�l : u_char : ���̓��[�h
u_char SoftKB_GetInputMode(skey_t *pKey)
{
//	pKey->ucShift
	u_char flag = 0;

	if(pKey->ucShift & LAYOUT_ZENKAKU)
	{
		flag = SKBMODE_ZENKAKU;
	}

	return (pKey->ucInputMode | flag);
}


//============================================================================
//  SoftKB_GetSize											ref:  libsoftkb.h
//
//  ���̓��[�h�擾
//
//  skey_t *pKey : �L�[�{�[�h�\���̂ւ̃|�C���^
//  int *w : ��
//  int *h : ����
//
//  �߂�l : void
void SoftKB_GetSize(skey_t *pKey, int *w, int *h)
{
	*w = pKey->pSkbInfo->w;
	*h = pKey->pSkbInfo->h;
}


//============================================================================
//  SoftKB_SearchFocus										ref:  libsoftkb.h
//
//  �\�t�g�E�F�A�L�[�{�[�h�̃t�H�[�J�X���擾
//
//	skey_t *pKeyNow  : ���ݑI������Ă���\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	skey_t *pKeyNext : ����I�������\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
u_int SoftKB_SearchFocus(skey_t *pKeyNow, skey_t *pKeyNext)
{
	// ���݂̃t�H�[�J�X�ʒu(x,y,w,h)���擾
	sprite_t *pSp;
	int cur   = pKeyNow->uiCursorPos;
	int index = pKeyNow->pBtns[cur].spBaseIndex;
	pSp = &pKeyNow->pSps[index];

	// ���ɑI�������L�[�g�b�v���(x,y,w,h)�ɍł��߂��ꏊ��T���ă��^�[��
	return pKeyNext->export.searchFocus(pKeyNext, pSp);
}


//============================================================================
//  SoftKB_DrawFocus										ref:  libsoftkb.h
//
//  �t�H�[�J�X�ݒ聕�`��
//  �\�t�g�E�F�A�L�[�{�[�h�ȊO��
//  �\�t�g�E�F�A�L�[�{�[�h�Ɠ����t�H�[�J�X�g�������ꍇ
//
//  sceVif1Packet *pk : VIF�p�P�b�g�o�b�t�@�ւ̃|�C���^
//  sprite_t *sp : �t�H�[�J�X�����Ă�����`
//  sprite_t *focus : �t�H�[�J�X�̃X�v���C�g4��
//  u_int curPos    : �J�[�\���̃t���t���x
//  u_int col    : �F
//
void SoftKB_DrawFocus(sceVif1Packet *pk, sprite_t *sp, u_int curPos, u_int col)
{
	int i;
	sprite_t aSpFocus[4];
	makeCursorSprite(sp, aSpFocus, curPos);

	// �`��
	g_pSkbImport->loadTex(pk, &g_libskb.tex[TEX_BTN]);
	for(i=0; i<4; i++)
	{
		g_pSkbImport->drawTex(pk, col, &aSpFocus[i], (uv_t*)&g_uvCursors[i], &g_libskb.tex[TEX_BTN]);
	}
}


//============================================================================
//  SoftKB_GetMouseBtn										ref:  libsoftkb.h
//
//  �{�^�����擾
//
//  �߂�l : const u_int : �{�^�����
const u_int SoftKB_GetMouseBtn(void)
{
	return g_libskb.uiBtn;
}


//============================================================================
//  SoftKB_GetMousePoint									ref:  libsoftkb.h
//
//  �|�C���^���擾
//
//  �߂�l : const iPoint_t* : �ʒu���ւ̃|�C���^
const iPoint_t* SoftKB_GetMousePoint(void)
{
	return &g_libskb.mousePos;
}


//============================================================================
//  SoftKB_GetMouseWheel									ref:  libsoftkb.h
//
//  �z�C�[�����擾
//
//  �߂�l : const u_int : �z�C�[�����ւ̃|�C���^
const u_int SoftKB_GetMouseWheel(void)
{
	return g_libskb.uiWheel;
}


//============================================================================
//  SoftKB_SetStartCurPos									ref:  libsoftkb.h
//
//  �\�t�g�E�F�A�L�[�{�[�h�o������
//  �L�[�g�b�v�̎�ނɍ��킹�������J�[�\���ʒu��ݒ�
//
//  �߂�l : const u_int : �z�C�[�����ւ̃|�C���^
void SoftKB_SetStartCurPos(skey_t *pKey, u_int keyTopType)
{
	switch(keyTopType)
	{
	case KEYTOP_HIRAGANA:		// 50���z��Ђ炪��
	case KEYTOP_KATAKANA:		// 50���z��J�^�J�i
		pKey->uiCursorPos = 21;
		break;
	case KEYTOP_ABC:			// ABC�z��
		pKey->uiCursorPos = 12;
		break;
	case KEYTOP_SIGN:			// �L��(����{�^������)
	case KEYTOP_SIGNF:			// �L��(�ʉ�etc)
	case KEYTOP_IDIOMCHAR:		// �����L���t������
		pKey->uiCursorPos = 0;
		break;
	case KEYTOP_NUMJP:			// ����(���{�ŁF�S�p���p�{�^������)
	case KEYTOP_NUMF:			// ����(�C�O�ŁF�S�p���p�{�^������)
		pKey->uiCursorPos = 7;
		break;
	case KEYTOP_PC_JAPAN:		// ���{
		pKey->uiCursorPos = 30;
	case KEYTOP_PC_US:			// �A�����J
		pKey->uiCursorPos = 30;
		break;
	case KEYTOP_PC_FRANCE:		// �t�����X
		pKey->uiCursorPos = 16;
		break;
	case KEYTOP_PC_SPAIN:		// �X�y�C��
	case KEYTOP_PC_GERMAN:		// �h�C�c
	case KEYTOP_PC_DUTCH:		// �I�����_
	case KEYTOP_PC_ITALIA:		// �C�^���A
	case KEYTOP_PC_PORTUGRESS:	// �|���g�K��
	case KEYTOP_PC_UK:			// �C�M���X
	case KEYTOP_PC_CANADAF:		// �J�i�f�B�A���t�����`
		pKey->uiCursorPos = 29;
		break;

	default:
		g_pSkbImport->dprintf("ERROR : SoftKB_SetStartCurPos - Unknown keytopType\n");
	};
}


//============================================================================
// STATIC FUNCTIONS
//============================================================================
//============================================================================
//	openingAnim
//
//	�A�j���[�V�����J�n�p�R�[���o�b�N
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
static void openingAnim(skey_t *pKey)
{
	if(g_pSkbImport->skb.opening())
	{
		pKey->iPhase = SKBPHASE_OPENED;
	}
}


//============================================================================
//	closingAnim
//
//	�A�j���[�V�����I��
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
static void closingAnim(skey_t *pKey)
{
	if(g_pSkbImport->skb.closing())
	{
		pKey->iPhase = SKBPHASE_CLOSED;
	}
}


//============================================================================
//	makeModebars
//
//	���[�h�o�[�쐬
static void makeModebars(void)
{
	int i=0;
	int uvindex = 0;
	int keyNum = g_libskb.uiModebarItemNum;
	int mrw, mrh;
	int base = 0;

	if(keyNum == 0)
	{
		return;
	}

	memset(g_libskb.modebarBgSp,   0x0, sizeof(sprite_t)*KEYTOP_NUM_MAX);	// ���[�h�o�[�X�v���C�g�w�i������
	memset(g_libskb.modebarCharSp, 0x0, sizeof(sprite_t)*KEYTOP_NUM_MAX);	// ���[�h�o�[�X�v���C�g����������

	// �����̃��[�h��`��
	if(keyNum == 1)
	{
		// ���[�h��������Ȃ��ꍇ
		uvindex = MODEBAR_1;

		g_libskb.modebarBgSp[0].x = g_libskb.modebarPos16[0] + (i*MODEBAR_BASE_SP_W16);
		g_libskb.modebarBgSp[0].y = g_libskb.modebarPos16[1];
		g_libskb.modebarBgSp[0].z = 0;
		g_libskb.modebarBgSp[0].w = MODEBAR_BASE_SP_W16;
		g_libskb.modebarBgSp[0].h = g_pSkbImport->thinkYHn(MODEBAR_BASE_SP_H16);
	}
	else
	{
		for(i=0; i<keyNum; i++)
		{
			// sprite���쐬
			g_libskb.modebarBgSp[i].x = g_libskb.modebarPos16[0] + (i*MODEBAR_BASE_SP_W16);
			g_libskb.modebarBgSp[i].y = g_libskb.modebarPos16[1];
			g_libskb.modebarBgSp[i].z = 0;
			g_libskb.modebarBgSp[i].w = MODEBAR_BASE_SP_W16;
			g_libskb.modebarBgSp[i].h = g_pSkbImport->thinkYHn(MODEBAR_BASE_SP_H16);
		}
	}

	// �����̂ݕ`�� ------
	mrw = (MODEBAR_BASE_SP_W16 - MODEBAR_CHAR_SP_W16)/2;
	mrh = g_pSkbImport->thinkYHn(MODEBAR_BASE_SP_H16 - MODEBAR_CHAR_SP_H16) / 2;
	for(i=0; i<keyNum; i++)
	{
		if((g_libskb.auiModebarType[i] == MODEBAR_CHAR_ALPHABET_J ||
			g_libskb.auiModebarType[i] == MODEBAR_CHAR_ALPHABET_F ||
			g_libskb.auiModebarType[i] == MODEBAR_CHAR_NUM))
		{
			base = g_pSkbImport->thinkYH16(MODEBAR_CHAR_SP_DOWN);	// �x�[�X���C�������킹��
		}
		// sprite���쐬
		g_libskb.modebarCharSp[i].x = g_libskb.modebarPos16[0] + (i*MODEBAR_BASE_SP_W16) + mrw;
		g_libskb.modebarCharSp[i].y = g_libskb.modebarPos16[1] + mrh + base;
		g_libskb.modebarCharSp[i].z = 0;
		g_libskb.modebarCharSp[i].w = MODEBAR_CHAR_SP_W16;
		g_libskb.modebarCharSp[i].h = g_pSkbImport->thinkYHn(MODEBAR_CHAR_SP_H16);
		base = 0;
	}
}


//============================================================================
//	makePackets_modebars
//
//	�`��p���[�h�o�[�p�P�b�g�쐬
//
//	sceVif1Packet *pk : VIF�p�P�b�g�o�b�t�@�ւ̃|�C���^
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	textureInfo_t *pTexBtn  : �{�^���e�N�X�`���̃|�C���^
//	textureInfo_t *pTexChar : �����e�N�X�`���̃|�C���^
static void makePackets_modebars(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pTexBtn, textureInfo_t *pTexChar)
{
	// ���[�h�`��
	int i=0;
	int uvindex = 0;
	int keyNum = g_libskb.uiModebarItemNum;
	u_int col = DEFAULT_RGBA;
	int mrw, mrh;
	int inputMode = g_pSkbImport->skb.getNowInputMode();

	if(keyNum == 0)
	{
		return;
	}


	// �y��̂ݕ`��
	g_pSkbImport->loadTex(pk, pTexBtn);
	// �����̃��[�h��`��
	if(keyNum == 1)
	{
		// ���[�h��������Ȃ��ꍇ
		uvindex = MODEBAR_1;

		col = affectAlpha(DEFAULT_RGBA, pKey->ucAlpha[SKBALPHA_BG]);
		g_pSkbImport->drawTex(pk, col, &g_libskb.modebarBgSp[0], (uv_t*)&g_uvModebarCursor[uvindex], pTexBtn);
	}
	else
	{
		for(i=0; i<keyNum; i++)
		{
			if(i == 0)
			{
				uvindex = MODEBAR_LEFT;			// ��
			}
			else if(i == (keyNum-1))
			{
				uvindex = MODEBAR_RIGHT;		// �E
			}
			else
			{
				uvindex = MODEBAR_CENTER;		// ��
			}

			col = affectAlpha(DEFAULT_RGBA, pKey->ucAlpha[SKBALPHA_BG]);
			if(inputMode == i)
			{
				// ���ݎg����L�[�^�C�v
				g_pSkbImport->drawTex(pk, col, &g_libskb.modebarBgSp[i], (uv_t*)&g_uvModebarCursor[uvindex], pTexBtn);
			}
			else
			{
				g_pSkbImport->drawTex(pk, col, &g_libskb.modebarBgSp[i], (uv_t*)&g_uvModebarBase[uvindex], pTexBtn);
			}
		}
	}

	// �����̂ݕ`�� ------
	mrw = (MODEBAR_BASE_SP_W16 - MODEBAR_CHAR_SP_W16)/2;
	mrh = g_pSkbImport->thinkYHn(MODEBAR_BASE_SP_H16 - MODEBAR_CHAR_SP_H16) / 2;
	g_pSkbImport->loadTex(pk, pTexChar);
	for(i=0; i<keyNum; i++)
	{
		col = DEFAULT_RGBA;
		if(inputMode == i)
		{
			col = SKBDEF_ATTENTION_COL | SKBDEF_ATTENTION_ALPHA(pKey->ucAlpha[SKBALPHA_CHAR]);
		}
		uvindex = g_libskb.auiModebarType[i];
		g_pSkbImport->drawTex(pk, col, &g_libskb.modebarCharSp[i], (uv_t*)&g_uvModebarChar[uvindex], pTexChar);
	}
}


//============================================================================
//	initHelp
//
//	�w���v������
static void initHelp(void)
{
	int i;
	int atok = 0;
	u_int lang = g_pSkbImport->usLang;
	u_int region = g_pSkbImport->iRegion;
	helpMaps_t *pMap = &g_HelpMapsNTSC[lang];
	sprite_t *pSp = NULL;

	if(region == REGION_J)
	{
		atok = 0;
		if(lang == SCE_ENGLISH_LANGUAGE)
		{
			lang = HELP_J_ENGLISH;
			atok = 1;
		}
	}
	else
	{
		atok = -1;
		#if 1//SKB_EX LANG
		switch( lang ){
		default:
		case SCE_JAPANESE_LANGUAGE  :
		case SCE_ENGLISH_LANGUAGE   :	lang = HELP_ENGLISH;    break;
		case SCE_FRENCH_LANGUAGE    :	lang = HELP_FRENCH;     break;
		case SCE_SPANISH_LANGUAGE   :	lang = HELP_SPANISH;    break;
		case SCE_GERMAN_LANGUAGE    :	lang = HELP_GERMAN;     break;
		case SCE_ITALIAN_LANGUAGE   :	lang = HELP_ITALIAN;    break;
		case SCE_DUTCH_LANGUAGE     :	lang = HELP_DUTCH;      break;
		case SCE_PORTUGUESE_LANGUAGE:	lang = HELP_PORTUGUESE; break;
		}
		#endif
	}

	pMap = &g_HelpMapsNTSC[lang];

	// �X�v���C�g�����R�s�[
	// PAD�A�C�e��
	pSp = pMap->spItems;
	for(i=0; i<pMap->itemNum; i++)
	{
		g_libskb.SpHelpItems[i].x = g_pSkbImport->thinkXW16(pSp[i].x);
		g_libskb.SpHelpItems[i].y = g_pSkbImport->thinkYH16(pSp[i].y);
		g_libskb.SpHelpItems[i].z = g_pSkbImport->thinkXW16(pSp[i].z);
		g_libskb.SpHelpItems[i].w = g_pSkbImport->thinkXW16(pSp[i].w);
		g_libskb.SpHelpItems[i].h = g_pSkbImport->thinkYH16(pSp[i].h);
	}

	// �w���v������
	pSp = pMap->spChars;
	for(i=0; i<pMap->charNum; i++)
	{
		g_libskb.SpHelpChars[i].x = g_pSkbImport->thinkXW16(pSp[i].x);
		g_libskb.SpHelpChars[i].y = g_pSkbImport->thinkYH16(pSp[i].y);
		g_libskb.SpHelpChars[i].z = g_pSkbImport->thinkXW16(pSp[i].z);
		g_libskb.SpHelpChars[i].w = g_pSkbImport->thinkXW16(pSp[i].w);
		g_libskb.SpHelpChars[i].h = g_pSkbImport->thinkYH16(pSp[i].h);
	}

	// "��{�����ʁh
	g_libskb.SpHelpTitle.x = g_pSkbImport->thinkXW16(pMap->spTitle->x);
	g_libskb.SpHelpTitle.y = g_pSkbImport->thinkYH16(pMap->spTitle->y);
	g_libskb.SpHelpTitle.z = g_pSkbImport->thinkXW16(pMap->spTitle->z);
	g_libskb.SpHelpTitle.w = g_pSkbImport->thinkXW16(pMap->spTitle->w);
	g_libskb.SpHelpTitle.h = g_pSkbImport->thinkYH16(pMap->spTitle->h);

	#ifndef ATOK_NO_LINK
	if(atok != -1)
	{
		// ATOK�o�[�W����
		pSp = &g_spAtokVerSrc[atok];
		g_libskb.SpAtokVer.x = g_pSkbImport->thinkXW16(pSp->x);
		g_libskb.SpAtokVer.y = g_pSkbImport->thinkYH16(pSp->y);
		g_libskb.SpAtokVer.z = g_pSkbImport->thinkXW16(pSp->z);
		g_libskb.SpAtokVer.w = g_pSkbImport->thinkXW16(pSp->w);
		g_libskb.SpAtokVer.h = g_pSkbImport->thinkYH16(pSp->h);

		// ATOK���S
		pSp = &g_spAtokLogoSrc[atok];
		g_libskb.SpAtokLogo.x = g_pSkbImport->thinkXW16(pSp->x);
		g_libskb.SpAtokLogo.y = g_pSkbImport->thinkYH16(pSp->y);
		g_libskb.SpAtokLogo.z = g_pSkbImport->thinkXW16(pSp->z);
		g_libskb.SpAtokLogo.w = g_pSkbImport->thinkXW16(pSp->w);
		g_libskb.SpAtokLogo.h = g_pSkbImport->thinkYH16(pSp->h);
	}
	#endif

	for(i=0; i<9; i++)
	{
		g_libskb.rcFrame[i] = pMap->rcFrames[i];		// DUM

		setFrame(&g_libskb.SpFrame[i][0], &pMap->rcFrames[i],0,0);
	}
}


#if 1//SKB_EX HelpOffset
//============================================================================
//	HelpOffset
//
//	�w���v������ initHelp�I�t�Z�b�g�w���
static int helpOffsetX,helpOffsetY;
void HelpOffset( int x, int y )
{
	int i;
	int atok = 0;
	u_int lang = g_pSkbImport->usLang;
	u_int region = g_pSkbImport->iRegion;
	helpMaps_t *pMap = &g_HelpMapsNTSC[lang];
	sprite_t *pSp = NULL;

	if(region == REGION_J)
	{
		atok = 0;
		if(lang == SCE_ENGLISH_LANGUAGE)
		{
			lang = HELP_J_ENGLISH;
			atok = 1;
		}
	}
	else
	{
		atok = -1;
		#if 1//SKB_EX LANG
		switch( lang ){
		default:
		case SCE_JAPANESE_LANGUAGE  :
		case SCE_ENGLISH_LANGUAGE   :	lang = HELP_ENGLISH;    break;
		case SCE_FRENCH_LANGUAGE    :	lang = HELP_FRENCH;     break;
		case SCE_SPANISH_LANGUAGE   :	lang = HELP_SPANISH;    break;
		case SCE_GERMAN_LANGUAGE    :	lang = HELP_GERMAN;     break;
		case SCE_ITALIAN_LANGUAGE   :	lang = HELP_ITALIAN;    break;
		case SCE_DUTCH_LANGUAGE     :	lang = HELP_DUTCH;      break;
		case SCE_PORTUGUESE_LANGUAGE:	lang = HELP_PORTUGUESE; break;
		}
		#endif
	}

	helpOffsetX=g_pSkbImport->thinkXW16(x);
	helpOffsetY=g_pSkbImport->thinkYH16(y);

	pMap = &g_HelpMapsNTSC[lang];

	// �X�v���C�g�����R�s�[
	// PAD�A�C�e��
	pSp = pMap->spItems;
	for(i=0; i<pMap->itemNum; i++)
	{
		g_libskb.SpHelpItems[i].x = g_pSkbImport->thinkXW16(pSp[i].x+x);
		g_libskb.SpHelpItems[i].y = g_pSkbImport->thinkYH16(pSp[i].y+y);
		g_libskb.SpHelpItems[i].z = g_pSkbImport->thinkXW16(pSp[i].z);
		g_libskb.SpHelpItems[i].w = g_pSkbImport->thinkXW16(pSp[i].w);
		g_libskb.SpHelpItems[i].h = g_pSkbImport->thinkYH16(pSp[i].h);
	}

	// �w���v������
	pSp = pMap->spChars;
	for(i=0; i<pMap->charNum; i++)
	{
		g_libskb.SpHelpChars[i].x = g_pSkbImport->thinkXW16(pSp[i].x+x);
		g_libskb.SpHelpChars[i].y = g_pSkbImport->thinkYH16(pSp[i].y+y);
		g_libskb.SpHelpChars[i].z = g_pSkbImport->thinkXW16(pSp[i].z);
		g_libskb.SpHelpChars[i].w = g_pSkbImport->thinkXW16(pSp[i].w);
		g_libskb.SpHelpChars[i].h = g_pSkbImport->thinkYH16(pSp[i].h);
	}

	// ��{������
	g_libskb.SpHelpTitle.x = g_pSkbImport->thinkXW16(pMap->spTitle->x+x);
	g_libskb.SpHelpTitle.y = g_pSkbImport->thinkYH16(pMap->spTitle->y+y);
	g_libskb.SpHelpTitle.z = g_pSkbImport->thinkXW16(pMap->spTitle->z);
	g_libskb.SpHelpTitle.w = g_pSkbImport->thinkXW16(pMap->spTitle->w);
	g_libskb.SpHelpTitle.h = g_pSkbImport->thinkYH16(pMap->spTitle->h);

	#ifndef ATOK_NO_LINK
	if(atok != -1)
	{
		// ATOK�o�[�W����
		pSp = &g_spAtokVerSrc[atok];
		g_libskb.SpAtokVer.x = g_pSkbImport->thinkXW16(pSp->x+x);
		g_libskb.SpAtokVer.y = g_pSkbImport->thinkYH16(pSp->y+y);
		g_libskb.SpAtokVer.z = g_pSkbImport->thinkXW16(pSp->z);
		g_libskb.SpAtokVer.w = g_pSkbImport->thinkXW16(pSp->w);
		g_libskb.SpAtokVer.h = g_pSkbImport->thinkYH16(pSp->h);

		// ATOK���S
		pSp = &g_spAtokLogoSrc[atok];
		g_libskb.SpAtokLogo.x = g_pSkbImport->thinkXW16(pSp->x+x);
		g_libskb.SpAtokLogo.y = g_pSkbImport->thinkYH16(pSp->y+y);
		g_libskb.SpAtokLogo.z = g_pSkbImport->thinkXW16(pSp->z);
		g_libskb.SpAtokLogo.w = g_pSkbImport->thinkXW16(pSp->w);
		g_libskb.SpAtokLogo.h = g_pSkbImport->thinkYH16(pSp->h);
	}
	#endif

	for(i=0; i<9; i++)
	{
		g_libskb.rcFrame[i] = pMap->rcFrames[i];		// DUM

		setFrame(&g_libskb.SpFrame[i][0], &pMap->rcFrames[i],x,y);
	}
}
#endif


//============================================================================
//	setFrame
//
//	�w���v�p�t���[���쐬
#if 1//SKB_EX HelpOffset
static void setFrame(sprite_t *sp0, iRect_t *pRc, int x, int y )
#else
static void setFrame(sprite_t *sp0, iRect_t *pRc)
#endif
{
	sprite_t *pSp;
	sprite_t *pSpPrev;
	int s, i;
	iRect_t rc;
	int spw = g_pSkbImport->thinkXW16(FRAME_SP_W);
	int sph = g_pSkbImport->thinkYH16(FRAME_SP_H);

	#if 1//SKB_EX HelpOffset
	rc.x = g_pSkbImport->thinkXW16(pRc->x+x);
	rc.y = g_pSkbImport->thinkYH16(pRc->y+y);
	#else
	rc.x = g_pSkbImport->thinkXW16(pRc->x);
	rc.y = g_pSkbImport->thinkYH16(pRc->y);
	#endif
	rc.w = g_pSkbImport->thinkXW16(pRc->w);
	rc.h = g_pSkbImport->thinkYH16(pRc->h);

	// ������
	for(i=0; i<3; i++)
	{
		for(s=0; s<3; s++)
		{
			pSp    = sp0+((3*i)+s);
			pSp->x = rc.x + (i*spw);
			pSp->y = rc.y + (s*sph);
			pSp->z = 0;
			pSp->w = spw;
			pSp->h = sph;
		}
	}

	pSp = sp0+1;
	pSp->h = rc.h;						// Pane[1]�̍���

	pSpPrev = (sp0+1);
	pSp = (sp0+2);
	pSp->y = rc.h + pSpPrev->y;			// Pane[2]��Y�l

	pSpPrev = (sp0+2);
	pSp = (sp0+3);
	pSp->w = rc.w;						// Pane[3]�̕�

	pSpPrev = (sp0+3);
	pSp = (sp0+4);
	pSp->h = rc.h;						// Pane[4]�̍���
	pSp->w = rc.w;						// Pane[4]�̕�

	pSpPrev = (sp0+4);
	pSp = (sp0+5);
	pSp->w = rc.w;						// Pane[5]�̕�
	pSp->y = rc.h + pSpPrev->y;			// Pane[5]��Y�l

	pSpPrev = (sp0+5);
	pSp = (sp0+6);
	pSp->x = pSpPrev->x + pSpPrev->w;	// Pane[6]��X�l

	pSpPrev = (sp0+6);
	pSp = (sp0+7);
	pSp->x = pSpPrev->x;				// Pane[7]��X�l
	pSp->h = rc.h;						// Pane[7]�̍���

	pSpPrev = (sp0+7);
	pSp = (sp0+8);
	pSp->y = pSpPrev->h + pSpPrev->y;	// Pane[8]��Y�l
	pSp->x = pSpPrev->x;				// Pane[8]��X�l
}


//============================================================================
//	searchModebarByPoint
//
//	���[�h�o�[�̂����蔻��
//
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	iPoint_t *pt : �ʒu���ւ̃|�C���^
//
//	�߂�l : int : �������Ă���:���[�h�̃C���f�b�N�X/�O��Ă���:SKBMODE_NOHIT
static int searchModebarByPoint(skey_t *pKey, iPoint_t *pt)
{
	int i;
	int cx = g_pSkbImport->thinkXW16(pt->x);
	int cy = g_pSkbImport->thinkYH16(pt->y);
	int keyNum = g_libskb.uiModebarItemNum;
	sprite_t *pSp = NULL;
	int inputMode = g_pSkbImport->skb.getNowInputMode();

	// �w�i�ł����蔻�������
	for(i=0; i<keyNum; i++)
	{
		pSp = &g_libskb.modebarBgSp[i];
		if((pSp->x <= cx) && (pSp->y <= cy) &&
		   ((pSp->x+pSp->w) > cx) && ((pSp->y+pSp->h) > cy))
		{
			return (i-inputMode);
		}
	}

	return SKBMODE_NOHIT;		// ���肦�Ȃ��l��Ԃ�
}


//============================================================================
//	drawHelp
//
//	�w���v�\��
//
//	sceVif1Packet *pk : VIF�p�P�b�g�o�b�t�@�ւ̃|�C���^
//	skey_t *pKey : �\�t�g�E�F�A�L�[�{�[�h�\���̂ւ̃|�C���^
//	textureInfo_t *pChars : �w���v�����e�N�X�`���ւ̃|�C���^
//	textureInfo_t *pItems : �R���g���[���e�N�X�`���ւ̃|�C���^
static void drawHelp(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pChars, textureInfo_t *pItems)
{
	int i, j;
	u_int col = 0x80808080;
	u_int lang = g_pSkbImport->usLang;
	helpMaps_t *pMap = &g_HelpMapsNTSC[lang];
	int itemNum = 0;
	iRect_t		pane = {0, 0, 650<<4, (700<<4)};
	u_int region = g_pSkbImport->iRegion;

	SKB_ASSERT(pChars);
	SKB_ASSERT(pItems);

	#if 1//SKB_EX HelpOffset
	pane.x=helpOffsetX;
	pane.y=helpOffsetY;
	#endif
	// ��ʂ�ŉB��
	g_pSkbImport->drawPane(pk, &pane, 0x80000000, 0);

	g_pSkbImport->loadTex(pk, pItems);		// �e�N�X�`�����[�h
	// ����ݒ�擾
	if(region == REGION_J)
	{
		int atok = 0;
		if(lang == SCE_ENGLISH_LANGUAGE)
		{
			lang = HELP_J_ENGLISH;
			atok = 1;
		}
		// ATOK��������
		g_pSkbImport->drawTex(pk, col, &g_libskb.SpAtokLogo, &g_uvAtokLogoSrc,		pItems);	// ATOK ���S�`��
		g_pSkbImport->drawTex(pk, col, &g_libskb.SpAtokVer,  &g_uvAtokVerSrc[atok],	pItems);	// ATOK �o�[�W�����`��
	}
	#if 1//SKB_EX LANG
	else{
		switch( lang ){
		default:
		case SCE_JAPANESE_LANGUAGE  :
		case SCE_ENGLISH_LANGUAGE   :	lang = HELP_ENGLISH;    break;
		case SCE_FRENCH_LANGUAGE    :	lang = HELP_FRENCH;     break;
		case SCE_SPANISH_LANGUAGE   :	lang = HELP_SPANISH;    break;
		case SCE_GERMAN_LANGUAGE    :	lang = HELP_GERMAN;     break;
		case SCE_ITALIAN_LANGUAGE   :	lang = HELP_ITALIAN;    break;
		case SCE_DUTCH_LANGUAGE     :	lang = HELP_DUTCH;      break;
		case SCE_PORTUGUESE_LANGUAGE:	lang = HELP_PORTUGUESE; break;
		}
	}
	#endif


	pMap = &g_HelpMapsNTSC[lang];

	/* "��{������"�`�� */
	g_pSkbImport->drawTex(pk, 0x80808080, &g_libskb.SpHelpTitle, pMap->uvTitle, pItems);

	/* PAD�A�C�e���`��    */
	itemNum = pMap->itemNum;
	for(i=0; i<itemNum; i++)
	{
		g_pSkbImport->drawTex(pk, col, &g_libskb.SpHelpItems[i], &pMap->uvItems[i], pItems);
	}

	/* �t���[���`��       */
	for(i=0; i<9; i++)
	{
		for(j=0; j<9; j++)
		{
			g_pSkbImport->drawTex(pk, DEFAULT_RGBA, &g_libskb.SpFrame[i][j], &pMap->uvFrames[j], pItems);
		}
	}

	/* �����`�� */
	itemNum = pMap->charNum;
	g_pSkbImport->loadTex(pk, pChars);
	for(i=0; i<itemNum; i++)
	{
		g_pSkbImport->drawTex(pk, col, &g_libskb.SpHelpChars[i], &pMap->uvChars[i], pChars);
	}
}


//============================================================================
//	SoftKB_se_play
//
//	���ʉ��R�[���o�b�N�֐��Ăяo��
//
#if 1//SKB_EX Sound
void SoftKB_se_play(int sound_No)
{
	if( g_pSkbImport->se ){
		g_pSkbImport->se(sound_No);
	}
}
#endif
