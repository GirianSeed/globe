/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          Name : sample.c
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            1.0     Nov 28, 2000     eder      Initial
 */                                                            
#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <eeregs.h>
#include <libvu0.h>
#include <libdma.h>
#include <libhig.h>
#include <libhip.h>
#include "util.h"
#include "camera.h"
#include "collision.h"

#define PI      3.14159265f
float  static dr = 0.5f / 180.0f * PI;
float  static dt = 0.2f;



#define	NUM	2				/* num of obj*/

enum {OBJA,OBJB};

/****************************************/
/*	GJK Collision Detection Sample    */
/****************************************/

void SAMPLECD_init();
int SAMPLECD_main();
void SAMPLECD_exit();
static void _obj2_ctrl(void);

static void		*data_addr[NUM];
static void		*gjk_data_addr[NUM];
static sceHiPlug	*frameP[NUM];
static sceHiPlug	*microP[NUM];
static u_int		*microD[NUM];
static sceHiPlug	*shapeP[NUM];
static u_int		*shapeD[NUM];

/* for obj_crtl */
static sceHiPlug   *hrchyP[NUM];
static sceHiPlugHrchyPreCalcArg_t       hrchy_arg[NUM];
static sceVu0FMATRIX root_mat[NUM]=      {
  {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}},
  {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}}   };
static sceVu0FVECTOR root_rot2={0,0,0,0};
static sceVu0FVECTOR root_trans2={0,0,0,1}; 

/* pointer to Basematrix-data */
static sceVu0FMATRIX *obj_local_world[NUM];
static sceVu0FMATRIX *obj_local_world_rot[NUM];


static Simplex my_simplex;
static GJKObj my_gjkobj[NUM];


static void point_draw(sceVu0FVECTOR *vect);
static void change_coll_function(int coll_mode,int calc_points);

void gjk_init() {
  my_gjkobj[OBJA].type=GJK_MESH;
  my_gjkobj[OBJA].lw=obj_local_world[OBJA];
  my_gjkobj[OBJA].lwrot=obj_local_world_rot[OBJA];

  my_gjkobj[OBJB].type=GJK_MESH;
  my_gjkobj[OBJB].lw=obj_local_world[OBJB];
  my_gjkobj[OBJB].lwrot=obj_local_world_rot[OBJB];

  my_simplex.cache_valid=0;
  my_simplex.epsilon=0.0000002;
  my_simplex.hintA=0;
  my_simplex.hintB=0;
}

void reset_simplex() {
  my_simplex.cache_valid=0;
}

void SAMPLECD_init(void)
{
    sceHiErr	err;
    sceHiType	type;
    char	filename[NUM][40] = {"data/block.bin","data/ellipsoid.bin"};
    char	framename[NUM][12] = {"Frame","Frame"};
    char       gjk_filename[NUM][40] = {"data/cblock.bin","data/cellipsoid.bin"};
    int	i;

    /* allocate neighbor-lists for gjk-meshes */
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
      if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
      err = sceHiGetPlug((u_int *)data_addr[i], framename[i], &frameP[i]);
      if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
      
      type.repository = SCE_HIP_COMMON;
      type.project = SCE_HIP_FRAMEWORK;
      type.category = SCE_HIP_MICRO;
      type.status = SCE_HIG_PLUGIN_STATUS;
      type.id = SCE_HIP_MICRO_PLUG;
      type.revision = SCE_HIP_REVISION;
      err = sceHiGetInsPlug(frameP[i], &microP[i], type);
      if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
      
      type.status = SCE_HIG_DATA_STATUS;
      type.id = SCE_HIP_MICRO_DATA;
      err = sceHiGetData(microP[i], &microD[i], type);
      if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
      
      type.repository = SCE_HIP_COMMON;
      type.project = SCE_HIP_FRAMEWORK;
      type.category = SCE_HIP_SHAPE;
      type.status = SCE_HIG_PLUGIN_STATUS;
      type.id = SCE_HIP_SHAPE_PLUG;
      type.revision = SCE_HIP_REVISION;
      err = sceHiGetInsPlug(frameP[i], &shapeP[i], type);
      if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
      
      type.repository = SCE_HIP_COMMON;
      type.project = SCE_HIP_FRAMEWORK;
      type.category = SCE_HIP_SHAPE;
      type.status = SCE_HIG_DATA_STATUS;
      type.id = SCE_HIP_BASEMATRIX;
      type.revision = SCE_HIP_REVISION;
      err = sceHiGetData(shapeP[i], &shapeD[i], type);
      if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
      obj_local_world[i]= (sceVu0FMATRIX *)(shapeD[i]+8);
      obj_local_world_rot[i] =  (sceVu0FMATRIX *)(shapeD[i]+8+4*4);



      /* get hrchy_root for obj_ctrl */
      err = sceHiGetPlug((u_int *)data_addr[i], "Hrchy", &hrchyP[i]);
      if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
      hrchy_arg[i].root= &root_mat[i];
      hrchyP[i]->args = (u_int)&hrchy_arg[i];
    }
  

    /*	INIT PROCESS	*/
    for(i=0;i<NUM;i++){
	err = sceHiCallPlug(frameP[i], SCE_HIG_INIT_PROCESS);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    camera_init();
    light_init();
    collision_init(); /*load Vu0 micro-code */
    gjk_init();     
}

extern _light_t _light[3][3];


int SAMPLECD_main(void)
{
    sceHiErr	err;
    int		i;
    static int mode = 0;
    static int coll_mode=0;
    static int calc_points=1;
    if (gPad.SelectSwitch)
	mode = (++mode) > 3 ? 0 : mode;
 
    switch (mode) {
      case 0:
	draw_debug_print(1, 4, "MODE : CAMERA MOVE");
	camera_ctrl();
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
      draw_debug_print(1, 4, "MODE : LIGHT CTRL");
      light_ctrl(PARA_LIGHT);
      /* light info */
      draw_debug_print(50, 1, "Rup/Rdown    : Light Up/Down");
      draw_debug_print(50, 2, "Rleft/Rright : Light Left/Right");
      draw_debug_print(50, 3, "R1/R2        : Light Back/Front");
      draw_debug_print(50, 4, "Lup/Ldown    : ITEM CHANGE");
      draw_debug_print(50, 5, "Lleft/Lright : LIGHT CHANGE");
      draw_debug_print(50, 7, "SELECT       : MODE CHANGE");	    
      draw_debug_print(50, 8, "START        : EXIT");	    
      break;
    case 2:
      draw_debug_print(1, 4, "MODE : OBJ CTRL");
      _obj2_ctrl();
      /* controll print */
      draw_debug_print(50, 1, "Rup/Rdown    : Obj Up/Down");
      draw_debug_print(50, 2, "Rleft/Rright : Obj Left/Right");
      draw_debug_print(50, 3, "R1/R2        : Obj Back/Front");
      draw_debug_print(50, 4, "Lup/Ldown    : Obj Rot X");
      draw_debug_print(50, 5, "Lleft/Lright : Obj Rot Y");
      draw_debug_print(50, 6, "L1/L2        : Obj Rot Z");
      draw_debug_print(50, 7, "SELECT       : MODE CHANGE");
      draw_debug_print(50, 8, "START        : EXIT");
      break;                                         
    case 3:
      draw_debug_print(1, 4, "MODE : COLL CTRL");
      draw_debug_print(50, 1, "Rleft/Rright : change mode");
      if(gPad.RleftSwitch) {coll_mode-=1;if (coll_mode<0) coll_mode=0;}
      if(gPad.RrightSwitch) {
	/*reset simplex if you switch form mode0 to mode1 */
	/*because last result might not be valid */
	if(coll_mode==0) reset_simplex();
	coll_mode+=1;if (coll_mode>2) coll_mode=2;
      }
      if(coll_mode!=0) {
	draw_debug_print(50, 2, "Rup/Rdown : points on/off");
	if(gPad.RdownSwitch) {calc_points-=1;if (calc_points<0) calc_points=0;}
	if(gPad.RupSwitch)   {calc_points+=1;if (calc_points>1) calc_points=1;}
      }
      draw_debug_print(50, 7, "SELECT       : MODE CHANGE");	    
      draw_debug_print(50, 8, "START        : EXIT");
    }       

    for(i=0;i<NUM;i++){
	camera_set((u_int *)microD[i]);
	light_set((u_int *)microD[i]);
    }

    /* PRE-process has to be called bevor is_collision/calc_dist to
       update the loca_world/light_rot matrices */
    for(i=0;i<NUM;i++){
	err = sceHiCallPlug(frameP[i], SCE_HIG_PRE_PROCESS);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    change_coll_function(coll_mode,calc_points);

    for(i=0;i<NUM;i++){
	err = sceHiCallPlug(frameP[i], SCE_HIG_POST_PROCESS);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }


    utilSetDMATimer();
    err = sceHiDMASend();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiDMAWait();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (gPad.StartSwitch)
	return 1;
    return 0;
}

void SAMPLECD_exit(void)
{
    sceHiErr	err;
    int		i;

    for(i=0;i<NUM;i++){
	err = sceHiCallPlug(frameP[i], SCE_HIG_END_PROCESS);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    for(i=0;i<NUM;i++){
	free(data_addr[i]);
	free(gjk_data_addr[i]);
	if (my_gjkobj[i].type==GJK_MESH)
	  free_gjkobj_neighbors(&(my_gjkobj[i]));
    }
}




static void _obj2_ctrl(void)
{
  
  if (gPad.LdownOn) {
    root_rot2[0]+=dr;
    if(root_rot2[0]>PI) root_rot2[0] -= 2.0f*PI;
  }
  if(gPad.LupOn) {
    root_rot2[0]-=dr;
    if(root_rot2[0]<-PI) root_rot2[0] += 2.0f*PI;
  }
  if(gPad.LrightOn) {
    root_rot2[1]-=dr;
    if(root_rot2[1]<-PI) root_rot2[1] += 2.0f*PI;
  }
  if(gPad.LleftOn) {
    root_rot2[1]+=dr;
    if(root_rot2[1]>PI) root_rot2[1] -= 2.0f*PI;
  }
  if(gPad.L1On) {
    root_rot2[2]-=dr;
    if(root_rot2[2]<-PI) root_rot2[2] += 2.0f*PI;
  }
  if(gPad.L2On) {
    root_rot2[2]+=dr;
    if(root_rot2[2]>PI) root_rot2[2] -= 2.0f*PI;
  }          
  if(gPad.RdownOn)
    root_trans2[1]+=dt;
  if(gPad.RupOn)
    root_trans2[1] -= dt;
  if(gPad.RrightOn)
    root_trans2[0] += dt;
  if(gPad.RleftOn)
    root_trans2[0] -= dt;
  if(gPad.R1On)
    root_trans2[2] -= dt;
  if(gPad.R2On)
    root_trans2[2] += dt;
  
  sceVu0UnitMatrix(root_mat[OBJB]);
  sceVu0RotMatrixX(root_mat[OBJB], root_mat[OBJB], root_rot2[0]);
  sceVu0RotMatrixY(root_mat[OBJB], root_mat[OBJB], root_rot2[1]);
  sceVu0RotMatrixZ(root_mat[OBJB], root_mat[OBJB], root_rot2[2]);
  sceVu0TransMatrix(root_mat[OBJB], root_mat[OBJB], root_trans2);
  
  /* infomation */
  draw_debug_print(1, 6, "OBJ ROOT POS : %5.2f %5.2f %5.2f",
		   root_trans2[0],
		   root_trans2[1],
		   root_trans2[2]);
  draw_debug_print(1, 7, "OBJ ROOT ROT : %5.2f %5.2f %5.2f",
		   root_rot2[0],
		   root_rot2[1],
		   root_rot2[2]);
 
}                                  


extern CAMERA_DAT cam;  

static void point_draw(sceVu0FVECTOR *vect)
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

#define       CUBE_SIZE	0.1
    sceVu0FMATRIX	work;
    sceVu0FVECTOR	v00, v01;
    sceVu0IVECTOR	v02;
    sceVu0FVECTOR	color;
    sceVu0FVECTOR	point;


    color[0]=255.0f;color[1]=0.0f;color[2]=0.0f;color[3]=255.0f;
    sceVu0CopyVector(point, *vect);


    point[3] = 1.0f;

    pack.count = 0;
    pack.buf[pack.count].ul64[0] = SCE_GIF_SET_TAG(14, 1, 1, 0x04, 0, 2);
    pack.buf[pack.count].ul64[1] = 0x0041L;
    pack.count++;
    sceVu0UnitMatrix(work);
    for (j = 0; j < 14; j++) {
	sceVu0ApplyMatrix(v00, work, cube[j]);
	sceVu0ScaleVector(v00, v00, CUBE_SIZE);
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


static void change_coll_function(int coll_mode,int calc_points)
{
  int coll;
  float dist;
  sceVu0FVECTOR help;

    if (coll_mode==0) {
      coll=is_collision(&my_simplex,&my_gjkobj[OBJA],&my_gjkobj[OBJB]);
      draw_debug_print(30, 23, "COLLISON CHECK");
      if (coll) {
	draw_debug_print(50, 25, "COLLISION!!!"); 
      } else {
	draw_debug_print(50, 25, "NO COLLISION");
      }
    }


    if (coll_mode!=0) {
      dist=calc_dist(&my_simplex,&my_gjkobj[OBJA],&my_gjkobj[OBJB],calc_points);
      if (coll_mode==1) {
	draw_debug_print(30, 23, "CALC. DISTANCE");
      }
      else draw_debug_print(30, 23, "FOLLOW SHORTEST DISTANCE");
      
      dist=sqrtf(dist);
      draw_debug_print(30, 24, "shortest distance: %f",dist);

      if (coll_mode==2 && dist > 0.1) {
	sceVu0Normalize(help,my_simplex.cache_vect);

	(root_mat[OBJA])[3][0]-=0.1*help[0];
	(root_mat[OBJA])[3][1]-=0.1*help[1];
	(root_mat[OBJA])[3][2]-=0.1*help[2];
      } 


      if (dist!=0 && calc_points){
	my_simplex.closest_pointA[3]=1.0f;
	my_simplex.closest_pointB[3]=1.0f;
	sceVu0ApplyMatrix(my_simplex.closest_pointA,*my_gjkobj[OBJA].lw,my_simplex.closest_pointA);
	sceVu0ApplyMatrix(my_simplex.closest_pointB,*my_gjkobj[OBJB].lw,my_simplex.closest_pointB);
	point_draw(&my_simplex.closest_pointA);
	point_draw(&my_simplex.closest_pointB);	
      }
    }
}
