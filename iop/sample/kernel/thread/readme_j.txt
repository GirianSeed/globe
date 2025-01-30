[SCEI CONFIDENTIAL DOCUMENT]
DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
                  Copyright (C) 1999 by Sony Computer Entertainment Inc.
                                                     All Rights Reserved

マルチスレッドサンプル

<サンプルの解説>

スレッドの生成と起動、スレッドのプライオリティの操作、スレッド間の同期等の
基本的なサンプルです。

<ファイル>

	createth.c		スレッドの生成と起動方法の基本的なサンプル
	sleepth.c		SleepThread()/WakeupThread() のサンプル
	eventth.c		SetEventFlag()/WaitEventFlag() のサンプル
	startthargs.c		スレッドの起動サンプル

<起動方法>

	% make					：コンパイル

    createth の実行

	まず、別のウインドウで
	% dsicons 
	を実行しておきます。

	% dsreset 0 2; dsistart createth.irx

	dsicons を実行したウインドウに 6 つのスレッドの起動の様子が
	表示されます。

	以下の方法でも実行可能
	% dsidb
	> reset 0 2 ; mstart createth.irx


    sleepth の実行

	まず、別のウインドウで
	% dsicons 
	を実行しておきます。

	% dsreset 0 2; dsistart sleepth.irx

	dsicons を実行したウインドウに '0..5,up,down > ' というプロンプト
	が表示されます。
	この時、プロンプトを表示して入力待ちになっているコマンド入力
	スレッドと、その他の 6 つのスレッドが起動しています。
	up を入力して、RETURN キーを入力すると、プロンプトを表示して
	入力待ちになっているスレッドのプライオリティが、他の 6 つの
	スレッドよりも高くなります。

	down を入力して、RETURN キーを入力すると、プロンプトを表示して
	入力待ちになっているスレッドのプライオリティが、他の 6 つの
	スレッドよりも低くなります。

	0 から 5 の数字を入力して、RETURN キーを入力すると対応する
	スレッドを起床(WakeupThread)させます。

	以下の方法でも実行可能
	% dsidb
	> reset 0 2 ; mstart sleepth.irx


    eventth の実行

	まず、別のウインドウで
	% dsicons 
	を実行しておきます。

	% dsreset 0 2; dsistart eventth.irx

	dsicons を実行したウインドウに '0..5,a,up,down > ' という
	プロンプトが表示されます。
	この時、プロンプトを表示して入力待ちになっているコマンド入力
	スレッドと、その他の 6 つのスレッドが起動しています。
	up を入力して、RETURN キーを入力すると、プロンプトを表示して
	入力待ちになっているスレッドのプライオリティが、他の 6 つの
	スレッドよりも高くなります

	down を入力して、RETURN キーを入力すると、プロンプトを表示して
	入力待ちになっているスレッドのプライオリティが、他の 6 つの
	スレッドよりも低くなります。

	0 から 5 の数字を入力して、RETURN キーを入力するとイベントフラグの
	対応するビットを立てます。

	a を入力して、RETURN キーを入力するとイベントフラグの全部のビット
	を立てます。

	以下の方法でも実行可能
	% dsidb
	> reset 0 2 ; mstart eventth.irx


    startthargs の実行

	まず、別のウインドウで
	% dsicons 
	を実行しておきます。

	% dsreset 0 2; dsistart startthargs.irx

	dsicons を実行したウインドウに スレッドの起動引数の様子が
	表示されます。

	以下の方法でも実行可能
	% dsidb
	> reset 0 2 ; mstart startthargs.irx
