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
 *                          Name : pad.c
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000      
 *
 */

/*******************************************************
 * �T�v
 *	Frame Work���ł� Pad Operation�ׂ̈̊֐��Q
 *
 *	Frame Work���ł� ��̃p�b�h�����������Ă܂���B
 *	�������A�����̊֐��̂�����Ƃ̉����ŕ����ɑΉ��ł��܂��B
 *	�܂� fwPadState_t�^�̕ϐ��̃r�b�g�����o��
 *		�`On
 *		�`Switch
 *	������܂����A�`On�� ������Ă���ԏ�ɂP�ƂȂ�r�b�g
 *	�`Switch�� �����ꂽ�u�Ԃ����P�ƂȂ�r�b�g���Ӗ����܂��B
 *******************************************************/

#include <eekernel.h>
#include <libpad.h>
#include "pad.h"

/*******************************************************
 * Global Variables (substance definition)
 *******************************************************/
fwPadState_t	fwPadStateVal;

/*******************************************************
 * Static Variables
 *******************************************************/
u_long128	_fwPadBufVal[scePadDmaBufferMax] __attribute__((aligned(64)));

/*******************************************************
 * fwPadInit
 *	Pad�̏�����
 *******************************************************/
void fwPadInit(void)
{
    scePadInit(0);
    scePadPortOpen(0, 0, _fwPadBufVal);
}

/*******************************************************
 * fwPadCheck
 *	Pad Data�̍쐬�i���t���[���ĂԂ��Ɓj
 *	fwPadState_t�̕��т� libpad.h�ł̒�`�ɑ�����
 *	���тɂȂ��Ă���A�Z����bit����œ��邱�Ƃ��ł���
 *	�l�ɂȂ��Ă��邪�A�����n�ˑ��ȃR�[�h�ł��邱�Ƃ�
 *	����
 *******************************************************/
void fwPadCheck(void)
{
    static u_short	oldpad = 0;
    u_short		pad;
    u_char		rdata[32];
    u_int		data;
    
    if (scePadRead(0, 0, rdata) > 0) 
		pad = 0xffff ^ ((rdata[2] << 8) | rdata[3]);
    else
	pad = 0;    

    data = ((u_int)pad) << 16;
    data |= pad & (~oldpad);
    fwPadStateVal = *((fwPadState_t *)&data);
    oldpad = pad;
}
