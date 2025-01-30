[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.5
                  Copyright (C) 2002 by Sony Computer Entertainment Inc.
                                                     All Rights Reserved

libeenet ライブラリを用いた、httpプロトコルにより
リモートファイルを取得するサンプル


<サンプルの解説>
        このプログラムは、libeenet ライブラリを用いて、
        リモートのhttpポートに接続し、
        ファイルダウンロードを行うサンプルプログラムです。

<ファイル>
        http.c        : メインプログラム

<起動方法>
        $ make                          : コンパイル
        $ dsedb -r run http.elf [saddr] : 実行

        saddrはWWWサーバアドレスです(www.scei.co.jp等)。
        実行すると、HTTPを利用してファイル http://<saddr>/index.html の
        取得を行ない、ローカルにセーブします。

<コントローラの操作方法>
        なし

<備考>
	本プログラムでは接続環境として、DHCP(IPv4/HDDイーサネット)、
	PPPoE(IPv4/HDDイーサネット)、Stateless Address Autoconfiguration
	(IPv6/HDDイーサネット)を想定しています。

	本プログラム中の先頭にある以下の #define を切り替えることによって
	接続環境を変更することが可能です。

	#define HDD_ETHERNET		: DHCP(IPv4/HDDイーサネット)
	#define HDD_ETHERNET_WITH_PPPOE	: PPPoE(IPv4/HDDイーサネット)
	#define HDD_ETHERNET_INET6	: Autoconf(IPv6/HDDイーサネット)

	IPアドレスの変更やPPPoE のユーザ名変更を行なったりする場合は、
	各接続環境用の設定名 "Combination?"(? は数値) が示す設定ファイ
	ルを適切に変更して下さい。
	なお、下記がそれぞれに対応する設定名および設定ファイルです。

	DHCP(HDDイーサネット)     Combination6         net005.cnf
	PPPoE(HDDイーサネット)    Combination7         net006.cnf
	Autoconf(HDDイーサネット) Combination8	       net007.cnf

	また本プログラムでは、../../../conf/net 以下にある平文の設定ファ
	イルを参照しています。これは本プログラムが開発サンプル用途であ
	るためで、正式のタイトル等では個体符号化された設定ファイルを用
	いる必要がある事に注意して下さい。
	個体符号化されたファイルの利用方法については、設定アプリケーション
	サンプルを参照して下さい。

	現在の接続環境設定ファイルでは、一つの設定ファイルには必ず一つしか
	インタフェースが指定されないため、本プログラムでも複数のデバイスを
	同時に利用することは想定していません。
	本プログラムは、目的のデバイスがただ一つ接続されている状況で
	動作します。
	同一デバイスの複数接続なども考慮していませんのでご注意下さい。

<処理概略>
        処理の概略は、以下の通りです。

        1. libeenet の初期化、各種モジュールのロード
        2. リモートポートへの接続
        3. GETメッセージの送信
        4. データの受信
	5. 各種モジュールのアンロード
	6. libeenet の終了処理
