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
#include <math.h>
#include "util.h"
#include "camera.h"
#include "light.h"

#define      PORTNUM 3                      /* num of effects */
enum { TEXTURE_PORT, REFLECT_PORT, REFRACT_PORT };	/* Port  ID */

#define	NUM	6			/* num of scenes */
enum { REFLECT, REFLECT_FISH_EYE, REFRACT, REFRACT_FISH_EYE,  ROOM, REFOBJ };	/* Scene ID */

/* MAPING TEXTURE */
#define  MAP_TEX_SIZE 256 
#define  MAP_TEX_ZDEPTH SCE_GS_PSMZ24 
#define  MAP_TEX_COLOR_FORMAT  SCE_GS_PSMCT16S
static sceHiGsMemTbl    *tex_tbl=NULL;      /* buffer addres for texture render*/  


/* REFOBJ PlugBlk */
static sceHiPlug   *hrchyP;
static sceHiPlug   *shapeP;

/* for root_crtl */
static sceVu0FMATRIX root_mat={{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
static sceVu0FVECTOR root_rot={0,0,0,0};
static sceVu0FVECTOR root_trans={0,0,0,1};
static sceHiPlugHrchyPreCalcArg_t	hrchy_arg;

/* for back_ground texture allocation */
static sceHiPlugTex2dInitArg_t  tex2d_arg;

/* for reflection_camera setting */
static sceVu0FVECTOR *ref_obj_pivot;
static sceVu0FMATRIX *ref_obj_local_world;


/****************************************/
/*	Micro Codes			*/
/****************************************/
extern u_int sce_micro_reflectR[];
extern u_int sce_micro_refractR[];
extern u_int sce_micro_fisheye[];
extern u_int sce_micro_cull[];

typedef struct{
    sceVu0FVECTOR dir;
    sceVu0FVECTOR up;
    sceVu0FVECTOR pos;
} REF_CAMERA;

/****************************************/
/*	Drawport			*/
/****************************************/
static sceHiGsCtx *port[PORTNUM];		/* for reflection, refraction Render */

static void drawport_init(void)
{
    sceHiErr err;
  
    u_int fbp; 
    u_int th,tw;

    /* texture port */
    port[TEXTURE_PORT]=sceHiGsCtxCreate(0);
    if(port[TEXTURE_PORT] == NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsCtxSetDefaultValues(port[TEXTURE_PORT],MAP_TEX_COLOR_FORMAT,MAP_TEX_ZDEPTH,1, MAP_TEX_SIZE, MAP_TEX_SIZE);
    fbp=tex_tbl->addr/2048;
    /* set buffer pointers */
    err = sceHiGsCtxSetLumpBuffer(port[TEXTURE_PORT], fbp);
    /* set clear color */
    err = sceHiGsCtxSetClearColor(port[TEXTURE_PORT], 0, 0, 0, 0);
    err = sceHiGsCtxUpdate(port[TEXTURE_PORT]);


    if(!MAP_TEX_SIZE) {
	th = 0;
    } else {
	th = (u_int) (0.5f+logf((float)MAP_TEX_SIZE)/logf(2.0f));
    }
    if(!MAP_TEX_SIZE) {
	tw = 0;
    } else {
	tw = (u_int) (0.5f+logf((float)MAP_TEX_SIZE)/logf(2.0f));
    }   

    /* default port */
    port[REFLECT_PORT]=sceHiGsCtxCreate(1);
    if(port[REFLECT_PORT] == NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsCtxCopy(port[REFLECT_PORT], sceHiGsStdCtx);
    err = sceHiGsCtxSetRegAlpha(port[REFLECT_PORT], SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_ZERO, 0x47); /* NONE */  
    err = sceHiGsCtxSetRegTest(port[REFLECT_PORT], SCE_GS_FALSE, SCE_GS_ALPHA_NEVER, 0, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GREATER); 
    err = sceHiGsCtxSetRegTex0(port[REFLECT_PORT], tex_tbl->addr/64, (MAP_TEX_SIZE+63)/64, MAP_TEX_COLOR_FORMAT, tw, th, 1,SCE_GS_DECAL , 0, 0, 0, 0, 0);  
    err = sceHiGsCtxUpdate(port[REFLECT_PORT]);

    port[REFRACT_PORT]=sceHiGsCtxCreate(1);
    if(port[REFRACT_PORT] == NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsCtxCopy(port[REFRACT_PORT], sceHiGsStdCtx);
    err = sceHiGsCtxSetRegAlpha(port[REFRACT_PORT], SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CD, 0x47); /* ADD */
    err = sceHiGsCtxSetRegTest(port[REFRACT_PORT], SCE_GS_FALSE, SCE_GS_ALPHA_NEVER, 0, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL); 
    err = sceHiGsCtxSetRegTex0(port[REFRACT_PORT], tex_tbl->addr/64, (MAP_TEX_SIZE+63)/64, MAP_TEX_COLOR_FORMAT, tw, th, 1,SCE_GS_DECAL , 0, 0, 0, 0, 0);  
    err = sceHiGsCtxUpdate(port[REFRACT_PORT]);
}


static void drawport_exit(void)
{
    sceHiErr	err;
    int		i;

    for(i=0;i<PORTNUM;i++){
	err = sceHiGsCtxDelete(port[i]);
    }
}


static void drawport_alpha(int reflection_alpha,int refraction_alpha)
{
    sceHiErr err;
  
    err = sceHiGsCtxSetRegAlpha(port[REFLECT_PORT], SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_ZERO, reflection_alpha); /* NONE */   
    err = sceHiGsCtxUpdate(port[REFLECT_PORT]);
  

    err = sceHiGsCtxSetRegAlpha(port[REFRACT_PORT], SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CD, refraction_alpha); /* ADD */  
    err = sceHiGsCtxUpdate(port[REFRACT_PORT]);
}

/****************************************/
/*	Environment Mapping Sample	*/
/****************************************/
static void root_ctrl(void);
static void ref_camera_init();
static void set_camera_reflection(REF_CAMERA *camera);
static void set_camera_refraction(REF_CAMERA *camera);

#define NUM_OF_MICRO	5
static void		*data_addr[NUM];
static sceHiPlug	*frameP[NUM];
static sceHiPlug	*microP[NUM_OF_MICRO];		/* REFOBJ has no microP !*/
static sceHiPlugMicroData_t *microD[NUM_OF_MICRO];	/* REFOBJ has no microP !*/

void SAMPLE21_init(void)
{
    sceHiErr	err;
    sceHiType	type;
    char	filename[NUM][24] = {"data/reflect.bin","data/reflect_fish.bin","data/refract.bin","data/refract_fish.bin","data/refroom.bin","data/refobj.bin"};
    char	framename[NUM][12] = {"Reflect","Reflect_Fish","Refract","Refract_Fish","Frame","Frame"};
    int		i;
    sceHiPlugFishEyeInitArg_t fish_eye_init_arg;
    sceHiPlug   *backgroundTex2dP;
    size_t        size;

    sceHiPlugMicroTbl_t	micro_tbl[NUM_OF_MICRO] = {
	{ sce_micro_reflectR,   SCE_HIP_MICRO_ATTR_NONE },
	{ sce_micro_fisheye,   SCE_HIP_MICRO_ATTR_NONE },
	{ sce_micro_refractR,   SCE_HIP_MICRO_ATTR_NONE },
	{ sce_micro_fisheye,   SCE_HIP_MICRO_ATTR_NONE },
	{ sce_micro_cull,   SCE_HIP_MICRO_ATTR_NONE }
    };
    sceHiPlugMicroInitArg_t	micro_arg[NUM_OF_MICRO] = {
	{&micro_tbl[REFLECT], 1},
	{&micro_tbl[REFLECT_FISH_EYE], 1},
	{&micro_tbl[REFRACT], 1},
	{&micro_tbl[REFRACT_FISH_EYE], 1},
	{&micro_tbl[ROOM], 1}
    };

    /*	Get Micro Plug & Data	*/
    for(i=0;i<NUM_OF_MICRO;i++){
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
	microP[i]->args = (u_int)&micro_arg[i];		/* exec micro id default 0 */

	if((microD[i] = sceHiPlugMicroGetData(microP[i])) == NULL)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    /*	load REFOBJ	*/
    data_addr[REFOBJ] = file_read(filename[REFOBJ]);
    err = sceHiParseHeader((u_int *)data_addr[REFOBJ]);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGetPlug((u_int *)data_addr[REFOBJ], framename[REFOBJ], &frameP[REFOBJ]);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* get hrchy	*/
    err = sceHiGetPlug((u_int *)data_addr[REFOBJ], "Hrchy", &hrchyP);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    hrchy_arg.root= &root_mat;
    hrchyP->args = (u_int)&hrchy_arg;

    /* get shape	*/
    err = sceHiGetPlug((u_int *)data_addr[REFOBJ], "Shape", &shapeP);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* set fish_eye init parameters */
    fish_eye_init_arg.zdepth=MAP_TEX_ZDEPTH;
    fish_eye_init_arg.rmin=1.0;
    fish_eye_init_arg. rmax=16000000;
    frameP[REFLECT_FISH_EYE]->args=(u_int)&fish_eye_init_arg;	
    frameP[REFRACT_FISH_EYE]->args=(u_int)&fish_eye_init_arg;

    /* allocate tex-memory for tex-mapping */
    size = sceHiGsPageSize(MAP_TEX_SIZE,MAP_TEX_SIZE,MAP_TEX_ZDEPTH)+sceHiGsPageSize(MAP_TEX_SIZE,MAP_TEX_SIZE,MAP_TEX_COLOR_FORMAT);
    tex_tbl   = sceHiGsMemAlloc(SCE_HIGS_PAGE_ALIGN, size); 

    /* ROOM tex allocation */
    err = sceHiGetPlug((u_int *)data_addr[ROOM], "Tex2d", &backgroundTex2dP);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    tex2d_arg.resident = TRUE;
    tex2d_arg.tbl = NULL; 
    backgroundTex2dP->args = (u_int)&tex2d_arg;

    /*	INIT PROCESS	*/
    for(i=0;i<NUM;i++){
	err = sceHiCallPlug(frameP[i], SCE_HIG_INIT_PROCESS);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    for(i=0;i<NUM_OF_MICRO;i++){
	camerainit(microD[i]);
	lightinit(microD[i]);
    }
    ref_camera_init();
    drawport_init();
}

static float  refract_index=0.05;
static float  reflect_zoom=1.0;
static float  refract_zoom=1.0;
static int  reflect_alpha=0x47;
static int  refract_alpha=0x47;
static float reflect_z_shift=-1.0; /* avoid "out of range" reflection */
static float refract_z_shift=0.0;

static    sceHiPlugFishEyePreArg_t	reflect_fish_arg;
static    sceHiPlugFishEyePreArg_t	refract_fish_arg;
static    sceHiPlugReflectPreArg_t	reflect_arg;
static    sceHiPlugRefractPreArg_t	refract_arg;

enum {CAMERA_MODE,LIGHT_MODE,ROOT_MODE,REFRACT_MODE,REFLECT_MODE,ALPHA_MODE};

int SAMPLE21_main(void)
{
    sceHiErr	err;
    REF_CAMERA reflect_camera,refract_camera;
    int		i;
    static int mode = CAMERA_MODE;

    if (gPad.SelectSwitch)
	mode = (++mode) > ALPHA_MODE ? CAMERA_MODE : mode;

    switch (mode) {
      case CAMERA_MODE:
	  draw_debug_print(1, 4, "MODE : CAMERA CTRL");
	  cameractrl();
	  cameraview(microD[ROOM]);
	  break;
      case LIGHT_MODE:
	  draw_debug_print(1, 4, "MODE : LIGHT CTRL");
	  lightctrl(microD[ROOM],DIR_LIGHT);
	  break;
      case ROOT_MODE:
	  draw_debug_print(1, 4, "MODE : ROOT CTRL");
	  root_ctrl();
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
      case REFRACT_MODE:
	  if (gPad.RleftOn) {
	      refract_index-=0.001;
	  }
	  if (gPad.RrightOn) {
	      refract_index+=0.001;
	  }      
	  if (gPad.RdownOn) {
	      refract_zoom-=0.001;
	  }
	  if (gPad.RupOn) {
	      refract_zoom+=0.001;
	  }
	  if (gPad.R1On) {
	      refract_z_shift-=0.001;
	  }
	  if (gPad.R2On) {
	      refract_z_shift+=0.001;
	  }
	  draw_debug_print(1, 4, "MODE : REFRACT_INDEX CTRL %f",refract_index);
	  draw_debug_print(1, 5, "       Zoom: %f",refract_zoom);
	  draw_debug_print(1, 6, "       zShift: %f",refract_z_shift);     
	  draw_debug_print(50, 1, "Rleft/Rright : Index Param");
	  draw_debug_print(50, 2, "Rup/Rdown    : Zoom Param");
	  draw_debug_print(50, 3, "R1/R2        : Shift Param");
	  draw_debug_print(50, 7, "SELECT       : MODE CHANGE");
	  draw_debug_print(50, 8, "START        : EXIT");
	  break;
      case REFLECT_MODE:
	  if (gPad.RdownOn) {
	      reflect_zoom-=0.001;
	  }
	  if (gPad.RupOn) {
	      reflect_zoom+=0.001;
	  }
	  if (gPad.R1On) {
	      reflect_z_shift-=0.001;
	  }
	  if (gPad.R2On) {
	      reflect_z_shift+=0.001;
	  }   
      
	  draw_debug_print(1, 4, "MODE : REFLECT CTRL ");
	  draw_debug_print(1, 5, "       Zoom: %f",reflect_zoom);
	  draw_debug_print(1, 6, "       zShift: %f",reflect_z_shift);
	  draw_debug_print(50, 2, "Rup/Rdown    : Zoom Param");
	  draw_debug_print(50, 3, "R1/R2        : Shift Param");
	  draw_debug_print(50, 7, "SELECT       : MODE CHANGE");
	  draw_debug_print(50, 8, "START        : EXIT");
	  break;
      case ALPHA_MODE:
	  if (gPad.RleftOn) {
	      reflect_alpha-=1;
	      if (reflect_alpha<0) reflect_alpha=0;
	  }
	  if (gPad.RrightOn) {
	      reflect_alpha+=1;
	      if (reflect_alpha>255) reflect_alpha=255;
	  }
	  if (gPad.RdownOn) {
	      refract_alpha-=1;
	      if (refract_alpha<0) refract_alpha=0;
	  }
	  if (gPad.RupOn) {
	      refract_alpha+=1;
	      if (refract_alpha>255) refract_alpha=255;
	  }
	  draw_debug_print(1, 4, "MODE : Blending  CTRL  reflect %d",reflect_alpha);
	  draw_debug_print(1, 5, "                       refract %d",refract_alpha);
	  drawport_alpha(reflect_alpha,refract_alpha);
	  draw_debug_print(50, 1, "Rleft/Rright : Reflect Param");
	  draw_debug_print(50, 2, "Rup/Rdown    : Refract Param");
	  draw_debug_print(50, 7, "SELECT       : MODE CHANGE");	    
	  draw_debug_print(50, 8, "START        : EXIT");
    }
    
    lightcopy(microD[REFLECT_FISH_EYE],microD[ROOM],DIR_LIGHT);
    lightcopy(microD[REFRACT_FISH_EYE],microD[ROOM],DIR_LIGHT);

    cameracopy(microD[REFLECT], microD[ROOM]);
    cameracopy(microD[REFRACT], microD[ROOM]);


    /* PRE process for reflection obj */ 
    /* get local_world from reflection obj */
    err = sceHiCallPlug(frameP[REFOBJ], SCE_HIG_PRE_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


    /* set fish_plug paramter */
    set_camera_reflection(&reflect_camera); /* get camera pos for reflection-texture */
    reflect_fish_arg.camera_pos=&reflect_camera.pos;
    reflect_fish_arg.camera_zdir=&reflect_camera.dir;
    reflect_fish_arg.camera_up=&reflect_camera.up;
    reflect_fish_arg.tex_size=MAP_TEX_SIZE+5; /* add some pixel for safty */
    frameP[REFLECT_FISH_EYE]->args = (u_int)&reflect_fish_arg;  
    
    /* set reflection_plug parameter */
    reflect_arg.camera_pos=&reflect_camera.pos;
    reflect_arg.camera_zdir=&reflect_camera.dir;
    reflect_arg.camera_up=&reflect_camera.up;
    reflect_arg.zoom=reflect_zoom;
    reflect_arg.z_shift=reflect_z_shift;
    frameP[REFLECT]->args = (u_int)&reflect_arg;
   

    /* set fish_plug paramter */
    set_camera_refraction(&refract_camera); /* get camera pos for refraction-texture */
    refract_fish_arg.camera_pos=&refract_camera.pos;
    refract_fish_arg.camera_zdir=&refract_camera.dir;
    refract_fish_arg.camera_up=&refract_camera.up;
    refract_fish_arg.tex_size=MAP_TEX_SIZE+5; /* add some pixel for safty */
    frameP[REFRACT_FISH_EYE]->args = (u_int)&refract_fish_arg;  
    
    /* set refraction_plug parameter */
    refract_arg.camera_pos=&refract_camera.pos;
    refract_arg.camera_zdir=&refract_camera.dir;
    refract_arg.camera_up=&refract_camera.up;
    refract_arg.refract_index=refract_index;
    refract_arg.zoom=refract_zoom;
    refract_arg.z_shift=refract_z_shift;
    frameP[REFRACT]->args = (u_int)&refract_arg;


    for(i=0;i<NUM_OF_MICRO;i++){ /* PRE for REFOJ already called */
	err = sceHiCallPlug(frameP[i], SCE_HIG_PRE_PROCESS);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    
    /* background */
    err = sceHiGsCtxRegist(sceHiGsStdCtx, 0);
    err = sceHiCallPlug(frameP[ROOM], SCE_HIG_POST_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


    /* reflection*/
    err = sceHiGsCtxRegist(port[TEXTURE_PORT], 1);
    err = sceHiCallPlug(frameP[REFLECT_FISH_EYE], SCE_HIG_POST_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiStopPlugStatus(microP[ROOM]);
    err = sceHiCallPlug(frameP[ROOM], SCE_HIG_POST_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiContPlugStatus(microP[ROOM]);

    err = sceHiGsCtxRegist(port[REFLECT_PORT], 0);
    err = sceHiCallPlug(frameP[REFLECT], SCE_HIG_POST_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiCallPlug(frameP[REFOBJ], SCE_HIG_POST_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    
    /* refraction */
    err = sceHiGsCtxRegist(port[TEXTURE_PORT], 1);
    err = sceHiCallPlug(frameP[REFRACT_FISH_EYE], SCE_HIG_POST_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiStopPlugStatus(microP[ROOM]);
    err = sceHiCallPlug(frameP[ROOM], SCE_HIG_POST_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiContPlugStatus(microP[ROOM]);    

    err = sceHiGsCtxRegist(port[REFRACT_PORT], 0);
    err = sceHiCallPlug(frameP[REFRACT], SCE_HIG_POST_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiCallPlug(frameP[REFOBJ], SCE_HIG_POST_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


    err = sceHiGsCtxRegist(sceHiGsStdCtx, 0);

    utilSetDMATimer();
    err = sceHiDMASend();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiDMAWait();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (gPad.StartSwitch)
	return 1;
    return 0;
}


void SAMPLE21_exit(void)
{
    sceHiErr	err;
    int		i;

    for(i=0;i<NUM;i++){
	err = sceHiCallPlug(frameP[i], SCE_HIG_END_PROCESS);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    for(i=0;i<NUM;i++){
	free(data_addr[i]);
    }

    drawport_exit();
    sceHiGsMemFree(tex_tbl); /* free maping tex*/
}

static void ref_camera_init()
{
    sceHiType	type;
    sceHiPlugHrchyHead_t *hh;	/* pivot head */
    sceHiPlugShapeHead_t *mh;	/* matrix head */
    sceHiPlugShapeMatrix_t *m;	/* matrix data */

    type.repository = SCE_HIP_COMMON;
    type.project = SCE_HIP_FRAMEWORK;
    type.category = SCE_HIP_HRCHY;
    type.status = SCE_HIG_DATA_STATUS;
    type.id = SCE_HIP_PIVOT_DATA;
    type.revision = SCE_HIP_REVISION;
    hh = sceHiPlugHrchyGetHead(hrchyP, type);
    if(hh == NULL) error_handling(__FILE__,__LINE__,"cannot find pivot");
    ref_obj_pivot= sceHiPlugHrchyGetPivot(hh, 0);

    type.repository = SCE_HIP_COMMON;
    type.project = SCE_HIP_FRAMEWORK;
    type.category = SCE_HIP_SHAPE;
    type.status = SCE_HIG_DATA_STATUS;
    type.id = SCE_HIP_BASEMATRIX;
    type.revision = SCE_HIP_REVISION;
    mh = sceHiPlugShapeGetHead(shapeP, type);
    if(mh == NULL) error_handling(__FILE__,__LINE__,"cannot find matrix");
    m = sceHiPlugShapeGetMatrix(mh, 0);
    ref_obj_local_world= &m->local;
}

static void set_camera_reflection(REF_CAMERA *camera)
{
    sceVu0FMATRIX m;
    sceVu0FVECTOR pos,rot,tag,up;
    sceVu0FVECTOR eye;

    viewget(pos, rot, tag, up);

    sceVu0ApplyMatrix(eye, *ref_obj_local_world, *ref_obj_pivot);
    sceVu0UnitMatrix(m);
    sceVu0RotMatrix(m, m, rot);
    sceVu0ApplyMatrix(pos, m, pos);
    sceVu0ApplyMatrix(up, m, up);

    sceVu0SubVector(camera->dir, pos, eye);
    sceVu0CopyVector(camera->up, up);
    sceVu0CopyVector(camera->pos, eye);
}

static void set_camera_refraction(REF_CAMERA *camera)
{
    sceVu0FMATRIX m;
    sceVu0FVECTOR pos,rot,tag,up;
    sceVu0FVECTOR yd={0.0f,1.0f,0.0f,0.0f},zd={0.0f,0.0f,1.0f,0.0f};

    viewget(pos, rot, tag, up);

    sceVu0UnitMatrix(m);
    sceVu0RotMatrix(m, m, rot);
    sceVu0ApplyMatrix(camera->dir, m, zd);
    sceVu0ApplyMatrix(camera->up, m, yd);
    sceVu0ApplyMatrix(camera->pos, m, pos);
}

static void root_ctrl(void)
{
    float  dr = 0.5f / 180.0f * PI;
    float  dt = 0.2f;
 
    if (gPad.LdownOn) {
	root_rot[0]-=dr;
	if(root_rot[0]>PI) root_rot[0] += 2.0f*PI;
    }
    if(gPad.LupOn) {
	root_rot[0]+=dr;
	if(root_rot[0]<-PI) root_rot[0] -= 2.0f*PI;
    }
    if(gPad.LrightOn) {
	root_rot[1]+=dr;
	if(root_rot[1]<-PI) root_rot[1] -= 2.0f*PI;
    }
    if(gPad.LleftOn) {
	root_rot[1]-=dr;
	if(root_rot[1]>PI) root_rot[1] += 2.0f*PI;
    }
    if(gPad.L1On) {
	root_rot[2]+=dr;
	if(root_rot[2]<-PI) root_rot[2] -= 2.0f*PI;
    }
    if(gPad.L2On) {
	root_rot[2]-=dr;
	if(root_rot[2]>PI) root_rot[2] += 2.0f*PI;
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
    draw_debug_print(1, 6, "TORUS ROOT POS : %5.2f %5.2f %5.2f",
		     root_trans[0],
		     root_trans[1],
		     root_trans[2]);
    draw_debug_print(1, 7, "TORUS ROOT ROT : %5.2f %5.2f %5.2f",
		     root_rot[0],
		     root_rot[1],
		     root_rot[2]);
 
}            
