[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                       Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                       All Rights Reserved


	共通ネットワーク設定インタフェースライブラリ
	サンプルプログラム(load_entry)


< サンプルの解説 >

	このプログラムは、共通ネットワーク設定インタフェースライブラリで
	あなたのネットワーク設定ファイルの内容の取得を行うサンプルプログラム
	です。また、取得したあなたのネットワーク設定ファイルの内容を使用し、
	接続テスト(Libnet ライブラリを用いて接続し、アドレスを取得した後、
	切断)を行います。


< ファイル >

	Makefile	： メイクファイル
	load_entry.c	： メインプログラム
	load_module.c	： ロードモジュールプログラム
	load_module.h	： ロードモジュールプログラムヘッダ


< 使用方法 >

	1. /usr/local/sce/ee/sample/netcnfif/add_entry 等を使用して
	   メモリーカード(PS2)に正しく接続できる接続機器設定、接続プロバイダ
	   設定を作成し、組み合わせ1(Combination1)に登録する。
	   そのメモリーカード(PS2)を差込口1に挿入します。

	   ※本サンプルプログラムでは接続機器として
	     「SCE/Ethernet (Network Adaptor)」を使用することを前提とした
	     IOP モジュールのロードを行っています。その他の内容で接続を
	     行う場合、適切な IOP モジュールをロードするように変更する
	     必要があります。

	   ※各 "PlayStation 2" 毎に符号化されたあなたのネットワーク
	     設定ファイルを使用する場合は、
	     sceNetcnfifSetFNoDecode(sceNetcnfifArg_f_no_decode_on);
	     をコメントアウトして下さい。

	2. /usr/local/sce/ee/sample/netcnfif/load_entry で make する

	   > cd /usr/local/sce/ee/sample/netcnfif/load_entry
	   > make

	3. dsedb を起動する

	   > dsedb

	4. 以下のように実行する

	   dsedb S> reset;run ./load_entry.elf

	5. /usr/local/sce/ee/sample/netcnfif/load_entry/load_entry.c 内で
	   #define DEBUG が定義されているならばあなたのネットワーク設定
	   ファイルの内容を以下のように出力します。

	   -----------------------
	   attach_ifc       : "ifc000.cnf"
	   attach_dev       : "dev000.cnf"
	   dhcp_host_name   : "(null)"
	   address          : "192.168.0.1"
	   netmask          : "255.255.255.0"
	   gateway          : "192.168.0.2"
	   dns1_address     : "192.168.0.3"
	   dns2_address     : "192.168.0.4"
	   phone_numbers1   : "(null)"
	   phone_numbers2   : "(null)"
	   phone_numbers3   : "(null)"
	   auth_name        : "(null)"
	   auth_key         : "(null)"
	   peer_name        : "(null)"
	   vendor           : "SCE"
	   product          : "Ethernet (Network Adaptor)"
	   chat_additional  : "(null)"
	   outside_number   : "(null)"
	   outside_delay    : "(null)"
	   ifc_type         : "-1"
	   mtu              : "-1"
	   ifc_idle_timeout : "-1"
	   dev_type         : "3"
	   phy_config       : "1"
	   dialing_type     : "-1"
	   dev_idle_timeout : "-1"
	   dhcp             : "0"
	   dns1_nego        : "255"
	   dns2_nego        : "255"
	   f_auth           : "0"
	   auth             : "0"
	   pppoe            : "255"
	   prc_nego         : "255"
	   acc_nego         : "255"
	   accm_nego        : "255"
	   -----------------------
	   address: "192.168.0.1"
	   [load_entry.c] complete
	   # TLB spad=0 kernel=1:12 default=13:36 extended=37:47
	   *** Unexpected reply - type=BREAKR result=PROGEND
	   dsedb S> 
