/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.7.1
 */
/* 
 *
 *      Copyright (C) 2002-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2003/01/08      ksh         First version
 */

#ifndef __callback_h_
#define __callback_h_

#include <netcnfif.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int sce_create_callback_thread(sceNetcnfifCallback_t *callback, char *stack, int size, int priority);
void sce_delete_callback_thread(int tid);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__callback_h_ */

