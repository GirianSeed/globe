/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
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
 *                         netcnf_env.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 */

#ifndef __netcnf_env_h_
#define __netcnf_env_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <netcnf.h>

void sceNetcnfifEnvInit(sceNetCnfEnv_t *env, void *mem_area, int size, int f_no_decode);
void sceNetcnfifEnvDump(sceNetCnfEnv_t *e);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__netcnf_env_h_ */
