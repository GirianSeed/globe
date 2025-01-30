/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *              Emotion Engine Library Sample Program
 *                          Version 0.11
 *                           Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *	                  EzADPCM - rpc.c
 *		             RPC routine
 *
 *	Version		Date		Design	Log
 *  --------------------------------------------------------------------
 *	0.10		Feb. 3, 2000	kaol		
 *	0.11		Feb.27, 2000	kaol	same as 0.10
 */

#include <eekernel.h>
#include <sif.h>
#include <sifrpc.h>
#include "ezadpcm.h"

#define DATA_SIZE_STRING 64
#define DATA_SIZE_NORMAL 16


/* RPC ��n���p�f�[�^�o�b�t�@
 * 64 bytes: �L���b�V�����C���s����������邽�߂� */
static unsigned int sbuf [16] __attribute__((aligned (64)));
static sceSifClientData cd;

/*
 * ezAdpcm_RPCinit()	SIF RPC �ڑ��̊m��
 *	����:   �Ȃ�
 *	�Ԃ�l:
 *		�ڑ��m���̌���;
 *		   0 ... �m������
 *		  -1 ... �G���[
 */
int
ezAdpcm_RPCinit (void)
{
    sceSifInitRpc (0);
    while (1) {
	if (sceSifBindRpc (&cd, EzADPCM_DEV, 0) < 0) {
	    scePrintf ("error: sceSifBindRpc \n");
	    return (-1);
	}
	if (cd.serve != 0) break;
    }
    return 0;
}

/* ezAdpcm_RPC()	RPC �ɂăR�}���h�Ŏw�肳�ꂽ IOP ���̊֐������s����
 *	����:
 *		command: EzADPCM_COMMAND_*: IOP �����R�}���h
 *		data:    �R�}���h�I�v�V����
 *	�Ԃ�l:
 *		IOP ���Ŏ��s���ꂽ�֐����Ԃ����Ԃ�l
 */
int
ezAdpcm_RPC (int command, int data)
{
    int rsize = 64;

    /* EzADPCM_OPEN �̂� data �𕶎���Ƃ��Ă��̂܂ܓn���B
       ���͐��l�� sbuf[0] �ɑ�����ēn�� */
    if ((command & EzADPCM_COMMAND_MASK) == EzADPCM_OPEN) {
	sceSifCallRpc (&cd,  command, 0, (void *) data, DATA_SIZE_STRING,
		       (void *) sbuf, rsize, NULL, NULL);
    } else {
	sbuf[0] = data;
	sceSifCallRpc (&cd,  command, 0, (void *) sbuf, DATA_SIZE_NORMAL,
		       (void *) sbuf, rsize, NULL, NULL);
    }

    return sbuf [0];
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADD STUFF AFTER THIS */
