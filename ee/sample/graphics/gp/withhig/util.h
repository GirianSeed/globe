/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *        Emotion Engine Library Sample Program
 *
 *                         - libgp: withhig -
 *
 *                         Version 1.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            util.h
 *			    libgp + HiG
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            Sep,20,2001      aoki
 */
#ifndef __UTIL_H__
#define __UTIL_H__

typedef struct {
    /* Switch : while pushed, 1 */
    unsigned	L2Switch	: 1;
    unsigned	R2Switch	: 1;
    unsigned	L1Switch	: 1;
    unsigned	R1Switch	: 1;
    unsigned	RupSwitch	: 1;
    unsigned	RrightSwitch    : 1;
    unsigned	RdownSwitch	: 1;
    unsigned	RleftSwitch	: 1;
    unsigned	SelectSwitch	: 1;
    unsigned	reserve3	: 1;
    unsigned	reserve4	: 1;
    unsigned	StartSwitch	: 1;
    unsigned	LupSwitch	: 1;
    unsigned	LrightSwitch	: 1;
    unsigned	LdownSwitch	: 1;
    unsigned	LleftSwitch	: 1;

    /* On : while pushed, 1 */
    unsigned	L2On		: 1;
    unsigned	R2On		: 1;
    unsigned	L1On		: 1;
    unsigned	R1On		: 1;
    unsigned	RupOn		: 1;
    unsigned	RrightOn     	: 1;
    unsigned	RdownOn		: 1;
    unsigned	RleftOn		: 1;
    unsigned	SelectOn	: 1;
    unsigned	reserve1	: 1;
    unsigned	reserve2	: 1;
    unsigned	StartOn		: 1;
    unsigned	LupOn		: 1;
    unsigned	LrightOn	: 1;
    unsigned	LdownOn		: 1;
    unsigned	LleftOn		: 1;
} PadData;

extern  PadData	gPad;

enum {
    PARA_LIGHT	= 0,
    POINT_LIGHT	= 1,
    SPOT_LIGHT	= 2
};

extern  PadData	gPad;

extern void pad_init(void);
extern void pad_ctrl(void);
extern void *file_read(char *fname);
extern void draw_debug_print_init(void);
extern void draw_debug_print(int x, int y, char *fmt, ...);
extern void draw_debug_print_exec(void);
extern void error_handling(char *file, int line, const char *mes);

#endif /* __UTIL_H__ */
