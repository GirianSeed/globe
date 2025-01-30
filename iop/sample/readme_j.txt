[SCEI CONFIDENTIAL DOCUMENT]
DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
                  Copyright (C) 1999 by Sony Computer Entertainment Inc.
                                                     All Rights Reserved

                                                          サンプルの構成
========================================================================

サンプルディレクトリ構成
------------------------------------------------------------------------
sce/iop/sample/
├─hello
├─kernel
｜  ├─module
│  └─thread
├─sif
｜  ├─sifcmd
│  └─sifrpc
└─spu2
    ├─autodma
    ├─seqplay
    ├─stream
    └─voice


サンプル一覧
------------------------------------------------------------------------
サンプルコードには以下のものがあります。

hello:
	hello		"hello !" を表示するサンプル
kernel:
	kernel/module	常駐ライブラリモジュールの作成方法サンプル
	kernel/thread	スレッドの生成と起動、スレッドのプライオリティの
			操作、スレッド間の同期を行うサンプル
sif:
	sif/sifcmd	SIF CMD プロトコルのサンプル
	sif/sifrpc	SIF RPC プロトコルのサンプル
spu2:
	spu2/autodma	暫定サウンドライブラリ（libspu2） のAutoDMA転送
			機能を使って、ストレートPCM入力による発音を行う
			サンプル
	spu2/seqplay	暫定サウンドライブラリ（libspu2, libsnd2）を
			使って、IOP上でMIDIシーケンスの再生を行うサンプル
	spu2/stream	暫定サウンドライブラリ（libspu2）を使って、IOP上で
			SPU2のボイスによるストリーム再生を行うサンプル
	spu2/voice	暫定サウンドライブラリ（libspu2）を使って、IOP上で
			SPU2のボイス発音を行うサンプル


サンプルコンパイルの前に行っておくこと
------------------------------------------------------------------------
sce/iop/lib ディレクトリに移動して

 % make

と実行してください。
コンパイラのパスなどが設定されます。


