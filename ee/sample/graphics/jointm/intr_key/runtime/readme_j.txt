[SCEI CONFIDENTIAL DOCUMENT]
DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
　　　　　　　　　Copyright (C) 1999 by Sony Computer Entertainment Inc.
                                                   All Rights Reserved

回転軸内挿によるキーフレームマトリクスアニメーションのランタイムサンプル


<サンプルの解説>
	intr_key 以下のプログラムは、回転軸内挿という方法で、関節のマ
	トリクスによるキーフレームアニメーションを行なうサンプルです。

	このディレクトリ runtime のプログラムは、回転軸内挿用のパラメー
	タシーケンスを使用して実際のキーフレームアニメーションを行ないます

<ファイル>
	main.c      メイン関数、一般表示関連関数
	matintr.c   キーフレームアニメーション関連関数
	axis.c      回転軸内挿関連関数
	aseq.c      パラメータシーケンスデータ読み込み用
	axisseq.dat パラメータシーケンスデータ実体 (mkdata で作成したもの)
	basic.vsm   VU1 マイクロプログラム
	dma.dsm     DMA 用データ一般
	torso.dsm   胴体部モデルデータ
	head.dsm    頭部モデルデータ
	arm.dsm     腕部モデルデータ
	leg.dsm     脚部モデルデータ

<起動方法>
	% make		：コンパイル
	% make run	：実行
	もしくは、以下の方法でも実行できます
	% dsedb
	> run *.elf

<コントローラの操作方法>
	○,△,×,□,R1,R2	: カメラ位置変更
	start			: リプレイ

<備考>
	回転内挿用のパラメータシーケンスは、mkdata ディレクトリのサンプル
	プログラムを使用して作成できます。
