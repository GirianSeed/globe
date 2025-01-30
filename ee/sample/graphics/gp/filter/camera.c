/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *        Emotion Engine Library Sample Program
 *
 *                         - libgp: filtering -
 *
 *                         Version 1.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            camera.c
 *                     simple 2D filtering w/ libgp
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            Sep,20,2001      aoki
 */
#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libdma.h>
#include <libvu0.h>
#include <libdev.h>
#include <sifdev.h>
#include <libpad.h>
#include <string.h>
#include <malloc.h>
#include "util.h"
#include "camera.h"

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



/*
  light[0] : parallel light
  light[1] : point light
  light[2] : spot light
*/
_light_t	_light[3][3];

/* default light */
static _light_t	_para_light_def = {
    {0.7f, 0.7f, 0.7f, 0.0f},		/* color */
    {0.0f, 1.5f, 0.5f, 0.0f},		/* vector(direction) */
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
	{0.0f, 0.0f, 1.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 0.0f},
	400.0f, 0.12f
    }
};

sceVu0FVECTOR	ambient = {0.6f, 0.6f, 0.6f, 0.6f};

/* FOR CAMERA CTRL */
#define PI	3.14159265f
#define OFFX	2048
#define OFFY	2048
#define SCREEN_WIDTH	(640.0f)
#define SCREEN_HEIGHT	(224.0f)
#define CLIP_ALPHA	(300.0f)	/* extra clipping area width */
static sceVu0FVECTOR	clip_volume = {1000.0f, 1000.0f, 1.0f, 65535.0f};
CameraPos	camerapos;
static CameraPos camerapos_init = {
    { 0.0f, 0.0f, -30.0f, 0.0f },		// pos
    { 0.0f, 0.0f, 1.0f, 1.0f },			// zdir
   { 0.0f, 1.0f, 0.0f, 1.0f },			// ydir
    { 0.0f, 0.0f, 0.0f, 0.0f },			// rot
};
CAMERA_DAT cam;

void get_camx(float *);
void get_camy(float *);
void get_camz(float *);
inline void Vu1MicroSetWorldScreen(u_int *mic, sceVu0FMATRIX mat)
{  sceVu0CopyMatrix((float (*)[4])(&mic[0]), mat);}

inline void Vu1MicroSetWorldClip(u_int *mic, sceVu0FMATRIX mat)
{  sceVu0CopyMatrix((float (*)[4])(&mic[(sizeof(sceVu0FMATRIX)>>2)]), mat);}

inline void Vu1MicroSetClipScreen(u_int *mic, sceVu0FMATRIX mat)
{  sceVu0CopyMatrix((float (*)[4])(&mic[(sizeof(sceVu0FMATRIX)>>2)*2]), mat);}



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


void sceVu0RotCameraMatrix(sceVu0FMATRIX m, 
			   CameraPos *cp)
{
    sceVu0FMATRIX work;
    sceVu0FVECTOR direction, vertical, position;

    sceVu0UnitMatrix(work);
    sceVu0RotMatrixX(work,work,cp->rot[0]);
    sceVu0RotMatrixY(work,work,cp->rot[1]);
    sceVu0RotMatrixZ(work,work,cp->rot[2]);
    sceVu0ApplyMatrix(direction, work, cp->zdir);
    sceVu0ApplyMatrix(vertical, work, cp->ydir);
    sceVu0ApplyMatrix(position, work, cp->pos);
    sceVu0CameraMatrix(m, position, direction, vertical);
}


/* make view_screen & view_clip & clip_screen matrix */
void SetVu0ViewScreenClipMatrix(sceVu0FMATRIX m, sceVu0FMATRIX mc,
			     sceVu0FMATRIX mcs,
			     sceVu0FVECTOR clip_vol,
			     float scrz, float ax, float ay, 
			     float cx, float cy, 
			     float zmin, float zmax, 
			     float nearz, float farz)
{
	float	az, cz;
	float   gsx, gsy;
	sceVu0FMATRIX	mt;		

	gsx = nearz*clip_vol[0]/scrz;
	gsy = nearz*clip_vol[1]/scrz;

	cz = (-zmax * nearz + zmin * farz) / (-nearz + farz);
	az  = farz * nearz * (-zmin + zmax) / (-nearz + farz);

	//     | scrz    0  0 0 |
	// m = |    0 scrz  0 0 | 
	//     |    0    0  0 1 |
	//     |    0    0  1 0 |
	sceVu0UnitMatrix(m);
	m[0][0] = scrz;	m[1][1] = scrz;
	m[2][2] = 0.0f;	m[3][3] = 0.0f;
	m[3][2] = 1.0;	m[2][3] = 1.0;

	//     | ax  0  0 cx |
	// m = |  0 ay  0 cy | 
	//     |  0  0 az cz |
	//     |  0  0  0  1 |
	sceVu0UnitMatrix(mt);
	mt[0][0] = ax;	mt[1][1] = ay;	mt[2][2] = az;
	mt[3][0] = cx;	mt[3][1] = cy;	mt[3][2] = cz;
	sceVu0MulMatrix(m, mt, m);

	
	//     |2n/2gsx    0        0           0      |
	// m = |   0    2n/2gsy     0           0      |
	//     |   0       0   (f+n)/(f-n) -2f*n/(f-n) |
	//     |   0       0        1           0      |
	sceVu0UnitMatrix(mc);
	mc[0][0] = 2.0f * nearz / (gsx-(-gsx));
	mc[1][1] = 2.0f * nearz / (gsy-(-gsy));
	mc[2][2] = (farz + nearz) / (farz-nearz);
	mc[3][2] = -2*(farz * nearz) / (farz-nearz);
	mc[2][3] = 1.0f;	mc[3][3] = 0.0f;

	//     |scrz*ax*gsx/n    0            0               cx      |
	// m = |   0        scrz*ay*gsy/n     0               cy      |
	//     |   0             0      (-zmax+zmin)/2  (zmin+zmax)/2 |
	//     |   0             0            0               1       |
	sceVu0UnitMatrix(mcs);
	mcs[0][0] = scrz*ax*gsx/nearz;
	mcs[1][1] = scrz*ay*gsy/nearz;
	mcs[2][2] = (-zmax+zmin)/2.0f;
	mcs[3][2] = (zmax+zmin)/2.0f;
	mcs[3][0] = cx;	mcs[3][1] = cy;	
		mcs[3][3] = 1.0f;	

	return;
}



/* inline */ void Camera_SetCamera_fromCameraPos(CAMERA_DAT *cam, CameraPos *cam_pos)
{
    cam->cp= *cam_pos;
}


/* inline */ void Camera_SetClipScreen(CAMERA_DAT *cam, float screen_dist, 
			  float xaspect, float yaspect, 
			  float gs_xsize, float gs_ysize,
			  float zmini, float zmax, 
			  float nearz, float farz)
{
  SetVu0ViewScreenClipMatrix((cam)->view_screen, (cam)->view_clip,
			     (cam)->clip_screen, (cam)->clip_volume,
			     screen_dist, xaspect, yaspect,
			     gs_xsize, gs_ysize, zmini, zmax,
			     nearz, farz); 
}

void Camera_SetClipData(CAMERA_DAT *cam, float sw, float sh, float sn, float sf)
{
    cam->screen_w = sw;
    cam->screen_h = sh;
    cam->near = sn;
    cam->far = sf;
}
