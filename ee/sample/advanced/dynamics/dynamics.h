/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
*/
/*
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                                                      All Right Reserved
 *
 */

#include "collision.h"

#define PASSIVE 0
#define ACTIVE 1

typedef struct _body{

  int type;
  /* constants */
  float m; /* mass */
  float invM; /* inverse mass */
  sceVu0FMATRIX Iinv; /* inverse inertia tensor */


  /* state variables */
  sceVu0FVECTOR x; /* center of mass position */
  sceVu0FVECTOR rq; /* rotation quaternion */
  sceVu0FVECTOR p; /* linear momentum */
  sceVu0FVECTOR L; /* angular momentum */

  /* force and torque accumulator */
  sceVu0FVECTOR F;
  sceVu0FVECTOR torque;

  /* internal temp var. */
  sceVu0FMATRIX Iinv_t;
  sceVu0FVECTOR omega;

  /* gjkobj */
  GJKObj *gjkobj;

} rigid_body;


typedef struct {
  rigid_body *bodyA;
  rigid_body *bodyB;
  Simplex simplex;
  sceVu0FVECTOR normal;
  sceVu0FVECTOR ra;
  sceVu0FVECTOR rb;
  float dist;
  float vrel;
} Contact;  

typedef struct {
  sceVu0FVECTOR *Xstart,*Xtemp,*Xend,*d1,*d2,*d3,*d4,*d5;
  Contact *cache_contact_list;
} SimMEM;


extern void simulation(rigid_body **bodylist,int nbody,float tstep,int nsteps,float tstart,SimMEM *mem,void (*calc_force)(sceVu0FVECTOR *,rigid_body **,int ,float ));
extern void simulation_with_rollback(rigid_body **bodylist,int nbody,float tstep,int nsteps,float tstart,SimMEM *mem,void (*calc_force)(sceVu0FVECTOR *,rigid_body **,int ,float ));  
