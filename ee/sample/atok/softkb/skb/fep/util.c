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

#include <stdio.h>

#define	IS_KANJI(_c)		((((_c)>=0x81) && ((_c)<=0x9F)) || (((_c)>=0xE0) && ((_c)<=0xFC)))
#define	IS_HANKAKU(_c)		(((_c)>=0xA1) && ((_c)<=0xDF))
#define	IS_ASCII(_c)		(((_c)>=0x20) && ((_c)<=0x7F))

unsigned int CharHanToZen(int c);
unsigned int CharZenToHan(unsigned int sjis);
void StrHanToZen(char *pszDst, const char *pszSrc);
void StrZenToHan(char *pszDst, const char *pszSrc);


// ���p������S�p�����ɕϊ�����
unsigned int CharHanToZen(int c)
{
	static const char szHan2Zen[] =
		"�@�I�h���������f�i�j���{�C�|�D�^"		// 0x20�`0x2F
		"�O�P�Q�R�S�T�U�V�W�X�F�G�������H"		// 0x30�`0x3F
		"���`�a�b�c�d�e�f�g�h�i�j�k�l�m�n"		// 0x40�`0x4F
		"�o�p�q�r�s�t�u�v�w�x�y�m�_�n�O�Q"		// 0x50�`0x5F	0x5C�͉~�}�[�N�Ƃ����ꍇ������
		"�e������������������������������"		// 0x60�`0x6F
		"�����������������������o�b�p�`  "		// 0x70�`0x7F	0x7E�̓A�b�p�[���C���Ƃ����ꍇ������
		"                                "		// 0x80�`0x8F
		"                                "		// 0x90�`0x9F
		"  �B�u�v�A�E���@�B�D�F�H�������b"		// 0xA0�`0xAF
		#ifdef LANG_CSJIS
		"�[�A�C�E�G�I�J�L�N�P�I�T�V�X�Z�\"		// 0xB0�`0xBF
		#else
		"�[�A�C�E�G�I�J�L�N�P�I�T�V�X�Z�\\"	//"	// 0xB0�`0xBF
		#endif
		"�^�`�c�e�g�i�j�k�l�m�n�q�t�w�z�}"		// 0xC0�`0xCF
		"�~���������������������������J�K"		// 0xD0�`0xDF
		"                                "		// 0xE0�`0xEF
		"                                ";		// 0xF0�`0xFF
	unsigned int sjis;

	if(IS_KANJI(c)) {
		// SJIS��2�o�C�g�̂Ƃ��A���̂܂܃R�s�[
		sjis = c;
	} else if(IS_ASCII(c)) {
		// ���pASCII��S�p�ɕϊ�����ꍇ
		sjis = (szHan2Zen[c*2]<<8) | szHan2Zen[c*2 + 1];
	} else if(IS_HANKAKU(c)) {
		// ���p�J�^�J�i�������ꍇ
		sjis = (szHan2Zen[c*2]<<8) | szHan2Zen[c*2 + 1];
	} else {
		// 1�o�C�g�����Ƃ���
		sjis = c;
	}
	return(sjis);
}


// �S�p�����𔼊p�����ɕϊ�����
unsigned int CharZenToHan(unsigned int sjis)
{
	unsigned char c=0;
	// �Ђ炪�Ȕ���p
	static const char us829F_82F1[] =
		"� � � � � � � � � � � �޷ �޸ ��"
		"� �޺ �޻ �޼ �޽ �޾ �޿ ��� ��"
		"� �ޯ � ��� ��� ��� � � � � � ��"
		"��� ����� ����� ����� ����� � � "
		"� � � � � � � � � � � � �   � "
		"    � � ��    ";

	// �J�^�J�i����p
	static const char us8340_8396[] =
		"� � � � � � � � � � � �޷ �޸ ��"
		"� �޺ �޻ �޼ �޽ �޾ �޿ ��� ��"
		"� �ޯ � ��� ��� ��� � � � � � ��"
		"��� ����� ����� ����� ����� �   "
		"� � � � � � � � � � � � � �   � "
		"    � � ��    ";

	if(sjis<0x10) {
		// 1�o�C�g�����Ƃ��āA���̂܂܃R�s�[
		c = sjis;
	} else {
		// SJIS��2�o�C�g�����̂Ƃ��AszZen2Han���ɕ��������邩�ǂ���
		if(0x824F<=sjis && sjis<=0x8258) {
			// '0'�`'9'
			return((sjis - 0x824F) + '0');
		} else if(0x8260<=sjis && sjis<=0x8279) {
			// 'A'�`'Z'
			return((sjis - 0x8260) + 'A');
		} else if(0x8281<=sjis && sjis<=0x829A) {
			// 'a'�`'z'
			return((sjis - 0x8281) + 'a');
		} else if(0x829F<=sjis && sjis<=0x82F1) {
			// '��'�`'��'
			unsigned char c1=0, c2=0;

			c1 = us829F_82F1[(sjis - 0x829F) * 2];
			c2 = us829F_82F1[(sjis - 0x829F) * 2 + 1];
			if(c1==' ') {
				// �ϊ��ł��Ȃ�����(��,��,��)
				return(0);
			} else if(c2==' ') {
				// ���p1�����ɕϊ��ł��镶��
				return(c1);
			} else {
				// ���p2�����ɕϊ��ł���ꍇ
				return((c1<<8) | c2);
			}
		} else if(0x8340<=sjis && sjis<=0x8396) {
			// '�A'�`'��'
			unsigned char c1, c2;
			c1 = us8340_8396[(sjis - 0x8340) * 2];
			c2 = us8340_8396[(sjis - 0x8340) * 2 + 1];
			if(c1==' ') {
				// �ϊ��ł��Ȃ�����(��,��,��,��,��)
				return(0);
			} else if(c2==' ') {
				// ���p1�����ɕϊ��ł��镶��
				return(c1);
			} else {
				// ���p2�����ɕϊ��ł���ꍇ
				return((c1<<8) | c2);
			}
		}
	}
	return(c);
}


// ���p�J�^�J�i��S�p�J�^�J�i�ɕϊ�����
void StrHanToZen(char *pszDst, const char *pszSrc)
{
	unsigned char c;
	unsigned int sjis;

	do {
		c = *pszSrc++;
		sjis = CharHanToZen(c);
		if(sjis<0x100) {
			*pszDst++ = c;
		} else {
			*pszDst++ = sjis>>8;
			*pszDst++ = sjis & 0xFF;
		}
	} while(c!=0x00);
	return;
}



// �S�p�Ђ炪��,�J�^�J�i�𔼊p�J�^�J�i�ɕϊ�����
void StrZenToHan(char *pszDst, const char *pszSrc)
{
	unsigned char c;

	do {
		c = *pszSrc++;
		if(IS_KANJI(c)) {
			// SJIS��2�o�C�g�̂Ƃ��A���̂܂܃R�s�[
			// 
			*pszDst++ = c;
			*pszDst++ = *pszSrc++;
		} else if(IS_ASCII(c)) {
			// ���pASCII�������Ƃ�
			*pszDst++ = c;
		} else if(IS_HANKAKU(c)) {
			// ���p�J�^�J�i�������ꍇ
			*pszDst++ = c;
		} else {
			// 1�o�C�g�����Ƃ���
			*pszDst++ = c;
		}
	} while(c!=0x00);

	return;
}


