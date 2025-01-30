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

/* ���[�hinfo�pstr : app.h��mode_t�ɍ����鎖 */
static const char *modestr[] = {
    "���͕ϊ��e�X�g",
    "���[�U�P��o�^",
    "���[�U�P��폜"
};

static sceCccUTF8 dst[256];             // ccc�p�̃o�b�t�@

// ���݂̃��[�h��\��
void info_disp_mode(int mode, int x, int y)
{

    sceCccSJIStoUTF8(dst, 256, "���[�h:");
    font_put(x, y, dst);
    sceCccSJIStoUTF8(dst, 256, modestr[mode]);
    font_put(x+4, y, dst);
}

// ���ʂ̓��͕��@(mode�ؑ�)�ɑ΂��Ă̕\��
void info_disp_common(int x, int y)
{
    sceCccSJIStoUTF8(dst, 256, "SELECT   : ���[�h�ؑ�");
    font_put(x, y, dst);    
}

// ���͂Ɋւ��鑀����@��\��
void info_disp_input_info(int x, int y)
{
    static char *string[] = 
        { "����     : ���̓A���t�@�x�b�g�I��",
          "��       : ���̓A���t�@�x�b�g�m��",
          "��       : �A���t�@�x�b�g�P�����폜",
          "��/START : ���͂��m�肷��" };
    static const int num_of_str = sizeof(string)/sizeof(char *);
    int i;

    for (i = 0; i < num_of_str; i++) {
        sceCccSJIStoUTF8(dst, 256, string[i]);
        font_put(x, y++, dst);
    }
}

// �ϊ��Ɋւ��鑀����@��\��
// can_extend��true�Ȃ當�ߐL�΂��^�k�߂��info���o��
void info_disp_conv_info(int x, int y, int can_extend)
{
    static char *string1[] =
        { "��       : ���ڕ��ߊm��",
          "��       : ���ڕ��ߕϊ�",
          "START    : �ϊ����m�肷��"
        };
    static char *string2[] =
        { "��       : ���ڕ��߂�L�΂�",
          "��       : ���ڕ��߂��k�߂�" };
    static const int num_of_str1 = sizeof(string1)/sizeof(char *);
    static const int num_of_str2 = sizeof(string2)/sizeof(char *);
    int i;

    for (i = 0; i < num_of_str1; i++) {
        sceCccSJIStoUTF8(dst, 256, string1[i]);
        font_put(x, y++, dst);
    }
    if (!can_extend)    return;
    for (i = 0; i < num_of_str2; i++) {
        sceCccSJIStoUTF8(dst, 256, string2[i]);
        font_put(x, y++, dst);
    }
}

// �o�^�^�폜�I������continue���b�Z�[�W
void info_disp_continue(int x, int y)
{
    sceCccSJIStoUTF8(dst, 256, "START    : �߂�");
    font_put(x, y, dst);
}

// ���݂̃t�@�C������\��
void info_disp_file_stat(int x, int y)
{
    struct sce_stat buf;
    int ret;

    sceSync(ATOK_DEVICE ATOK_DIR USERDIC, 0);
    if ((ret = sceGetstat(ATOK_DEVICE ATOK_DIR USERDIC, &buf)) != 0)
        scePrintf("ERROR sceGetstat [%d]\n", ret);
    else {
        sceCccSJIStoUTF8(dst, 256, "���[�U�[�����T�C�Y :");
        font_put(x, y, dst);
        sprintf(dst, "%d", buf.st_size);
        font_put(x + 10, y, dst);
    }
}

