[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2000 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

UNIX上でTCPを用いて、
クライアント-サーバ間でパケットを送受信するサンプル
(クライアントプログラム)


<サンプルの解説>
        このプログラムはeeからlibnetを用いてサーバとTCPコネクションを確立
	し、指定サイズのデータを送受信するサンプルプログラムのUNIX版です。
	linux上で動作を確認しております。
	データサイズと同時起動コネクション数を引数で指定できます。
	対応するプログラムがあらかじめサーバ側で起動されている必要が
	あります。
	本プログラムは
	/usr/local/sce/ee/sample/inet/load_test/clientと同一の動作をします。

<ファイル>
        main.c        : メインプログラム


<起動方法>

        $ make                                   : コンパイル
        $ ./load_test <saddr> <th_num> <pktsize> : load_testの実行。
        <saddr>: サーバアドレス
	<th_num>: 同時に確立するコネクション数
	<pktsize>: 送受信するデータサイズ

<備考>
        inet API と BSD socket API の比較ができます。
