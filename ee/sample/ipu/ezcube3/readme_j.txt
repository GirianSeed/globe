[SCEI CONFIDENTIAL DOCUMENT]
DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
                  Copyright (C) 1999 by Sony Computer Entertainment Inc.
                                                     All Rights Reserved

複数の圧縮データをデコードするサンプル
<サンプルの解説>
	本サンプルは sample ディレクトリの中の ipu/ezcube のサンプルを
	拡張したものです。
	複数の圧縮されたデータを IPUで1つ1つデコードし、テクスチャバッ
	ファにDMA転送し、テクスチャとして表示しています。
	またIPUからのDMA終了割り込みを用いて、1つのデータのデコードが
	終了した直後に次のデコードを行っており、効率良くIPUを利用して
	います。
	
<ファイル>
	main.c
	ldimage.c
	ezcube.c
	ezcube.h
	ezmovie.h
	ez.dat
	rugby.dat
	movie3.dat
	456.dat
	1_6.dat
	wada.dat

<起動方法>
	% make		：コンパイル
	% make run	：実行

	コンパイル後、以下の方法でも実行可能
	% dsedb
	> run main.elf

<コントローラの操作方法>
	↑,↓ボタン	：x軸回転
	←,→ボタン	：y軸回転
	L1,L2ボタン	：z軸回転
	R1,R2ボタン	：立方体のz軸方向の移動
	△,×ボタン	：カメラのx軸回転
	○,□ボタン	：カメラのy軸回転
	startボタン	：リセット

<備考>
	EB-2000, EB-2000SにはGSにアンチエイリアシングのバグがあります。
	そのため以下の3行のコードを追加しました。

	/* GSのバグのため以下の3つのパケットを追加で転送 */
	sceVif1PkAddGsAD(&packet, SCE_GS_PRMODECONT, SCE_GS_SET_PRMODECONT(0));
	sceVif1PkAddGsAD(&packet, SCE_GS_PRMODE,
			 SCE_GS_SET_PRMODE(0,0,0,0,0,0,0,0));
	sceVif1PkAddGsAD(&packet, SCE_GS_PRMODECONT, SCE_GS_SET_PRMODECONT(1));
