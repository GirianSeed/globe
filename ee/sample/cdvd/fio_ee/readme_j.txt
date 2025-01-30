[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

CD/DVD-ROM Drive コマンド関数の標準入出力での呼び出しサンプル

<サンプルの解説>
	CD/DVD-ROMのコマンド関数のテストプログラムです。

	１・CD/DVD-ROM関係のイニシャライズ
	２・コマンド関数の呼び出し
	３・コマンド関数の結果表示
	４・メディア交換処理

	を順に繰り返し行います。

<ファイル>
	main.c

<起動方法>
	% dsreset 0 0
	% make		：コンパイル

	CD/DVD-ROM Driveに"PlayStation"のCDをセットして
	dsedb を起動します。
	% dsedb
	> run main.elf
	を実行します。

	メディア交換を促すメッセージの表示が出ましたらメディアの交換を
	行ってください。

<コントローラの操作方法>
	なし

<備考>
	ハードディスクが接続されていない DTL-T10000 で実行しますと、以下の
	モジュールのロードに失敗しますが、サンプルの実行には影響ありません。

		host0:/usr/local/sce/iop/modules/atad.irx
		host0:/usr/local/sce/iop/modules/hdd.irx

