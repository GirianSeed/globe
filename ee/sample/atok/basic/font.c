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
#include <malloc.h>
#include <string.h>
#include <sifdev.h>
#include <libdma.h>
#include <libgraph.h>
#include "libpfont.h"
#include "app.h"
#include "io.h"

#define CACHE_SIZE      (1024)  /* 1k */
#define PKTBUF_SIZE     (4096)  /* 4k */
// Makefile��� PFONT_PATH�����ꤵ��Ƥ���������.
// �ե�����ǥХ�����io.c�����
#define PFONT_DAT_PATH  PFONT_PATH "/sample/basic/sample.pf"

/* pfont ID */
static int              pfontId;

static u_long128        font_cache[CACHE_SIZE / sizeof(u_long128)];
static u_long128        *font_data;
static sceVif1Packet    packet;
/* packet buffer ɽ/΢: Packet��double buffering���� */
static u_long128        pktbuf0[PKTBUF_SIZE / sizeof(u_long128)] __attribute__((aligned(64)));
static u_long128        pktbuf1[PKTBUF_SIZE / sizeof(u_long128)] __attribute__((aligned(64)));
/* font environment */
static sceVu0FMATRIX    font_screen;
static sceVu0FMATRIX    font_matrix;
static sceVu0FVECTOR    font_locate;
static sceVu0FVECTOR    font_color = { 0xff, 0xff, 0xff, 0xff };
static float            font_pitch = 2.5f;
static union {
    u_long d[2];
    u_long128 tag;
} const s_gif = {{ SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0xeUL }};

static void *file_read(const char *path);
static void kick_fonts(void);

void font_init(void)
{
    scePrintf("data path = %s\n", PFONT_DAT_PATH);

    // �ƥХåե���0 clear
    memset(font_cache, 0, CACHE_SIZE);
    memset(&packet, 0, sizeof(sceVif1Packet));
    memset(pktbuf0, 0, PKTBUF_SIZE);
    memset(pktbuf1, 0, PKTBUF_SIZE);

    // packet�����
    sceVif1PkInit(&packet, pktbuf0);

    // pfont�����
    {
        // csize : ��ʸ��Cache����٤Υ�����
        // fsize : Cache�����ΰ襵����
        // num   : font_cache���������ν����ʸ����
        int csize = scePFontCalcCacheSize(2) - scePFontCalcCacheSize(1);
        int fsize = scePFontCalcCacheSize(1) - csize;
        int num   = (sizeof(font_cache) - fsize) / csize;

        // ������¤�Τν����
        pfontId = scePFontInit(num, font_cache);
        if (pfontId < 0) {
            scePrintf("scePFontInit() Failed\n");
            exit(-1);
        } else
            scePrintf("PFONT ID = %d\n", pfontId);
        // Texture�Хåե��λ��� : see app.h
        scePFontSetTexMem(pfontId, FONT_GSMEM, FONT_GSMEMSIZE, CLUT_GSMEM);
        // font data�μ���
        font_data = file_read(PFONT_DAT_PATH);
        // font data��attach
        if (scePFontAttachData(pfontId, font_data) != 0) {
            scePrintf("scePFontAttachData() Failed\n");
            exit(-1);
        }
        // font�Ķ��ν����
        {
            sceVu0FMATRIX m;
            // screen matrix
            sceVu0UnitMatrix(m);
            sceVu0UnitMatrix(font_screen);
            m[1][1] = 0.5f;     // �ĥ�������Ⱦʬ��
            sceVu0MulMatrix(font_screen, m, font_screen);
            font_screen[3][0] = (float)SCREEN_LEFT;
            font_screen[3][1] = (float)SCREEN_TOP;
            // font matrix
            sceVu0UnitMatrix(font_matrix);
        }
    }
}

void font_put(int x, int y, const char *str)
{
    int ret;

    // locate���������
    font_locate[0] = ((float)x) * 26.5f;
    font_locate[1] = ((float)y + 1) * (FFMODE == SCE_GS_FRAME ? 32.0f:26.0f);

    // Pfont�δĶ�����
    scePFontFlush(pfontId);
    scePFontSetScreenMatrix(pfontId, (const sceVu0FMATRIX *)&font_screen);
    scePFontSetFontMatrix(pfontId, (const sceVu0FMATRIX *)&font_matrix);
    scePFontSetLocate(pfontId, (const sceVu0FVECTOR *)&font_locate);
    scePFontSetColor(pfontId, (const sceVu0FVECTOR *)&font_color);
    scePFontSetPitch(pfontId, font_pitch);

    // Packet�δĶ�����:TEST1/ALPHA1��񴹤���ȾƩ�������
    sceVif1PkCnt(&packet, 0);
    sceVif1PkOpenDirectCode(&packet, 0);
    sceVif1PkOpenGifTag(&packet, s_gif.tag);
    sceVif1PkAddGsAD(&packet,
                     SCE_GS_TEST_1, SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 0, 0));
    sceVif1PkAddGsAD(&packet,
                     SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA(0, 1, 0, 1, 0));
    sceVif1PkCloseGifTag(&packet);
    sceVif1PkCloseDirectCode(&packet);

    // �ѥ��åȺ�����ž��: scePFontPuts���Ϥ���������END Tagʬ���餷�Ƥ���
    ret = scePFontPuts(pfontId, &packet, PKTBUF_SIZE/sizeof(u_long128) - 1, str);
    if (ret == 1) {
        do {
            kick_fonts();
        } while ((ret = scePFontPutsContinue(pfontId, &packet, PKTBUF_SIZE/sizeof(u_long128) - 1)) == 1);
    }
    if (ret != 0) {
        scePrintf("scePFontPuts[Continue]() failed. %d\n", ret);
        exit(-1);
    }
    kick_fonts();
}

static void kick_fonts(void)
{
    sceDmaChan  *dc = sceDmaGetChan(SCE_DMA_VIF1);
    u_int       addr = (u_int)packet.pBase;

    /* end tag */
    sceVif1PkEnd(&packet, 0);

    /* terminate */
    sceVif1PkTerminate(&packet);

    /* Cache Flush */
    FlushCache(0);

    /* DMA enable */
    dc->chcr.TTE = 1;

    /* Wait current DMA trans */
    sceDmaSync(dc, 0, 0);

    /* Send DMA */
    sceDmaSend(dc, (void *)addr);

    /* libpfont��Double Buffer���� */
    sceVif1PkInit(&packet,
                  packet.pBase == (u_long128 *)pktbuf0 ?
                  (u_long128 *)pktbuf1 : (u_long128 *) pktbuf0);
    sceGsSyncPath(0, 0);
}
