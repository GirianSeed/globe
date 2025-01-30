[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                   Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

voicesdr - ボイス発音のサンプル (libsdr 使用)

<サンプルの解説>
	libsdr のサンプルです。EE から IOP 経由にて SPU2 のボイス発音
	を行ないます。
	オプションで、バッチ処理、転送終了割り込みハンドラ、および 
	SPU2 割り込みハンドラの実験ができます。

	波形データである VAG ファイルを独自に作成する場合は AIFF2VAG 
	を使用してください。

	SPU2 ではコアの選択に注意が必要です。コア 0 の出力はコア 1 の
	メインボリュームを通る（つまりコア 1 のメインボリュームがゼロ
	の場合はコア 0 の音も発音されない）ことに注意してください。

	BATCH_MODE を 1 に設定してコンパイルした場合、バッチ処理のコー
	ドが有効になります。
	これにより、これまで独立の関数呼び出しで行なっていた複数の処理
	をバッチ処理にて一括して行ないます。

	IRQ_CB_TEST を 1 に設定してコンパイルすると、SPU2 割り込みハン
	ドラを設定するコードが有効になります。
	これにより、波形データの先頭から 0x1000 バイト（IRQ_ADDR_OFST）
	後方の番地が発音処理によってアクセスされた時、"interrupt
	detected" のメッセージがコンソールに出力されます。

	DMA_CB_TEST を 1 に設定してコンパイルすると、転送終了割り込み
	ハンドラを設定するコードが有効になります。
	VAG ファイルの転送が終了した時に "interrupt detected" のメッセー
	ジがコンソールに出力されます。

<ファイル>
	main.c	

<起動方法>
	% make		：コンパイル
	% make run	：実行

	Piano音で1オクターブ8音の音階が発音されれば正常。

<コントローラの操作方法>
	なし

<備考>
	このサンプルは IOP 側でファイルの内容を読み込む関数 
	sceSifLoadIopHeap() を使用しているため、ファイル読み込み時のファ
	イル名指定に IOP 側の指定である "host1:" を使用しています。
	また、make run 時には dsifilesv によって実行ファイルを実行させ
	ています。
