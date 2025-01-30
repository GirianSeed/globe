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

//#define NO_HDD				/* HDD �Ȃ� */
//#define NO_HOST				/* HOST �Ȃ� */
//#define MC_NO_DECODE			/* �������[�J�[�h�ɕۑ�����ݒ�t�@�C���� decode ���Ȃ� */
//#define HDD_NO_DECODE			/* �n�[�h�f�B�X�N�ɕۑ�����ݒ�t�@�C���� decode ���Ȃ� */
#define HOST_NO_DECODE			/* �z�X�g�ɕۑ�����ݒ�t�@�C���� decode ���Ȃ� */
#define HOST_READ_ONLY			/* �z�X�g�� Read Only �ɂ��� */

//#define CD_ROM				/* CD-ROM �g�p */
//#define REPLACE				/* ���W���[�����ꊷ�������� */
#ifndef CD_ROM
#define MEM2MB_MODE			/* T10000 �� 2MB ���[�h�ɂ��鎞�Ɏg�p */
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
/* Rpc_NetCnf �֐������E�Ԓl */
/******************************/
typedef struct sceNetCnf {
  int data;				/* return value or other data                               (0x004) */
  int src_addr;				/* source address                                           (0x008) */
  int dst_addr;				/* destination address                                      (0x00c) */
  int size;				/* data size                                                (0x010) */
  int type;				/* �t�@�C���̎�ޓ�                                         (0x014) */
  u_char hwaddr[6];			/* MAC �A�h���X                                             (0x01a) */
  char p0;				/*                                                          (0x01b) */
  char p1;				/*                                                          (0x01c) */
  int device_stat;			/* �f�o�C�X�̏��                                           (0x02c) */
  int no_decode;			/* no_decode �t���O(0: decode ����, 1: decode ���Ȃ�)       (0x020) */
  int no_decode2;			/* no_decode �t���O(0: decode ����, 1: decode ���Ȃ�)       (0x024) */
  int no_decode3;			/* no_decode �t���O(0: decode ����, 1: decode ���Ȃ�)       (0x024) */
  int p4;				/*                                                          (0x030) */
  char sys_name[MAX_LEN];		/* �V�X�e��������ݒ�t�@�C������                         (0x130) */
  char usr_name[MAX_LEN];		/* ���[�U������ݒ�t�@�C������                           (0x230) */
  char usr_name2[MAX_LEN];		/* ���[�U������ݒ�t�@�C������                           (0x330) */
  char dir_name[MAX_LEN];		/* ���΃p�X�����̂��߂̌��ɂȂ�p�X��: �ݒ�Ǘ��t�@�C������ (0x430) */
  char dir_name2[MAX_LEN];		/* ���΃p�X�����̂��߂̌��ɂȂ�p�X��: �ݒ�Ǘ��t�@�C������ (0x530) */
  char dir_name3[MAX_LEN];		/* ���΃p�X�����̂��߂̌��ɂȂ�p�X��: �ݒ�Ǘ��t�@�C������ (0x630) */
} sceNetCnf_t;

/**********************/
/* �ݒ�t�@�C�����X�g */
/**********************/
typedef struct sceNetCnfList2 {
  int select_device;			/* �I������Ă���f�o�C�X                                   (0x004) */
  int flag;				/* �g�p�\����t���O(0:�g�p�s��, 1:�g�p��)                 (0x008) */
  int type;				/* �t�@�C���̎��                                           (0x00c) */
  int stat;				/* �t�@�C���X�e�[�^�X                                       (0x010) */
  char sys_name[MAX_LEN];		/* �V�X�e��������ݒ�t�@�C����                           (0x110) */
  char usr_name[MAX_LEN];		/* ���[�U������ݒ�t�@�C����                             (0x210) */
} sceNetCnfList2_t;

/************************************/
/* �l�b�g���[�N�C���^�t�F�[�X���X�g */
/************************************/
typedef struct sceInetInterfaceList {
  int select_device;			/* �I������Ă���f�o�C�X                                   (0x004) */
  int id;				/* �l�b�g���[�N�C���^�t�F�[�X ID                            (0x008) */
  int type;				/* �l�b�g���[�N�C���^�t�F�[�X�^�C�v                         (0x00c) */
  int mode;				/* ���X�g���[�h(1: ���ݒ�I/F+���ݒ�I/F, 0: ���ݒ�I/F)       (0x010) */
  int flag;				/* �ݒ�σt���O(2: �s��, 1: �ݒ��, 0: ���ݒ�)              (0x014) */
  int p0;				/*                                                          (0x018) */
  int p1;				/*                                                          (0x01c) */
  int p2;				/*                                                          (0x020) */
  char vendor[MAX_LEN];			/* �x���_��                                                 (0x120) */
  char product[MAX_LEN];		/* �v���_�N�g��                                             (0x220) */
} sceInetInterfaceList_t;

/********************/
/* �ݒ�t�@�C�����e */
/********************/
typedef struct sceNetCnfEnvData {
  char attach_ifc[MAX_LEN];		/* �ڑ��v���o�C�_�ݒ�t�@�C����(system name)                (0x0100) */
  char attach_dev[MAX_LEN];		/* �ڑ��@��ݒ�t�@�C����(system name)                      (0x0200) */
  char address[MAX_LEN];		/* IP �A�h���X                                              (0x0300) */
  char netmask[MAX_LEN];		/* �l�b�g�}�X�N                                             (0x0400) */
  char gateway[MAX_LEN];		/* �f�t�H���g���[�^                                         (0x0500) */
  char dns1_address[MAX_LEN];		/* �v���C�}�� DNS                                           (0x0600) */
  char dns2_address[MAX_LEN];		/* �Z�J���_�� DNS                                           (0x0700) */
  char phone_numbers1[MAX_LEN];		/* �ڑ���d�b�ԍ� 1                                         (0x0800) */
  char phone_numbers2[MAX_LEN];		/* �ڑ���d�b�ԍ� 2                                         (0x0900) */
  char phone_numbers3[MAX_LEN];		/* �ڑ���d�b�ԍ� 3                                         (0x0a00) */
  char auth_name[MAX_LEN];		/* ���[�U ID                                                (0x0b00) */
  char auth_key[MAX_LEN];		/* �p�X���[�h                                               (0x0c00) */
  char vendor[MAX_LEN];			/* �x���_��                                                 (0x0d00) */
  char product[MAX_LEN];		/* �v���_�N�g��                                             (0x0e00) */
  char chat_additional[MAX_LEN];	/* �ǉ� AT �R�}���h                                         (0x0f00) */
  char outside_set[MAX_LEN];		/* �O�����M�ԍ��ݒ�                                         (0x1000) */
  char dhcp_host_name[MAX_LEN];		/* DHCP �p�z�X�g��                                          (0x1100) */
  int dialing_type;			/* �_�C�A�����@                                             (0x1104) */
  int type;				/* type(eth,ppp,nic)                                        (0x1108) */
  int phy_config;			/* �C�[�T�l�b�g�ڑ��@��̓��샂�[�h                         (0x110c) */
  int idle_timeout;			/* ����ؒf�ݒ�(�P��(��))                                   (0x1110) */
  u_int p0;				/*                                                          (0x1114) */
  u_int p1;				/*                                                          (0x1118) */
  u_int p2;				/*                                                          (0x111c) */
  u_char dhcp;				/* dhcp(use,no use)                                         (0x111d) */
  u_char pppoe;				/* pppoe(use,no use)                                        (0x111e) */
  u_char p3;				/*                                                          (0x111f) */
  u_char p4;				/*                                                          (0x1120) */
} sceNetCnfEnvData_t;
