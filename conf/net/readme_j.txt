[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5
                Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                   All Rights Reserved

「ネットワーク設定ファイルサンプルについて」

/usr/local/sce/conf/net には以下のネットワーク設定ファイルサンプルが
含まれています。

net000.cnf - ifc000.cnf と dev000.cnf の組み合わせ
net001.cnf - ifc001.cnf と dev001.cnf の組み合わせ
net002.cnf - ifc002.cnf と dev002.cnf の組み合わせ
net003.cnf - ifc003.cnf と dev003.cnf の組み合わせ
net004.cnf - ifc004.cnf と dev003.cnf の組み合わせ
net005.cnf - ifc005.cnf と dev002.cnf の組み合わせ
net006.cnf - ifc004.cnf と dev002.cnf の組み合わせ

ifc000.cnf - type eth サンプル
ifc001.cnf - type ppp サンプル
ifc002.cnf - type nic サンプル
ifc003.cnf - type eth サンプル(use dhcp)
ifc004.cnf - pppoe サンプル
ifc005.cnf - type nic サンプル(use dhcp)

dev000.cnf - type eth サンプル
dev001.cnf - type ppp サンプル
dev002.cnf - type nic サンプル
dev003.cnf - type eth サンプル

※ ???000.cnf ～ ???001.cnf については dev ファイル内の vendor, product
   を適切な名称に変更し、ifc ファイル内の IP Address, User ID, Password 等
   を適切な値等に変更することにより使用可能となります。

※ ???002.cnf については ifc ファイル内の IP Address 等を適切な値に変更
   することにより使用可能となります。

※ ???003.cnf,???005.cnf についてはサンプル等でそのまま使用可能です。

※ ???004.cnf,???006 については ifc ファイル内の User ID, Password 等を適切な
   値等に変更することにより使用可能となります。

<net.dbについて>
設定管理ファイル net.db 内の構造は sceNetCnfList 構造体で取得できる
データを ',' で並べた内容です。
(ただし type, stat は ASCII <-> バイナリ値 の変換が介在します)
sceNetCnfList 構造体については IOPライブラりリファレンス ->
共通ネットワーク設定ライブラリ(ntcnf_rf) を参照下さい。
また実際の設定管理ファイルの使い方（エントリ追加など）につきましては
sce/iop/sample/inet/setapp を参考にしてください。

