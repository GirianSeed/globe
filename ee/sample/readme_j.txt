[SCEI CONFIDENTIAL DOCUMENT]
DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
                  Copyright (C) 1999 by Sony Computer Entertainment Inc.
                                                     All Rights Reserved

                                                          サンプルの構成
========================================================================

サンプルディレクトリ構成
------------------------------------------------------------------------
sce/ee/sample/
├─basic3d
│  ├─core
│  ├─vu0
│  └─vu1
├─deci2
│  └─system
├─device
│  ├─file
│  └─pad
├─graphics
│  ├─capture
│  ├─clip_vu0
│  ├─clip_vu1
│  ├─clut
│  ├─clut_csa
│  ├─jointm
｜  ｜  └─intr_key
｜  ｜      ├─mkdata	
｜  ｜      └─runtime
│  ├─mipmap
│  ├─refmap	
│  └─tex_swap
├─ipu
│  ├─ezcube
│  ├─ezcube3	
│  └─ezmovie
├─kernel
│  └─thread
├─mpeg
│  └─ezmpeg
├─pc
│  └─dcache
├─pkt
│  ├─balls
│  ├─cube
│  └─mfifo
├─sif
│  ├─sifcmd
│  └─sifrp
├─spu2
│  ├─rautodma
│  ├─rseqplay
│  ├─rstream
│  └─rvoice
├─vu0
│  ├─tballs
│  ├─tdiff
│  ├─tfog
│  ├─tmip
│  ├─tskin
│  └─tsquare
└─vu1
　  ├─blow
　  ├─bspline
　  ├─deform
　  ├─hako
　  └─iga

サンプル一覧
------------------------------------------------------------------------
サンプルコードには以下のものがあります。

basic3d:
        3Dのオブジェクトを表示するまでの、基本的な手続きを理解するための
        サンプル
        basic3d/core    cpu のコアの命令のみで記述されたアルゴリズム
                        理解用サンプル
        basic3d/vu0     core のプログラムをもとにメインの処理を VU0 
                        マクロ命令に置き換えたサンプル
        basic3d/vu1     core、vu0 で行われている処理を、さらに VU1 
                        マイクロ命令で実現したサンプル

deci2:
	deci2/system    DECI2 プロトコルを用いたホスト上の system() を
			実行するサンプル

device:
        device/file     ホストマシンのファイル操作サンプル
        device/pad      コントローラ読み込みサンプル

graphics:
        graphics/capture        フレームバッファ内容をホストPCの
                                ファイルにセーブするサンプル
	graphics/clip_vu0	Clipping サンプル(VU0) 
	graphics/clip_vu1	Clipping サンプル(VU1)
        graphics/clut           4bit/8bitカラーのテクスチャを表示する
				サンプル
	graphics/clut_csa	4bit テクスチャCLUTの一時バッファへの
				ロードをcsa/cldフィールドによって制御する
				サンプル
	graphics/jointm/intr_key/mkdata
				回転軸内挿によるキーフレームマトリクス
				アニメーションのデータ作成サンプル
	graphics/jointm/intr_key/runtime
				回転軸内挿によるキーフレームマトリクス
				アニメーションのランタイムサンプル
        graphics/mipmap         mipmapを行うサンプル
	graphics/refmap		スペキュラーのある照度計算、リフレク
				ションマッピングを行うサンプル
	graphics/tex_swap	テクスチャを入れ替えながら描画を行う
				サンプル

ipu:
	ipu/ezcube	3次元ポリゴンに IPU を用いてデコードした
			テクスチャを張りつけたサンプル
	ipu/ezcube3	複数の圧縮データをデコードするサンプル
        ipu/ezmovie     IPU を使用した動画の簡易再生サンプル

kernerl:
	kernel/thread	マルチスレッドを用いてオブジェクトの描画を
			行なうサンプル

mpeg:
	mpeg/ezmpeg	IPU を使用した MPEG2(MPEG1を含む)ストリーム
			再生サンプル

pc:
	pc/dcache	Performance Counter を用いて D Cache miss を
			カウントするサンプル

pkt:
        pkt/balls       スプライト描画サンプル
        pkt/cube        ポリゴンモデル描画サンプル
	pkt/mfifo	MFIFOを用いてオブジェクトの描画を行うサンプル

sif:
	sif/sifcmd	SIF CMD プロトコルのサンプル
	sif/sifrpc	SIF RPC プロトコルのサンプル

spu2:
	spu2/rautodma	AutoDMA 転送機能を使って、ストレートPCM 入力に
			よる発音を行なうサンプル
	spu2/rseqplay	MIDIシーケンス再生を行なうサンプル
	spu2/rstream	ボイスによるストリーム再生を行なうサンプル
	spu2/rvoice	ボイスの発音を行なうサンプル

vu0:
        VU0を使用してジオメトリ処理を行なうサンプル
        vu0/tballs      ３次元の球体が動作するサンプル
        vu0/tdiff       ポリゴンのクリップを行なうサンプル
        vu0/tfog        方向なし点光源とオブジェクトクリップを行なう
                        サンプル
        vu0/tmip        MiMeの線形内挿とMIPMODELを行なうサンプル
        vu0/tskin       １つのモデルをマトリクスを使用して複数表示する
                        サンプル
        vu0/tsquare     インラインアセンブラを使用したVU0マクロのサンプル

vu1:
        VU1を使用してジオメトリ処理を行なうサンプル
        マイクロコードの記述にはdvpasm形式を使用
        vu1/blow        パーティクル描画サンプル
        vu1/bspline     VU1 B-Spline曲面描画サンプル
        vu1/deform      ２形状補間サンプル
        vu1/hako        VU1透視変換サンプル
        vu1/iga         VU1透視変換、光源計算サンプル

