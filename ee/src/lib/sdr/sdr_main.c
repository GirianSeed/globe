/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: sdr_main.c,v 1.30 2003/04/22 11:10:45 kaol Exp $
 */
/* 
 * Emotion Engine Library
 *
 * Copyright (C) 1998-2003 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * libsdr - sdr_main.c
 *     main funcitons
 */

#include <eekernel.h>
#include <sif.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <libsdr.h>
#include <stdarg.h>
#include <sdrcmd.h>
#include "sdr_i.h"

#define DATA_SIZE_B (64)
#define UNCHASH 0x20000000

static u_int sbuff [16] __attribute__((aligned (64)));
static sceSifClientData sceSd_gCd;

static sceSifEndFunc sceSdEndFunc = NULL;
static         void *sceSdEndGp   = NULL;


/* ------------------------------------------------------------------------
   SPU2�����[�g���̏������B������IOP��RPC�֐���Bind���s���B
   ------------------------------------------------------------------------*/
int
sceSdRemoteInit (void)
{
	int i;

	sceSifInitRpc(0);
	while (1) {
		if (sceSifBindRpc (&sceSd_gCd, sce_SDR_DEV, 0) < 0) {
			scePrintf("sceSdRemoteInit() RPC bind error!\n");
			return (-1);
		}
		i = 10000;
		while (i--) {
		}
		if(sceSd_gCd.serve != 0) break;
	}

	sceSdEndFunc = NULL;
	sceSdEndGp   = NULL;

	FlushCache (0); /* �O�̂��� */
	/* sceSdRemote (1, rSdInit); */

	return 0;
}

/* ------------------------------------------------------------------------
   EE��̃o�b�t�@��IOP�������ɓ]������B
   *buff ��DRAM��ɏ���Ă���K�v������B�i�L���b�V���̂��Ƃ͊֒m���Ȃ��j
   ------------------------------------------------------------------------*/
/* ����:
 *     SIF DMA �ɂ����āA���� EE ���ɂ� sceSifDmaStatIntr() ��
 *     ��������Ă��Ȃ����߁A�m���u���b�N�w�� (isBlock: 0) �͖����B
 */
int
sceSdTransToIOP (void *buff, u_int sendAddr, u_int size, u_int isBlock)
{
	unsigned int did;
	static sceSifDmaData transData;

	if (isBlock == 0) {
		return (-1);	/* ���݂̎����ł̓m���u���b�N�w��͖��� */
	}

	transData.data = (u_int)buff;
	transData.addr = (u_int)sendAddr;
	transData.size = size;
	transData.mode = 0;

	did = sceSifSetDma (&transData, 1);
	if (did == 0) return (-1);

	if (isBlock == 1) {
		while (sceSifDmaStat (did) >= 0){}
		PRINTF(("send SpuStEnv completed \n"));
	}

	return 0;
}

/* ------------------------------------------------------------------------
   SIF�ɂ��]�����I���������ɔ�������A���荞�݃n���h����ݒ�B
   ------------------------------------------------------------------------*/
sceSifEndFunc
sceSdCallBack (sceSifEndFunc end_func)
{
	sceSifEndFunc old_func;

	old_func = sceSdEndFunc;
	sceSdEndFunc = end_func;
	sceSdEndGp   = GetGp();

	return (old_func);
}

/* ------------------------------------------------------------------------
   IOP����libsd�̊֐��������[�g���s����B
   ------------------------------------------------------------------------*/
int
sceSdRemote (int arg, ...)
{
	int ret, i;
	unsigned int isBlock, command;
	va_list ap;
	sceSifEndFunc end_func = NULL;
	int intr_ret;
	void *curGp = NULL;

	va_start (ap, arg);
	isBlock = arg;
	command = va_arg (ap, int);
	sbuff [0] = (int)sbuff; 
	for (i = 1; i < 7; i++) {
		sbuff [i] = va_arg (ap, int);
	}

	if (isBlock == 0){
		isBlock = SIF_RPCM_NOWAIT;
		end_func = sceSdEndFunc;
	} else {
		isBlock = 0;
	}

	intr_ret = (int)NULL;
#ifdef SCE_OBSOLETE
	if (command == rSdSetTransCallback) {
		if ((void *)sbuff [1] == 0){ /* ��P�����̓`�����l���ԍ� */
			intr_ret = (int)_sce_sdr_gDMA0CB;
			_sce_sdr_gDMA0CB = (void *)sbuff [2];
			_sce_sdr_gDMA0Gp = GetGp();
		} else {
			intr_ret = (int)_sce_sdr_gDMA1CB;
			_sce_sdr_gDMA1CB = (void *)sbuff [2];
			_sce_sdr_gDMA1Gp = GetGp();
		}
	} else if (command == rSdSetIRQCallback) {
		intr_ret = (int)_sce_sdr_gIRQCB;
		_sce_sdr_gIRQCB = (void *)sbuff [1];
		_sce_sdr_gIRQGp = GetGp();
	} else
#endif
	{
		if (command == rSdSetTransIntrHandler) {
			if (sbuff [1] == 0) { /* �� 1 �����̓`�����l���ԍ� */
				intr_ret = (int)_sce_sdr_transIntr0Hdr;
				_sce_sdr_transIntr0Hdr = (sceSdTransIntrHandler)sbuff [2];
				_sce_sdr_transIntr0Arg = (void *)sbuff [3];
				_sce_sdr_transIntr0Gp  = GetGp();
			} else{
				intr_ret = (int)_sce_sdr_transIntr1Hdr;
				_sce_sdr_transIntr1Hdr = (sceSdTransIntrHandler)sbuff [2];
				_sce_sdr_transIntr1Arg = (void *)sbuff [3];
				_sce_sdr_transIntr1Gp  = GetGp();
			}
		} else if (command == rSdSetSpu2IntrHandler) {
			intr_ret = (int)_sce_sdr_spu2IntrHdr;
			_sce_sdr_spu2IntrHdr = (sceSdSpu2IntrHandler)sbuff [1];
			_sce_sdr_spu2IntrArg = (void *)sbuff [2];
			_sce_sdr_spu2IntrGp  = GetGp();
		}
	}

	PRINTF(("sceSifCallRpc start - [%04x] ", command));
	PRINTF(("send value = %x %x, %x, %x\n", sbuff [0], sbuff [1], sbuff [2]));

	ret = 0;
	if (command == rSdSetEffectAttr ||
		command == rSdSetEffectMode ||
		command == rSdSetEffectModeParams) {
		/* SET�n�ō\���̂Œl��n������ */
		if (end_func != NULL) curGp = SetGp(sceSdEndGp);
		sceSifCallRpc (&sceSd_gCd, (command | sbuff [1]), isBlock, 
					   (void *)(sbuff [2]), DATA_SIZE_B,
					   sbuff, DATA_SIZE_B,
					   end_func, (void *)(sbuff [0]));
		if (end_func != NULL) (void)SetGp(curGp);
		ret = sbuff [0];
	} else if (command == rSdGetEffectAttr) {
		/* GET�n�ō\���̂ɒl���Ԃ��Ă������ */
		if (end_func != NULL) curGp = SetGp(sceSdEndGp);
		sceSifCallRpc (&sceSd_gCd, (command | sbuff [1]), isBlock, 
					   (void *)(&sbuff [0]), DATA_SIZE_B,
					   (void *)(sbuff [2]), 64, 
					   end_func, (void *)(sbuff [2]));
		if (end_func != NULL) (void)SetGp(curGp);
	} else if (command == rSdProcBatch2 ||
			   command == rSdProcBatchEx2) {
		/* �\���̂Œl��n���A�������̈�ɒl���Ԃ��Ă������ */
		sceSdBatch *p;
		u_int r;
		int s;
		/* sceSdProcBatch() �ɓn�������̃Z�b�g�A�b�v:
		   �R�}���h�z��� 0 �Ԗڂ̗v�f���g�� */
		p = (sceSdBatch *)sbuff [1];	/* 1: �R�}���h�z�� */
		p [0].entry = sbuff [2];	/* 2: �R�}���h�� */
		if (command == rSdProcBatchEx2) {
			p [0].value = sbuff [5];	/* 5: �r�b�g�p�^�[�� */
		}
		r = sbuff [3];			/* 3: �Ԃ�l�ۑ��̈� */
		s = sbuff [4];			/* 4: �Ԃ�l�ۑ��̈�T�C�Y */
		if (sbuff [3] == (u_int)NULL) {
			r = (unsigned int)&ret;	/* �֐��̕Ԃ�l�̂ݎ󂯂Ƃ� */
			s = 4;
		}
		if (end_func != NULL) curGp = SetGp(sceSdEndGp);
		sceSifCallRpc (&sceSd_gCd, command, isBlock, 
					   (void *)(sbuff [1]), (int)(sizeof (sceSdBatch) * (sbuff [2] + 1)),
					   (void *)r, s,
					   end_func, (void *)sbuff);
		if (end_func != NULL) (void)SetGp(curGp);
		if (sbuff [3] != (u_int)NULL) {
			ret = (int)(*((u_int *)sbuff [3]));	/* �ŏ��� 4 �o�C�g�͕Ԃ�l */
		}
	} else if (command >= rSdUserCommandMin &&
			   command <= rSdUserCommandMax) {
		if (end_func != NULL) curGp = SetGp(sceSdEndGp);
		sceSifCallRpc (&sceSd_gCd, command, isBlock,
					   (void *)sbuff [1], (int)sbuff [2],
					   (void *)(&sbuff [0]), 16,
					   end_func, (void *)(&sbuff [0]));
		if (end_func != NULL) (void)SetGp(curGp);
		ret = sbuff [0];
	} else {
		if (end_func != NULL) curGp = SetGp(sceSdEndGp);
		sceSifCallRpc (&sceSd_gCd, command, isBlock,
					   (void *)(&sbuff [0]),
					   DATA_SIZE_B, (void *)(&sbuff [0]), 16,
					   end_func, (void *)(&sbuff [0]));
		if (end_func != NULL) (void)SetGp(curGp);
		ret = sbuff [0];
	}

	PRINTF (("sceSifCallRpc cmplete \n"));
	va_end (ap);

	switch (command) {
#ifdef SCE_OBSOLETE
	case rSdSetTransCallback:
	case rSdSetIRQCallback:
#endif
	case rSdSetTransIntrHandler:
	case rSdSetSpu2IntrHandler:
		ret = intr_ret;
		break;
	}

	return ret;
}
