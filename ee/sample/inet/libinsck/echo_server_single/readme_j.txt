[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

libinsckライブラリ上で動作する単一スレッドのechoサーバサンプル


<サンプルの解説>
	このプログラムは、eeからlibinsckを用いてTCPのpassive openを
	行ない、接続してきたクライアントから受信したデータをそのまま
	クライアントに送信するechoサーバプログラムです。
	ee/sample/inet/libnet/echo_server の、ソケットライブラリ版です。

[ 注意 ]
	このサンプルでは USB イーサアダプタを使用する場合に、an986.irx が
	必要です。
	an986.irx については、タイトル内での使用を禁止としているため
	（詳細は 2000/11/22 付けの SCE-NET の「技術情報」を参照ください）
	リリースパッケージ（sce/iop/modules 下）には含まれておりません。
	SCE-NET の「ダウンロード」->「その他共通ツール」からダウンロード
	してください。

<ファイル>
        main.c        : メインプログラム
        single.c      : 通信部分のコード


<起動方法>

        $ make                               : コンパイル
        $ dsedb	                             : dsedbを起動
        > run main.elf <backlog>             : main.elfの実行。

echo_serverを起動したら、 linuxなどのtelnetプログラムから接続します。

        % telnet <saddr> 7                   : <saddr>, server address
        Trying 192.168.8.1...
        Connected to 192.168.8.1.
        Escape character is '^]'.
        hello
        hello
        bye
        bye


上記のように、クライアントから送信した文字がそのままサーバから返されます。


<コントローラの操作方法>
        なし

<備考>
	本プログラムは、一般にTCPのポート番号7に予約されているechoサーバ
	プログラムの"PlayStation 2"版です。
	UNIXマシンなどに対して、% telnet <unix_ip_addr> 7 とした場合と
	同じ挙動を示します。

	本プログラムでは接続環境として、USBイーサネット、HDD イーサネット、
	PPPoE(USBイーサネット)、PPPoE(HDDイーサネット)を想定しています。
	本プログラム中の先頭にある以下の #define を切り替えることによって
	接続環境を変更することが可能です。

	#define USB_ETHERNET			: USBイーサネット
	#define USB_ETHERNET_WITH_PPPOE		: PPPoE(USBイーサネット)
	#define HDD_ETHERNET			: HDDイーサネット
	#define HDD_ETHERNET_WITH_PPPOE		: PPPoE(HDDイーサネット)

	IPアドレスの変更やPPPoE のユーザ名変更を行なったりPPP を使用する
	場合は、各接続環境用の設定名 "Combination?"(? は数値) が示す設定
	ファイルを適切に変更して下さい。
	なお、下記がそれぞれに対応する設定名および設定ファイルです。

	USBイーサネット          Combination4         net003.cnf
	PPPoE(USBイーサネット)   Combination5         net004.cnf
	HDDイーサネット          Combination6         net005.cnf
	PPPoE(HDDイーサネット)   Combination7         net006.cnf

	また本プログラムでは、/usr/local/sce/conf/net 以下にある平文の設定
	ファイルを参照しています。これは本プログラムが開発サンプル用途で
	あるためで、正式のタイトル等では個体符号化された設定ファイルを
	用いる必要がある事に注意して下さい。
	個体符号化されたファイルの利用方法については、設定アプリケーション
	サンプルを参照して下さい。

	現在の接続環境設定ファイルでは、一つの設定ファイルには必ず一つしか
	インタフェースが指定されないため、本プログラムでも複数のデバイスを
	同時に利用することは想定していません。
	本プログラムは、目的のデバイスがただ一つ接続されている状況で
	動作します。
	同一デバイスの複数接続なども考慮していませんのでご注意下さい。

