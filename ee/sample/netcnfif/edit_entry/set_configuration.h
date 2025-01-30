/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *      Netcnf Interface Library Sample Program
 *
 *                         Version 1.1
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         set_configuration.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 */

#ifndef __set_configuration_h_
#define __set_configuration_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void set_net_cnf(sceNetcnfifData_t *data, char *ifc, char *dev);
void set_attach_cnf(sceNetcnfifData_t *data, int type, int pattern);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __set_configuration_h_ */
