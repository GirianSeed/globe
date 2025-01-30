/* SCE CONFIDENTIAL
   "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
*/
/*
 *     Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                   All Rights Reserved.
 *
 */

#include <eekernel.h>
#include <eetypes.h>
#include <eestruct.h>
#include <eeregs.h>
#include <libdma.h>

#include <string.h>

#include "mfifo.h"

#ifndef UNUSED_ALWAYS
#	define UNUSED_ALWAYS(a)	(void)(a)
#endif

typedef enum{
	MFIFO_STAT_DORMANT	= 0,
	MFIFO_STAT_STOP		= 1,
	MFIFO_STAT_PAUSE	= 2,
	MFIFO_STAT_RUN		= 3,
}MFIFO_STAT;

typedef struct{
	MFIFO_STAT stat;
	int ch;
	void* base;
	size_t size;
	sceDmaChan volatile* dcc;
	sceDmaChan volatile* dc8;

	int dmacc_ae;
	int dmacc_sema;
	int dmacc_handler;

	int dmac14_sema;
	int dmac14_handler;
}MFIFOContext;

static MFIFOContext _mc = { MFIFO_STAT_DORMANT, 0, NULL, 0, NULL, NULL, -1, -1, -1, -1, -1 };


static int _dmacc_handler(int ch);
static int _dmac14_handler(int ch);
static void _set_d_ctrl_mfd(u_int mfd);
static void _update_stat(void);

static __inline__ void _set_d_ctrl_mfd(u_int mfd){
	DPUT_D_CTRL((DGET_D_CTRL() & ~D_CTRL_MFD_M) | (mfd << D_CTRL_MFD_O));
}

static __inline__ void _update_stat(void){
	if(MFIFO_STAT_RUN == _mc.stat){
		if(0 == _mc.dcc->chcr.STR){
			_mc.stat = MFIFO_STAT_PAUSE;
		}
	}
}

int _mfifo_init(int ch, void* buffer, size_t size){

	if((SCE_DMA_VIF1 != ch) && (SCE_DMA_GIF != ch)){
		return -1;
	}

	{
		struct SemaParam sp;
		memset(&sp, 0, sizeof(sp));
		sp.initCount = 1;
		sp.maxCount = 1;
		sp.option = 0;
		_mc.dmacc_sema = CreateSema(&sp);
		if(0 > _mc.dmacc_sema){
			_mfifo_close();
			return -1;
		}
	}

	{
		int dmach = (ch == SCE_DMA_VIF1) ? DMAC_VIF1 : DMAC_GIF;
		_mc.dmacc_handler = AddDmacHandler(dmach, _dmacc_handler, 0);
		if(0 > _mc.dmacc_handler){
			_mfifo_close();
			return -1;
		}
		_mc.dmacc_ae = EnableDmac(dmach);
	}

	{
		_mc.ch = ch;
		_mc.size = size;
		_mc.base = buffer;
		_mc.dcc = sceDmaGetChan(ch);
		_mc.dc8 = sceDmaGetChan(SCE_DMA_fromSPR);

		DPUT_D_RBOR((u_int)_mc.base);
		DPUT_D_RBSR(_mc.size - 0x10);
	}

	_mc.stat = MFIFO_STAT_STOP;

	return 0;
}

void _mfifo_close(void){
	_mfifo_stop();

	if(0 <= _mc.dmacc_sema){
		DeleteSema(_mc.dmacc_sema);
		_mc.dmacc_sema = -1;
	}

	if(0 <= _mc.dmacc_handler){
		int dmach = (_mc.ch == SCE_DMA_VIF1) ? DMAC_VIF1 : DMAC_GIF;
		RemoveDmacHandler(dmach, _mc.dmacc_handler);
		_mc.dmacc_handler = -1;

		if(0 != _mc.dmacc_ae){
			DisableDmac(dmach);
		}
	}

	_mc.stat = MFIFO_STAT_DORMANT;
}

void _mfifo_start(int tte, int tie){

	if(MFIFO_STAT_STOP != _mc.stat){
		return;
	}

	sceDmaSync((sceDmaChan*)(u_int)_mc.dcc, 0, 0);

	_set_d_ctrl_mfd(1 + _mc.ch);

	_mc.dc8->madr = _mc.base;

	PollSema(_mc.dmacc_sema);

	{
		tD_CHCR chcr = { 1, 0, 1, 0, 0, 0, 1, 0, 0 };

		chcr.TTE = tte;
		chcr.TIE = tie;
		_mc.dcc->tadr = (sceDmaTag*)_mc.base;
		_mc.dcc->qwc = 0;
		_mc.dcc->chcr = chcr;
	}

	_mc.stat = MFIFO_STAT_RUN;
}

void _mfifo_stop(void){
	if((MFIFO_STAT_RUN != _mc.stat) && (MFIFO_STAT_PAUSE != _mc.stat)){
		return;
	}

	_mfifo_sync(0);

	_mc.dcc->chcr.STR = 0;

	_set_d_ctrl_mfd(0);

	_mc.stat = MFIFO_STAT_STOP;
}

void _mfifo_pause(void){
	if(MFIFO_STAT_RUN != _mc.stat){
		return;
	}

	_mc.dcc->chcr.STR = 0;

	_mc.stat = MFIFO_STAT_PAUSE;
}

void _mfifo_continue(void){

	_update_stat();

	if(MFIFO_STAT_PAUSE != _mc.stat){
		return;
	}

	PollSema(_mc.dmacc_sema);

	_mc.dcc->chcr.STR = 1;

	_mc.stat = MFIFO_STAT_RUN;
}

int _mfifo_sync(int async){
	u_int madr;

	if(MFIFO_STAT_DORMANT == _mc.stat){
		return 0;
	}

	_update_stat();

	if(MFIFO_STAT_PAUSE == _mc.stat){
		async = 1;		// ポーズ中はブロックしない
	}

	madr = (u_int)_mc.dcc->madr;

	if(0 != async){
		u_int tadr;

		if(0 != sceDmaSync((sceDmaChan*)(u_int)_mc.dc8, 1, 0)){
			return 1;
		}

		while(0 == (tadr = (u_int)_mc.dcc->tadr)){
			// nop
		}

		if((u_int)_mc.dcc->tadr != madr){
			return 1;
		}

	}else{
		sceDmaSync((sceDmaChan*)(u_int)_mc.dc8, 0, 0);

		while((u_int)_mc.dcc->tadr != madr){
			// タイムアウト処理いれてない
		}
	}

	return 0;
}

int _mfifo_sync2(int async){

	if(MFIFO_STAT_STOP == _mc.stat){
		return 0;
	}

	_update_stat();

	if(MFIFO_STAT_PAUSE == _mc.stat){
		async = 1;		// ポーズ中はブロックしない
	}

	if(0 != async){
		if(0 != sceDmaSync((sceDmaChan*)(u_int)_mc.dc8, 1, 0)){
			return 1;
		}

		if(0 > PollSema(_mc.dmacc_sema)){
			return 1;
		}
		SignalSema(_mc.dmacc_sema);

	}else{
		sceDmaSync((sceDmaChan*)(u_int)_mc.dc8, 0, 0);

		WaitSema(_mc.dmacc_sema);

		SignalSema(_mc.dmacc_sema);
	}

	return 0;
}

u_int _mfifo_getspace(void){
	u_int tadr;
	u_int madr;

	while(0 == (tadr = (u_int)_mc.dcc->tadr)){
		// nop
	}

	madr = (u_int)_mc.dc8->madr;

	return ((tadr - madr - 1) & (_mc.size - 0x10)) >> 4;
}

int _mfifo_kick(u_long128 const* spr_packet, u_int qwc, int mode){

	sceDmaSync((sceDmaChan*)(u_int)_mc.dc8, 0, 0);

	//printf("D_CTRL=%08x D_STAT=%08x D8_CHCR=%08x D8_MADR=%08x D8_SADR=%08x D8_QWC=%08xD1_CHCR=%08x D1_TADR=%08x\n", *D_CTRL, *D_STAT, *D8_CHCR, *D8_MADR, *D8_SADR, *D8_QWC, *D1_CHCR, *D1_TADR);

	if(0 != mode){
		if(qwc > _mfifo_getspace()){
			return 1;
		}
	}else{
		while(qwc > _mfifo_getspace()){
			// nop
		}
	}

	_mc.dc8->sadr = (void*)((u_int)spr_packet & 0x00003ff0U);
	_mc.dc8->qwc = qwc;
	{
		tD_CHCR const chcr = { 0, 0, 0, 0, 0, 0, 1, 0, 0 };
		_mc.dc8->chcr = chcr;
	}

	return 0;
}

int _mfifo_sync_dbg(void){

	int ae;

	if(MFIFO_STAT_RUN != _mc.stat){
		return _mfifo_sync(1);
	}

	_update_stat();

	if(MFIFO_STAT_RUN == _mc.stat){
		_mfifo_pause();

		if(0 == _mfifo_sync(1)){
			_mfifo_continue();
			return 0;
		}

	}else{
		return _mfifo_sync(1);
	}

	{
		struct SemaParam sp;
		memset(&sp, 0, sizeof(sp));
		sp.initCount = 0;
		sp.maxCount = 1;
		sp.option = 0;
		_mc.dmac14_sema = CreateSema(&sp);
		if(0 > _mc.dmac14_sema){
			return -1;
		}
	}

	{
		_mc.dmac14_handler = AddDmacHandler(14, _dmac14_handler, 0);
		if(0 > _mc.dmac14_handler){
			return -2;
		}
		ae = EnableDmac(14);
	}

	_mfifo_continue();

	WaitSema(_mc.dmac14_sema);

	{
		if(0 != ae){
			DisableDmac(14);
		}
		RemoveDmacHandler(14, _mc.dmac14_handler);
	}

	{
		DeleteSema(_mc.dmac14_sema);
	}

	return 0;
}

static int _dmac14_handler(int ch){
#if 0
	if(14 != ch){
		ExitHandler();
		return 0;
	}
#else
	UNUSED_ALWAYS(ch);
#endif

	iSignalSema(_mc.dmac14_sema);

	ExitHandler();	
	return 0;
}

static int _dmacc_handler(int ch){
#if 0
	int dmach = (_mc.ch == SCE_DMA_VIF1) ? DMAC_VIF1 : DMAC_GIF;
	if(dmach != ch){
		ExitHandler();
		return 0;
	}
#else
	UNUSED_ALWAYS(ch);
#endif

	if(0x7000U != (_mc.dcc->chcr.TAG & 0xf000U)){
		ExitHandler();
		return 0;
	}

	iPollSema(_mc.dmacc_sema);
	iSignalSema(_mc.dmacc_sema);

	ExitHandler();	
	return -1;
}

// [eof]
