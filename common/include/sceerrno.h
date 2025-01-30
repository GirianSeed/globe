/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.7
 $Id: sceerrno.h,v 1.12.2.1 2002/10/24 06:26:58 kaol Exp $
 */
/* 
 * Emotion Engine Library / I/O Processor Library
 *
 * Copyright (C) 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * sceerrno.h - common error number definition and utilities
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2002-09-11      style       initial draft
 *      2002-09-13      style       revised
 *      2002-09-17      style       official first version
 *      2002-10-15      style       solve EE/IOP diffs
 *      2002-10-18      style       lib. prefix BNNETCNF added
 *      2002-10-22      style       `SCE_' prefixed standard errno in sceerr.h
 */

#ifndef _SCE_ERRNO_H
#define _SCE_ERRNO_H

#include <errno.h>
#ifdef R3000
#include <kerror.h>		/* IOP kernel original error no. */
#endif

/*
 * ����`�G���[�ԍ��̒�`:
 *     errno.h �� EE/IOP �ɂ����鍷�ق��z��
 */
#ifndef EFORMAT
#define EFORMAT		47
#endif
#ifndef EUNSUP
#define EUNSUP		48
#endif
#ifndef ENOSHARE
#define ENOSHARE	136
#endif

/*
 * `SCE_' �v���t�B�b�N�X�t���W���G���[�ԍ�
 */
#include <sceerr.h>

/*
 * SCE �Ǝ���`�G���[�ԍ�:
 * errno.h/kerror.h ����`�G���[�ԍ� (���� 16 �r�b�g: SCE_EBASE..0xffff)
 *     errno.h:         0-1xx
 *     kerror.h(IOP): 100-4xx
 */

#define SCE_EBASE		0x8000		/* 32768 */

/* �Ώۂ��ėp�ȃG���[ */
#define SCE_EINIT		0x8001		/* �������������s���Ă��Ȃ�	*/
#define SCE_EID			0x8002		/* ID �����݂��Ȃ�		*/
#define SCE_ESEMAPHORE		0x8003		/* �Z�}�t�H�������s���Ȃ�	*/
#define SCE_ESEMA		SCE_ESEMAPHORE
#define SCE_ETHREAD		0x8004		/* �X���b�h�������s���Ȃ�	*/
#define SCE_ETIMER		0x8005		/* �^�C�}�[/�A���[���������s���Ȃ� */

/* ���C�u�����ˑ��ȃG���[ */
#define SCE_EBASE_LIB		0x9000		/* 36864 */

#define SCE_EDEVICE_BROKEN	0x9001		/* �f�o�C�X�j���̉\��		*/
#define SCE_EFILE_BROKEN	0x9002		/* �t�@�C���܂��̓f�B���N�g���j���̉\�� */
#define SCE_ENEW_DEVICE		0x9003		/* �V�K�f�o�C�X���o		*/
#define SCE_EMDEPTH		0x9004		/* �f�B���N�g�����[�߂���	*/

#define SCE_ENO_PROGRAM		0x9005		/* �v���O�����`�����N������	*/
#define SCE_ENO_SAMPLESET	0x9006		/* �T���v���Z�b�g�`�����N������	*/
#define SCE_ENO_SAMPLE		0x9007		/* �T���v���`�����N������	*/
#define SCE_ENO_VAGINFO		0x9008		/* VAGInfo�`�����N������	*/
#define SCE_ENO_SBADDR		0x9009		/* �X�v���b�g�u���b�N�̃A�h���X��񂪖��� */
#define SCE_EBAD_PNUM		0x9010		/* �v���O�����i���o�[�͔͈͊O	*/
#define SCE_ENO_PNUM		0x9011		/* �v���O�����i���o�[�͖��g�p	*/
#define SCE_EBAD_SSNUM		0x9012  	/* �T���v���Z�b�g�i���o�[�͔͈͊O */
#define SCE_ENO_SSNUM		0x9013  	/* �T���v���Z�b�g�i���o�[�͖��g�p */
#define SCE_EBAD_SPNUM		0x9014  	/* �T���v���i���o�[�͔͈͊O	*/
#define SCE_ENO_SPNUM		0x9015  	/* �T���v���i���o�[�͖��g�p	*/
#define SCE_EBAD_VAGNUM		0x9016  	/* VAGInfo�i���o�[�͔͈͊O	*/
#define SCE_ENO_VAGNUM		0x9017  	/* VAGInfo�i���o�[�͖��g�p	*/
#define SCE_EBAD_SBNUM		0x9018  	/* �X�v���b�g�u���b�N�i���o�[�͔͈͊O */
#define SCE_EVAGINFO_NOISE	0x9019  	/* VAGInfo���w������VAG�̓m�C�Y�ł��� */
#define SCE_ENO_SPLITNUM	0x9020  	/* �X�v���b�g�i���o�[�͖��g�p	*/

/*
 * �ėp����I���}�N��
 */
#define SCE_OK	0

/*
 * ���C�u�������ʃv���t�B�b�N�X
 */

/* �G���[���ʃr�b�g */
#define SCE_ERROR_PREFIX_ERROR		0x80000000

/* �v���t�B�b�N�X�E�V�t�g�� */
#define SCE_ERROR_PREFIX_SHIFT		16

/* �G���[�E�r�b�g�}�X�N */
#define SCE_ERROR_MASK_LIBRARY_PREFIX	0x7fff0000
#define SCE_ERROR_MASK_ERRNO		0x0000ffff

/* ���C�u�������ʃv���t�B�b�N�X�l */
#define SCE_ERROR_PREFIX_MC2	  0x01010000 /* �������[�J�[�h        libmc2  */
#define SCE_ERROR_PREFIX_BNNETCNF 0x01020000 /* PS BB Navi. Net conf. bnnetcf */
#define SCE_ERROR_PREFIX_SDHD	  0x01030000 /* Sound Data .HD        libsdhd */

/*
 * ���[�e�B���e�B
 */ 

/* for library author */
#define SCE_ERROR_ENCODE(prefix,err) (SCE_ERROR_PREFIX_ERROR | (prefix) | (err))

/* for user */
#define SCE_ERROR_ERRNO(err)      ((err) & SCE_ERROR_MASK_ERRNO)
#define SCE_ERROR_LIB_PREFIX(err) ((err) & SCE_ERROR_MASK_LIBRARY_PREFIX)

#endif /* _SCE_ERRNO_H */
