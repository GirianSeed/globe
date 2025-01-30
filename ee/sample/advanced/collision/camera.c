/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/*
 * 
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          Name : camera.c
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            1.1     Sept 20, 2000    kaneko      Initial
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

#include "camera.h"

inline void Vu1MicroSetWorldScreen(u_int *mic, sceVu0FMATRIX mat)
{  sceVu0CopyMatrix((float (*)[4])(&mic[0]), mat);}

inline void Vu1MicroSetWorldClip(u_int *mic, sceVu0FMATRIX mat)
{  sceVu0CopyMatrix((float (*)[4])(&mic[(sizeof(sceVu0FMATRIX)>>2)]), mat);}

inline void Vu1MicroSetClipScreen(u_int *mic, sceVu0FMATRIX mat)
{  sceVu0CopyMatrix((float (*)[4])(&mic[(sizeof(sceVu0FMATRIX)>>2)*2]), mat);}

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
