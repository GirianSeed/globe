/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4.2
 */
/*
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: hrchy.c,v 1.8 2001/09/21 04:34:27 kaneko Exp $	*/
#include <eekernel.h>
#include <stdio.h>
#include <eeregs.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>\

#define PI 	3.14159265f
#define PADDR_MASK 0x0fffffff
#define Paddr(x) ((u_int)(x)&PADDR_MASK)

/************************************************/
/*		Structure			*/
/************************************************/
typedef struct	_Hierarchy{
    sceVu0FVECTOR	trans;
    sceVu0FVECTOR	rot;
    sceVu0FVECTOR	scale;
    int			shape;		/* not use */
    int			parent;
    int			child;
    int			sibling;
}Hierarchy;

typedef	struct _Matrix{
    u_int		rorder;		/* not use */
    u_int		reserved0;
    u_int		reserved1;
    int			shape;
    sceVu0FMATRIX	local_matrix;
    sceVu0FMATRIX	light_matrix;
}Matrix;

typedef	struct	_HRCHY_FRAME{
  int		num;
  Hierarchy	*hrc;
  Matrix	*mat;
  sceVu0FVECTOR	*pvt;
  sceVu0FMATRIX	*root;
  u_int		order;	/* rotation order */
}HRCHY_FRAME;

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
    "HiP Hrchy : the plugin's stack is broken(maybe didnt do INIT_PROCESS)\n",
};

static sceHiErr _hip_hrchy_err(_local_err err)
{
    sceHiErrState.mes = _hrchy_err_mes[err];
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

static void SetRotMatrix(sceVu0FMATRIX m, sceVu0FVECTOR r, u_int order)
{
  switch(order&RXYZ_M){
  case RXYZ:
    sceVu0RotMatrix(m, m, r);
    break;
  case RXZY:
    sceVu0RotMatrixY(m, m, r[Y]);
    sceVu0RotMatrixZ(m, m, r[Z]);
    sceVu0RotMatrixX(m, m, r[X]);
    break;
  case RYZX:
    sceVu0RotMatrixX(m, m, r[X]);
    sceVu0RotMatrixZ(m, m, r[Z]);
    sceVu0RotMatrixY(m, m, r[Y]);
    break;
  case RZXY:
    sceVu0RotMatrixY(m, m, r[Y]);
    sceVu0RotMatrixX(m, m, r[X]);
    sceVu0RotMatrixZ(m, m, r[Z]);
    break;
  case RZYX:
    sceVu0RotMatrixX(m, m, r[X]);
    sceVu0RotMatrixY(m, m, r[Y]);
    sceVu0RotMatrixZ(m, m, r[Z]);
    break;

  case RYXZ:	/* for LightWave3D and Default */
  default:
    sceVu0RotMatrixZ(m, m, r[Z]);
    sceVu0RotMatrixX(m, m, r[X]);
    sceVu0RotMatrixY(m, m, r[Y]);
    break;
  }
}

static void _nullifyTranspose(sceVu0FMATRIX m0)
{
    asm __volatile__("
        sqc2    vf0,0x30(%0)
        ": : "r" (m0) );
}

static void SetHierarchyMatrix(HRCHY_FRAME *hf, int id, sceVu0FMATRIX m, sceVu0FMATRIX r)
{
    int i;
    sceVu0FMATRIX m2;
    sceVu0FMATRIX m0;
    Hierarchy	*hrc;
    Matrix	*mat;
    sceVu0FVECTOR rot;

    hrc = (Hierarchy *)(hf->hrc + id);
    mat = (Matrix *)(hf->mat + id);

    /* Normalize hrc->rot angle [-PI..PI] */
    {
	sceVu0FVECTOR v;
	int npi;

	sceVu0CopyVector(rot, hrc->rot);
	sceVu0ScaleVectorXYZ(v,rot, 1.0f/PI);
	for (i=0; i<3; i++){
	  
	    npi=(int)v[i];
	    if (npi<0) npi-=1;
	    else npi+=1;
	    npi/=2;
	    if (npi!=0){
		rot[i]-= npi*(2.0f*PI);
	    }
	}
    }
    {
	sceVu0FMATRIX tm,rm,sm,pm;
	sceVu0FVECTOR pv;

	/*	Scale	*/
	sceVu0UnitMatrix(sm);
	sm[0][0] = hrc->scale[X];
	sm[1][1] = hrc->scale[Y];
	sm[2][2] = hrc->scale[Z];

	/*	Rot	*/
	sceVu0UnitMatrix(rm);
	SetRotMatrix(rm, rot, hf->order);

	/*	Pivot	*/
	sceVu0CopyMatrix(pm, rm);
	if(hf->pvt != NULL){
	    sceVu0UnitMatrix(tm);
	    sceVu0ScaleVector(pv, hf->pvt[id], -1.0f);
	    sceVu0TransMatrix(tm, tm, pv);
	    sceVu0MulMatrix(pm, pm, tm);
	    sceVu0UnitMatrix(tm);
	    sceVu0CopyVector(pv, hf->pvt[id]);
	    sceVu0TransMatrix(tm, tm, pv);
	    sceVu0MulMatrix(pm, tm, pm);
	}

	/*	Trans	*/
	sceVu0UnitMatrix(tm);
	sceVu0TransMatrix(tm, tm, hrc->trans);

	/*	LocalWorld	*/
	sceVu0UnitMatrix(m0);
	sceVu0MulMatrix(m0, sm, m0);
	sceVu0MulMatrix(m0, pm, m0);
	sceVu0MulMatrix(m0, tm, m0);
	sceVu0MulMatrix(m0,  m, m0);

	/*	LightRot	*/
	sceVu0UnitMatrix(m2);
	sceVu0MulMatrix(m2, r, rm);
    }

    sceVu0CopyMatrix(mat->local_matrix, m0);
    sceVu0CopyMatrix(mat->light_matrix, m2);

    for(i=0;i<hf->num;i++){
	hrc = (Hierarchy *)(hf->hrc + i);
	if((hrc->parent != -1)&&(hrc->parent == id)){
	    SetHierarchyMatrix(hf, i, m0, m2);
	}
    }
}
static void SetHierarchy(HRCHY_FRAME *hf)
{
    int i;
    Hierarchy *hrc;
    sceVu0FMATRIX m,r;

    for(i=0;i<hf->num;i++){
	hrc = (Hierarchy *)(hf->hrc + i);
	if(hrc->parent == -1){			/* Root */
	    if(hf->root){
		sceVu0CopyMatrix(m, *hf->root);
		sceVu0CopyMatrix(r, *hf->root);
		_nullifyTranspose(r);		/* rm trans */
	    }
	    else{
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
static sceHiErr HrchyInit(HRCHY_FRAME *hf, u_int *hrcP, u_int *matP, u_int *pvtP)
{
    if((hrcP == NULL) || (matP == NULL))	return _hip_hrchy_err(_NULL_POINTER);
/*    if(hrcP[3] != matP[3])	return SCE_HIG_NO_DATA;*/

    hf->order = hrcP[ORDER];
    hf->num = hrcP[NUMIDX];

    hf->hrc = (Hierarchy *)(hrcP+QWSIZE);
    hf->mat = (Matrix *)(matP+QWSIZE);
    if(pvtP != NULL){
      hf->pvt = (sceVu0FVECTOR *)(pvtP+QWSIZE);
    }
    else{
      hf->pvt = NULL;
    }
    return SCE_HIG_NO_ERR;
}

/************************************************/
/*		Hierarchy Plug			*/
/************************************************/
sceHiErr sceHiPlugHrchy(sceHiPlug *plug, int process)
{
    sceHiType	type;
    u_int	*data;
    sceHiErr	err;
    HRCHY_FRAME	*hrchy_frame;

    u_int	*hrcP;
    u_int	*matP;
    u_int	*pvtP;

    switch(process){
      case SCE_HIG_INIT_PROCESS:
	type.repository = SCE_HIP_COMMON;
	type.project = SCE_HIP_FRAMEWORK;
	type.category = SCE_HIP_HRCHY;
	type.id = SCE_HIP_HRCHY_DATA;
	type.status = SCE_HIG_DATA_STATUS;
	type.revision = SCE_HIP_REVISION;

	err = sceHiGetData(plug, &data, type);	if(err != SCE_HIG_NO_ERR) return err;
	hrcP = data;

	type.category = SCE_HIP_SHAPE;
	type.id = SCE_HIP_BASEMATRIX;
	err = sceHiGetData(plug, &data, type);	if(err != SCE_HIG_NO_ERR) return err;
	matP = data;

	type.category = SCE_HIP_HRCHY;
	type.id = SCE_HIP_PIVOT_DATA;
	err = sceHiGetData(plug, &data, type);
	if(err == SCE_HIG_NO_ERR)	pvtP = data;
	else			pvtP = NULL;

	hrchy_frame = (HRCHY_FRAME *)sceHiMemAlign(16, sizeof(HRCHY_FRAME) * 1);
	if(hrchy_frame == NULL) return _hip_hrchy_err(_NO_HEAP);

	err = HrchyInit(hrchy_frame, hrcP, matP, pvtP);		if(err != SCE_HIG_NO_ERR) return err;

	plug->stack = (u_int)hrchy_frame;		/* push stack */
	break;

      case SCE_HIG_PRE_PROCESS:
	hrchy_frame = (HRCHY_FRAME *)plug->stack;	/* pop stack */
	if(hrchy_frame == NULL) return _hip_hrchy_err(_PLGBLK_STACK_BROKEN);
	if (plug->args) hrchy_frame->root=((sceHiPlugHrchyPreCalcArg_t *)(plug->args))->root;
	else hrchy_frame->root=NULL;
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
