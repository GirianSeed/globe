/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0.2
 $Id: write_env.h,v 1.2 2002/10/19 12:32:57 tetsu Exp $
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
 *                         write_env.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 *       1.2            2002.10.19      tetsu       Change path of netcnfif.h
 */

#ifndef __write_env_h_
#define __write_env_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <netcnf.h>
#include <netcnfif.h>

int sceNetcnfifWriteEnv(sceNetCnfEnv_t *e, sceNetcnfifData_t *data, int type);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__write_env_h_ */
