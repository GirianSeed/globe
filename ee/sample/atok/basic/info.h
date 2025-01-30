/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                      ATOK Library Sample
 *
 *                         Version 0.10
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.10
 */

#ifndef __INFO_H__
#define __INFO_H__

extern void info_disp_mode(int mode, int x, int y);
extern void info_disp_common(int x, int y);
extern void info_disp_input_info(int x, int y);
extern void info_disp_conv_info(int x, int y, int can_extend);
extern void info_disp_continue(int x, int y);
extern void info_disp_file_stat(int x, int y);

#endif /* __INFO_H__ */
