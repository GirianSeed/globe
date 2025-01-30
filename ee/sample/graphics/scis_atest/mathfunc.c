/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: mathfunc.c,v 1.1 2002/10/18 12:26:13 aoki Exp $
 */
/* 
 * Emotion Engine Library Sample Program
 *
 * Copyright (C) 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * scis_at - mathfunc.c
 *	camera math functions
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2002-10-08      aoki        
 */

#include <libvu0.h>
#include"mathfunc.h"

extern float My_matrix[];
extern float My_rot_trans_matrix[];
extern float My_light_matrix[];
extern float My_lcolor_matrix[];

sceVu0FVECTOR camera_p = { 0, 0, -25, 0 };
sceVu0FVECTOR camera_zd = { 0, 0, 1, 1 };
sceVu0FVECTOR camera_yd = { 0, 1, 0, 1 };
sceVu0FVECTOR camera_rot = { 0.0f, 0.0f, 0, 0 };

sceVu0FVECTOR light0 = { -0.5, 1.0, 2.0, 0 };
sceVu0FVECTOR light1 = { 0.5, -1.5, -1.0, 0 };
sceVu0FVECTOR light2 = { -1.5, -0.5, 0.5, 0 };

sceVu0FVECTOR color0  = { 0.8, 0.8, 0.4, 0 };
sceVu0FVECTOR color1  = { 0.4, 0.2, 0.4, 0 };
sceVu0FVECTOR color2  = { 0.0, 0.0, 0.0, 0 };

sceVu0FVECTOR ambient = { 0.2, 0.2, 0.2, 0 };

sceVu0FVECTOR obj_trans = { 0, 0, 0, 0 };
sceVu0FVECTOR obj_rot = { 0.7f, 0, 0, 0 };

sceVu0FMATRIX local_world;
sceVu0FMATRIX world_view;
sceVu0FMATRIX view_screen;
sceVu0FMATRIX local_screen;

sceVu0FMATRIX normal_light;
sceVu0FMATRIX light_color;
sceVu0FMATRIX local_light;
sceVu0FMATRIX local_color;


static void CopyMatrix(float *dst, sceVu0FMATRIX src);


void sceVu0RotCameraMatrix(sceVu0FMATRIX m, sceVu0FVECTOR p, sceVu0FVECTOR zd,
 sceVu0FVECTOR yd, sceVu0FVECTOR rot)
{
        sceVu0FMATRIX work;
        sceVu0FVECTOR direction, vertical, position;

        sceVu0UnitMatrix(work);
        sceVu0RotMatrixX(work,work,rot[0]);
        sceVu0RotMatrixY(work,work,rot[1]);
        sceVu0RotMatrixZ(work,work,rot[2]);
        sceVu0ApplyMatrix(direction, work, zd);
        sceVu0ApplyMatrix(vertical, work, yd);
        sceVu0ApplyMatrix(position, work, p);
        sceVu0CameraMatrix(m, position, direction, vertical);
}



static void CopyMatrix(float *dst, sceVu0FMATRIX src)
{
    int i,j;

    for (i=0; i<4; i++){
	for (j=0; j<4; j++){
	    dst[i*4+j]=src[i][j];
	}
    }
}

void cal_matrix(void)
{
    sceVu0FMATRIX work;

    /* local -> world (rotate)matrix */
    sceVu0UnitMatrix(work);
    sceVu0RotMatrix(local_world, work, obj_rot);

    /* color&normal matrix setting */
    sceVu0NormalLightMatrix(normal_light, light0, light1, light2);
    sceVu0LightColorMatrix(light_color, color0, color1, color2, ambient);
    CopyMatrix(My_lcolor_matrix, light_color);

    /* light(normal) -> local_light matrix */
    sceVu0MulMatrix(local_light, normal_light, local_world);
    CopyMatrix(My_light_matrix, local_light);

		/* local -> world (rotate&translate)matrix */
    sceVu0TransMatrix(local_world, local_world, obj_trans);
    CopyMatrix(My_rot_trans_matrix, local_world);

		/* world -> view matrix */
    sceVu0RotCameraMatrix(world_view,
			  camera_p, camera_zd, camera_yd, camera_rot);

    /* view -> screen matrix */
    sceVu0ViewScreenMatrix(view_screen, 512.0f, 1.0f, 0.47f,
			   2048.0f, 2048.0f, 1.0f, 16777215.0f,
			   1.0f,65536.0f);

    /* local -> screen matrix */
    sceVu0MulMatrix(local_screen, view_screen, world_view);
    CopyMatrix(My_matrix, local_screen);
}



