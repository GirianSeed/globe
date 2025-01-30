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
#include <eekernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <libgraph.h>
#include <libpkt.h>
#include <malloc.h>
#include <libatok.h>
#include <libdma.h>
#include <libccc.h>
#include <string.h>

#include "app.h"                // application�ݒ�w�b�_
#include "pad.h"                // pad.c�p�w�b�_
#include "font.h"               // font.c�p�w�b�_
#include "ccc.h"                // ccc.c�p�w�b�_
#include "info.h"               // information�\���p�w�b�_
/*
 * libatok.a�̊�{�I�����m��ׂ̃T���v��
 * �d���Ǘ����͈�؍s���Ă��Ȃ��̂ŁA���ۂ̎g�p�ɂ�����
 * ���ӎ�������softkb�̃T���v�����Q��
 *
 * ATOK���C�u�����֌W�̃R�����g��C�R�����g�ŁA
 * ���̑��̃R�����g��C++�R�����g�ɂ��Ă���
 */

// GS Double Buffer
static sceGsDBuff db;
/* Atok Library Context: align����Ă���K�v������ */
sceAtokContext atokctx __attribute__ ((aligned(64)));
/* dynamic library place: atok engine�̏ꏊ�B�������align���K�v */
static char dlib[512 * 1024] __attribute__ ((aligned(64)));
/* �����g���ׂ̈�work buffer: align�����16KB�ȏ�ł��邱�Ƃ���������� */
#define WORK_BUFFER_SIZE        (2 * 1024 * 1024)
static char work_buffer[WORK_BUFFER_SIZE] __attribute__((aligned(64)));


// PAD���͗p
static char alphabet[] =
{ 'a', 'b', 'c', 'd', 'e',
  'f', 'g', 'h', 'i', 'j',
  'k', 'l', 'm', 'n', 'o',
  'p', 'q', 'r', 's', 't',
  'u', 'v', 'w', 'x', 'y', 'z' };
static const u_int num_alpha = sizeof(alphabet) / sizeof(char);


static void init_modules(void);
static void init_ps2(void);
static void mount_pfs(void);
static void display_sync(sceGsDBuff *dbp, int side);

static void init_atok(void);
static void setup_atok(void);

static void test_mode_func(int);
static void reg_mode_func(int);
static void del_mode_func(int);

static void input_func(int, int);
static char *convert_func(int, int, int);

int main(int argc, char **argv)
{
    int frame = 0;

    // ���W���[���̏�����
    init_modules();

    // �ʏ��PS2������
    init_ps2();

    // PFS�̃}�E���g
    mount_pfs();

    // libccc�̏�����
    ccc_init();

    // libatok�̏�����
    init_atok();		/* Atok�G���W���̏����� */
    setup_atok();		/* Atok�G���W���̐ݒ� */

    // �\��Font�̏�����: ���̃T���v���ł�libpfont���g���Ă���
    font_init();                // in font.c

    for (;;) {
        sceSamplePad_t *pad;            // in pad.h
        static mode_t mode = TEST_MODE;
        static int init = 0;

        // pad���̍X�V���擾
        pad = pad_update();             // in pad.c

        // mode info
        info_disp_mode(mode, 0, 1);	// in info.c

        // file info
        info_disp_file_stat(3, 0);	// in info.c

        // Atok Configure info
        info_disp_common(3, 7);		// in info.c
        switch (mode) {
          case TEST_MODE:	// �ϊ��e�X�g���[�h
            test_mode_func(init);
            init = 1;
            break;
          case REG_MODE:	// �����P��o�^���[�h
            reg_mode_func(init);
            init = 1;
            break;
          case DEL_MODE:	// �����P��폜���[�h
            del_mode_func(init);
            init = 1;
            break;
        }

        init = 1;

        // ���[�h�ؑւ�select
        if (pad->SelectSw) {
            ++mode;
            if (mode > DEL_MODE)
                mode = TEST_MODE;
            init = 0;
        }
        display_sync(&db, frame & 1);
        frame++;
    }
    
    return 0;
}

static void init_modules(void)
{
#define SIO2MAN         "host0:/usr/local/sce/iop/modules/sio2man.irx"
#define PADMAN          "host0:/usr/local/sce/iop/modules/padman.irx"
#define DEV9            "host0:/usr/local/sce/iop/modules/dev9.irx"
#define ATAD            "host0:/usr/local/sce/iop/modules/atad.irx"
#define HDD             "host0:/usr/local/sce/iop/modules/hdd.irx"
#define PFS             "host0:/usr/local/sce/iop/modules/pfs.irx"
    sceSifInitRpc(0);
    while (sceSifLoadModule(SIO2MAN, 0, NULL) < 0)
        scePrintf("Can't load module %s\n", SIO2MAN);
    while (sceSifLoadModule(PADMAN, 0, NULL) < 0)
        scePrintf("Can't load module %s\n", PADMAN);
    while (sceSifLoadModule(DEV9, 0, NULL) < 0)
        scePrintf("Can't load module %s\n", DEV9);
    while (sceSifLoadModule(ATAD, 0, NULL) < 0)
        scePrintf("Can't load module %s\n", ATAD);
    while (sceSifLoadModule(HDD, 0, NULL) < 0)
        scePrintf("Can't load module %s\n", HDD);
    while (sceSifLoadModule(PFS, 0, NULL) < 0)
        scePrintf("Can't load module %s\n", PFS);
#undef PFS
#undef HDD
#undef ATAD
#undef DEV9
#undef PADMAN
#undef SIO2MAN
}

static void init_ps2(void)
{
    // DMA�̏�����
    sceDmaReset(1);

    // GS�̏�����
    sceGsResetPath();
    sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
    sceGsSetDefDBuff(&db, FBUF_FORMAT, SCREEN_WIDTH, SCREEN_HEIGHT,
                     SCE_GS_ZGREATER, ZBUF_FORMAT, SCE_GS_CLEAR);
    sceGsSyncPath(0, 0);
    sceGsSyncV(0);

    *(u_long *)&db.clear0.rgbaq = SCE_GS_SET_RGBAQ(0x00, 0x20, 0x40, 0x00, 0);
    *(u_long *)&db.clear1.rgbaq = SCE_GS_SET_RGBAQ(0x00, 0x20, 0x40, 0x00, 0);

    // Pad�̏�����
    pad_init();         // pad.c
}

static void mount_pfs(void)
{
    int res;

    // drive���𓾂�
    res = sceDevctl(ATOK_DRIVE, HDIOC_STATUS, NULL, 0, NULL, 0);
    switch (res) {
      case 0:   scePrintf("HDD ok\n");                          break;
      case 1:   scePrintf("HDD drive unformat\n");              break;
      case 2:   scePrintf("HDD reserved\n");                    break;
      case 3:   scePrintf("HDD not connected\n");               break;
      case -19: scePrintf("%s device not found\n", ATOK_DRIVE); break;
      case -24: scePrintf("cannot open\n");                     break;
      default:  scePrintf("Unknown error\n");                   break;
    }

    // �}�E���g���s��
    if (res == 0) {
        res = sceMount(ATOK_DEVICE, ATOK_DRIVE "__sysconf", 0, NULL, 0);
    }

    if (res < 0) {
        scePrintf("cannot mount device. errno = %d\n", res);
        exit (-1);
    }
}

static void display_sync(sceGsDBuff *dbp, int side)
{
    sceGsDrawEnv1 *draw;        // next draw target
    sceGsGParam *gp;
    int field;

    FlushCache(0);

    draw = side ? &dbp->draw1 : &dbp->draw0;

    gp = sceGsGetGParam();
    field = !sceGsSyncV(0);
    if ((gp->sceGsInterMode == SCE_GS_INTERLACE) &&
        (gp->sceGsFFMode = SCE_GS_FRAME)) {
        sceGsSetHalfOffset(draw, SCREEN_CENTERX, SCREEN_CENTERY, field);
    }
    sceGsSwapDBuff(dbp, side);
}

/*--------------------------------------------------
 * libatok���g�����֐��Q
 *--------------------------------------------------*/
/*
 * Atok�̏������Bapp.h�Œ�`����Ă���t�@�C�����g����
 * atokctx���R���e�L�X�g�Ƃ��ď���������
 * dynamic library��load����ꏊdlib�Aatok�̃R���e�L�X�gatokctx��
 * align�����Ȃ��Ă͂Ȃ�Ȃ����ɒ���
 */
void init_atok(void)
{
    int ret;

    ret = sceAtokInit(&atokctx,                         /* context pointer */
                      ATOK_DEVICE ATOK_DIR SYSDIC,      /* system dic path */
                      ATOK_DEVICE ATOK_DIR USERDIC,     /* user dic path   */
                      ATOK_DEVICE,                      /* sysconf device  */
                      (void *)dlib);                    /* dlib buffer     */
    if (ret != 0) {
        scePrintf("ERROR: sceAtokInit [%d]\n", ret);
        exit(-1);
    }
    ret = sceAtokKanjiOn(&atokctx);
    if (ret != 0) {
        scePrintf("ERROR: sceAtokKanjiOn [%d]\n", ret);
        exit(-1);
    }
}

/*
 * Atok�̏����̐ݒ������
 * ���̃T���v���ɂ����Ă͉p���Љ�����S�p�ϊ�����悤�ɕύX����
 */
void setup_atok(void)
{
    sceAtokConfig       atokcfg;
    int ret;

    /* atokcfg�ɂ�sizeof(sceAtokConfig)��ݒ肵�Ȃ��Ă͂Ȃ�Ȃ�
     * (Get���g���ɂ������Ă�)					*/
    atokcfg.Size = sizeof(sceAtokConfig);

    /* ���݂̐ݒ���擾���ݒ�ύX���s��
     * �����ł͎Q�l�Ƃ��đS�擾->�ύX->�S�ݒ���s��		*/
    ret = sceAtokGetConfig(&atokctx, &atokcfg, SCE_ATOK_CONFIG_ALL);
    if (ret != 0) {
        scePrintf("ERROR: sceAtokGetConfig [%d]\n", ret);
        exit(-1);
    }
    atokcfg.HZCnvDetail = 0x38; /* bit 4,5,6 : �����A�p���A�Љ���->�S�p
                                 * see. libatok.h			*/
    atokcfg.ConvMode = SCE_ATOK_CONVMODE_AUTO;  /* �P�^�A���߂̎������� */
    ret = sceAtokGetConfig(&atokctx, &atokcfg, SCE_ATOK_CONFIG_ALL);
    if (ret != 0) {
        scePrintf("ERROR: sceAtokSetConfig [%d]\n", ret);
        exit(-1);
    }
}

// �ϊ��e�X�g�p�̃��[�h
static void test_mode_func(int init)
{
    sceSamplePad_t *pad = pad_get();		// in pad.c
    static enum {
        INPUT_SUB,		// �ǂݓ���
        CONVERT_SUB		// �ϊ�
    } smode = INPUT_SUB;

    if (init == 0) {
        smode = INPUT_SUB;
        input_func(-1, -1);
        convert_func(-1, -1, FALSE);
    }

    switch (smode) {
      case INPUT_SUB:
        input_func(2, 5);
        if (pad->RupSw || pad->StartSw) {
            smode = CONVERT_SUB;
            // �ʏ��FEP(IME)�ł͓��͊m��Ƌ��ɍŏ��̕ϊ���₪�����̂�
            // SUB MODE�ڍs�O�ɏ����ϊ����s���Ă���
            sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_CONVFORWARD, 0);
        }
        break;
      case CONVERT_SUB:
        if (pad->StartSw) {
            // INPUT_SUB�ɖ߂鎞�ɂ͌Â��ϊ����͏���
            convert_func(-1, -1, FALSE);
            smode = INPUT_SUB;
        }
        convert_func(2, 5, TRUE);	// TRUE�͕��ߐL�k�\�ł��鎖�����߂�
        break;
    }
}

// �����o�^���[�h
static void reg_mode_func(int init)
{
#define BUFSZ   (256)
    sceSamplePad_t *pad = pad_get();		// in pad.c
    static enum {
        YOMI_INPUT_SUB,			// �ǂݓ���
        REG_INPUT_SUB,			// �o�^������̓���
        REG_CONVERT_SUB,		// �o�^������̕ϊ�
        REGIST_SUB,			// �o�^
        REGIST_CONT_SUB			// �o�^�������
    } smode = YOMI_INPUT_SUB;
    static char yomi[BUFSZ];
    static char *registword;
    int ret;

    if (init == 0) {
        smode = YOMI_INPUT_SUB;
        input_func(-1, -1);
        convert_func(-1, -1, FALSE);
    }

    switch (smode) {
      case YOMI_INPUT_SUB:
        input_func(2, 5);
        if (pad->RupSw || pad->StartSw) {
            /* alphabet���͂ɂ����atok�G���W���ɓ�����Ă���
             * ���͕������yomi�o�b�t�@�ɓ���Ă���		*/
            sceAtokGetConvertingStr(&atokctx, yomi, BUFSZ);
            smode = REG_INPUT_SUB;
            input_func(-1,-1);
        }
        break;
      case REG_INPUT_SUB:
        // ��݂͏�ɕ\�����Ă���
        font_put(2, 5, ccc_conv_sjis2utf8(yomi));		// in font.c
        // �o�^�����̓���
        input_func(2, 6);
        if (pad->RupSw || pad->RupSw) {
            smode = REG_CONVERT_SUB;
            /* Atok�G���W���ɓ��͂��ꂽ������̏����ϊ����s���Ă��� */
            sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_CONVFORWARD, 0);
        }
        break;
      case REG_CONVERT_SUB:
        // ��݂͏�ɕ\��
        font_put(2, 5, ccc_conv_sjis2utf8(yomi));		// in font.c

        if (pad->StartSw) {
            smode = REGIST_SUB;
        }
        registword = convert_func(2, 6, TRUE);
        break;
      case REGIST_SUB:
	/* ���o�^���s��
	 * ��Q�����͕i���ԍ��������A�ȉ��̔ԍ���n��
	 * �A�v���P�[�V�����ɕi���T�O�������ꍇ��
	 * 6:�����T�ςł̓o�^���D�܂���(���̃T���v���ł��g�p)
	 *
	 * 1  : ��ʖ���        11 : �A�̎�     21 : �i�s�ܒi
	 * 2  : �ŗL�l��        12 : �ڑ���     22 : �n�s�ܒi
	 * 3  : �ŗL�n��        13 : ������     23 : �o�s�ܒi
	 * 4  : �ŗL�g�D        14 : �ړ���     24 : �}�s�ܒi
	 * 5  : �ŗL���        15 : �ڔ���     25 : ���s�ܒi
	 * 6  : �����T��        16 : ����       26 : ���s�ܒi
	 * 7  : �����U��        17 : �J�s�ܒi   27 : ��ʓ���
	 * 8  : �����`��        18 : �K�s�ܒi   28 : �J�ϓ���
	 * 9  : �Ɨ���          19 : �T�s�ܒi   29 : �T�ϓ���
	 * 10 : �P����          20 : �^�s�ܒi   30 : �U�ϓ���
	 *                                      31 : �`�e��
	 *                                      32 : �`�e����
	 *                                      33 : ����
	 */
        ret = sceAtokDicRegWord(&atokctx, 6, registword, yomi);
	/* ret��10�̏ꍇ�͎����g�����s��Ȃ��Ă͂Ȃ�Ȃ�
	 * ���̓���͌��݂̏��ꍇ�ɂ���Ă͔��ɒ������Ԃ������邽��
	 * ���A�v���P�[�V�����ɂ����Ă�Waiting��Ԃ�\�����鎖		*/
        if (ret == 10) {
            scePrintf("Block Full!!! Expand Dictionary Now!!!\n");
            ret = sceAtokDicExpand(&atokctx, (void *)work_buffer, (size_t) WORK_BUFFER_SIZE);
            if (ret == 0) {
                ret = sceAtokDicRegWord(&atokctx, 6, registword, yomi);
            }
        }
        if (ret != 0)
            scePrintf("ERROR: sceAtokDicRegWord/Expand. [%d]\n", ret);
        smode = REGIST_CONT_SUB;
        // fall throgh
      case REGIST_CONT_SUB:
        font_put(2, 5, ccc_conv_sjis2utf8("�o�^����"));
        info_disp_continue(3, 8);
        if (pad->StartSw) {
            smode = YOMI_INPUT_SUB;
            input_func(-1, -1);
            convert_func(-1, -1, FALSE);
        }
        break;
    }
}

// �����o�^�P��̍폜�p���[�h
static void del_mode_func(int init)
{
#define BUFSZ   (256)
    sceSamplePad_t *pad = pad_get();
    static enum {
        YOMI_INPUT_SUB,			// �ǂݓ���
        CONVERT_SELECT_SUB,		// �ϊ������I��
        DELETE_SUB,			// �폜
        DELETE_CONT_SUB			// �폜�������
    } smode = YOMI_INPUT_SUB;
    static char yomi[BUFSZ];
    static char *delword;

    if (init == 0) {
        smode = YOMI_INPUT_SUB;
        input_func(-1, -1);
        convert_func(-1, -1, FALSE);
    }
    switch (smode) {
      case YOMI_INPUT_SUB:
        input_func(2, 5);
        if (pad->StartSw || pad->RupSw) {
	    /* �ǂݕ����񂪊m�肳�ꂽ��
	     * yomi�o�b�t�@�Ɍ��݂̕������擾���Ă���	*/
            sceAtokGetConvertingStr(&atokctx, yomi, BUFSZ);
            smode = CONVERT_SELECT_SUB;
            /* �����ϊ����s�� */
            sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_CONVFORWARD, 0);
            /* ���̃T���v���ɂ����ẮA�폜���[�h�œǂ݂ɑ΂��ĕ����̕��߂�
	     * ���݂͈Ӗ����Ȃ��̂ŁA���߂̃J�E���g���P�ȏ�Ȃ當�߂�L�΂�������
	     */
            while (sceAtokGetConvertingClauseCount(&atokctx) > 1)
		   sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_FOCUSRIGHT, 0);
        }
        break;
      case CONVERT_SELECT_SUB:
        // ��݂͏�ɕ\�����Ă���
        font_put(2, 5, ccc_conv_sjis2utf8(yomi));
        if (pad->StartSw) {
            smode = DELETE_SUB;
            /* �����ϊ����s���Ă��� */
            sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_CONVFORWARD, 0);
        }
        delword = convert_func(2, 6, FALSE);
        break;
      case DELETE_SUB:
        {
            int ret;

            /* ���������s��
             * ��Q������16-8�r�b�g�͒P��̎�ނ�����
	     *          1       : ���[�U�[�o�^�P��
             *          2       : �����o�^�P��
	     *          4       : �S��
             * ���Ӗ�����B���̃T���v���ł�4�̑S�Ă��w�肵�Ă���
             * �܂���R�����͌��C���f�b�N�X�ɂ��폜�̏ꍇ�Ɏg��������
             * �����ϊ����ɁA���C���f�b�N�X���w�肵�č폜����ꍇ�Ɏg��
             * ���̏ꍇ�͑�Q����������Ȃ�
             * ���̃T���v���ł�0xffff(�C���f�b�N�X�w��łȂ�)�ɂ��Ă���
	     */
            ret = sceAtokDicDelWord(&atokctx, (4 << 8) | 6, 0xffff, delword, yomi, 0);
            if (ret != 0) {
                scePrintf("ERROR sceAtokDicDelWord [%d]\n", ret);
            }
            smode = DELETE_CONT_SUB;
        }
        /* fall throught */
      case DELETE_CONT_SUB:
        font_put(2, 5, ccc_conv_sjis2utf8("�폜����"));
        info_disp_continue(3, 8);
        if (pad->StartSw) {
            smode = YOMI_INPUT_SUB;
            input_func(-1, -1);
            convert_func(-1, -1, FALSE);
        }
        break;
    }
#undef BUFSZ
}

//
// �������͗p�֐�
// Args:
//      x, y    : ���͂��s���ꏊ(2byte font�P�ʍ��W)
//                x, y���ɕ����̏ꍇ�A�ێ�����clear����
//      pad     : pad���
// ����:
//      �p�b�h�������œ���alphabet�I��
//      �p�b�h�E����alphabet�m��
//      �p�b�h�E����alphabet 1�����폜
//
static void input_func(int x, int y)
{
#define BUFSZ   (256)
    sceSamplePad_t *pad = pad_get();
    static char buf[BUFSZ];
    static int alpha = 0;
    int len;

    // x, y�����ɕ����̏ꍇ�������Ƃ݂Ȃ�
    if (x < 0 && y < 0) {
        alpha = 0;
        buf[0] = '\0';
        /* atok�G���W���̕ϊ���Ԃ�clear���Ă���
	 * �����I��clear���Ȃ��ƍŌ�̏�Ԃ��c��
	 */
        sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_DELETEALL, 0);
        return;
    } else
        // ������@�\��
        info_disp_input_info(3, 8);

    if (pad->LrightSw) ++alpha;
    if (pad->LleftSw) --alpha;
    /* ���͕����͕����R�[�h��sceAtokEditConv�ɓn�� */
    if (pad->RrightSw) sceAtokEditConv(&atokctx, alphabet[alpha], 0);
    /* ���͉�����SCE_ATOK_EDIT_BACKSPACE�ōs�� */
    if (pad->RleftSw) sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_BACKSPACE, 0);

    if (alpha < 0)              alpha = num_alpha - 1;
    if (alpha >= num_alpha)     alpha = 0;

    /* ���͂��ꂽ������\��
     * len�ɂ͊Y��������byte�����Ԃ�B�������ł͂Ȃ�
     * �܂��A���͕����͓K���ϊ����s����
     * ���̃T���v���ł̓��[�}�����͂̏ꍇ�������ɕϊ�����Ă���
     * see. setup_atok()
     */
    if ((len = sceAtokGetConvertingStr(&atokctx, buf, BUFSZ)) > 0) {
        font_put(x, y, ccc_conv_sjis2utf8(buf));
    }

    // �I�𒆂�alphabet��\��
    buf[0] = alphabet[alpha];
    buf[1] = '\0';
    font_put(x + len/2, y, ccc_conv_sjis2utf8(buf));

    // �J�[�\����\��
    font_put(x + len/2, y, ccc_conv_sjis2utf8("_"));

#undef BUFSZ
}

//
//  �����ϊ��p�֐�
// Args:
//      x, y    : �ϊ����s���ꏊ(2byte font�P�ʍ��W)
//              x, y���ɕ����̏ꍇ�A�ێ�����clear����
// ����:
//      �p�b�h�����Œ��ڕ��ߊm��
//      �p�b�h�E���Œ��ڕ��ߕϊ�
//      �p�b�h�E���Œ��ڕ��߂����΂�
//      �p�b�h�E���Œ��ڕ��߂��k�߂�
// can_extend��FALSE�̏ꍇ�͕��߂̐L�k�͂ł��Ȃ�
static char *convert_func(int x, int y, int can_extend)
{
#define BUFSZ   (256)
    sceSamplePad_t *pad = pad_get();
    static char buf[BUFSZ];
    static char det[BUFSZ];     // �m�蕶����
    int i, len;

    if (x < 0 && y < 0) {
        det[0] = '\0';
        buf[0] = '\0';
        /* SCE_ATOK_EDIT_DELETEALL�ŕϊ��G���W���̏�Ԃ���ɂ��Ă��� */
        sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_DELETEALL, 0);
    } else
        // ������@�\��
        info_disp_conv_info(3, 8, TRUE);


    /* �m�蕶���񂪑��݂����ꍇdet��cat����
     * �m����s����atok engine�͂��̕�����̕ێ����~�߂Ă��܂��̂ŁA
     * �m�蕶���̕\���ɕK�v�ƂȂ�
     */
    if (sceAtokGetConvertedStr(&atokctx, buf, BUFSZ) > 0) {
        strcat(det, buf);
        sceAtokFlushConverted(&atokctx); /* �ꉞ�Q�d�o�^���Ȃ��悤��Flush */
    }

    // �m�蕶�����\�� : len�ɂ͕�������������
    // ccc_get_conv_length()�͍Ō��ccc_conv_sjis2utf8()�ɂ�����ϊ��ɂ����
    // �������ꂽ�u�������v��Ԃ�(sceCccSJIStoUTF8�̎d�l)�̂�
    // sceAtokGetConveringStr()�Ƃ͈Ⴄ���Ƃɒ���
    font_put(x, y, ccc_conv_sjis2utf8(det));
    len = ccc_get_conv_length();

    // ���m�蕶���񂪂���ꍇbuf�ɓ����B�܂����삪�ł���悤�ɂ���
    if (sceAtokGetConvertingStr(&atokctx, buf, BUFSZ) > 0) {
        if (pad->RupSw)
            sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_CONVFORWARD, 0);
        if (pad->LrightSw)
            sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_KAKUTEIPART, 0);
        if (can_extend) {
            if (pad->RrightSw)
                sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_FOCUSRIGHT, 0);
            if (pad->RleftSw)
                sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_FOCUSLEFT, 0);
        }
    }

    // ���m�蕶�����\���B�m�蕶���񕪕\�������炷�K�v������
    font_put(x + len, y, ccc_conv_sjis2utf8(buf));

    // ���ڕ��߂�underline������
    /* sceAtokGetFocusClauseLen()�͕������ł͂Ȃ��u�o�C�g���v��
     * �Ԃ����Ƃɒ��ӁB						*/
    for (i = 0; i < sceAtokGetFocusClauseLen(&atokctx) / 2; i++) {
        font_put(x + len + i, y, ccc_conv_sjis2utf8("�Q"));
    }
    return det;
#undef BUFSZ
}
