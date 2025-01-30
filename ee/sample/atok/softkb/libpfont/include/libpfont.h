/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
*/
/*
 *     Copyright (C) 2003 Sony Computer Entertainment Inc.
 *                   All Rights Reserved.
 *
 */

#ifndef __libpfont__
#define __libpfont__

#include <eetypes.h>
#include <libvu0.h>

#include <libvifpk.h>

#if defined(__cplusplus)
extern "C"{
#endif

/* �t�H���g�f�[�^��`�\���̌Q */

typedef struct _scePFontTag{
	u_int id;
	u_int version;
	int size;
	int reserved;
}scePFontTag;

typedef struct _scePFontInfo{
	scePFontTag tag;

	char name[32];				/* name(UTF8) */
	char comment[32];			/* comment(UTF8) */
	short max_ascent;
	short max_descent;
	short max_width;
	short reserved;

	int block_num;
	int block_offset[0];
}scePFontInfo;

typedef struct _scePFontBlockAttribute{
	u_int color_mode:3;
	u_int propotional:1;
	u_int reserved:28;
}scePFontBlockAttribute;

typedef struct _scePFontBlock{
	scePFontTag tag;

	scePFontBlockAttribute attr;
	float scale_x;
	float scale_y;
	short image_width;
	short image_height;
	short max_ascent;
	short max_descent;
	short max_width;
	short reserved;

	struct{
		int character_num;
		int image_offset;

		int codeindex_num;
		int codeindex_offset;

		int codemap_num;
		int codemap_offset;

		int proportional_num;
		int proportional_offset;

		int kerning_num;
		int kerning_offset;

		int clut_num;
		int clut_offset;
	}data;

}scePFontBlock;

typedef struct _scePFontCodeIndex{
	int start_code;
	int end_code;
	int map_index;
	int chr_index;
}scePFontCodeIndex;

typedef struct _scePFontProportional{
	short base_x;
	short base_y;
	short l_bearing;
	short r_bearing;
	short ascent;
	short descent;
	short width;
	short kerning;
}scePFontPropotional;

typedef struct _scePFontGlyph{
	scePFontBlock const* block;
	u_long128 const* image;
	scePFontPropotional const* prop;
	void const* kerning;
}scePFontGlyph;

/* �t�H���g�p�̋�`�̈�\�� */
typedef struct _scePFontFRect{
	float top;
	float bottom;
	float left;
	float right;
}scePFontFRect;


/* �R�[���o�b�N�ɓn�����֐��e�[�u�� */
typedef struct _scePFontControl{

	/* �����擾
	   return : �����R�[�h�i0=�I��,-1=�G���[�j*/
	int (* const Getc)(int fd);

	/* �����̎擾�ʒu���P�������
	   return : 0=����, -1=�擪�ʒu������ */
	int (* const Ungetc)(int fd);

	/* �����o��
	   chr : �����R�[�h
	   return : 0=����,-1=�������[�s��,-2=�O���t������ */
	int (* const Putc)(int fd, int chr);

	/* �����o��(�`��͂������W�v�Z�̂�)
	   chr : �����R�[�h
	   return : 0=����,-1=�������[�s��,-2=�O���t������ */
	int (* const PutcX)(int fd, int chr);

	/* �������o�͂����ꍇ�̃T�C�Y���擾
	   chr : �����R�[�h
	   pRect_i : �����̋�`���󂯂Ƃ�ϐ��ւ̃|�C���^
	   pRect_o : �O���̋�`���󂯂Ƃ�ϐ��ւ̃|�C���^
	   return : 0=����,-1=�������[�s��,-2=�O���t������ */
	int (* const Calc)(int fd, int chr, scePFontFRect* pRect_i, scePFontFRect* pRect_o);

}scePFontControl;

/* �t�H���g�������ɕK�v�ȃ��C���������[�̃T�C�Y���v�Z����B
   (�Œ�T�C�Y�̃��[�N�{������*�������[�N)
   num : �L���b�V�����镶����
   return : �����������L���b�V������̂ɕK�v�ȃ������[�T�C�Y�i�o�C�g�P�ʁj*/
size_t scePFontCalcCacheSize(int num);

/* �t�H���g������
   scePFontCalcCacheSize�ɂ��v�Z�����\���ȃ������[���m�ۂ��ēn���B
   scePFontRelease()������܂œ����ŕێ���������B
   num : �L���b�V�����镶�����i�Œ�P�����ȏ�K�v�j
   work : �L���b�V���p���[�N�������[�i128bit�A���C�������g)
   return : �t�H���g���ʎq(0>�G���[) */
int scePFontInit(int num, u_long128* pWork);

/* �t�H���g���\�[�X����B�֘A�Â��j���B*/
void scePFontRelease(int fd);

/* GS�p�P�b�g�̃R���e�L�X�g���w��
   ctxt : GS�R���e�L�X�g(0<default> or 1)
   return: �ȑO�̒l */
int scePFontSetGsCtxt(int fd, int ctxt);

/* �e�N�X�`���]���p��GS�������[
   �P�����̓]���ɖ����Ȃ��ꍇ�̓���͕ۏ؂��Ȃ��B
   ���̃������[�̏���j�󂵂��ꍇscePFontFlush()�Ŗ��������邱�ƁB
   fd : �t�H���g���ʎq
   tbp : GS�̃��[�h�A�h���X
   size : GS�������[�̃��[�h��(2048���[�h�̔{��)
   cbp : 32bitCLUT�p���[�h�A�h���X(256�G���g�����K�v) */
void scePFontSetTexMem(int fd, u_int tbp, u_int size, u_int cbp);

/* �o�͓r���̕������j�����A�L���b�V�����N���A�B
   �ό`�}�g���N�X��P�ʍs��ɁALOCATE��(0,0,0,0)�ɐݒ�B
   color���f�t�H���g(1.0,1.0,1.0,1.0)�ɐݒ�B
   fd : �t�H���g���ʎq */
void scePFontFlush(int fd);

/* �t�H���g�f�[�^�Ɗ֘A�Â�
   fd : �t�H���g���ʎq
   pData : �t�H���g�f�[�^�iqword���E�ɂ��邱�Ɓj
   return : 0=����,-1=���s */
int scePFontAttachData(int fd, u_long128 const* pData);

/* �֘A�Â����Ă���t�H���g���ւ̎Q�Ƃ�Ԃ�
   fd : �t�H���g���ʎq
   return : �t�H���g���ւ̎Q�Ɓi�G���[����NULL�j*/
scePFontInfo const* scePFontGetFontInfo(int fd);

/* �֘A�Â����Ă���t�H���g����w��u���b�N�ւ̎Q�Ƃ����o���B
   fd : �t�H���g���ʎq
   idx : �u���b�N�ԍ�
   return : �t�H���g�u���b�N���ւ̎Q�Ɓi�G���[����NULL�j*/
scePFontBlock const* scePFontGetBlock(int fd, int idx);

/* �֘A�Â����Ă���t�H���g����w�蕶���R�[�h�ɑΉ�����O���t���擾�B
   fd : �t�H���g���ʎq
   chr : �����R�[�h
   pGlyph : glyph�f�[�^���󂯂Ƃ邽�߂̕ϐ��ւ̃|�C���^�B
   return : 0=����,-1=���s */
int scePFontGetGlyph(int fd, int chr, scePFontGlyph* pGlyph);

/* ���W�ϊ��}�g���N�X�̐ݒ�(���[�J�����X�N���[���j
   fd : �t�H���g���ʎq
   pMatrix : ���W�ϊ��}�g���N�X�ւ̃|�C���^ */
void scePFontSetScreenMatrix(int fd, sceVu0FMATRIX const* pMatrix);

/* ���݂̍��W�ϊ��}�g���N�X���擾
   fd : �t�H���g���ʎq
   pMatrix : ���W�ϊ��}�g���N�X���󂯂Ƃ�ϐ��ւ̃A�h���X */
void scePFontGetScreenMatrix(int fd, sceVu0FMATRIX* pMatrix);

/* �ό`�}�g���N�X�̐ݒ�B�����P�ʂ̃X�P�[�����O���]��ړ�
   fd : �t�H���g���ʎq
   pMatrix : �ό`�}�g���N�X�ւ̃|�C���^ */
void scePFontSetFontMatrix(int fd, sceVu0FMATRIX const* pMatrix);

/* ���݂̕ό`�}�g���N�X���擾
  �i���S�� base_x+width/2,base_y�j
   fd : �t�H���g���ʎq
   pMatrix : �ό`�}�g���N�X���󂯂Ƃ�ϐ��ւ̃A�h���X */
void scePFontGetFontMatrix(int fd, sceVu0FMATRIX* pMatrix);

/* �`��J�n�ʒu�ݒ�
   fd : �t�H���g���ʎq
   pLocate : �`��ʒu */
void scePFontSetLocate(int fd, sceVu0FVECTOR const* pLocate);

/* �J�����g�̕`��ʒu�擾
   fd : �t�H���g���ʎq
   pLocate : �ʒu���󂯂Ƃ�ϐ��ւ̃|�C���^ */
void scePFontGetLocate(int fd, sceVu0FVECTOR* pLocate);

/* �`��F�ݒ�i���̕����̐F�ɏ�Z�����j
   fd : �t�H���g���ʎq
   pColor : �F */
void scePFontSetColor(int fd, sceVu0FVECTOR const* pColor);

/* �`��F�擾
   fd : �t�H���g���ʎq
   pColor : �F���󂯂Ƃ�ϐ��ւ̃|�C���^ */
void scePFontGetColor(int fd, sceVu0FVECTOR* pColor);

/* ���Ԃ�ݒ�
   fd : �t�H���g���ʎq
   pitch : pitch */
void scePFontSetPitch(int fd, float pitch);

/* ���Ԃ��擾
   fd : �t�H���g���ʎq
   width : 0=�f�t�H���g 1<=�Œ蕶���� */
float scePFontGetPitch(int fd);

/* �v�Z��̕�������ݒ�i�\�����͕ς��Ȃ��j
   fd : �t�H���g���ʎq
   width : 0=�f�t�H���g 1<=�Œ蕶���� */
void scePFontSetWidth(int fd, short width);

/* �ݒ肳��Ă��镶�������擾
   fd : �t�H���g���ʎq
   width : 0=�f�t�H���g 1<=�Œ蕶���� */
short scePFontGetWidth(int fd);




/* �`��p�P�b�g�ɂ��Ă̕ߑ��B
   �p�P�b�g��cnt�ŕt����������B
   �A���t�@��on�ŕ`�悳���B
   �R���e�L�X�g�P���g�p�����B
   ALPHA_0��TEST_0�͊O���Őݒ肷�邱�ƁB*/

/* �P�����o��
   fd : �t�H���g���ʎq
   packet : �������ς݃p�P�b�g
   size : �ő�p�P�b�g�T�C�Y(qwc)
   chr : �o�͕����iUCS2�j
   return : 0=����, -1=���s�i�p�P�b�g�ɋ󂫂��Ȃ��j*/
int scePFontPutc(int fd, sceVif1Packet* pPacket, int size, int chr);

/* ������o�́i�p�P�b�g���j
   fd : �t�H���g���ʎq
   packet : �������ς݃p�P�b�g
   size : �ő�p�P�b�g�T�C�Y(qwc)
   str : ������
   return : 0=�I��, 1=����������, -1=�p�P�b�g������������ */
int scePFontPuts(int fd, sceVif1Packet* pPacket, int size, char const* str);

/* �t�H���g�o�͌p��
   fd : �t�H���g���ʎq
   packet : �������ς݃p�P�b�g
   size : �ő�p�P�b�g�T�C�Y(qwc)
   return : 0=�I��, 1=����������, -1=�p�P�b�g������������ */
int scePFontPutsContinue(int fd, sceVif1Packet* pPacket, int size);

/* �t�H���g�o�͔͈͌v�Z�i���[�J�����W�n�j
   �i���݂̃X�e�[�^�X�͕ۑ������j
   fd : �t�H���g���ʎq
   str : �o�̓e�X�g���镶����
   pRect_i : �����̋�`���󂯂Ƃ�ϐ��ւ̃|�C���^
   pRect_o : �O���̋�`���󂯂Ƃ�ϐ��ւ̃|�C���^ */
void scePFontCalcRect(int fd, char const* str, scePFontFRect* pRect_i, scePFontFRect* pRect_o);

/* �t�B���^�[�֐��ݒ�iscePFontPuts, scePFontPutsContinue, scePFontCalcRect����Ă΂��j
   func : �t�B���^�[�֐��ւ̃|�C���^�BNULL��n���ƃf�t�H���g�����B*/
#if 0 /* sample (default) */
/* �t�B���^�[�֐�
   fd : �t�H���g���ʎq
   pCtrl : �t�H���g����p�\����
   return  : 0=�I��, 1=����������, -1=�p�P�b�g������������ */
int default_filter(int fd, scePFontControl* pCtrl){
	int stat = -1;
	int c;
	while(0 < (c = pCtrl->Getc(fd))){
		if(-1 == pCtrl->Putc(fd, c)){
			pCtrl->Ungetc(fd);
			return stat;
		}
		stat = 1;
	};
	return 0;
}
#endif
void scePFontSetFilter(int fd, int (*filter)(int fd, scePFontControl* pCtrl));

/* libpfont ���C�u�����̃G���g���̎擾�B */
void *scePFontGetErxEntries(void);

#if defined(__cplusplus)
}
#endif

#endif /* __pfont__ */
