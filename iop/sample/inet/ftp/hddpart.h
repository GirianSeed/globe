/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                    I/O Processor Library
 *
 *                           - ftp -
 *
 *                        Version 1.0.0
 *                          Shift-JIS
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 * hddpart.h - header file for hddpart.c
 *
 * $Id: hddpart.h,v 1.4 2002/03/27 05:22:01 xokano Exp $
 * $Author: xokano $
 *
 */

#ifndef _HDDPART_H
#define _HDDPART_H

#include <ctype.h>
#include <kernel.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <dirent.h>
#include <stdlib.h>

#define MAX_PATH                1024
#define SECTOR_SIZE             512

int showHddDir(char *dir);

void releaseHdd(char* path);

int setupHdd(char* path, char* cwd, int mode);

void updateCwd(char *p, char* cwd);



#endif //_HDDPART_H
