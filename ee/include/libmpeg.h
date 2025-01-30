/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/* 
 *                      Emotion Engine Library
 *                          Version 0.10
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                       libmpeg - libmpeg.h
 *                     header file of libmpeg 
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.10            Aug.14.1999     umemura     the first version
 */
#ifndef _LIBMPEG_H_
#define _LIBMPEG_H_

#include "eetypes.h"
#include "eeregs.h"
#include "libipu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SCE_MPEG_BUFFER_SIZE(w, h)  ((w)*(h)*9/2 + 512)

typedef enum {
    sceMpegCauseError,
    sceMpegCauseNodata
} sceMpegCause;

typedef struct {
    int width;
    int height;
    int frameCount;
    char *errMessage;
    void *sys;
} sceMpeg;

typedef int (*sceMpegHandler)(sceMpeg *mp, sceMpegCause ca, void *data);

int sceMpegInit(void);
int sceMpegCreate(sceMpeg *mp, u_char *buff, int buffSize);
int sceMpegDelete(sceMpeg *mp);
int sceMpegAddBs(sceMpeg *mp, u_long128 *p, int size);
int sceMpegGetPicture(sceMpeg *mp, sceIpuRGB32 *rgb32, int mbcount);
int sceMpegRewind(sceMpeg *mp);
int sceMpegIsEnd(sceMpeg *mp);
sceMpegHandler sceMpegSetHandler(sceMpeg *mp,
				sceMpegHandler handler, void *data);

#ifdef __cplusplus
}
#endif

#endif // _LIBMPEG_H_

