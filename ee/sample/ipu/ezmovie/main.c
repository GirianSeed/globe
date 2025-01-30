/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                         - easy movie -
 *
 *                         Version 0.10
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            main.c
 *                   main function for ezmovie
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.10           Mar.29.1999     umemura     the first version
 *       0.20           Aug.06.1999     umemura     new IPU movie format
 */

// //////////////////////////////////////////////////////////////////
// 
// 
// bsData
// |-------------------------------------------------|
//        |
//        | toIPU DMA
//        v
// +-------------+
// |     IPU     |
// +-------------+
//        |
//        | from IPU DMA
//        v
// cscBuff on RDRAM (RGB32 format, single buffer)
// +---------------+
// |               |
// |               |  path3
// |               |--+-----------------+
// |               |  |                 |
// +---------------+  |                 |
//                    |                 |
// frame buffer       |                 |
// +------------------+----------+      |
// |                  |          |      |
// |                  v          |      |
// |      +---------------+      |      |
// |      |               |      |      |
// |      |               |      | 480  |
// |      |               |      |      |
// |      |               |      |      |
// |      +---------------+      |      |
// |                             |      |
// |                             |      |
// +-----------------------------+      |
// |                             |      |
// |                             |      |
// |      +---------------+      |      |
// |      |               |<-----+------+
// |      |               |      |
// |      |               |      |
// |      |               |      | 480
// |      +---------------+      |
// |                             |
// |                             |
// +-----------------------------+
// <----------- 640 ------------->
//  
// 640 x 480
// 
// //////////////////////////////////////////////////////////////////

#include <eeregs.h>
#include <eekernel.h>
#include <sifdev.h>
#include <libgraph.h>
#include <libipu.h>
#include "ezmovie.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define IPU_CTRL_VAL	0x01000000 // mp1 == 0 -> mpeg2
#define MAX_MBX		(MAX_WIDTH/16)
#define MAX_MBY		(MAX_HEIGHT/16)
#define MAX_BS_DATA_SIZE (128*1024*1024) // in bytes
#define PK_SIZE		(MAX_MBX * MAX_MBY * 6 + 10)
#define SCREEN_PK_SIZE	(DISP_WIDTH/16 * DISP_HEIGHT/16 * 6 + 10)
#define BSDMASIZE	(1024*1024 - 16)

char *commandname = NULL;
char *VERSION = "1.1";
char *CopyRight =
    "(C) 1999 Sony Computer Entertainment Inc. All Rights Reserved.";


// //////////////////////////////////////
// 
//  data for uncached access
// 
static u_int path3tag[2][bound(PK_SIZE*4, 64)] bss_align(64);
static u_int bsTags[(MAX_BS_DATA_SIZE/(BSDMASIZE) + 1)*4] bss_align(64);
static sceIpuRGB32 cscBuff[MAX_MBX * MAX_MBY] bss_align(64);
static u_int backTileTags[bound(SCREEN_PK_SIZE*4*2, 64)] bss_align(64);
static u_int backTile[16*16] bss_align(64);

extern inline void scTag(
    u_int *p,
    void *addr,
    u_int id,
    u_int qwc
)
{
    *(u_long*)p = \
    	  (u_long)(u_int)addr << 32
    	| (u_long)id << 28
    	| (u_long)qwc << 0;
}

static void usage();
static void clearBackground(u_int val);
static void setToIPUtags(u_int *tag, u_char *p, int size);
static void setDMAtoIPU(u_int *tag);
static int setDMAfromIPU(sceIpuRGB32 *rgb32, int count);
static void ipuMovie(char *filename);
static int readFile(char *filename, unsigned char **data, int *size);
void setLoadImageTags(u_int *tags, void *image,
	int x, int y, int  width, int height);
void setLoadImageTagsTile(u_int *tags, void *image,
	int x, int y, int  width, int height);
void loadImage(u_int *tags);

int main(int argc, char *argv[])
{
    char bsFilename[256];

    commandname = argv[0];

    if (argc != 2) {
	usage();
    	Exit(1);
    }
    sprintf(bsFilename, "sim:%s", argv[1]);

    ipuMovie(bsFilename);

    return 0;
}

static void ipuMovie(char *filename)
{
    int frame_no = 0;
    int i;
    int drawid;
    sceGsDispEnv disp;
    u_char *bsData;
    int bsDataSize;
    int width, height, nframes;
    int mbx, mby;
    int origin_x, origin_y;
    int ret;

    // //////////////////////////////////////
    // 
    //  Read IPU movie file
    // 
    ret = readFile(filename, &bsData, &bsDataSize);
    if (!ret) {
    	return;
    }

    // //////////////////////////////////////
    // 
    //  Read IPU movie Header
    // 
    width =  (int)*(u_short*)(bsData + 8);
    height = (int)*(u_short*)(bsData + 10);
    nframes = (int)*(u_int*)(bsData + 12);

    mbx = width/16;
    mby = height/16;
    origin_x = (DISP_WIDTH - width)/2;
    origin_y = (DISP_HEIGHT - height)/2;

    printf("%s(%dbytes, %dx%dx%d)\n",
    	filename, bsDataSize, width, height, nframes);

    // //////////////////////////////////////
    // 
    //  Initialize GS
    // 
    sceGsResetGraph(
    	0,	// all reset
	1,	// interlace
	2,	// NTSC
	0	// field
    );
    sceGsResetPath();
    sceGsSetDefDispEnv(
    	&disp,
	SCE_GS_PSMCT32,
	DISP_WIDTH,
	DISP_HEIGHT,
	0,
	0
    );
    sceGsPutDispEnv(&disp);

    // //////////////////////////////////////
    // 
    //  Initialize IPU and IPU library
    // 
    sceIpuInit();

    // ////////////////////////////
    // 
    // clear background
    // 
    clearBackground(0x80404040);

    // ////////////////////////////
    // 
    // set DMA tags for path3 (uncached access)
    // 
    setLoadImageTags(path3tag[0], cscBuff,
    	origin_x, origin_y, width, height);
    setLoadImageTags(path3tag[1], cscBuff,
    	origin_x, origin_y + DISP_HEIGHT, width, height);

    // ////////////////////////////
    // 
    // set DMA tags for IPU (uncached access)
    // 
    setToIPUtags(bsTags, bsData + 16, bsDataSize);

    // ////////////////////////////
    // 
    // loop play
    // 
    for (i = 0; ; i++) {

	printf("----------------------- %d ---------------------\n", i);
	// ////////////////////////////
	// 
	// reset IPU
	// 
	sceIpuReset();	// reset IPU
	sceIpuSync(0, 0);

	sceIpuBCLR(0);	// reset IPU_in_FIFO
	sceIpuSync(0, 0);

	// ////////////////////////////
	// 
	// set DMA to IPU
	// 
	setDMAtoIPU(bsTags);

	// ////////////////////////////
	// 
	// Set IPU_CTRL
	// 
	DPUT_IPU_CTRL(IPU_CTRL_VAL);

	// ////////////////////////////
	// 
	// play one sequence ... main loop
	// 
	drawid = 1;
	for (frame_no = 0; frame_no < nframes; frame_no++) {

	    // set DMA from IPU
	    setDMAfromIPU(cscBuff, mbx*mby);

	    // IDEC command
	    sceIpuIDEC(
	    	0,			// ofm: RGB32
		0,			// dte: no dither
		0,			// sgn: no offset
		0,			// dtd: no dt decode
		1,			// qsc: dummy
		0			// fb
	    );

	    // wait IDEC command to complete
	    sceIpuSync(0, 0);
	    // wait until DMA from IPU finishes
	    while ((*D3_CHCR>>8) & 0x01)
		;

	    // skip start code
	    sceIpuFDEC(32);	// fdec 32 bit
	    sceIpuSync(0, 0);

	    // ////////////////////////////
	    // 
	    // load decompressed image
	    // to the frame buffer
	    // 
	    loadImage(path3tag[drawid]);
	    sceGsSyncPath(0, 0);

	    // ////////////////////////////
	    // 
	    // wait vsync
	    // 
	    sceGsSyncV(0);

	    // ////////////////////////////
	    // 
	    // switch display
	    // 
	    if (drawid) {
		disp.dispfb.FBP = 0x96; // display bottom buffer
		drawid = 0;
	    } else {
		disp.dispfb.FBP = 0x00; // display top buffer
		drawid = 1;
	    }
	    sceGsPutDispEnv(&disp); // switch display
	}
    }

    free(bsData);
}

static int readFile(char *filename, unsigned char **data, int *size)
{
    int fd;
    int ret = 1;
    int readcount;

    // //////////////////////////////////////
    // 
    //  open a file to read and check its size
    // 
    fd = sceOpen(filename, SCE_RDONLY);
    if (fd < 0) {
    	printf("Cannot open %s\n", filename);
	return 0;
    }
    *size = sceLseek(fd, 0, SCE_SEEK_END);
    if (*size < 0) {
    	printf("sceLseek() fails (%s)\n", filename);
	sceClose(fd);
	return 0;
    }

    ret = sceLseek(fd, 0, SCE_SEEK_SET);
    if (ret < 0) {
    	printf("sceLseek() fails (%s)\n", filename);
	sceClose(fd);
	return 0;
    }

    *data = (u_char*)memalign(64, *size);
    if (*data == NULL) {
    	printf("memalign(%d) fails\n", *size);
	sceClose(fd);
	return 0;
    }

    // //////////////////////////////////////
    // 
    //  read bs data
    // 
    readcount = sceRead(fd, *data, *size);
    if (readcount != *size) {
	printf("Cannot read %s\n", filename);
	sceClose(fd);
	free(*data);
	return 0;
    }

    sceClose(fd);
    return 1;
}

static void setToIPUtags(u_int *tag, u_char *p, int size)
{
    int chunksize;
    u_int *unctag = (u_int*)UncachedAddr(tag);
    u_char *dmap = DmaAddr(p);

    while (size > 0) {
    	chunksize = min(size, BSDMASIZE);
	size -= chunksize;
	scTag(
	    unctag,		// u_int *p,
	    dmap,		// u_int addr,
	    (size)? 3: 0,	// u_int id,(ref/refe)
	    bound(chunksize,16)/16	// qwc
	);
	unctag += 4;
	dmap += chunksize;
    }
}

static void setDMAtoIPU(u_int *tag)
{
    DPUT_D4_TADR((u_int)DmaAddr(tag));
    DPUT_D4_QWC(0);
    DPUT_D4_CHCR(0x105);
}

static int setDMAfromIPU(sceIpuRGB32 *rgb32, int count)
{
    DPUT_D3_MADR((u_int)DmaAddr(rgb32));
    DPUT_D3_QWC(count*sizeof(sceIpuRGB32)/16);
    DPUT_D3_CHCR(0x100);	// STR: 1, DIR: 0
}

static void clearBackground(u_int val)
{
    int i;
    u_int *p = (u_int*)UncachedAddr(backTile);

    for (i = 0; i < 16*16; i++) {
        p[i] = val;
    }
    setLoadImageTagsTile(backTileTags, backTile,
    	0, 0, DISP_WIDTH, DISP_HEIGHT*2);
    loadImage(backTileTags);
    sceGsSyncPath(0, 0);
}

static void usage()
{
    printf("%s\n", CopyRight);
    printf(
	"usage: run %s ipu_movie_file          (version %s)\n"
	, commandname, VERSION);
}

