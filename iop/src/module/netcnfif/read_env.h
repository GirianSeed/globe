/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
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
 *                         read_env.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 */

#ifndef __read_env_h_
#define __read_env_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <netcnf.h>

#include "../../../../common/include/netcnfif.h"

int sceNetcnfifReadEnv(sceNetcnfifData_t *data, sceNetCnfEnv_t *e, int type);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__read_env_h_ */
