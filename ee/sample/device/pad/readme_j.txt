[SCEI CONFIDENTIAL DOCUMENT]
DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
                  Copyright (C) 1999 by Sony Computer Entertainment Inc.
                                                     All Rights Reserved

コントローラ読み込みサンプル

<サンプルの解説>
	コントローラを読み込んで、そのビットパターンを表示するサンプル
	プログラムです

<ファイル>
	main.c

<起動方法>
	% make		：コンパイル
	% make run	：実行

	コンパイル後、以下の方法でも実行可能
	% dsedb
	> run main.elf

	コントローラのビットパターンが表示。
	cnt の次の数字が増加して、コントローラのボタンを押したときに
	ビットパターンが変化するかを確認。

<コントローラの操作方法>
	なし

<備考>
	ファイル main.c の /* #define PADS */ のコメントアウトを外すことに
	より、2コントローラのサンプルになります。

