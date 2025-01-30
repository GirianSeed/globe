/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
*/
/*
 *
 *	Copyright (C) 2002 Sony Computer Entertainment Inc.
 *							All Right Reserved
 *
 */
#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <eeregs.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>
#include <libdma.h>
#include "util.h"
#include "camera.h"
#include "light.h"
#include "vector.h"
#include "math.h"
#include "furball.h"
#include <string.h>
#include <malloc.h>

extern u_int hairshading_micro[];

static sceHiPlugMicroTbl_t my_micro[]={{hairshading_micro, SCE_HIP_MICRO_ATTR_NONE }};

// for performance check
typedef struct {
        int     frame;                  /* current frame count */
        int     cpu_count;              /* cpu processing time */
        int     dma_count;              /* dma processing time */
} _status_t;
     
extern  _status_t    status;    

/***************************************************
 * vu0 micromode -sample 
 ***************************************************/

sceVu0FMATRIX root_mat={{1.0f,0.0f,0.0f,0.0f},{0.0f,1.0f,0.0f,0.0f},{0.0f,0.0f,1.0f,0.0f},{0.0f,0.0f,0.0f,1.0f}};
sceVu0FVECTOR root_rot={0.0f,0.0f,0.0f,0.0f};
sceVu0FVECTOR root_trans={0.0f,0.0f,0.0f,0.0f};
 
u_int hair_count;
 
sceVu0FVECTOR hair_points[NPARTICLE/HAIR_LENGTH];
sceVu0FVECTOR hair_orig[NPARTICLE/HAIR_LENGTH];
 
sceVu0FVECTOR simulation_parameter={10.0f,0.5f,0.0003f,0.0f};// x=hair_tension,y=restlength*restlength,z=tstep*tstep   

#ifdef MEMALIGN
extern sceVu0FVECTOR *data;
#else
extern sceVu0FVECTOR data[NPARTICLE*QW_PER_PARTICLE] __attribute__((aligned(64)));
#endif 


static void	*DATA1_ADDR,*DATA2_ADDR;

static sceHiPlug		*frameP1,*frameP2;
static sceHiPlug		*microP1,*microP2;
static sceHiPlug        *shapeP1;
static sceHiPlug		*tim2P2;
static sceHiPlug        *hrchyP1,*hrchyP2;
static u_int *tim2D;
static sceHiPlugMicroData_t	*microD1,*microD2;

static sceHiPlugTex2dInitArg_t	tim2_arg;
static sceHiPlugMicroPreCalcArg_t	arg;
static sceHiPlugHrchyPreCalcArg_t  hrchy_arg;


static void root_ctrl(void);
static void param_ctrl(void);

sceVu0FVECTOR *vertexP; //pointer to vertex data in HiG Shape-Data structure



extern unsigned int  vu0pstart __attribute__((section(".vudata")));  

void init_root_matrix()
{
 sceVu0UnitMatrix(root_mat);
 sceVu0ScaleVector(root_trans,root_trans,0.0f);
 sceVu0ScaleVector(root_rot,root_rot,0.0f);
}

void init_hair_points()
{
   u_int i;
   float angle1,angle2;
   
   for(i=0;i<NPARTICLE/HAIR_LENGTH;i++)  {
      angle1=PI*random(0.0f,1.0f);
      angle2=2*PI*random(0.0f,1.0f);


      hair_points[i][0]=RADIUS*cosf(angle2)*sinf(angle1);
      hair_points[i][1]=RADIUS*cosf(angle1);
      hair_points[i][2]=RADIUS*sinf(angle2)*sinf(angle1);
      hair_points[i][3]=1.0f;
      sceVu0CopyVector(hair_orig[i],hair_points[i]);
   }
}


void init_data()
{
  u_int i,index;
  u_int np,particles=0;
  
  u_int count=0;  
       
 while(particles<NPARTICLE) {
   if (NPARTICLE-particles<VU0_MAX_PARTICLE) {
      np=NPARTICLE-particles;
   } else {
      np=VU0_MAX_PARTICLE;
   }  
   index=particles*QW_PER_PARTICLE;
   for(i=0;i<np;i++)  {
         if (i%HAIR_LENGTH==0) {
           data[index+np+i][0]=data[index+i][0]=hair_points[count][0];
           data[index+np+i][1]=data[index+i][1]=hair_points[count][1];
           data[index+np+i][2]=data[index+i][2]=hair_points[count][2];
           count++;
         } else {
           data[index+np+i][0]=data[index+i][0]=data[index+i-1][0]+random(-0.1f,0.1f);
           data[index+np+i][1]=data[index+i][1]=data[index+i-1][1]+1.0f+random(-0.1f,0.1f);
           data[index+np+i][2]=data[index+i][2]=data[index+i-1][2]+random(-0.1f,0.1f);
         }
   }
   particles+=np;
 }  
}


void loadmicro()
{
  sceDmaChan* dmaVIF02;
 
  dmaVIF02 = sceDmaGetChan(SCE_DMA_VIF0);
  dmaVIF02->chcr.TTE = 1;
  sceDmaSync(dmaVIF02, 0, 0);
  sceDmaSend(dmaVIF02, &vu0pstart);
  sceDmaSync(dmaVIF02, 0, 0);
}      


void packet_init() {
   u_long *spr;

    spr = (u_long *)SPR;
    spr+=2;
    *(spr++) =  7L <<28; // end DMA tag
    *(spr++) =  ( (u_long) SCE_VIF0_SET_FLUSHE(0) <<32) | SCE_VIF0_SET_MSCAL(0,0);      
}


void copy_data() 
{
 u_int np,particles=0;
 u_int i;  
 sceVu0FVECTOR *ver=vertexP;

 while(particles<NPARTICLE) {
   if (NPARTICLE-particles<VU0_MAX_PARTICLE) {
      np=NPARTICLE-particles;
   } else {
      np=VU0_MAX_PARTICLE;
   }  
   for (i=0;i<np/HAIR_LENGTH;i++) {   
    memcpy(ver,data[particles*QW_PER_PARTICLE+i*HAIR_LENGTH],HAIR_LENGTH*16);
    ver+=HAIR_LENGTH*4+1;
    
   }
   particles+=np;
 }
}




void update_particles()
{
    u_int num_of_sent_particles;
    u_int np,index;
    sceVu0FVECTOR acc;
    //set spr-pointers for double buffering
    static u_int sprP1=SPR+0x20; //the first 2QW are used for DMA tag
    static u_int sprP2=SPR+0x1020; 
    u_int cpu_spr,vu0_spr,temp;
    
   
    DPUT_D_CTRL(DGET_D_CTRL()|D_CTRL_RELE_M);      

    /* setup constants */
    hair_count=0;        //global variable to keep track of processed hair
    acc[1]=10.5f+random(-15.0f,15.0f); //gravity
    acc[0]=random(-35.0f,35.0f); //random wind
    acc[2]=random(-35.0f,35.0f);
    
    /* prolog */    
    if (NPARTICLE<VU0_MAX_PARTICLE) {
     np=NPARTICLE;
    } else {
     np=VU0_MAX_PARTICLE;
    }
    
    /* micro code parameter */
    __asm__ volatile ("ctc2 %0,$vi01;"::"r"(np)); //number of particle
    __asm__ volatile ("lqc2 $vf31,0(%0);"::"r"(&acc)); // particle acceleration   	
    __asm__ volatile ("lqc2 $vf30,0(%0);"::"r"(&root_trans));//origin of the hair-ball
    __asm__ volatile ("lqc2 $vf29,0(%0);"::"r"(&simulation_parameter)); // hair_tension,reslength_sqr,tstep_sqr
 
    send_data(&data[0],np*QW_PER_PARTICLE);//send data and start vu0
    num_of_sent_particles=np; //while vu0 is calculating
    sync_dma_bc0(); //sync dma
    while((vu0_status() &0x1)){}; // Wait for [E]-bit found 
    get_data(sprP1,np*QW_PER_PARTICLE); // get data from Vu0
    sync_dma_bc0();
   
    
    
    if (NPARTICLE>VU0_MAX_PARTICLE) {
      if (NPARTICLE-num_of_sent_particles<VU0_MAX_PARTICLE) {
         np=NPARTICLE-num_of_sent_particles;
      } else {
         np=VU0_MAX_PARTICLE;
      }  
      /* micro code parameter */
      __asm__ volatile ("ctc2 %0,$vi01;"::"r"(np)); //number of particles
      //send data and start vu0 calc.
      send_data(&data[num_of_sent_particles*QW_PER_PARTICLE],np*QW_PER_PARTICLE);
      num_of_sent_particles+=np;
      cpu_spr=sprP1;vu0_spr=sprP2;
      //do cpu calc. and get data from Vu0
      cpu_get_data(cpu_spr,vu0_spr,VU0_MAX_PARTICLE,np);
    }
    
    /* loop */
    while (num_of_sent_particles<NPARTICLE) {
      if (NPARTICLE-num_of_sent_particles<VU0_MAX_PARTICLE) {
         np=NPARTICLE-num_of_sent_particles;
      } else {
         np=VU0_MAX_PARTICLE;
      }  
      /* micro code parameter */
      __asm__ volatile ("ctc2 %0,$vi01;"::"r"(np)); //number of particles
    
      index=num_of_sent_particles-2*VU0_MAX_PARTICLE; 
      
      // sync 
      sync_dma_bc0();      
      //send new data and write results back to MM
      send_data(&(data[num_of_sent_particles*QW_PER_PARTICLE]),np*QW_PER_PARTICLE);
	  
	  //while vu0 is calc. do:
      temp=cpu_spr;cpu_spr=vu0_spr;vu0_spr=temp;  //swap scratch-pad pointers  
      cpu_write_get_data(cpu_spr,vu0_spr,VU0_MAX_PARTICLE,np,index);//do cpu calc. and get data from Vu0 and write data from spr to MM
      num_of_sent_particles+=np;
    }
    
    
    
    /* epilog */
    if (NPARTICLE>VU0_MAX_PARTICLE) {  
      index=num_of_sent_particles-VU0_MAX_PARTICLE-np;
      //write results back to MM 
      write_data(&data[index*QW_PER_PARTICLE][0],cpu_spr,VU0_MAX_PARTICLE*QW_PER_PARTICLE);
      cpu(vu0_spr,np);//do cpu calc.
      index=NPARTICLE-np; 
      sync_dma_bc0();
      //write results back to MM
      write_data(&data[index*QW_PER_PARTICLE][0],vu0_spr,np*QW_PER_PARTICLE);
      sync_dma_bc0();      
    } else {
      cpu(sprP1,np); // do cpu calc.
      index=NPARTICLE-np; 
      //write results back to MM
      write_data(&data[index*QW_PER_PARTICLE][0],sprP1,np*QW_PER_PARTICLE); 
      sync_dma_bc0();      
    }  
}



static int find_vertex_pointer(sceHiPlug *frame)
{
    sceHiPlugShapeHead_t *head, *sh,*mh,*gh;
    sceHiPlug *shape_plug;
    
    sceHiType type1={                   /* SHAPE_PLUG */
        SCE_HIP_COMMON,                         /* repository */
        SCE_HIP_FRAMEWORK,                      /* project */
        SCE_HIP_SHAPE,                          /* category */
        SCE_HIG_PLUGIN_STATUS,                  /* status */
        SCE_HIP_SHAPE_PLUG,                     /* id */
        SCE_HIP_REVISION                        /* revision */
    };
    sceHiType type2={                   /* SHAPE_DATA */
        SCE_HIP_COMMON,                         /* repository */
        SCE_HIP_FRAMEWORK,                      /* project */
        SCE_HIP_SHAPE,                          /* category */
        SCE_HIG_PLUGIN_STATUS,                  /* status */
        SCE_HIP_SHAPE_DATA,                     /* id */
        SCE_HIP_REVISION                        /* revision */
    };
 
    
    if (sceHiGetInsPlug(frame, &shape_plug, type1) != SCE_HIG_NO_ERR) return -1;
 
    head=sceHiPlugShapeGetHead(shape_plug, type2);
    if (head==NULL) return -1;
 
    sh=sceHiPlugShapeGetDataHead(head, 0);
    if (sh==NULL) return -1;
    
    mh=sceHiPlugShapeGetMaterialHead(sh, 0);
    if (mh==NULL) return -1;

    gh=sceHiPlugShapeGetGeometryHead(mh, 0);
    if (gh==NULL) return -1;
    
    vertexP=sceHiPlugShapeGetGeometryVertex(gh, 0); 
    return 0;
}                   
/*********************************************/


void SAMPLE0_init(void)
{
    sceHiErr	err;
    sceHiPlugMicroInitArg_t init_arg;
    sceHiType type;
   
    
    DATA1_ADDR = file_read("hair4.bin");
	DATA2_ADDR = file_read("ball.bin");


    if ((err = sceHiParseHeader((u_int *)DATA1_ADDR)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if ((err = sceHiParseHeader((u_int *)DATA2_ADDR)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);



    if ((err = sceHiGetPlug((u_int *)DATA1_ADDR, "Frame", &frameP1)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if ((err = sceHiGetPlug((u_int *)DATA2_ADDR, "Frame", &frameP2)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    
    
    if ((err = sceHiGetPlug((u_int *)DATA1_ADDR, "Hrchy", &hrchyP1)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if ((err = sceHiGetPlug((u_int *)DATA2_ADDR, "Hrchy", &hrchyP2)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    
    
    hrchy_arg.root=&root_mat;
    hrchyP2->args=(u_int)&hrchy_arg;
    hrchyP1->args=(u_int)&hrchy_arg;

    find_vertex_pointer(frameP1);

    if ((err = sceHiGetPlug((u_int *)DATA2_ADDR, "Tim2", &tim2P2)) == SCE_HIG_NO_ERR) {
    
    type.repository=SCE_HIP_COMMON;
    type.project=SCE_HIP_FRAMEWORK;
    type.category=SCE_HIP_TIM2;
    type.id=SCE_HIP_TIM2_DATA;
    type.revision=SCE_HIP_REVISION;
    type.status=SCE_HIG_DATA_STATUS;
    
    sceHiGetData(tim2P2,&tim2D,type);
    if(tim2D==NULL)
      scePrintf("no tim2 texture found\n");
    
    { int num,i;
      char *name;
      u_int *tim2data;
      
      num = sceHiPlugTim2Num(tim2P2);
      for (i=0;i<num;i++) {
          name = sceHiPlugTim2GetName(tim2P2,i);
          tim2data = file_read(name);
          sceHiPlugTim2SetData(tim2P2,i,tim2data);
      }
    }   

	tim2_arg.resident = TRUE;
	tim2_arg.tbl = NULL;
	tim2P2->args = (u_int)&tim2_arg;
    } else
	tim2P2 = NULL;


    if ((err = sceHiGetPlug((u_int *)DATA1_ADDR, "Shape", &shapeP1)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if ((err = sceHiGetPlug((u_int *)DATA1_ADDR, "Micro", &microP1)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	if ((err = sceHiGetPlug((u_int *)DATA2_ADDR, "Micro", &microP2)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);



    init_arg.tbl=my_micro;
    init_arg.tblnum=1;
    microP1->args =(u_int)&init_arg;
	    
    if ((err = sceHiCallPlug(frameP1, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if ((err = sceHiCallPlug(frameP2, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


    sceHiPlugShapeMasterChainSetting(shapeP1,1);

    if((microD1 = sceHiPlugMicroGetData(microP1)) == NULL)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if((microD2 = sceHiPlugMicroGetData(microP2)) == NULL)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


    camerainit(microD1);
    lightinit(microD1); 
    camerainit(microD2);
    lightinit(microD2); 

    microP1->args = (u_int)&arg;
    arg.micro = 0;

#ifdef MEMALIGN
    data=(sceVu0FVECTOR *)memalign(128,(NPARTICLE*QW_PER_PARTICLE+4+1)*sizeof(u_long128));
    data++;
#endif

    init_root_matrix();
    init_hair_points();
    init_data();
    copy_data();
    FlushCache(0);
    loadmicro();
    packet_init();
    
}
 

enum {ROOT_MODE,PARAM_MODE,CAMERA_MODE,LIGHT_MODE};

#define MAXMODE 3 

int SAMPLE0_main(void)
{
    u_int i;
    sceHiErr	err;
    static int mode = ROOT_MODE;


    if (gPad.SelectSwitch)
	mode = (++mode) > MAXMODE ? ROOT_MODE : mode;

    switch(mode){
      case CAMERA_MODE:
	  cameractrl();
	  cameraview(microD1);
	  cameraview(microD2);
	  break;
      case ROOT_MODE:
	  root_ctrl();
	  for(i=0;i<NPARTICLE/HAIR_LENGTH;i++) sceVu0ApplyMatrix(hair_points[i],root_mat,hair_orig[i]);
          draw_debug_print(50, 1, "Rup/Rdown    : Obj Up/Down");
          draw_debug_print(50, 2, "Rleft/Rright : Obj Left/Right");
          draw_debug_print(50, 3, "R1/R2        : Obj Back/Front");
          draw_debug_print(50, 4, "Lup/Ldown    : Obj Rot X");
          draw_debug_print(50, 5, "Lleft/Lright : Obj Rot Y");
          draw_debug_print(50, 6, "L1/L2        : Obj Rot Z");
          draw_debug_print(50, 7, "SELECT       : MODE CHANGE");
          draw_debug_print(50, 8, "START        : EXIT");     
	  break;
      case LIGHT_MODE:
	  lightctrl(microD1,DIR_LIGHT);
	  lightctrl(microD2,DIR_LIGHT);
	  break;
      case PARAM_MODE:
          param_ctrl();
          draw_debug_print(50, 1, "Rup/Rdown    : hair tention");
          draw_debug_print(50, 2, "Rleft/Rright : hair length");
          draw_debug_print(50, 3, "R1/R2        : sim time");
          draw_debug_print(50, 7, "SELECT       : MODE CHANGE");
          draw_debug_print(50, 8, "START        : EXIT");         
          break;
    }


#ifdef DOUBLE
    sceHiDMASend();
    sceHiDMASwap();
#endif
   

    FlushCache(0);
    *T1_COUNT=0;

    update_particles(); // hair dynamics simulation


    status.cpu_count=*T1_COUNT;

#ifndef DOUBLE
    /* update shape */
    copy_data();   
#endif

#ifdef DOUBLE
    sceHiStopPlugStatus(hrchyP1);
    sceHiStopPlugStatus(hrchyP2);
#endif
    
    if ((err = sceHiCallPlug(frameP1, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if ((err = sceHiCallPlug(frameP2, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if ((err = sceHiCallPlug(frameP1, SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if ((err = sceHiCallPlug(frameP2, SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (gPad.StartSwitch) {
	return 1;
    }




#ifndef DOUBLE
    sceHiDMASend();
#endif

    if ((err =   sceHiDMAWait()) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


#ifdef DOUBLE 
    /* update shape */
    copy_data();
    sceHiContPlugStatus(hrchyP1);
    if ((err = sceHiCallPlug(hrchyP1, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    sceHiContPlugStatus(hrchyP2);
    if ((err = sceHiCallPlug(hrchyP2, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
            error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);     
#endif
 
    return 0;
}

void SAMPLE0_exit(void)
{
    sceHiErr	err;

    if ((err = sceHiCallPlug(frameP1, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    if ((err = sceHiCallPlug(frameP2, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);



    free(DATA1_ADDR);
    free(DATA2_ADDR);
#ifdef MEMALIGN
   data--;
   free(data);
#endif    
}



static void root_ctrl(void)
{
    float  dr = 5.5f / 180.0f * PI;
    float  dt = 0.3f;

	if (gPad.LdownOn) {
		root_rot[0]-=dr; 
		if(root_rot[0]<-PI) root_rot[0] += 2.0f*PI; 
	}
	if(gPad.LupOn) {
		root_rot[0]+=dr;
		if(root_rot[0]>PI) root_rot[0] -= 2.0f*PI; 
	}
	if(gPad.LrightOn) {
		root_rot[1]+=dr;
		if(root_rot[1]>PI) root_rot[1] -= 2.0f*PI; 
	}
	if(gPad.LleftOn) {
		root_rot[1]-=dr;
		if(root_rot[1]<-PI) root_rot[1] += 2.0f*PI; 
	}
	if(gPad.L1On) {
		root_rot[2]+=dr;
		if(root_rot[2]>PI) root_rot[2] -= 2.0f*PI; 
	}
	if(gPad.L2On) {
		root_rot[2]-=dr;
		if(root_rot[2]<-PI) root_rot[2] += 2.0f*PI; 
	}
	if(gPad.RdownOn)
		root_trans[1]+=dt;
	if(gPad.RupOn)
		root_trans[1] -= dt;
	if(gPad.RrightOn)
		root_trans[0] += dt;
	if(gPad.RleftOn)
		root_trans[0] -= dt;
	if(gPad.R1On)
		root_trans[2] -= dt;
	if(gPad.R2On)
		root_trans[2] += dt;

	sceVu0UnitMatrix(root_mat);
	sceVu0RotMatrixX(root_mat, root_mat, root_rot[0]);
	sceVu0RotMatrixY(root_mat, root_mat, root_rot[1]);
	sceVu0RotMatrixZ(root_mat, root_mat, root_rot[2]);
	sceVu0TransMatrix(root_mat, root_mat, root_trans);

	/* infomation */
	draw_debug_print(1, 6, "ROOT POS : %5.2f %5.2f %5.2f",
			 root_trans[0],
			 root_trans[1],
			 root_trans[2]);
	draw_debug_print(1, 7, "ROOT ROT : %5.2f %5.2f %5.2f",
			 root_rot[0],
			 root_rot[1],
			 root_rot[2]);
			 
}


static void param_ctrl(void)
{
    float  length_dt = 0.03f;
    float  tension_dt = 0.05f;
    float  time_dt = 0.000001f;


        if(gPad.RdownOn) 
                simulation_parameter[0]+=tension_dt;
        if(gPad.RupOn)
                simulation_parameter[0] -=tension_dt;
        if(gPad.RrightOn)
                simulation_parameter[1] +=length_dt;
        if(gPad.RleftOn)
                simulation_parameter[1] -=length_dt;
        if(gPad.R1On)
                simulation_parameter[2] -=time_dt;
        if(gPad.R2On)
                simulation_parameter[2] +=time_dt;
 
        if(simulation_parameter[0]<=0.0f) simulation_parameter[0]=tension_dt;
        if(simulation_parameter[1]<=0.0f) simulation_parameter[1]=length_dt;
        if(simulation_parameter[2]<=0.0f) simulation_parameter[2]=time_dt;
            
        /* infomation */
        draw_debug_print(1, 6, "tension %f",simulation_parameter[0]);
        draw_debug_print(1, 7, "hair length %f",simulation_parameter[1]);
        draw_debug_print(1, 8, "tstep %f",simulation_parameter[2]);
 
}                                   
