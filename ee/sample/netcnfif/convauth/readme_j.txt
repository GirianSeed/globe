[SCE CONFIDENTIAL DOCUMENT]
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                     All Rights Reserved


共通ネットワーク設定インタフェースライブラリサンプルプログラム(convauth)


< サンプルの解説 >

	このプログラムは、あなたのネットワーク設定ファイル使用規定で
	使用を規定されている sceNetcnfifConvAuthname() 関数の使用方法を
	示したサンプルプログラムです。


< ファイル >

	Makefile		： メイクファイル
	convauth.c		： メインプログラム


< 使用方法 >

	1. make します。

	your-host > make


	2. 以下のように起動して下さい。

	dsedb S> reset;run ./convauth.elf aaa


	3. 以下のように入力した文字列がそのまま出力されれば OK です。

	EE DECI2 Manager version 0.06 Feb 19 2002 10:08:18
	  CPUID=2e14, BoardID=4126, ROMGEN=2002-0319, 32M

	Loading program (address=0x00100000 size=0x00008da4) ...
	Loading program (address=0x00108e00 size=0x00002550) ...
	Loading 593 symbols ...
	Entry address = 0x00100008
	GP value      = 0x00113370

	"aaa" -> "aaa"

	*** End of Program
	*** retval=0x00000000
