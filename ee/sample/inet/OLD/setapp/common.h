/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/*
 *      Inet Setting Application Sample
 *
 *                          Version 1.7
 *
 *      Copyright (C) 2000-2002 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                          common.h
 *
 *        Version       Date             Design     Log
 *  --------------------------------------------------------------------
 *        1.1           2000.12.22       tetsu      Initial
 *        1.2           2001.01.31       tetsu      Change Final Flow
 *        1.3           2001.03.11       tetsu      Change for HDD
 *        1.4           2001.03.16       tetsu      Change for PPPoE
 *        1.5           2001.07.26       tetsu      1.4.5 -> 1.4.6
 *        1.6           2001.09.27       tetsu      1.4.6 -> 1.4.7(Add redial_count)
 *        1.7           2002.01.12       tetsu      1.4.7 -> 1.4.8(event bug fix)
 */

//#define DEBUG				/* debug print */

//#define NO_HDD				/* HDD なし */
//#define NO_HOST				/* HOST なし */
//#define MC_NO_DECODE			/* メモリーカードに保存する設定ファイルを decode しない */
//#define HDD_NO_DECODE			/* ハードディスクに保存する設定ファイルを decode しない */
#define HOST_NO_DECODE			/* ホストに保存する設定ファイルを decode しない */
#define HOST_READ_ONLY			/* ホストを Read Only にする */

//#define CD_ROM				/* CD-ROM 使用 */
//#define REPLACE				/* モジュール入れ換えをする */
#ifndef CD_ROM
#define MEM2MB_MODE			/* T10000 で 2MB モードにする時に使用 */
#endif /* CD_ROM */

#define SETAPP_VER		"1.4.8"

#define IEV_Attach		(1)	/* I/F attach */
#define IEV_Detach		(2)	/* I/F detach */
#define IEV_Start		(3)	/* I/F start (Running=1)*/
#define IEV_Stop		(4)	/* I/F stop (Running=0) */
#define IEV_Error		(5)	/* I/F error (Error=1) */
#define IEV_Conf		(6)	/* Configuration found */
#define IEV_NoConf		(7)	/* Configuration not found */
#define IEV_Redial		(8)	/* Redialing */

#define GET_COUNT		(0)
#define GET_LIST		(1)
#define LOAD_ENTRY		(2)
#define ADD_ENTRY		(3)
#define DELETE_ENTRY		(4)
#define SET_CNF			(5)
#define UP			(6)
#define DOWN			(7)
#define ALLOC_AREA		(8)
#define FREE_AREA		(9)
#define WAIT_EVENT		(10)
#define GET_IF_COUNT		(11)
#define GET_IF_LIST		(12)
#define EDIT_ENTRY		(13)
#define GET_HWADDR		(14)
#define COPY_ENTRY		(15)
#define DELETE_ALL		(16)

#define NETCNF_INTERFACE	(0x11)
#define NETCNF_STATUS		(0x12)
#define SSIZE			(4096)
#define MAX_LEN			(256)

#define DEVICE_STAT_MC_ON	(1 << 0)
#define DEVICE_STAT_HDD_ON	(1 << 2)
#define DEVICE_STAT_HOST_ON	(1 << 3)
#define DEVICE_STAT_MC_OFF	(~(1 << 0))
#define DEVICE_STAT_HDD_OFF	(~(1 << 2))
#define DEVICE_STAT_HOST_OFF	(~(1 << 3))

#define NO_SELECT		(0)
#define SELECT_MC		(1)
#define SELECT_HDD		(3)
#define SELECT_HOST		(4)

/******************************/
/* Rpc_NetCnf 関数引数・返値 */
/******************************/
typedef struct sceNetCnf {
  int data;				/* return value or other data                               (0x004) */
  int src_addr;				/* source address                                           (0x008) */
  int dst_addr;				/* destination address                                      (0x00c) */
  int size;				/* data size                                                (0x010) */
  int type;				/* ファイルの種類等                                         (0x014) */
  u_char hwaddr[6];			/* MAC アドレス                                             (0x01a) */
  char p0;				/*                                                          (0x01b) */
  char p1;				/*                                                          (0x01c) */
  int device_stat;			/* デバイスの状態                                           (0x02c) */
  int no_decode;			/* no_decode フラグ(0: decode する, 1: decode しない)       (0x020) */
  int no_decode2;			/* no_decode フラグ(0: decode する, 1: decode しない)       (0x024) */
  int no_decode3;			/* no_decode フラグ(0: decode する, 1: decode しない)       (0x024) */
  int p4;				/*                                                          (0x030) */
  char sys_name[MAX_LEN];		/* システムがつける設定ファイル名等                         (0x130) */
  char usr_name[MAX_LEN];		/* ユーザがつける設定ファイル名等                           (0x230) */
  char usr_name2[MAX_LEN];		/* ユーザがつける設定ファイル名等                           (0x330) */
  char dir_name[MAX_LEN];		/* 相対パス処理のための元になるパス名: 設定管理ファイル名等 (0x430) */
  char dir_name2[MAX_LEN];		/* 相対パス処理のための元になるパス名: 設定管理ファイル名等 (0x530) */
  char dir_name3[MAX_LEN];		/* 相対パス処理のための元になるパス名: 設定管理ファイル名等 (0x630) */
} sceNetCnf_t;

/**********************/
/* 設定ファイルリスト */
/**********************/
typedef struct sceNetCnfList2 {
  int select_device;			/* 選択されているデバイス                                   (0x004) */
  int flag;				/* 使用可能判定フラグ(0:使用不可, 1:使用可)                 (0x008) */
  int type;				/* ファイルの種類                                           (0x00c) */
  int stat;				/* ファイルステータス                                       (0x010) */
  char sys_name[MAX_LEN];		/* システムがつける設定ファイル名                           (0x110) */
  char usr_name[MAX_LEN];		/* ユーザがつける設定ファイル名                             (0x210) */
} sceNetCnfList2_t;

/************************************/
/* ネットワークインタフェースリスト */
/************************************/
typedef struct sceInetInterfaceList {
  int select_device;			/* 選択されているデバイス                                   (0x004) */
  int id;				/* ネットワークインタフェース ID                            (0x008) */
  int type;				/* ネットワークインタフェースタイプ                         (0x00c) */
  int mode;				/* リストモード(1: 既設定I/F+未設定I/F, 0: 未設定I/F)       (0x010) */
  int flag;				/* 設定済フラグ(2: 不明, 1: 設定済, 0: 未設定)              (0x014) */
  int p0;				/*                                                          (0x018) */
  int p1;				/*                                                          (0x01c) */
  int p2;				/*                                                          (0x020) */
  char vendor[MAX_LEN];			/* ベンダ名                                                 (0x120) */
  char product[MAX_LEN];		/* プロダクト名                                             (0x220) */
} sceInetInterfaceList_t;

/********************/
/* 設定ファイル内容 */
/********************/
typedef struct sceNetCnfEnvData {
  char attach_ifc[MAX_LEN];		/* 接続プロバイダ設定ファイル名(system name)                (0x0100) */
  char attach_dev[MAX_LEN];		/* 接続機器設定ファイル名(system name)                      (0x0200) */
  char address[MAX_LEN];		/* IP アドレス                                              (0x0300) */
  char netmask[MAX_LEN];		/* ネットマスク                                             (0x0400) */
  char gateway[MAX_LEN];		/* デフォルトルータ                                         (0x0500) */
  char dns1_address[MAX_LEN];		/* プライマリ DNS                                           (0x0600) */
  char dns2_address[MAX_LEN];		/* セカンダリ DNS                                           (0x0700) */
  char phone_numbers1[MAX_LEN];		/* 接続先電話番号 1                                         (0x0800) */
  char phone_numbers2[MAX_LEN];		/* 接続先電話番号 2                                         (0x0900) */
  char phone_numbers3[MAX_LEN];		/* 接続先電話番号 3                                         (0x0a00) */
  char auth_name[MAX_LEN];		/* ユーザ ID                                                (0x0b00) */
  char auth_key[MAX_LEN];		/* パスワード                                               (0x0c00) */
  char vendor[MAX_LEN];			/* ベンダ名                                                 (0x0d00) */
  char product[MAX_LEN];		/* プロダクト名                                             (0x0e00) */
  char chat_additional[MAX_LEN];	/* 追加 AT コマンド                                         (0x0f00) */
  char outside_set[MAX_LEN];		/* 外線発信番号設定                                         (0x1000) */
  char dhcp_host_name[MAX_LEN];		/* DHCP 用ホスト名                                          (0x1100) */
  int dialing_type;			/* ダイアル方法                                             (0x1104) */
  int type;				/* type(eth,ppp,nic)                                        (0x1108) */
  int phy_config;			/* イーサネット接続機器の動作モード                         (0x110c) */
  int idle_timeout;			/* 回線切断設定(単位(分))                                   (0x1110) */
  u_int p0;				/*                                                          (0x1114) */
  u_int p1;				/*                                                          (0x1118) */
  u_int p2;				/*                                                          (0x111c) */
  u_char dhcp;				/* dhcp(use,no use)                                         (0x111d) */
  u_char pppoe;				/* pppoe(use,no use)                                        (0x111e) */
  u_char p3;				/*                                                          (0x111f) */
  u_char p4;				/*                                                          (0x1120) */
} sceNetCnfEnvData_t;
