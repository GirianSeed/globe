[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2000 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

UNIX上でTCPを用いて、
クライアント-サーバ間でパケットを送受信するサンプル
(サーバプログラム)


<サンプルの解説>
        このプログラムはTCPのpassive openを行ない、
	接続してきたクライアントとデータを送受信するサンプルプログラムの
	UNIX版です。linux上で動作確認をしています。
	/usr/local/sce/ee/sample/inet/load_test/daemonと同一の動作をします。

<ファイル>
        main.c        : メインプログラム


<起動方法>

        $ make		: コンパイル
        $ ./load_testd  : サーバプログラムの起動

サーバを起動したら、クライアントプログラムからコネクションを確立し
通信を開始します。
クライアントプログラムはee上、iop上で動作するプログラムが用意されています。
それぞれ、
/usr/local/sce/ee/sample/inet/load_test/client/readme_j.txt
/usr/local/sce/iop/sample/inet/load_test/client/readme_j.txt
を参照して下さい。

<備考>
        inet APIとBSD socket APIの比較やノリをつかむために
	利用して下さい。
