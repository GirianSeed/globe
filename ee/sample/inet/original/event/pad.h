/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *      Inet Event Sample Program
 *
 *                         Version 1.1
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         pad.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.02.18      tetsu       Change directory
 */

#ifndef __pad_h_
#define __pad_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void pad_setup(void);
u_short pad_read(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__pad_h_ */
