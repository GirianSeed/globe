/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4.2
 */
/*
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: clutbump.c,v 1.1 2001/05/20 14:04:00 aoki Exp $	*/
#include <eekernel.h>
#include <stdlib.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>

#define	NTABLE	256

/*******************************************************
 *	ClutBump Structure
 *******************************************************/
typedef struct{
  sceVu0FMATRIX *matrix;
  sceVu0FVECTOR *normal_table;
  u_int         *clut_table;
} ClutGroup;

typedef struct{
  u_int		nclut;
  ClutGroup	*clutgroup;
} ClutBump;

/***************************************************
 * Local Errors
 ***************************************************/
typedef enum {
    _NO_HEAP,
    _CLUTBUMP_DATA_ERR,
    _TEX2D_DATA_ERR,
    _BASEMATRIX_DATA_ERR,
    _NORMAL_DATA_ERR,
    _PLGBLK_STACK_BROKEN
} _local_err;

static const sceHiErr _local2higerr[] = {
    SCE_HIG_NO_HEAP,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
};

static const char *_clutbump_err_mes[] = {
    "HiP Clut-bump : can't allocate memory from HiG Heap\n",
    "HiP Clut-bump : CLUTBUMP_DATA error\n",
    "HiP Clut-bump : TEX2D_DATA error\n",
    "HiP Clut-bump : BASEMATRIX error\n",
    "HiP Clut-bump : NORMAL_DATA error\n",
    "HiP Clut-bump : the plugin stack is null\n"
};

/* error handling */
static sceHiErr _hip_clutbump_err(_local_err err)
{
    sceHiErrState.mes = _clutbump_err_mes[err];
    return _local2higerr[err];
}


/*******************************************************
 *	ClutBump PreCalc
 *******************************************************/
static void calc_clut(ClutBump *clutbump, sceHiPlugClutBumpPreArg_t *arg)
{
    int i,j,k,factor;
    u_char  *pwrite;
    sceVu0FVECTOR local_light;
    sceVu0FMATRIX world_local;
    sceVu0FVECTOR *normal_p;
    float result;
    sceVu0IVECTOR color;
    float alpha;
    float specular;
    sceHiPlugClutBumpPreArg_t  arg_pre = {
	{0.0f,0.0f,1.0f,0.0f},{0.0f,0.5f,0.0f,0.0f}
    };

    if(arg != NULL){
	sceVu0CopyVector(arg_pre.light_dir,arg->light_dir);
	sceVu0CopyVector(arg_pre.shading, arg->shading);
    }

    for (i=0;i<clutbump->nclut;i++) {
	sceVu0TransposeMatrix(world_local,*(clutbump->clutgroup[i].matrix));
	sceVu0ApplyMatrix(local_light,world_local,arg_pre.light_dir);
	sceVu0Normalize(local_light,local_light); 
	sceVu0ScaleVector(local_light,local_light,-1.0);
	pwrite=(u_char *)clutbump->clutgroup[i].clut_table;
	normal_p=(sceVu0FVECTOR *)  clutbump->clutgroup[i].normal_table;
	for (j=0;j<NTABLE;j++){
	    result=sceVu0InnerProduct(local_light,*normal_p);
	    if(result<0) result=0.0;
	    if(arg_pre.shading[2] != 0.0f) {
		specular=result;
		factor=(int)arg_pre.shading[3];
		for(k=0;k<factor;k++){
		    specular=specular*specular;
		}
		//alpha=arg_pre.shading[0]  + arg_pre.shading[1] * result *255 + arg_pre.shading[2] * pow(result,arg_pre.shading[3])*255;
		alpha=arg_pre.shading[0]  + arg_pre.shading[1] * result *255 + arg_pre.shading[2] * specular*255;
	    } else {
		alpha=arg_pre.shading[0]  + arg_pre.shading[1] * result*255;
	    }
	    if (alpha>255) alpha=255;
	    color[3]=(int) alpha;
	    if (color[3]> 255) color[3]=255;
	    if(color[3]<0) color[3]=0;
	    pwrite[3]=(u_char) color[3];
	    pwrite+=4;
	    normal_p++;
	}
    }
}

/*******************************************************
 *	ClutBump Init
 *******************************************************/
#define QWSIZE 4
#define NUMIDX 3

static sceHiErr ClutBumpInit(ClutBump *clutbump, sceHiPlug *plug)
{
    sceHiErr	err;
    sceHiType	type;
    int		i,j;

    u_int	nclut;
    u_int	ntex;
    u_int	nmatrix;
    u_int	nnormal;

    u_int	*clutbumpD;
    u_int	*basematrixD;
    u_int	*tex2dD;
    u_int	*normalD;

    u_int	*pmatrix;
    u_int	*pclut;
    u_int	*ptex;
    u_int	*pnormal;

    type.repository = SCE_HIP_COMMON;
    type.project = SCE_HIP_FRAMEWORK;
    type.category = SCE_HIP_BUMP;
    type.status = SCE_HIG_DATA_STATUS;
    type.id = SCE_HIP_CLUTBUMP_DATA;
    type.revision = SCE_HIP_REVISION;
    err = sceHiGetData(plug, &clutbumpD, type);
    if(err != SCE_HIG_NO_ERR) { return _hip_clutbump_err(_CLUTBUMP_DATA_ERR);}

    nclut=clutbumpD[NUMIDX];
    clutbump->nclut=nclut;
    if (nclut<=0) return _hip_clutbump_err(_CLUTBUMP_DATA_ERR);
    clutbump->clutgroup = (ClutGroup *)sceHiMemAlign(16, sizeof(ClutGroup) * nclut);
    if(clutbump->clutgroup == NULL) { return _hip_clutbump_err(_NO_HEAP);}

    type.id = SCE_HIP_CLUTBUMP_NORMAL;
    err = sceHiGetData(plug, &normalD, type);
    if(err != SCE_HIG_NO_ERR) {return _hip_clutbump_err(_NORMAL_DATA_ERR);};
    nnormal=normalD[NUMIDX];
    if (nnormal<=0) return _hip_clutbump_err(_NORMAL_DATA_ERR);

    type.category = SCE_HIP_SHAPE;
    type.id = SCE_HIP_BASEMATRIX;
    err = sceHiGetData(plug, &basematrixD, type);
    if(err != SCE_HIG_NO_ERR) { return _hip_clutbump_err(_BASEMATRIX_DATA_ERR);};
    nmatrix=basematrixD[NUMIDX];
    if (nmatrix<=0) return _hip_clutbump_err(_BASEMATRIX_DATA_ERR);

    type.category = SCE_HIP_TEX2D;
    type.id = SCE_HIP_TEX2D_DATA;
    err = sceHiGetData(plug, &tex2dD, type);
    if(err != SCE_HIG_NO_ERR) { return _hip_clutbump_err(_TEX2D_DATA_ERR);};
    ntex=tex2dD[NUMIDX];
    if (ntex<=0) return _hip_clutbump_err(_TEX2D_DATA_ERR);

    /* skip data header */
    clutbumpD+=QWSIZE;
    basematrixD+=QWSIZE;
    tex2dD+=QWSIZE;
    normalD+=QWSIZE;

    /* search for light_rot matrix */
    pclut=clutbumpD;
    for (i=0;i<nclut;i++){
	pmatrix=basematrixD;
	for (j=0;j<nmatrix;j++) {  
	    if (*pclut==*(pmatrix+3)) {
		clutbump->clutgroup[i].matrix=(sceVu0FMATRIX *) (pmatrix+4);
		break;
	    }
	    pmatrix+=32;
	}
	pclut+=QWSIZE;
    }

    /* search for clut address */
    pclut=clutbumpD+1;
    for (i=0;i<nclut;i++){    
	ptex=tex2dD;
	if ((*pclut+1)>ntex) return _hip_clutbump_err(_CLUTBUMP_DATA_ERR);
	for (j=0;j<*pclut;j++){
	    ptex+=*(ptex+4)+*(ptex+5)+8;
	}
	ptex+=*(ptex+4)+8;
	clutbump->clutgroup[i].clut_table=(u_int *) ptex;
	pclut+=QWSIZE;
    }
    
    /* search for normal_table */
    pclut=clutbumpD+2;
    for (i=0;i<nclut;i++){
	pnormal=normalD;
	if ((*pclut+1)>nnormal) return _hip_clutbump_err(_CLUTBUMP_DATA_ERR);
	for (j=0;j<*pclut;j++){
	    pnormal+=NTABLE*4;
	}
	clutbump->clutgroup[i].normal_table=(sceVu0FVECTOR *) pnormal;
	pclut+=QWSIZE;
    }

    return SCE_HIG_NO_ERR;
}

/*******************************************************
 *	ClutBump Plug
 *******************************************************/
sceHiErr sceHiPlugClutBump(sceHiPlug *plug, int process)
{
    sceHiErr	err;
    ClutBump	*clutbump;

    switch(process){
      case SCE_HIG_INIT_PROCESS:
	  clutbump = (ClutBump *)sceHiMemAlign(16, sizeof(ClutBump));
	  if(clutbump == NULL) { return _hip_clutbump_err(_NO_HEAP);}
	  clutbump->clutgroup = NULL;
	  err = ClutBumpInit(clutbump, plug);
	  if(err != SCE_HIG_NO_ERR){
	      if(clutbump->clutgroup != NULL) sceHiMemFree((u_int *)((u_int)(clutbump->clutgroup) & 0x0fffffff));
	      sceHiMemFree((u_int *)((u_int)clutbump & 0x0fffffff));
	      return err;
	  }

	  plug->args = NULL;
	  plug->stack = (u_int)clutbump;
	  break;

      case SCE_HIG_PRE_PROCESS:
	  clutbump = (ClutBump *)plug->stack;
	  if(clutbump == NULL) return _hip_clutbump_err(_PLGBLK_STACK_BROKEN);
	  calc_clut(clutbump, (sceHiPlugClutBumpPreArg_t *)plug->args);
	  plug->args = NULL;
	  break;

      case SCE_HIG_POST_PROCESS:
	  break;

      case SCE_HIG_END_PROCESS:
	  clutbump = (ClutBump *)plug->stack;
	  if(clutbump == NULL) return _hip_clutbump_err(_PLGBLK_STACK_BROKEN);
	  sceHiMemFree((u_int *)((u_int)clutbump & 0x0fffffff));
	  plug->stack = NULL;
	  plug->args = NULL;
	  break;

      default:
	  break;
    }

    return SCE_HIG_NO_ERR;
}
