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
 * ftp.h - header file for ftp.c
 *
 * $Id: ftp.h,v 1.4 2002/03/27 05:22:01 xokano Exp $
 * $Author: xokano $
 *
 */

#ifndef _FTP_H
#define _FTP_H

#include <ctype.h>
#include <kernel.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <inet/inet.h>

#include "hddpart.h"

ModuleInfo Module = { "ftp client program", 0x0101 };

#define INET_DEFAULT_NRETRY     4
#define INET_DEFAULT_TIMEOUT    6000    // in milliseconds
#define INET_INFINITE_TIMEOUT   -1
#define INET_ZERO_TIMEOUT       0

#define BUFFER_SIZE             8*1024
#define DATA_BUFF_SIZE          512*1024

#define FTP_PORT                21
#define FTP_DATA_PORT           20
#define LOCAL_PORT              9000

#define DEL                     0x7f

#define CMD_FLG_PUT              1
#define CMD_FLG_GET              2
#define CMD_FLG_LS               3

#define FTP_REPLY_CODE_SERVICE_READY         220

#define min(x, y)	(((x) < (y))? (x): (y))

typedef struct _command_arg {
    int c;
    char* v[16];
} CmdArg;

typedef struct _command_type {
    char* input_cmd;
    char* ftp_cmd;
    int (*func)(CmdArg* cmd_arg, char* buffer);
} CmdType;
    

#endif  // _FTP_H

