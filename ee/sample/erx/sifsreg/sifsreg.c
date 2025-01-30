/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                      ERX Library Sample
 *
 *                     - <liberx sifsreg> -
 *
 *                         Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          <sifsreg.c>
 *                   <debug program injection>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           May,15,2003     kono        first version
 */

#include <sys/types.h>
#include <stdio.h>
#include <sifcmd.h>
#include <liberx.h>


// モジュール情報
const SceErxModuleInfo Module = {
	"SIF_Sreg_Dump",
	0x0201
};

static u_int sreg[32];

int start(int reason, int argc, const char *const argv[]);

static void TakeSregSnapshot(void);
static void DumpAllSregEE(void);

int start(int reason, int argc, const char *const argv[])
{
	(void)argc;
	(void)argv;

	switch (reason) {
		case SCE_ERX_REASON_START:
			// モジュール起動
			TakeSregSnapshot();
			DumpAllSregEE();
			return (SCE_ERX_NO_RESIDENT_END);

		case SCE_ERX_REASON_STOP:
			// モジュール停止
			return (SCE_ERX_NO_RESIDENT_END);
	}

	// 不明な理由コードのとき
	return (SCE_ERX_NO_RESIDENT_END);
}



// SIFソフトウェアレジスタのスナップショットを作る
static void TakeSregSnapshot(void)
{
	int i;
	int intr;

	intr = DI();
	for (i=0; i<32; i++) {
		sreg[i] = sceSifGetSreg(i);
	}
	if (intr) EI();
	return;
}


// SIFソフトウェアレジスタの列挙
static void DumpAllSregEE(void)
{
	int i;

	printf("SIF software Register(EE)====\n");
	for (i=0; i<8; i++) {
		printf("%2d: 0x%08X[SystemUse]\n", i, sreg[i]);
	}
	for (i=8; i<32; i++) {
		printf("%2d: 0x%08X\n", i, sreg[i]);
	}
	return;
}


