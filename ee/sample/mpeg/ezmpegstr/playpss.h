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
 *                           playpss.c
 *             main function for mpeg streaming program
 *
 *       Version        Date            Design   Log
 *  --------------------------------------------------------------------
 *       0.10           08.21.2001      mikedean the first version
 */

#ifndef _PLAYPSS_H_
#define _PLAYPSS_H_

#include <eetypes.h>

// ///////////////////////////////////////////////////////
//
// Resources required for PSS playback
//
typedef struct
{
  u_char      *mpegBuff;
  int         mpegBuffSize;
  sceIpuRGB32 *rgb32;
  u_int       *path3tag;
  void        *demuxBuff;
  int         demuxBuffSize;
  u_char      *audioBuff;
  int         audioBuffSize;
  int         iopBuff;
  int         iopBuffSize;
  u_char      *zeroBuff;
  int         iopZeroBuff;

} playPssRsrcs;

// ///////////////////////////////////////////////////////
//
// Functions
//
void playPss(int (*cbFileRead) (void *, int), playPssRsrcs *rsrcs);

#endif	// _PLAYPSS_H_
