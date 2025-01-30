/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: dma.h,v 1.2 2002/10/19 12:31:42 tetsu Exp $
 */
/* 
 *      Netcnf Interface Library
 *
 *                         Version 1.2
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
 *       1.2            2002.10.19      tetsu       Remove sceNetcnfifDmaCheck()
 */

#ifndef __dma_h_
#define __dma_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int sceNetcnfifSendEE(unsigned int data, unsigned int addr, unsigned int size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__dma_h_ */
