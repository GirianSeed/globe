/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0.2
 */
/* 
 *
 *      Copyright (C) 2002-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.0            2003/02/26      ksh         First version
 */

#ifndef __rpc_h_
#define __rpc_h_

#include <netcnfif.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int sce_call_rpc(sceSifClientData *pcd, void *buf, int type);
int sce_sync(int mode);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__rpc_h_ */

