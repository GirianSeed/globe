/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *
 *      Copyright (C) 1998-2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version   Date           Design              Log
 *  --------------------------------------------------------------------
 *       1.00      Nov,07,2001    aoki
 */

extern void init_bg_texture();
extern void load_bg_texture(int fbp, int field);
extern u_int pad_read(void);
extern void pad_init(void);
extern void draw_debug_print_init();
extern void draw_debug_print(int x, int y, char *fmt, ...);
extern void draw_debug_print_exec();
