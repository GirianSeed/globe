/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *              Emotion Engine Library Sample Program
 *                          Version 1.20
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        EzBGM - bgm_rpc.c
 *                        rpc routine for EzBGM
 *
 *     Version    Date          Design     Log
 *  --------------------------------------------------------------------
 *     1.20       Nov.23.1999   morita     first checked in.
 */

#include <eekernel.h>
#include <sif.h>
#include <sifrpc.h>
#include "bgm_i.h"

#define DATA_SIZE_STRING (64)
#define DATA_SIZE_NORMAL (16)

/* RPC ��n���p�f�[�^�o�b�t�@
 * 64 bytes: �L���b�V�����C���s����������邽�߂� */
static u_int sbuff[16] __attribute__((aligned (64)));
static sceSifClientData gCd;


/*
 * ezBgmInit()		SIF RPC �ڑ��̊m��
 *	����:   �Ȃ�
 *	�Ԃ�l:
 *		�ڑ��m���̌���;
 *		   1 ... �m������
 */
int ezBgmInit( void )
{
	int i;

	sceSifInitRpc(0);

	while(1) {
		if (sceSifBindRpc( &gCd, EZBGM_DEV, 0) < 0) {
			scePrintf("error: sceSifBindRpc \n");
			while(1);
		}
		i = 10000;
		while( i-- ){}
		if(gCd.serve != 0) break;
	}

	return 1;
}


/* ezBgm()		RPC �ɂăR�}���h�Ŏw�肳�ꂽ IOP ���̊֐������s����
 *	����:
 *		command: EzBGM_COMMAND_*: IOP �����R�}���h
 *		data:    �R�}���h�I�v�V����
 *	�Ԃ�l:
 *		IOP ���Ŏ��s���ꂽ�֐����Ԃ����Ԃ�l
 */
int ezBgm( int command,  int data )
{
	int rsize = 0;

//	if( (command&0x8000) !=0 )
		rsize = 64;

	/* EzBGM_OPEN �̂� data �𕶎���Ƃ��Ă��̂܂ܓn���B
           ���͐��l�� sbuf[0] �ɑ�����ēn�� */
	if( (command&0xFFF0) == EzBGM_OPEN ){
		sceSifCallRpc( &gCd,  command, 0, (void *)data
		, DATA_SIZE_STRING, (void *)(&sbuff[0]),rsize,NULL,NULL);
	}
	else{
		sbuff[0] = data;
		sceSifCallRpc( &gCd,  command, 0, (void *)(&sbuff[0])
		, DATA_SIZE_NORMAL, (void *)(&sbuff[0]),rsize,NULL,NULL );
	}

	return sbuff[0];
}


