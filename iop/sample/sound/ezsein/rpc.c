/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 * I/O Processor Library Sample Program
 *
 * Copyright (C) 2003 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * ezsein - rpc.c
 *     command dispatch routine
 */

#include <kernel.h>
#include <sys/types.h>
#include <stdio.h>
#include <sif.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <libsd.h>
#include <csl.h>
#include <cslse.h>
#include "ezsein.h"

/* EE����]�������RPC�̈����̎󂯌� */
int gRpcArg[16];

static void *rpc_dispatch(unsigned int fno, void *data, int size);

extern int SeInit(int foo);
extern int SeQuit(void);
extern int SeSetHd(EzSEInBank *bank);
extern int SeStop(int addr);
extern int SeTransBd(EzSEInBank *bank);
extern int SeTransBdPacket(EzSEInBank *bank);
extern int SeGetIopFileLength(char *filename);
extern int SeGetStatus(void);
extern void SeSetMaxVoices(unsigned int attr);
extern void SeSetPortAttr(int attr);
extern void SeSetPortVolume(int vol);

/* ---------------------------------------------------------------- */
/*  SIF RPC �T�[�o: EE �̃��N�G�X�g���󂯂Ƃ�A������U�蕪����     */
/* ---------------------------------------------------------------- */
int rpc_server(void)
{
    sceSifQueueData qd;
    sceSifServeData sd;

    sceSifInitRpc(0);

    sceSifSetRpcQueue(&qd, GetThreadId());
    sceSifRegisterRpc(&sd, EZSEIN_DEV,
		      rpc_dispatch, (void *)gRpcArg, NULL, NULL, &qd);
    PRINTF(("goto SE command loop\n"));

    /* �R�}���h�҂����[�v */
    sceSifRpcLoop(&qd);

    return 0;
}

/* ---------------------------------------------------------------- */
/*  EE����̃��N�G�X�g�ɂ���ČĂ΂��֐�                          */
/* ---------------------------------------------------------------- */
static int ret = 0;

static void *rpc_dispatch(unsigned int command, void *data, int size)
{ 
    PRINTF ((" sefunc [%x] %x, %x, %x, %x\n",
	     command,
	     *((int *)data + 0), *((int *)data + 1),
	     *((int *)data + 2), *((int *)data + 3)));
    
    switch(command & 0xfff0) {
    case EZSEIN_INIT:
	/* CSL ���̏������Ə����̊J�n */
	ret = SeInit(*((int *)data));
	break;
    case EZSEIN_QUIT:
	/* �����I�� */
	SeQuit();
	break;
    case EZSEIN_SETHD:
	/* HD �f�[�^���̐ݒ� */
	SeSetHd((EzSEInBank *)((int *)data));
	break;
    case EZSEIN_TRANSBD:
	/* BD �f�[�^�̓]�� */
	ret = SeTransBd((EzSEInBank *)((int *)data));
	break;
    case EZSEIN_TRANSBDPACKET:
	/* BD �t�@�C���̓ǂݍ��� + �]�� */
	ret = SeTransBdPacket((EzSEInBank *)((int *)data));
	break;
    case EZSEIN_GETFILELENGTH:
	/* �t�@�C���T�C�Y�̎擾 */
	ret = SeGetIopFileLength((char *)((int *)data));
	break;
    case EZSEIN_SETATTR:
	/* CSL �n�[�h�E�F�A�V���Z�T�C�U: �|�[�g�����̐ݒ� */
	SeSetPortAttr(*((int *)data));
	break;
    case EZSEIN_SETVOL:
	/* CSL �n�[�h�E�F�A�V���Z�T�C�U: ���t�{�����[���̐ݒ� */
	SeSetPortVolume(*((int *)data));
	break;
    case EZSEIN_SETMAXVOICES:
	/* CSL �n�[�h�E�F�A�V���Z�T�C�U: se-stream �ɂ��S���ʉ������������̐ݒ� */
	
	SeSetMaxVoices(*((unsigned int *)data));
	break;
    default:
	ERROR(("EzSEIn driver error: unknown command %d \n", *((int *)data)));
	break;
    }
    PRINTF(("return value = %x\n", ret)); 

    return((void *)(&ret));
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* DON'T ADD STUFF AFTER THIS */

