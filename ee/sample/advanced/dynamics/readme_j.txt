[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                       Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                      All Rights Reserved 

剛体ダイナミクスサンプル

＜サンプルの解説＞

	本プログラムでは、いくつかの剛体ダイナミクスシミュレーションを行います。
	オブジェクト同士の衝突を検出するために、advanced/collisionサンプル
	の衝突検出機能を使います。

	使用されるアルゴリズムとその制限事項の詳細に関しては、
	overview_j.txtを参照してください。


＜ファイル＞

        main.c          ：サンプルプログラムのシステムソースコード
	sample*.c       ：サンプルプログラムのソースコード
	dynamics.c      ：剛体シミュレーション関数
	collision.c     ：衝突検出関数
	vector.h        ：collision.cで使われるベクトル/マトリクス
			  演算関数を含むインライン関数
        util.c          ：ユーティリティプログラムのソースコード
        camera.c       	：カメラ設定のためのプログラムソース
        light.c         ：光源設定関連プログラムソース
        sample_func.tbl ：登録済みサンプルプログラム関数を含む
			　ファイル
        vu0Coll.vcl     ：衝突アルゴリズムで使われるVu0マイクロ
			　関連VCL (VU コマンドライン)ソース
        vu0Coll.vsm     ：VSMソース            
       	data/*.bin      ：バイナリデータファイル
	data/*.s        ：ASCIIデータファイル


	＜データファイル＞
		data/block.bin          ： 描画モデル
		data/ellipsoid.bin      ： 描画モデル
		data/dpyramid.bin       ： 描画モデル
		data/plane1.bin         ： 描画モデル
		data/plane2.bin         ： 描画モデル
                data/plane3.bin         ： 描画モデル
                data/cube.bin           ： 描画モデル
                data/ball.bin           ： 描画モデル


(gjkobj-fileフォーマットについては、 advanced/collision/overview_j.txtを
参照してください)
                data/cblock.bin         ： 衝突検出用gjkobj-file 
                data/cellipsoid.bin     ： 衝突検出用gjkobj-file
		data/cdpyramid.bin      ： 衝突検出用gjkobj-file
		data/cplane1.bin        ： 衝突検出用gjkobj-file
		data/cplane2.bin        ： 衝突検出用gjkobj-file
		data/cplane3.bin        ： 衝突検出用gjkobj-file
                data/ccube.bin          ： 衝突検出用gjkobj-file
                data/cball.bin          ： 衝突検出用gjkobj-file


＜起動方法＞

        % make run


＜コントローラの操作方法＞

	プログラムを起動するとサンプルメニューが表示されます。
	このメニューでプログラムを開始します。

	＜メニューの操作方法＞
	STARTボタンを押すとサンプル画面からメニュー画面に戻ることが
	できます。

	＜サンプルモードの操作方法＞
	 STARTボタン：メニューに戻ります

	本サンプルは様々な操作モードで利用することができます。
	SELECTボタンを押してモードを変更します。すると、現在
	選択されているモードと使用方法が画面に表示されます。
 
         以下のモードを選択することができます。
	＜Camera Move Mode＞   	カメラパラメータの設定
	＜Light Ctrl Mode＞     ライトパラメータの設定（指向性ライト）
	＜Force Mode＞          選択したオブジェクトに力を加える
	＜Select Obj +Type Mode＞ 	オブジェクトを選択し、ACTIVE
					またはPASSIVEに設定する
			    ACTIVE...オブジェクトが全ての
				シミュレーションプロセスに参加している。 
                            PASSIVE...オブジェクトに重力が働かず、
				無限大質量を持っている物として扱われる。


＜備考＞

	overview_j.txtも参照してください。
	衝突アルゴリズムの解説についてはadvanced/collision/*.txtを参照
	してください。
	CPUとDMAのusageには、デバグメッセージの使用分も含まれています。
	
  
