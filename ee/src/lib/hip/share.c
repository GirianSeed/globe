/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/*
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: share.c,v 1.8 2001/05/20 14:04:00 aoki Exp $	*/
#include <eekernel.h>
#include <stdio.h>
#include <eeregs.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>

#define PADDR_MASK 0x0fffffff
#define Paddr(x) ((u_int)(x)&PADDR_MASK)

#define QWSIZE 4
/************************************************/
/*		Structure			*/
/************************************************/
typedef struct _Sharedvn{
  int	bv,lv;		/* shared vertex info */
  int	bn,ln;		/* shared normal info */
}Sharedvn;

typedef	struct _Matrix{
    u_int		rorder;		/* not use */
    u_int		reserved0;
    u_int		reserved1;
    int			shape;
    sceVu0FMATRIX	local_matrix;
    sceVu0FMATRIX	light_matrix;
}Matrix;

typedef struct _Share{
  int   srcidx;		/* src index array */
  int	dstidx;		/* dst index array */
  int   geomID;		/* vertex array ptr */
  int	num;		/* num of vertex index */
}Share;

typedef struct _ShareP{
    u_int	*sharedvnP;
    u_int	*srcdstvP;
    u_int	*srcdstnP;
    u_int	*vidxP;
    u_int	*nidxP;
    u_int	*sharevP;
    u_int	*sharenP;
    u_int	*mtxP;
    u_int	*shapeP;
}ShareP;

typedef struct _SHARE_FRAME{
    int nsrcv,ndstv,nsrcn,ndstn;
    sceVu0FVECTOR *srcv,*srcn, *dstv,*dstn;
    int	*vidx,*nidx;
    int	shapev,shapen;
    int	nsharevertex,nsharenormal;
    Share *sharevertex, *sharenormal;
    Sharedvn *sharedvn; int nsharedvn;
    Matrix *mtx; int nmtx;

    sceVu0FVECTOR** pGeomVertex;
    sceVu0FVECTOR** pGeomNormal;
    u_int	pad0,pad1;
}SHARE_FRAME;

/***************************************************
 * Local Errors
 ***************************************************/
typedef enum {
    _CANT_FIND_SHAPEV,
    _NO_HEAP,
    _PLGBLK_STACK_BROKEN,
} _local_err;

static const sceHiErr _local2higerr[] = {
    SCE_HIG_INVALID_DATA,
    SCE_HIG_NO_HEAP,
    SCE_HIG_INVALID_DATA
};

static char *_share_err_mes[] = {
    "HiP Share : can't find shapev data(maybe shape/share data is broken)\n",
    "HiP Share : can't allocate memory from HiG Heap\n",
    "HiP Share : the plugin's stack is broken(maybe didnt do INIT_PROCESSS)\n"
};

/* error handling */
static sceHiErr _hip_share_err(_local_err err)
{
    sceHiErrState.mes = _share_err_mes[err];
    return _local2higerr[err];
}

/************************************************/
/*		Vector				*/
/************************************************/
static void ApplyVector(sceVu0FMATRIX m, sceVu0FVECTOR *src, sceVu0FVECTOR *dst, int begin, int length)
{
  int i;

  for(i=begin;i<begin+length;i++){
    sceVu0ApplyMatrix(dst[i], m, src[i]);
  }
}

static sceHiErr SetVector(SHARE_FRAME *sf)
{
    int i;
    Sharedvn	*svn;
    Matrix	*mtx;

    for(i=0;i<sf->nsharedvn;i++){
	svn = (Sharedvn *)(sf->sharedvn + i);
	mtx = (Matrix *)(sf->mtx + i);
	if(svn->lv > 0){
	    ApplyVector(mtx->local_matrix, sf->srcv, sf->dstv, svn->bv, svn->lv);
	    ApplyVector(mtx->light_matrix, sf->srcn, sf->dstn, svn->bn, svn->ln);
	}
    }

    return SCE_HIG_NO_ERR;
}

/************************************************/
/*		Share				*/
/************************************************/
static void MakeShare(Share *s, sceVu0FVECTOR **dstbase, sceVu0FVECTOR *dst, int *index)
{
  int i;
  int idx;

  for(i=0;i<s->num;i++){
      idx=s->srcidx+i;
      sceVu0CopyVector((dstbase[s->dstidx])[i], dst[(index[idx])]);
  }
}
static sceHiErr SetShare(SHARE_FRAME *sf)
{
    int i;

    /*	Make Vertex	*/
    for(i=0;i<sf->nsharevertex;i++){
	MakeShare(&sf->sharevertex[i], sf->pGeomVertex, sf->dstv, sf->vidx);
    }

    /*	Make Normal	*/
    for(i=0;i<sf->nsharenormal;i++){
	MakeShare(&sf->sharenormal[i], sf->pGeomNormal, sf->dstn, sf->nidx);
    }

    return SCE_HIG_NO_ERR;
}

/************************************************/
/*		Share Init			*/
/************************************************/
static sceHiErr GeomInit(SHARE_FRAME *sf, ShareP *sp)
{
    /*		Find Geometry from Shape		*/
    int i,j,k;
    int num;
    u_int *currR;
    u_int size;
    u_int *p;
    int mateNum;
    int geomNum;
    int geomidx;

    currR = sp->shapeP;
    num = currR[3];
    currR += QWSIZE;
    mateNum = 0;
    for(i=0;i<num;i++){
	size = currR[1];
	mateNum = currR[3];
	if(i == sf->shapev) break;	/* sf->shapev == sf->shapen , := num-1 */
	currR += (size + QWSIZE);
    }
    if(i == num) return _hip_share_err(_CANT_FIND_SHAPEV);

    /* count geomNum */
    geomNum = 0;
    p=currR+QWSIZE;
    for (j=0; j<mateNum; j++){	/* each Material */
	geomNum += p[1];
	p+=12;
	for (k=0; k<geomNum; k++){		/* each geometry */
	    p+=p[1]+4;			/* geomSize + geom-header */
	}
    }

    sf->pGeomVertex = (sceVu0FVECTOR **)sceHiMemAlign(16, sizeof(sceVu0FVECTOR *) * geomNum);
    sf->pGeomNormal = (sceVu0FVECTOR **)sceHiMemAlign(16, sizeof(sceVu0FVECTOR *) * geomNum);
    if((sf->pGeomVertex == NULL) || (sf->pGeomNormal == NULL))
	return _hip_share_err(_NO_HEAP);

    /* set pGeomVertex, pGeomNormal */
    p=currR+4;
    geomidx=0;
    for (j=0; j<mateNum; j++){	/* each Material */
	geomNum=p[1];
	p+=12;				/* skip material header, info */
	for (k=0; k<geomNum; k++){		/* each geometry */
	    sf->pGeomVertex[geomidx]=(sceVu0FVECTOR *)(p+4);
	    sf->pGeomNormal[geomidx]=(sceVu0FVECTOR *)(p+4+(p[3]*4)); /* vertex num= p[3] */
	    geomidx++;
	    p+=p[1]+4;			/* skip geom-header + geomSize */
	}
    }
    return SCE_HIG_NO_ERR;
}

static sceHiErr ShareInit(SHARE_FRAME *sf, ShareP *sp)
{
    sceHiErr	err;

    sf->nsrcv = sp->srcdstvP[3];
    sf->ndstv = sf->nsrcv;
    sf->nsrcn = sp->srcdstnP[3];
    sf->ndstn = sf->nsrcn;
    sf->srcv = (sceVu0FVECTOR *)(sp->srcdstvP + QWSIZE);
    sf->srcn = (sceVu0FVECTOR *)(sp->srcdstnP + QWSIZE);
    sf->dstv = sf->srcv + sf->nsrcv;
    sf->dstn = sf->srcn + sf->nsrcn;

    sf->vidx = (int *)(sp->vidxP + QWSIZE);
    sf->nidx = (int *)(sp->nidxP + QWSIZE);

    sf->shapev = sp->sharevP[2];
    sf->nsharevertex = sp->sharevP[3];
    sf->sharevertex = (Share *)(sp->sharevP + QWSIZE);

    sf->shapen = sp->sharenP[2];
    sf->nsharenormal = sp->sharenP[3];
    sf->sharenormal = (Share *)(sp->sharenP + QWSIZE);

    sf->nsharedvn = sp->sharedvnP[3];
    sf->sharedvn = (Sharedvn *)(sp->sharedvnP + QWSIZE);

    sf->nmtx = sp->mtxP[3];
    sf->mtx = (Matrix *)(sp->mtxP + QWSIZE);

    err = GeomInit(sf, sp);	if(err != SCE_HIG_NO_ERR) return err;

    return SCE_HIG_NO_ERR;
}

/************************************************/
/*		Share Plug			*/
/************************************************/
sceHiErr sceHiPlugShare(sceHiPlug *plug, int process)
{
    sceHiType	type;
    u_int	*data;
    sceHiErr	err;
    ShareP	shareP;
    SHARE_FRAME	*share_frame;

    switch(process){
      case SCE_HIG_INIT_PROCESS:
	type.repository = SCE_HIP_COMMON;
	type.project = SCE_HIP_FRAMEWORK;
	type.category = SCE_HIP_SHARE;
	type.id = SCE_HIP_SHARE_DATA;
	type.revision = SCE_HIP_REVISION;

	err = sceHiGetData(plug, &data, type);	if(err != SCE_HIG_NO_ERR) return err;
	shareP.sharedvnP = data;

	type.id = SCE_HIP_SRCDSTVERTEX;
	err = sceHiGetData(plug, &data, type);	if(err != SCE_HIG_NO_ERR) return err;
	shareP.srcdstvP = data;

	type.id = SCE_HIP_SRCDSTNORMAL;
	err = sceHiGetData(plug, &data, type);	if(err != SCE_HIG_NO_ERR) return err;
	shareP.srcdstnP = data;

	type.id = SCE_HIP_VERTEXINDEX;
	err = sceHiGetData(plug, &data, type);	if(err != SCE_HIG_NO_ERR) return err;
	shareP.vidxP = data;

	type.id = SCE_HIP_NORMALINDEX;
	err = sceHiGetData(plug, &data, type);	if(err != SCE_HIG_NO_ERR) return err;
	shareP.nidxP = data;

	type.id = SCE_HIP_SHAREVERTEX;
	err = sceHiGetData(plug, &data, type);	if(err != SCE_HIG_NO_ERR) return err;
	shareP.sharevP = data;

	type.id = SCE_HIP_SHARENORMAL;
	err = sceHiGetData(plug, &data, type);	if(err != SCE_HIG_NO_ERR) return err;
	shareP.sharenP = data;

	type.category = SCE_HIP_SHAPE;
	type.id = SCE_HIP_BASEMATRIX;
	err = sceHiGetData(plug, &data, type);	if(err != SCE_HIG_NO_ERR) return err;
	shareP.mtxP = data;

	type.category = SCE_HIP_SHAPE;
	type.id = SCE_HIP_SHAPE_DATA;
	err = sceHiGetData(plug, &data, type);	if(err != SCE_HIG_NO_ERR) return err;
	shareP.shapeP = data;

	share_frame = (SHARE_FRAME *)sceHiMemAlign(16, sizeof(SHARE_FRAME) * 1);
	if(share_frame == NULL) return _hip_share_err(_NO_HEAP);

	err = ShareInit(share_frame, &shareP);	if(err != SCE_HIG_NO_ERR) return err;
	plug->stack = (u_int)share_frame;
	break;

      case SCE_HIG_PRE_PROCESS:
	share_frame = (SHARE_FRAME *)plug->stack;
	if(share_frame == NULL) return _hip_share_err(_PLGBLK_STACK_BROKEN);
	err = SetVector(share_frame);		if(err != SCE_HIG_NO_ERR) return err;
	err = SetShare(share_frame);		if(err != SCE_HIG_NO_ERR) return err;
	break;

      case SCE_HIG_POST_PROCESS:
	break;

      case SCE_HIG_END_PROCESS:
	share_frame = (SHARE_FRAME *)plug->stack;
	if(share_frame == NULL) return _hip_share_err(_PLGBLK_STACK_BROKEN);
	sceHiMemFree((u_int *)Paddr(share_frame->pGeomVertex));
	sceHiMemFree((u_int *)Paddr(share_frame->pGeomNormal));
	sceHiMemFree((u_int *)Paddr(share_frame));
	plug->stack = NULL;
	plug->args = NULL;
	break;

      default:
	break;
    }
    return SCE_HIG_NO_ERR;
}
