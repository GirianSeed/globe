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
 *                     Name : phys.c
 *                     Description : ball game physical calc.
 *
 *
 *       Version        Date           Design      Log
 *  --------------------------------------------------------------------
 *       1.0.0          Sep,29,2000    munekis     first verison
 */


#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <libdev.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libdma.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libpad.h>
#include <libvu0.h>

#include <stdlib.h>
#include <math.h>
#include "main.h"


extern float My_rot_trans_matrix[] __attribute__((section(".vudata")));
extern float My_light_matrix[] __attribute__((section(".vudata")));
extern float My_light_color_matrix[] __attribute__((section(".vudata")));
extern float matrix_sphere0[] __attribute__((section(".vudata")));
extern float matrix_sphere1[] __attribute__((section(".vudata")));

extern u_int My_buffer4[] __attribute__((section(".vudata")));


#define RAND_VALUE() ((float) rand() / ((float) RAND_MAX))


void init_sphere(Object_t *spheres) {

        int i, j, k;
   
        for(i = 0; i < N_SPHERES; i++) {

            j = i / 6;
            k = i % 6;
            spheres[i].position[0] = 10.0f * RAND_VALUE() - 5.0f + j * 10.0f - 20.0f;
            spheres[i].position[1] = -10.0f * RAND_VALUE() - 40.0f;
            spheres[i].position[2] = 10.0f * RAND_VALUE() - 5.0f + k * 10.0f - 20.0f;
            spheres[i].position[3] = 1.0f;

            spheres[i].velocity[0] = 2.0f * RAND_VALUE() - 1.0f;
            spheres[i].velocity[1] = 0.0f;
            spheres[i].velocity[2] = 2.0f * RAND_VALUE() - 1.0f;
            spheres[i].velocity[3] = 0.0f;

            spheres[i].accl[0] = 0.0f;
            spheres[i].accl[1] = 0.0f;
            spheres[i].accl[2] = 0.0f;
            spheres[i].accl[3] = 0.0f;

            spheres[i].rotation[0] = 0.0f;
            spheres[i].rotation[1] = 0.0f;
            spheres[i].rotation[2] = 0.0f;
            spheres[i].rotation[3] = 0.0f;
                
        }
        return;
}


// -----------------------------------------------------------------------
//
//
//
//

void calc_position(Object_t *spheres) {

        int i, j;
        float x, y, z, distance;
        
        for(i = 0; i < N_SPHERES; i++) {

            spheres[i].accl[0] = 0.0f;
            spheres[i].accl[1] = GRAVITY;
            spheres[i].accl[2] = 0.0f;

            if(spheres[i].position[1] == GROUND_LEVEL) {
                spheres[i].accl[0] = -FRICTION_COEFF * spheres[i].velocity[0];
                spheres[i].accl[2] = -FRICTION_COEFF * spheres[i].velocity[2];
            }

            spheres[i].velocity[0] += spheres[i].accl[0];
            spheres[i].velocity[1] += spheres[i].accl[1];
            spheres[i].velocity[2] += spheres[i].accl[2];
                
            spheres[i].position[0] += spheres[i].velocity[0];
            spheres[i].position[1] += spheres[i].velocity[1];
            spheres[i].position[2] += spheres[i].velocity[2];
        

            if(spheres[i].position[1] >= GROUND_LEVEL) {
                spheres[i].position[1] = GROUND_LEVEL;
                spheres[i].velocity[1] *= - REBOUND_COEFF;
            }

            // ---- x coordinates ----
            if(spheres[i].position[0] > WALL) {
                spheres[i].position[0] = WALL;
                spheres[i].velocity[0] *= - REBOUND_COEFF;
            }
            if(spheres[i].position[0] < -WALL) {
                spheres[i].position[0] = -WALL;
                spheres[i].velocity[0] *= - REBOUND_COEFF;
            }

            if(spheres[i].position[2] > WALL) {
                spheres[i].position[2] = WALL;
                spheres[i].velocity[2] *= - REBOUND_COEFF;
            }
            if(spheres[i].position[2] < -WALL) {
                spheres[i].position[2] = -WALL;
                spheres[i].velocity[2] *= - REBOUND_COEFF;
            }

            spheres[i].rotation[2] -= (RAD_TO_DEG(spheres[i].velocity[0]) * 0.005f);
            spheres[i].rotation[0] -= (RAD_TO_DEG(spheres[i].velocity[2]) * 0.005f); 

        }

#define RADIUS 5.0f

        for(i = 0; i < N_SPHERES - 1; i++) {
            for(j = i+1; j < N_SPHERES; j++) {
                x = spheres[i].position[0] - spheres[j].position[0];                   
                y = spheres[i].position[1] - spheres[j].position[1];                   
                z = spheres[i].position[2] - spheres[j].position[2];
                distance = x*x + y*y + z*z;

                if(distance < 2.0f * 2.0f * RADIUS * RADIUS) {
                    distance = sqrtf(distance);
                    spheres[i].position[0] 
                        = spheres[j].position[0] + 10.0f * x / distance; 
                    spheres[i].position[1] 
                        = spheres[j].position[1] + 10.0f * y / distance; 
                    spheres[i].position[2] 
                        = spheres[j].position[2] + 10.0f * z / distance; 

                    x = spheres[i].velocity[0];
                    y = spheres[i].velocity[1];
                    z = spheres[i].velocity[2];

                    spheres[i].velocity[0] = spheres[j].velocity[0];
                    spheres[i].velocity[1] = spheres[j].velocity[1];
                    spheres[i].velocity[2] = spheres[j].velocity[2];
                    spheres[j].velocity[0] = x;
                    spheres[j].velocity[1] = y;
                    spheres[j].velocity[2] = z;
                }                
            }
        }

        return;
}


extern sceSamp0FVECTOR light0;
extern sceSamp0FVECTOR light1;
extern sceSamp0FVECTOR light2;

void set_sphere_matrix(Object_t *spheres) {


    sceSamp0FVECTOR rot = { 0.0f, 0.0f, 0.0f, 0.0f };
    sceSamp0FVECTOR trans = { 0.0f, 2.0f, 0.0f, 1.0f };
    sceSamp0FMATRIX local_world, unit, normal_light, local_light;

    sceSamp0UnitMatrix(unit);
 

    sceSamp0RotMatrix(local_world, unit, rot);
    sceSamp0NormalLightMatrix(normal_light, light0, light1, light2);
    sceSamp0MulMatrix(local_light, normal_light, local_world);

    // local -> world (rotate&translate)matrix
    sceSamp0TransMatrix(local_world, local_world, trans);

    // My_light_matrix <- local_light
    My_light_matrix[0] = local_light[0][0];
    My_light_matrix[1] = local_light[1][0];
    My_light_matrix[2] = local_light[2][0];
    My_light_matrix[3] = local_light[3][0];

    My_light_matrix[4] = local_light[0][1];
    My_light_matrix[5] = local_light[1][1];
    My_light_matrix[6] = local_light[2][1];
    My_light_matrix[7] = local_light[3][1];

    My_light_matrix[8] = local_light[0][2];
    My_light_matrix[9] = local_light[1][2];
    My_light_matrix[10] = local_light[2][2];
    My_light_matrix[11] = local_light[3][2];

    My_light_matrix[12] = local_light[0][3];
    My_light_matrix[13] = local_light[1][3];
    My_light_matrix[14] = local_light[2][3];
    My_light_matrix[15] = local_light[3][3];

        
    return;
}

void set_sphere_matrix2(int i, Object_t *spheres) {

    int offset;
    float *sphere_matrix, *shadow_matrix;
    sceSamp0FMATRIX local_world, unit, normal_light, local_light;

    sceSamp0UnitMatrix(unit);

    // My_rot_trans_matrix <- world_view
    sphere_matrix = (float*) matrix_sphere0;
    offset = (float*) matrix_sphere1 - (float*) matrix_sphere0;

    shadow_matrix = (float*) matrix_sphere1;

    sceSamp0RotMatrix(local_world, unit, spheres[i].rotation);
    sceSamp0NormalLightMatrix(normal_light, light0, light1, light2);
    sceSamp0MulMatrix(local_light, normal_light, local_world);

    // local -> world (rotate&translate)matrix
    sceSamp0TransMatrix(local_world, local_world, spheres[i].position);


    // sphere_matrix <- world_view
    sphere_matrix[0] = local_world[0][0];
    sphere_matrix[1] = local_world[1][0];
    sphere_matrix[2] = local_world[2][0];
    sphere_matrix[3] = local_world[3][0];

    sphere_matrix[4] = local_world[0][1];
    sphere_matrix[5] = local_world[1][1];
    sphere_matrix[6] = local_world[2][1];
    sphere_matrix[7] = local_world[3][1];

    sphere_matrix[8] = local_world[0][2];
    sphere_matrix[9] = local_world[1][2];
    sphere_matrix[10] = local_world[2][2];
    sphere_matrix[11] = local_world[3][2];

    sphere_matrix[12] = local_world[0][3];
    sphere_matrix[13] = local_world[1][3];
    sphere_matrix[14] = local_world[2][3];
    sphere_matrix[15] = local_world[3][3];

    sphere_matrix += 16;

    sphere_matrix[0] = local_light[0][0];
    sphere_matrix[1] = local_light[1][0];
    sphere_matrix[2] = local_light[2][0];
    sphere_matrix[3] = local_light[3][0];

    sphere_matrix[4] = local_light[0][1];
    sphere_matrix[5] = local_light[1][1];
    sphere_matrix[6] = local_light[2][1];
    sphere_matrix[7] = local_light[3][1];

    sphere_matrix[8] = local_light[0][2];
    sphere_matrix[9] = local_light[1][2];
    sphere_matrix[10] = local_light[2][2];
    sphere_matrix[11] = local_light[3][2];

    sphere_matrix[12] = local_light[0][3];
    sphere_matrix[13] = local_light[1][3];
    sphere_matrix[14] = local_light[2][3];
    sphere_matrix[15] = local_light[3][3];

    return;
}






void set_shadow_matrix(int i, Object_t *spheres) {

        float *shadow_matrix, f1;
        unsigned int ui1;

        sceSamp0FVECTOR trans = { 0.0f, 2.0f, 0.0f, 0.0f };
        sceSamp0FMATRIX local_world, unit, normal_light, local_light;

        sceSamp0UnitMatrix(unit);

        // ------ SHADOW MATRIX ------
        shadow_matrix = (float*) matrix_sphere1;

        sceSamp0UnitMatrix(local_world);
        sceSamp0NormalLightMatrix(normal_light, light0, light1, light2);
        sceSamp0MulMatrix(local_light, normal_light, local_world);


        // local -> world (rotate&translate)matrix
        sceVu0CopyVector(trans, spheres[i].position);
        trans[1] = 3.8f;
        sceSamp0TransMatrix(local_world, local_world, trans);

        // scaling.
        f1 = spheres[i].position[1];
        f1 = (50.0f - f1) / 50.0f;        
        local_world[0][0] = local_world[2][2] = f1 * f1;        
        ui1 = (unsigned int) (1.0f / (f1 * f1 * f1) * 80.0f);
        My_buffer4[13] = ui1;

        // shadow_matrix <- world_view
        shadow_matrix[0] = local_world[0][0];
        shadow_matrix[1] = local_world[1][0];
        shadow_matrix[2] = local_world[2][0];
        shadow_matrix[3] = local_world[3][0];

        shadow_matrix[4] = local_world[0][1];
        shadow_matrix[5] = local_world[1][1];
        shadow_matrix[6] = local_world[2][1];
        shadow_matrix[7] = local_world[3][1];

        shadow_matrix[8] = local_world[0][2];
        shadow_matrix[9] = local_world[1][2];
        shadow_matrix[10] = local_world[2][2];
        shadow_matrix[11] = local_world[3][2];

        shadow_matrix[12] = local_world[0][3];
        shadow_matrix[13] = local_world[1][3];
        shadow_matrix[14] = local_world[2][3];
        shadow_matrix[15] = local_world[3][3];

        shadow_matrix += 16;

        shadow_matrix[0] = local_light[0][0];
        shadow_matrix[1] = local_light[1][0];
        shadow_matrix[2] = local_light[2][0];
        shadow_matrix[3] = local_light[3][0];

        shadow_matrix[4] = local_light[0][1];
        shadow_matrix[5] = local_light[1][1];
        shadow_matrix[6] = local_light[2][1];
        shadow_matrix[7] = local_light[3][1];

        shadow_matrix[8] = local_light[0][2];
        shadow_matrix[9] = local_light[1][2];
        shadow_matrix[10] = local_light[2][2];
        shadow_matrix[11] = local_light[3][2];

        shadow_matrix[12] = local_light[0][3];
        shadow_matrix[13] = local_light[1][3];
        shadow_matrix[14] = local_light[2][3];
        shadow_matrix[15] = local_light[3][3];

        return;
}












