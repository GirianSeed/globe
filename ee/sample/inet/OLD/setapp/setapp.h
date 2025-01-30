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

#define sceNETCNF_TOO_MANY_ENTRIES	(-12)							// �G���g�������ő�l
#define sceNETCNF_SYNTAX_ERROR		(-14)							// �Z�[�u�f�[�^�����Ă���
#define sceNETCNF_MAGIC_ERROR		(-15)							// ���� PS2 �ŃZ�[�u���ꂽ�ݒ肪���݂���
#define sceNETCNF_CAPACITY_ERROR	(-16)							// �e�ʂ�����܂���

#define sceNetCnf_PHYCONFIG_AUTO	(1)							// auto-nego
#define sceNetCnf_PHYCONFIG_10		(2)							// 10,Half-Duplex
#define sceNetCnf_PHYCONFIG_10_FD	(3)							// 10,Full-Duplex
#define sceNetCnf_PHYCONFIG_10_FD_PAUSE	(4)							// 10,Full-Duplex,Pause
#define sceNetCnf_PHYCONFIG_TX		(5)							// TX,Half-Duplex
#define sceNetCnf_PHYCONFIG_TX_FD	(6)							// TX,Full-Duplex
#define sceNetCnf_PHYCONFIG_TX_FD_PAUSE	(7)							// TX,Full-Duplex,Pause

#define MC_DEV_MAX		(4)								// �������[�J�[�h�ɕۑ��ł���ő�ڑ��@��ݒ萔
#define MC_IFC_MAX		(4)								// �������[�J�[�h�ɕۑ��ł���ő�ڑ��v���o�C�_�ݒ萔

/* Screen Infomation */
#define SCREEN_WIDTH		(640)
#define SCREEN_HEIGHT		(224)

/* Other Information */
#define BASE_NAME		"/usr/local/sce"						// ��� PATH
//#define BASE_NAME		"../../../.."							// ���� PATH
#ifdef CD_ROM
#define ICON_FILE	"cdrom0:\\MODULES\\SYS_NET.ICO;1"
#define ICON_SYS_FILE	"cdrom0:\\MODULES\\ICON.SYS;1"
#define IOPRP			"cdrom0:\\MODULES\\"IOP_IMAGE_FILE";1"				// ���v���C�X���W���[��
#define MEM2MB			"cdrom0:\\MODULES\\MEM2MB.IRX;1"				// 2MB ���[�h�p���W���[��
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
#define IOPRP			"host0:"BASE_NAME"/iop/modules/"IOP_IMAGE_file			// ���v���C�X���W���[��
#define MEM2MB			"host0:"BASE_NAME"/iop/modules/mem2MB.irx"			// 2MB ���[�h�p���W���[��
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
#define DIR_NAME		"/BWNETCNF"							// �ݒ�̕ۑ������f�B���N�g����
#define MC_DB_NAME0		"mc0:"DIR_NAME"/BWNETCNF"					// MC0-�ݒ�Ǘ��t�@�C����(�f�B���N�g���Ɠ���)
#define MC_DB_NAME1		"mc1:"DIR_NAME"/BWNETCNF"					// MC1-�ݒ�Ǘ��t�@�C����(�f�B���N�g���Ɠ���)
#define HDD_DB_NAME		"pfs0:/etc/network/net.db"					// HDD-�ݒ�Ǘ��t�@�C����
#ifdef HOST_NO_DECODE
#define HOST_DB_NAME		"host0:"BASE_NAME"/conf/net/net.db"				// HOST-�ݒ�Ǘ��t�@�C����
#else /* HOST_NO_DECODE */
#define HOST_DB_NAME		"host0:"BASE_NAME"/conf/net/decode/net.db"			// HOST-�ݒ�Ǘ��t�@�C����
#endif /* HOST_NO_DECODE */
#define CMAX			(95)								// �����ő吔

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

extern int device_stat;										/* �f�o�C�X�̏�� */

extern int no_decode;										/* no_decode �t���O(MC) */
extern int no_decode2;										/* no_decode �t���O(HDD) */
extern int no_decode3;										/* no_decode �t���O(HOST) */

extern char db_name[MAX_LEN];									/* ���݂� DB_NAME */

extern int viewer_flag;										/* ���݂� viewer_flag */
extern int viewer_type;										/* ���݂� viewer_type */
extern int viewer_mode;										/* ���݂� viewer_mode */
extern int viewer_init;										/* ��ʂ̏�������� */
extern int add_flag;										/* Add Flag */
extern int add_type;										/* Add Type */
extern int delete_flag;										/* Delete Flag */
extern int hdd_flag;										/* HDD Flag */

extern int connect_flag;									/* Connect Flag */
extern int connect_stat;									/* Connect Status */
extern int connect_type;									/* Connect Type */
extern int connect_id;										/* Connect ID */

extern int list_update_arg;									/* list_update() ���� */
extern int select_error_device;									/* �G���[���N�����Ă���f�o�C�X */
extern int device_error_type;									/* �f�o�C�X�G���[�^�C�v */
extern int mc_error_type;									/* �������[�J�[�h�G���[�^�C�v */
extern int status;										/* Status */
extern int mc_stat0, mc_stat1;									/* Memory Card Status */

extern char list_msg[MAX_LEN];									/* List Message */

extern sceInetInterfaceList_t *if_list;								/* Network Interface ID + vendor,product �̃��X�g */
extern sceNetCnfList2_t *env_list;								/* Env List */
extern sceNetCnfList2_t *ifc_list;								/* Ifc List */
extern sceNetCnfList2_t *dev_list;								/* Dev List */

extern int if_menu;										/* ���ݑI�����Ă��� I/F Menu �� menu �ԍ� */
extern int env_menu;										/* ���ݑI�����Ă��� Env Menu �� menu �ԍ� */
extern int ifc_menu;										/* ���ݑI�����Ă��� Ifc Menu �� menu �ԍ� */
extern int dev_menu;										/* ���ݑI�����Ă��� Dev Menu �� menu �ԍ� */

extern int if_menu_max;										/* I/F Menu �̍ő�l */
extern int env_menu_max;									/* Env Menu �̍ő吔 */
extern int ifc_menu_max;									/* Ifc Menu �̍ő吔 */
extern int dev_menu_max;									/* Dev Menu �̍ő吔 */

extern int env_select;										/* Env Select Flag */
extern int ifc_select;										/* Ifc Select Flag */
extern int dev_select;										/* Dev Select Flag */

extern char env_usr_name[MAX_LEN];								/* Env �t�@�C����(user name) */
extern char ifc_usr_name[MAX_LEN];								/* Ifc �t�@�C����(user name) */
extern char dev_usr_name[MAX_LEN];								/* Dev �t�@�C����(user name) */
extern char env_usr_name_bak[MAX_LEN];								/* Env �t�@�C����(user name)�o�b�N�A�b�v */
extern char ifc_usr_name_bak[MAX_LEN];								/* Ifc �t�@�C����(user name)�o�b�N�A�b�v */
extern char dev_usr_name_bak[MAX_LEN];								/* Dev �t�@�C����(user name)�o�b�N�A�b�v */

extern int select_dev_device;
extern int select_ifc_device;

/* Inet Information */
extern sceNetCnfEnvData_t env_data __attribute__((aligned(64)));
extern sceNetCnfEnvData_t last_data __attribute__((aligned(64)));

/* Pad ��� */
extern int port;
extern int slot;

/* Rpc ��� */
extern sceNetCnf_t result, result_stat;
extern sceSifClientData cd, cd_stat;
extern u_char sdata[SSIZE] __attribute__((aligned(64)));
extern u_char sdata_stat[SSIZE] __attribute__((aligned(64)));

/* ������� */
extern int disp_char_no;
extern char disp_char[];

/* ���ԏ�� */
extern int ct;

/* �`��� */
extern sceGsDBuff db;
extern int console;

/********************/
/* �֐��v���g�^�C�v */
/********************/
void Gs_Initialize(void);									/* GS ������ */

u_short pad_read(void);										/* �R���g���[���`�F�b�N */
void init_data(sceNetCnfEnvData_t *);								/* �f�[�^�̏����� */
void save_data(void);										/* �f�[�^�̃Z�[�u */
#ifdef DEBUG
void dump_data(sceNetCnfEnvData_t *);								/* �f�[�^�̃_���v */
void list_dump(sceNetCnfList2_t *, int);							/* ���X�g�̃_���v */
#endif /* DEBUG */

void connect_viewer(void);									/* �l�b�g���[�N�ڑ���� */
void menu_viewer(void);										/* �l�b�g���[�N�ݒ��� Menu */
void list_viewer(void);										/* �l�b�g���[�N�ݒ��� List */
void dev_viewer(void);										/* �l�b�g���[�N�ݒ��� Dev */
void ifc_viewer(void);										/* �l�b�g���[�N�ݒ��� Ifc */
void dev_more_viewer(void);									/* �l�b�g���[�N�ݒ��� Dev �ڍ� */
void ifc_more_viewer(void);									/* �l�b�g���[�N�ݒ��� Ifc �ڍ� */

void list_update(int);										/* Env, Ifc, Dev List ���X�V���� */
int change_device(int);										/* ���̃f�o�C�X���������� */
void print_device(int, int);									/* �f�o�C�X���o�͂��� */

int search_char(char);										/* �w�蕶���̈ʒu�� search */
void delete_char(char *, int);									/* �ꕶ���폜 */
void insert_char(char *, int, char, int, int);							/* �ꕶ���}�� */
void write_char(char *, int, char, int);							/* �ꕶ���㏑�� */

void set_return(int, int, int, int, int, int);							/* Return Viewer ��ݒ� */
int get_return(void);										/* Return Viewer ���擾 */

int send_iop(u_int, u_int, u_int, u_int);							/* IOP �փf�[�^�𑗂� */
int dma_check(int);										/* DMA �̏I�����`�F�b�N */

void Rpc_setup(void);										/* Rpc ���̃Z�b�g�A�b�v */
void Rpc_Init(void);										/* Rpc Buffer Initialize */
void Rpc_Init_Stat(void);									/* Rpc Status Buffer Initialize */
int Rpc_Check(void);										/* NETCNF_INTERFACE �֐��̏�Ԃ��`�F�b�N */
int Rpc_Check_Stat(void);									/* NETCNF_STATUS �֐��̏�Ԃ��`�F�b�N */
int Rpc_Get_Result(void);									/* NETCNF_INTERFACE �֐��̕Ԓl���擾 */
int Rpc_Get_Result_Stat(void);									/* NETCNF_STATUS �֐��̕Ԓl���擾 */
void Rpc_sceNetCnfGetCount(int);								/* �w�肵����ނ̃t�@�C�������擾 */
void Rpc_sceNetCnfGetList(sceNetCnfList2_t *, int, int);					/* �w�肵����ނ̃t�@�C�����X�g���擾 */
void Rpc_sceNetCnfLoadEntry(sceNetCnfEnvData_t *, int, char *, int);				/* �w�肵���t�@�C����ǂݍ��� */
void Rpc_sceNetCnfAddEntry(sceNetCnfEnvData_t *, int, char *, int);				/* �ݒ�Ǘ��t�@�C���ɒǉ����A�t�@�C���ɏ����o�� */
void Rpc_sceNetCnfEditEntry(sceNetCnfEnvData_t *, int, char *, char *, int);			/* �ݒ�Ǘ��t�@�C����ҏW���A�t�@�C���ɏ����o�� */
void Rpc_sceNetCnfDeleteEntry(int, char *, int);						/* �ݒ�Ǘ��t�@�C������G���g�����폜���A�t�@�C�����폜 */
void Rpc_sceNetCnfDeleteAll(int);								/* �w�肳�ꂽ�f�o�C�X��̐ݒ��S�č폜 */
void Rpc_sceNetCnfSetConfiguration(int, sceNetCnfEnvData_t *, char *, char *, int, int);	/* netcnf.irx ���Q�Ƃ���ݒ�f�[�^��u�������A�ڑ� */
void Rpc_sceNetCnfDownInterface(void);								/* �ڑ����� */
void Rpc_AllocSysMemory(int);									/* Alloc ���� */
void Rpc_FreeSysMemory(int);									/* Free ���� */
void Rpc_WaitEvent(int);									/* �w�肵�� ID �̃C�x���g��҂� */
void Rpc_GetInterfaceCount(int);								/* Interface List�� ���擾���� */
void Rpc_GetInterfaceList(int, int, int);							/* Interface List ���擾���� */
void Rpc_GetHWAddr(int);									/* �n�[�h�E�F�A�A�h���X���擾���� */
void Rpc_sceNetCnfCopyEntry(int, char *, char *, int, int);					/* �w�肳�ꂽ�G���g����ʃf�o�C�X�� copy ���� */
