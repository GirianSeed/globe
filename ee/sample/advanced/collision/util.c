/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          Name : util.c
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            1.0     Oct 27, 2000     kazama      Initial
 */
#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libvu0.h>
#include <libpad.h>
#include <sifdev.h>
#include <malloc.h>
#include <libdev.h>
#include <libhig.h>
#include <libhip.h>
#include "util.h"
#include "camera.h"
#include "light.h"

/* FOR PAD CTRL */
sceSamplePAD	gPad;
static u_long128 _pad_buf[scePadDmaBufferMax] __attribute__((aligned(64)));

/* FOR CAMERA CTRL */
#define PI	3.14159265f
#define OFFX	2048
#define OFFY	2048
#define SCREEN_WIDTH	(640.0f)
#define SCREEN_HEIGHT	(224.0f)
#define CLIP_ALPHA	(600.0f)	/* extra clipping area width */
static sceVu0FVECTOR	clip_volume = {1000.0f, 1000.0f, 1.0f, 65535.0f};
CameraPos	camerapos;
static CameraPos camerapos_init = {
    { 0.0f, 0.0f, -30.0f, 0.0f },		// pos
    { 0.0f, 0.0f, 1.0f, 1.0f },			// zdir
   { 0.0f, 1.0f, 0.0f, 1.0f },			// ydir
//    { -0.660f, 0.340f, 0.0f, 0.0f },			// rot
    { 0.0f, 0.0f, 0.0f, 0.0f },                  // rot      
};
CAMERA_DAT cam;

static int _dma_timer_s, _dma_timer_e;

typedef struct {
    sceVu0FMATRIX	vector;
    sceVu0FMATRIX	point;
    sceVu0FMATRIX	color;
} micro_light_t;

typedef struct {
    sceVu0FMATRIX	wscr;
    sceVu0FMATRIX	wclp;
    sceVu0FMATRIX	cscr;
    sceVu0FMATRIX	init_data;
    micro_light_t	lightgrp[3];
} micro_data_t;

#define Vu1MicroSetLightVect(mic, grp, vec0, vec1, vec2) \
		sceVu0NormalLightMatrix(((micro_data_t *)(mic))->lightgrp[(grp)].vector, vec0, vec1, vec2); \
		((micro_data_t *)(mic))->lightgrp[(grp)].vector[3][3] = 1.0f;

#define Vu1MicroSetLightColor(mic, grp, col0, col1, col2, col3) \
		sceVu0LightColorMatrix(((micro_data_t *)(mic))->lightgrp[(grp)].color, col0, col1, col2, col3);

#define Vu1MicroSetLightPoint(mic, grp, point0, point1, point2) \
		sceVu0CopyVector(((micro_data_t *)(mic))->lightgrp[(grp)].point[0], point0); \
		sceVu0CopyVector(((micro_data_t *)(mic))->lightgrp[(grp)].point[1], point1); \
		sceVu0CopyVector(((micro_data_t *)(mic))->lightgrp[(grp)].point[2], point2);

static int	_consID;		/* debug console ID */

void get_camx(float *);
void get_camy(float *);
void get_camz(float *);

/*
  light[0] : parallel light
  light[1] : point light
  light[2] : spot light
*/
_light_t	_light[3][3];

/* default light */
static _light_t	_para_light_def = {
    {1.0f, 1.0f, 1.0f, 0.0f},		/* color */
    {1.0f, 2.0f, 1.0f, 0.0f},		/* vector(direction) */
    {0.0f, 0.0f, 0.0f, 0.0f},		/* point (not used for Para-Light */
    400.0f, 0.12f
};

static _light_t _spot_light_def[3] = {
    {	/* LIGHT_0 */
	{0.2f, 0.0f, 0.0f, 0.0f},		/* color */
	{-1.0f, 0.0f, 0.0f, 0.0f},		/* vector */
	{-7.5f, -3.0f, 6.0f, 1.0f},		/* point */
	400.0f, 0.12f
    },
    {	/* LIGHT_1 */
	{0.0f, 0.2f, 0.0f, 0.0f},
	{1.0f, 0.0f, 0.0f, 0.0f},
	{7.5f, 0.0f, 0.0f, 0.0f},
	400.0f, 0.12f
    },
    {	/* LIGHT_2 */
	{0.0f, 0.0f, 0.2f, 0.0f},
	{0.0f, 0.0f, -1.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 0.0f},
	400.0f, 0.12f
    }
};

static _light_t _point_light_def[3] = {
    {	/* LIGHT_0 */
	{0.2f, 0.0f, 0.0f, 0.0f},		/* color */
	{-1.0f, 0.0f, 0.0f, 0.0f},		/* vector */
	{-37.5f, -3.0f, 100.0f, 0.0f},		/* point */
	0.0f, 0.12f
    },
    {	/* LIGHT_1 */
	{0.0f, 0.2f, 0.0f, 0.0f},
	{1.0f, 0.0f, 0.0f, 0.0f},
	{7.5f, 0.0f, 0.0f, 0.0f},
	400.0f, 0.12f
    },
    {	/* LIGHT_2 */
	{0.0f, 0.0f, 0.2f, 0.0f},
	{0.0f, 0.0f, 1.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 0.0f},
	0.0f, 0.12f
    }
};

static sceVu0FVECTOR	ambient = {0.3f, 0.3f, 0.3f, 0.3f};

void pad_init(void)
{
	scePadInit(0);
	scePadPortOpen(0, 0, _pad_buf);
}

void pad_ctrl(void)
{
	static u_short	oldpad = 0;
	u_short		pad;
	u_char		rdata[32];

	if (scePadRead(0, 0, rdata) > 0) 
		pad = 0xffff ^ ((rdata[2] << 8) | rdata[3]);
	else
		pad = 0;

	gPad.LleftOn	= pad & SCE_PADLleft ? 1 : 0;
	gPad.LupOn	= pad & SCE_PADLup ? 1 : 0;
	gPad.LrightOn	= pad & SCE_PADLright ? 1 : 0;
	gPad.LdownOn	= pad & SCE_PADLdown ? 1 : 0;
	gPad.L1On	= pad & SCE_PADL1 ? 1 : 0;
	gPad.L2On	= pad & SCE_PADL2 ? 1 : 0;
	gPad.RleftOn	= pad & SCE_PADRleft ? 1 : 0;
	gPad.RupOn	= pad & SCE_PADRup ? 1 : 0;
	gPad.RrightOn	= pad & SCE_PADRright ? 1 : 0;
	gPad.RdownOn	= pad & SCE_PADRdown ? 1 : 0;
	gPad.SelectOn	= pad & SCE_PADselect ? 1 : 0;
	gPad.StartOn	= pad & SCE_PADstart ? 1 : 0;
	gPad.R1On	= pad & SCE_PADR1 ? 1 : 0;
	gPad.R2On	= pad & SCE_PADR2 ? 1 : 0;
	gPad.LleftSwitch	= (pad & SCE_PADLleft) && (!(oldpad & SCE_PADLleft)) ? 1 : 0;
	gPad.LupSwitch		= (pad & SCE_PADLup) && (!(oldpad & SCE_PADLup)) ? 1 : 0;
	gPad.LrightSwitch	= (pad & SCE_PADLright) && (!(oldpad & SCE_PADLright)) ? 1 : 0;
	gPad.LdownSwitch	= (pad & SCE_PADLdown) && (!(oldpad & SCE_PADLdown)) ? 1 : 0;
	gPad.L1Switch		= (pad & SCE_PADL1) && (!(oldpad & SCE_PADL1)) ? 1 : 0;
	gPad.L2Switch		= (pad & SCE_PADL2) && (!(oldpad & SCE_PADL2)) ? 1 : 0;
	gPad.RleftSwitch	= (pad & SCE_PADRleft) && (!(oldpad & SCE_PADRleft)) ? 1 : 0;
	gPad.RupSwitch		= (pad & SCE_PADRup) && (!(oldpad & SCE_PADRup)) ? 1 : 0;
	gPad.RrightSwitch	= (pad & SCE_PADRright) && (!(oldpad & SCE_PADRright)) ? 1 : 0;
	gPad.RdownSwitch	= (pad & SCE_PADRdown) && (!(oldpad & SCE_PADRdown)) ? 1 : 0;
	gPad.R1Switch		= (pad & SCE_PADR1) && (!(oldpad & SCE_PADR1)) ? 1 : 0;
	gPad.R2Switch		= (pad & SCE_PADR2) && (!(oldpad & SCE_PADR2)) ? 1 : 0;
	gPad.SelectSwitch	= (pad & SCE_PADselect) && (!(oldpad & SCE_PADselect)) ? 1 : 0;
	gPad.StartSwitch	= (pad & SCE_PADstart) && (!(oldpad & SCE_PADstart)) ? 1 : 0;

	oldpad = pad;
}

void *file_read(char *fname)
{
	char	file[256];
	size_t	size;
	void	*dptr;
	int	fd;

	sprintf(file, "host0:%s", fname);
	if ((fd = sceOpen(file, SCE_RDONLY)) < 0) {
		printf("Can't open file %s\n", fname);
		while (1)
			;
	}
	size = sceLseek(fd, 0, SCE_SEEK_END);
	sceLseek(fd, 0, SCE_SEEK_SET);

	dptr = memalign(64, size);
	if (dptr == NULL) {
		printf("Can't allocate memory for file reading %s\n", fname);
		while (1)
			;
	}
	if (sceRead(fd, (u_int *)dptr, size) < 0) {
		printf("Can't read file %s\n", fname);
		while (1)
			;
	}
	FlushCache(0);
	sceClose(fd);
	return dptr;
}

void light_set_one(int gp, int num, _light_t *lt)
{
    sceVu0CopyVector(_light[gp][num].vector, lt->vector);
    sceVu0CopyVector(_light[gp][num].color,  lt->color);
    sceVu0CopyVector(_light[gp][num].point,  lt->point);
    _light[gp][num].intensity = lt->intensity;
    _light[gp][num].angle = lt->angle;
}

void light_init(void)
{
    _light_t	zero_light = {
	{0.0f, 0.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 0.0f}
    };

    light_set_one(PARA_LIGHT, 0, &_para_light_def);
    light_set_one(PARA_LIGHT, 1, &zero_light);
    light_set_one(PARA_LIGHT, 2, &zero_light);
    light_set_one(POINT_LIGHT, 0, &_point_light_def[0]);
    light_set_one(POINT_LIGHT, 1, &_point_light_def[1]);
    light_set_one(POINT_LIGHT, 2, &_point_light_def[2]);
    light_set_one(SPOT_LIGHT, 0, &_spot_light_def[0]);
    light_set_one(SPOT_LIGHT, 1, &_spot_light_def[1]);
    light_set_one(SPOT_LIGHT, 2, &_spot_light_def[2]);
}

void light_set(u_int *micro_data_addr)
{
    int	gp;
    sceVu0FMATRIX	vec, col, pnt;

    /* apply light */
    for (gp = 0; gp < 3; gp++) {
	sceVu0CopyVector(vec[0], _light[gp][0].vector);
	sceVu0CopyVector(vec[1], _light[gp][1].vector);
	sceVu0CopyVector(vec[2], _light[gp][2].vector);
	sceVu0CopyVector(col[0], _light[gp][0].color);
	sceVu0CopyVector(col[1], _light[gp][1].color);
	sceVu0CopyVector(col[2], _light[gp][2].color);
	sceVu0CopyVector(pnt[0], _light[gp][0].point);
	sceVu0CopyVector(pnt[1], _light[gp][1].point);
	sceVu0CopyVector(pnt[2], _light[gp][2].point);
	col[0][3] = _light[gp][0].intensity;
	col[1][3] = _light[gp][1].intensity;
	col[2][3] = _light[gp][2].intensity;
	pnt[0][3] = _light[gp][0].angle;
	pnt[1][3] = _light[gp][1].angle;
	pnt[2][3] = _light[gp][2].angle;

	Vu1MicroSetLightVect(micro_data_addr, gp, vec[0], vec[1], vec[2]);
	Vu1MicroSetLightColor(micro_data_addr, gp, col[0], col[1], col[2], ambient);
	Vu1MicroSetLightPoint(micro_data_addr, gp, pnt[0], pnt[1], pnt[2]);
    }
}

void light_ctrl(int gp)
{
    static const char *target_name[] = {
	"POINT", "VECTOR", "COLOR", "INTENT", "ANGLE"
    };
#define MOVE_MIN	(.1f)
    static unsigned int	num = 0;	/* light no. */
    static unsigned int	item = 0;	/* item no. */

    /* change light no. by Lleft/Lright */
    if (gPad.LleftSwitch)
	num--;
    if (gPad.LrightSwitch)
	num++;
    num = num > 2 ? 0 : num;

    /* change item no. by Lup/Ldown */
    if (gPad.LupSwitch)
	item--;
    if (gPad.LdownSwitch)
	item++;
    item = item > 4 ? 0 : item;

    switch (item) {
      case 0: /* point */
	if (gPad.RleftOn)	_light[gp][num].point[0] -= MOVE_MIN * 10;
	if (gPad.RrightOn)	_light[gp][num].point[0] += MOVE_MIN * 10;
	if (gPad.RupOn)		_light[gp][num].point[1] -= MOVE_MIN * 10;
	if (gPad.RdownOn)	_light[gp][num].point[1] += MOVE_MIN * 10;
	if (gPad.R1On)		_light[gp][num].point[2] += MOVE_MIN * 10;
	if (gPad.R2On)		_light[gp][num].point[2] -= MOVE_MIN * 10;
	break;
      case 1: /* vector */
	if (gPad.RleftOn)	_light[gp][num].vector[0] -= MOVE_MIN;
	if (gPad.RrightOn)	_light[gp][num].vector[0] += MOVE_MIN;
	if (gPad.RupOn)		_light[gp][num].vector[1] -= MOVE_MIN;
	if (gPad.RdownOn)	_light[gp][num].vector[1] += MOVE_MIN;
	if (gPad.R1On)		_light[gp][num].vector[2] += MOVE_MIN;
	if (gPad.R2On)		_light[gp][num].vector[2] -= MOVE_MIN;
	break;
      case 2: /* color */
	if (gPad.RdownOn)	_light[gp][num].color[0] += MOVE_MIN / 10.0f;
	if (gPad.RupOn)		_light[gp][num].color[0] -= MOVE_MIN / 10.0f;
	if (gPad.RrightOn)	_light[gp][num].color[1] -= MOVE_MIN / 10.0f;
	if (gPad.RleftOn)	_light[gp][num].color[1] += MOVE_MIN / 10.0f;
	if (gPad.R1On)		_light[gp][num].color[2] -= MOVE_MIN / 10.0f;
	if (gPad.R2On)		_light[gp][num].color[2] += MOVE_MIN / 10.0f;
	if (_light[gp][num].color[0] < 0.0f)
	    _light[gp][num].color[0] = 0.0f;
	if (_light[gp][num].color[0] > 1.0f)
	    _light[gp][num].color[0] = 1.0f;
	if (_light[gp][num].color[1] < 0.0f)
	    _light[gp][num].color[1] = 0.0f;
	if (_light[gp][num].color[1] > 1.0f)
	    _light[gp][num].color[1] = 1.0f;
	if (_light[gp][num].color[2] < 0.0f)
	    _light[gp][num].color[2] = 0.0f;
	if (_light[gp][num].color[2] > 1.0f)
	    _light[gp][num].color[2] = 1.0f;
	break;
      case 3: /* intensity */
	if (gPad.RupOn)		_light[gp][num].intensity += MOVE_MIN * 10.0f;
	if (gPad.RdownOn)	_light[gp][num].intensity -= MOVE_MIN * 10.0f;
	break;
      case 4: /* angle */
	if (gPad.RupOn)		_light[gp][num].angle += MOVE_MIN / 10.0f;
	if (gPad.RdownOn)	_light[gp][num].angle -= MOVE_MIN / 10.0f;
	if (_light[gp][num].angle > 1.0f)
	    _light[gp][num].angle = 1.0f;
	if (_light[gp][num].angle < 0.0f)
	    _light[gp][num].angle = 0.0f;
	break;
    }    

    /* information */
    draw_debug_print(1, 9, "LIGHT NUMBER %d", num);
    draw_debug_print(1, 10, "TARGET ITEM %s", target_name[item]);
    draw_debug_print(1, 11, "  POINT  %5.2f %5.2f %5.2f",
		     _light[gp][num].point[0],
		     _light[gp][num].point[1],
		     _light[gp][num].point[2]);
    draw_debug_print(1, 12, "  VECTOR %5.2f %5.2f %5.2f",
		     _light[gp][num].vector[0],
		     _light[gp][num].vector[1],
		     _light[gp][num].vector[2]);
    draw_debug_print(1, 13,"  COLOR  %5.2f %5.2f %5.2f",
		     _light[gp][num].color[0],
		     _light[gp][num].color[1],
		     _light[gp][num].color[2]);
    draw_debug_print(1, 14,"  INTENT %5.2f", _light[gp][num].intensity);
    draw_debug_print(1, 15,"  ANGLE  %5.2f", _light[gp][num].angle);

    light_draw(gp, num);
}


void light_draw(int gp, int num)
{
union QWdata {
    u_long128	ul128;
    u_long		ul64[2];
    u_int		ui32[4];
    sceVu0FVECTOR	fvect;
    sceVu0IVECTOR	ivect;
};
sceVu0FVECTOR cube[14] = {
                        {-5.0, -5.0, 5.0, 0.0},
                        {-5.0, -5.0, -5.0, 0.0},
                        {-5.0, 5.0, 5.0, 0.0},
                        {-5.0, 5.0, -5.0, 0.0},
                        {5.0, 5.0, -5.0, 0.0},
                        {-5.0, -5.0, -5.0, 0.0},
                        {5.0, -5.0, -5.0, 0.0},
                        {-5.0, -5.0, 5.0, 0.0},
                        {5.0, -5.0, 5.0, 0.0},
                        {-5.0, 5.0, 5.0, 0.0},
                        {5.0, 5.0, 5.0, 0.0},
                        {5.0, 5.0, -5.0, 0.0},
                        {5.0, -5.0, 5.0, 0.0},
                        {5.0, -5.0, -5.0, 0.0}};
    int			j;
    struct {
	union QWdata	buf[29];
	u_int		count;
    } pack __attribute__ ((aligned(16)));

#define LIGHT_CUBE_SIZE	0.1
    sceVu0FMATRIX	work;
    sceVu0FVECTOR	v00, v01;
    sceVu0IVECTOR	v02;
    sceVu0FVECTOR	color;
    sceVu0FVECTOR	point;
    sceVu0FVECTOR	rot;

    sceVu0ScaleVector(color, _light[gp][num].color, 255.0f);
    sceVu0ClampVector(color, color, 0.0f, 255.0f);
    sceVu0CopyVector(point, _light[gp][num].point);
    sceVu0CopyVector(rot,   _light[gp][num].vector);

    point[3] = 1.0f;

    pack.count = 0;
    pack.buf[pack.count].ul64[0] = SCE_GIF_SET_TAG(14, 1, 1, 0x04, 0, 2);
    pack.buf[pack.count].ul64[1] = 0x0041L;
    pack.count++;
    sceVu0UnitMatrix(work);
    for (j = 0; j < 14; j++) {
	sceVu0ApplyMatrix(v00, work, cube[j]);
	sceVu0ScaleVector(v00, v00, LIGHT_CUBE_SIZE);
	sceVu0AddVector(v01, v00, point);
	sceVu0RotTransPers(v02, cam.world_screen, v01, 0);
	sceVu0FTOI0Vector(pack.buf[pack.count++].ivect, color);
	pack.buf[pack.count++].ul128 = *((u_long128 *)v02);
    }
    *D2_QWC = pack.count;
    *D2_MADR = (u_int) &pack.buf & 0x0fffffff;
    *D_STAT = 2;
    FlushCache(0);
    *D2_CHCR = 1 | (0 << 2) | (0 << 4) | (0 << 5) | (0 << 6) | (0 << 7) | (1 << 8);
    sceGsSyncPath(0, 0);
}

void camera_init(void)
{
    camerapos = camerapos_init;
    Camera_SetCamera_fromCameraPos(&cam, &camerapos);
    Camera_SetClipVolume(&cam, clip_volume);
    Camera_SetClipScreen(&cam, 512.0f, 1.0f, 0.47f,
			 (float)OFFX, (float)OFFY,
			 5.0f, 16777000.0f,
			 1.0f, 65536.0f);
    Camera_SetClipData(&cam, SCREEN_WIDTH + CLIP_ALPHA, SCREEN_HEIGHT * 2.0f + CLIP_ALPHA, 1.0f, 65536.0f);
    Camera_SetMatrix(&cam);

}

void camera_set(u_int *micro_data_addr)
{
    sceVu0FMATRIX	work;
    micro_data_t	*mdata;
    sceVu0FVECTOR	pos;
    float		fPn, fSn, sw, sh;

    Camera_SetCamera_fromCameraPos(&cam, &camerapos);
    Camera_SetMatrix(&cam);
    Vu1MicroSetWorldScreen(micro_data_addr, cam.world_screen);
    Vu1MicroSetWorldClip(micro_data_addr, cam.world_clip);
    Vu1MicroSetClipScreen(micro_data_addr, cam.clip_screen);

    mdata = (micro_data_t *)micro_data_addr;
    sceVu0UnitMatrix(work);
    sceVu0RotMatrixX(work, work, camerapos.rot[0]);
    sceVu0RotMatrixY(work, work, camerapos.rot[1]);
    sceVu0RotMatrixZ(work, work, camerapos.rot[2]);
    sceVu0ApplyMatrix(pos, work, camerapos.pos);
    mdata->init_data[0][0] = pos[0];
    mdata->init_data[0][1] = pos[1];
    mdata->init_data[0][2] = pos[2];

    fPn = cam.far + cam.near;
    fSn = cam.far - cam.near;
    sw  = cam.screen_w;
    sh  = cam.screen_h;
    mdata->init_data[2][0] = fSn / sw;
    mdata->init_data[2][1] = fSn / sh;
    mdata->init_data[2][2] = fPn / 2.0f;
    mdata->init_data[2][3] = fSn / 2.0f;
    mdata->init_data[3][0] = mdata->init_data[3][1] = 1.2345678f;
}

void camera_ctrl(void)
{
    float  dr = 0.50f / 180.0f * PI;
    float  dt = -0.1f;

	if (gPad.LdownOn) {
		camerapos.rot[0]+=dr; 
		if(camerapos.rot[0]>PI) camerapos.rot[0] -= 2.0f*PI; 
	}
	if(gPad.LupOn) {
		camerapos.rot[0]-=dr;
		if(camerapos.rot[0]<-PI) camerapos.rot[0] += 2.0f*PI; 
	}
	if(gPad.LrightOn) {
		camerapos.rot[1]-=dr;
		if(camerapos.rot[1]<-PI) camerapos.rot[1] += 2.0f*PI; 
	}
	if(gPad.LleftOn) {
		camerapos.rot[1]+=dr;
		if(camerapos.rot[1]>PI) camerapos.rot[1] -= 2.0f*PI; 
	}
	if(gPad.L1On) {
		camerapos.rot[2]-=dr;
		if(camerapos.rot[2]<-PI) camerapos.rot[2] += 2.0f*PI; 
	}
	if(gPad.L2On) {
		camerapos.rot[2]+=dr;
		if(camerapos.rot[2]>PI) camerapos.rot[2] -= 2.0f*PI; 
	}
	if(gPad.RdownOn){
		camerapos.pos[1]-=dt;
                //vu1d_available();     
        }
	if(gPad.RupOn)
		camerapos.pos[1] += dt;
	if(gPad.RrightOn)
		camerapos.pos[0] -= dt;
	if(gPad.RleftOn)
		camerapos.pos[0] += dt;
	if(gPad.R1On)
		camerapos.pos[2] += dt;
	if(gPad.R2On)
		camerapos.pos[2] -= dt;

	/* infomation */
	draw_debug_print(1, 6, "CAM POS : %5.2f %5.2f %5.2f",
			 camerapos.pos[0],
			 camerapos.pos[1],
			 camerapos.pos[2]);
	draw_debug_print(1, 7, "CAM ROT : %5.2f %5.2f %5.2f",
			 camerapos.rot[0],
			 camerapos.rot[1],
			 camerapos.rot[2]);
			 
}

void fog_ctrl(u_int *micro_data_addr, float start, float end)
{
    float	fogA, fogB;
    micro_data_t	*mdata;

    fogA = 1 / (start - end);
    fogB = end;
    mdata = (micro_data_t *) micro_data_addr;
    mdata->init_data[1][0] = fogA;
    mdata->init_data[1][1] = fogB;
}

void anti_ctrl(u_int *micro_data_addr, float antiparam)
{
    micro_data_t	*mdata;

    if (antiparam < 0.09f)
	antiparam = 0.09f;
    if (antiparam > 2.0f)
	antiparam = 2.0f;
    mdata = (micro_data_t *) micro_data_addr;
    mdata->init_data[0][3] = antiparam;
}

void get_camx(float *camx)
{
	*camx = camerapos.pos[0];
}

void get_camy(float *camy)
{
	*camy = camerapos.pos[1];
}

void get_camz(float *camz)
{
	*camz = camerapos.pos[2];
}

void draw_debug_print_init(void)
{
    sceDevConsInit();
    _consID = sceDevConsOpen(((4096 - 640) / 2) << 4,
			     ((4096 - 224) / 2) << 4,
			     79,
			     40);
}

void draw_debug_print(int x, int y, char *fmt, ...)
{
	va_list	ap;
	char	strings[640 / 4];

	va_start(ap, fmt);
	vsprintf(strings, fmt, ap);
	va_end(ap);

	if (strings[0]) {
		sceDevConsLocate(_consID, x, y);
		sceDevConsPrintf(_consID, strings);
	}	
}

void draw_debug_print_exec(void)
{
	sceDevConsDraw(_consID);
	sceDevConsClear(_consID);
}


static int _dma_timer_handler(int ch)
{
    _dma_timer_e = *T0_COUNT;

    ExitHandler();
    return 0;
}

void utilInitDMATimer(void)
{
    AddDmacHandler(DMAC_VIF1, _dma_timer_handler, 0);
    EnableDmac(DMAC_VIF1);
}

void utilSetDMATimer(void)
{
    _dma_timer_s = *T0_COUNT;
}

int utilGetDMATimerStart(void)
{
    return _dma_timer_s;
}

int utilGetDMATimerEnd(void)
{
    return _dma_timer_e;
}

void utilResetDMATimer(void)
{
    _dma_timer_s = _dma_timer_e = 0;
}

void error_handling(char *file, int line, const char *mes)
{
    printf("%s:%d: %s\n",file,line,mes);
    while (1)
	;
}
