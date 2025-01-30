[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5
		Copyright (C) 2001 Sony Computer Entertainment Inc.
						All Rights Reserved


	共通ネットワーク設定用アイコンファイル


< 内容 >

	- 日本語版用アイコンファイル

	japanese/icon.sys
	japanese/SYS_NET.ICO

	- 英語版用アイコンファイル

	english/icon.sys
	english/SYS_NET.ICO


< 注意事項 >

	※ 共通ネットワーク設定ライブラリ(netcnf)を使用する時にアイコン
	   ファイルを以下のように指定してお使い下さい。

	   - dsidb コンソールより起動する場合

	   dsidb R> mstart netcnf.irx icon=SYS_NET.ICO iconsys=icon.sys

	   - EE 側プログラムよりロードする場合

	   {
	       static char netcnf_arg[] = "icon=host0:SYS_NET.ICO\0iconsys=host0:icon.sys";

	       while(sceSifLoadModule("host0:netcnf.irx", sizeof(netcnf_arg), netcnf_arg) < 0)
	           {
	               printf("Can't load module netcnf.irx\n");
	           }
	   }

	※ 英語圏地域では必ず英語版用アイコンファイルをお使い下さい。
