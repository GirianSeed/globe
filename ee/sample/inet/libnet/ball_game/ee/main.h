/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                         - ball_game -
 *
 *                         Version 1.0.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                     Name : main.h
 *                     Description : 
 *
 *
 *       Version        Date           Design      Log
 *  --------------------------------------------------------------------
 *       1.0.0          Sep,29,2000    munekis     first verison
 */


#ifndef __MAIN_H__
#define __MAIN__H__


#include "mathfunc.h"

#define GROUND_LEVEL -1.0f
#define WALL 50.0f
#define REBOUND_COEFF 0.70f
#define FRICTION_COEFF 0.02f
#define N_SPHERES 50


// --- init values ---
#define INIT_POS_X  0.0f
#define INIT_POS_Y  -40.0f
#define INIT_POS_Z  0.0f

#define GRAVITY 9.8f / 60.0f

typedef struct {
    sceSamp0FVECTOR position;
    sceSamp0FVECTOR velocity;
    sceSamp0FVECTOR accl;
    sceSamp0FVECTOR rotation;
} Object_t;



void init_sphere(Object_t *spheres);
void calc_position(Object_t *spheres);
void set_sphere_matrix(Object_t *spheres);
void set_sphere_matrix2(int, Object_t *spheres);
void set_shadow_matrix(int, Object_t *spheres);


#endif // 

















