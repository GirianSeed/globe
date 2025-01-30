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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <eekernel.h>
#include <eetypes.h>
#include <sifdev.h>
#include <libpkt.h>
#include <libpad.h>

#include "libfep.h"
#include "fepbar.inc"

#ifdef SKBRES_LINK  //Makefile define
//----------------------------------------------------------------
#include <skbres.h>
#define FEPBAR_TEXNAME_FILE		fepbar_tm2
//----------------------------------------------------------------
#else
//----------------------------------------------------------------
#if CDBOOT
#define FEPBAR_TEXNAME_FILE		LOCALPATH"\\TEX\\SKB\\FEPBAR.TM2;1"
#else
#define FEPBAR_TEXNAME_FILE		LOCALPATH"/tex/skb/fepbar.tm2"
#endif
//----------------------------------------------------------------
#endif

gImport_t		*g_pFepbarImport;


static void ctrlPad(u_short up, u_short down, u_short press, u_short repeat);
static void padDown(u_short up, u_short down, u_short press, u_short repeat);
static void padUp(u_short up);
static void make_packets(sceVif1Packet *pk, textureInfo_t *pTex);
static void make_packets_F(sceVif1Packet *pk, textureInfo_t *pTex);

static struct
{
	u_int uiFepMode[FEPMODE_MAX];	// ���͕��@
	u_int uiFepmodeNum;				// ���͕��@�̐�
	u_int uiFepIndex;				// ���ݑI�𒆂̓��͕��@

	// pad
	u_short			up;
	u_short			down;
	u_short			press;
	u_short			repeat;


	textureInfo_t	tex;			// ���[�h�e�N�X�`��
	int				modebarPos16[2];// ���[�h�o�[�ʒu

} g_Fepbar;


//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//  Fepbar_Init													ref: fepbar.h
//
//  FEPBAR�̏�����
//
//  �߂�l : void
int Fepbar_Init( gImport_t *pImport )
{
	if(!pImport)	return -1;
	g_pFepbarImport = pImport;

	memset(&g_Fepbar, 0x0, sizeof(g_Fepbar));

	// �e�N�X�`���ǂݍ���
	if(g_pFepbarImport->readTex(&g_Fepbar.tex, FEPBAR_TEXNAME_FILE))
	{
		return 1;
	}
	return  0;
}


//============================================================================
//  Fepbar_Destroy												ref: fepbar.h
//
//  FEPBAR�̍폜
//
//  �߂�l : void
void Fepbar_Destroy(void)
{
	g_pFepbarImport->deleteTex(&g_Fepbar.tex);	// �e�N�X�`���j��
}


//============================================================================
//  Fepbar_Run													ref: fepbar.h
//
//  ���t���[������
//
//  sceVif1Packet *pk : �p�P�b�g�o�b�t�@
//
//  �߂�l : void
void Fepbar_Run(sceVif1Packet *pk)
{
	if(g_pFepbarImport->iRegion == REGION_J)
	{
		// ���{
		make_packets(pk, &g_Fepbar.tex);
	}
	else
	{
		// �C�O
		make_packets_F(pk, &g_Fepbar.tex);
	}

}


//============================================================================
//  Fepbar_Move													ref: fepbar.h
//
//  �ړ�
//
//  int x        : �ړI�ʒuX
//  int y        : �ړI�ʒuY
//
//  �߂�l : void
void Fepbar_Move(int x, int y)
{
	g_Fepbar.modebarPos16[0] = x << 4;
	g_Fepbar.modebarPos16[1] = VRESOLUTION_ADJUST(y << 4);
}


//============================================================================
//  Fepbar_SetPadData											ref: fepbar.h
//
//  �p�b�h���ݒ�
//
//  u_long paddata : �p�b�h���
//
//  �߂�l : void
void Fepbar_SetPadData(u_long paddata)
{
	u_short down, press, repeat, up;

	up     = (paddata>>48)	& 0xFFFFU;
	down   = (paddata>>32)	& 0xFFFFU;
	press  = (paddata>>16)	& 0xFFFFU;
	repeat = (paddata)		& 0xFFFFU;

	if(g_Fepbar.up == up && g_Fepbar.down == down && g_Fepbar.press == press && g_Fepbar.repeat == repeat)
	{
		// �ω��������������͏I��
		return;
	}

	// �p�b�h���ݒ�
	ctrlPad(up, down, press, repeat);

	g_Fepbar.up     = up;
	g_Fepbar.down   = down;
	g_Fepbar.press  = press;
	g_Fepbar.repeat = repeat;
}


//============================================================================
//  Fepbar_CreateModeType										ref: fepbar.h
//
//  ���[�h�^�C�v�̐ݒ�
//
//  const u_int *pTypes : �^�C�v�z��ւ̃|�C���^
//  int typeNum         : �^�C�v��
//
//  �߂�l : void
void Fepbar_CreateModeType(const u_int *pTypes, int typeNum)
{
	int i;
	g_Fepbar.uiFepIndex = 0;
	if(typeNum > FEPMODE_MAX)
	{
		g_pFepbarImport->dprintf("ERROR : Fepbar_CreateModeType - over %d\n", typeNum);
		return;
	}

	for(i=0; i<typeNum; i++)
	{
		if(pTypes[i] >= FEPMODE_MAX)
		{
			g_pFepbarImport->dprintf("ERROR : Fepbar_CreateModeType - unKnown type\n");
		}
		g_Fepbar.uiFepMode[i] = pTypes[i];		// ���̓��[�h�ݒ�
	}

	g_Fepbar.uiFepmodeNum = typeNum;			// ���̓��[�h���ݒ�
}


//============================================================================
//  Fepbar_SetModeType											ref: fepbar.h
//
//  ���[�h�^�C�v�̐ݒ�
//
//  fepAtokInputMode_m type : ���[�h�^�C�v
//
//  �߂�l : void
void Fepbar_SetModeType(int type)
{
	int ttype = FEPOFF;
	int i;
	// ���ȃ��b�N��Ԃ��ǂ����\�����Ă��܂���B

	switch(type)
	{
	#if 1//SKB_EX FixMode
	case EX_ATOK_FIXHIRAGANA:		// �Œ���͂Ђ炪��
		ttype = FEPON_HIRA_FIX;
		break;
	case EX_ATOK_FIXZENKANA:		// �Œ���͑S�p�J�^�J�i
		ttype = FEPON_ZEN_KATAKANA_FIX;
		break;
	case EX_ATOK_FIXZENRAW:			// �Œ���͑S�p���ϊ�
		ttype = FEPON_ZEN_ALPHABET_FIX;
		break;
	case EX_ATOK_FIXHANKANA:		// �Œ���͔��p�J�^�J�i
		ttype = FEPON_HAN_KATAKANA_FIX;
		break;
	case EX_ATOK_FIXHANRAW:			// �Œ���͔��p���ϊ�
		ttype = FEPON_HAN_ALPHABET_FIX;
		break;
	#else
	case EX_ATOK_FIXHIRAGANA:		// �Œ���͂Ђ炪��
		ttype = FEPON_HIRA;
		break;
	case EX_ATOK_FIXZENKANA:		// �Œ���͑S�p�J�^�J�i
		ttype = FEPON_ZEN_KATAKANA;
		break;
	case EX_ATOK_FIXZENRAW:			// �Œ���͑S�p���ϊ�
		ttype = FEPON_ZEN_ALPHABET;
		break;
	case EX_ATOK_FIXHANKANA:		// �Œ���͔��p�J�^�J�i
		ttype = FEPON_HAN_KATAKANA;
		break;
	case EX_ATOK_FIXHANRAW:			// �Œ���͔��p���ϊ�
		ttype = FEPON_HAN_ALPHABET;
		break;
	#endif
	
	case EX_ATOK_FIXOFF:			// �Œ���̓��[�hOFF
		ttype = FEPOFF;
		break;

	case EX_ATOK_DSPHIRAGANA:		// �Ђ炪�ȓ���
		ttype = FEPON_HIRA;
		break;
	case EX_ATOK_DSPZENKANA:		// �S�p�J�^�J�i����
		ttype = FEPON_ZEN_KATAKANA;
		break;
	case EX_ATOK_DSPZENRAW:			// �S�p���ϊ�����
		ttype = FEPON_ZEN_ALPHABET;
		break;
	case EX_ATOK_DSPHANKANA:		// ���p�J�^�J�i����
		ttype = FEPON_HAN_KATAKANA;
		break;
	case EX_ATOK_DSPHANRAW:			// ���p���ϊ�����
		ttype = FEPON_HAN_ALPHABET;
		break;

	case EX_ATOK_EDIT_KANJIOFF:
		ttype = FEPOFF;
		break;
	default:
		g_pFepbarImport->dprintf("ERROR : Fepbar_SetModeType - Unknown types\n");
		break;
	}

	for(i=0; i<g_Fepbar.uiFepmodeNum; i++)
	{
		if(ttype == g_Fepbar.uiFepMode[i])
		{
			// ���ɓo�^����Ă��郂�[�h�ɍ��v����C���f�b�N�X��ݒ肷��
			g_Fepbar.uiFepIndex = i;
			break;
		}
	}
}


//============================================================================
//  Fepbar_ClearModeType										ref: fepbar.h
//
//  ���[�h�^�C�v�̏�����
//
//  �߂�l : void
void Fepbar_ClearModeType(void)
{
	g_Fepbar.uiFepIndex = 0;
	g_Fepbar.uiFepmodeNum = 0;
}


#if 1//SKB_EX Fepbar_GetNowMode
//============================================================================
//  Fepbar_GetNowMode											ref: fepbar.h
//
//  FEP���̓��[�h�̎擾
//
//  �߂�l : Fep���̓��[�h
//
int Fepbar_GetNowMode()
{
	return g_Fepbar.uiFepMode[ g_Fepbar.uiFepIndex ];
}
#endif


#if 1//SKB_EX Fepbar_NextMode
//============================================================================
//  Fepbar_NextMode													ref: None
//
//  FEP���̓��[�h������FEP���̓��[�h�փV�t�g����B�i���Ԃ̓N���G�C�g���j
//
//  �߂�l : void
void Fepbar_NextMode(void)
{
	u_int nextMode = g_Fepbar.uiFepIndex+1;

	if(g_Fepbar.uiFepmodeNum == 1)
	{
		return;
	}
	if(nextMode >= g_Fepbar.uiFepmodeNum)
	{
		nextMode = 0;
	}
	printf("Fepbar_NextMode() Num[%d] nextMode[%d]\n",g_Fepbar.uiFepmodeNum,nextMode);
	
	Fep_SetInputMode(g_Fepbar.uiFepMode[nextMode]);	// ���̓��̓��[�h�ݒ�
	//g_Fepbar.uiFepIndex = nextMode;	libfep�̎w���ɏ]���̂Őݒ肵�Ȃ��B

	Fep_se_play(SKBSE_CHANGEKEYTOP); 
}
#endif


//============================================================================
// STATIC FUNCTIONS
//============================================================================
// �p�b�h�R���g���[��
static void ctrlPad(u_short up, u_short down, u_short press, u_short repeat)
{
	padDown(up, down, press, repeat);
	padUp  (up);
}

// �p�b�h����
static void padDown(u_short up, u_short down, u_short press, u_short repeat)
{
	if(!g_pFepbarImport->isConv())
	{
		// �ϊ����Ă��Ȃ����݈̂ȉ��̑���OK
		if(down & SCE_PADRright)
		{
			// �\�t�g�E�F�A�L�[�{�[�h�\��
			g_pFepbarImport->fep.openSoftKB(1);
		}
		if(down & SCE_PADselect)
		{
			#if 1//SKB_EX Fepbar_NextMode
			Fepbar_NextMode(); // AtokModeChange
			#else
			// ���[�h�ؑ�
			u_int nextMode = g_Fepbar.uiFepIndex+1;
			if(nextMode >= g_Fepbar.uiFepmodeNum)
			{
				nextMode = 0;
			}
	
			Fep_SetInputMode(g_Fepbar.uiFepMode[nextMode]);	// ���̓��̓��[�h�ݒ�
			g_Fepbar.uiFepIndex = nextMode;					// ���̓��[�h�̃C���f�b�N�X�w��
			#endif
		}
	}
	if(down & SCE_PADRdown)
	{
		// �������� �܂��́ACancel���M
		if(g_pFepbarImport->iRegion == REGION_J)
		{
			Fep_SetKanjiCancel();
		}
	}
}


// �p�b�h����
static void padUp(u_short up)
{
	if(up & SCE_PADL1){}
	if(up & SCE_PADL2){}
	if(up & SCE_PADR1){}
	if(up & SCE_PADR2){}
	if(up & SCE_PADRup){}
	if(up & SCE_PADRdown){}
	if(up & SCE_PADRleft){}
	if(up & SCE_PADRright){}
	if(up & SCE_PADselect){}
	if(up & SCE_PADstart){}
}


// �C�O��FEP���[�h�o�[�쐬
static void make_packets_F(sceVif1Packet *pk, textureInfo_t *pTex)
{
	int num=0, i=0;
	u_int col = DEFAULT_RGBA;
	uv_t *pUv = NULL;
	sprite_t sp;

	g_pFepbarImport->loadTex(pk, pTex);

	// �y���`��
	num = sizeof(g_spFepbarBase_F16)/sizeof(sprite_t);
	for(i=0; i<num; i++)
	{
		sp.x = g_spFepbarBase_F16[i].x + g_Fepbar.modebarPos16[0];
		sp.y = g_pFepbarImport->thinkYHn(g_spFepbarBase_F16[i].y) + g_Fepbar.modebarPos16[1];
		sp.z = 0;
		sp.w = g_spFepbarBase_F16[i].w;
		sp.h = g_pFepbarImport->thinkYHn(g_spFepbarBase_F16[i].h);
		g_pFepbarImport->drawTex(pk, col, &sp, (uv_t*)&g_uvFepbarBase_F[i], pTex);
	}

	// �\�t�g�E�F�A�L�[�{�[�h�\���{�^���`��
	pUv = (uv_t*)&g_uvFepbarSkbNormal;
	sp.x = g_spFepbarSkbVisible_F16.x + g_Fepbar.modebarPos16[0];
	sp.y = g_pFepbarImport->thinkYHn(g_spFepbarSkbVisible_F16.y) + g_Fepbar.modebarPos16[1];
	sp.z = 0;
	sp.w = g_spFepbarSkbVisible_F16.w;
	sp.h = g_pFepbarImport->thinkYHn(g_spFepbarSkbVisible_F16.h);
	g_pFepbarImport->drawTex(pk, col, &sp, pUv, pTex);

	// �t�H�[�J�X�`��
	g_pFepbarImport->drawFocus(pk, &sp, 0, 0x80808080);
}


// ���{��FEP���[�h�o�[�쐬
static void make_packets(sceVif1Packet *pk, textureInfo_t *pTex)
{
	int num=0, i=0;
	u_int col = DEFAULT_RGBA;
	uv_t *pUv = NULL;
	sprite_t sp;
	int index = g_Fepbar.uiFepIndex;
	int mode  = g_Fepbar.uiFepMode[index];

	g_pFepbarImport->loadTex(pk, pTex);

	// �y���`��
	num = sizeof(g_spFepbarBase_J16)/sizeof(sprite_t);
	for(i=0; i<num; i++)
	{
		sp.x = g_spFepbarBase_J16[i].x + g_Fepbar.modebarPos16[0];
		sp.y = g_pFepbarImport->thinkYHn(g_spFepbarBase_J16[i].y) + g_Fepbar.modebarPos16[1];
		sp.z = 0;
		sp.w = g_spFepbarBase_J16[i].w;
		sp.h = g_pFepbarImport->thinkYHn(g_spFepbarBase_J16[i].h);
		g_pFepbarImport->drawTex(pk, col, &sp, (uv_t*)&g_uvFepbarBase_J[i], pTex);
	}

	// ���݂̃��[�h��`��
	#if 1//SKB_EX FixMode
	if( mode >= FEPON_HIRA_FIX ){
		mode-=FEPON_HIRA_FIX;
		pUv = (uv_t*)g_uvFepbarPress;
	}
	else{
		pUv = (uv_t*)g_uvFepbarNormal;
	}
	#else
	pUv = (uv_t*)g_uvFepbarNormal;
	#endif
	sp.x = g_spInputMode16.x + g_Fepbar.modebarPos16[0];
	sp.y = g_pFepbarImport->thinkYHn(g_spInputMode16.y) + g_Fepbar.modebarPos16[1];
	sp.w = g_spInputMode16.w;
	sp.h = g_pFepbarImport->thinkYHn(g_spInputMode16.h);
	g_pFepbarImport->drawTex(pk, col, &sp, &pUv[mode], pTex);

	// �\�t�g�E�F�A�L�[�{�[�h�\���{�^���`��
	pUv = (uv_t*)&g_uvFepbarSkbNormal;
	sp.x = g_spFepbarSkbVisible_J16.x + g_Fepbar.modebarPos16[0];
	sp.y = g_pFepbarImport->thinkYHn(g_spFepbarSkbVisible_J16.y) + g_Fepbar.modebarPos16[1];
	sp.z = 0;
	sp.w = g_spFepbarSkbVisible_J16.w;
	sp.h = g_pFepbarImport->thinkYHn(g_spFepbarSkbVisible_J16.h);
	g_pFepbarImport->drawTex(pk, col, &sp, pUv, pTex);

	// �t�H�[�J�X�`��
	g_pFepbarImport->drawFocus(pk, &sp, 0, 0x80808080);
}
