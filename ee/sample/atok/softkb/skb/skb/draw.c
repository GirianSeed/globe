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

// �v���~�e�B�u�̐���
#include <sys/types.h>
#include <eetypes.h>
#include <eeregs.h>
#include <eestruct.h>
#include <libpkt.h>
#include <libgraph.h>

#include "env.h"
#include "shareddef.h"


void SetVif1GsReg(sceVif1Packet *pk, u_int GsAdr, u_long GsData);

#if 1//SKB_EX ClipOffset 
static int clipOffsetX=0,clipOffsetY=0;
void SetClipOffset(int x,int y)
{
	clipOffsetX = x;
	clipOffsetY = y;
}
#endif
// �`��G���A���w��RECT�ŃN���b�s���O����
// RECT�͑S��ʂ̍����(0,0)�Ƃ������W�n
// NULL��n�����Ƃ��A�f�t�H���g�ɖ߂�
void SetClipArea(sceVif1Packet *pVif1Pk, const iRect_t *pRc)
{
	// �V�U�����O�̐ݒ�
	if(pRc)
	{
		#if 1//SKB_EX ClipOffset 
		SetVif1GsReg(pVif1Pk, SCE_GS_SCISSOR_1, 
							  SCE_GS_SET_SCISSOR_1( clipOffsetX+pRc->x, (clipOffsetX + pRc->x + pRc->w),
							                        clipOffsetY+pRc->y, (clipOffsetY + pRc->y + pRc->h) )
					 );
		#else
		SetVif1GsReg(pVif1Pk, SCE_GS_SCISSOR_1, 
								SCE_GS_SET_SCISSOR_1(pRc->x, (pRc->x + pRc->w), pRc->y, (pRc->y + pRc->h)));
		#endif
	}
	else
	{
		// NULL���n���ꂽ�Ƃ��A�f�t�H���g�̃V�U�����O�̈�ɖ߂�
		SetVif1GsReg(pVif1Pk, SCE_GS_SCISSOR_1, 
								SCE_GS_SET_SCISSOR_1(0, SCREEN_WIDTH-1, 0, SCREEN_HEIGHT-1));
	}
}

// �ėpGS���W�X�^�ݒ��VIF1�p�P�b�g�ɒǉ�
void SetVif1GsReg(sceVif1Packet *pk, u_int GsAdr, u_long GsData)
{
	union128 giftag;

	sceVif1PkCnt(pk, 0);						// DMAcnt�Ńp�P�b�g���J��
	sceVif1PkOpenDirectCode(pk, 0);				// DIRECT�R�[�h���J��

	// GIF�^�O���J��
	// NLOOP=0, EOP=1, PRE=0, PRIM=0, PACKED, NREGS=1
	giftag.aul[0] = SCE_GIF_SET_TAG(0, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
	giftag.aul[1] = 0x000000000000000EUL;
	sceVif1PkOpenGifTag(pk, giftag.qw);

	sceVif1PkAddGsAD(pk, GsAdr, GsData);		// A+D�f�[�^��ǉ�

	sceVif1PkCloseGifTag(pk);					// GIF�^�O�����
	sceVif1PkCloseDirectCode(pk);				// DIRECT�R�[�h�����
	sceVif1PkTerminate(pk);						// VIF1 DMAcnt�p�P�b�g�����
	return;
}




void SetRectFillEx(sceVif1Packet *pk, const IRECT *pRc, const RGBA32 *c, int z)
{
	int	x, y;
	int	w, h;
	union128 giftag;

	sceVif1PkCnt(pk, 0);						// DMAcnt�p�P�b�g�Œǉ�
	sceVif1PkOpenDirectCode(pk, 0);				// DIRECT�R�[�h���J��

	// ��`�^�C���pGIF�^�O���J��
	// NLOOP=0, EOP=1, PRE=0, PRIM=0, REGLIST, NREGS=4
	giftag.aul[0] = SCE_GIF_SET_TAG(0, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_REGLIST, 9);
	giftag.aul[1] = 0x0000000414141410UL;		// ���W�X�^�L�q�q
	sceVif1PkOpenGifTag(pk, giftag.qw);

	// PRIM���W�X�^�̒l��ݒ�(0x0)
	// sprite,�O�[��,�e�N�X�`���Ȃ�,�t�H�OOFF,�A���t�@ON
	// �A���`�G�C���A�XOFF,UV�w��,�R���e�L�X�g1,�t���O�����g�l����
	sceVif1PkAddGsData(pk, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, SCE_GS_TRUE, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE,
											SCE_GS_FALSE, 0, 0, SCE_GS_FALSE));

	
	// RGBAQ���W�X�^�̒l��ݒ�(0x1)
	sceVif1PkAddGsData(pk, (u_long)*c);
	// XY�l�����ꂼ��ݒ�(0x4)
	x = pRc->x;
	y = pRc->y;
	w = pRc->w;
	h = pRc->h;
	sceVif1PkAddGsData(pk, SCE_GS_SET_XYZ(OFFX+x,   OFFY+y,   z));		// (0x4)
	sceVif1PkAddGsData(pk, *(c+1));
	sceVif1PkAddGsData(pk, SCE_GS_SET_XYZ(OFFX+x+w, OFFY+y, z));		// (0x4)
	sceVif1PkAddGsData(pk, (u_long)*(c+2));
	sceVif1PkAddGsData(pk, SCE_GS_SET_XYZ(OFFX+x,   OFFY+y+h,   z));	// (0x4)
	sceVif1PkAddGsData(pk, (u_long)*(c+3));
	sceVif1PkAddGsData(pk, SCE_GS_SET_XYZ(OFFX+x+w, OFFY+y+h, z));		// (0x4)

	sceVif1PkCloseGifTag(pk);					// GIF�^�O�����
	sceVif1PkCloseDirectCode(pk);				// DIRECT�R�[�h�p�P�b�g�����
	sceVif1PkTerminate(pk);						// DMAcnt�p�P�b�g�����
	return;
}

// �^�C����VIF1�p�P�b�g�ɒǉ�
void SetRectFill(sceVif1Packet *pk, const IRECT *pRc, const RGBA32 c, int z)
{
	int	x, y;
	int	w, h;
	union128 giftag;

	sceVif1PkCnt(pk, 0);						// DMAcnt�p�P�b�g�Œǉ�
	sceVif1PkOpenDirectCode(pk, 0);				// DIRECT�R�[�h���J��

	// ��`�^�C���pGIF�^�O���J��
	// NLOOP=0, EOP=1, PRE=0, PRIM=0, REGLIST, NREGS=4
	giftag.aul[0] = SCE_GIF_SET_TAG(0, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_REGLIST, 4);
	giftag.aul[1] = 0x0000000000004410UL;		// ���W�X�^�L�q�q
	sceVif1PkOpenGifTag(pk, giftag.qw);

	// PRIM���W�X�^�̒l��ݒ�(0x0)
	// sprite,�t���b�g,�e�N�X�`���Ȃ�,�t�H�OOFF,�A���t�@ON
	// �A���`�G�C���A�XOFF,UV�w��,�R���e�L�X�g1,�t���O�����g�l����
	sceVif1PkAddGsData(pk, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE,
											SCE_GS_FALSE, 0, 0, SCE_GS_FALSE));

	// RGBAQ���W�X�^�̒l��ݒ�(0x1)
	sceVif1PkAddGsData(pk, (u_long)c);

	// XY�l�����ꂼ��ݒ�(0x4,0x4)
	x = pRc->x;
	y = pRc->y;
	w = pRc->w;
	h = pRc->h;
	sceVif1PkAddGsData(pk, SCE_GS_SET_XYZ(OFFX+x,   OFFY+y,   z));	// (0x4)
	sceVif1PkAddGsData(pk, SCE_GS_SET_XYZ(OFFX+x+w, OFFY+y+h, z));	// (0x4)

	sceVif1PkCloseGifTag(pk);					// GIF�^�O�����
	sceVif1PkCloseDirectCode(pk);				// DIRECT�R�[�h�p�P�b�g�����
	sceVif1PkTerminate(pk);						// DMAcnt�p�P�b�g�����
	return;
}


// ���C���Ђ�����
void SetLine(sceVif1Packet *pk, int x, int y, int cx, int cy, const RGBA32 c, int z)
{
	union128 giftag;

	sceVif1PkCnt(pk, 0);						// DMAcnt�p�P�b�g�Œǉ�
	sceVif1PkOpenDirectCode(pk, 0);				// DIRECT�R�[�h���J��

	// GIF�^�O���J��
	// NLOOP=0, EOP=1, PRE=0, PRIM=0, REGLIST, NREGS=8
	giftag.aul[0] = (u_long)SCE_GIF_SET_TAG(0, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_REGLIST, 5);
	giftag.aul[1] = (u_long)0x00000000000F4410UL;
	sceVif1PkOpenGifTag(pk, giftag.qw);

	// PRIM���W�X�^�̒l��ݒ�(0x0)
	// LineStrip,�t���b�g,�e�N�X�`���Ȃ�,�t�H�OOFF,�A���t�@OFF
	// �A���`�G�C���A�XOFF,UV�w��,�R���e�L�X�g1,�t���O�����g�l����
	sceVif1PkAddGsData(pk, SCE_GS_SET_PRIM(SCE_GS_PRIM_LINESTRIP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_FALSE,
											SCE_GS_FALSE, 0, 0, SCE_GS_FALSE));
	// RGBAQ���W�X�^�̒l��ݒ�(0x1)
	sceVif1PkAddGsData(pk, (u_long)c);

	// XY�l�����ꂼ��ݒ�(0x4,0x4,0x4,0x4,0x4)
	sceVif1PkAddGsData(pk, SCE_GS_SET_XYZ(OFFX+x,   OFFY+y,     z));	// (0x4)
	sceVif1PkAddGsData(pk, SCE_GS_SET_XYZ(OFFX+cx,  OFFY+cy,   z));	// (0x4)

	// NOP��ǉ�(0xF)
	sceVif1PkAddGsData(pk, 0x000000000000000UL);

	sceVif1PkCloseGifTag(pk);					// GIF�^�O�����
	sceVif1PkCloseDirectCode(pk);				// DIRECT�R�[�h�p�P�b�g�����
	sceVif1PkTerminate(pk);						// DMAcnt�p�P�b�g�����
}
