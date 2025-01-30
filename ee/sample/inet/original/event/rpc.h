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
 *                         rpc.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.02.18      tetsu       Change directory
 */

#ifndef __rpc_h_
#define __rpc_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void rpc_init(void);
void rpc_setup(void);
int rpc_check0(void);
int rpc_check1(void);
int rpc_get_event(void);
int rpc_get_id(void);
char *rpc_get_line_speed(void);
char *rpc_get_ip_addr(void);
char *rpc_get_phone_number(void);
void rpc_wait_event(int interface_id);
void rpc_up(int addr);
void rpc_down(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__rpc_h_ */
