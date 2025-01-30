/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/*
 *	Inet Setting Application Sample
 *
 *                          Version 1.4
 *
 *      Copyright (C) 2000-2001 Sony Computer Entertainment Inc.
 *	                 All Rights Reserved.
 *
 *                             setapp.h
 *
 *        Version       Date            Design     Log
 *  --------------------------------------------------------------------
 *        1.1           2000.12.22      tetsu      Initial
 *        1.2           2001.01.31      tetsu      Change Final Flow
 *        1.3           2001.03.11      tetsu      Change for HDD
 *        1.4           2001.03.16      tetsu      Change for PPPoE
 */
#include <eekernel.h>
#include <eeregs.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libpad.h>
#include <libgraph.h>
#include <libdma.h>
#include <libpkt.h>
#include <libdev.h>
#include <libvu0.h>
#include <libmc.h>
#include <libcdvd.h>
#include <sif.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <malloc.h>

#include "./common.h"

#define sceNETCNF_TOO_MANY_ENTRIES	(-12)							// エントリ数が最大値
#define sceNETCNF_SYNTAX_ERROR		(-14)							// セーブデータが壊れている
#define sceNETCNF_MAGIC_ERROR		(-15)							// 他の PS2 でセーブされた設定が存在する
#define sceNETCNF_CAPACITY_ERROR	(-16)							// 容量が足りません

#define sceNetCnf_PHYCONFIG_AUTO	(1)							// auto-nego
#define sceNetCnf_PHYCONFIG_10		(2)							// 10,Half-Duplex
#define sceNetCnf_PHYCONFIG_10_FD	(3)							// 10,Full-Duplex
#define sceNetCnf_PHYCONFIG_10_FD_PAUSE	(4)							// 10,Full-Duplex,Pause
#define sceNetCnf_PHYCONFIG_TX		(5)							// TX,Half-Duplex
#define sceNetCnf_PHYCONFIG_TX_FD	(6)							// TX,Full-Duplex
#define sceNetCnf_PHYCONFIG_TX_FD_PAUSE	(7)							// TX,Full-Duplex,Pause

#define MC_DEV_MAX		(4)								// メモリーカードに保存できる最大接続機器設定数
#define MC_IFC_MAX		(4)								// メモリーカードに保存できる最大接続プロバイダ設定数

/* Screen Infomation */
#define SCREEN_WIDTH		(640)
#define SCREEN_HEIGHT		(224)

/* Other Information */
#define BASE_NAME		"/usr/local/sce"						// 絶対 PATH
//#define BASE_NAME		"../../../.."							// 相対 PATH
#ifdef CD_ROM
#define ICON_FILE	"cdrom0:\\MODULES\\SYS_NET.ICO;1"
#define ICON_SYS_FILE	"cdrom0:\\MODULES\\ICON.SYS;1"
#define IOPRP			"cdrom0:\\MODULES\\"IOP_IMAGE_FILE";1"				// リプレイスモジュール
#define MEM2MB			"cdrom0:\\MODULES\\MEM2MB.IRX;1"				// 2MB モード用モジュール
#define SIO2MAN			"cdrom0:\\MODULES\\SIO2MAN.IRX;1"				// sio2man.irx
#define PADMAN			"cdrom0:\\MODULES\\PADMAN.IRX;1"				// padman.irx
#define MCMAN			"cdrom0:\\MODULES\\MCMAN.IRX;1"					// mcman.irx
#define MCSERV			"cdrom0:\\MODULES\\MCSERV.IRX;1"				// mcserv.irx
#define NETCNF			"cdrom0:\\MODULES\\NETCNF.IRX;1"				// netcnf.irx
#define INET			"cdrom0:\\MODULES\\INET.IRX;1"					// inet.irx
#define INETCTL			"cdrom0:\\MODULES\\INETCTL.IRX;1"				// inetctl.irx
#define PPP			"cdrom0:\\MODULES\\PPP.IRX;1"					// ppp.irx
#define PPPOE			"cdrom0:\\MODULES\\PPPOE.IRX;1"					// pppoe.irx
#define USBD			"cdrom0:\\MODULES\\USBD.IRX;1"					// usbd.irx
#define USBMLOAD		"cdrom0:\\MODULES\\USBMLOAD.IRX;1"				// usbmload.irx
#define IFNETCNF		"cdrom0:\\MODULES\\IFNETCNF.IRX;1"				// ifnetcnf.irx
#define DEV9			"cdrom0:\\MODULES\\DEV9.IRX;1"					// dev9.irx
#define ATAD			"cdrom0:\\MODULES\\ATAD.IRX;1"					// atad.irx
#define HDD			"cdrom0:\\MODULES\\HDD.IRX;1"					// hdd.irx
#define PFS			"cdrom0:\\MODULES\\PFS.IRX;1"					// pfs.irx
#define SMAP			"cdrom0:\\MODULES\\SMAP.IRX;1"					// smap.irx
#define USBMLOAD_CNF		"cdrom0:\\CONF\\USBMLOAD.CNF;1"					// USB Auto Load Configration File
#else /* CD_ROM */
#define ICON_FILE	"host0:/usr/local/sce/ee/sample/inet/setapp/SYS_NET.ICO"
#define ICON_SYS_FILE	"host0:/usr/local/sce/ee/sample/inet/setapp/icon.sys"
#define IOPRP			"host0:"BASE_NAME"/iop/modules/"IOP_IMAGE_file			// リプレイスモジュール
#define MEM2MB			"host0:"BASE_NAME"/iop/modules/mem2MB.irx"			// 2MB モード用モジュール
#define SIO2MAN			"host0:"BASE_NAME"/iop/modules/sio2man.irx"			// sio2man.irx
#define PADMAN			"host0:"BASE_NAME"/iop/modules/padman.irx"			// padman.irx
#define MCMAN			"host0:"BASE_NAME"/iop/modules/mcman.irx"			// mcman.irx
#define MCSERV			"host0:"BASE_NAME"/iop/modules/mcserv.irx"			// mcserv.irx
#define NETCNF			"host0:"BASE_NAME"/iop/modules/netcnf.irx"			// netcnf.irx
#define INET			"host0:"BASE_NAME"/iop/modules/inet.irx"			// inet.irx
#define INETCTL			"host0:"BASE_NAME"/iop/modules/inetctl.irx"			// inetctl.irx
#define PPP			"host0:"BASE_NAME"/iop/modules/ppp.irx"				// ppp.irx
#define PPPOE			"host0:"BASE_NAME"/iop/modules/pppoe.irx"			// pppoe.irx
#define USBD			"host0:"BASE_NAME"/iop/modules/usbd.irx"			// usbd.irx
#define USBMLOAD		"host0:"BASE_NAME"/iop/modules/usbmload.irx"			// usbmload.irx
#define IFNETCNF		"host0:"BASE_NAME"/iop/sample/inet/setapp/ifnetcnf.irx"		// ifnetcnf.irx
#define DEV9			"host0:"BASE_NAME"/iop/modules/dev9.irx"			// dev9.irx
#define ATAD			"host0:"BASE_NAME"/iop/modules/atad.irx"			// atad.irx
#define HDD			"host0:"BASE_NAME"/iop/modules/hdd.irx"				// hdd.irx
#define PFS			"host0:"BASE_NAME"/iop/modules/pfs.irx"				// pfs.irx
#define SMAP			"host0:"BASE_NAME"/iop/modules/smap.irx"			// smap.irx
#define INETLOG			"host0:"BASE_NAME"/iop/util/inet/inetlog.irx"			// inetlog.irx
#define USBMLOAD_CNF		"host0:"BASE_NAME"/conf/usb/usbdrvho.cnf"			// USB Auto Load Configration File
#endif /* CD_ROM */
#define DIR_NAME		"/BWNETCNF"							// 設定の保存されるディレクトリ名
#define MC_DB_NAME0		"mc0:"DIR_NAME"/BWNETCNF"					// MC0-設定管理ファイル名(ディレクトリと同名)
#define MC_DB_NAME1		"mc1:"DIR_NAME"/BWNETCNF"					// MC1-設定管理ファイル名(ディレクトリと同名)
#define HDD_DB_NAME		"pfs0:/etc/network/net.db"					// HDD-設定管理ファイル名
#ifdef HOST_NO_DECODE
#define HOST_DB_NAME		"host0:"BASE_NAME"/conf/net/net.db"				// HOST-設定管理ファイル名
#else /* HOST_NO_DECODE */
#define HOST_DB_NAME		"host0:"BASE_NAME"/conf/net/decode/net.db"			// HOST-設定管理ファイル名
#endif /* HOST_NO_DECODE */
#define CMAX			(95)								// 文字最大数

#define NO_RETURN		(-1)
#define ADD_MODE		(0)
#define EDIT_MODE		(1)
#define DELETE_MODE		(2)
#define DEFAULT			(3)
#define ALL_ADD			(4)
#define COPY_MODE		(5)

#define BLUE			(5)
#define YELLOW			(6)
#define WHITE			(7)

#define sceInetDevF_ARP		(0x0010)
#define sceInetDevF_NIC		(0x0080)

#define LIMIT(x, min, max)	(((x)<(min))?(min):(((max)<(x))?(max):(x)))
#define GET_SIZE(size)		(((size) & 0xfffffff0) + (((size) % 0x10) ? 0x10 : 0))

extern int device_stat;										/* デバイスの状態 */

extern int no_decode;										/* no_decode フラグ(MC) */
extern int no_decode2;										/* no_decode フラグ(HDD) */
extern int no_decode3;										/* no_decode フラグ(HOST) */

extern char db_name[MAX_LEN];									/* 現在の DB_NAME */

extern int viewer_flag;										/* 現在の viewer_flag */
extern int viewer_type;										/* 現在の viewer_type */
extern int viewer_mode;										/* 現在の viewer_mode */
extern int viewer_init;										/* 画面の初期化情報 */
extern int add_flag;										/* Add Flag */
extern int add_type;										/* Add Type */
extern int delete_flag;										/* Delete Flag */
extern int hdd_flag;										/* HDD Flag */

extern int connect_flag;									/* Connect Flag */
extern int connect_stat;									/* Connect Status */
extern int connect_type;									/* Connect Type */
extern int connect_id;										/* Connect ID */

extern int list_update_arg;									/* list_update() 引数 */
extern int select_error_device;									/* エラーを起こしているデバイス */
extern int device_error_type;									/* デバイスエラータイプ */
extern int mc_error_type;									/* メモリーカードエラータイプ */
extern int status;										/* Status */
extern int mc_stat0, mc_stat1;									/* Memory Card Status */

extern char list_msg[MAX_LEN];									/* List Message */

extern sceInetInterfaceList_t *if_list;								/* Network Interface ID + vendor,product のリスト */
extern sceNetCnfList2_t *env_list;								/* Env List */
extern sceNetCnfList2_t *ifc_list;								/* Ifc List */
extern sceNetCnfList2_t *dev_list;								/* Dev List */

extern int if_menu;										/* 現在選択している I/F Menu の menu 番号 */
extern int env_menu;										/* 現在選択している Env Menu の menu 番号 */
extern int ifc_menu;										/* 現在選択している Ifc Menu の menu 番号 */
extern int dev_menu;										/* 現在選択している Dev Menu の menu 番号 */

extern int if_menu_max;										/* I/F Menu の最大値 */
extern int env_menu_max;									/* Env Menu の最大数 */
extern int ifc_menu_max;									/* Ifc Menu の最大数 */
extern int dev_menu_max;									/* Dev Menu の最大数 */

extern int env_select;										/* Env Select Flag */
extern int ifc_select;										/* Ifc Select Flag */
extern int dev_select;										/* Dev Select Flag */

extern char env_usr_name[MAX_LEN];								/* Env ファイル名(user name) */
extern char ifc_usr_name[MAX_LEN];								/* Ifc ファイル名(user name) */
extern char dev_usr_name[MAX_LEN];								/* Dev ファイル名(user name) */
extern char env_usr_name_bak[MAX_LEN];								/* Env ファイル名(user name)バックアップ */
extern char ifc_usr_name_bak[MAX_LEN];								/* Ifc ファイル名(user name)バックアップ */
extern char dev_usr_name_bak[MAX_LEN];								/* Dev ファイル名(user name)バックアップ */

extern int select_dev_device;
extern int select_ifc_device;

/* Inet Information */
extern sceNetCnfEnvData_t env_data __attribute__((aligned(64)));
extern sceNetCnfEnvData_t last_data __attribute__((aligned(64)));

/* Pad 情報 */
extern int port;
extern int slot;

/* Rpc 情報 */
extern sceNetCnf_t result, result_stat;
extern sceSifClientData cd, cd_stat;
extern u_char sdata[SSIZE] __attribute__((aligned(64)));
extern u_char sdata_stat[SSIZE] __attribute__((aligned(64)));

/* 文字情報 */
extern int disp_char_no;
extern char disp_char[];

/* 時間情報 */
extern int ct;

/* 描画環境 */
extern sceGsDBuff db;
extern int console;

/********************/
/* 関数プロトタイプ */
/********************/
void Gs_Initialize(void);									/* GS 初期化 */

u_short pad_read(void);										/* コントローラチェック */
void init_data(sceNetCnfEnvData_t *);								/* データの初期化 */
void save_data(void);										/* データのセーブ */
#ifdef DEBUG
void dump_data(sceNetCnfEnvData_t *);								/* データのダンプ */
void list_dump(sceNetCnfList2_t *, int);							/* リストのダンプ */
#endif /* DEBUG */

void connect_viewer(void);									/* ネットワーク接続画面 */
void menu_viewer(void);										/* ネットワーク設定画面 Menu */
void list_viewer(void);										/* ネットワーク設定画面 List */
void dev_viewer(void);										/* ネットワーク設定画面 Dev */
void ifc_viewer(void);										/* ネットワーク設定画面 Ifc */
void dev_more_viewer(void);									/* ネットワーク設定画面 Dev 詳細 */
void ifc_more_viewer(void);									/* ネットワーク設定画面 Ifc 詳細 */

void list_update(int);										/* Env, Ifc, Dev List を更新する */
int change_device(int);										/* 次のデバイスを検索する */
void print_device(int, int);									/* デバイスを出力する */

int search_char(char);										/* 指定文字の位置を search */
void delete_char(char *, int);									/* 一文字削除 */
void insert_char(char *, int, char, int, int);							/* 一文字挿入 */
void write_char(char *, int, char, int);							/* 一文字上書き */

void set_return(int, int, int, int, int, int);							/* Return Viewer を設定 */
int get_return(void);										/* Return Viewer を取得 */

int send_iop(u_int, u_int, u_int, u_int);							/* IOP へデータを送る */
int dma_check(int);										/* DMA の終了をチェック */

void Rpc_setup(void);										/* Rpc 環境のセットアップ */
void Rpc_Init(void);										/* Rpc Buffer Initialize */
void Rpc_Init_Stat(void);									/* Rpc Status Buffer Initialize */
int Rpc_Check(void);										/* NETCNF_INTERFACE 関数の状態をチェック */
int Rpc_Check_Stat(void);									/* NETCNF_STATUS 関数の状態をチェック */
int Rpc_Get_Result(void);									/* NETCNF_INTERFACE 関数の返値を取得 */
int Rpc_Get_Result_Stat(void);									/* NETCNF_STATUS 関数の返値を取得 */
void Rpc_sceNetCnfGetCount(int);								/* 指定した種類のファイル数を取得 */
void Rpc_sceNetCnfGetList(sceNetCnfList2_t *, int, int);					/* 指定した種類のファイルリストを取得 */
void Rpc_sceNetCnfLoadEntry(sceNetCnfEnvData_t *, int, char *, int);				/* 指定したファイルを読み込む */
void Rpc_sceNetCnfAddEntry(sceNetCnfEnvData_t *, int, char *, int);				/* 設定管理ファイルに追加し、ファイルに書き出す */
void Rpc_sceNetCnfEditEntry(sceNetCnfEnvData_t *, int, char *, char *, int);			/* 設定管理ファイルを編集し、ファイルに書き出す */
void Rpc_sceNetCnfDeleteEntry(int, char *, int);						/* 設定管理ファイルからエントリを削除し、ファイルを削除 */
void Rpc_sceNetCnfDeleteAll(int);								/* 指定されたデバイス上の設定を全て削除 */
void Rpc_sceNetCnfSetConfiguration(int, sceNetCnfEnvData_t *, char *, char *, int, int);	/* netcnf.irx が参照する設定データを置き換え、接続 */
void Rpc_sceNetCnfDownInterface(void);								/* 接続解除 */
void Rpc_AllocSysMemory(int);									/* Alloc する */
void Rpc_FreeSysMemory(int);									/* Free する */
void Rpc_WaitEvent(int);									/* 指定した ID のイベントを待つ */
void Rpc_GetInterfaceCount(int);								/* Interface List数 を取得する */
void Rpc_GetInterfaceList(int, int, int);							/* Interface List を取得する */
void Rpc_GetHWAddr(int);									/* ハードウェアアドレスを取得する */
void Rpc_sceNetCnfCopyEntry(int, char *, char *, int, int);					/* 指定されたエントリを別デバイスへ copy する */
