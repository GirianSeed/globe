/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0.2
 */
/*
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: hrchy.c,v 1.19 2003/04/24 07:57:48 xkazama Exp $	*/
#include <eekernel.h>
#include <stdio.h>
#include <math.h>
#include <eeregs.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>
#include "quat.h"

#define PI 	3.14159265f
#define PADDR_MASK 0x0fffffff
#define Paddr(x) ((u_int)(x)&PADDR_MASK)

static const sceHiType hrchyt = { SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_HRCHY, SCE_HIG_DATA_STATUS, SCE_HIP_HRCHY_DATA, SCE_HIP_REVISION };
static const sceHiType pivott = { SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_HRCHY, SCE_HIG_DATA_STATUS, SCE_HIP_PIVOT_DATA, SCE_HIP_REVISION };
static const sceHiType basemt = { SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_SHAPE, SCE_HIG_DATA_STATUS, SCE_HIP_BASEMATRIX, SCE_HIP_REVISION };

/************************************************/
/*		Structure			*/
/************************************************/
typedef	struct	_HRCHY_FRAME{
    sceHiPlugHrchyHead_t	*hrchy_head;
    sceHiPlugShapeHead_t	*basem_head;
    sceHiPlugHrchyHead_t	*pivot_head;
    sceVu0FMATRIX		*root;
} HRCHY_FRAME;

/***************************************************
 * Local Errors
 ***************************************************/
typedef enum {
    _NULL_POINTER,
    _NO_HEAP,
    _PLGBLK_STACK_BROKEN
} _local_err;

static const sceHiErr _local2higerr[] = {
    SCE_HIG_INVALID_VALUE,
    SCE_HIG_NO_HEAP,
    SCE_HIG_INVALID_DATA
};

static const char *_hrchy_err_mes[] = {
    "HiP Hrchy : null pointer\n",
    "HiP Hrchy : can't allocate memory from HiG Heap\n",
    "HiP Hrchy : plug stack is null\n"
};

static sceHiErr _hip_hrchy_err(_local_err err)
{
    sceHiErrStateType *s = sceHiGetErrStatePtr();

    s->mes = _hrchy_err_mes[err];
    return _local2higerr[err];
}

/************************/
/*	Hierarchy	*/
/************************/

enum _order_t{
  TRS =	(1<<0),
  TSR =	(1<<1),
  RTS =	(1<<2),
  RST =	(1<<3),
  STR =	(1<<4),
  SRT =	(1<<5),
  RXYZ = (1<<6),
  RXZY = (1<<7),
  RYXZ = (1<<8),
  RYZX = (1<<9),
  RZXY = (1<<10),
  RZYX = (1<<11)
};

enum _factor_t{
  X=0,Y,Z,XYZ
};

#define RXYZ_M (0x3f<<6)

/* taylor coef. */
        __asm__ ("\n\
        .data\n\
        .align 4\n\
        CONSTANTS:\n\
        .word   0x362e9c14, 0xb94fb21f, 0x3c08873e, 0xbe2aaaa4\n\
        .text\n\
        ");
 
 
#if 0
/* unoptimized version */
static void set_euler_quat(sceVu0FVECTOR ex,sceVu0FVECTOR ey,sceVu0FVECTOR ez,sceVu0FVECTOR angS,sceVu0FVECTOR angC)
{
  __asm__ __volatile__ ("\n\
                la      $8,CONSTANTS  \n\
                lqc2 vf31, 0($8);\n\
                lqc2 vf01, 0(%3); #angle for sin\n\
                lqc2 vf02, 0(%4); #angle for cos\n\
		\n\
		vmove.xyz vf20,vf00 #clear quaternions\n\
                vmove.xyz vf21,vf00\n\
                vmove.xyz vf22,vf00 \n\
 		\n\
		#sin^\n\
                vmul.xyz vf03,vf01,vf01\n\
                vmul.xyz vf04,vf01,vf03 #3\n\
                vmul.xyz vf05,vf04,vf03 #5\n\
                vmul.xyz vf06,vf05,vf03 #7\n\
                vmul.xyz vf07,vf06,vf03 #9\n\
		\n\
		\n\
	        #cos^\n\
                vmul.xyz vf08,vf02,vf02\n\
                vmul.xyz vf09,vf02,vf08 #3\n\
                vmul.xyz vf10,vf09,vf08 #5\n\
                vmul.xyz vf11,vf10,vf08 #7\n\
                vmul.xyz vf12,vf11,vf08 #9   \n\
               	\n\
                vadda.xyz ACC,vf00,vf01\n\
                vmaddaw.xyz ACC,vf04,vf31\n\
                vmaddaz.xyz ACC,vf05,vf31\n\
                vmadday.xyz ACC,vf06,vf31\n\
                vmaddx.xyz vf13,vf07,vf31   \n\
		\n\
                vadda.xyz ACC,vf00,vf02\n\
                vmaddaw.xyz ACC,vf09,vf31\n\
                vmaddaz.xyz ACC,vf10,vf31\n\
                vmadday.xyz ACC,vf11,vf31\n\
                vmaddx.xyz vf14,vf12,vf31 \n\
		\n\
                vmove.x  vf20,vf13\n\
                vmove.y  vf21,vf13\n\
                vmove.z  vf22,vf13\n\
		\n\
                vmulx.w  vf20,vf00,vf14\n\
                vmuly.w  vf21,vf00,vf14\n\
                vmulz.w  vf22,vf00,vf14\n\
		\n\
		\n\
                sqc2 vf20,0(%0); \n\
                sqc2 vf21,0(%1); \n\
                sqc2 vf22,0(%2);   \n\
                "
                :
                : "r" (&ex[0]), "r" (&ey[0]), "r" (&ez[0]), "r" (&angS[0]), "r" (&angC[0])
                : "memory", "$8");               
}
#endif

/* optimized version */
static inline void  set_euler_quat(sceVu0FVECTOR ex,sceVu0FVECTOR ey,sceVu0FVECTOR ez,sceVu0FVECTOR angS,sceVu0FVECTOR angC)
{
    int ret = DI();
    __asm__  volatile("\n\
                la      $8,CONSTANTS\n\
                lqc2 vf31, 0($8);\n\
                lqc2 vf01, 0(%3); #angle for sin\n\
                lqc2 vf02, 0(%4); #angle for cos\n\
 		\n\
 		\n\
                vmul.xyz vf03,vf01,vf01\n\
                vmul.xyz vf08,vf02,vf02\n\
                vadda.xyz ACC,vf00,vf01\n\
                vmove.xyz vf20,vf00 \n\
		\n\
		\n\
                vmul.xyz vf04,vf01,vf03 \n\
                vmul.xyz vf09,vf02,vf08 \n\
                vmove.xyz vf21,vf00 \n\
                vmove.xyz vf22,vf00\n\
		\n\
                vmaddaw.xyz ACC,vf04,vf31\n\
                vmul.xyz vf05,vf04,vf03\n\
                vmul.xyz vf10,vf09,vf08 \n\
		\n\
		\n\
                vmaddaz.xyz ACC,vf05,vf31\n\
                vmul.xyz vf06,vf05,vf03\n\
                vmul.xyz vf11,vf10,vf08\n\
		\n\
                vmadday.xyz ACC,vf06,vf31  \n\
                vmul.xyz vf07,vf06,vf03 \n\
                vmul.xyz vf12,vf11,vf08 \n\
 		\n\
                vmaddx.xyz vf13,vf07,vf31\n\
 		\n\
                vadda.xyz ACC,vf00,vf02\n\
                vmaddaw.xyz ACC,vf09,vf31\n\
                vmaddaz.xyz ACC,vf10,vf31\n\
                vmadday.xyz ACC,vf11,vf31\n\
                vmaddx.xyz vf14,vf12,vf31\n\
 		\n\
                vmove.x  vf20,vf13\n\
                vmove.y  vf21,vf13\n\
                vmove.z  vf22,vf13\n\
 		\n\
                vmulx.w  vf20,vf00,vf14\n\
                vmuly.w  vf21,vf00,vf14\n\
                vmulz.w  vf22,vf00,vf14\n\
 		\n\
                sqc2 vf20,0(%0);\n\
                sqc2 vf21,0(%1);\n\
                sqc2 vf22,0(%2);\n\
                "
                : 
                : "r" (&ex[0]), "r" (&ey[0]), "r" (&ez[0]), "r" (&angS[0]), "r" (&angC[0])
                : "memory", "$8");
    if (ret) EI();
}                 


static inline void set_rot_quat(sceVu0FVECTOR result,sceVu0FVECTOR q,sceVu0FVECTOR r,sceVu0FVECTOR t)  
{
    int ret = DI();
    __asm__ volatile ("\n\
                lqc2 vf01, 0(%1);\n\
                lqc2 vf02, 0(%2);\n\
                lqc2 vf03, 0(%3);\n\
		\n\
                vmul.xyzw vf08, vf01, vf02;  \n\
		\n\
                vopmula.xyz ACC,vf01,vf02;\n\
                vopmsub.xyz vf06,vf02,vf01;\n\
                vmulaw.xyz ACC,vf01,vf02;\n\
                vmaddaw.xyz ACC,vf02,vf01;\n\
                vmaddw.xyz vf07,vf06,vf00;\n\
 		\n\
  		\n\
                vsubaz.w ACC, vf08w, vf08z;\n\
                vmsubay.w ACC, vf00w, vf08y;\n\
                vmsubx.w vf07,vf00w,vf08x;\n\
 		\n\
		\n\
                vmul.xyzw vf08, vf07, vf03;\n\
	       	\n\
                vopmula.xyz ACC,vf07,vf03;\n\
                vopmsub.xyz vf06,vf03,vf07;\n\
                vmulaw.xyz ACC,vf07,vf03;\n\
                vmaddaw.xyz ACC,vf03,vf07;\n\
                vmaddw.xyz vf09,vf06,vf00;\n\
 		\n\
                vsubaz.w ACC, vf08w, vf08z;\n\
                vmsubay.w ACC, vf00w, vf08y;\n\
                vmsubx.w vf09,vf00w,vf08x;     \n\
		\n\
                sqc2 vf09, 0(%0);\n\
                "
                :
                : "r" (&result[0]), "r" (&q[0]), "r" (&r[0]), "r" (&t[0])
                : "memory");
    if (ret) EI();
}              


static inline void set_rot_matrix(sceVu0FMATRIX m,sceVu0FVECTOR rot, u_int order)
{
  sceVu0FVECTOR euler_quat_X,euler_quat_Y,euler_quat_Z,qr;
  sceVu0FVECTOR v,rotsin,rotcos;
  int i,npi;

  /* Normalize hrc->rot angle [-PI..PI] for sin */
  /*  and [0..PI] for cos */
  sceVu0CopyVector(rotsin,rot);
  sceVu0ScaleVectorXYZ(v,rotsin, 1.0f/PI);
  for (i=0; i<3; i++){
    npi=(int)v[i];
    if (npi<0) npi-=1;
    else npi+=1;
    npi/=2;
    if (npi!=0){
       rotsin[i]-= npi*(2.0f*PI);
    }
  }

  {
      int ret = DI();
      __asm__ __volatile__("\n\
      	lqc2   vf01,0(%1)\n\
      	vabs   vf01,vf01\n\
      	sqc2   vf01,0(%0)\n\
	": : "r" (&rotcos[0]),"r" (&rotsin[0]) : "memory" );
      if (ret) EI();
  }
  /* quaterions are used so only angle/2.0f is needed */
  rotsin[X]*=0.5f;rotsin[Y]*=0.5f;rotsin[Z]*=0.5f;
  /* cos(angle)=sin(-angle+pi/2.0f) */
  rotcos[X]=(-rotcos[X]+PI)*0.5f;rotcos[Y]=(-rotcos[Y]+PI)*0.5f;rotcos[Z]=(-rotcos[Z]+PI)*0.5f;


  set_euler_quat(euler_quat_X,euler_quat_Y,euler_quat_Z,rotsin,rotcos);


  switch(order&RXYZ_M){

  case RXYZ:
    set_rot_quat(qr,euler_quat_X,euler_quat_Y,euler_quat_Z);
    break;
  case RXZY:
    set_rot_quat(qr,euler_quat_X,euler_quat_Z,euler_quat_Y);
    break;
  case RYZX:
    set_rot_quat(qr,euler_quat_Y,euler_quat_Z,euler_quat_X);
    break;
  case RZXY:
    set_rot_quat(qr,euler_quat_Z,euler_quat_X,euler_quat_Y);
    break;
  case RZYX:
    set_rot_quat(qr,euler_quat_Z,euler_quat_Y,euler_quat_X);
    break;

  case RYXZ:	/* for LightWave3D and Default */
  default:
    set_rot_quat(qr,euler_quat_Y,euler_quat_X,euler_quat_Z);
    break;
  }

  quat_to_matrix(m,qr);
}

static inline void _nullifyTranspose(sceVu0FMATRIX m0)
{
    int ret = DI();
    __asm__ __volatile__("\n\
        sqc2    vf0,0x30(%0)\n\
        ": : "r" (m0) : "memory");
    if (ret) EI();
}


static inline void set_pivot_matrix(sceVu0FMATRIX pm,sceVu0FMATRIX rm,sceVu0FVECTOR pivot)
{
    int ret = DI();
    __asm__ __volatile__ ("\n\
                lqc2 vf01, 0(%2); #pivot\n\
         	\n\
                vsub  vf08,vf00,vf01 # -pivot\n\
                vmove.w vf10,vf00    # w=1.0\n\
		\n\
                lqc2    vf04,0x0(%1) #rot-matrix\n\
                lqc2    vf05,0x10(%1)\n\
                lqc2    vf06,0x20(%1)\n\
               	\n\
                vmulax ACC, vf04, vf08x; # rot-matrix*(-pivot)\n\
                vmadday ACC, vf05, vf08y;\n\
                vmaddz.xyz vf07, vf06, vf08z;            \n\
		\n\
	        vadd.xyz  vf10,vf07,vf01\n\
		\n\
                sqc2 vf04, 0x00(%0);\n\
                sqc2 vf05, 0x10(%0);\n\
                sqc2 vf06, 0x20(%0);   \n\
                sqc2 vf10, 0x30(%0);\n\
                "
                :
                : "r" (&pm[0]), "r" (&rm[0]), "r" (&pivot[0])
                : "memory");                       
    if (ret) EI();
}

static inline void set_local_world(sceVu0FMATRIX lw,sceVu0FMATRIX m,sceVu0FVECTOR trans,sceVu0FMATRIX rot,sceVu0FVECTOR scale)    
{
    int ret = DI();
    __asm__ __volatile__ ("\n\
                lqc2 vf01, 0(%4); #scale\n\
 		\n\
                lqc2    vf04,0x0(%2) #rot-matrix\n\
                lqc2    vf05,0x10(%2)\n\
                lqc2    vf06,0x20(%2)   \n\
                lqc2    vf07,0x30(%2)\n\
		\n\
		\n\
                lqc2    vf08, 0(%3); #translate \n\
                vmulx.xyz  vf04,vf04,vf01 #scale\n\
                vmuly.xyz  vf05,vf05,vf01	\n\
                vmulz.xyz  vf06,vf06,vf01  	\n\
                vadd.xyz vf07,vf07,vf08  	\n\
		\n\
                lqc2    vf10,0x0(%1) #parent-matrix\n\
                lqc2    vf11,0x10(%1)\n\
                lqc2    vf12,0x20(%1)\n\
                lqc2    vf13,0x30(%1)\n\
		\n\
                vmulax.xyzw     ACC,   vf10,vf04\n\
                vmadday.xyzw    ACC,   vf11,vf04\n\
                vmaddaz.xyzw    ACC,   vf12,vf04\n\
                vmaddw.xyzw     vf20,vf13,vf04    \n\
		\n\
                vmulax.xyzw     ACC,   vf10,vf05\n\
                vmadday.xyzw    ACC,   vf11,vf05\n\
                vmaddaz.xyzw    ACC,   vf12,vf05\n\
                vmaddw.xyzw     vf21,vf13,vf05\n\
		\n\
                vmulax.xyzw     ACC,   vf10,vf06\n\
                vmadday.xyzw    ACC,   vf11,vf06\n\
                vmaddaz.xyzw    ACC,   vf12,vf06\n\
                vmaddw.xyzw     vf22,vf13,vf06\n\
                \n\
                vmulax.xyzw     ACC,   vf10,vf07\n\
                vmadday.xyzw    ACC,   vf11,vf07\n\
                vmaddaz.xyzw    ACC,   vf12,vf07\n\
                vmaddw.xyzw     vf23,vf13,vf07\n\
              	\n\
                sqc2 vf20, 0x00(%0);\n\
                sqc2 vf21, 0x10(%0);\n\
                sqc2 vf22, 0x20(%0);\n\
                sqc2 vf23, 0x30(%0); \n\
		\n\
                \n\
                "
                :
                : "r" (&lw[0]), "r" (&m[0]), "r" (&rot[0]), "r" (&trans[0]),"r" (&scale[0])
                : "memory");
    if (ret) EI();
}                                    

static inline void set_light_rot(sceVu0FMATRIX lr,sceVu0FMATRIX r,sceVu0FMATRIX rm)
{   
    int ret = DI();
    __asm__ __volatile__ ("\n\
 		\n\
                lqc2    vf04,0x0(%2) #rot-matrix\n\
                lqc2    vf05,0x10(%2)\n\
                lqc2    vf06,0x20(%2)\n\
 		\n\
 		\n\
                lqc2    vf10,0x0(%1) #parent-matrix\n\
                lqc2    vf11,0x10(%1)\n\
                lqc2    vf12,0x20(%1)\n\
		\n\
		\n\
                vmulax.xyzw     ACC,   vf10,vf04\n\
                vmadday.xyzw    ACC,   vf11,vf04\n\
                vmaddz.xyzw     vf20,   vf12,vf04\n\
 		\n\
                vmulax.xyzw     ACC,   vf10,vf05\n\
                vmadday.xyzw    ACC,   vf11,vf05\n\
                vmaddz.xyzw     vf21,   vf12,vf05\n\
 		\n\
                vmulax.xyzw     ACC,   vf10,vf06\n\
                vmadday.xyzw    ACC,   vf11,vf06\n\
                vmaddz.xyzw    vf22,   vf12,vf06\n\
		\n\
		\n\
                sqc2 vf20, 0x00(%0);\n\
                sqc2 vf21, 0x10(%0);\n\
                sqc2 vf22, 0x20(%0);\n\
                sqc2 vf00, 0x30(%0);\n\
 		\n\
 		\n\
                "
                :
                : "r" (&lr[0]), "r" (&r[0]), "r" (&rm[0])
                : "memory");   
    if (ret) EI();
}

static void SetHierarchyMatrix(HRCHY_FRAME *hf, int id, sceVu0FMATRIX m, sceVu0FMATRIX r)
{
    sceHiPlugHrchyData_t	*hrchy;
    sceHiPlugShapeMatrix_t	*basem;
    sceVu0FVECTOR		*pivot;
    sceVu0FMATRIX		rm, pm;
    int	i;

    hrchy = sceHiPlugHrchyGetData(hf->hrchy_head, id);
    basem = sceHiPlugShapeGetMatrix(hf->basem_head, id);

    /* rot */
    set_rot_matrix(rm, hrchy->rot, hf->hrchy_head->rorder);

    /* pivot */
    if (hf->pivot_head != NULL) {
	pivot = sceHiPlugHrchyGetPivot(hf->pivot_head, id);
	set_pivot_matrix(pm, rm, *pivot);
    } else
	sceVu0CopyMatrix(pm, rm);

    /* local world */
    set_local_world(basem->local, m, hrchy->trans, pm, hrchy->scale);

    /* light rot */
    set_light_rot(basem->light, r, rm);

    for (i = 0; i < hf->hrchy_head->num; i++) {
	hrchy = sceHiPlugHrchyGetData(hf->hrchy_head, i);
	if ((hrchy->parent != -1) && (hrchy->parent == id)) {
	    SetHierarchyMatrix(hf, i, basem->local, basem->light);
	}
    }
}
static void SetHierarchy(HRCHY_FRAME *hf)
{
    int i;
    sceHiPlugHrchyData_t	*hrchy;
    sceVu0FMATRIX		m, r;

    for (i = 0; i < hf->hrchy_head->num; i++) {
	hrchy = sceHiPlugHrchyGetData(hf->hrchy_head, i);
	if (hrchy->parent == -1) {	/* root */
	    if (hf->root) {
		sceVu0CopyMatrix(m, *hf->root);
		sceVu0CopyMatrix(r, *hf->root);
		_nullifyTranspose(r);
	    } else {
		sceVu0UnitMatrix(m);
		sceVu0UnitMatrix(r);
	    }
	    SetHierarchyMatrix(hf, i, m, r);
	}
    }
}

#define QWSIZE 4
#define NUMIDX 3
#define ORDER 2

/************************************************/
/*		Hierarchy Init			*/
/************************************************/
static sceHiErr HrchyInit(HRCHY_FRAME *hf, sceHiPlug *plug)
{
    hf->hrchy_head = sceHiPlugHrchyGetHead(plug, hrchyt);
    if (hf->hrchy_head == NULL)	return _hip_hrchy_err(_NULL_POINTER);
    hf->pivot_head = sceHiPlugHrchyGetHead(plug, pivott);
    hf->basem_head = sceHiPlugShapeGetHead(plug, basemt);
    if (hf->basem_head == NULL) return _hip_hrchy_err(_NULL_POINTER);

    if (hf->hrchy_head == NULL || hf->basem_head == NULL)
	return _hip_hrchy_err(_NULL_POINTER);

    return SCE_HIG_NO_ERR;
}

/************************************************/
/*		Hierarchy Plug			*/
/************************************************/
sceHiErr sceHiPlugHrchy(sceHiPlug *plug, int process)
{
    sceHiErr	err;
    HRCHY_FRAME	*hrchy_frame;

    switch(process){
      case SCE_HIG_INIT_PROCESS:
	hrchy_frame = (HRCHY_FRAME *)sceHiMemAlign(16, sizeof(HRCHY_FRAME) * 1);
	if(hrchy_frame == NULL) return _hip_hrchy_err(_NO_HEAP);

	err = HrchyInit(hrchy_frame, plug);
	if(err != SCE_HIG_NO_ERR) return err;

	plug->stack = (u_int)hrchy_frame;		/* push stack */
	break;

      case SCE_HIG_PRE_PROCESS:
	hrchy_frame = (HRCHY_FRAME *)plug->stack;	/* pop stack */
	if(hrchy_frame == NULL) return _hip_hrchy_err(_PLGBLK_STACK_BROKEN);
	if (plug->args)
	    hrchy_frame->root=((sceHiPlugHrchyPreCalcArg_t *)(plug->args))->root;
	else 
	    hrchy_frame->root=NULL;
	SetHierarchy(hrchy_frame);

	break;

      case SCE_HIG_POST_PROCESS:
	break;

      case SCE_HIG_END_PROCESS:
	hrchy_frame = (HRCHY_FRAME *)plug->stack;	/* pop stack */
	if(hrchy_frame == NULL) return _hip_hrchy_err(_PLGBLK_STACK_BROKEN);
	sceHiMemFree((u_int *)Paddr(hrchy_frame));
	plug->stack = NULL;
	plug->args = NULL;
	break;

      default:
	break;
    }
    return SCE_HIG_NO_ERR;
}
