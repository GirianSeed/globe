[SCE CONFIDENTIAL DOCUMENT]
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                     All Rights Reserved


高レベルグラフィックスライブラリサンプル

<サンプルの解説>
	高レベルグラフィックスライブラリのユーザーインターフェースを使用
	して登録されたプラグインを実行するサンプルです。
	プラグインはSCE提供のプラグインを前提にしています。

<ファイル>
	main.c			SAMPLE PROGRAMのシステム部分ソース
	sample0~32.c		SAMPLE0~32 PROGRAMソース
	util.c			Utility PROGRAMソース
	camera.c		カメラ設定プログラムソース
	light.c			光源設定関連プログラムソース
	sample_func.tbl		SAMPLE PROGRAM FUNCTION登録ファイル
	micro.dsm		DVPソース
	data/*.bin		データバイナリファイル
	micro/*.vcl		VCL (VU Command Line)ソース
	micro/*.vsm		VSMソース

<起動方法>

        % make run

<コントローラの操作方法>
	このサンプルは、HiGの機能を紹介するためのサンプルプログラム群
	によって構成されています。
	起動後サンプルのメニューが現れます。
	このメニューにおいて、各サンプルを選択・起動することができます。

	＜メニュー操作ボタン＞
		↑/↓（方向ボタン）	メニューカーソルの移動
		○ ボタン		メニュー決定（サンプル起動）

	また、各サンプルからメニュー画面に戻るにはSTARTボタンの
	押し下げで判別しています。

	＜サンプルモード操作ボタン＞
		START	メニューに戻る

	以下、多くのサンプルに共通の操作を示します。詳細はサンプルごとの説明、
	および起動後の画面表示をごらんください。

	一部のサンプルはモードの概念を持ち、各モードは SELECTボタンで切り替わります。
	現在操作中のモードは画面に表示されます。

	CAMERA CTRL:			カメラ制御モード
		↑/↓ (方向ボタン)	x軸回転
		←/→ (方向ボタン)	y軸回転
		L1/L2 ボタン		z軸回転
		□/○ ボタン		x方向移動
		△/× ボタン		y方向移動
		R1/R2 ボタン		z方向移動
		カメラの回転は、ワールド座標原点を中心とした回転となっています。

	MICRO CHANGE:			マイクロ切替えモード
		□/○ ボタン		マイクロプログラムの切替

	LIGHT CTRL:			ライト制御モード

		←/→ (方向ボタン)	ライト番号の切替え
		↑/↓ (方向ボタン)	パラメータの切替え
					(DIRECTION, POS/ANGLE, COL/INTENS)

		・DIRECTION
			□/○ ボタン	x方向増減
			△/× ボタン	y方向増減
			R1/R2 ボタン	z方向増減
		・POS/ANGLE
			□/○ ボタン	x位置増減
			△/× ボタン	y位置増減
			R1/R2 ボタン	z位置増減
			L1/L2 ボタン	角度増減
		・COL/INTENS
			□/○ ボタン	Rカラー増減
			△/× ボタン	Gカラー増減
			R1/R2 ボタン	Bカラー増減
			L1/L2 ボタン	強度増減

＜サンプルの説明＞

		SAMPLE0		
			es2higの出力データを利用した１つのオブジェクト
			表示のサンプル
			データは内部でHOSTマシンからロードされます。
			・関連ファイル
				sample0.c
				data/dino.bin

		SAMPLE1
			es2higの出力データを利用した２つのオブジェクトを
			同時に表示するサンプル
			データは内部でHOSTマシンからロードされます。
			・関連ファイル
				sample1.c
				data/dino.bin
				data/shuttle.bin

		SAMPLE2
			es2higの出力データを利用した２つのオブジェクトの
			BLOCK操作関数による入れ替えを行うサンプル
			データは内部でHOSTマシンからロードされます。
			・関連ファイル
				sample2.c
				data/dino.bin
				data/shuttle.bin
			＜操作ボタン＞
				SELECT ボタン		OBJECTの入れ替え

		SAMPLE3
			各Pluginが使用できるraw data(生データ)から
			Plugin Block/Data BlockをBLOCK操作関数によって
			構築するサンプル
			データは内部でHOSTマシンからロードされます。
			・関連ファイル
				sample3.c 
				data/ball_basem.bin
				data/ball_hrchy.bin
				data/ball_micro.bin
				data/ball_anim.bin
				data/ball_key.bin
				data/ball_val.bin
				data/ball1_tex2d.bin
				data/ball1_shape.bin

		SAMPLE4
			各Pluginが使用できるraw data(生データ)から
			Plugin Block/Data BlockをBLOCK操作関数によって
			構築し、カメラからの位置によってモデルを切り替える
			LODの動作をさせた サンプル
			・関連ファイル
				sample4.c 
				data/ball_basem.bin
				data/ball_hrchy.bin
				data/ball_micro.bin
				data/ball_anim.bin
				data/ball_key.bin
				data/ball_val.bin
				data/ball1_shape.bin
				data/ball1_tex2d.bin
				data/ball2_shape.bin
				data/ball2_tex2d.bin
				data/ball3_shape.bin
				data/ball3_tex2d.bin
				data/ball4_shape.bin
				data/ball4_tex2d.bin

		SAMPLE5
			HiPで描画できるプリミティブのサンプル
			point/line/line strip/triangle/triangle strip/
			triangle fanの各プリミティブにTextureのあり/なし
			の計１２コのモデルを表示します。
			・ 関連ファイル
				sample5.c
				data/point.bin
				data/line.bin
				data/linestrip.bin
				data/triangle.bin
				data/tristrip.bin
				data/trifan.bin
				data/point_t.bin
				data/line_t.bin
				data/linestrip_t.bin
				data/triangle_t.bin
				data/tristrip_t.bin
				data/trifan_t.bin

		SAMPLE6
			Micro Codeの切替を行うサンプル
			・ 関連ファイル
				sample6.c
				data/ballS.bin

		SAMPLE7
			es2rawデータ(frameworkデータ)をHiGで利用する
			サンプル
			・関連ファイル
				sample7.c
				data/mbox_raw.bin

		SAMPLE8
			HiG データの内部を操作するサンプル
			アクセス関数を使用して、HiG データの内部の値を変更すること
			により、アニメーションやテクスチャの有無を操作します。
			・関連ファイル
				sample8.c
				data/APEWALK39.bin
				data/dino.bin

		SAMPLE9
			TIM2サンプル
			・関連ファイル
				sample9.c
				data/compmip.tm2
				data/abc-24.tm2
				data/abc-32.tm2
				data/info8-16.tm2
				data/info8-32-128.tm2
				data/Lysol1.tm2
				data/planeclut4.tm2
				data/tim2_raw.bin

		SAMPLE10
			CPU処理とDMA転送をダブルバッファリングする SAMPLE1
			アニメーションのフレームコントロールも行ないます。
			・関連ファイル
				sample10.c
				data/APEWALK39.bin
			＜操作ボタン＞
				L1 ボタン: 順方向再生
				L2 ボタン: 逆方向再生

		SAMPLE11
			PCRTCを使って２画面合成をするサンプル
			・関連ファイル
				sample11.c
				data/dino.bin
				data/subwin.raw
			＜操作ボタン＞
				←/→/↑/↓ (方向ボタン): サブウィンドウの移動
				○/□/×/△/L1/L2 ボタン: サブウィンドウの拡大率操作


		SAMPLE12
			テクスチャの常駐・非常駐のサンプル
			・関連ファイル
				sample12.c
				data/restex.bin
			＜操作ボタン＞
				SELECT ボタン: 常駐・非常駐の切替

		
		SAMPLE13
			GS サービスを利用したサブウィンドウ （マルチウィンドウ) の作成サンプル
			・関連ファイル
				sample13.c 
				data/shuttle.bin 
				data/curtain.bin
			＜操作ボタン＞
				 ←/→/↑/↓ (方向ボタン): 右下のサブウィンドウの移動
				 ○/□/×/△/R1/R2 ボタン: オブジェクト#2 の移動
		
		
		SAMPLE14
			GS サービスを利用した、テクスチャ領域への描画サンプル
			・関連ファイル
				sample14.c 
				data/APEWALK39.bin 
				data/curtain.bin
			＜操作ボタン＞
				 ←/→/↑/↓(方向ボタン)/L1/L2: テクスチャオブジェクトの回転
				 ○/□/×/△/R1/R2 ボタン: テクスチャオブジェクトの移動
		
		SAMPLE15
			モーションブラーのサンプル
			・関連ファイル
				sample15.c
				data/APEWALK39.bin
			＜操作ボタン＞
				 L1/L2: ブラーの強さの変更
				 ←/→/↑/↓(方向ボタン): カメラの回転
				 ○/□/×/△ボタン: カメラの移動

		SAMPLE16
			エンボスバンプマップサンプル
			・関連ファイル
				sample16.c
				micro/vu1embossPoint.vsm
				data/emboss0.bin 
				data/emboss1.bin
			＜操作ボタン＞
				ALPHA CTRL:
				   ○/□: ベースオブジェクト α値
				   ×/△: バンプオブジェクト α値
				BUMP CTRL:
				   ○/□: エンボスシフト量

		SAMPLE17
			CLUTバンプマップサンプル
			・関連ファイル
				sample17.c
				data/clutbump0.bin 
				data/clutbump1.bin
			＜操作ボタン＞
				○/□: ambient α値
				×/△: diffuse α値
				R1/R2: specular α値
				L1/L2: shininess 値
				CLUT α = ambient α + diffuse α * (light.normal) + specular α * (light.normal)^shininess
				 ※ (light.normal) ライトベクトルとノーマルベクトルとの内積

		SAMPLE18
			シャドーマップサンプル
			・関連ファイル
				sample18.c
				micro/vu1shadowTex.vsm
				micro/vu1shadowSTQCull.vsm
				data/shadowmap0.bin 
				data/shadowmap1.bin
			＜操作ボタン＞
				←/→/↑/↓/L1/L2: オブジェクトの回転
				○/□/×/△/R1/R2: オブジェクトの移動
				○/□: シャドーα値

		SAMPLE19
			ライトマップサンプル
			・関連ファイル
				sample19.c
				micro/vu1lightmapSTQCull.vsm
				data/wall.bin 
				data/litmap.bin 
				data/spot.raw
			＜操作ボタン＞
				Light Mode 操作

		SAMPLE20
			リフレクション・リフラクションサンプル
			・関連ファイル
				sample20.c
				micro/vu1reflectS.vsm 
				micro/vu1refractS.vsm
				data/reftorus.bin 
				data/refback.raw
			＜操作ボタン＞
				MICRO CHANGE:
				   □/○: リフレクション・リフラクションの切替
				PARAMETER CTRL: （リフラクションのみ）
				   □/○: 屈折率
				   ×/△: 拡大率

		SAMPLE21
			魚眼レンズを使ったダイナミックなリフレクション・リフラクションサンプル
			・関連ファイル
				sample21.c
				micro/vu1reflectR.vsm
				micro/vu1refractR.vsm
				micro/vu1fisheye.vsm
				data/reflect.bin
				data/reflect_fish.bin
				data/refract.bin
				data/refract_fish.bin
				data/refroom.bin
				data/refobj.bin
			＜操作ボタン＞
				OBJ CTRL:
				   ←/→/↑/↓/L1/L2: オブジェクトの回転
				   ○/□/×/△/R1/R2: オブジェクトの移動
				REFRACT CTRL:
				   □/○: 屈折率
				   ×/△: 拡大率
				   R1/R2: Zシフト量
				REFLECT CTRL:
				   ×/△: 拡大率
				   R1/R2: Zシフト量
				Blending CTRL:
				   □/○: リフレクションα値
				   ×/△: リフラクションα値

		SAMPLE22
			スペキュラーライトマップサンプル
			・関連ファイル
				sample22.c
				micro/vu1reflectS.vsm
				data/teapot.bin
			＜操作ボタン＞
				PARAMETER CTRL:
				   ×/△: 拡大率

		SAMPLE23
			フルスクリーンアンチエイリアスのサンプル
			・関連ファイル
				sample23.c
				data/APEWALK39.bin
				data/dino.bin
			＜操作ボタン＞
				   ○: アンチエイリアスの ON/OFF
				   ×/△ボタン: 描画バッファの深さの変更
				   L1/L2: ブラーの強さの変更
				   ←/→/↑/↓(方向ボタン): カメラの回転
				   R1/R2: カメラの移動
		SAMPLE24
                        魚眼レンズを使って描画するサンプル
                        ・ 関連ファイル
                                sample24.c
                                micro/vu1cullVo.vsm     通常の透視変換用マイクロプログラム
                                micro/vu1fisheye.vsm    魚眼レンズ変換用マイクロプログラム
                                data/refroom.bin
                
                        ＜操作ボタン＞
                                MICRO CHANGE:
                                   ○/□ ボタン: 魚眼レンズ変換と通常の透視変換の切替

                                PARAMETER CTRL: 
                                   ○/□ ボタン: 描画するイメージの大きさ変更
                

		SAMPLE25
			一つのモデルに複数のアニメーションをつけて表示するサンプル
			シェイププラグインほか必要なプラグインブロック・データブロッ
			クを多重化して使用する。
			・関連ファイル
				sample25.c 
				data/cubeman.bin
				data/animation.bin

		SAMPLE26
			ObjectのClippingを行う、Clip Plugのサンプル
			カメラのスクリーンを通常の(640, 448)ではなく
			(320, 224)として扱い、スクリーン枠としてGP
			で赤いラインを表示しています。

			また、Vu1のPrimitive単位でのClippingでClipされて
			しまうとサンプルとして解りづらくなるので、
			マイクロコードをPrimitive ClipなしのbasicVoに
			しています。
			オブジェクトをスクリーン外（赤枠外）に完全に
			出した場合、表示されず、またDMA転送もされないため
			DMA負荷が低減しているのが確認できます。
			・関連ファイル
				sample26.c
				data/clip.bin
				
		SAMPLE27
			1/60fps よりも遅いフレームレートでの表示サンプル
			遅いフレームレート上での描画サンプルです。
			3つのモードをデモンストレーションします。

			INTERLACE, V448 プログレッシブ解像度で描画し、インタレースで
				表示します。縦の解像度が落ちません。


			NON-INTERLACE, V224 ノンインタレースで表示します。
				縦の表示解像度が半分に落ちますが、VRAM の消費は少なくて
				済みます。
 			
			INTERLACE, V224  1/60fps と同じように描画・表示します。
				表示が縦に揺れてしまいます。

			・関連ファイル
				sample27.c
				data/APEWALK39.bin
			＜操作ボタン＞
				SELECT 表示方法の変更

		SAMPLE28
			ラジオシティーサンプル
			・関連ファイル
				sample28.c
				data/radtea.bin
				micro/vu1noshade.vsm
				micro/vu1vertexCspec.vsm
				micro/vu1cullSo.vsm
			＜操作ボタン＞
				MICRO CHANGE:
				   △/×:	マイクロコードの切替

		SAMPLE29
			クローンオブジェクトサンプル。
			tree.binデータのクローンを作成しています。
			・関連ファイル
				sample29.c
				data/tree.bin

		SAMPLE30
			VU1によるスキンデフォメーションサンプルです。
			最大4ボーン、4ウェイトのデータを扱う事ができます。
			・関連ファイル
				sample30.c
				micro/vu1skin.vsm
				data/skin1.bin
				data/under.tm2
				data/over.tm2
                        ＜操作ボタン＞
                                SELECT: モード切替
                                Bone Ctrl Mode:
                                   ←/→/↑/↓: ボーンの回転
                                   ○/□: 操作ボーン変更
				Root Ctrl Mode:
				   ○/□/×/△/R1/R2 ボタン:
				   オブジェクトの回転・移動

		SAMPLE31
			VU0によるスキンデフォメーションサンプルです。
			複数ボーン、4ウェイトのデータを扱う事ができます。
                        ・関連ファイル
                                sample31.c
                                mtxdisp.c
				data/skin0.bin
                        ＜操作ボタン＞
                                SELECT: モード切替
                                Bone Ctrl Mode:
                                   ←/→/↑/↓: ボーンの回転
                                   L1/L2/R1/R2: ボーンの移動
                                   ○/□: 操作ボーン変更

		SAMPLE32
			複数のピクチャーデータを持つTim2 Textureを使って
			Texture Change Animationをするサンプル
			・関連ファイル
				sample32.c
				data/water.bin
				data/ani_w.tm2
			＜操作ボタン＞
			    再生時
				SELECT : Animationの停止
				↑/↓  : Change Direction
				△/×  : Skip Ratio
			    停止時
				SELECT : Animationの再開
				↑/↓  : Change Direction
				△/×  : Skip Ratio
				←/→  : コマ送り

＜データのロードについて＞
	このサンプルプログラムはデータをホストからロードする形式をとって
	います。したがってホスト側に 上で記した必要データを作成しておいて
	ください。sample実行ファイル main.elfのあるディレクトリの相対パス
	で記述されています。このサンプルの場合、main.elfのあるディレクトリ
	にあるdataディレクトリ以下にデータバイナリを置いてください。

＜データについて＞
        サンプルで使用しているモデルデータは、米国 NewTek社の
        LightWave 3Dにより配布されているデータを元にして、
        株式会社ディ・ストーム社開発のeS Export for LightWave 3Dで
        エクスポートしました。

＜付録＞
	※ ユーザーのサンプル追加について
	このサンプルプログラム群は 簡単にサンプルプログラムを追加できる様
	になっています。
		sample_func.tbl
	ここで記述されている
		DEFFUNC( ... )
	の ... の部分が 各サンプルの登録となっています。
	たとえば
		DEFFUNC(USER_PROG)
	と登録すると、サンプルのメインメニューには
		> USER_PROG
	という項目が追加されるはずです。それが○ボタンで選択されると
		USER_PROG_init()
	が１回呼び出され、その後毎フレーム
		USER_PROG_main()
	が呼び出されます。USER_PROG_main()が ０を返し続ける限り
	USER_PROG_main()を呼び続けます。
	USER_PROG_main()が非０な値を返すとその次のフレームで
		USER_PROG_exit()
	が呼び出されます。ここで終了処理を行ないます。

	すなわち、ユーザがMY_PROGRAMという名称のプログラムを追加したければ

		1. sample_func.tblに
			DEFFUNC(MY_PROGRAM,comment)
		   を追加

		2. 以下のものを含むファイルを作成
			void MY_PROGRAM_init()
			int MY_PROGRAM_main()
			void MY_PROGRAM_exit()

		3. 2で作成したファイルをMakefileのソース定義部分に追加する

		4. makeする

	で 独自のプログラムを簡単に追加できます。

＜商標に関する注意書き＞
　　　　LightWave 3D(TM)は、米国NewTek社の商標です。
