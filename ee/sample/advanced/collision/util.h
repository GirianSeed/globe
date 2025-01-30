#ifndef __PAD_H__
#define __PAD_H__

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

enum {
    PARA_LIGHT	= 0,
    POINT_LIGHT	= 1,
    SPOT_LIGHT	= 2
};

typedef struct {
    sceVu0FVECTOR	color;		/* R/G/B */
    sceVu0FVECTOR	vector;		/* X/Y/Z */
    sceVu0FVECTOR	point;		/* X/Y/Z */
    float		intensity;	/* for Point/Spot */
    float		angle;		/* for Spot */
} _light_t;

extern  sceSamplePAD	gPad;

extern void pad_init(void);
extern void pad_ctrl(void);
extern void camera_init(void);
extern void camera_ctrl(void);
extern void camera_set(u_int *);
extern void light_init(void);
extern void light_ctrl(int);
extern void light_set(u_int *);
extern void light_draw(int, int);
extern void fog_ctrl(u_int *, float, float);
extern void anti_ctrl(u_int *, float);
extern void get_camx(float *);
extern void get_camy(float *);
extern void get_camz(float *);
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

#endif //__PAD_H__
