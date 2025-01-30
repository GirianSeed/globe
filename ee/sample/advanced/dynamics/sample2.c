/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/*
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          Name : sample_cd.c
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *
 */
#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <stdarg.h>
#include <eeregs.h>
#include <libvu0.h>
#include <libdma.h>
#include <libhig.h>
#include <libhip.h>
#include "util.h"
#include "camera.h"
#include "light.h"
#include "dynamics.h"

#define	NUM	5				/* num of obj*/
enum { OBJA,OBJB,OBJC,OBJD,OBJE};


static void alloc_mem(SimMEM *mem,int nbody);
static void free_mem(SimMEM mem);
static void init_contact_cache();

static rigid_body body0,body1,body2,body3,body4;
static rigid_body *bodylist[NUM];
static SimMEM mem;
static int add_force;
static int bodyid=2;

static void calc_force(sceVu0FVECTOR *Xn,rigid_body **bodylist,int nbody,float t)
{
  int i;
  rigid_body *body;

  for (i=0;i<nbody;i++){
    body=*(bodylist+i);

    body->F[0]=0.0f;
    body->F[1]=body->m*40.0f; /*gravity */      
    body->F[2]=0.0f;

    
    body->torque[0]=0.0f;
    body->torque[1]=0.0f;
    body->torque[2]=0.0f;
  }
  if (add_force) {
    body=*(bodylist+bodyid);
#define FORCE 50.0f
#define TRQ 10.0f
    if (gPad.RrightOn) body->F[0]=FORCE*body->m;
    if (gPad.RleftOn) body->F[0]=-FORCE*body->m;
    if (gPad.RupOn) body->F[2]=FORCE*body->m;
    if (gPad.RdownOn) body->F[2]=-FORCE*body->m;
    if (gPad.R2On) body->F[1]+=FORCE*body->m;
    if (gPad.R1On) body->F[1]+=-FORCE*body->m;

    if (gPad.LrightOn) body->torque[0]=TRQ;
    if (gPad.LleftOn) body->torque[0]=-TRQ;
    if (gPad.LupOn) body->torque[2]=TRQ;
    if (gPad.LdownOn) body->torque[2]=-TRQ;
    if (gPad.L2On) body->torque[1]=TRQ;
    if (gPad.L1On) body->torque[1]=-TRQ;

  }
}

/****************************************/
/*	ClutBump Mapping Sample		*/
/****************************************/
void SAMPLED2_init();
int SAMPLED2_main();
void SAMPLED2_exit();

static void		*data_addr[NUM];
static void		*gjk_data_addr[NUM];
static sceHiPlug	*frameP[NUM];
static sceHiPlug	*microP[NUM];
static sceHiPlugMicroData_t	*microD[NUM];
static sceHiPlug	*shapeP[NUM];
static u_int		*shapeD[NUM];


/* pointer to vertex-arrays */
static sceVu0FMATRIX *obj_local_world[NUM];
static sceVu0FMATRIX *obj_local_world_rot[NUM];


static GJKObj my_gjkobj[NUM];


static void init_body_ball(rigid_body *body)
{


  body->type=ACTIVE;
  body->m=1.0f;
  body->invM=1.0f/body->m;
  sceVu0UnitMatrix(body->Iinv);
  body->Iinv[0][0]=body->Iinv[1][1]=body->Iinv[2][2]=(1.0f/0.8f)*body->invM;

  body->x[0]=7.0f;
  body->x[1]=-10.0f;
  body->x[2]=0.0f;

  /* start with a arb. rotated orientation  */
  /* quaternion will be normalized inside simulation() */
  body->rq[0]=0.1f;
  body->rq[1]=0.2f;
  body->rq[2]=0.3f;  
  body->rq[3]=1.0f;


  body->p[0]=0.0f;
  body->p[1]=0.0f;
  body->p[2]=0.0f;

  body->L[0]=0.0f;
  body->L[1]=0.0f;
  body->L[2]=0.0f;  
  
}
static void init_body_ell(rigid_body *body)
{

  body->type=ACTIVE;
  body->m=2.0f;
  body->invM=1.0f/body->m;
  sceVu0UnitMatrix(body->Iinv);
  body->Iinv[0][0]=body->invM*(1.0f/0.501927f);
  body->Iinv[1][1]=body->invM*(1.0f/4.558571f);
  body->Iinv[2][2]=body->invM*(1.0f/4.653725f);

  body->x[0]=-6.0f;
  body->x[1]=0.0f;
  body->x[2]=5.0f;

  body->rq[0]=0.0f;
  body->rq[1]=0.0f;
  body->rq[2]=0.0f;  
  body->rq[3]=1.0f;

  body->p[0]=0.0f;
  body->p[1]=0.0f;
  body->p[2]=0.0f;

  body->L[0]=0.0f;
  body->L[1]=0.0f;
  body->L[2]=0.0f;  
  
}


static void init_body_plane1(rigid_body *body)
{
  float x0,y0,z0;

  x0=20.0f;y0=0.0f;z0=20.0f;
  body->type=PASSIVE;
  body->m=4;
  body->invM=1.0f/body->m;
  sceVu0UnitMatrix(body->Iinv);
  body->Iinv[0][0]=1/((body->invM*12.0f)*(y0*y0+z0*z0));
  body->Iinv[1][1]=1/((body->invM*12.0f)*(x0*x0+z0*z0));
  body->Iinv[2][2]=1/((body->invM*12.0f)*(x0*x0+y0*y0));

  body->x[0]=-0.0f;
  body->x[1]=5.201f;
  body->x[2]=3.0f;

  body->rq[0]=0.0f;
  body->rq[1]=0.0f;
  body->rq[2]=0.087155742754;
  body->rq[3]=0.99619469809;

  body->p[0]=0.0f;
  body->p[1]=0.0f;
  body->p[2]=0.0f;

  body->L[0]=0.0f;
  body->L[1]=0.0f;
  body->L[2]=0.0f;  
  
}

static void init_body_plane2(rigid_body *body)
{
  float x0,y0,z0;

  x0=0.0f;y0=10.0f;z0=20.0f;
  body->type=PASSIVE;
  body->m=3.0f;
  body->invM=1.0f/body->m;
  sceVu0UnitMatrix(body->Iinv);
  body->Iinv[0][0]=1/((body->invM*12.0f)*(y0*y0+z0*z0));
  body->Iinv[1][1]=1/((body->invM*12.0f)*(x0*x0+z0*z0));
  body->Iinv[2][2]=1/((body->invM*12.0f)*(x0*x0+y0*y0));

  body->x[0]=10.0f;
  body->x[1]=0.201f;
  body->x[2]=3.0f;

  body->rq[0]=0.0f;
  body->rq[1]=0.0f;
  body->rq[2]=0.0f;  
  body->rq[3]=1.0f;

  body->p[0]=0.0f;
  body->p[1]=0.0f;
  body->p[2]=0.0f;

  body->L[0]=0.0f;
  body->L[1]=0.0f;
  body->L[2]=0.0f;  
  
}


static void init_body_block(rigid_body *body)
{


  body->type=ACTIVE;
  body->m=3.0f;
  body->invM=1.0f/body->m;
  body->Iinv[0][0]=body->invM*0.377f;
  body->Iinv[1][1]=body->invM*0.327f;
  body->Iinv[2][2]=body->invM*0.310f;
  body->Iinv[0][1]=body->Iinv[1][0]=-body->invM*0.085f;
  body->Iinv[0][2]=body->Iinv[2][0]=-body->invM*0.114f;
  body->Iinv[1][2]=body->Iinv[2][1]=body->invM*0.085f;


  body->x[0]=0.0f;
  body->x[1]=1.5f;
  body->x[2]=0.0f;

  body->rq[0]=0.0f;
  body->rq[1]=0.0f;
  body->rq[2]=0.0f;  
  body->rq[3]=1.0f;

  body->p[0]=0.0f;
  body->p[1]=0.0f;
  body->p[2]=0.0f;

  body->L[0]=0.0f;
  body->L[1]=0.0f;
  body->L[2]=0.0f;   
}


static void gjk_init() {
  int i;
  for(i=0;i<NUM;i++){
    my_gjkobj[i].type=GJK_MESH;
    my_gjkobj[i].lw=obj_local_world[i];
    my_gjkobj[i].lwrot=obj_local_world_rot[i];
  }
}


static char	filename[NUM][40] = {"data/cube.bin","data/plane3.bin","data/ball.bin","data/ellipsoid.bin","data/plane2.bin"};

void SAMPLED2_init(void)
{
    sceHiErr	err;
    sceHiType	type;
    
    char	framename[NUM][12] = {"Frame","Frame","Frame","Frame","Frame"};
    char       gjk_filename[NUM][40] = {"data/ccube.bin","data/cplane3.bin","data/cball.bin","data/cellipsoid.bin","data/cplane2.bin"};
    int	i;

    for (i=0;i<NUM;i++) {
      gjk_data_addr[i] = file_read(gjk_filename[i]);
      if (!alloc_gjkobj_neighbors(gjk_data_addr[i],&(my_gjkobj[i]))) {
	error_handling(__FILE__,__LINE__,"Can-t allocate neighbor-list!");
      }
    }
    /*	Get Micro Plug & Data	*/
    for(i=0;i<NUM;i++){
      data_addr[i] = file_read(filename[i]);
      err = sceHiParseHeader((u_int *)data_addr[i]);
      if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
      err = sceHiGetPlug((u_int *)data_addr[i], framename[i], &frameP[i]);
      if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
      
      type.repository = SCE_HIP_COMMON;
      type.project = SCE_HIP_FRAMEWORK;
      type.category = SCE_HIP_MICRO;
      type.status = SCE_HIG_PLUGIN_STATUS;
      type.id = SCE_HIP_MICRO_PLUG;
      type.revision = SCE_HIP_REVISION;
      err = sceHiGetInsPlug(frameP[i], &microP[i], type);
      if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
     

      if((microD[i] = sceHiPlugMicroGetData(microP[i])) == NULL)
        error_handling(__FILE__,__LINE__,sceHiErrState.mes);

      camerainit(microD[i]);
      lightinit(microD[i]);   
      
      type.repository = SCE_HIP_COMMON;
      type.project = SCE_HIP_FRAMEWORK;
      type.category = SCE_HIP_SHAPE;
      type.status = SCE_HIG_PLUGIN_STATUS;
      type.id = SCE_HIP_SHAPE_PLUG;
      type.revision = SCE_HIP_REVISION;
      err = sceHiGetInsPlug(frameP[i], &shapeP[i], type);
      if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
      
      type.repository = SCE_HIP_COMMON;
      type.project = SCE_HIP_FRAMEWORK;
      type.category = SCE_HIP_SHAPE;
      type.status = SCE_HIG_DATA_STATUS;
      type.id = SCE_HIP_BASEMATRIX;
      type.revision = SCE_HIP_REVISION;
      err = sceHiGetData(shapeP[i], &shapeD[i], type);
      if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
      obj_local_world[i]= (sceVu0FMATRIX *)(shapeD[i]+8);
      obj_local_world_rot[i] =  (sceVu0FMATRIX *)(shapeD[i]+8+4*4);


    }
  

    /*	INIT PROCESS	*/
    for(i=0;i<NUM;i++){
	err = sceHiCallPlug(frameP[i], SCE_HIG_INIT_PROCESS);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    }


    collision_init();
    gjk_init();
    alloc_mem(&mem,NUM);


    /* init. rigid bodies */
    init_body_block(&body0);
    init_body_plane1(&body1);
    init_body_ball(&body2);
    init_body_ell(&body3);
    init_body_plane2(&body4);
    
    /* assign collsion(gjk) objects to rigid bodies*/ 
    body0.gjkobj=&(my_gjkobj[OBJA]);
    body1.gjkobj=&(my_gjkobj[OBJB]);
    body2.gjkobj=&(my_gjkobj[OBJC]);
    body3.gjkobj=&(my_gjkobj[OBJD]);
    body4.gjkobj=&(my_gjkobj[OBJE]);

    /* create a list of rigid bodies */
    bodylist[0]=&body0;
    bodylist[1]=&body1;
    bodylist[2]=&body2;
    bodylist[3]=&body3;
    bodylist[4]=&body4;
   
    init_contact_cache(); 
}



int SAMPLED2_main(void)
{
    sceHiErr	err;
    int		i;
    static int mode = 2;
    static float  t=0;


    if (gPad.SelectSwitch)
	mode = (++mode) > 2 ? 0 : mode;
    
    switch (mode) {
    case 0:
      add_force=0;
      draw_debug_print(1, 4, "MODE : CAMERA MOVE");
      cameractrl();
      for(i=0;i<NUM;i++){   
	cameraview(microD[i]);
      } 
      /* camera mode info */
      draw_debug_print(50, 1, "Rup/Rdown    : Cam Up/Down");
      draw_debug_print(50, 2, "Rleft/Rright : Cam Left/Right");
      draw_debug_print(50, 3, "R1/R2        : Cam Back/Front");
      draw_debug_print(50, 4, "Lup/Ldown    : Cam Rot X");
      draw_debug_print(50, 5, "Lleft/Lright : Cam Rot Y");
      draw_debug_print(50, 6, "L1/L2        : Cam Rot Z");
      draw_debug_print(50, 7, "SELECT       : MODE CHANGE");	    
      draw_debug_print(50, 8, "START        : EXIT");
      break;
    case 1:
      draw_debug_print(1, 4, "MODE : ADD FORCE");
      draw_debug_print(50, 1, "Rup/Rdown    : move z");
      draw_debug_print(50, 2, "Rleft/Rright : move x");
      draw_debug_print(50, 3, "R1/R2        : move y");
      draw_debug_print(50, 4, "Lup/Ldown    : rot z");
      draw_debug_print(50, 5, "Lleft/Lright : rot x");
      draw_debug_print(50, 6, "L1/L2        : rot y");
      draw_debug_print(50, 7, "SELECT       : MODE CHANGE");	    
      draw_debug_print(50, 8, "START        : EXIT");

      if ((*(bodylist+bodyid))->type == PASSIVE)
	draw_debug_print(1, 8, "obj: %s  passive",filename[bodyid]);
      else
	draw_debug_print(1, 8, "obj: %s  active",filename[bodyid]);

      add_force=1;
      break;                                         
    case 2:
      draw_debug_print(1, 4, "MODE : SELECT OBJ/SET TYPE");
      add_force=0;
      if(gPad.LupSwitch) bodyid++;
      if (bodyid>NUM-1) bodyid=NUM-1;
      if(gPad.LdownSwitch) bodyid--;
      if (bodyid<0) bodyid=0;
      if(gPad.RrightSwitch)  (*(bodylist+bodyid))->type=ACTIVE;      
      if(gPad.RleftSwitch) (*(bodylist+bodyid))->type=PASSIVE;

      if ((*(bodylist+bodyid))->type == PASSIVE)
	draw_debug_print(1, 8, "obj: %s  passive",filename[bodyid]);
      else
	draw_debug_print(1, 8, "obj: %s  active",filename[bodyid]);


      draw_debug_print(50, 1, "Lup/Ldown : select object");
      draw_debug_print(50, 2, "Rleft/Rright : passive/active");
      draw_debug_print(50, 3, "SELECT       : MODE CHANGE");	    
      draw_debug_print(50, 4, "START        : EXIT");


      break;                                         

    }       


    
#define STEPS 4
    simulation_with_rollback(bodylist,NUM,1.0f/(60.0f*STEPS),STEPS,t,&mem,calc_force);
    t+=1.0f/60.0f;
    //draw_debug_print(30, 24, "simulation time :%f",t);


    for(i=0;i<NUM;i++){
	err = sceHiCallPlug(frameP[i], SCE_HIG_PRE_PROCESS);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    }

    for(i=0;i<NUM;i++){
	err = sceHiCallPlug(frameP[i], SCE_HIG_POST_PROCESS);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    }


    utilSetDMATimer();
    err = sceHiDMASend();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    err = sceHiDMAWait();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);

    if (gPad.StartSwitch)
	return 1;
    return 0;
}

void SAMPLED2_exit(void)
{
    sceHiErr	err;
    int		i;

    for(i=0;i<NUM;i++){
	err = sceHiCallPlug(frameP[i], SCE_HIG_END_PROCESS);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiErrState.mes);
    }
    for(i=0;i<NUM;i++){
	free(data_addr[i]);
	free(gjk_data_addr[i]);
	if (my_gjkobj[i].type==GJK_MESH) 
	  free_gjkobj_neighbors(&(my_gjkobj[i]));
    }
    free_mem(mem);
}


// caching contacts from the previous frame increases performance !!!
 
//Contact cache c;
static Contact contact_cache[(NUM*(NUM-1))/2];
 
static void init_contact_cache()
{
        int i;
        Contact *cl=&contact_cache[0];
        for (i=0;i<(NUM*(NUM-1))/2;i++) {
          cl[i].simplex.hintB=cl[i].simplex.hintA=cl[i].simplex.Wk=cl[i].simplex.cache_valid=0;
          cl[i].simplex.epsilon=0.0f;
        }
}
 
static void alloc_mem(SimMEM *mem1,int nbody)
{
  mem1->Xstart=(sceVu0FVECTOR *)memalign(16,sizeof(sceVu0FVECTOR)*4*nbody);
  mem1->Xtemp=(sceVu0FVECTOR *)memalign(16,sizeof(sceVu0FVECTOR)*4*nbody);
  mem1->Xend=(sceVu0FVECTOR *)memalign(16,sizeof(sceVu0FVECTOR)*4*nbody);
  mem1->d1=(sceVu0FVECTOR *)memalign(16,sizeof(sceVu0FVECTOR)*4*nbody);
  mem1->d2=(sceVu0FVECTOR *)memalign(16,sizeof(sceVu0FVECTOR)*4*nbody);
  mem1->d3=(sceVu0FVECTOR *)memalign(16,sizeof(sceVu0FVECTOR)*4*nbody);
  mem1->d4=(sceVu0FVECTOR *)memalign(16,sizeof(sceVu0FVECTOR)*4*nbody);
  mem1->d5=(sceVu0FVECTOR *)memalign(16,sizeof(sceVu0FVECTOR)*4*nbody);
  mem1->cache_contact_list=&contact_cache[0];
}                            

static void free_mem(SimMEM mem) {
  return;
  free(mem.Xstart);
  free(mem.Xtemp);
  free(mem.Xend);
  free(mem.d1);
  free(mem.d2);
  free(mem.d3);
  free(mem.d4);
  free(mem.d5);
}
