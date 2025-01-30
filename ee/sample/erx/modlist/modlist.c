/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                      ERX Library Sample
 *
 *                     - <liberx modlist> -
 *
 *                         Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          <modlist.c>
 *                         <module list>
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
	"modlist",
	0x0201
};


int start(int reason, int argc, const char *const argv[]);
void EnumModules(void);
void DumpModuleStatus(int modid);

int start(int reason, int argc, const char *const argv[])
{
	(void)argc;
	(void)argv;

	switch (reason) {
		case SCE_ERX_REASON_START:
			// モジュール起動
			EnumModules();
			return (SCE_ERX_NO_RESIDENT_END);

		case SCE_ERX_REASON_STOP:
			// モジュール停止
			return (SCE_ERX_NO_RESIDENT_END);
	}

	// 不明な理由コードのとき
	return (SCE_ERX_NO_RESIDENT_END);
}



// モジュールの列挙
void EnumModules(void)
{
	int modids[1024];
	int cnt;
	int i;

	cnt = sceErxGetModuleIdList(modids, 1024, NULL);
	printf("sceErxGetModuleIdList: %d\n", cnt);

	for (i=0; i<cnt; i++) {
		DumpModuleStatus(modids[i]);
	}
	return;
}



// モジュールのステータス表示
void DumpModuleStatus(int modid)
{
	int res;
	SceErxModuleStatus modstat;

	// 指定されたモジュールIDの状態を取得
	res = sceErxReferModuleStatus(modid, &modstat);

	printf("-----\n");
	printf("id:            %d\n",   modstat.id);			// モジュールを識別するID番号
	printf("name:          %s\n",   modstat.name);			// モジュール名
	printf("version:       %04X\n", modstat.version);		// モジュールのバージョン番号
	printf("entry_addr:    %08X\n", modstat.entry_addr);	// モジュールの実行開始アドレス
	printf("gp_value:      %08X\n", modstat.gp_value);		// モジュールの GP レジスタ値
	printf("text_addr:     %08X\n", modstat.text_addr);		// モジュールのテキスト先頭アドレス
	printf("text_size:     %08X\n", modstat.text_size);		// モジュールのテキストセグメントサイズ
	printf("data_size:     %08X\n", modstat.data_size);		// モジュールのデータセグメントサイズ
	printf("bss_size:      %08X\n", modstat.bss_size);		// モジュールの未初期化データセグメントサイズ
	printf("erx_lib_addr:  %08X\n", modstat.erx_lib_addr);	// ERXライブラリエントリの開始アドレス
	printf("erx_lib_size:  %08X\n", modstat.erx_lib_size); 	// ERXライブラリエントリのサイズ
	printf("erx_stub_addr: %08X\n", modstat.erx_stub_addr);	// ERXライブラリスタブの開始アドレス
	printf("erx_stub_size: %08X\n", modstat.erx_stub_size);	// ERXライブラリスタブのサイズ
	return;
}
