/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *               Graphics Framework Sample Program
 *                  
 * 
 *      Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          Name : user.c
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000    
 */

/***************************************************
 * �T�v
 *	User Application���ŗL�ɏ���������ł��낤
 *	�������ꕔ���o
 *	����Aapplication�ˑ�������������Ɉڂ��\��
 *	(source�̍ė��p�������߂邽��)
 *
 *	���ۂ�application�ł͂����͏����������A
 *	�������� �폜�����Ǝv������̂𒆐S��
 *	�����o���Ă���
 ***************************************************/

#include <eekernel.h>
#include <libvu0.h>
#include <libgraph.h>
#include "defines.h"
#include "packbuf.h"
#include "frame_t.h"
#include "data.h"
#include "micro.h"
#include "malloc.h"
#include "micro.h"
#include "info.h"
#include "object.h"
#include "camera.h"
#include "pad.h"

/*
  micro.dsm�Œ�`����Ă���}�C�N���R�[�h(packet)�̊e��f�[�^
*/
extern u_int UserMicro_base[] __attribute__((section(".vudata")));
extern u_int UserMicro_cull[] __attribute__((section(".vudata")));
extern u_int UserMicro_pointl[] __attribute__((section(".vudata")));
extern u_int UserMicro_spotl[] __attribute__((section(".vudata")));
extern u_int UserMicro_fog[] __attribute__((section(".vudata")));
extern u_int UserMicro_anti[] __attribute__((section(".vudata")));

static const char *_UserMicroName[]={  // for DrawStatus
    "basic", "cull", "point light", "spot light", "fog", "AA1-polygon AntiAlias"
};

static fwMicroTable_t	_UserMicroTblVal[] =
{ 
    { UserMicro_base,	FW_MATTR_NONE },	/* BASE */
    { UserMicro_cull,	FW_MATTR_NONE },	/* CULL */
    { UserMicro_pointl,	FW_MATTR_NONE },	/* POINT */
    { UserMicro_spotl,	FW_MATTR_NONE },	/* SPOT */
    { UserMicro_fog,	FW_MATTR_FGE },		/* FOG */
    { UserMicro_anti,	FW_MATTR_ANTI }		/* ANTI */
};

static int	_UserMicroTblNumVal = sizeof(_UserMicroTblVal) / sizeof(fwMicroTable_t);

/*
  UserInit
  	User��`�����̏����������邽�߂̊֐�
	����
		�E micro code table
	��ݒ肷�邱�Ƃ��s���Ă���
*/
void UserInit(void)
{
    /* Micro Code Table�̐ݒ� */
    fwMicroSetTable(_UserMicroTblVal, _UserMicroTblNumVal);
}

/*
  UserMain
  	User��`�����̎��s���s�����߂̊֐�
	����
		�E micro code name��information�ւ̓o�^
	���s���Ă���
*/
void UserMain(void)
{
    /* Information �� current micro code��name���Z�b�g */
    fwInfoStatusVal.micro_name = _UserMicroName[fwMicroGetCurrent()];
}
