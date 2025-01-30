/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                         - tex_swap -
 *
 *                        Version 0.4.0
 *
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                     Name : physics.c
 *                     Description : main procedures
 *
 *       Version   Date           Design              Log
 *  --------------------------------------------------------------------
 *       0.4.0     Jul,12,1999    munekis            first version
 */



#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <math.h>

#include "gamed.h"
#include "physics.h"

Object_t spheres[MAX_CLIENTS * N_BALLS_PER_CLIENT];

#define RAND_VALUE() ((float) rand() / ((float) RAND_MAX))


void init_sphere(int n_sphere) {

    int j, k;

    j = n_sphere / 6;
    k = n_sphere % 6;

    spheres[n_sphere].position[0] = 10.0f * RAND_VALUE() - 5.0f + j * 10.0f - 20.0f;
    spheres[n_sphere].position[1] = -10.0f * RAND_VALUE() - 40.0f;
    spheres[n_sphere].position[2] = 10.0f * RAND_VALUE() - 5.0f + k * 10.0f - 20.0f;

    spheres[n_sphere].velocity[0] = 2.0f * RAND_VALUE() - 1.0f;
    spheres[n_sphere].velocity[1] = 0.0f;
    spheres[n_sphere].velocity[2] = 2.0f * RAND_VALUE() - 1.0f;

    spheres[n_sphere].accl[0] = 0.0f;
    spheres[n_sphere].accl[1] = 0.0f;
    spheres[n_sphere].accl[2] = 0.0f;

    spheres[n_sphere].rotation[0] = 0.0f;
    spheres[n_sphere].rotation[1] = 0.0f;
    spheres[n_sphere].rotation[2] = 0.0f;
   

#if 0
     printf("======= initial value (sphere = %d) ======\n", n_sphere);
     printf("pos = (%f, %f, %f)\n", spheres[n_sphere].position[0],
                                   spheres[n_sphere].position[1],
                                   spheres[n_sphere].position[2]);

     printf("velocity = (%f, %f, %f)\n", 
                                   spheres[n_sphere].velocity[0],
                                   spheres[n_sphere].velocity[1],
                                   spheres[n_sphere].velocity[2]);
#endif

     return;
}


// -----------------------------------------------------------------------
//
//
//
//

void calc_position(int n_all) {

        int i, j;
        float x, y, z, distance;
        
        for(i = 0; i < n_all; i++) {

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

        for(i = 0; i < n_all - 1; i++) {
	    for(j = i+1; j < n_all; j++) {
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











