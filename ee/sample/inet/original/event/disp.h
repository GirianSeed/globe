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
 *                         disp.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.02.18      tetsu       Change directory
 */

#ifndef __disp_h_
#define __disp_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void init_disp(void);
void add_disp(char *str_top);
void print_disp(int console);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__disp_h_ */
