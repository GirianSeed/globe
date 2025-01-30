/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/*
 *                      ATOK Library Sample
 *
 *                         Version 0.10
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.10
 */

#ifndef _MTFIFO_H_
#define _MTFIFO_H_

#if defined(__LANGUAGE_C_PLUS_PLUS) || defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif  // defined(__LANGUAGE_C_PLUS_PLUS) || defined(__cplusplus) || defined(c_plusplus)

typedef struct
{
	volatile int nSendReqSize;
	volatile int nRecvReqSize;
	volatile int nInBytes;
	volatile int nQueHead;

	volatile int FifoSema;
	volatile int SendSema;
	volatile int RecvSema;
	volatile int nBufSize;

	volatile char*pData ;
} FIFO;

u_int FifoPollData(FIFO *pFifo, void *pData, int nSize, int fPeek);
int FifoSendData(FIFO *pFifo, const void *pData, int nSize);
u_int FifoRecvData(FIFO *pFifo, void *pData, int nSize, int fPeek);
int FifoPostData(FIFO *pFifo, const void *pData, int nSize);
u_int FifoGetBytes(FIFO *pFifo);
void FifoFlush(FIFO *pFifo);
void FifoDelete(FIFO *pFifo);
int FifoCreate(FIFO *pFifo, void *pBuf, int nBufSize);


#if defined(__LANGUAGE_C_PLUS_PLUS) || defined(__cplusplus) || defined(c_plusplus)
}
#endif  // defined(__LANGUAGE_C_PLUS_PLUS) || defined(__cplusplus) || defined(c_plusplus)

#endif  // !_MTFIFO_H_


