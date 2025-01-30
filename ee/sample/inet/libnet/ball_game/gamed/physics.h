/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                         - ball_game -
 *
 *                        Version 2.0.0
 *
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                     Name : physics.h
 *                     Description : param/prototypes for this sample.
 *
 *       Version   Date           Design              Log
 *  --------------------------------------------------------------------
 *       0.4.0     Jul,12,1999    munekis            first version
 */


#ifndef __PHYSICS_H__
#define __PHYSICS_H__


#include "gamed.h"

#define GROUND_LEVEL -1.0f
#define WALL 50.0f
#define REBOUND_COEFF 0.70f
#define FRICTION_COEFF 0.02f
#define N_SPHERES 100


// --- init values ---
#define INIT_POS_X  0.0f
#define INIT_POS_Y  -40.0f
#define INIT_POS_Z  0.0f

#define GRAVITY 9.8f / 60.0f

typedef struct {
    float position[3];
    float velocity[3];
    float accl[3];
    float rotation[3];
} Object_t;



void init_sphere(int);
void calc_position(int);

extern Object_t spheres[MAX_CLIENTS * N_BALLS_PER_CLIENT];

// --- defines ---
#define PI 3.14159265358979323846f
#define RAD_TO_DEG(x) (x * 180.0f / PI)
#define DEG_TO_RAD(x) (x * PI / 180.0f)


#endif // __PHYSICS_H__

















