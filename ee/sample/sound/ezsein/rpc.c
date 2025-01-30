/* SCE CONFIDENTIAL
 * "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 * Emotion Engine Library Sample Program
 *
 * Copyright (C) 2003 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * ezsein - rpc.c
 *     rpc routine for EzSEIn
 */

#include <eekernel.h>
#include <sif.h>
#include <sifrpc.h>
#include <csl.h>
#include <cslse.h>
#include "ezsein.h"

#define DATA_SIZE_STRING (64)
#define DATA_SIZE_NORMAL (16)

static u_int sbuff[32] __attribute__((aligned (64)));
static sceSifClientData gCd;

/* ---------------------------------------------------------------- */
/* SIF RPC �̏�����                                                */
/* ---------------------------------------------------------------- */
int ezSeInit (void)
{
    int i;

    while (1) {
	if (sceSifBindRpc(&gCd, EZSEIN_DEV, 0) < 0) {
	    scePrintf("Error: sceSifBindRpc \n");
	    while(1);
	}
	i = 10000;
	while (i --){}
	if (gCd.serve != 0) break;
    }

    return 0;
}

/* ---------------------------------------------------------------- */
/* SIF RPC �̎��s                                                  */
/* ---------------------------------------------------------------- */
int ezSe (int command, int data)
{
    int rsize = 0;

    /* �߂�l������ꍇ�͎�M����f�[�^�o�b�t�@�̃T�C�Y���w�肷�� */
    if ((command & 0x8000) != 0)
	rsize = 64;

    if ((command & 0x1000) != 0){
	/* �������\���̂̏ꍇ */
	sceSifCallRpc(&gCd, command, 0,
		      (void *)data, DATA_SIZE_STRING,
		      (void *)(&sbuff[0]), rsize, NULL, NULL);
    } else {
	sbuff [0] = data;
	sceSifCallRpc(&gCd, command, 0,
		      (void *)(&sbuff[0]), DATA_SIZE_NORMAL,
		      (void *)(&sbuff[0]), rsize, NULL,NULL);
    }

    return sbuff [0];
}

/* ---------------------------------------------------------------- */
/* �f�[�^�]�� (SE ���b�Z�[�W�̓]��)                                */
/* ---------------------------------------------------------------- */
static sceSifDmaData transData;
int ezTransToIOP(void *iopAddr, void *eeAddr, int size)
{
    unsigned int did;

    transData.data = (u_int)eeAddr;
    transData.addr = (u_int)iopAddr;
    transData.size = size;
    transData.mode = 0;

    /* FlushCashe�͒x���̂ŁA��L���b�V���A�N�Z�X�̕����ǂ����Ƃ��B */
    FlushCache(WRITEBACK_DCACHE); 

    did = sceSifSetDma(&transData, 1);
    while (sceSifDmaStat(did) >= 0){}
    PRINTF(("send data completed\n"));
    if (did == 0) return (-1);
    return 0;
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADD STUFF AFTER THIS */
