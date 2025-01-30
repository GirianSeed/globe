[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                       Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                       All Rights Reserved


	共通ネットワーク設定インタフェースライブラリ
	サンプルプログラム(edit_entry)


< サンプルの解説 >

	このプログラムは、共通ネットワーク設定インタフェースライブラリで
	あなたのネットワーク設定ファイルの編集を行うサンプルプログラム
	です。


< ファイル >

	Makefile		： メイクファイル
	edit_entry.c		： メインプログラム
	load_module.c		： ロードモジュールプログラム
	load_module.h		： ロードモジュールプログラムヘッダ
	set_configuration.c	： 設定プログラム
	set_configuration.h	： 設定プログラムヘッダ


< 使用方法 >

	1. /usr/local/sce/ee/sample/netcnfif/add_entry/readme_j.txt に
	   従って、add_entry サンプルプログラムを実行する。

	2. 接続機器設定の情報はプログラムが取得するので、
	   使用する接続機器のみを接続する。

	   ※本サンプルプログラムでは接続機器として
	     an986.irx をドライバとする接続機器を使用することを前提とした
	     IOP モジュールのロードを行っています。その他の内容で接続を行う
	     場合、適切な IOP モジュールをロードするように変更する必要が
	     あります。

	   ※an986.irx は /usr/local/sce/iop/modules にあるものとします。

	3. /usr/local/sce/ee/sample/netcnfif/edit_entry で make する

	   > cd /usr/local/sce/ee/sample/netcnfif/edit_entry
	   > make

	4. dsedb を起動する

	   > dsedb

	5. 以下のように実行する

	   dsedb S> reset;run ./edit_entry.elf

	6. 以下のように表示されたら編集完了です。

	   [edit_entry.c] complete
	   # TLB spad=0 kernel=1:12 default=13:36 extended=37:47
	   *** Unexpected reply - type=BREAKR result=PROGEND
	   dsedb S> 

< 確認方法 >

	設定内容を確認するには例えば以下のような方法があります。

	1. /usr/local/sce/ee/sample/netcnfif/edit_entry/edit_entry.c 内で
	   sceNetcnfifSetFNoDecode(sceNetcnfifArg_f_no_decode_on); が
	   実行されていることを確認する。
	   (これが実行されていない場合、あなたのネットワーク設定ファイルは
	   符号化されるため内容の確認が行えません)

	2. /usr/local/sce/ee/sample/hdd/shell で make する

	   > cd /usr/local/sce/ee/sample/hdd/shell
	   > make

	3. dsecons を起動する

	   > dsecons

	4. dsedb を起動する

	   > dsedb

	5. 以下のように実行する

	   dsedb S> reset;run ./main.elf

	6. dsecons 画面であなたのネットワーク設定ファイルを保存した
	   デバイスに移動し、内容を確認する

	   [host1:]/ $ mc0:
	   [mc0:]/ $ ls
	   d------rwx 0     0     0x0027     0         8 Jan 24  5:36 .
	   d------rw- 0     0     0x0026     0         0 Nov 30 16:14 ..
	   d------rwx 0     0     0x0027     0         9 Jan 24  6:32 BWNETCNF
	   [mc0:]/ $ cd BWNETCNF
	   [mc0:]/BWNETCNF $ ls
	   d------rwx 0     0     0x0027     0         0 Jan 24  5:36 .
	   d------rwx 0     0     0x0027     0         0 Nov 30 16:14 ..
	   -------rwx 0     0     0x0017     0     33688 Jan 24  5:36 SYS_NET.ICO
	   -------rwx 0     0     0x0017     0       964 Jan 24  5:36 icon.sys
	   -------rwx 0     0     0x0017     0       115 Jan 24  6:32 dev000.cnf
	   -------rwx 0     0     0x0017     0       108 Jan 24  6:32 BWNETCNF
	   -------rwx 0     0     0x0017     0       133 Jan 24  6:32 ifc000.cnf
	   -------rwx 0     0     0x0017     0       100 Jan 24  5:36 net000.cnf
	   [mc0:]/BWNETCNF $ 

	7. 上記のディレクトリ及びファイルが存在していることを確認する。

	8. 設定管理ファイル(BWNETCNF)の内容を確認する。
	   内容は /usr/local/sce/ee/sample/netcnfif/edit_entry/edit_entry.c 内で
	   設定される以下の定義によって変わる。

	   #define IFC_NEW_USR_NAME "Ethernet Setting"

	   [mc0:]/BWNETCNF $ cat BWNETCNF
	   1,1,ifc000.cnf,Ethernet Setting
	   2,1,dev000.cnf,ethernet-vendor/ethernet-product
	   0,1,net000.cnf,Combination1

	   [mc0:]/BWNETCNF $ 

	9. 接続機器設定(dev000.cnf)と接続プロバイダ設定(ifc000.cnf)の内容を
	   確認する。
	   内容は /usr/local/sce/ee/sample/netcnfif/edit_entry/edit_entry.c 内で
	   設定される以下の定義によって変わる。

	   #define DEV_PATTERN (0)
	   #define IFC_PATTERN (0)

	   - dev000.cnf について

	     < #define DEV_PATTERN (0) の場合:
	       (接続機器としてネットワークアダプターを使用する設定) >

	     [mc0:]/BWNETCNF $ cat dev000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type nic
	     vendor "SCE"
	     product "Ethernet (Network Adaptor)"
	     phy_config auto

	     [mc0:]/BWNETCNF $ 

	     < #define DEV_PATTERN (1) の場合:
	       (接続機器としてネットワークアダプター以外のイーサネットアダ
	       プタを使用する設定) >

	     [mc0:]/BWNETCNF $ cat dev000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type eth
	     vendor "ethernet-vendor"
	     product "ethernet-product"
	     phy_config auto

	     [mc0:]/BWNETCNF $ 

	     < #define DEV_PATTERN (2) の場合:
	       (接続機器としてモデム・ TA を使用する設定) >

	     [mc0:]/BWNETCNF $ cat dev000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type ppp
	     vendor "modem-vendor"
	     product "modem-product"
	     chat_additional "\"\" ATM0 OK \\c"
	     outside_number "0"
	     outside_delay ",,,"
	     dialing_type tone
	     idle_timeout 600

	     [mc0:]/BWNETCNF $ 

	   - ifc000.cnf について

	     < #define IFC_PATTERN (0) の場合:
	       (接続機器としてネットワークアダプターを使用し、DHCP を使用
		しない設定) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type nic
	     -dhcp
	     address "192.168.0.1"
	     netmask "255.255.255.0"
	     route add -net 0.0.0.0 gw 192.168.0.2 netmask 0.0.0.0
	     nameserver add 192.168.0.3
	     nameserver add 192.168.0.4

	     [mc0:]/BWNETCNF $ 

	     < #define IFC_PATTERN (1) の場合:
	       (接続機器としてネットワークアダプターを使用し、DHCP を使用する
		設定) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type nic
	     dhcp
	     dhcp_host_name "host_name"
	     nameserver add 192.168.0.1
	     nameserver add 192.168.0.2

	     [mc0:]/BWNETCNF $ 

	     < #define IFC_PATTERN (2) の場合:
	       (接続機器としてネットワークアダプター以外のイーサネットアダ
	       プタを使用し、DHCP を使用しない設定) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type eth
	     -dhcp
	     address "192.168.0.1"
	     netmask "255.255.255.0"
	     route add -net 0.0.0.0 gw 192.168.0.2 netmask 0.0.0.0
	     nameserver add 192.168.0.3
	     nameserver add 192.168.0.4

	     [mc0:]/BWNETCNF $ 

	     < #define IFC_PATTERN (3) の場合:
	       (接続機器としてネットワークアダプター以外のイーサネットアダ
	       プタを使用し、DHCP を使用する設定) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type eth
	     dhcp
	     dhcp_host_name "host_name"
	     nameserver add 192.168.0.1
	     nameserver add 192.168.0.2

	     [mc0:]/BWNETCNF $ 

	     < #define IFC_PATTERN (4) の場合:
	       (接続機器としてイーサネットアダプタを使用し、PPPoEを使用し、
	       DNSサーバアドレスを自動取得する設定) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type ppp
	     -dhcp
	     auth_name "userid"
	     auth_key "password"
	     peer_name "*"
	     idle_timeout 0
	     -want.accm_nego
	     -want.prc_nego
	     -want.acc_nego
	     want.dns1_nego
	     want.dns2_nego
	     allow.auth chap/pap
	     pppoe
	     mtu 1454
	     route add -net 0.0.0.0 netmask 0.0.0.0

	     [mc0:]/BWNETCNF $ 

	     < #define IFC_PATTERN (5) の場合:
	       (接続機器としてイーサネットアダプタを使用し、PPPoEを使用し、
	       DNSサーバアドレスを自動取得しない設定) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type ppp
	     -dhcp
	     auth_name "userid"
	     auth_key "password"
	     peer_name "*"
	     idle_timeout 0
	     -want.accm_nego
	     -want.prc_nego
	     -want.acc_nego
	     allow.auth chap/pap
	     pppoe
	     mtu 1454
	     route add -net 0.0.0.0 netmask 0.0.0.0
	     nameserver add 192.168.0.1
	     nameserver add 192.168.0.2

	     [mc0:]/BWNETCNF $ 

	     < #define IFC_PATTERN (6) の場合:
	       (接続機器としてモデム・ TA を使用し、DNS サーバアドレスを
	       自動取得する設定) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type ppp
	     -dhcp
	     auth_name "userid"
	     auth_key "password"
	     peer_name "*"
	     want.dns1_nego
	     want.dns2_nego
	     allow.auth chap/pap
	     phone_number0 "11-1111-1111"
	     phone_number1 "22-2222-2222"
	     phone_number2 "33-3333-3333"
	     route add -net 0.0.0.0 netmask 0.0.0.0

	     [mc0:]/BWNETCNF $ 

	     < #define IFC_PATTERN (7) の場合:
	       (接続機器としてモデム・ TA を使用し、DNS サーバアドレスを
	       自動取得しない設定) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type ppp
	     -dhcp
	     auth_name "userid"
	     auth_key "password"
	     peer_name "*"
	     allow.auth chap/pap
	     phone_number0 "11-1111-1111"
	     phone_number1 "22-2222-2222"
	     phone_number2 "33-3333-3333"
	     route add -net 0.0.0.0 netmask 0.0.0.0
	     nameserver add 192.168.0.1
	     nameserver add 192.168.0.2

	     [mc0:]/BWNETCNF $ 
