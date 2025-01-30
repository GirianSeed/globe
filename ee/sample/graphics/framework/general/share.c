/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *               Graphics Framework Sample Program
 *                  
 * 
 *      Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          Name : share.c
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 */
#include <eekernel.h>
#include <malloc.h>
#include "defines.h"
#include "data.h"
#include "frame_t.h"
#include "micro.h"
#include "object.h"
#include "share.h"

#define QWSIZE	4

typedef struct {
    u_int	*sharedvnP;
    u_int	*srcdstvP;
    u_int	*srcdstnP;
    u_int	*vidxP;
    u_int	*nidxP;
    u_int	*sharevP;
    u_int	*sharenP;
    u_int	*mtxP;
    u_int	*shapeP;
} _fwShareP_t;    

static fwShareFrame_t* _fwShareInitData(fwShareFrame_t *, _fwShareP_t *);
static void _fwShareInitGeom(fwShareFrame_t *, _fwShareP_t *);
static void _fwShareSet(fwShareFrame_t *);
static void _fwShareMake(fwShare_t *, int, sceVu0FVECTOR **, sceVu0FVECTOR *, int *);
static void _fwShareSetVector(fwShareFrame_t *);
static void _fwShareApplyVector(sceVu0FMATRIX, sceVu0FVECTOR *, sceVu0FVECTOR *, int, int);

fwShareFrame_t* fwShareInit(fwDataHead_t *data)
{
    fwShareFrame_t	*share_frame;
    _fwShareP_t		shareP;

    shareP.sharedvnP	= data->share_data;
    shareP.srcdstvP	= data->srcdstvertex;
    shareP.srcdstnP	= data->srcdstnormal;
    shareP.vidxP	= data->vertexindex;
    shareP.nidxP       	= data->normalindex;
    shareP.sharevP	= data->sharevertex;
    shareP.sharenP	= data->sharenormal;
    shareP.mtxP		= data->basematrix;
    shareP.shapeP	= data->shape_data;
    share_frame = (fwShareFrame_t *)memalign(16, sizeof(fwShareFrame_t));
    if (share_frame == NULL)
	return NULL;
    share_frame = _fwShareInitData(share_frame, &shareP);
    return share_frame;
}

void fwSharePreCalc(fwShareFrame_t *sf)
{
    _fwShareSetVector(sf);
    _fwShareSet(sf);
}

void fwSharePostCalc(fwShareFrame_t *sf)
{
}

void fwShareFinish(fwShareFrame_t *sf)
{
    free((u_int *)Paddr(sf->pGeomVertex));
    free((u_int *)Paddr(sf->pGeomNormal));
    free((u_int *)Paddr(sf));
}

static fwShareFrame_t* _fwShareInitData(fwShareFrame_t *sf, _fwShareP_t *sp)
{
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
    sf->sharevertex = (fwShare_t *)(sp->sharevP + QWSIZE);

    sf->shapen = sp->sharenP[2];
    sf->nsharenormal = sp->sharenP[3];
    sf->sharenormal = (fwShare_t *)(sp->sharenP + QWSIZE);

    sf->nsharedvn = sp->sharedvnP[3];
    sf->sharedvn = (fwShareDvn_t *)(sp->sharedvnP + QWSIZE);

    sf->nmtx = sp->mtxP[3];
    sf->mtx = (fwHrchyMat_t *)(sp->mtxP + QWSIZE);

    _fwShareInitGeom(sf, sp);

    return sf;
}

static void _fwShareInitGeom(fwShareFrame_t *sf, _fwShareP_t *sp)
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
    if(i == num)
	return;

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

    sf->pGeomVertex = (sceVu0FVECTOR **)memalign(16, sizeof(sceVu0FVECTOR *) * geomNum);
    sf->pGeomNormal = (sceVu0FVECTOR **)memalign(16, sizeof(sceVu0FVECTOR *) * geomNum);
    if((sf->pGeomVertex == NULL) || (sf->pGeomNormal == NULL))
	return;

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
}

static void _fwShareSet(fwShareFrame_t *sf)
{
    int		i;

    /* make vertex */
    for (i = 0; i < sf->nsharevertex; i++) {
	_fwShareMake(&sf->sharevertex[i], i, sf->pGeomVertex, sf->dstv, sf->vidx);
    }

    /* make normal */
    for (i = 0; i < sf->nsharenormal; i++) {
	_fwShareMake(&sf->sharenormal[i], i, sf->pGeomNormal, sf->dstn, sf->nidx);
    }
}

static void _fwShareMake(fwShare_t *s, int id, sceVu0FVECTOR **dstbase, sceVu0FVECTOR *dst, int *index)
{
    int		i, idx;

    for (i = 0; i < s->num; i++) {
	idx = s->srcidx + i;
	sceVu0CopyVector((dstbase[s->dstidx])[i], dst[(index[idx])]);
    }
}

static void _fwShareSetVector(fwShareFrame_t *sf)
{
    int		i;
    fwShareDvn_t	*svn;
    fwHrchyMat_t	*mtx;

    for (i = 0; i < sf->nsharedvn; i++) {
	svn = (fwShareDvn_t *) (sf->sharedvn + i);
	mtx = (fwHrchyMat_t *) (sf->mtx + i);
	if (svn->lv > 0) {
	    _fwShareApplyVector(mtx->local_matrix, sf->srcv, sf->dstv, svn->bv, svn->lv);
	    _fwShareApplyVector(mtx->light_matrix, sf->srcn, sf->dstn, svn->bn, svn->ln);
	}
    }
}

static void _fwShareApplyVector(sceVu0FMATRIX m, sceVu0FVECTOR *src, sceVu0FVECTOR *dst, int begin, int length)
{
    int		i;

    for (i = begin; i < begin + length; i++) {
	sceVu0ApplyMatrix(dst[i], m, src[i]);
    }
}
