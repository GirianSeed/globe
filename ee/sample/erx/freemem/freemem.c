/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                      ERX Library Sample
 *
 *                     - <liberx freemem> -
 *
 *                         Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          <freemem.c>
 *                     <liberx free memory size>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           May,15,2003     kono        first version
 */

#include <sys/types.h>
#include <stdio.h>
#include <liberx.h>

// モジュール情報
const SceErxModuleInfo Module = {
	"freemem",
	0x0201
};


int start(int reason, int argc, const char *const argv[]);

int start(int reason, int argc, const char *const argv[])
{
	(void)argc;
	(void)argv;

	switch (reason) {
		case SCE_ERX_REASON_START:
			// モジュール起動
			{
				u_int uiTotalFree, uiMaxFree;

				sceErxQueryFreeMemSize(&uiTotalFree, &uiMaxFree);
				printf("sceErxQueryFreeMemSize: uiTotalFree=%d, uiMaxFree=%d\n", uiTotalFree, uiMaxFree);

				sceErxQueryFreeMemSticky(&uiTotalFree, &uiMaxFree);
				printf("sceErxQueryFreeMemSticky: uiTotalFree=%d, uiMaxFree=%d\n", uiTotalFree, uiMaxFree);
			}
			return (SCE_ERX_NO_RESIDENT_END);

		case SCE_ERX_REASON_STOP:
			// モジュール停止
			return (SCE_ERX_NO_RESIDENT_END);
	}

	// 不明な理由コードのとき
	return (SCE_ERX_NO_RESIDENT_END);
}

