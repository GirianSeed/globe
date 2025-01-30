[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                     All Rights Reserved  

衝突検出サンプル


＜サンプルの解説＞

本プログラムは、オブジェクトの衝突を検出するためにGJKアルゴリズムを使用
しています。また、レンダリングにはHiG/HiPライブラリを使用しています。


＜ファイル＞	
        main.c          	サンプルプログラムシステムソースコード
        sample.c        	サンプルプログラムのソースコード
	collision.c     	衝突検出のための関数
	vector.h        	collision.cで使われるベクトル/マトリクス
				演算関数を含むインライン関数
        util.c          	ユーティリティプログラムのソースコード
        camera.c       		カメラ設定のためのプログラムソース
        light.c         	光源設定のためのプログラムソース
        sample_func.tbl 	登録済みサンプルプログラム関数を含む
				ファイル
        vu0Coll.vcl     	衝突アルゴリズムで使われるVu0マイクロ
				のためのVCL (VU コマンドライン)ソース
        vu0Coll.vsm     	VSMソース            
       	data/*.bin      	バイナリデータファイル
	data/*.s        	ASCIIデータファイル

	＜データファイル＞

		data/block.bin          レンダリングモデル
		data/ellipsoid.bin      レンダリングモデル
              	data/cblock.bin         衝突検出のためのgjkobj-file
              	data/cellipsoid.bin     衝突検出のためのgjkobj-file
 

＜起動方法 ＞

        % make run


＜ コントローラの操作方法 ＞

	プログラムを起動するとサンプルメニューが表示されます。
	このメニューでプログラムを開始します。	
	
	＜メニューの操作方法＞
	STARTボタンを押すとサンプル画面からメニュー画面に戻ることが
	できます。

	＜サンプルモードの操作方法＞
	 STARTボタン：メニューに戻ります

	本サンプルは様々な操作モードで利用することができます。
	SELECTボタンを押してモードを選択して変更します。すると、現在
	選択されているモードと使用方法が画面に表示されます。

        以下のモードを選択することができます。
	<Camera Move Mode>   	カメラパラメータの設定
	<Light Ctrl Mode>      	照明パラメータの設定（指向性照明）
	<Obj Ctrl Mode>        	オブジェクト0の移動・回転
	<Coll Ctrl Mode>        衝突モードの選択：
	COLLISION CHECK : 
		オブジェクトが交差しているかどうかをチェックする
		（関数is_collision()を使用）
       	CALC. DISTANCE : 
		オブジェクト間の最短距離を計算する（calc_dist()を使用）
	FOLLOW SHORTEST DISTANCE : 
		オブジェクト間の距離が0.01以下になるまでオブジェクト1を
		最短距離に移動させる（calc_dist()を使用）

			
＜衝突検出の関数の使用について＞

  alloc_gjkobj_neighbors()   	衝突オブジェクトの隣接点を確保する

  init_collision()              Vu0マイクロコードをVu0プログラムメモリ
				にロードする

  gjk_init()                    サンプルで使われるGJKObjとSimplex構造体
				を初期化する

  is_collision()                衝突の発生を確認する

  calc_dist()                   オブジェクト間の最短距離を計算する 
				      
  free_gjkobj_neighbors()       隣接点に使われたメモリーを解放する   


＜注意事項＞

	本サンプルで使用するアルゴリズムの詳細については
	overview_j.txtを参照してください。

	表示されるCPUとDMAの使用量には、デバグメッセージやオブジェクト間の
	最短距離を示すためのデバグボックス表示時間も含まれています。


＜データのロード＞

	本プログラムでは、ホストからデータをロードすることができます。
	
	データをロードする前に上で述べた必要なデータをホスト側に作成して
	おいてください。データパスについては、sample実行ファイルmain.elf
	のあるディレクトリの相対パスで説明されています。
	
	本サンプルでは、バイナリデータをmain.elfのあるディレクトリの
	dataディレクトリ以下に入れてください。


＜付録 ＞

	* ユーザのサンプル追加について
	本サンプルプログラム群は以下のサンプルプログラムに簡単に追加
	することができます。
	
	DEFFUNC( ... ) はsample_func.tblで解説されています。

	括弧内に追加したいサンプル名を入力します。	

	たとえば、DEFFUNC(USER_PROG)を入力すると、以下の項目がプログラム
	のメインメニューに追加されます。	

			＞ USER_PROG
	
	これを○ボタンで選択します。すると、以下の関数が一度呼び出され
	ます。
			
			USER_PROG_init()
	
	そして、以下の関数が各フレームに呼び出されます。

			USER_PROG_main()
	
	USER_PROG_main()が0を返し続ける限り、USER_PROG_main()が呼び出
	されます。
	
	USER_PROG_main()が非0の値を返した場合、USER_PROG_exit()が次の
	フレームに呼び出され、終了処理が実行されます。
	

	MY_PROGRAMという名前で独自のプログラムを追加したい場合、以下の
	手順で簡単に実行することができます。

	1. まず、sample_func.tblにDEFFUNC(MY_PROGRAM,comment)を追加します。

	2. 次に、以下のものを含んだファイルを作成します。

			void MY_PROGRAM_init()
			int MY_PROGRAM_main()
			void MY_PROGRAM_exit()

	3. 2で作成したファイルをmake fileのソース定義部分に追加します。

	4. make fileを実行します。
		

