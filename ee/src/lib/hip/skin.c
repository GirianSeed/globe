/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/*
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: skin.c,v 1.1 2002/05/20 02:45:48 kaneko Exp $	*/
#include <eekernel.h>
#include <stdlib.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>

/********************************************************/
/*		Local Errors				*/
/********************************************************/
typedef enum {
    _NO_HEAP,
    _NO_SKIN_LB,
    _NO_SKIN_LW,
    _NO_SKIN_BW,
    _NO_MATRIX,
    _NO_SHAPE,
    _NO_STACK,
    _ILLEGAL_SHAPE_FORMAT,
    _INVALID_SKIN_FUNCTION,
    _NOT_IMPLEMENTED_YET
} _local_err;

static const sceHiErr _local2hierr[] = {
    SCE_HIG_NO_HEAP,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_DATA
};

static const char *_err_mes[] = {
    "HiP Skin : can't allocate memory\n",
    "HiP Skin : can't find SKIN_LB\n",
    "HiP Skin : can't find SKIN_LW\n",
    "HiP Skin : can't find SKIN_BW\n",
    "HiP Skin : can't find BASEMATRIX\n",
    "HiP Skin : can't find SHAPE_DATA\n",
    "HiP Skin : plug stack is null\n",
    "HiP Skin : illegal shape data format\n",
    "HiP Skin : illegal skin function\n",
    "HiP Skin : illegal skin function\n"
};

/* error handling */
static sceHiErr _hip_err(_local_err err)
{
    sceHiErrState.mes = _err_mes[err];
    return _local2hierr[err];
}

enum {LB,LW,BW,DATA,MAXSKINH};
enum {BM,MAXSHAPEH};

/********************************************************/
/*		Structure				*/
/********************************************************/
typedef struct {
    sceHiPlugSkinHead_t		*skinh[MAXSKINH];	/* skin header LB,LW,BW,DATA */
    sceHiPlugShapeHead_t	*shapeh[MAXSHAPEH];	/* shape header BM */
    u_int			padding[3];
}Skin_t;


/***********************************************
 *	vu0 skin
 ***********************************************/
enum {
    VN_WITH_NORMALIZE=0,
    VN_WITHOUT_NORMALIZE=1,
    VN_NORMAL_FIRST_MATRIX=2,
    VONLY=3
};
enum {
    RT_ONLY_MATRIX,
    REGULAR34_MATRIX,
    NO_INVERSE

};

/* vout = m0*vin*w0 + m1*vin*w1 + m2*vin*w2 + m3*vin*w3 */
static void Vu0Weight(u_int *vout, sceVu0FVECTOR vin, 
		sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FMATRIX m2, sceVu0FMATRIX m3, 
		sceVu0FVECTOR w)
{
    /*
      MAT00 vf01 %3, %4, %5, %6
      MAT01 vf02
      MAT02 vf03
      MAT03 vf04

      VIN  vf27 %1
      W   vf28 %2

      MV0 vf10 (m0*v)
      MV1 vf11 (m1*v)
      MV2 vf12 (m2*v)
      MV3 vf13 (m3*v)

      VOUT  vf31 %0
    */
    __asm__  __volatile__("
	vmove.w vf31, vf00			# Vout.w=1.0f
	lqc2    vf28,0x0(%2)			# load weight
	lqc2    vf27,0x0(%1)			# load vin
	vmove.w vf27, vf00			# Vin.w=1.0f
	lqc2    vf01,0x0(%3)			# load m0
	lqc2    vf02,0x10(%3)
	lqc2    vf03,0x20(%3)
	lqc2    vf04,0x30(%3)
	vmulax.xyz	ACC, vf01, vf27		# mv0= m0*vin
	vmadday.xyz	ACC, vf02, vf27
	vmaddaz.xyz	ACC, vf03, vf27
	vmaddw.xyz	vf10, vf04, vf27

	lqc2    vf01,0x0(%4)			# load m1
	lqc2    vf02,0x10(%4)
	lqc2    vf03,0x20(%4)
	lqc2    vf04,0x30(%4)
	vmulax.xyz	ACC, vf01, vf27		# mv1= m1*vin
	vmadday.xyz	ACC, vf02, vf27
	vmaddaz.xyz	ACC, vf03, vf27
	vmaddw.xyz	vf11, vf04, vf27

	lqc2    vf01,0x0(%5)			# load m2
	lqc2    vf02,0x10(%5)
	lqc2    vf03,0x20(%5)
	lqc2    vf04,0x30(%5)
	vmulax.xyz	ACC, vf01, vf27		# mv2= m2*vin
	vmadday.xyz	ACC, vf02, vf27
	vmaddaz.xyz	ACC, vf03, vf27
	vmaddw.xyz	vf12, vf04, vf27

	lqc2    vf01,0x0(%6)			# load m3
	lqc2    vf02,0x10(%6)
	lqc2    vf03,0x20(%6)
	lqc2    vf04,0x30(%6)
	vmulax.xyz	ACC, vf01, vf27		# mv3= m3*vin
	vmadday.xyz	ACC, vf02, vf27
	vmaddaz.xyz	ACC, vf03, vf27
	vmaddw.xyz	vf13, vf04, vf27
	
	vmulax.xyz	ACC, vf10, vf28		# vout= mv0*w0
	vmadday.xyz	ACC, vf11, vf28		# + mv1*w1
	vmaddaz.xyz	ACC, vf12, vf28		# + mv2*w2
	vmaddw.xyz	vf31, vf13, vf28	# + mv3*w3
	
	sqc2    vf31,0x0(%0)			# store vout
	": : "r" (vout) , "r" (vin), "r" (w) ,
			  "r" (m0), "r" (m1), "r" (m2), "r" (m3) : "memory");
}


static inline void applyMatrix(u_int *v, sceVu0FMATRIX m0 )
{
    __asm__  __volatile__("
	lqc2    vf5,0x0(%0)			# load v
	lqc2    vf01,0x0(%1)			# load m0
	lqc2    vf02,0x10(%1)
	lqc2    vf03,0x20(%1)
	lqc2    vf04,0x30(%1)
	vmulax.xyz	ACC, vf01, vf5		# vf5= m0*v
	vmadday.xyz	ACC, vf02, vf5
	vmaddaz.xyz	ACC, vf03, vf5
	vmaddw.xyz	vf5, vf04, vf5
	sqc2    vf5,0x0(%0)			# store v
	": : "r" (v) , "r" (m0) : "memory");
}


static inline void applyMatrixNormal(u_int *v, sceVu0FMATRIX m0 )
{
    __asm__  __volatile__("
	lqc2    vf5,0x0(%0)			# load v
	lqc2    vf01,0x0(%1)			# load m0
	lqc2    vf02,0x10(%1)
	lqc2    vf03,0x20(%1)
	vmulax.xyz	ACC, vf01, vf5		# vf5= m0*v
	vmadday.xyz	ACC, vf02, vf5
	vmaddz.xyz	vf5, vf03, vf5
	sqc2    vf5,0x0(%0)			# store v
	": : "r" (v) , "r" (m0) : "memory");
}

/***********************************************
    void Vu0InverseMatrixRT(sceVu0FMATRIX m0, sceVu0FMATRIX m1)

	以下の形式の matrix のみ扱える inverse matrix
	(3x3 部分は、回転行列でなければならない)

	
	{{ ra,rb,rc, 0 },
	 { rd,re,rf, 0 },
	 { rg,rh,ri, 0 },
	 { t0,t1,t2, 1 }};

	m1: 4x4 input matrix (vf06-09)
	mr: 3x3 (rotation) part (ra-ri), (vf06-08)
	T: translation part of m1 (t0-t2) (vf09)
	mr-save  (vf10-12): 
	-T: (vf13)
	m0: 4x4 output matrix (vf06-09)
 ***********************************************/

static void Vu0InverseMatrixRT(sceVu0FMATRIX m0, sceVu0FMATRIX m1)
{

    __asm__  __volatile__("
	# load m1 (vf06-09)
	lqc2    vf06,0x0(%1)
	lqc2    vf07,0x10(%1)
	lqc2    vf08,0x20(%1)
	lqc2    vf09,0x30(%1)

	# save mr (vf10-12)
	vmove vf10, vf06
	vmove vf11, vf07
	vmove vf12, vf08

	# calc -T (vf13)
	vsub.xyz vf13,vf00,vf09

	# transpose mr (using saved regs: vf10-vf12)
	vaddx.y vf06,vf00,vf11
	vaddx.z vf06,vf00,vf12
	vaddy.x vf07,vf00,vf10
	vaddy.z vf07,vf00,vf12
	vaddz.x vf08,vf00,vf10
	vaddz.y vf08,vf00,vf11

	# -T * mo (vf09)
	vmulax.xyz  ACC,vf06,vf13
	vmadday.xyz  ACC,vf07,vf13
	vmaddz.xyz  vf09,vf08,vf13

	# store m0
	sqc2    vf06,0x0(%0)
	sqc2    vf07,0x10(%0)
	sqc2    vf08,0x20(%0)
	sqc2    vf09,0x30(%0)

	": : "r" (m0) , "r" (m1) : "memory");
}


/***********************************************

    void Vu0InverseMatrix34(sceVu0FMATRIX m0, sceVu0FMATRIX m1)

	以下の形式の matrix のみ扱える inverse matrix
	(ただし 3x3 部分は、正則でさえあれば回転行列でなくても構わない)

	{{  a, b, c, 0 },
	 {  d, e, f, 0 },
	 {  g, h, i, 0 },
	 { t0,t1,t2, 1 }};

	m1: 4x4 input matrix (vf01-04)
	mi: 3x3 part of the input matrix  (a-i), (vf01-03)
	T: t0-t2, (vf04)
	mtmp: {{ei-fh, ch-bi, ...}, {}, {}} (vf06-08)  (save area vf10-12): 
	|mi|: det. of mi (vf05)
	-T: (vf13)
	mo: 3x3 part of the output matrix (vf06-08)
	m0: 4x4 output matrix (vf06-09)

 ***********************************************/

static void Vu0InverseMatrix34(sceVu0FMATRIX m0, sceVu0FMATRIX m1)
{

    __asm__  __volatile__("
	# load m1
	lqc2    vf01,0x0(%1)
	lqc2    vf02,0x10(%1)
	lqc2    vf03,0x20(%1)
	lqc2    vf04,0x30(%1)

	# mtmp: vf06-vf08
	vopmula.xyz ACC, vf01,vf02
	vopmsub.xyz vf08, vf02,vf01

	vopmula.xyz ACC, vf02,vf03
	vopmsub.xyz vf06, vf03,vf02

	vopmula.xyz ACC, vf03,vf01
	vopmsub.xyz vf07, vf01,vf03

	# calc |mi|
	vmul.xyz  vf05, vf03, vf08

		# set w field
		vsub.w vf06,vf00,vf00
		vsub.w vf07,vf00,vf00
		vsub.w vf08,vf00,vf00
		vmove.w vf09,vf00

	vadday.x  ACC, vf05, vf05
	vmaddz.x  vf05, vf29, vf05	# vf05.x= |mi|,    vf29=(1,1,1,1)

		# save mtmp
		vmove vf10, vf06
		vmove vf11, vf07
		vmove vf12, vf08

	# calc 1/|mi|
	vdiv	Q, vf00w, vf05x

	# calc -T (vf13)
	vsub.xyz vf13,vf00,vf04

		# transpose mtmp 3x3 part (using saved regs: vf10-vf12)
		vaddy.x vf07,vf00,vf10
		vaddx.y vf06,vf00,vf11
		
		vaddz.x vf08,vf00,vf10
		vaddx.z vf06,vf00,vf12
		
		vaddz.y vf08,vf00,vf11
		vaddy.z vf07,vf00,vf12

	vwaitq

	#  mo =  mtmp/|mi|
	vmulq.xyzw vf06,vf06,Q
	vmulq.xyzw vf07,vf07,Q
	vmulq.xyzw vf08,vf08,Q

	# -T * mo (vf09)
	vmulax.xyz  ACC,vf06,vf13
	vmadday.xyz  ACC,vf07,vf13
	vmaddz.xyz  vf09,vf08,vf13

	# store m0
	sqc2    vf06,0x0(%0)
	sqc2    vf07,0x10(%0)
	sqc2    vf08,0x20(%0)
	sqc2    vf09,0x30(%0)


	": : "r" (m0) , "r" (m1) : "memory");
}

static void Vu0SetInit(void)
{
    /* vf29 = (1, 1, 1, 1)  */
    __asm__  __volatile__("
	vmaxw.xyzw vf29, vf00, vf00			# VF29
	");
}

static void Vu0WeightNormal(u_int *vout, sceVu0FVECTOR vin, 
		sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FMATRIX m2, sceVu0FMATRIX m3, 
		sceVu0FVECTOR w, int normalize)
{
    /*
      MAT00 vf01 %3, %4, %5, %6
      MAT01 vf02
      MAT02 vf03
      MAT03 vf04

      VIN  vf27 %1
      W   vf28 %2

      MV0 vf10 (m0*v)
      MV1 vf11 (m1*v)
      MV2 vf12 (m2*v)
      MV3 vf13 (m3*v)

      tmp normal  vf28 
      length(normal) vf30 
      VOUT  vf31 %0
    */
    __asm__  __volatile__("
	lqc2    vf28,0x0(%2)			# load weight
	lqc2    vf27,0x0(%1)			# load vin
	vmove.w vf27, vf00			# Vin.w=1.0f
	lqc2    vf01,0x0(%3)			# load m0
	lqc2    vf02,0x10(%3)
	lqc2    vf03,0x20(%3)
	lqc2    vf04,0x30(%3)
	vmulax.xyz	ACC, vf01, vf27		# mv0= m0*vin
	vmadday.xyz	ACC, vf02, vf27
	vmaddz.xyz	vf10, vf03, vf27


	lqc2    vf01,0x0(%4)			# load m1
	lqc2    vf02,0x10(%4)
	lqc2    vf03,0x20(%4)
	lqc2    vf04,0x30(%4)
	vmulax.xyz	ACC, vf01, vf27		# mv1= m1*vin
	vmadday.xyz	ACC, vf02, vf27
	vmaddz.xyz	vf11, vf03, vf27

	lqc2    vf01,0x0(%5)			# load m2
	lqc2    vf02,0x10(%5)
	lqc2    vf03,0x20(%5)
	lqc2    vf04,0x30(%5)
	vmulax.xyz	ACC, vf01, vf27		# mv2= m2*vin
	vmadday.xyz	ACC, vf02, vf27
	vmaddz.xyz	vf12, vf03, vf27

	lqc2    vf01,0x0(%6)			# load m3
	lqc2    vf02,0x10(%6)
	lqc2    vf03,0x20(%6)
	lqc2    vf04,0x30(%6)
	vmulax.xyz	ACC, vf01, vf27		# mv3= m3*vin
	vmadday.xyz	ACC, vf02, vf27
	vmaddz.xyz	vf13, vf03, vf27
	
	vmulax.xyz	ACC, vf10, vf28		# normal(vf30)= mv0*w0
	vmadday.xyz	ACC, vf11, vf28		# + mv1*w1
	vmaddaz.xyz	ACC, vf12, vf28		# + mv2*w2
	vmaddw.xyz	vf30, vf13, vf28	# + mv3*w3
	": : "r" (vout) , "r" (vin), "r" (w) ,
			  "r" (m0), "r" (m1), "r" (m2), "r" (m3) : "memory");
    if (normalize){
	__asm__  __volatile__("
	
# normalize
	vmul.xyz	vf28, vf30, vf30	# vf28= vf30^2
	vadday.x	ACC, vf28, vf28		# ACC.x=vf28.y+vf28.x
	vmaddz.x	vf28, vf29, vf28	# vf28.x=ACC.x+(1.0*vf28.z)  (vf29=(1,1,1,1)

	vrsqrt 		Q, vf00w, vf28x		# Q= 1/vf28.x

	vsub.w vf31, vf00, vf31			# Vout.w=0.0f

	vwaitq

	vmulq.xyz	vf31, vf30, Q		# vout(vf31)= vf30 * Q
	sqc2    vf31,0x0(%0)			# store vout
	": : "r" (vout) , "r" (vin), "r" (w) ,
			  "r" (m0), "r" (m1), "r" (m2), "r" (m3) : "memory");
    } else{
	__asm__  __volatile__("
	vsub.w vf30, vf00, vf30			# Vout.w=0.0f
	sqc2    vf30,0x0(%0)			# store vout
	": : "r" (vout) , "r" (vin), "r" (w) ,
			  "r" (m0), "r" (m1), "r" (m2), "r" (m3) : "memory");
    }
}


static sceHiPlugShapeHead_t *getgeomptr(sceHiPlugShapeHead_t *th, int *id)
{
    sceHiPlugShapeHead_t *dh, *mh, *gh;

    if (id[0]>=th->top.num) 
	return NULL;
    dh = sceHiPlugShapeGetDataHead(th, id[0]);
    if (dh==NULL) 
	return NULL;

    if (id[1]>=dh->dat.num) 
	return NULL;
    mh = sceHiPlugShapeGetMaterialHead(dh, id[1]);
    if (mh==NULL) 
	return NULL;

    if (id[2]>=mh->mat.num) 
	return NULL;
    gh = sceHiPlugShapeGetGeometryHead(mh, id[2]);
    if (gh==NULL) 
	return NULL;

    return gh;

}




static sceVu0FMATRIX *searchbasemat(sceHiPlugShapeHead_t *bmdb, int shapeid)
{
    sceHiPlugShapeMatrix_t *bm= (sceHiPlugShapeMatrix_t *)(bmdb +1);
    int i;

    for (i=0; i<bmdb->top.num; i++){
	if (bm->shape==shapeid) return &bm->local;
	bm++;
    }
    return NULL;
}


/********************************************************/
/*		Skin Calc				*/
/********************************************************/
static sceHiErr skin_calc(Skin_t *s)
{
    sceVu0FMATRIX ibm;				/* inverse base matrix  */
    sceVu0FMATRIX *m[3];		/* LB,LW,BW matrix */
    int num;				/* num of skin unit */
    int *id;				/* BW matrix id from BASEMATRIX */
    int i;
    int newshapeid, oldshapeid;
    sceVu0FMATRIX *bm;
    int invertm, rt_only, regular34;


    m[LB] = (sceVu0FMATRIX *)(s->skinh[LB]+1);
    m[LW] = (sceVu0FMATRIX *)(s->skinh[LW]+1);
    id = (int *)(s->skinh[BW]+1);
    num = s->skinh[LW]->num;
    for(i=0;i<num;i++){

	/* get BW matrix */
	m[BW] = (sceVu0FMATRIX *)(sceHiPlugShapeGetMatrix(s->shapeh[BM], *id)->local);

	/* LW = LB * BW */
	sceVu0MulMatrix(*m[LW], *m[BW], *m[LB]);

	m[LB]++;	/* next LB matrix */
	m[LW]++;	/* next LW matrix */
	id++;		/* next BW matrix id */
    }

    if (s->skinh[DATA]){				/* vu0 skin  */
	sceHiPlugSkinData_t	*dat;
	sceVu0FMATRIX *mtx;
	union {
	    float* fp;
	    u_int **fvpp;
	} vaddr;
	int normalize;
	int calnormal;

	switch ((s->skinh[DATA]->func)&0xffff){
	case VN_WITH_NORMALIZE:
	    normalize=1; 
	    calnormal=1; 
	    break;
	case VN_WITHOUT_NORMALIZE: 
	    normalize=0; 
	    calnormal=1; 
	    break;
	case VN_NORMAL_FIRST_MATRIX:
	    normalize=0; 
	    calnormal=1; 
	    return _hip_err(_NOT_IMPLEMENTED_YET);
	    break;
	case VONLY:
	    normalize=0; 
	    calnormal=0; 
	    break;
	default:
	    return _hip_err(_INVALID_SKIN_FUNCTION);
	}

	switch ((s->skinh[DATA]->func)>>16){
	case RT_ONLY_MATRIX:
	    rt_only=1;
	    regular34=1;
	    invertm=1;
	    break;
	case REGULAR34_MATRIX:
	    rt_only=0;
	    regular34=1;
	    invertm=1;
	    break;
	case NO_INVERSE:
	    regular34=0;
	    rt_only=1;
	    invertm=0;
	    break;

	default:
	    return _hip_err(_INVALID_SKIN_FUNCTION);
	}

	Vu0SetInit();

	mtx = (sceVu0FMATRIX *)(s->skinh[LW]+1);
	dat = (sceHiPlugSkinData_t *)(s->skinh[DATA]+1);

	oldshapeid= -1;
	bm=NULL;

	for(i=0;i<s->skinh[DATA]->num;i++){

	    newshapeid= dat->id[0];

	    if (newshapeid!=oldshapeid){
		if (invertm){

		    bm= searchbasemat(s->shapeh[BM], newshapeid);
		    
		    if (bm){
			if (rt_only){
			    Vu0InverseMatrixRT(ibm, *bm);
			} else if (regular34){
			    Vu0InverseMatrix34(ibm, *bm);
			}
		    }
		    else sceVu0UnitMatrix(ibm);	/* no base matrix found  */
		}

	    }


	    oldshapeid=newshapeid;

	    vaddr.fp= &dat->vertex[3];

	    Vu0Weight(*vaddr.fvpp, dat->vertex, 
		      mtx[dat->matrix[0]], 
		      mtx[dat->matrix[1]],
		      mtx[dat->matrix[2]], 
		      mtx[dat->matrix[3]], 
		      dat->weight);

	    if (invertm) applyMatrix(*vaddr.fvpp, ibm);

	    if (calnormal){
		vaddr.fp= &dat->normal[3];

		Vu0WeightNormal(*vaddr.fvpp, dat->normal, 
				mtx[dat->matrix[0]], 
				mtx[dat->matrix[1]],
				mtx[dat->matrix[2]], 
				mtx[dat->matrix[3]], 
				dat->weight, normalize);
		if (invertm) applyMatrixNormal(*vaddr.fvpp, ibm);
	    }
		

	    dat++;
	}

    }
    return SCE_HIG_NO_ERR;
}

/********************************************************/
/*		Skin Init				*/
/********************************************************/
static sceHiErr skin_init(sceHiPlug *p, Skin_t *s)
{
    static const sceHiType skint[MAXSKINH] = {
	{SCE_HIP_COMMON,SCE_HIP_FRAMEWORK,SCE_HIP_SKIN,SCE_HIG_DATA_STATUS,SCE_HIP_SKIN_LB,SCE_HIP_REVISION},
	{SCE_HIP_COMMON,SCE_HIP_FRAMEWORK,SCE_HIP_SKIN,SCE_HIG_DATA_STATUS,SCE_HIP_SKIN_LW,SCE_HIP_REVISION},
	{SCE_HIP_COMMON,SCE_HIP_FRAMEWORK,SCE_HIP_SKIN,SCE_HIG_DATA_STATUS,SCE_HIP_SKIN_BW,SCE_HIP_REVISION},
	{SCE_HIP_COMMON,SCE_HIP_FRAMEWORK,SCE_HIP_SKIN,SCE_HIG_DATA_STATUS,SCE_HIP_SKIN_DATA,SCE_HIP_REVISION}
    };
    static const sceHiType shapet[MAXSHAPEH] = {
	{SCE_HIP_COMMON,SCE_HIP_FRAMEWORK,SCE_HIP_SHAPE,SCE_HIG_DATA_STATUS,SCE_HIP_BASEMATRIX,SCE_HIP_REVISION},
    };
    static const sceHiType shape_data_t= {
	SCE_HIP_COMMON,SCE_HIP_FRAMEWORK,SCE_HIP_SHAPE,SCE_HIG_DATA_STATUS,SCE_HIP_SHAPE_DATA,SCE_HIP_REVISION
    };
    

    int i;
        sceHiErr err;
    sceHiPlugShapeHead_t *th;


    /*	get skin  header (LB,LW,BW) */
    for(i=0;i<MAXSKINH;i++){
	if (i!=DATA){
	    s->skinh[i] = sceHiPlugSkinGetHead(p, skint[i]);
	    if(s->skinh[i] == NULL) return _hip_err(_NO_SKIN_LB+i);
	}
    }
    s->skinh[DATA] = sceHiPlugSkinGetHead(p, skint[DATA]);

    /*	get BASEMATRIX header	*/
    s->shapeh[BM] = sceHiPlugShapeGetHead(p, shapet[BM]);
    if(s->shapeh[BM] == NULL) return _hip_err(_NO_MATRIX);


    if (s->skinh[DATA]){ /* VU0 skin */
	sceHiPlugSkinData_t	*dat;
	int i;
	union {
	    sceVu0FVECTOR **fpp;
	    float *fp;
	} addr;


	err=sceHiGetData(p, (u_int **)&th, shape_data_t);
	if (err!=SCE_HIG_NO_ERR) return err;


	dat = (sceHiPlugSkinData_t *)(s->skinh[DATA]+1);

	for(i=0;i<s->skinh[DATA]->num;i++){
	    sceHiPlugShapeHead_t *gh;

	    gh=getgeomptr(th, dat->id);
	    if (gh==NULL) 
		return _hip_err(_ILLEGAL_SHAPE_FORMAT);

	    addr.fp= &dat->vertex[3];

	    *addr.fpp=sceHiPlugShapeGetGeometryVertex(gh, dat->id[3]);



	    addr.fp= &dat->normal[3];

	    *addr.fpp=sceHiPlugShapeGetGeometryNormal(gh, dat->id[3]);


	    dat++;
	}
    }

    return SCE_HIG_NO_ERR;
}

/********************************************************/
/*		Skin Plug for Vu1/Vu0			*/
/********************************************************/
sceHiErr sceHiPlugSkin(sceHiPlug *plug, int process)
{
    sceHiErr	err;
    Skin_t	*skin;

    switch(process){
      case SCE_HIG_INIT_PROCESS:
	  skin = (Skin_t *)sceHiMemAlloc(sizeof(Skin_t));
	  if(skin == NULL) return _hip_err(_NO_HEAP);
	  err = skin_init(plug, skin);
	  if(err != SCE_HIG_NO_ERR){
	      sceHiMemFree((u_int *)((u_int)skin & 0x0fffffff));
	      return err;
	  }
	  plug->args = NULL;
	  plug->stack = (u_int)skin;
	  break;

      case SCE_HIG_PRE_PROCESS:
	  skin = (Skin_t *)plug->stack;
	  if(skin == NULL) return _hip_err(_NO_STACK);
	  err=skin_calc(skin);
	  if (err != SCE_HIG_NO_ERR){
	      sceHiMemFree((u_int *)((u_int)skin & 0x0fffffff));
	      return err;
	  }
	  break;

      case SCE_HIG_POST_PROCESS:
	  break;

      case SCE_HIG_END_PROCESS:
	  skin = (Skin_t *)plug->stack;
	  if(skin == NULL) return _hip_err(_NO_STACK);
	  sceHiMemFree((u_int *)((u_int)skin & 0x0fffffff));
	  plug->stack = NULL;
	  plug->args = NULL;
	  break;

      default:
	  break;
    }

    return SCE_HIG_NO_ERR;
}
