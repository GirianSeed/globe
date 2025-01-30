/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                      ATOK Library Sample
 *
 *                         Version 0.10
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.10
 */
/* standard C include file */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

/* sce include file */
#include <eekernel.h>
#include <eeregs.h>
#include <devvif1.h>
#include <devvu1.h>
#include <libdma.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <libgraph.h>
#include <libvu0.h>
#include <libcdvd.h>
#include <libdev.h>
#include <libpkt.h>
#include <sif.h>
#include <sifcmd.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libpad.h>
#include <libmc.h>
#include <libcdvd.h>
#include <libmrpc.h>
#include <libsdr.h>
#include <libscf.h>

#include "pad.h"
#include "kbd.h"
#include <libccc.h>
#include <fontwrap.h>

#include <libfep.h>
#include <libsoftkb.h>
#include <skb.h>


//--------------------------- 
#define USE_USBKB           1 /* USB �L�[�{�[�h���g�p���� */
//---------------------------


#if BOOT_FROM_CD
//--------------------------------------------------------------------------------
// ���W���[�����b�c����ǂޏꍇ�̃p�X
//--------------------------------------------------------------------------------
#define SIO2MAN_IRX     "cdrom0:\\MODULES\\SIO2MAN.IRX;1"
#define PADMAN_IRX      "cdrom0:\\MODULES\\PADMAN.IRX;1"
#define MCMAN_IRX       "cdrom0:\\MODULES\\MCMAN.IRX;1"
#define MCSERV_IRX      "cdrom0:\\MODULES\\MCSERV.IRX;1"
#define DEV9_IRX        "cdrom0:\\MODULES\\DEV9.IRX;1"
#define ATAD_IRX        "cdrom0:\\MODULES\\ATAD.IRX;1"
#define HDD_IRX         "cdrom0:\\MODULES\\HDD.IRX;1"
#define PFS_IRX         "cdrom0:\\MODULES\\PFS.IRX;1"

#define USBD_IRX        "cdrom0:\\MODULES\\USBD.IRX;1"
#define USBKB_IRX       "cdrom0:\\MODULES\\USBKB.IRX;1"


#define JIS2UCS         "cdrom0:\\DATA\\JIS2UCS.BIN;1"
#define UCS2JIS         "cdrom0:\\DATA\\UCS2JIS.BIN;1"
#define PFONT_DAT_PATH  "cdrom0:\\DATA\\SAMPLE.PF;1"


#else /* BOOT_FROM_CD */
//--------------------------------------------------------------------------------
// ���W���[����host����ǂޏꍇ�̃p�X
//--------------------------------------------------------------------------------
#define TOP_DIR         "host0:/usr/local/sce/"
#define IOP_MOD_DIR     TOP_DIR"iop/modules/"

#define SIO2MAN_IRX     IOP_MOD_DIR"sio2man.irx"
#define PADMAN_IRX      IOP_MOD_DIR"padman.irx"
#define MCMAN_IRX       IOP_MOD_DIR"mcman.irx"
#define MCSERV_IRX      IOP_MOD_DIR"mcserv.irx"
#define DEV9_IRX        IOP_MOD_DIR"dev9.irx"
#define ATAD_IRX        IOP_MOD_DIR"atad.irx"
#define HDD_IRX         IOP_MOD_DIR"hdd.irx"
#define PFS_IRX         IOP_MOD_DIR"pfs.irx"

#define USBD_IRX        IOP_MOD_DIR"usbd.irx"
#define USBKB_IRX       IOP_MOD_DIR"usbkb.irx"


#define JIS2UCS         "host0:utf8/libccc/lib/jis2ucs.bin"
#define UCS2JIS         "host0:utf8/libccc/lib/ucs2jis.bin"
#define PFONT_DAT_PATH  "host0:libpfont/sample/basic/sample.pf"

//--------------------------------------------------------------------------------
#endif

//--------------------------------------------------------------------------------
// �`�s�n�j�̃p�X�A�V�X�e�����t�@�C���̃p�X�B
// �g�c�c�t���c�h�r�b�A�V�X�e���\�t�g�E�F�A�̃C���X�g�[���ŁA�C���X�g�[������܂��B
//--------------------------------------------------------------------------------
#define SYSCONF_PARTITION "hdd0:__sysconf"  // Atok�C���X�g�[���ς݃p�[�e�B�V����
#define SYSCONF_DEVICE    "pfs0:"           // (system.ini �̃p�[�e�B�V����������)
#define ATOK_DEVICE       SYSCONF_DEVICE    // 
#define SYSTEM_INI_DEVICE SYSCONF_DEVICE    // 
#define ATOK_SYSDIC_PATH  ATOK_DEVICE"/atok/atokp.dic"    // ATOK�V�X�e�������̃p�X
#define ATOK_USERDIC_PATH ATOK_DEVICE"/atok/atokpyou.dic" // ATOK���[�U�[�����̃p�X
//#define ATOK_USERDIC_PATH "host0:AtokUser.dic" // �f�o�b�O�p ATOK���[�U�[�����̃p�X



//--------------------------------------------------------------------------------
// Gs WorkAddress
//--------------------------------------------------------------------------------
#define SAMPLE_FREE_GSMEM (8960) // 0-8960����ʕ\���Ɏg�p����̂ŁA�ȍ~���󂫗̈�
                                // �P��64���[�h�i256�o�C�g�j
// �󂫗̈���Apfont �� �\�t�g�E�F�A�L�[�{�[�h�Ŏg�p����
// libpfont�Ŏg�p����A�h���X�𒼎w��B������͒P�ʂ�word
#define CLUT_GSMEM       (SAMPLE_FREE_GSMEM*256/4)     // libpfont�pClut�]���A�h���X
#define CLUT_GSMEMSIZE   (64*4)                        // 64���[�h*4
#define FONT_GSMEM       (CLUT_GSMEM + CLUT_GSMEMSIZE) // ���̌���Tex�]���A�h���X��
#define FONT_GSMEMSIZE   (64*64)                       // 64���[�h*64 fontsizeMax 64��64
// �����ă\�t�g�E�F�A�L�[�{�[�h�Ŏg�p����A�h���X�𒼎w��B�P�ʂ�64���[�h�i256�o�C�g�j
#define FONT_GSMEM_END   ((FONT_GSMEM+FONT_GSMEMSIZE)/(256/4)) // �P�ʂ�߂��B
#define SKB_GSMEM        FONT_GSMEM_END  // �\�t�g�E�F�A�L�[�{�[�h�̓]�����[�N�A�h���X
//--------------------------------------------------------------------------------

// VSync���荞�݊֌W
int VSync_HandleID;
volatile int VSync_SemaID;

// �R���g���[�� �c�l�`�o�b�t�@
u_long128 pad_dma_buf[scePadDmaBufferMax]  __attribute__((aligned (64)));
u_long ulPad=0;

// USB �L�[�{�[�h 
int UsbKB_InitFlag=0;
USBKBDATA_t UsbKbData;

// libpccc ���C�u���� �����R�[�h�ϊ��e�[�u���p�o�b�t�@
static sceCccJISCS JisUcs2Tbl[65536];   // jis->ucs �ϊ��e�[�u���p�o�b�t�@
static sceCccUCS2  Ucs2JisTbl[65536];   // ucs->jis �ϊ��e�[�u���p�o�b�t�@

// libpfont���C�u���� �t�H���g�f�[�^�o�b�t�@
#define FONT_DATASIZE  (3 * 1024 * 1024) // �]�T�݂�3MB�A�z��łƂ��Ă��܂��Ă���B
static u_long128 pfont_data[(FONT_DATASIZE+sizeof(u_long128)-1)/sizeof(u_long128)] __attribute__((aligned (128)));

static sceGsDBuff g_db;         // �_�u���o�b�t�@
static sceVif1Packet PkVif1[2]; // �p�P�b�g�̊Ǘ����
static int db_id=0;

char TextBufSjis[1024];
int  TextBufUcs[1024];

//--------------------------------------------------------------------------------
//  irx ���W���[���A���[�h�I�v�V����
//--------------------------------------------------------------------------------
static const char hdd_arg[] = "-o" "\0" "3" "\0" "-n" "\0" "3";
                             // -o �����I�[�v���o�[�e�B�V������(default 10)
                             // -n   �u���b�N�o�b�t�@��(default 3)
static const char pfs_arg[] = "-m" "\0" "3" "\0" "-o" "\0" "10" "\0" "-n" "\0" "10";
                             // -m �����}�E���g��(default 1)
                             // -o �L���b�V���o�b�t�@��(default 8)
                             // -n �����I�[�v���t�@�C����(1�I�[�v���ɃL���b�V���o�b�t�@�g�p1-2)



//--------------------------------------------------------------------------------
//   �L�[�{�[�h�̃Z�b�g�̃��C�A�E�g���`
//--------------------------------------------------------------------------------
#define SKB_OFFSET_X (40)
#define SKB_OFFSET_Y (54)
//�`��\���ʒu
#define TEXT_BOX_X ( 60 - SKB_OFFSET_X )
#define TEXT_BOX_Y ( 62 - SKB_OFFSET_Y )
#define TEXT_BOX_W (500)
#define TEXT_BOX_H ( 50)
#define KEYBORD_X  (TEXT_BOX_X)
#define KEYBORD_Y  (TEXT_BOX_Y+TEXT_BOX_H-2 )
#define FEP_BAR_X  (TEXT_BOX_X+TEXT_BOX_W-80)
#define FEP_BAR_Y  (TEXT_BOX_Y+TEXT_BOX_H-2 )
#define MODE_BAR_RX (TEXT_BOX_X+TEXT_BOX_W-2-12)
#define MODE_BAR_Y  (TEXT_BOX_Y+264)

//�e�d�o�̓��̓��[�h
static u_int FepMode_Kanji[] =
{
	FEPON_HIRA,            // �Ђ炪�ȁA�����ϊ�
	FEPON_ZEN_KATAKANA,    // �J�^�J�i�A�����ϊ�
	FEPON_HAN_KATAKANA,    // ���p�J�^�J�i�ϊ�����
	FEPON_ZEN_ALPHABET,    // �S�p�p����  �ϊ�����
	FEPON_HAN_ALPHABET,    // ���p�p����  �ϊ�����
	FEPOFF                 // �ϊ�����
};

static u_int FepMode_HiKanji[] =
{
	FEPON_HIRA_FIX,           // �Ђ炪�ȁA�����ϊ�����
	FEPON_ZEN_KATAKANA_FIX,   // �J�^�J�i�A�����ϊ�����
	FEPOFF                    // �ϊ�����
};

static u_int FepMode_Off[] =
{
	FEPOFF                    // �ϊ�����
};

//���͋֎~�L�[��`�i�R���g���[���L�[�j
u_int CtrlKeyDisabele_HanCaps[]={ SKBCTRL_KANJI, SKBCTRL_CAPS, SKBCTRL_DOWN, SKBCTRL_UP, SKBCTRL_TAB, 0, };
u_int CtrlKeyDisabele_Zenkaku[]={ SKBCTRL_KANJI,               SKBCTRL_DOWN, SKBCTRL_UP, SKBCTRL_TAB, 0, };
u_int CtrlKeyDisabele_Caps[]   ={                SKBCTRL_CAPS, SKBCTRL_DOWN, SKBCTRL_UP, SKBCTRL_TAB, 0, };
u_int CtrlKeyDisabele_UpDown[] ={                              SKBCTRL_DOWN, SKBCTRL_UP, SKBCTRL_TAB, 0, };


// < �T���v�� �L�[�{�[�h��` ���� >
//   �\�t�g�E�F�A�L�[�{�[�h�́A�Z���N�g�{�^���ŁA���̃L�[�{�[�h�ɑ���A�J��Ԃ����ƂŃ��[�v���܂��B
//   KEYBOARD_SET�\���̂́A�L�[�{�[�h�Z�b�g�Ƃ��Ă������`����\���̂ł��B
//
//   �\�t�g�E�F�A�L�[�{�[�h�B�Z���N�g�{�^���ŁA���̃L�[�{�[�h�ɑ���A�J��Ԃ����ƂŃ��[�v���܂��B
//     �f�t�H���g�̃L�[�{�[�h : �ŏ��ɕ\�������L�[�{�[�h�ł��B
//     ���̃L�[�{�[�h         : �Z���N�g�{�^���ő���L�[�{�[�h���w���A�g�p����L�[�{�[�h�Ń��[�v������f�[�^�Ƃ��܂��B
//     Fep�̓��̓��[�h        : Fep�̓��̓��[�h�̓L�[�{�[�h���ƂɌŒ肳��Ă܂����AFEPOFF�w�肪�ł��܂��B�܂��A
//                              �I�[�v������͂��̎w����g�p���Ă��܂��̂ŁA�L�[�{�[�h�ƃ��b�N��Ԃɍ���Ďw�肵�܂��B
//     ���b�N�̏�����Ԃ̎w�� : �S�p�L�[�A�L���v�X�L�[�����̃L�[�̂Ȃ��L�[�{�[�h�͎w��o���܂���B
//     �g�p�s�R���g���[���L�[�̎w��F SKBCTRL_xxx �Ŏw��Bnull �^�[�~�l�[�gu_int �z��|�C���^�B
//     �\�t�g�E�F�A�L�[�{�[�h�\���ʒu�F �L�[�{�[�h�̕\���ʒu�B
//   FEP�o�[�p��`�B�i�t�r�a�L�[�{�[�h�ł̓��͎��ɕ\�������B�j
//     FEP�̓��̓��[�h : �g�p����e�d�o�̓��̓��[�h���`�B�Z���N�g�{�^���AF12�ŁA���̏��ԂŃ��[�h���[�v�B
//     FEP�o�[�\���ʒu : ����
//   ���[�h�o�[�p��`�B�I�[�v���L�[�{�[�h��̈ꗗ�\���o�[�B�\�t�g�E�F�A�L�[�{�[�h�Ɠ�������B
//     ���[�h�o�[�̒�`�F�w��s�v�B�\�t�g�E�F�A�L�[�{�[�h�̒�`���玩�������B
//     ���[�h�o�[�̈ʒu�F����
//   �e�L�X�g�{�b�N�X�p��`�B
//     �e�L�X�g�{�b�N�X�̈ʒu�ƃT�C�Y�F�e�L�X�g�t�B�[���h�̈ʒu�ƃT�C�Y�B

// �񊿎��p�L�[�{�[�h��`
KEYBOARD_SET KeyBoardSet_HiKanji = {
	SKBTYPE_HIRA,// �Ђ炪�ȃL�[�{�[�h���f�t�H���g�Ƃ���B
	{
	/*              ���̃L�[�{�[�h      Fep�̓��̓��[�h   �������b�N���  �g�p�s�R���g���[���L�[  �\�����W */
	/* �Ђ炪��   */{ SKBTYPE_KANA  , FEPOFF            , LAYOUT_ZENKAKU, CtrlKeyDisabele_HanCaps, { KEYBORD_X,KEYBORD_Y } },
	/* �J�^�J�i   */{ SKBTYPE_QWERTY, FEPOFF            , LAYOUT_ZENKAKU, CtrlKeyDisabele_HanCaps, { KEYBORD_X,KEYBORD_Y } },
	/* �`�a�b��   */{ SKBTYPE_NUM   , FEPOFF            , 0             , CtrlKeyDisabele_Zenkaku, { KEYBORD_X,KEYBORD_Y } },
	/* �L�[�{�[�h */{ SKBTYPE_NUM   , FEPOFF            , 0             , CtrlKeyDisabele_Zenkaku, { KEYBORD_X,KEYBORD_Y } },
	/* ����       */{ SKBTYPE_HIRA  , FEPOFF            , 0             , CtrlKeyDisabele_Zenkaku, { KEYBORD_X,KEYBORD_Y } },
	/* �L��       */{ SKBTYPE_HIRA  , FEPOFF            , 0             , 0                      , { KEYBORD_X,KEYBORD_Y } },
	// �L���͎g�p���Ă��Ȃ��B
	},
	/*           Fep�̓��̓��[�h��             Fep�̓��̓��[�h        �\�����W              */
	{ sizeof( FepMode_HiKanji )/sizeof(u_int), FepMode_HiKanji,  { FEP_BAR_X, FEP_BAR_Y }},
	{ 0,{0},{0},    { MODE_BAR_RX-24*4, MODE_BAR_Y }},
	{ { TEXT_BOX_X, TEXT_BOX_Y, TEXT_BOX_W, TEXT_BOX_H } },
};


// �A���t�@�x�b�g�Ɛ����L�[�{�[�h�p�̒�`
KEYBOARD_SET KeyBoardSet_ABC123 = {
	SKBTYPE_QWERTY, // �L�[�{�[�h�z����f�t�H���g�Ƃ���B
	{
	/*              ���̃L�[�{�[�h      Fep�̓��̓��[�h   �������b�N���  �g�p�s�R���g���[���L�[  �\�����W */
	/* �Ђ炪��   */{ SKBTYPE_QWERTY, FEPOFF            , LAYOUT_ZENKAKU, CtrlKeyDisabele_HanCaps, { KEYBORD_X,KEYBORD_Y } },
	/* �J�^�J�i   */{ SKBTYPE_QWERTY, FEPOFF            , LAYOUT_ZENKAKU, CtrlKeyDisabele_HanCaps, { KEYBORD_X,KEYBORD_Y } },
	/* �`�a�b��   */{ SKBTYPE_NUM   , FEPOFF            , 0             , CtrlKeyDisabele_Zenkaku, { KEYBORD_X,KEYBORD_Y } },
	/* �L�[�{�[�h */{ SKBTYPE_NUM   , FEPOFF            , 0             , CtrlKeyDisabele_Zenkaku, { KEYBORD_X,KEYBORD_Y } },
	/* ����       */{ SKBTYPE_QWERTY, FEPOFF            , 0             , CtrlKeyDisabele_Zenkaku, { KEYBORD_X,KEYBORD_Y } },
	/* �L��       */{ SKBTYPE_QWERTY, FEPOFF            , 0             , 0                      , { KEYBORD_X,KEYBORD_Y } },
	// �Ђ炪�� �J�^�J�i �L���́A�g�p���Ă��Ȃ��B
	},
	/*     Fep�̓��̓��[�h��                   Fep�̓��̓��[�h        �\�����W              */
	{ sizeof( FepMode_Off )/sizeof(u_int),        FepMode_Off, { FEP_BAR_X, FEP_BAR_Y }},
	{ 0,{0},{0},    { MODE_BAR_RX-24*2, MODE_BAR_Y }},
	{ { TEXT_BOX_X, TEXT_BOX_Y, TEXT_BOX_W, TEXT_BOX_H } },
};


// �����L�[�{�[�h�̂ݗp�̒�`
KEYBOARD_SET KeyBoardSet_123 = {
	SKBTYPE_NUM, // �����L�[�{�[�h���f�t�H���g�Ƃ���B
	{
	/*              ���̃L�[�{�[�h      Fep�̓��̓��[�h   �������b�N���  �g�p�s�R���g���[���L�[  �\�����W */
	/* �Ђ炪��   */{ SKBTYPE_NUM   , FEPOFF            , LAYOUT_ZENKAKU, CtrlKeyDisabele_HanCaps, { KEYBORD_X,KEYBORD_Y } },
	/* �J�^�J�i   */{ SKBTYPE_NUM   , FEPOFF            , LAYOUT_ZENKAKU, CtrlKeyDisabele_HanCaps, { KEYBORD_X,KEYBORD_Y } },
	/* �`�a�b��   */{ SKBTYPE_NUM   , FEPOFF            , 0             , CtrlKeyDisabele_Zenkaku, { KEYBORD_X,KEYBORD_Y } },
	/* �L�[�{�[�h */{ SKBTYPE_NUM   , FEPOFF            , 0             , CtrlKeyDisabele_Zenkaku, { KEYBORD_X,KEYBORD_Y } },
	/* ����       */{ SKBTYPE_NUM   , FEPOFF            , 0             , CtrlKeyDisabele_Zenkaku, { KEYBORD_X,KEYBORD_Y } },
	/* �L��       */{ SKBTYPE_NUM   , FEPOFF            , 0             , 0                      , { KEYBORD_X,KEYBORD_Y } },
	// �����ȊO�́A�g�p���Ă��Ȃ��B
	},
	/*     Fep�̓��̓��[�h��                   Fep�̓��̓��[�h        �\�����W              */
	{ sizeof( FepMode_Off )/sizeof(u_int),        FepMode_Off , { FEP_BAR_X, FEP_BAR_Y }},
	{ 0,{0},{0},    { MODE_BAR_RX-24*1, MODE_BAR_Y }},
	/*  �e�L�X�g�{�b�N�X  �w�x  �v�g                      */
	{ { TEXT_BOX_X, TEXT_BOX_Y, TEXT_BOX_W, TEXT_BOX_H } },
};

#ifdef SAMPLE_REGION_J
// ���ׂẴL�[�{�[�h�p��`�i�������͂��Aýėp�j
KEYBOARD_SET KeyBoardSet_All = {
	SKBTYPE_HIRA,// �Ђ炪�ȃL�[�{�[�h���f�t�H���g�Ƃ���B
	{
	/*              ���̃L�[�{�[�h    Fep�̓��̓��[�h     �������        �g�p�s�R���g���[���L�[  �\�����W */
	/* �Ђ炪��   */{ SKBTYPE_KANA  , FEPON_HIRA        , LAYOUT_ZENKAKU, CtrlKeyDisabele_HanCaps, { KEYBORD_X,KEYBORD_Y } },
	/* �J�^�J�i   */{ SKBTYPE_QWERTY, FEPON_ZEN_KATAKANA, LAYOUT_ZENKAKU, CtrlKeyDisabele_Caps   , { KEYBORD_X,KEYBORD_Y } },
	/* �`�a�b��   */{ SKBTYPE_NUM   , FEPOFF            , 0             , CtrlKeyDisabele_UpDown , { KEYBORD_X,KEYBORD_Y } },
	/* �L�[�{�[�h */{ SKBTYPE_NUM   , FEPOFF            , 0             , CtrlKeyDisabele_UpDown , { KEYBORD_X,KEYBORD_Y } },
	/* ����       */{ SKBTYPE_SIGN  , FEPOFF            , 0             , CtrlKeyDisabele_UpDown , { KEYBORD_X,KEYBORD_Y } },
	/* �L��       */{ SKBTYPE_HIRA  , FEPOFF            , 0             , 0                      , { KEYBORD_X,KEYBORD_Y } },
	},
	/*           Fep�̓��̓��[�h��                Fep�̓��̓��[�h        �\�����W            */
	{ sizeof( FepMode_Kanji )/sizeof(u_int),      FepMode_Kanji,  { FEP_BAR_X, FEP_BAR_Y }},
	{ 0,{0},{0},    { MODE_BAR_RX-24*5, MODE_BAR_Y }},
	{ { TEXT_BOX_X, TEXT_BOX_Y, TEXT_BOX_W, TEXT_BOX_H } },
};
#else
// ���ׂẴL�[�{�[�h�p��`�i���e�������A���t�@�x�b�g�i�v�t�H���g�T�|�[�g�jýėp�j
KEYBOARD_SET KeyBoardSet_All_AE = {
	SKBTYPE_QWERTY,// �L�[�{�[�h�z����f�t�H���g�Ƃ���B
	{
	/*              ���̃L�[�{�[�h      Fep�̓��̓��[�h       �������    �g�p�s�R���g���[���L�[  �\�����W */
	/* �Ȃ�       */{ SKBTYPE_ABC   , FEPOFF            , LAYOUT_ZENKAKU, CtrlKeyDisabele_UpDown , { KEYBORD_X,KEYBORD_Y } },
	/* �Ȃ�       */{ SKBTYPE_ABC   , FEPOFF            , LAYOUT_ZENKAKU, CtrlKeyDisabele_UpDown , { KEYBORD_X,KEYBORD_Y } },
	/* Latin�p��  */{ SKBTYPE_NUM   , FEPOFF            , 0             , CtrlKeyDisabele_UpDown , { KEYBORD_X-10,KEYBORD_Y } },
	/* �L�[�{�[�h */{ SKBTYPE_ABC   , FEPOFF            , 0             , CtrlKeyDisabele_UpDown , { KEYBORD_X,KEYBORD_Y } },
	/* ����       */{ SKBTYPE_SIGN  , FEPOFF            , 0             , CtrlKeyDisabele_UpDown , { KEYBORD_X,KEYBORD_Y } },
	/* Latin�L��  */{ SKBTYPE_QWERTY, FEPOFF            , 0             , CtrlKeyDisabele_UpDown , { KEYBORD_X,KEYBORD_Y } }, 
	// �Ђ炪�ȁA�J�^�J�i�A�g�p���Ă��Ȃ��B
	},
	/*           Fep�̓��̓��[�h��                Fep�̓��̓��[�h        �\�����W            */
	{ sizeof( FepMode_Off )/sizeof(u_int),     FepMode_Off,  { FEP_BAR_X, FEP_BAR_Y }},
	{ 0,{0},{0},    { MODE_BAR_RX-24*4, MODE_BAR_Y }},
	{ { TEXT_BOX_X, TEXT_BOX_Y, TEXT_BOX_W, TEXT_BOX_H } },
};
#endif

//--------------------------------------------------------------------------------
// �����R�[�h�ϊ�
//--------------------------------------------------------------------------------
// Sjis -> Ucs ������ւ̕ϊ�
int SjisToUcs4(const char *pSjis, u_int* ucs4, u_int buffMax )
{
	int i = 0;
	sceCccJISCS	jis;
	sceCccUCS4 chr=0;
	sceCccSJISTF *pPtr = (sceCccSJISTF *)pSjis;

	if( (!ucs4) || buffMax==0 )	return -1;

	while( *pPtr )
	{
		if( i < buffMax-1 ){
			jis = sceCccDecodeSJIS((sceCccSJISTF const **)&pPtr); // SJIS -> JIS
			ucs4[i] = sceCccJIStoUCS(jis, chr);                   // JIS  -> UCS
			i++;
		}
		else{
			ucs4[i] = 0;
			return -i;
		}
	}
	ucs4[i] = 0;

	return i;
}

// Ucs -> Sjis ������ւ̕ϊ�
static void UcsToSjis( u_int* ucs4, unsigned char *sjis, u_int buffMax )
{
	u_char utf8char[12] = "";
	sceCccUTF8 *pUtf8;       // UTF8-Buffer�擪(Pointer�i�[�p)
	sceCccUTF8**ppUtf8 = &pUtf8; // ���ϊ��擪�ʒu
	int len;

	if( *ucs4 == 0 ){
		if( buffMax )	*sjis = 0;
		return;
	}

	while( *ucs4 && buffMax >= 0 ){
		pUtf8  = utf8char; // UTF8-Buffer�擪(Pointer�i�[�p)
		sceCccEncodeUTF8( ppUtf8, *ucs4 ); // UCS4 -> UTF8�ϊ�
		ucs4++;                            
		**ppUtf8 = 0;

		sceCccUTF8toSJIS( sjis, buffMax, utf8char );
		len = strlen(sjis);
		buffMax-=len;
		sjis   +=len;
	}
}


//--------------------------------------------------------------------------------
// VBlankStart ���荞�݃n���h��
//--------------------------------------------------------------------------------
static int InterlaceField=0;
static int VBlankStartHandler(int ch)
{
	struct SemaParam sp;

	InterlaceField^=1;
	iReferSemaStatus( VSync_SemaID, &sp );

	/* �J�E���g��0�̂Ƃ��̂݃V�O�i������ */
	if( sp.currentCount == 0 ){
	    iSignalSema(VSync_SemaID);
	}
	PadVSyncCount();

	ExitHandler(); /* ���荞�݃n���h���I��               */
	return -1;     /* �Ȍ�̊��荞�݃n���h���͋N�����Ȃ� */
}

//--------------------------------------------------------------------------------
// �d���R���g���[��
//--------------------------------------------------------------------------------
#define STACKSZ (512 * 16)
int PowerOff_ThreadID;
int PowerOff_SemaID;
int (*GetWaitPowerOff)(void) = ( int(*)(void) )0;
int PowerOff_Start=0;
int PowerOff_Device=0;
static u_char stack[STACKSZ] __attribute__ ((aligned(16)));

static void PowerOffThread(void *arg)
{
	int sid = (int)arg;
	int stat = 0;
	int waitPowerOff_SemaID=-1;

	WaitSema(sid); /* �N����A�����ɑ҂��ɓ��� */

	printf("power off request has come.\n");
	sceCdPOffCallback( 0, (void *)0 );  /* �d�����Ƃ��̂ŁA�Q�x�Ăт����Ȃ��B*/

	PowerOff_Start=1;

	if( GetWaitPowerOff )	waitPowerOff_SemaID = GetWaitPowerOff();
	else                 	waitPowerOff_SemaID = -1;
	if( waitPowerOff_SemaID >= 0 )
		WaitSema(waitPowerOff_SemaID);  /*  �\�t�g�E�F�A���ŏI������������ꍇ�͑҂� */

	/* �g���C���J�����܂܁A�d���������Ȃ��悤�ɁA�b�c�g���C�N���[�Y�B*/
	sceCdTrayReq(SCECdTrayClose, &stat);

	/* dev9 power off, need to power off PS2 */
	if( PowerOff_Device ){
		/* close all files */
		sceDevctl("pfs:", PDIOC_CLOSEALL, NULL, 0, NULL, 0);
	}
	/* ���C�u�����o�[�W���� 2.53 ����Adev9x �̓d���n�e�e�ŁA�g�c�c�̓d�����n�e�e����܂��B*/
	while( sceDevctl("dev9x:", DDIOC_OFF    , NULL, 0, NULL, 0) < 0 );

	/* �O�̂��߁A�Ō�ɂb�c�g���C�N���[�Y�B*/
	sceCdTrayReq(SCECdTrayClose, &stat);

	/* PS2 power off */
	while(!sceCdPowerOff(&stat)||stat);
	printf("power off ps2\n");

	for(;;); /* ���̃X���b�h�ɏ�����n���Ȃ� */

	return;
}

static void PowerOffHandler(void *arg)
{
	int sid = (int)arg;

	iSignalSema(sid); // PowerOffThread�̑҂�������
}

static void CreatePowerOffThread( int prio )
{
	struct ThreadParam tparam;
	struct SemaParam   sparam;

	sparam.initCount = 0;
	sparam.maxCount  = 1;
	sparam.option    = 0;
	PowerOff_SemaID = CreateSema( &sparam );

	tparam.stackSize = STACKSZ;
	tparam.gpReg = &_gp;
	tparam.entry = PowerOffThread;
	tparam.stack = (void *)stack;
	tparam.initPriority = prio;
	PowerOff_ThreadID = CreateThread(&tparam);

	StartThread( PowerOff_ThreadID, (void *)PowerOff_SemaID );

	sceCdPOffCallback( PowerOffHandler, (void *)PowerOff_SemaID );
}

//--------------------------------------------------------------------------------
// irx ���W���[�����[�h�֐�
//--------------------------------------------------------------------------------
// ���[�h�����܂ŁA�������[�v
int sifLoadModule( const char *module, int args, const char *argp )
{
	int ret;

	for(;;){
		ret = sceSifLoadModule( module, args , argp );

		if( ret < 0 ){
			printf( "Can't load module [ %s ]:%d\n",module,-ret );
			continue;
		}
		break;
	}
	return ret;
}

// ���[�h���s���A�܂��́A irx ���A�풓���Ȃ��ŏI�������ꍇ�́A
// printf�o�͂��āA�}�C�i�X�l��Ԃ��B
int _sifLoadStartModule( const char *module, int args, const char *argp )
{
	int ret,result;

	ret = sceSifLoadStartModule( module, args, argp, &result );
	if( ret < 0 ){
		printf( "Can't load module [ %s ]:%d\n",module,-ret );
		return ret;
	}
	if( result == NO_RESIDENT_END ){
		printf( "module [ %s ]:NO_RESIDENT_END\n",module );
		return -0x10000000; // ���̒l�Ƃ��āA���ʉ\�Ȓl��Ԃ��Ă���B
	}
	return ret;
}


static int skbStep=0;
#define STEP_SKB_NONE   ( 0)
#define STEP_SKB_INIT   ( 1)
#define STEP_SKB_OPENED ( 2)
#define STEP_SKB_CLOSE  (-1)
#define STEP_SKB_EXIT   (-2)

// �T���v���\�t�g�E�F�A�L�[�{�[�h�A�R�[���o�b�N�֐��B
static int MessageFromSKB( int messType, void * p )
{
	/* ���b�Z�[�W�ʂ� ���� */
	switch( messType ){
	/* --- �ҏW���ʂ��A�擾���ăN���[�Y -------------------------------------*/
	case SKBMES_RESULT   :/* �e�L�X�g�ҏW���ʕ� */
	                        if( p ){
	                        	UcsToSjis( (u_int*)p, TextBufSjis, 1024 );
	                        }
	                        skbStep = STEP_SKB_CLOSE;
	                        break;
	/* --- ��ʉ����̑���K�C�h�̕ύX ---------------------------------------*/
	case SKBMES_USBKEY_ON:/* USBKEYBORD�ɐ؂�ւ��� */
	                        break;

	case SKBMES_HELP_SHOW:/* Help��ʕ\���� */
	                        break;
	case SKBMES_FEP_ON   :/* �ϊ��L����     */
	                        break;

	case SKBMES_USBKEY_OFF:/* ���KEYBORD�ɐ؂�ւ��� */
	case SKBMES_HELP_HIDE :/* Help��ʕ\�������� */
	case SKBMES_FEP_OFF   :/* FEP_Off�� */
	                        break;
	/* -------- ���ʉ� --------------------------------------------- */
	case SKBMES_SE        :/* ���ʉ��������^�C�~���O�� */
	                        break;
	}
	return 0;
}


//--------------------------------------------------------------------------------
//  ���C��
//--------------------------------------------------------------------------------
int main()
{
	static u_int use_hdd = 0;
	int hddDeviceFlag=0;
	int ret = 0;
	int i;

	sceSifInitRpc(0);

	#if BOOT_FROM_CD
	sceCdInit(SCECdINIT);
	while( sceSifRebootIop( "cdrom0:\\MODULES\\"IOP_IMAGE_FILE";1" ) == 0 );
	#else
	while( sceSifRebootIop( IOP_MOD_DIR IOP_IMAGE_file ) == 0 );
	#endif /* BOOT_FROM_CD */

	while( sceSifSyncIop() == 0 );
	sceSifInitRpc(0);
	sceSifLoadFileReset();

	#if BOOT_FROM_CD
	sceCdInit(SCECdINIT);
	sceCdMmode(SCECdCD);
	#endif /* BOOT_FROM_CD */
	sceFsReset();
	sceSifInitIopHeap();

	/* iop���W���[���̃��[�h ----------------------------------------*/
	/* �R���g���[���A�������[�J�[�h�֌W */
	sifLoadModule( SIO2MAN_IRX , 0                   , NULL         );
	sifLoadModule( PADMAN_IRX  , 0                   , NULL         );
	sifLoadModule( MCMAN_IRX   , 0                   , NULL         );
	sifLoadModule( MCSERV_IRX  , 0                   , NULL         );

	/* �n�[�h�f�B�X�N�h���C�u�֌W */
	ret = _sifLoadStartModule( DEV9_IRX, 0 , NULL );
	if( ret >= 0 ){
		if( _sifLoadStartModule( ATAD_IRX, 0 , NULL ) >= 0 ){
			if( _sifLoadStartModule( HDD_IRX , sizeof(hdd_arg) , hdd_arg ) >= 0 ){
				hddDeviceFlag = 1; // �n�[�h�f�B�X�NOK
				if( _sifLoadStartModule( PFS_IRX , sizeof(pfs_arg) , pfs_arg ) >= 0 ){
					use_hdd = 1;
				}
			}
		}
	}

	sifLoadModule( USBD_IRX    , 0                   , NULL         );
	#if USE_USBKB
    sifLoadModule( USBKB_IRX   , 0                   , NULL         );
	#endif
	/*----------------------------------------------------------------*/


	sceDmaReset(1);

	// BCOND0�ݒ�
	{
		sceDmaEnv env;
		
		sceDmaGetEnv(&env);							// DMA���ʊ����擾
		env.notify = 1<<SCE_DMA_VIF1;				// VIF�`���l���̂ݒʒm
		sceDmaPutEnv(&env);							// DMA���ʊ���ݒ�
		((volatile tD_CHCR *)D1_CHCR)->TTE = 1;		// VIF1�ɑ΂���DMA�^�O�����o
	}

	sceGsResetPath();

	/* �R���g���[���A������ */
	scePadInit(0);
	scePadPortOpen(0, 0, pad_dma_buf);


	/* Libccc������ -------------------------------------------------*/
	{
		int fd;

		for(;;){
			fd = sceOpen( JIS2UCS, SCE_RDONLY ); // jis->ucs �ϊ��e�[�u�����[�h
			if(0 > fd){
				printf("sceOpen(%s, SCE_RDONLY) failed\n", JIS2UCS);
				continue;
			}
			sceRead(fd,  JisUcs2Tbl, sizeof( JisUcs2Tbl ));
			sceClose(fd);
			break;
		}

		for(;;){
			fd = sceOpen( UCS2JIS, SCE_RDONLY ); // ucs->jis �ϊ��e�[�u�����[�h
			if(0 > fd){
				printf("sceOpen(%s, SCE_RDONLY) failed\n", UCS2JIS);
				continue;
			}
			sceRead(fd,  Ucs2JisTbl, sizeof(Ucs2JisTbl) );
			sceClose(fd);
			break;
		}
	}
	sceCccSetTable( JisUcs2Tbl, Ucs2JisTbl );
	/*----------------------------------------------------------------*/


	/* �t�H���g�f�[�^�ǂ݂��݁�LibPFont ������ -----------------------*/
	{
		int fd;

		for(;;){
			fd = sceOpen( PFONT_DAT_PATH, SCE_RDONLY );
			if(0 > fd){
				printf("sceOpen(FONTPATH, SCE_RDONLY) failed\n");
				continue;
			}
			sceRead(fd, pfont_data, FONT_DATASIZE);
			sceClose(fd);
			break;
		}

		Font_Init();                                    // �t�H���g������
		Font_Load( CLUT_GSMEM, FONT_GSMEM, pfont_data ); // �t�H���g�Z�b�g�A�b�v
	}
	/*---------------------------------------------------------------*/

	if( hddDeviceFlag )
	{
		int res = sceDevctl("hdd0:", HDIOC_STATUS, NULL, 0, NULL, 0);

		switch( res ){
		case 0: printf("HDD ok.\n");                  break; // ����I���B
		case 1: printf("HDD driver unformat.\n");     break; // HDD�h���C�u���A���t�H�[�}�b�g������(��UD�ŏ��������Ă�)�B
		case 2: printf("HDD reserved.\n");            break; // �i�\��)
		case 3: printf("HDD not connected.\n");       break; // HDD�h���C�u���ڑ�����Ă��Ȃ������B
		case -19: printf(" 'hdd0:' device error\n");  break; // �w�肳�ꂽ�f�o�C�X�����݂��Ȃ��B
		case -24: printf(" cannnot open.\n");         break; // �I�[�v���ł���f�B�X�N���v�^�̍ő吔�ɒB���Ă���B
		}

		if( res != 3 )	PowerOff_Device=1;	// �d���n�e�e���ɁAHDD�̓d����؂�K�v����B
		if( res == 0 ){
			// __sysconf�p�[�e�B�V�������}�E���g����
			res = sceMount( SYSCONF_DEVICE, SYSCONF_PARTITION, 0, NULL, 0);
			if(res<0) {
				printf("mount error: %d\n", res);
			}
			{// HDD�� �A�C�h�������Ȃ��ݒ�ɁB
				u_char standbytimer = 0x00;
				sceDevctl("hdd0:", HDIOC_IDLE, &standbytimer, sizeof(char), NULL, 0);
			}
		}
	}

	ChangeThreadPriority(GetThreadId(), 4);	//�d������X���b�h�̃X�^���o�C�܂ŁA�v���C�I���e�B���ꎞ������B
	CreatePowerOffThread( 3 ); // �d���Ւf�X���b�h���쒆���A�`��X�V���s�����߁A
	                           // ���C���X���b�h���A�Ⴂ�v���C�I���e�B�ɐݒ�B
	                           // �d���Ւf�X���b�h���쒆�́A�b�c�A�g�c�c�A�N�Z�X���Ȃ��悤�ɂ��܂��B
	ChangeThreadPriority(GetThreadId(), 1);

	// �_�u���o�b�t�@�̐ݒ�
	sceGsSetDefDBuff(&g_db, SCE_GS_PSMCT32, 640, 224,
							SCE_GS_ZGEQUAL, SCE_GS_PSMZ24, SCE_GS_CLEAR);

	// �_�u���o�b�t�@�̃N���A�J���[��ݒ�
	*(u_long *)(void *)&g_db.clear0.rgbaq = SCE_GS_SET_RGBAQ(0x00, 0x20, 0x40, 0x00, 0);
	*(u_long *)(void *)&g_db.clear1.rgbaq = SCE_GS_SET_RGBAQ(0x00, 0x20, 0x40, 0x00, 0);


    // VIF1�p�P�b�g�o�b�t�@���m��
	// VIF1�p�P�b�g�̏�����
	for(i=0; i<2; i++) {
		u_long128 *p;
		p = (u_long128 *)memalign(128, 128*20); // �v���~�e�B�u�o�b�t�@���m��
		sceVif1PkInit(&PkVif1[i], p);           // sceVif1Packet�\���̂�������
		sceVif1PkReset(&PkVif1[i]);             // sceVif1Packet�\���̂����Z�b�g
		sceVif1PkEnd(&PkVif1[i], 0);            // �����v���~�e�B�u�Ƃ���DMAend�^�O��ǉ�
		sceVif1PkTerminate(&PkVif1[i]);         // DMAend�^�O���I�[
	}


	/*---------------------------------------------------------------*/
	{
		SKB_ARG skbArg;
		int kbType,repWait,repSpeed,region;

		#ifdef SAMPLE_REGION_J
		skbArg.region = REGION_J; // ���{��PS2
		#else
			#ifdef SAMPLE_REGION_E
		skbArg.region = REGION_E; // �C�OPS2(���Ή�)
			#else
		skbArg.region = REGION_A; // �C�OPS2
			#endif
		#endif
		skbArg.AtokPath.system = ATOK_DEVICE;
		skbArg.AtokPath.sysDic = ATOK_SYSDIC_PATH;
		skbArg.AtokPath.usrDic = ATOK_USERDIC_PATH;

		skbArg.Gs_workAddress256 = SKB_GSMEM;
		skbArg.memAlign = memalign;
		skbArg.memFree  = free;

		// �\�t�g�E�F�A�L�[�{�[�h�̊��������B�V�X�e�����t�@�C���̂���f�o�C�X��
		//                                �t�@�C�������������ꍇ�̃f�t�H���g�������B
		Init_SKBConfig( &skbArg, SYSTEM_INI_DEVICE, 1, KEYREPW_MIDDLE, KEYREPS_MIDDLE );
		// �V�X�e�����̎擾�B
		SKB_GetKbdConfig( &kbType, &repWait, &repSpeed, &region );
		
		// USB�L�[�{�[�h���C�u�����̏�����
		Kbd_Init( kbType, repWait, repSpeed, region );
		Kbd_SetConfig();    // �L�[�{�[�h�ݒ�
		UsbKB_InitFlag = 1; // UsbKB����̏��������ς܂������Ƃ��A�L�^���Ă����B
	}

	// �\�t�g�E�F�A�L�[�{�[�h������
	GetWaitPowerOff = SKB_Init();  // �Ԃ�l�́A�g�c�c�A�N�Z�X���m�ۂ����Z�}�t�H�h�c��Ԃ��A�֐��B
	                               // �d���R���g���[�������ŁA�g�p�B
	/* �\�t�g�E�F�A�L�[�{�[�h����̃��b�Z�[�W���󂯎��֐���ݒ肵�Ă��� */
	SKB_SetCallback( MessageFromSKB ); /* �r�d���N�G�X�g��A�ҏW���ʂ̎󂯎��p */
	skbStep = STEP_SKB_INIT;

	strcpy( TextBufSjis, "1234");

	/*---------------------------------------------------------------*/
	InterlaceField = sceGsSyncV(0)^1;

	/* v-blank �J�n��҂Z�}�t�H��ݒ� */
	{
		struct SemaParam sp;

		sp.initCount = 0;
		sp.maxCount = 1;
		VSync_SemaID = CreateSema(&sp);
	}
	/* v-blank �J�n�̊��荞�݃n���h���o�^ */
	VSync_HandleID = AddIntcHandler(INTC_VBLANK_S, VBlankStartHandler, 0 );
	/* v-blank ���荞�݋��� */
	EnableIntc(INTC_VBLANK_S);

	/*---------------------------------------------------------------*/

	while(1){
		sceVif1Packet* pkVif1;
		u_long128*     pkSkb;
		int padOn, kbStat;

		/* ���͏��擾 */
		ulPad = PadGetKey(0);           // �p�b�h���擾

		padOn = ((ulPad>>32)&0xFFFFU);  // �{�^�� On �g���K
		if( UsbKB_InitFlag ) kbStat = Kbd_Main(&UsbKbData); // �L�[�{�[�h���擾
		else                 kbStat = -1;

		pkSkb = (u_long128*)0;
		/* �p���[�n�e�e�������A���C���X���b�h���A�v���C�I���e�B�̒Ⴂ�X���b�h�ɂ��Ă���̂� */
		/* �p���[�n�e�e�X���b�h���쒆�ɁA�f�o�C�X�A�N�Z�X�Ȃǂ��N�������Ȃ��悤�ɂ���         */
		if(! PowerOff_Start){
			switch(skbStep){
			case STEP_SKB_CLOSE:
			        SKB_Close(0);
			        skbStep = STEP_SKB_EXIT;	//Exit�܂ł��
			        //skbStep = STEP_SKB_INIT;	//Close�܂�
			        break;
			case STEP_SKB_EXIT:
			        SKB_Exit();
			        GetWaitPowerOff = ( int(*)(void) )0;
			        skbStep = STEP_SKB_NONE;
			        break;
			case STEP_SKB_NONE :
			case STEP_SKB_INIT :
			        if( padOn & SCE_PADRright ){
			        	#define MAX_INHIBIT_UCS4        (100)       // ���͋��֎~�������ő吔
			        	u_int inhibitWorkBuf[MAX_INHIBIT_UCS4][10]; // ���͋֎~�����p�o�b�t�@
			        	u_int inhibitMaskBuf[2][10];                // ���͋֎~�����p�o�b�t�@
			        	KEYBOARD_SET* kbSet;
		
			        	if( skbStep == STEP_SKB_NONE ){
			        		printf("SKB_Init\n");
			        		GetWaitPowerOff = SKB_Init();  // �Ԃ�l�́A�g�c�c�A�N�Z�X���m�ۂ����Z�}�t�H�h�c��Ԃ��A�֐��B
			        		                               // �d���R���g���[�������ŁA�g�p�B
			        		SKB_SetCallback( MessageFromSKB ); /* �r�d���N�G�X�g��A�ҏW���ʂ̎󂯎��p */
			        		skbStep = STEP_SKB_INIT;
			        	}

			        	inhibitMaskBuf[0][0]=0x309b;	inhibitMaskBuf[0][1]=0x0000;	//���_
			        	inhibitMaskBuf[1][0]=0x309c;	inhibitMaskBuf[1][1]=0x0000;	//�����_
			        	inhibitWorkBuf[0][0]=0x309b;	inhibitWorkBuf[0][1]=0x0000;	//���_
			        	inhibitWorkBuf[1][0]=0x309c;	inhibitWorkBuf[1][1]=0x0000;	//�����_

			        	SKB_SetInhibit_MaskChar( inhibitMaskBuf, 2 ); //���͋֎~�ł��A�{�^���͉����镶���B
			        	SKB_SetInhibit_Disable( inhibitWorkBuf , 2 ); //���́A�֎~�����Ȃ��B
			        	SjisToUcs4( TextBufSjis, TextBufUcs, 1024 );

			        	kbSet = &KeyBoardSet_All;
			        	      // �L�[�{�[�h�Z�b�g�A�ҏW���샂�[�h�A�ҏW�����e�L�X�g���w��A0�ő���͕�����
			        	SKB_Open( kbSet, 0, TextBufUcs, 0 ); 
			        	skbStep = STEP_SKB_OPENED;
			        }
			        break;
			case STEP_SKB_OPENED:
			        if( SKB_GetStatus() ){
			        	SKB_SetUsbKeyborad( kbStat, &UsbKbData, KbdGetNumLockState());
			        	SKB_Control( ulPad );      // �\�t�g�E�F�A�L�[�{�[�h���ɓ��͏���n���Đ��䂷��B
			        	pkSkb = SKB_MakeVif1CallPacket( 0, 0 );
			        }
			        break;
			}
		}

		/* �`��p�P�b�g�쐬 */
		pkVif1 = &PkVif1[db_id];
		sceVif1PkReset( pkVif1 );
		if( pkSkb ){
			sceVif1PkCall( pkVif1, pkSkb, 0 );
		}
		sceVif1PkEnd(pkVif1, 0);
		sceVif1PkTerminate( pkVif1 );
		/* VSync�҂��A�_�u���o�b�t�@�؂�ւ� */
		FlushCache(WRITEBACK_DCACHE);			// D�L���b�V���̑|���o��

		// toVIF1�`���l�����N�����āA�쐬���݂̃p�P�b�g��`��
		sceGsSyncPath(0, 0);          // �f�[�^�]���I���܂őҋ@

		// Atok���C�u�����́A�g�c�c�A�N�Z�X���Ƃ��Ȃ��̂ŁA�X���[�v���Ԃ������B
		// ���̂��߁A�T���v���ł́A���[�v���C�I���e�B�X���b�h�œ��삳���Ă���B
		// ���̂u�܂ŁA���C�������́AWaitSema���āA�`�s�n�j�ɏ���������B
		WaitSema( VSync_SemaID );

		if( db_id ) {
			sceGsSetHalfOffset(&g_db.draw1, 2048, 2048, InterlaceField );
		} else {
			sceGsSetHalfOffset(&g_db.draw0, 2048, 2048, InterlaceField );
		}
		sceGsSwapDBuff( &g_db, db_id );  // �_�u���o�b�t�@�؂�ւ�
		sceDmaSend( sceDmaGetChan(SCE_DMA_VIF1), pkVif1->pBase );
		db_id^=1;
	}
	/*---------------------------------------------------------------*/
	sceGsSyncPath(0, 0);


	/* v-blank �J�n�̊��荞�݂̋֎~ */
	DisableIntc( INTC_VBLANK_S );
	/* v-blank �J�n�̊��荞�݃n���h���o�^���� */
	RemoveIntcHandler(INTC_VBLANK_S,VSync_HandleID);
	/* v-blank �J�n��҂Z�}�t�H���폜 */
	DeleteSema( VSync_SemaID );

	/* �R���g���[���A�I������ */
	scePadPortClose( 0, 0 );
	scePadEnd();

	/* �d���R���g���[���̉��� */
	sceCdPOffCallback( 0, (void *)0);
	TerminateThread( PowerOff_ThreadID );
	DeleteThread( PowerOff_ThreadID );
	DeleteSema( PowerOff_SemaID );

	sceCdInit( SCECdEXIT );

	sceSifExitCmd();

	for(;;);

	return 0;
}

