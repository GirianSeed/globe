/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
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
 *                         netcnfif.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 *       1.2            2002.02.10      tetsu       Add sceNetcnfifCheckAdditionalAT()
 */

#ifndef __netcnfif_h_
#define __netcnfif_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "../../common/include/netcnfif.h"

void sceNetcnfifInit(void);
void sceNetcnfifSetup(void);
int sceNetcnfifCheck(void);
void sceNetcnfifGetResult(sceNetcnfifArg_t *result);
void sceNetcnfifSetFNoDecode(int f_no_decode);
void sceNetcnfifGetCount(char *fname, int type);
void sceNetcnfifGetList(char *fname, int type, sceNetcnfifList_t *addr, int size);
void sceNetcnfifLoadEntry(char *fname, int type, char *usr_name, sceNetcnfifData_t *addr);
void sceNetcnfifAddEntry(char *fname, int type, char *usr_name);
void sceNetcnfifEditEntry(char *fname, int type, char *usr_name, char *new_usr_name);
void sceNetcnfifDeleteEntry(char *fname, int type, char *usr_name);
void sceNetcnfifSetLatestEntry(char *fname, int type, char *usr_name);
void sceNetcnfifDeleteAll(char *fname);
void sceNetcnfifCheckCapacity(char *fname);
void sceNetcnfifCheckAdditionalAT(char *additional_at);
void sceNetcnfifGetAddr(void);
void sceNetcnfifAllocWorkarea(int size);
void sceNetcnfifFreeWorkarea(void);
void sceNetcnfifSetEnv(int type);

int sceNetcnfifSendIOP(unsigned int data, unsigned int addr, unsigned int size);
int sceNetcnfifDmaCheck(int id);

void sceNetcnfifDataInit(sceNetcnfifData_t *data);
void sceNetcnfifDataDump(sceNetcnfifData_t *data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__netcnfif_h_ */
