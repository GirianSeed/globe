/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
*/
/*
 *
 *	Copyright (C) 2002 Sony Computer Entertainment Inc.
 *							All Right Reserved
 *
 */
#ifdef __cplusplus
extern "C" {
#endif

#define PI 3.14159265f

typedef struct {
	/* Push(repeatï™ÇèEÇ¢ë±ÇØÇÈ) */
	unsigned	LleftOn		: 1;
	unsigned	LupOn		: 1;
	unsigned	LrightOn	: 1;
	unsigned	LdownOn		: 1;
	unsigned	L1On		: 1;
	unsigned	L2On		: 1;
	unsigned	RleftOn		: 1;
	unsigned	RupOn		: 1;
	unsigned	RrightOn	: 1;
	unsigned	RdownOn		: 1;
	unsigned	R1On		: 1;
	unsigned	R2On		: 1;
	unsigned	SelectOn	: 1;
	unsigned	StartOn		: 1;

	/* Switch(repeatï™ÇÕ èEÇÌÇ»Ç¢) */
	unsigned	LleftSwitch	: 1;
	unsigned	LupSwitch	: 1;
	unsigned	LrightSwitch	: 1;
	unsigned	LdownSwitch	: 1;
	unsigned	L1Switch	: 1;
	unsigned	L2Switch	: 1;
	unsigned	RleftSwitch	: 1;
	unsigned	RupSwitch	: 1;
	unsigned	RrightSwitch	: 1;
	unsigned	RdownSwitch	: 1;
	unsigned	R1Switch	: 1;
	unsigned	R2Switch	: 1;
	unsigned	SelectSwitch	: 1;
	unsigned	StartSwitch	: 1;
} sceSamplePAD;

extern  sceSamplePAD	gPad;

extern void pad_init(void);
extern void pad_ctrl(void);
extern void *file_read(char *);
extern void draw_debug_print_init();
extern void draw_debug_print(int x, int y, char *fmt, ...);
extern void draw_debug_print_exec();
extern void utilInitDMATimer(void);
extern void utilSetDMATimer(void);
extern int utilGetDMATimerEnd(void);
extern int utilGetDMATimerStart(void);
extern void utilResetDMATimer(void);
extern void error_handling(char *file, int line, const char *mes);

#ifdef __cplusplus
}
#endif
