[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

HDDのファイル操作サンプル

<サンプルの解説>
	HDDのファイルをアクセスするサンプルプログラムです。

	1. ファイル main.c の内容をコピーしたファイル copy.c を作成
	2. 4バイト毎のインクリメンタルデータを作成し、それを incdata と
	   いうファイルに書き込む。再度、incdata を読み出して正しいか検証

<ファイル>
	main.c

<起動方法>
	% make		：コンパイル
	% make run	：実行

	コンパイル後、以下の方法でも実行可能
	% dsedb
	> run main.elf

	sample end.
	と表示されれば正常終了。

<コントローラの操作方法>
	なし

<備考>
	なし
