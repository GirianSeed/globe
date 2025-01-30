/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                    - easy mpeg streaming -
 *
 *                         Version 0.10
 *                             ASCII
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          ezmpegstr.h
 *          global definitions for mpeg streaming program
 *
 *       Version        Date            Design   Log
 *  --------------------------------------------------------------------
 *       0.10           08.21.2001      mikedean the first version
 */

#ifndef _EZMPEGSTR_H_
#define _EZMPEGSTR_H_

//#define PAL
#define MEDIA_CD

#ifdef PAL
#define DISP_WIDTH  640
#define DISP_HEIGHT 512
#else
#define DISP_WIDTH  640
#define DISP_HEIGHT 448
#endif

#define MAX_WIDTH   720    // See readme
#ifdef PAL
#define MAX_HEIGHT  512
#else
#define MAX_HEIGHT  480
#endif

#define MAX_MBX         (MAX_WIDTH/16)
#define MAX_MBY         (MAX_HEIGHT/16)

#define ZERO_BUFF_SIZE  (0x800)

#define bound(val, x) ((((val) + (x) - 1) / (x))*(x))
#define min(x, y)	(((x) < (y))? (x): (y))

#define bss_align(val) \
    __attribute__ ((aligned(val))) __attribute__ ((section (".bss")))
#define data_align(val) __attribute__ ((aligned(val)))

#define UNCMASK 0x0fffffff
#define UNCBASE 0x20000000
#define DmaAddr(val) \
    ((void*)((unsigned int)(val) & UNCMASK))
#define UncachedAddr(val) \
    ((void*)(((unsigned int)(val) & UNCMASK) | UNCBASE))

#define PATH3_TAG_SIZE(x, y) (4 * 2 * bound(((x) * (y) * 6 + 10) * 4, 64))

// ldimage.c

void setLoadImageTags(u_int *tags, void *image,
	int x, int y, int  width, int height);
void setLoadImageTagsTile(u_int *tags, void *image,
	int x, int y, int  width, int height);
void loadImage(u_int *tags);

#endif /* _EZMPEGSTR_H_ */
