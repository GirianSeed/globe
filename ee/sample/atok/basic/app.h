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

#ifndef __APP_H__
#define __APP_H__

// Atok Settings
/* atok dataの存在位置設定 */
#define ATOK_DRIVE      "hdd0:"
#define ATOK_DEVICE     "pfs0:"
#define ATOK_DIR        "/atok"
#define SYSDIC          "/atokp.dic"
#define USERDIC         "/atokpyou.dic"

// libpfont settings
// see. font.c & Makefile
#define FONT_SIZE_X             (32)
#define FONT_SIZE_Y             (16)

// libccc settings
// see. ccc.c & Makefile

// GS Settings
#define FFMODE                  (SCE_GS_FRAME)
#define FBUF_FORMAT             (SCE_GS_PSMCT32)
#define ZBUF_FORMAT             (SCE_GS_PSMZ24)

// Screen Settings
#define SCREEN_WIDTH            (640)
#define SCREEN_HEIGHT           (224)
#define SCREEN_CENTERX          (2048)
#define SCREEN_CENTERY          (2048)
#define SCREEN_TOP              (SCREEN_CENTERY - SCREEN_HEIGHT / 2)
#define SCREEN_LEFT             (SCREEN_CENTERX - SCREEN_WIDTH / 2)
#define SCREEN_RIGHT            (SCREEN_LEFT + SCREEN_WIDTH)
#define SCREEN_BOTTOM           (SCREEN_TOP + SCREEN_HEIGHT)

// GS Local Memory Settings
// Notes:
//      libpfontはGS Memoryを(初期化において)Staticに必要とする
//      したがって、動的にメモリを管理する手段は取りづらい
//      このサンプルにおいては、GS memoryの全てを使いきる形で
//      設定されているが、他のTextureを使うような表示物と
//      合せて使用する場合は注意が必要
// 以下 WORD単位でのGS Local Memory Layoutの定義値を示します
#define GS_MEMTOP       (0)
#define GS_MEMBOTTOM    (0x100000)
#define FZ_ALIGN        (2048)          /* FRAME/Z buffer align size */
#define TC_ALIGN        (64)            /* TEX/CLUT buffer align size */
#define FBUF_PPW        ((FBUF_FORMAT & SCE_GS_PSMCT16) ?  2 : 1)
#define ZBUF_PPW        ((ZBUF_FORMAT & SCE_GS_PSMZ16) == SCE_GS_PSMZ16 ? 2 : 1)
#define F_BUFFER_SIZE   ((SCREEN_WIDTH * SCREEN_HEIGHT * FBUF_PPW) * (FZ_ALIGN - 1) / FZ_ALIGN)
#define Z_BUFFER_SIZE   ((SCREEN_WIDTH * SCREEN_HEIGHT * ZBUF_PPW) * (FZ_ALIGN - 1) / FZ_ALIGN)
#define FRAME_BUFFER0   (GS_MEMTOP)
#define FRAME_BUFFER1   (FRAME_BUFFER0 + F_BUFFER_SIZE)
#define ZBUFFER         (FRAME_BUFFER1 + F_BUFFER_SIZE)
#define TEXBUFFER       (ZBUFFER + Z_BUFFER_SIZE)
/* CLUT Tableのサイズ: 32bit * 256エントリ see. libpfont/doc/pfont_rf.doc */
#define CLUT_TABLE_SIZE (((4 /* 32bit */ * 256) * (TC_ALIGN - 1)) / TC_ALIGN)
#define CLUT_TABLE      (GS_MEMBOTTOM - CLUT_TABLE_SIZE)
#define TEXBUFFER_SIZE  (((CLUT_TABLE - TEXBUFFER) * (TC_ALIGN - 1)) / TC_ALIGN)

/* libpfont用 */
#define FONT_GSMEM      (TEXBUFFER)
#define FONT_GSMEMSIZE  (TEXBUFFER_SIZE)
#define CLUT_GSMEM      (CLUT_TABLE)

// Application Mode
typedef enum {
    TEST_MODE,
    REG_MODE,
    DEL_MODE
} mode_t;

#endif /* __APP_H__ */
