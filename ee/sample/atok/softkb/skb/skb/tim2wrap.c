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
// tim2.c�̃��b�v�֐��Q

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <eekernel.h>
#include <eetypes.h>
#include <sifdev.h>
#include <libgraph.h>
#include <libpkt.h>


#include "tim2.h"
#include "tim2wrap.h"

static int   setTexInfo(textureInfo_t *pTex);
static u_int tim2LoadClut(sceVif1Packet *pk, TIM2_PICTUREHEADER *ph, u_int cbp);
//static int getLog2(u_long32 n);

static struct
{
	u_int uiCreateNum; // �쐬������

	u_int sw, sh;      // �X�N���[�����A����
	u_int tbp;         // �e�N�X�`���x�[�X�|�C���^
	u_int cbp;         // �N���b�g�x�[�X�|�C���^
	u_int offx, offy;  // 

	void* (*malign)(size_t align, size_t size); // �������m�ۃR�[���o�b�N
	void  (*mfree)(void *p);                    // �������J���R�[���o�b�N
} g_Tim2;


//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
void Tim2_Init( u_int screenWidth, u_int screenHeight,
                u_int offX, u_int offY,
                u_int texBasePtr, u_int clutBasePtr,
                void* (*malign)(size_t align, size_t size),
                void  (*mfree)(void *p))
{
	memset(&g_Tim2, 0x0, sizeof(g_Tim2));

	g_Tim2.sw = screenWidth;  // �X�N���[����
	g_Tim2.sh = screenHeight; // ����
	g_Tim2.tbp = texBasePtr;  // �e�N�X�`���x�[�X�|�C���^
	g_Tim2.cbp = clutBasePtr; // �N���b�g�x�[�X�|�C���^
	g_Tim2.offx = offX;       //
	g_Tim2.offy = offY;       //
	g_Tim2.malign = malign;
	g_Tim2.mfree  = mfree;
}






//============================================================================
//  Tim2_ReadTim2                                            ref: tim2wrap.h
//
//  Tim2�t�@�C���Ǎ���
//
//  textureInfo_t *pTex  : �e�N�X�`���\����
//  const char *filename : TIM2�t�@�C����
//
//  �߂�l �F int 0:����/1:���s
int Tim2_ReadTim2(textureInfo_t *pTex, const char *filename)
{
	#if 0//SKB_EX Binlink
	// �^�[�Q�b�g����TIM2�t�@�C���Ǎ���
	int fd, ret, size;
	SKB_ASSERT(pTex);

	// �t�@�C���Ǎ���
	printf("MSG : reading - %s\n", filename);
	fd = sceOpen(filename, SCE_RDONLY);
	if(fd < 0)
	{
		printf("ERROR : readTIM2 - open(%s)\n", filename);
		return 1;
	}
	size = sceLseek(fd, 0, SCE_SEEK_END);
	sceLseek(fd, 0, SCE_SEEK_SET);
	if(size <= 0)
	{
		printf("ERROR : readTIM2 - size(%d)\n", size);
		return 1;
	}
	pTex->pTim2Buffer = (char*)g_Tim2.malign(64, size);
	if(pTex->pTim2Buffer == NULL)
	{
		printf("ERROR : readTIM2 - memory allocate failed(%d)\n", size);
		return 1;
	}
	ret = sceRead(fd, (char*)pTex->pTim2Buffer, size);
	if(ret < 0)
	{
		printf("ERROR : readTIM2 - failed to read(%d)\n", ret);
		g_Tim2.mfree(pTex->pTim2Buffer);
		pTex->pTim2Buffer = NULL;
		return 1;
	}
	sceClose(fd);
	#else
	int ret;
	pTex->pTim2Buffer = (char*)filename; 
	                   //���O��filename �ł����Atim2 ��address�Ƃ��Ďg�p�B
	#endif

	// �f�[�^�ݒ�
	ret = setTexInfo(pTex);
	if(!ret)
	{
		// �f�[�^�ݒ萬��
		g_Tim2.uiCreateNum++;
		return 0;
	}

	// �f�[�^�ݒ莸�s
	return 1;
}


//============================================================================
//  Tim2_DeleteTim2                                          ref: tim2wrap.h
//
//  TIM2�t�@�C���f�[�^���e�N�X�`���o�b�t�@�ɓ]������ׂ̃p�P�b�g�쐬
//
//  textureInfo_t *pTex : �e�N�X�`���\����
//
//  �߂�l �F void
void Tim2_DeleteTim2(textureInfo_t *pTex)
{
	assert(pTex);

	if(pTex->pTim2Buffer)
	{
		#if 0//SKB_EX BinLink
		// Tim2�o�b�t�@�폜
		g_Tim2.mfree(pTex->pTim2Buffer);
		#endif
		pTex->pTim2Buffer = NULL;
		--g_Tim2.uiCreateNum;
	}
}


//============================================================================
//  Tim2_SendTim2                                            ref: tim2wrap.h
//
//  TIM2�t�@�C���f�[�^���e�N�X�`���o�b�t�@�ɓ]������ׂ̃p�P�b�g�쐬
//
//  sceVif1Packet *pk   : �p�P�b�g�o�b�t�@�̃|�C���^
//  textureInfo_t *pTex : �e�N�X�`���\����
//
//  �߂�l �F int 0:����/1:���s
void Tim2_SendTim2(sceVif1Packet *pk, textureInfo_t *pTex)
{
//	assert(pTex);
	{
		TIM2_PICTUREHEADER *ph;
		ph = Tim2GetPictureHeader(pTex->pTim2Buffer, 0);
		if( !ph )
		{
			printf("ERROR : setTexInfo - broken tim2.\n");
			return;
		}

		// �s�N�`���f�[�^��VRAM�ɓǂݍ���
		{
			static const int psmtbl[] =
			{
				SCE_GS_PSMCT16,
				SCE_GS_PSMCT24,
				SCE_GS_PSMCT32,
				SCE_GS_PSMT4,
				SCE_GS_PSMT8
			};
			int psm;
			int w, h;
			u_long128 *pImage;
			psm = psmtbl[(ph->ImageType&0x3f) - 1];		// �s�N�Z���t�H�[�}�b�g�𓾂�

			// �C���[�W�f�[�^�̐擪�A�h���X���v�Z
			pImage = (u_long128 *)((char *)ph + ph->HeaderSize);
			((sceGsTex0 *)&ph->GsTex0)->PSM  = psm;

			w = ph->ImageWidth;   // �C���[�WX�T�C�Y
			h = ph->ImageHeight;  // �C���[�WY�T�C�Y

			// �s�N�`���f�[�^�p�P�b�g�p��
			sceVif1PkRefLoadImage( pk, (u_short)g_Tim2.tbp, (u_char)psm,
			                      (u_short)((ph->ImageWidth+63)/64),
			                      pImage,
			                      (u_int)(ph->ImageSize/16),
			                      (u_int)0, (u_int)0,
			                      (u_int)w, (u_int)h);

			if( ph->ClutColors )
			{
				// �N���b�g�p�P�b�g�p��
				tim2LoadClut(pk, ph, g_Tim2.cbp);
			}
		}

		{
			const u_long64 giftagAD[2] =
			{
				SCE_GIF_SET_TAG(0, 1, 0, 0, SCE_GIF_PACKED, 1), // A+D�pGIF�^�O
				0x000000000000000EL
			};
			// �e�N�X�`���t���b�V��
			sceVif1PkCnt(pk, 0);
			sceVif1PkOpenDirectHLCode( pk, 0 );
			sceVif1PkOpenGifTag(pk, *(u_long128*)&giftagAD); // GIF�^�O��ݒ�
			sceVif1PkAddGsData(pk, 0x00000000);       // D
			sceVif1PkAddGsData(pk, SCE_GS_TEXFLUSH);  // A(TEXFLUSH)
			sceVif1PkCloseGifTag(pk);                 // GIF�^�O�����
			sceVif1PkCloseDirectHLCode( pk );
		}
	}
}


//============================================================================
//  Tim2_DrawSprite                                          ref: tim2wrap.h
//
//  �X�v���C�g�`��
//
//  sceVif1Packet *pk : �p�P�b�g�o�b�t�@�ւ̃|�C���^
//  u_int col         : �X�v���C�g�F
//  sprite_t *pSp     : �X�v���C�g�\���̂̃|�C���^
//  uv_t *pUv         : UV�\���̂̃|�C���^
//  textureInfo_t *pTex : �e�N�X�`���\���̂̃|�C���^
//
//  �߂�l �F void
void Tim2_DrawSprite(sceVif1Packet *pk, u_int col, sprite_t *pSp, uv_t *pUv, textureInfo_t *pTex)
{
	int	x=0, y=0, w=0, h=0, z=0;
	int u=0, v=0, tw=0, th=0;
	u_int offx = g_Tim2.offx;
	u_int offy = g_Tim2.offy;

	// �X�v���C�g�pGIF�^�O
	const u_long64 giftagSPR[2] =
	{
		SCE_GIF_SET_TAG(0, 1, 0, 0, SCE_GIF_REGLIST, 8),
		0x00000000F4343810L   // PRIM,RGBAQ,CLAMP_1,UV,XYZF2,UV,XYZF2
	};
	const u_long64 giftagAD[2] =
	{
		SCE_GIF_SET_TAG(0, 1, 0, 0, SCE_GIF_PACKED, 1), // A+D�pGIF�^�O
		0x000000000000000EL
	};

	// GS�̃��W�X�^�ݒ�
	sceVif1PkCnt(pk, 0);
	sceVif1PkOpenDirectHLCode( pk, 0 );
	sceVif1PkOpenGifTag(pk, *((u_long128*)&giftagAD));  // GIF�^�O��ݒ�

	// TEX1���W�X�^�̐ݒ�
	sceVif1PkAddGsData(pk,
			SCE_GS_SET_TEX1(1, pTex->bMipmapLv, 0, pTex->nFilter, 0, 0, pTex->nLod));	// D(TEX1_1)
	sceVif1PkAddGsData(pk, SCE_GS_TEX1_1);		// A(TEX1_1)

	// TEX0���W�X�^�̒l��ݒ�
	if(pTex->GsTex0.PSM==SCE_GS_PSMT4)
	{
		// 16�F�e�N�X�`���̂Ƃ�
		sceGsTex0 Load;     // TEX0(CLUT�ǂݍ��ݗp)
		sceGsTex0 Change;   // TEX0(CSA�ύX�p)

		// GS��TEX0���W�X�^�̐ݒ���R�s�[���āACLUT�ǂݍ��ݗp��CSA�ύX�p�ɒl���C��
		Load        = pTex->GsTex0;      // �I���W�i����TEX0_1���R�s�[
		Load.PSM    = SCE_GS_PSMT8;      // 8bit�e�N�X�`�����ƃt�F�C�N
		Load.CBP   += pTex->nClutNo/16;  // CBP�̃I�t�Z�b�g�����炷
		Load.TCC    = 1;                 //
		Load.TFX    = 0;                 //
		Load.CSA    = 0;                 // CSA�͐擪���g��
		Load.CLD    = 1;                 // CLUT�o�b�t�@�̃��[�h�������Ȃ�

		Change      = pTex->GsTex0;          // �I���W�i����TEX0_1���R�s�[
		Change.CBP += pTex->nClutNo/16;      // CBP�̃I�t�Z�b�g�����炷
		Change.TCC    = 1;                   //
		Change.TFX    = 0;                   //
		Change.CSA  = (pTex->nClutNo & 0x0F);// CSA�͐擪���g��
		Change.CLD  = 0;                     // CLUT�o�b�t�@�̃��[�h�������Ȃ�Ȃ�

		// ��������256�FCLUT�Ƃ��܂���CLUT�o�b�t�@�Ƀ��[�h
		sceVif1PkAddGsData(pk, *(u_long64 *)&Load);  // D(TEX0_1)
		sceVif1PkAddGsData(pk, SCE_GS_TEX0_1);       // A(TEX0_1)

		// ����������TEX0���W�X�^�̒l��ݒ肵��CSA�����ύX
		sceVif1PkAddGsData(pk, *(u_long64 *)&Change);// D(TEX0_1)
		sceVif1PkAddGsData(pk, SCE_GS_TEX0_1);       // A(TEX0_1)
	}
	else
	{
		// 16�F�ȊO�̂Ƃ�
		sceGsTex0 t0;   // TEX0

		t0 = pTex->GsTex0;         // �I���W�i����TEX0_1��
		t0.CBP += pTex->nClutNo*4; // CBP�̃I�t�Z�b�g�����炷
		t0.TCC = 1;                //
		t0.TFX = 0;                //

		sceVif1PkAddGsData(pk, *(u_long64 *)&t0); // D(TEX0_1)
		sceVif1PkAddGsData(pk, SCE_GS_TEX0_1);    // A(TEX0_1)
	}

	// MIPTBP1���W�X�^�̐ݒ�
	sceVif1PkAddGsData(pk, pTex->GsMiptbp1);  // D(MIPTBP1_1)
	sceVif1PkAddGsData(pk, SCE_GS_MIPTBP1_1); // A(MIPTBP1_1)

	// MIPTBP2���W�X�^�̐ݒ�
	sceVif1PkAddGsData(pk, pTex->GsMiptbp2);  // D(MIPTBP2_1)
	sceVif1PkAddGsData(pk, SCE_GS_MIPTBP2_1); // A(MIPTBP2_1)

	// ALPHA_1���W�X�^�̐ݒ�
	sceVif1PkAddGsAD(pk, SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0x00));

	sceVif1PkCloseGifTag(pk); // GIF�^�O�����
	sceVif1PkCloseDirectHLCode( pk );

	// �X�v���C�g�`��
	sceVif1PkOpenDirectHLCode( pk, 0 );
	sceVif1PkOpenGifTag(pk, *((u_long128*)&giftagSPR)); // GIF�^�O��ݒ�

	// PRIM���W�X�^�̒l��ݒ�(0x0)
	// Sprite,�t���b�g,�e�N�X�`��,�t�H�OOFF,�A���t�@ON
	// �A���`�G�C���A�XON,UV�w��,�R���e�L�X�g1,�t���O�����g�l����
	// ���`���l���g�p���Ȃ�
	sceVif1PkAddGsData(pk, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 1, 0, 1, 1, 1, 0, 0));

	// RGBAQ���W�X�^�̒l��ݒ�(0x1)
	sceVif1PkAddGsData(pk, col);

	// CLAMP���W�X�^�̒l��ݒ�(0x8)
	sceVif1PkAddGsData(pk, SCE_GS_SET_CLAMP(0, 0, 0, 0, 0, 0));

	// UV,XY�l�����ꂼ��ݒ�(����,�E��)
	x = pSp->x;
	y = pSp->y;
	z = pSp->z;
	w = pSp->w;
	h = pSp->h;
	u = pUv->u;
	v = pUv->v;
	tw = pUv->tw;
	th = pUv->th;
	sceVif1PkAddGsData(pk, SCE_GS_SET_UV (u, v));                  // (0x3)
	sceVif1PkAddGsData(pk, SCE_GS_SET_XYZ(offx+x,   offy+y,   z)); // (0x4)
	sceVif1PkAddGsData(pk, SCE_GS_SET_UV (u+tw, v+th));            // (0x3)
	sceVif1PkAddGsData(pk, SCE_GS_SET_XYZ(offx+x+w, offy+y+h, z)); // (0x4)
	sceVif1PkAddGsData(pk, 0);       // 0xF(�A���C�������g����)
	sceVif1PkCloseGifTag(pk);        // GIF�^�O�����
	sceVif1PkCloseDirectHLCode( pk );
}


//============================================================================
// STATIC FUNCTIONS
//============================================================================
// �e�N�X�`���\���̂ɒl�ݒ�
static int setTexInfo(textureInfo_t *pTex)
{
	assert(pTex);

	if(!Tim2CheckFileHeaer(pTex->pTim2Buffer))
	{
		// TIM2�t�@�C�������Ă���
		printf("ERROR : setTexInfo - broken tim2.\n");
		return 1;
	}
	else
	{
		// �s�N�`���w�b�_�𓾂�
		TIM2_PICTUREHEADER *ph;
		ph = Tim2GetPictureHeader(pTex->pTim2Buffer, 0);
		if(!ph)
		{
			printf("ERROR : setTexInfo - failed to get TIM2_PICTUREHEADER.\n");
			return 1;
		}
		else
		{
			// �s�N�`���w�b�_��VRAM�ɓǍ���
			Tim2LoadPicture(ph, g_Tim2.tbp, g_Tim2.cbp);
			// �s�N�`���w�b�_���炢�����̃f�[�^�𓾂�
			pTex->GsTex0 = *(sceGsTex0 *)&ph->GsTex0;  // �s�N�`���w�b�_����TEX0���W�X�^�ւ̐ݒ�l�𓾂�
			pTex->nTexSizeW = ph->ImageWidth;          // �s�N�`���T�C�YX
			pTex->nTexSizeH = ph->ImageHeight;         // �s�N�`���T�C�YY
			// �~�b�v�}�b�v���x���𓾂�
			pTex->bMipmapLv = ph->MipMapTextures - 1;  // �~�b�v�}�b�v���x��
			if(pTex->bMipmapLv) {
				// MIPMAP���g�p����Ƃ�
				TIM2_MIPMAPHEADER *pm;
				pm = (TIM2_MIPMAPHEADER *)(ph + 1);  // �s�N�`���w�b�_�̒����MIPMAP�w�b�_
				pTex->GsMiptbp1 = pm->GsMiptbp1;
				pTex->GsMiptbp2 = pm->GsMiptbp2;
			} else {
				// MIPMAP���g�p���Ȃ��Ƃ�
				pTex->GsMiptbp1 = 0;
				pTex->GsMiptbp2 = 0;
			}
		}
	}

	return 0;
}


// �N���b�g���[�h
static u_int tim2LoadClut(sceVif1Packet *pk, TIM2_PICTUREHEADER *ph, u_int cbp)
{
	int i;
	int	cpsm;
	int colByte=0;
	u_long128 *pClut;

	// CLUT�s�N�Z���t�H�[�}�b�g�𓾂�
	if(ph->ClutType==TIM2_NONE)
	{
		// CLUT�f�[�^�����݂��Ȃ��Ƃ�
		return(1);
	}
	else if((ph->ClutType & 0x3F)==TIM2_RGB16)
	{
		cpsm = SCE_GS_PSMCT16;
		colByte = 2;
	}
	else if((ph->ClutType & 0x3F)==TIM2_RGB24)
	{
		cpsm = SCE_GS_PSMCT24;
		colByte = 3;
	}
	else
	{
		cpsm = SCE_GS_PSMCT32;
		colByte = 4;
	}
	((sceGsTex0 *)&ph->GsTex0)->CPSM = cpsm; // CLUT���s�N�Z���t�H�[�}�b�g�ݒ�
	((sceGsTex0 *)&ph->GsTex0)->CSM  = 0;    // CLUT�i�[���[�h(���CSM1)
	((sceGsTex0 *)&ph->GsTex0)->CSA  = 0;    // CLUT�G���g���I�t�Z�b�g(���0)
	((sceGsTex0 *)&ph->GsTex0)->CLD  = 1;    // CLUT�o�b�t�@�̃��[�h����(��Ƀ��[�h)

	if(cbp==-1)
	{
		// cbp�̎w�肪�Ȃ��Ƃ��A�s�N�`���w�b�_��GsTex0�����o����l���擾
		cbp = ((sceGsTex0 *)&ph->GsTex0)->CBP;
	}
	else
	{
		// cbp���w�肳�ꂽ�Ƃ��A�s�N�`���w�b�_��GsTex0�����o�̒l���I�[�o�[���C�h
		((sceGsTex0 *)&ph->GsTex0)->CBP = cbp;
	}

	// CLUT�f�[�^�̐擪�A�h���X���v�Z
	pClut = (u_long128 *)((char *)ph + ph->HeaderSize + ph->ImageSize);

	// CLUT�f�[�^��GS���[�J���������ɑ��M
	// CLUT�`���ƃe�N�X�`���`���ɂ����CLUT�f�[�^�̃t�H�[�}�b�g�Ȃǂ����܂�
	switch((ph->ClutType<<8) | ph->ImageType)
	{
	case (((TIM2_RGB16 | 0x40)<<8) | TIM2_IDTEX4): // 16�F,CSM1,16�r�b�g,���ёւ�����
	case (((TIM2_RGB24 | 0x40)<<8) | TIM2_IDTEX4): // 16�F,CSM1,24�r�b�g,���ёւ�����
	case (((TIM2_RGB32 | 0x40)<<8) | TIM2_IDTEX4): // 16�F,CSM1,32�r�b�g,���ёւ�����
	case (( TIM2_RGB16        <<8) | TIM2_IDTEX8): // 256�F,CSM1,16�r�b�g,���ёւ�����
	case (( TIM2_RGB24        <<8) | TIM2_IDTEX8): // 256�F,CSM1,24�r�b�g,���ёւ�����
	case (( TIM2_RGB32        <<8) | TIM2_IDTEX8): // 256�F,CSM1,32�r�b�g,���ёւ�����
		// 256�FCLUT���ACLUT�i�[���[�h��CSM1�̂Ƃ�
		// 16�FCLUT���ACLUT�i�[���[�h��CSM1�œ���ւ��ς݃t���O��ON�̂Ƃ�
		// ���łɃs�N�Z��������ւ����Ĕz�u����Ă���̂ł��̂܂ܓ]���\���[
		sceVif1PkRefLoadImage( pk, cbp, cpsm,
							  1,
							  pClut,
							  (u_int)((colByte*ph->ClutColors)/16),
							  (u_int)0, (u_int)0,
							  (u_int)16, (u_int)(ph->ClutColors/16));
		break;
	case (( TIM2_RGB16        <<8) | TIM2_IDTEX4): // 16�F,CSM1,16�r�b�g,���j�A�z�u
	case (( TIM2_RGB24        <<8) | TIM2_IDTEX4): // 16�F,CSM1,24�r�b�g,���j�A�z�u
	case (( TIM2_RGB32        <<8) | TIM2_IDTEX4): // 16�F,CSM1,32�r�b�g,���j�A�z�u
	case (((TIM2_RGB16 | 0x80)<<8) | TIM2_IDTEX4): // 16�F,CSM2,16�r�b�g,���j�A�z�u
	case (((TIM2_RGB24 | 0x80)<<8) | TIM2_IDTEX4): // 16�F,CSM2,24�r�b�g,���j�A�z�u
	case (((TIM2_RGB32 | 0x80)<<8) | TIM2_IDTEX4): // 16�F,CSM2,32�r�b�g,���j�A�z�u
	case (((TIM2_RGB16 | 0x80)<<8) | TIM2_IDTEX8): // 256�F,CSM2,16�r�b�g,���j�A�z�u
	case (((TIM2_RGB24 | 0x80)<<8) | TIM2_IDTEX8): // 256�F,CSM2,24�r�b�g,���j�A�z�u
	case (((TIM2_RGB32 | 0x80)<<8) | TIM2_IDTEX8): // 256�F,CSM2,32�r�b�g,���j�A�z�u
		// 16�FCLUT���ACLUT�i�[���[�h��CSM1�œ���ւ��ς݃t���O��OFF�̂Ƃ�
		// 16�FCLUT���ACLUT�i�[���[�h��CSM2�̂Ƃ�
		// 256�FCLUT���ACLUT�i�[���[�h��CSM2�̂Ƃ�
		// CSM2�̓p�t�H�[�}���X�������̂ŁACSM1�Ƃ��ē���ւ��Ȃ���]��
		{
			for(i=0; i<(ph->ClutColors/16); i++)
			{
				sceVif1PkRefLoadImage( pk, cbp, cpsm,
									  1,
									  pClut,
									  (u_int)((colByte*ph->ClutColors)/16),
									  (u_int)(i & 1)*8, (u_int)(i>>1)*2,
									  (u_int)8, (u_int)2);
				// ����16�F�ցA�A�h���X�X�V
				if((ph->ClutType & 0x3F)==TIM2_RGB16)
				{
					pClut = (u_long128 *)((char *)pClut + 2*16); // 16bit�F�̂Ƃ�
				}
				else if((ph->ClutType & 0x3F)==TIM2_RGB24)
				{
					pClut = (u_long128 *)((char *)pClut + 3*16); // 24bit�F�̂Ƃ�
				}
				else
				{
					pClut = (u_long128 *)((char *)pClut + 4*16); // 32bit�F�̂Ƃ�
				}
			}
			break;
		}
	default:
		printf("ERROR : tim2LoadClut - Illegal clut and texture combination. ($%02X,$%02X)\n", ph->ClutType, ph->ImageType);
		return 1;
	}

	return 0;
}

// �r�b�g���𓾂�
//static int getLog2(u_long32 n)
//{
//	u_long32 i;
//	for(i=31; i>0; i--) {
//		if(n & (1<<i)) {
//			break;
//		}
//	}
//	if(n>(1<<i)) {
//		i++;
//	}
//	return(i);
//}
