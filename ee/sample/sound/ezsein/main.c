/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 * Emotion Engine Library Sample Program
 *
 * Copyright (C) 2003 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * ezsein - main.c
 *     main routine
 */

#include <eekernel.h>
#include <string.h>		/* for strcpy() */
#include <sifdev.h>
#include <libgraph.h>		/* for sceGsSyncV() */
#include <libpad.h>
#include <libsdr.h>
#include <csl.h>
#include <cslse.h>
#include <modhsyn.h>
#include <libsein.h>
#include "ezsein.h"

/* Sound Data */
#define  HD_FILENAME	"host0:/usr/local/sce/data/sound/wave/eff.hd"
#define  BD_FILENAME	"host0:/usr/local/sce/data/sound/wave/eff.bd"

/* 先頭アドレス (SPU2 ローカルメモリ内) */
#define SPU_TOP_ADDR  0x5010

/* CSL 関連変数 */
static sceCslCtx     seinCtx;		/* CSL context */
static sceCslBuffGrp seinBfGrp[2];	/* CSL buffer group */
static sceCslBuffCtx seinBfCtx[1];	/* CSL buffer context */

/* IOP へ転送される変数 */
static int seinBf[SEIN_TRANS_SIZE/4]	__attribute__((aligned (64)));
static char gFilename[64]		__attribute__((aligned (64)));
static EzSEInBank gBank			__attribute__((aligned (64)));

/* 外部関数 (rpc.c) */
extern int ezSeInit(void);
extern int ezSe(int command,  int status);
extern int ezTransToIOP(void *iopAddr, void *eeAddr, int size);

/* コントローラ作業用バッファ */
u_long128 pad_dma_buf[scePadDmaBufferMax] __attribute__((aligned (64)));

#define _MOD_ROOT	"host0:/usr/local/sce/iop/modules/"
#define _MOD_SIO2MAN	"sio2man.irx"
#define _MOD_PADMAN	"padman.irx"
#define _MOD_LIBSD	"libsd.irx"
#define _MOD_SDRDRV	"sdrdrv.irx"
#define _MOD_MODHSYN	"modhsyn.irx"

#define LOAD_MODULE(mod) \
while (sceSifLoadModule(_MOD_ROOT mod, 0, NULL) < 0) { \
    scePrintf("  loading %s failed\n", mod); \
}

#define _IOP_SAMPLE_ROOT	"host0:../../../../iop/sample/sound/ezsein/"
#define _MOD_EZSEIN		"ezsein.irx"

#define LOAD_SAMPLE(mod) \
while (sceSifLoadModule(_IOP_SAMPLE_ROOT mod, 0, NULL) < 0) { \
    scePrintf("  loading %s failed\n", mod); \
}

/* ---------------------------------------------------------------- */
/* IOP モジュール読み込み                                          */
/* ---------------------------------------------------------------- */
void load_modules(void)
{
    /* SIF RPC の初期化 */
    sceSifInitRpc(0);
    sceSifInitIopHeap();

    /* モジュールのロード */
    LOAD_MODULE(_MOD_SIO2MAN);		/* SIO */
    LOAD_MODULE(_MOD_PADMAN);		/* contoller */
    LOAD_MODULE(_MOD_LIBSD);		/* sound device */
    LOAD_MODULE(_MOD_SDRDRV);		/* sound device control driver */
    LOAD_MODULE(_MOD_MODHSYN);		/* Hardware synthesizer */

    LOAD_SAMPLE(_MOD_EZSEIN);		/* EzSEIn driver */

    return;
}

/* ---------------------------------------------------------------- */
/* SPU2 設定                                                       */
/* ---------------------------------------------------------------- */
void set_spu2(void)
{
    int core;
    sceSdEffectAttr r_attr;

    /* initialize SPU */
    sceSdRemoteInit();
    sceSdRemote(1, rSdInit , SD_INIT_COLD);

    for (core = 0; core < 2; core ++) {
	/* effect */
	sceSdRemote(1, rSdSetAddr, core | SD_A_EEA, 0x1fffff - 0x20000 * core);
	/* set reverb attribute */
	r_attr.depth_L  = 0;
	r_attr.depth_R  = 0;
	r_attr.mode = SD_REV_MODE_HALL | SD_REV_MODE_CLEAR_WA;
	sceSdRemote(1, rSdSetEffectAttr, core, &r_attr);
	/* reverb on */
	sceSdRemote(1, rSdSetCoreAttr, core|SD_C_EFFECT_ENABLE, 1);
	sceSdRemote(1, rSdSetParam, core|SD_P_EVOLL , 0x3fff);
	sceSdRemote(1, rSdSetParam, core|SD_P_EVOLR , 0x3fff);
	
	/* master volume */
	sceSdRemote(1, rSdSetParam, core|SD_P_MVOLL , 0x3fff);
	sceSdRemote(1, rSdSetParam, core|SD_P_MVOLR , 0x3fff);
    }

    return;
}

/* ---------------------------------------------------------------- */
/* 効果音設定 (CSL)                                                */
/* ---------------------------------------------------------------- */
int init_se(void)
{
    unsigned int addr;
    int ret, hd_len;
    int bd_size;

    seinCtx.extmod = NULL;
    seinCtx.callBack = NULL;
    seinCtx.conf = NULL;
    seinCtx.buffGrpNum = 2;
    seinCtx.buffGrp = seinBfGrp;
      seinBfGrp[0].buffNum = 0;
      seinBfGrp[0].buffCtx = NULL;
      seinBfGrp[1].buffNum = 1;
      seinBfGrp[1].buffCtx = seinBfCtx;
        seinBfCtx[0].sema = 0;
        seinBfCtx[0].buff = seinBf;
    ((sceCslSeStream*)seinBf)->buffsize = SEIN_BUF_SIZE;
    ((sceCslSeStream*)seinBf)->validsize = 0;
    ((SEInBuf *)seinBf)->flag = 1; /* always `1' as the transfer flag */

    if (sceSEIn_Init(&seinCtx) != sceSEInNoError) {
	scePrintf("sceSEIn_Init Error\n");
	return (-1);
    }

    /* Program チャンクを持つ HD データ */
    gBank.type = sceHSynTypeHSyn;		/* set BANK attribute */

    /* HD データのサイズを取得 */
    strcpy(gFilename, HD_FILENAME);
    hd_len = ezSe(EZSEIN_GETFILELENGTH, (int)gFilename) ;

    /* IOP 内でのデータ領域の確保 */
    PRINTF(("allocate IOP heap memory - "));
    addr = (unsigned int)(int *)sceSifAllocSysMemory(0, hd_len, NULL);
    if (addr == (unsigned int)NULL) {
	scePrintf("\nCan't alloc heap \n");
	return (-1);
    }
    PRINTF(("alloced 0x%x  ", addr));
    ret = sceSifLoadIopHeap(HD_FILENAME, (void *)addr);
    if (ret < 0) {
	scePrintf("\nCan't load %s to iop heap %d\n", HD_FILENAME, ret);
	return (-1);
    }
    PRINTF(("- data loaded 0x%x \n", addr));
    gBank.hdAddr = addr;			/* set BANK attribute */

    gBank.spuAddr = SPU_TOP_ADDR;		/* set BANK attribute */

    strcpy(gBank.bdName,  BD_FILENAME);		/* set BANK attribute */
    scePrintf("BD name = %s\n", gBank.bdName);
    FlushCache(WRITEBACK_DCACHE); 

    /* BD データを読みこみ・セット */
    bd_size = gBank.bdSize =
	ezSe(EZSEIN_TRANSBDPACKET, (int)(&gBank)); /* set BANK attribute */

    /* HD ファイルをセット */
    ezSe(EZSEIN_SETHD, (int)(&gBank));

    /* プライオリティと最大ノート数をセット */
    ezSe(EZSEIN_SETATTR, 0x0840); // pri 0x40, max 0x08
    /* ボリュームをセット */
    ezSe(EZSEIN_SETVOL, sceHSyn_Volume_0db);

    return bd_size;
}

/* ---------------------------------------------------------------- */
/* メインループ                                                    */
/* ---------------------------------------------------------------- */
int mainloop(int iopBuffAddr)
{
#define _PAD_DATA (0xffff ^ ((rdata[2] << 8) | rdata[3]))
    int curStatus;
    u_int paddata, oldpad;
    u_char rdata[32];

    scePadInit(0);
    scePadPortOpen(0, 0, pad_dma_buf);

    oldpad = paddata = 0;
    while(1) {
	oldpad = paddata;
	if (scePadRead(0, 0, rdata) > 0) paddata = _PAD_DATA;
	else                              paddata = 0;

	if (paddata != oldpad) {
	    if (paddata & SCE_PADLright){
		//                            port, id,      bank, prog, note, vel,  pan
		sceSEIn_MakeNoteOn(&seinCtx, 0, 0x11223344, 0,    0,    48,   0x60, 127);
	    }
	    else if (paddata & SCE_PADLleft){
		//                            port, id,      bank, prog, note, vel,  pan
		sceSEIn_MakeNoteOn(&seinCtx, 0, 0x11223345, 0,    0,    48,   0x60, 0);
	    }
	    else if (paddata & SCE_PADLdown){
		//                            port, id,      bank, prog, note, vel,  pan
		sceSEIn_MakeNoteOn(&seinCtx, 0, 0x11223346, 0,    0,    72,   0x60, 64);
	    }
	    else if (paddata & SCE_PADLup){
		//                            port, id,      bank, prog, note, vel,  pan
		sceSEIn_MakeNoteOn(&seinCtx, 0, 0x11223347, 0,    0,    96,   0x7f, 64);
	    }

	    // QUIT
	    else if (paddata & SCE_PADselect){  
		ezSe(EZSEIN_QUIT, 0);
		return 0;
	    }
	}

	if (((sceCslSeStream*)seinBf)->validsize != 0) {
	    ezTransToIOP((void*)iopBuffAddr, (void*)seinBf, SEIN_TRANS_SIZE);
	    ((sceCslSeStream*)seinBf)->validsize = 0;
	}
	
	sceGsSyncV(0);
    }

    return 0;
}

/* ---------------------------------------------------------------- */
/* 開始エントリー                                                  */
/* ---------------------------------------------------------------- */
int main(int argc, char *argv[])
{
    int iopBuffAddr;
    int hd_addr, hd2addr, bd2size;

    /* モジュールのロード */
    load_modules();

    /* EzSEIn SIF RPC環境の初期化 */
    scePrintf("EzSEIn initialize...\n");
    ezSeInit();			/* rpc.c */

    /* SPU2の初期化 */
    set_spu2();

    /* EzSEIn ドライバの初期化
     *J   引数はbdの分割ロード用バッファサイズ
     *J   返値は効果音バッファ(seinBf)の転送先アドレス */
    iopBuffAddr = ezSe(EZSEIN_INIT, 16*1024); /* rpc.c */

    /* 効果音のセットアップ */
    (void)init_se();

    /* 演奏のコントロール */
    mainloop(iopBuffAddr);

    scePrintf("EzSEIn finished...\n");
    return 0;
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADD STUFF AFTER THIS */
