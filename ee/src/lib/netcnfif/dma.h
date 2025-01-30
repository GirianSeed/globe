/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/* 
 *      Netcnf Interface Library
 *
 *                         Version 1.1
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         dma.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 */

#ifndef __dma_h_
#define __dma_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int sceNetcnfifSendIOP(unsigned int data, unsigned int addr, unsigned int size);
int sceNetcnfifDmaCheck(int id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__dma_h_ */
