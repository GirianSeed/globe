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
 *       0.20           09.04.2001      mikedean timing change, buffer bugfix
 */

#include <eekernel.h>
#include <libgraph.h>
#include <libipu.h>
#include <libmpeg.h>
#include <libsdr.h>
#include <sdrcmd.h>
#include <stdio.h>
#include <string.h>
#include "ezmpegstr.h"
#include "audiodec.h"
#include "playpss.h"

// ////////////////////////////////////////////////////////////////
//
//  Definitions
//
#define READCHUNKSIZE   16384
#define DMACHUNKSIZE    4096          // bytes
#define PK_SIZE         (MAX_MBX * MAX_MBY * 6 + 10)
#define P3TAGBUFF1      0
#define P3TAGBUFF2      (bound(PK_SIZE * 4, 64))
#define FBP1            ((bound(MAX_WIDTH, 64) * bound(MAX_HEIGHT, 32)) >> 11)
#define CSCPERPIX       (0.000151f)   // number of scanlines to CSC one pixel
#define FILLBUFFMAXTIME (25)          // max scanlines that fillbuff can need
                                      // (empirical)

// ////////////////////////////////////////////////////////////////
//
//  Variables
//
static int isStarted;
static AudioDec audioDec;
static int (*fileRead) (void *, int);
static int bgDuration;

// ////////////////////////////////////////////////////////////////
//
// Buffer for multiplexed pss data
// 
static char muxBuff[READCHUNKSIZE] bss_align(64);
static int  muxBuffFullness;

// ////////////////////////////////////////////////////////////////
// 
//  Data for uncached access
// 
static u_int *path3tag;

// ////////////////////////////////////////////////////////////////
//
//  Buffer for demultiplexed m2v data (uncached access)
//
//  Regions: times are relative to DMA and circular wrt BuffStart, BuffSize
//
//  demuxBuffPresent -> demuxBuffFuture   = Area currently being processed
//  demuxBuffFuture  -> demuxBuffPast     = Demuxed data ready for processing
//  demuxBuffPast    -> demuxBuffPresent  = Processed data
//
static char *demuxBuff;
static int   demuxBuffSize;
static char *demuxBuffPast;
static char *demuxBuffPresent;
static char *demuxBuffFuture;


// ////////////////////////////////////////////////////////////////
//
//  Function declarations
//
static int videoCallback(sceMpeg *mp, sceMpegCbDataStr *cbstr, void *data);
static int audioCallback(sceMpeg *mp, sceMpegCbDataStr *cbstr, void *data);
static int nodataCallback(sceMpeg *mp, sceMpegCbData *cbstr, void *data);
static int backgroundCallback(sceMpeg *mp, sceMpegCbData *cbstr, void *data);
static int errorCallback(sceMpeg *mp, sceMpegCbData *cbstr, void *data);

static int fillBuff(sceMpeg *mp,int blocking);
static void clearBackground(u_int val);


// ///////////////////////////////////////////////////////////////////
// 
//  Play back pss file
// 
void playPss(int (*cbFileRead) (void *, int), playPssRsrcs *rsrcs)
{
  sceMpeg mp;
  int drawid = 1;
  sceGsDispEnv disp;
  
  // //////////////////////////////////////
  // 
  //  Initialize GS
  // 
  sceGsResetGraph(0, SCE_GS_INTERLACE,
#ifdef PAL
		  SCE_GS_PAL,
#else
		  SCE_GS_NTSC,
#endif
		  SCE_GS_FIELD);
  
  sceGsResetPath();
  path3tag = rsrcs->path3tag;
  
  // //////////////////////////////////////
  // 
  //  Initialize IPU and MPEG library
  // 
  sceMpegInit();
  
  // /////////////////////////////
  // 
  //  Initialize audio decoder
  // 
  sceSdRemoteInit();
  sceSdRemote(1, rSdInit, SD_INIT_COLD);
  
  audioDecCreate(&audioDec,
		 rsrcs->audioBuff,   rsrcs->audioBuffSize,
		 rsrcs->iopBuff,     rsrcs->iopBuffSize,
		 rsrcs->zeroBuff,    rsrcs->iopZeroBuff,    ZERO_BUFF_SIZE);
  
  isStarted = 0;
  
  // ////////////////////////////
  // 
  //  Clear background
  // 
  clearBackground(0x80404040);
  
  // ////////////////////////////
  // 
  //  Create MPEG decoder
  // 
  sceMpegCreate(&mp, rsrcs->mpegBuff, rsrcs->mpegBuffSize);
  
  sceMpegAddStrCallback(&mp, sceMpegStrM2V, 0,
			(sceMpegCallback) videoCallback, NULL);
  
  sceMpegAddStrCallback(&mp, sceMpegStrPCM, 0,
			(sceMpegCallback) audioCallback, NULL);
  
  sceMpegAddCallback(&mp, sceMpegCbNodata, nodataCallback, NULL);
  sceMpegAddCallback(&mp, sceMpegCbBackground, backgroundCallback, NULL);
  sceMpegAddCallback(&mp, sceMpegCbError, errorCallback, NULL);
    
  // ////////////////////////////
  // 
  //  Initialise read buffer for multiplexed pss data
  // 
  muxBuffFullness = 0;
  
  // ////////////////////////////
  // 
  //  Initialise buffer for demultiplexed m2v data
  // 
  demuxBuff        = rsrcs->demuxBuff;
  demuxBuffSize    = rsrcs->demuxBuffSize;
  demuxBuffPast    = demuxBuff + 16;
  demuxBuffPresent = demuxBuff;
  demuxBuffFuture  = demuxBuff + 16;
  
  // ////////////////////////////
  // 
  //  Prebuffer (bounded by which of the m2v and pcm buffers fills first)
  // 
  fileRead = cbFileRead;
  while (fillBuff(&mp, 1));
  
  // ////////////////////////////
  // 
  //  Initilize display
  // 
  sceGsSetDefDispEnv(&disp, SCE_GS_PSMCT32, DISP_WIDTH, DISP_HEIGHT, 0, 0);
  
  if (MAX_WIDTH > DISP_WIDTH) {
    disp.dispfb.DBX = (MAX_WIDTH - DISP_WIDTH) / 2;
  }
  if (MAX_HEIGHT > DISP_HEIGHT) {
    disp.dispfb.DBY = (MAX_HEIGHT - DISP_HEIGHT) / 2;
  }
  disp.dispfb.FBW = bound(MAX_WIDTH, 64) / 64;
  sceGsPutDispEnv(&disp);
  
  // ////////////////////////////
  // 
  //  Wait for even field
  // 
  while (sceGsSyncV(0) == 1);
  
  // ////////////////////////////
  // 
  //  Main loop to decode MPEG bitstream
  //
  while (!sceMpegIsEnd(&mp)) {
    
    *T0_MODE  = 1 << 7 | 3;  // Count hsyncs
    *T0_COUNT = 0;
    
    // ////////////////////////////
    // 
    //  Get decoded picture in sceIpuRGB32 format
    // 
    while (sceMpegGetPicture(&mp, rsrcs->rgb32, MAX_MBX * MAX_MBY) < 0) {
      printf ("sceMpegGetPicture failed\n");
      while (1);
    }
    
    // ////////////////////////////
    // 
    //  Initialize DMA tags for path3 (uncached access)
    // 
    if (mp.frameCount == 0) {
      
      int origin_x = (MAX_WIDTH - mp.width)/2;
      int origin_y = (MAX_HEIGHT - mp.height)/2;
      
      setLoadImageTags(&path3tag[P3TAGBUFF1], rsrcs->rgb32,
		       origin_x, origin_y, mp.width, mp.height);
      setLoadImageTags(&path3tag[P3TAGBUFF2], rsrcs->rgb32,
		       origin_x, origin_y + MAX_HEIGHT, mp.width, mp.height);

      bgDuration = (int) ((float) mp.width * (float) mp.height * CSCPERPIX);
    }
    
    // ////////////////////////////
    // 
    //  Load decoded image to GS via path3
    // 
    sceGsSyncPath(0, 0);
    loadImage(drawid ? &path3tag[P3TAGBUFF2] : &path3tag[P3TAGBUFF1]);
    
    // ////////////////////////////
    //
    //  Send audio data to IOP
    //
    audioDecSendToIOP(&audioDec);
    
    if (!isStarted && audioDecIsPreset(&audioDec)) {
      audioDecStart(&audioDec);
      isStarted = 1;
    }

#ifdef PAL
    while (*T0_COUNT < (625 - FILLBUFFMAXTIME))
#else
    while (*T0_COUNT < (525 - FILLBUFFMAXTIME))
#endif
      {
	fillBuff(&mp, 0);
      }

    // ////////////////////////////
    // 
    //  Wait until two vblank happen
    // 
    while (sceGsSyncV(0) == 1);
    
    // ////////////////////////////
    // 
    //  Switch display
    // 
    if (drawid) {
      disp.dispfb.FBP = FBP1;
      drawid = 0;
    } else {
      disp.dispfb.FBP = 0x00;
      drawid = 1;
    }
    sceGsPutDispEnv(&disp);
  }
  
  audioDecReset(&audioDec);
  
  // ////////////////////////////
  // 
  //  Reset MPEG decoder to the initial state
  // 
  sceMpegReset(&mp);
  
  // ////////////////////////////
  // 
  //  Delete MPEG decoder
  // 
  sceMpegDelete(&mp);
}

static int videoCallback(sceMpeg *mp, sceMpegCbDataStr *cbstr, void *data)
{
  int availSpace;
  int spill;
  
  availSpace = demuxBuffPresent - demuxBuffPast;    // processed data
  if (availSpace < 0) availSpace += demuxBuffSize;  // ring buffer

  if ((availSpace == 0) && (demuxBuffPresent == demuxBuffFuture))
    availSpace = demuxBuffSize;    // special case - buffer is empty, not full
  
  if (availSpace < cbstr->len) return 0;

  spill = (demuxBuffPast - demuxBuff) + cbstr->len - demuxBuffSize;
  
  if (spill > 0) {
    memcpy(UncachedAddr(demuxBuffPast), cbstr->data, cbstr->len - spill);
    memcpy(UncachedAddr(demuxBuff), cbstr->data + (cbstr->len - spill),
	   spill);
    demuxBuffPast = demuxBuff + spill;
  }
  
  else {
    memcpy(UncachedAddr(demuxBuffPast), cbstr->data, cbstr->len);
    demuxBuffPast += cbstr->len;
  }
  
  return 1;
}

static int audioCallback(sceMpeg *mp, sceMpegCbDataStr *cbstr, void *data)
{
  u_char *pd0, *pd1;
  int d0, d1;
  int spill;
  
  audioDecBeginPut(&audioDec, &pd0, &d0, &pd1, &d1);
  
  cbstr->len  -= 4;  // first four bytes are sub-stream ID
  cbstr->data += 4;
  
  if (cbstr->len > (d0 + d1)) return 0;
  
  spill = cbstr->len - d0;
  
  if (spill > 0) {
    memcpy(pd0, cbstr->data, cbstr->len - spill);
    memcpy(pd1, cbstr->data + (cbstr->len - spill), spill);
  }
  
  else memcpy(pd0, cbstr->data, cbstr->len);
  
  audioDecEndPut(&audioDec, cbstr->len);
  return 1;
}

static int nodataCallback(sceMpeg *mp, sceMpegCbData *cbstr, void *data)
{
  int dmaSize;
  int availData;
  
  demuxBuffPresent = demuxBuffFuture;
  
  availData = demuxBuffPast - demuxBuffFuture;    // data ready for processing
  if (availData < 0) availData += demuxBuffSize;  // ring buffer
  
  while (availData < DMACHUNKSIZE) {
    
    if (fillBuff(mp, 1) == 0) {
      
      // /////////////////////////////////////////////////////////////////
      //
      //  More data cannot be obtained from the demultiplexer so this must
      //  be the end of the stream. Round up the data to ensure there's no
      //  remainder from the quadword-multiple transfer, otherwise the
      //  decoder won't receive the MPEG end sequence code.
      // 
      printf("End of stream presumed\n");
      availData += 15;
      break;
    }  
    
    availData = demuxBuffPast - demuxBuffFuture;   // data ready for processing
    if (availData < 0) availData += demuxBuffSize; // ring buffer
  }
  
  dmaSize = availData & ~0xf;
  if (dmaSize > DMACHUNKSIZE) dmaSize = DMACHUNKSIZE;
  
  if ((demuxBuffPresent + dmaSize) > (demuxBuff + demuxBuffSize))
    dmaSize = demuxBuff + demuxBuffSize - demuxBuffPresent;  
  
  // ////////////////////////////
  // 
  //  DMA must have stopped so these fields are safe to write to
  // 
  *D4_QWC  = dmaSize / 16;
  *D4_MADR = (int) demuxBuffPresent;
  *D4_CHCR = 1 | 1 << 8;               // start, normal, from memory
  
  demuxBuffFuture = demuxBuffPresent + dmaSize;
  
  if (demuxBuffFuture >= (demuxBuff + demuxBuffSize))  // ring buffer
    demuxBuffFuture -= demuxBuffSize;  
  
  return 1;
}

static int backgroundCallback(sceMpeg *mp, sceMpegCbData *cbstr, void *data)
{
  int dueTime = *T0_COUNT + bgDuration;

  while (*T0_COUNT < dueTime) fillBuff(mp, 0);
  return 1;
}

static int errorCallback(sceMpeg *mp, sceMpegCbData *cbstr, void *data)
{
  sceMpegCbDataError *cberr= (sceMpegCbDataError *) cbstr;
  
  printf("MPEG decoding error: '%s'\n", cberr->errMessage);  
  while (1);
  return 1;
}

// /////////////////////////////////////////////////
//
//  Fill the demultiplex buffers from at most READCHUNKSIZE worth of
//  multiplexed data. Returns number of bytes put into buffers.
//
static int fillBuff(sceMpeg *mp, int blocking)
{
  char *startPos;
  int procBytes;
  
  if (muxBuffFullness) startPos = muxBuff + READCHUNKSIZE - muxBuffFullness;
  
  else {
    do {
      muxBuffFullness = fileRead(muxBuff, READCHUNKSIZE);
      
      if ((muxBuffFullness == 0) && !blocking) return 0;
      
    } while (muxBuffFullness == 0);
    
    startPos = muxBuff;
  }

  procBytes = sceMpegDemuxPss(mp, (u_char *) startPos, muxBuffFullness);
  muxBuffFullness -= procBytes;

  return procBytes;
}

// ///////////////////////////////////////////////////////////////////
// 
//  Clear background of display
// 
static void clearBackground(u_int val)
{
  int i;
  
  static u_int backTile[16*16] bss_align(64);
  u_int *p = (u_int*) UncachedAddr(backTile);
  
  for (i = 0; i < 16*16; i++) p[i] = val;
  
  setLoadImageTagsTile(path3tag, backTile,
		       0, 0, MAX_WIDTH, MAX_HEIGHT*2);
  loadImage(path3tag);
  sceGsSyncPath(0, 0);  
}
