/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 * Emotion Engine Library
 *
 * Copyright (C) 1998-2003 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * libsdr - sdr_cb.c
 *     interrupt callback functions
 */

#include <eekernel.h>
#include <sif.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <libsdr.h>
#include <stdarg.h>
#include "sdr_i.h"

static char stack [256 * 16] __attribute__((aligned(64)));
static SdrEECBData gEECBData __attribute__((aligned(64)));

static void _sdrCBThread (void *data);
static unsigned int _sdrCB (unsigned int fnd, void *data, int size);

#ifdef SCE_OBSOLETE
int (*_sce_sdr_gDMA0CB)(void) = (int (*)(void)) NULL;
int (*_sce_sdr_gDMA1CB)(void) = (int (*)(void)) NULL;
int (*_sce_sdr_gIRQCB)(void)  = (int (*)(void)) NULL;
void *_sce_sdr_gDMA0Gp = (void *) NULL;
void *_sce_sdr_gDMA1Gp = (void *) NULL;
void *_sce_sdr_gIRQGp  = (void *) NULL;
#endif

sceSdTransIntrHandler _sce_sdr_transIntr0Hdr = (sceSdTransIntrHandler) NULL;
sceSdTransIntrHandler _sce_sdr_transIntr1Hdr = (sceSdTransIntrHandler) NULL;
sceSdSpu2IntrHandler  _sce_sdr_spu2IntrHdr   = (sceSdSpu2IntrHandler)  NULL;

void *_sce_sdr_transIntr0Arg = (void *) NULL;
void *_sce_sdr_transIntr1Arg = (void *) NULL;
void *_sce_sdr_spu2IntrArg   = (void *) NULL;

void *_sce_sdr_transIntr0Gp = (void *) NULL;
void *_sce_sdr_transIntr1Gp = (void *) NULL;
void *_sce_sdr_spu2IntrGp   = (void *) NULL;

static sceSifQueueData _sce_sdr_qd;
static sceSifServeData _sce_sdr_sd;

static int _sce_sdr_cb_thid = -1;

/* ------------------------------------------------------------------------
   IOP側では、割り込みが入った時にEE側にRPCして_sdrCB() を起こそうとする。
   そのためのThreadをここで作成する。
   ------------------------------------------------------------------------*/
int
sceSdRemoteCallbackInit (int priority)
{
	int i;
	struct ThreadParam tp;

	if (_sce_sdr_cb_thid >= 0) return (-1);

#ifdef SCE_OBSOLETE
	_sce_sdr_gDMA0CB = (int (*)(void)) NULL;
	_sce_sdr_gDMA1CB = (int (*)(void)) NULL;
	_sce_sdr_gIRQCB  = (int (*)(void)) NULL;
	_sce_sdr_gDMA0Gp = (void *) NULL;
	_sce_sdr_gDMA1Gp = (void *) NULL;
	_sce_sdr_gIRQGp  = (void *) NULL;
#endif
	_sce_sdr_transIntr0Hdr = (sceSdTransIntrHandler) NULL;
	_sce_sdr_transIntr1Hdr = (sceSdTransIntrHandler) NULL;
	_sce_sdr_spu2IntrHdr   = (sceSdSpu2IntrHandler)  NULL;

	_sce_sdr_transIntr0Arg = (void *) NULL;
	_sce_sdr_transIntr1Arg = (void *) NULL;
	_sce_sdr_spu2IntrArg   = (void *) NULL;

	_sce_sdr_transIntr0Gp = (void *) NULL;
	_sce_sdr_transIntr1Gp = (void *) NULL;
	_sce_sdr_spu2IntrGp   = (void *) NULL;

	sceSdRemote (1, 0xe620); //IOP側でBindする

	tp.entry = _sdrCBThread;
	tp.stack = stack;
	tp.stackSize = sizeof(stack);
	tp.initPriority = priority;

	tp.option = (u_int)"SceSdrCallbackThread";

	tp.gpReg = &_gp;
	_sce_sdr_cb_thid = CreateThread (&tp);
	if (_sce_sdr_cb_thid < 0) return (-1);

	tp.option = _sce_sdr_cb_thid;
	i = StartThread ((int)tp.option, NULL);
	if (i < 0){
		scePrintf("Can't start thread for streaming.\n");
		return -1;
	}

	return tp.option;
}

int
sceSdRemoteCallbackQuit (void)
{
	int ret, result;
	void *ret2;

	if (_sce_sdr_cb_thid < 0) return (-1);

#ifdef SCE_OBSOLETE
	sceSdRemote (1, rSdSetIRQCallback,  NULL);
	sceSdRemote (1, rSdSetTransCallback, 0, NULL);
	sceSdRemote (1, rSdSetTransCallback, 1, NULL);
#endif
	sceSdRemote (1, rSdSetSpu2IntrHandler,  NULL, NULL);
	sceSdRemote (1, rSdSetTransIntrHandler, 0, NULL, NULL);
	sceSdRemote (1, rSdSetTransIntrHandler, 1, NULL, NULL);

	// Destroy callback thread on IOP side:
	sceSdRemote (1, 0xe630);

#ifdef SCE_OBSOLETE
	_sce_sdr_gDMA0CB = (int (*)(void)) NULL;
	_sce_sdr_gDMA1CB = (int (*)(void)) NULL;
	_sce_sdr_gIRQCB  = (int (*)(void)) NULL;
	_sce_sdr_gDMA0Gp = (void *) NULL;
	_sce_sdr_gDMA1Gp = (void *) NULL;
	_sce_sdr_gIRQGp  = (void *) NULL;
#endif
	_sce_sdr_transIntr0Hdr = (sceSdTransIntrHandler) NULL;
	_sce_sdr_transIntr1Hdr = (sceSdTransIntrHandler) NULL;
	_sce_sdr_spu2IntrHdr   = (sceSdSpu2IntrHandler)  NULL;

	_sce_sdr_transIntr0Arg = (void *) NULL;
	_sce_sdr_transIntr1Arg = (void *) NULL;
	_sce_sdr_spu2IntrArg   = (void *) NULL;

	_sce_sdr_transIntr0Gp = (void *) NULL;
	_sce_sdr_transIntr1Gp = (void *) NULL;
	_sce_sdr_spu2IntrGp   = (void *) NULL;

	result = 0;
	ret2 = sceSifRemoveRpc(&_sce_sdr_sd, &_sce_sdr_qd);
	if (ret2 == NULL) result = -1; 
	ret2 = sceSifRemoveRpcQueue(&_sce_sdr_qd);
	if (ret2 == NULL) result = -1; 

	/* `_sce_sdr_cb_thid' is already checked above */
	ret = TerminateThread(_sce_sdr_cb_thid);
	if (ret < 0) result = -1;
	ret = DeleteThread(_sce_sdr_cb_thid);
	if (ret < 0) result = -1;

	_sce_sdr_cb_thid = -1;
	return (result);
}

/* ------------------------------------------------------------------------
   libsdrのコールバック用スレッド。
   起動されるとコマンドの登録を行い、以後はIOPからリクエストがあるまでウエ
   イトする。
   ------------------------------------------------------------------------*/
static void
_sdrCBThread (void *data)
{
	PRINTF (("_sdrCBThread \n"));

	sceSifInitRpc (0);
	sceSifSetRpcQueue (&_sce_sdr_qd,  GetThreadId ());

	sceSifRegisterRpc (&_sce_sdr_sd, sce_SDRST_CB, (sceSifRpcFunc)_sdrCB,
					   (void *)&gEECBData, NULL, NULL, &_sce_sdr_qd);
	sceSifRpcLoop(&_sce_sdr_qd);

	return;
}

// please sync with `sdrdrv'
#ifdef SCE_OBSOLETE
#define SDR_CB_DMA0    (1 << 0)
#define SDR_CB_DMA1    (1 << 1)
#define SDR_CB_IRQ     (1 << 2)
#endif
#define SDR_CB_DMA0INT (1 << 8)
#define SDR_CB_DMA1INT (1 << 9)
#define SDR_CB_IRQINT  (1 << 10)

/* ------------------------------------------------------------------------
   IOPからのRPCによって起こされる関数。
   *dataにはSdrEECBDataの値。
   ------------------------------------------------------------------------*/
static unsigned int
_sdrCB (unsigned int fnd, void *data, int size)
{
	void *curGp;

	PRINTF(("**** sdrCB %d (%d)\n",
			*((int *)data), ((SdrEECBData *)data)->mode));

#ifdef SCE_OBSOLETE
	if ((*((int *)data)) & SDR_CB_DMA0){
		if (_sce_sdr_gDMA0CB){
			curGp = SetGp(_sce_sdr_gDMA0Gp);
			(*_sce_sdr_gDMA0CB)();
			(void)SetGp(curGp);
		}
	}
	if ((*((int *)data)) & SDR_CB_DMA1){
		if (_sce_sdr_gDMA1CB){
			curGp = SetGp(_sce_sdr_gDMA1Gp);
			(*_sce_sdr_gDMA1CB)();
			(void)SetGp(curGp);
		}
	}
	if ((*((int *)data)) & SDR_CB_IRQ){
		if (_sce_sdr_gIRQCB){
			curGp = SetGp(_sce_sdr_gIRQGp);
			(*_sce_sdr_gIRQCB)();
			(void)SetGp(curGp);
		}
	}
#endif
	if ((*((int *)data)) & SDR_CB_DMA0INT){
		if (_sce_sdr_transIntr0Hdr){
			curGp = SetGp(_sce_sdr_transIntr0Gp);
			(*_sce_sdr_transIntr0Hdr)(0, _sce_sdr_transIntr0Arg);
			(void)SetGp(curGp);
		}
	}
	if ((*((int *)data)) & SDR_CB_DMA1INT){
		if (_sce_sdr_transIntr1Hdr){
			curGp = SetGp(_sce_sdr_transIntr1Gp);
			(*_sce_sdr_transIntr1Hdr)(1, _sce_sdr_transIntr1Arg);
			(void)SetGp(curGp);
		}
	}
	if ((*((int *)data)) & SDR_CB_IRQINT){
		if (_sce_sdr_spu2IntrHdr){
			curGp = SetGp(_sce_sdr_spu2IntrGp);
			(*_sce_sdr_spu2IntrHdr) ((((SdrEECBData *) data)->voice_bit),
									 _sce_sdr_spu2IntrArg);
			(void)SetGp(curGp);
		}
	}

	return 0;
}

