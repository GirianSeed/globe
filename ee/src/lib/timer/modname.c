/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                       Timer Library
 *                        Version 1.0
 *                         Shift-JIS
 *
 *      Copyright (C) 2001-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         modname.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.0            2003/05/15      kono        First version
 */


#include <sys/types.h>
#include <string.h>
#include <eekernel.h>
#include <sceerrno.h>
#include <liberx.h>
#include <libtimer.h>

// プロトタイプ宣言
int start(int reason, int argc, const char *const argv[]);

// モジュール情報
const SceErxModuleInfo Module = {
	"libtimer",
	0x0203
};

int start(int reason, int argc, const char *const argv[])
{
	(void)argc;
	(void)argv;

	switch (reason) {
		case SCE_ERX_REASON_START:
			// モジュール起動(アンロード対応)
			{
				int i;
				int res;
				u_int mode = SCE_TIMER_PRESCALE256;

				// オプションを評価
				for (i=1; i<argc; i++) {
					if (strcmp(argv[i], "-prescale256")==0) {
						mode = SCE_TIMER_PRESCALE256;		// '-prescale256'オプション指定
					} else if (strcmp(argv[i], "-prescale16")==0) {
						mode = SCE_TIMER_PRESCALE16;		// '-prescale16'オプション指定
					} else if (strcmp(argv[i], "-prescale1")==0) {
						mode = SCE_TIMER_PRESCALE1;			// '-prescale1'オプション指定
					} else {
						scePrintf("Unknown option: %s\n", argv[i]);
					}
				}

				// libtimerライブラリを初期化
				res = sceTimerInit(mode);
				if (res<0) {
					return (SCE_ERX_NO_RESIDENT_END);
				}
			}
			return (SCE_ERX_REMOVABLE_RESIDENT_END);

		case SCE_ERX_REASON_STOP:
			// モジュール停止
			{
				int res;

				// libtimerライブラリの終了処理
				res = sceTimerEnd();
				if (SCE_ERROR_ERRNO(res)==SCE_EBUSY) {
					// まだ使用中のカウンタがあった場合
					return (SCE_ERX_REMOVABLE_RESIDENT_END);
				}
			}
			return (SCE_ERX_NO_RESIDENT_END);
	}

	// 不明な理由コードのとき
	return (SCE_ERX_NO_RESIDENT_END);
}

