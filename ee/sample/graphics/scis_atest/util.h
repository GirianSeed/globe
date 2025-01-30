/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: util.h,v 1.1 2002/10/18 12:26:13 aoki Exp $
 */
/* 
 * Emotion Engine Library Sample Program
 *
 * Copyright (C) 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * scis_at - util.h
 *	
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2002-10-08      aoki        
 */
#ifdef __cplusplus
extern "C" {
#endif
u_int pad_read(void);
void pad_init(void);
void draw_debug_print_init();
void draw_debug_print(char *fmt, ...);
void draw_debug_print_exec();
#ifdef __cplusplus
}
#endif
