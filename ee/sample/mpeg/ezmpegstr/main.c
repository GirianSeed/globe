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

#include <stdio.h>
#include <malloc.h>
#include <eetypes.h>
#include <libmpeg.h>
#include <sifdev.h>
#include <sifrpc.h>
#include "ezmpegstr.h"
#include "playpss.h"
#include "strfile.h"

#define MODULEDIR       "host0:/usr/local/sce/iop/modules/"

#define MPEG_BUFF_SIZE  (SCE_MPEG_BUFFER_SIZE(MAX_WIDTH, MAX_HEIGHT))
#define DEMUX_BUFF_SIZE (192 * 1024)
#define IOP_BUFF_SIZE   (12288 * 2) // 512 * 48
#define AUDIO_BUFF_SIZE (IOP_BUFF_SIZE * 2)

char *commandname;
char *VERSION = "1.0";

static void usage();


// ///////////////////////////////////////////////////////////////
//
//  Load modules
//
void loadModule(char *moduleName)
{
  while (sceSifLoadModule(moduleName, 0, "") < 0) {
    printf("Cannot load '%s'\n", moduleName);
  }
  
  printf("Loaded IOP module '%s'\n", moduleName);
}

// ///////////////////////////////////////////////////////////////
//
//  Allocate EE memory
//
void *memalign64(size_t size, char *desc)
{
  void *ret;
  
  if (!(ret = memalign(64, size))) {
    printf("Cannot allocate %d bytes of EE memory for %s\n", size, desc);
    while (1);
  }
  
  else printf("Allocated %7d bytes of  EE memory for %s\n", size, desc);
  return ret;
}

// ///////////////////////////////////////////////////////////////
//
//  Allocate IOP memory
//
int iopalloc(unsigned int size, char *desc)
{
  void *ret;
  
  if (!(ret = sceSifAllocIopHeap(size))) {
    printf("Cannot allocate %d bytes of IOP memory for %s\n", size, desc);
    while (1);
  }
  
  else printf("Allocated %7d bytes of IOP memory for %s\n", size, desc);
  return (int) ret;
}

// ///////////////////////////////////////////////////////////////////
// 
//  Main function of ezmpegstr
// 
int main(int argc, char *argv[])
{
  playPssRsrcs rsrcs;
  
  commandname = argv[0];
  
  sceSifInitRpc(0);
  sceSifInitIopHeap();
  
  // /////////////////////////////
  // 
  //  Load modules
  // 
  loadModule(MODULEDIR "libsd.irx");
  loadModule(MODULEDIR "sdrdrv.irx");
  loadModule(MODULEDIR "cdvdstm.irx");
  
  if (argc != 2) {
    usage();
    while (1);
  }
  
  // //////////////////////////////////////
  // 
  //  Allocate resources required for playback
  // 
  rsrcs.mpegBuff      = memalign64(MPEG_BUFF_SIZE, "mpeg work area");
  rsrcs.mpegBuffSize  =            MPEG_BUFF_SIZE;
  
  rsrcs.rgb32 = (sceIpuRGB32 *)
    memalign64(MAX_MBX * MAX_MBY * sizeof(sceIpuRGB32), "rgb32 buffer");
  
  rsrcs.path3tag      = memalign64(PATH3_TAG_SIZE(MAX_MBX, MAX_MBY),
				   "path 3 tags");
  
  rsrcs.demuxBuff     = memalign64(DEMUX_BUFF_SIZE, "demux buffer");
  rsrcs.demuxBuffSize =            DEMUX_BUFF_SIZE;
  
  rsrcs.audioBuff     = memalign64(AUDIO_BUFF_SIZE, "audio buffer");
  rsrcs.audioBuffSize =            AUDIO_BUFF_SIZE;
  
  rsrcs.iopBuff       = iopalloc(IOP_BUFF_SIZE, "audio buffer");
  rsrcs.iopBuffSize   =          IOP_BUFF_SIZE;
  
  rsrcs.zeroBuff      = memalign64(ZERO_BUFF_SIZE, "zero buffer");  
  rsrcs.iopZeroBuff   = iopalloc  (ZERO_BUFF_SIZE, "zero buffer");
  
  // //////////////////////////////////////
  // 
  //  Playback mpeg file specified on command line
  // 
  while (1)
    {
      while (!strFileOpen(argv[1])) {
	printf ("Can't open file %s\n", argv[1]);
      }

      playPss(strFileRead, &rsrcs);

      strFileClose();
    }
  
  // //////////////////////////////////////
  // 
  //  Deallocate resources
  //     
  free(rsrcs.mpegBuff);
  free(rsrcs.rgb32);
  free(rsrcs.path3tag);
  free(rsrcs.demuxBuff);
  free(rsrcs.audioBuff);
  sceSifFreeIopHeap((void *) rsrcs.iopBuff);
  free(rsrcs.zeroBuff);
  sceSifFreeIopHeap((void *) rsrcs.iopZeroBuff);
  
  return 0;
}

// ///////////////////////////////////////////////////////////////////
// 
//  Print usage of this program
// 
static void usage()
{
  printf(
	 "usage: run %s pss_file          (version %s)\n"
	 , commandname, VERSION);
}
