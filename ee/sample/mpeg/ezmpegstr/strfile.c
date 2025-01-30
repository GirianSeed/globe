/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                    - easy mpeg streaming -
 *
 *                         Version 0.20
 *                           Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            strfile.c
 *                   functions for file streaming
 *
 *       Version        Date            Design   Log
 *  --------------------------------------------------------------------
 *       0.10           02.29.2000      umemura  the first version
 *       0.20           08.21.2001      mikedean non-blocking
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <eekernel.h>
#include <sifdev.h>
#include "ezmpegstr.h"
#include "strfile.h"

static int isStrFileInit = 0;
static int nonBlockReadReq = 0;

static StrFile infile;

// ////////////////////////////////////////////////////////////////
//
//  Open a file to read and check its size
//
//	< file name conversion >
//	   = from HD =
//	    dir/file.pss           -> host0:dir/file.pss
//	    host0:dir/file.pss     -> host0:dir/file.pss
//
//	   = from CD/DVD =
//	    cdrom0:\dir\file.pss;1 -> cdrom0:\DIR\FILE.PSS;1
//	    cdrom0:/dir/file.pss;1 -> cdrom0:\DIR\FILE.PSS;1
//	    cdrom0:/dir/file.pss   -> cdrom0:\DIR\FILE.PSS;1
//
int strFileOpen(char *filename)
{
    int ret;
    char *body = NULL;
    char fn[256];
    char devname[64];

    body = index(filename, ':');

    if (body) {
    	int dlen;

	// copy device name
	dlen = body - filename;
	strncpy(devname, filename, dlen);
	devname[dlen] = 0;

	body += 1;

	if (!strcmp(devname, "cdrom0")) { // CD/DVD
	    int i;
	    int len = strlen(body);
	    char *tail;

	    infile.isOnCD = 1;

	    for (i = 0; i < len; i++) {
	        if (body[i] == '/') {
		    body[i] = '\\';
		}
		body[i] = toupper(body[i]);
	    }

	    tail = (index(filename, ';'))? "": ";1";
	    sprintf(fn, "%s%s", body, tail);

	} else {			// HD
	    infile.isOnCD = 0;
	    sprintf(fn, "%s:%s", devname, body);
	}
    } else {				// HD (default)
    	body = filename;
	strcpy(devname, "host0");
	infile.isOnCD = 0;
	sprintf(fn, "%s:%s", devname, body);
    }

    printf("file: %s\n", fn);

    if (infile.isOnCD) {
	sceCdRMode mode;

    	if (!isStrFileInit) {
	    sceCdInit(SCECdINIT);
#ifdef MEDIA_CD
	    sceCdMmode(SCECdCD);
#else
	    sceCdMmode(SCECdDVD);
#endif
	    sceCdDiskReady(0);

	    isStrFileInit = 1;
	}

	infile.iopBuf = sceSifAllocIopHeap((2048 * 80) + 16);
	sceCdStInit(80, 5, bound((u_int)infile.iopBuf, 16));

	if(!sceCdSearchFile(&infile.fp, fn)){
	    printf("Cannot open '%s'(sceCdSearchFile)\n", fn);
	    sceSifFreeIopHeap((void *)infile.iopBuf);
	    return 0;
	}

	infile.size = infile.fp.size;
	mode.trycount = 0;
	mode.spindlctrl = SCECdSpinStm;
	mode.datapattern = SCECdSecS2048;
	sceCdStStart(infile.fp.lsn, &mode);

    } else {

        int stillExecuting;
      
	infile.fd = sceOpen(fn, SCE_RDONLY |SCE_NOWAIT);

	do
	  {
	    sceIoctl(infile.fd, SCE_FS_EXECUTING, &stillExecuting);
	  } while (stillExecuting == 1);
	
	if (infile.fd < 0) {
	    printf("Cannot open '%s'(sceOpen)\n", fn);
	    return 0;
	}
	infile.size = sceLseek(infile.fd, 0, SCE_SEEK_END);
	if (infile.size < 0) {
	    printf("sceLseek() fails (%s): %d\n", fn, infile.size);
	    sceClose(infile.fd);
	    return 0;
	}

	ret = sceLseek(infile.fd, 0, SCE_SEEK_SET);
	if (ret < 0) {
	    printf("sceLseek() fails (%s)\n", fn);
	    sceClose(infile.fd);
	    return 0;
	}

	nonBlockReadReq = 0;
    }

    return 1;
}

// ////////////////////////////////////////////////////////////////
//
//  Close a file
//
int strFileClose(void)
{
    if (infile.isOnCD) {
    	sceCdStStop();
        sceSifFreeIopHeap((void *)infile.iopBuf);
    } else {
	sceClose(infile.fd);
    }
    return 1;
}

// ////////////////////////////////////////////////////////////////
//
//  Read data
//
int strFileRead(void *buff, int size)
{
    int count;

    if (infile.isOnCD) {
	u_int err;

        count= sceCdStRead(size >> 11, (u_int *)buff, STMNBLK, &err);
	count <<= 11;
    } else {
        int stillExecuting;
      
        if (!nonBlockReadReq) {
	  sceRead(infile.fd, buff, size);
	  nonBlockReadReq = 1;
	  
	  // //////////////////////////////////////////////////////////////////
	  //
	  //  Important:
	  //  In this non-blocking mode, no I/O services ** including printf **
	  //  can be used between here and a successful SCE_FS_EXECUTING
	  //  sceIoctl command.
	  //
	}
      
	sceIoctl(infile.fd, SCE_FS_EXECUTING, &stillExecuting);
	
	if (!stillExecuting) {
	    count = size;
	    nonBlockReadReq = 0;
	  }
	
	else count = 0;
    }

    return count;
}
