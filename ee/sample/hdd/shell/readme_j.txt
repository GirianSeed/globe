[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

HDDのパーティション操作サンプル

<サンプルの解説>
	shell に似たコマンドラインツールのサンプルプログラムです。
	別ウインドウで dsecons を使用して入力します。
	起動後、パーティションをみつけると自動的にマウントを
	試みますが、パーティションにパスワードが設定されていたり、
	パーティションは作成されているが、フォーマットされていな
	かったりした場合はマウントに失敗します。この時のエラーは
	無視してください。

<ファイル>
	main.c

<起動方法>
	% make		：コンパイル
	% make run	：実行

	コンパイル後、以下の方法でも実行可能
	% ds[ei]db
	> run main.elf

	別ウインドウで dsecons を使用して入力します。

<コントローラの操作方法>
	なし

<備考>
	なし
