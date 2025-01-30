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
 *                         gs_init.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.02.18      tetsu       Change directory
 */

#ifndef __gs_init_h_
#define __gs_init_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <libgraph.h>

#define SCREEN_WIDTH  (640)
#define SCREEN_HEIGHT (224)

#define OFFX (((4096 - SCREEN_WIDTH) / 2) << 4)
#define OFFY (((4096 - SCREEN_HEIGHT) / 2) << 4)

extern sceGsDBuff db;

void gs_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__gs_init_h_ */
