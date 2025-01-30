/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library  Release 1.6
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                       - mpeg csc on vu1 -
 *
 *                         Version 0.10
 *                           Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            readbuf.h
 *                  header file for read buffer
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.10           12.17.1999      umemura     the first version
 *       0.20           01.17.2000      umemura     vu1
 */
#ifndef _READBUF_H_
#define _READBUF_H_

#include <eetypes.h>

#define READ_UNIT_SIZE (64*1024)
#define N_READ_UNIT     5

// ////////////////////////////////////////////////////////////////
//
// Read buffer
//
typedef struct {
    u_char data[N_READ_UNIT * READ_UNIT_SIZE];
    int put;
    int count;
    int size;
} ReadBuf;

// ////////////////////////////////////////////////////////////////
//
// Functions
//
int readBufCreate(ReadBuf *buff);
void readBufDelete(ReadBuf *buff);
int readBufBeginPut(ReadBuf *buff, u_char **ptr);
int readBufEndPut(ReadBuf *buff, int size);
int readBufBeginGet(ReadBuf *buff, u_char **ptr);
int readBufEndGet(ReadBuf *buff, int size);

#endif // _READBUF_H_

