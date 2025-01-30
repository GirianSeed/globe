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
 *                         netcnfif_data.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 */

#ifndef __netcnfif_data_h_
#define __netcnfif_data_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "../../../../common/include/netcnfif.h"

void sceNetcnfifDataInit(sceNetcnfifData_t *data);
void sceNetcnfifDataDump(sceNetcnfifData_t *data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__netcnfif_data_h_ */
