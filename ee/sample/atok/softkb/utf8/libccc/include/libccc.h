/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library libccc Version 1.1
 */
/*
 *      Copyright (C) 2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                     <libccc - libccc.h>
 */

#ifndef __libccc__
#define __libccc__

#include <sys/types.h>
#if defined(__cplusplus)
extern "C"{
#endif

/* ------------------------------------------------------------------------- 
   �����R�[�h�ϊ����C�u���� 
  
   �E�}���`�X���b�h�͍l�����Ă��܂���B
   �E���݂̕ϊ��e�[�u����JIStoUCS2�AUCS2toJIS���ꂼ���65536�A�v256k�o�C�g�K�v�ł��B
   �ESJIS��0x20~0x7f�͈̔͂ɂ��Ă�ASCII 0x80~0xff�͈̔͂�"JIS X 0201"�����ł��B
   �EJIS��"JIS X 0213"������SCEI�Ǝ��̕����Z�b�g��z�肵�Ă��܂��B
   �Eunicode��"ver1.1"������SCEI�Ǝ��̕����Z�b�g��z�肵�Ă��܂��B
   ------------------------------------------------------------------------- */

/* �����R�[�h */
typedef unsigned short	sceCccJISCS;
typedef unsigned short	sceCccSJISCS;
typedef unsigned short	sceCccUCS2;
typedef unsigned int	sceCccUCS4;

/* �������� */
typedef unsigned char	sceCccJISTF;
typedef unsigned char	sceCccSJISTF;
typedef unsigned char	sceCccUTF8;
typedef unsigned short	sceCccUTF16;

/* �������ɂ��P����������̍ő�o�C�g�� */
#define __MAXCHRSIZE_SJIS__		(2)
#define __MAXCHRSIZE_UTF8__		(4)
#define __MAXCHRSIZE_UTF16__		(4)

/* ------------------------------------------------------------------------- 
   �����R�[�h�ϊ��e�[�u���ݒ�
   �ϊ����͕K���ݒ肷��K�v������܂�
   �e�[�u���T�C�Y��65536�BUCS2���z����͈͂̓T�|�[�g���Ȃ��B
   jis2ucs : jis ���� ucs2 �ւ̑Ή��\(jis����)
   ucs2jis : ucs2 ���� jis �ւ̑Ή��\(ucs2����)
   ------------------------------------------------------------------------- */
void sceCccSetTable(sceCccUCS2 const* jis2ucs, sceCccJISCS const* ucs2jis);


/* ------------------------------------------------------------------------- 
   �������J�E���g
   str : ������
   return : ������
   ------------------------------------------------------------------------- */
/* UTF16������̎����𐔂��� */
int sceCccStrlenUTF16(sceCccUTF16 const* str);

/* UTF8������̎����𐔂��� */
int sceCccStrlenUTF8(sceCccUTF8 const* str);

/* SJIS������̎����𐔂��� */
int sceCccStrlenSJIS(sceCccSJISTF const* str);


/* ------------------------------------------------------------------------- 
   �����R�[�h�̗L��������
   code : �����R�[�h
   return : 1=�L�� 0:����
   ���j���݂͌����ȃ`�F�b�N���s�Ȃ��Ă��܂���B
   ------------------------------------------------------------------------- */
/* UCS4�̃R�[�h�̗L�������� */
int sceCccIsValidUCS4(sceCccUCS4 code);

/* UCS2�̃R�[�h�̗L�������� */
int sceCccIsValidUCS2(sceCccUCS4 code);

/* JIS�̃R�[�h�̗L�������� */
int sceCccIsValidJIS(sceCccJISCS code);

/* UTF8�̃R�[�h�̗L�������� */
int sceCccIsValidUTF8(sceCccUCS4 code);

/* UTF16�̃R�[�h�̗L�������� */
int sceCccIsValidUTF16(sceCccUCS4 code);

/* SJIS�̃R�[�h�̗L�������� */
int sceCccIsValidSJIS(sceCccJISCS code);


/* ------------------------------------------------------------------------- 
   �P��������
   str : �擾���A�h���X�ւ̃|�C���^�B�擾��͈ꕶ�����i�ށB
   return : �����R�[�h
   ------------------------------------------------------------------------- */
/* UTF16�����񂩂�P�����擾 */
sceCccUCS4 sceCccDecodeUTF16(sceCccUTF16 const** str);

/* UTF8�����񂩂�P�����擾 */
sceCccUCS4 sceCccDecodeUTF8(sceCccUTF8 const** str);

/* SJIS�����񂩂�P�����擾 */
sceCccJISCS sceCccDecodeSJIS(sceCccSJISTF const** str);


/* ------------------------------------------------------------------------- 
   �P��������
   dst : �o�͐�A�h���X�ւ̃|�C���^�B�o�͌�͈ꕶ�����i�ށB
   code : �����R�[�h
   ------------------------------------------------------------------------- */
/* UTF16������Ƃ��ĂP�����o�� */
void sceCccEncodeUTF16(sceCccUTF16** dst, sceCccUCS4 code);

/* UTF8������Ƃ��ĂP�����o�� */
void sceCccEncodeUTF8(sceCccUTF8** dst, sceCccUCS4 code);

/* SJIS������Ƃ��ĂP�����o�� */
void sceCccEncodeSJIS(sceCccSJISTF** dst, sceCccJISCS code);


/* ------------------------------------------------------------------------- 
   �����R�[�h�ϊ�
   code : �����R�[�h
   chr : �ϊ��Ɏ��s�����Ƃ��ɏo�͂����L�����N�^(default:0)
   return : �ϊ���̃R�[�h
   ------------------------------------------------------------------------- */
/* UCS����JIS�ւP�����ϊ� */
sceCccJISCS sceCccUCStoJIS(sceCccUCS4 code, sceCccJISCS chr);

/* JIS����UCS�ւP�����ϊ� */
sceCccUCS4 sceCccJIStoUCS(sceCccJISCS code, sceCccUCS4 chr);


/* ------------------------------------------------------------------------- 
   �����񕄍������ϊ�
   dst : �o�͕�����
   count : �o�̓o�b�t�@�T�C�Y
   str : ���͕�����
   return : �ϊ���̕�����
   ------------------------------------------------------------------------- */
/* UTF8����UTF16�֕ϊ� */
int sceCccUTF8toUTF16(sceCccUTF16* dst, size_t count, sceCccUTF8 const* str);

/* UTF8����SJIS�֕ϊ� */
int sceCccUTF8toSJIS(sceCccSJISTF* dst, size_t count, sceCccUTF8 const* str);

/* SJIS����UTF8�֕ϊ� */
int sceCccSJIStoUTF8(sceCccUTF8* dst, size_t count, sceCccSJISTF const* str);

/* SJIS����UTF16�֕ϊ� */
int sceCccSJIStoUTF16(sceCccUTF16* dst, size_t count, sceCccSJISTF const* str);

/* UTF16����SJIS�֕ϊ� */
int sceCccUTF16toSJIS(sceCccSJISTF* dst, size_t count, sceCccUTF16 const* str);

/* UTF16����UTF8�֕ϊ� */
int sceCccUTF16toUTF8(sceCccUTF8* dst, size_t count, sceCccUTF16 const* str);


/* ------------------------------------------------------------------------- 
   ������ϊ����̖����f�[�^�u�������p�L�����N�^�ݒ�B
   chr : �L�����N�^
   return : �ȑO�ɐݒ肳��Ă����R�[�h
   ------------------------------------------------------------------------- */
sceCccJISCS sceCccSetErrorCharSJIS(sceCccJISCS chr);
sceCccUCS4 sceCccSetErrorCharUTF8(sceCccUCS4 chr);
sceCccUCS4 sceCccSetErrorCharUTF16(sceCccUCS4 chr);


/* ------------------------------------------------------------------------- 
   libccc���C�u�����̃G���g���̎擾�B
   return : libccc�̃G���g�����C�u����
   ------------------------------------------------------------------------- */
void *sceCccGetErxEntries(void);


#if defined(__cplusplus)
}
#endif


#endif /* __libccc__ */
