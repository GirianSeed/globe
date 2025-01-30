[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
	                  Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                          All Rights Reserved

	ＡＴＯＫライブラリ サンプルプログラム


< サンプルの解説 >

	このプログラムは、ＡＴＯＫライブラリ(libatok)の使用例を示した
	サンプルプログラムです。

	ＡＴＯＫライブラリ動作条件として、
	"PlayStation 2"専用 ハードディスクドライブユニットが、接続されており、
	システムソフトウェアのインストールが行われている必要があります。

	また、以下のパッケージをプログラム中で使用していますので SCE-NET より
	それぞれダウンロードしてカレントディレクトリに展開してください。

	- Shift JIS <-> UTF8 文字コード変換ライブラリパッケージ (libccc )
	- 簡易フォントライブラリパッケージ (libpfont)


< ファイル >

	Makefile         ： メイクファイル
	main.c           ： メインプログラム
	pad.c            ： コントローラ関連プログラム
	pad.h            ： コントローラ関連ヘッダ
	kbd.c            ： USBキーボード関連プログラム
	kbd.h            ： USBキーボード関連ヘッダ

	skb/ 以下        ： ATOKライブラリを使用した、
	                    ソフトウェアキーボード関連プログラム。
	                    ( libpfont を使用しています。)
	                    詳細は、skb/readme_j.txt を参照下さい。



< Shift JIS <-> UTF8 文字コード変換ライブラリパッケージ（libccc） >
	このプログラムは Shift JIS と UTF8 の文字コード変換ライブラリとして別途
	SCE-NETよりリリースされている Shift JIS <-> UTF8 文字コード変換ライ
	ブラリパッケージを使用しています。make時に必要となりますので適宜
	SCE-NET よりダウンロードし、サンプルプログラムディレクトリに展開して
	下さい。

< 簡易フォントライブラリパッケージ (libpfont)  >
	このプログラムは簡易フォントライブラリ(libpfont) として別途
	SCE-NETよりリリースされているライブラリパッケージを使用しています。
	make時に必要となりますので適宜 SCE-NET よりダウンロードし、
	サンプルプログラムディレクトリに展開して下さい。


< 起動方法 >
	% make clean ：クリーン
	% make       ：コンパイル
	% make run   ：実行


< コントローラの操作方法 >

	○ボタン：キーボードオープン
	×ボタン      ：キャンセル終了。
	スタートボタン：決定終了。

	その他、操作詳細については、skb/readme_j.txt を、参照ください。


< 制限事項・注意事項 >

	- このプログラムでは、ソース内に Shift JIS の文字列を含んで
	  いるため、環境変数 LANG の影響を受ける場合がありますので
	  ご注意ください。

	  このプログラムを使用される際は、環境変数 LANG に Shift JIS を
	  指定してください。

	 .bashrc の場合： 
	 $ export LANG=C-SJIS

	 .cshrc の場合：
	 $ setenv LANG C-SJIS

