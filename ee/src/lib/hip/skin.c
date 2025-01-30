/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: skin.c,v 1.10 2003/05/28 08:01:55 xkazama Exp $	*/
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
    sceHiErrStateType *s = sceHiGetErrStatePtr();

    s->mes = _err_mes[err];
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
    //    VN_NORMAL_FIRST_MATRIX=2,
    VONLY=3
};
enum {
    RT_ONLY_MATRIX,
    REGULAR34_MATRIX,
    NO_INVERSE

};

static inline void Vu0UnitMatrix(sceVu0FMATRIX m0)
{
    int ret = DI();
    __asm__ __volatile__("\n\
	vmr32.xyzw   vf17,vf0 			# 0,0,1,0\n\
	vsub.xzw   vf18,vf0,vf0			# 0,-,0,0\n\
	vsub.yzw   vf19,vf0,vf0			# -,0,0,0\n\
	vmaxw.y	vf18,vf0,vf0			# 0,1,0,0\n\
	vmaxw.x	vf19,vf0,vf0			# 1,0,0,0\n\
        sqc2    vf0,0x30(%0)			# 0,0,0,1\n\
        sqc2    vf17,0x20(%0) 			# 0,0,1,0\n\
        sqc2    vf18,0x10(%0)			# 0,1,0,0\n\
        sqc2    vf19,0x0(%0)			# 1,0,0,0\n\
        ": : "r" (m0) : "memory");
    if (ret) EI();
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
    int ret = DI();
    __asm__  __volatile__("\n\
	# load m1 (vf06-09)\n\
	lqc2    vf06,0x0(%1)\n\
	lqc2    vf07,0x10(%1)\n\
	lqc2    vf08,0x20(%1)\n\
	lqc2    vf09,0x30(%1)\n\
	\n\
	# save mr (vf10-12)\n\
	vmove vf10, vf06\n\
	vmove vf11, vf07\n\
	vmove vf12, vf08\n\
	\n\
	# calc -T (vf13)\n\
	vsub.xyz vf13,vf00,vf09\n\
	\n\
	# transpose mr (using saved regs: vf10-vf12)\n\
	vaddx.y vf06,vf00,vf11\n\
	vaddx.z vf06,vf00,vf12\n\
	vaddy.x vf07,vf00,vf10\n\
	vaddy.z vf07,vf00,vf12\n\
	vaddz.x vf08,vf00,vf10\n\
	vaddz.y vf08,vf00,vf11\n\
	\n\
	# -T * mo (vf09)\n\
	vmulax.xyz  ACC,vf06,vf13\n\
	vmadday.xyz  ACC,vf07,vf13\n\
	vmaddz.xyz  vf09,vf08,vf13\n\
	\n\
	# store m0\n\
	sqc2    vf06,0x0(%0)\n\
	sqc2    vf07,0x10(%0)\n\
	sqc2    vf08,0x20(%0)\n\
	sqc2    vf09,0x30(%0)\n\
	\n\
	": : "r" (m0) , "r" (m1) : "memory");
    if (ret) EI();
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
    int ret = DI();
    __asm__  __volatile__("\n\
	# load m1\n\
	lqc2    vf01,0x0(%1)\n\
	lqc2    vf02,0x10(%1)\n\
	lqc2    vf03,0x20(%1)\n\
	lqc2    vf04,0x30(%1)\n\
	\n\
	# mtmp: vf06-vf08\n\
	vopmula.xyz ACC, vf01,vf02\n\
	vopmsub.xyz vf08, vf02,vf01\n\
	\n\
	vopmula.xyz ACC, vf02,vf03\n\
	vopmsub.xyz vf06, vf03,vf02\n\
	\n\
	vopmula.xyz ACC, vf03,vf01\n\
	vopmsub.xyz vf07, vf01,vf03\n\
	\n\
	# calc |mi|\n\
	vmul.xyz  vf05, vf03, vf08\n\
	\n\
		# set w field\n\
		vsub.w vf06,vf00,vf00\n\
		vsub.w vf07,vf00,vf00\n\
		vsub.w vf08,vf00,vf00\n\
		vmove.w vf09,vf00\n\
	\n\
	vadday.x  ACC, vf05, vf05\n
	vmaddz.x  vf05, vf29, vf05	# vf05.x= |mi|,    vf29=(1,1,1,1)\n\
	\n\
		# save mtmp\n\
		vmove vf10, vf06\n\
		vmove vf11, vf07\n\
		vmove vf12, vf08\n\
	\n\
	# calc 1/|mi|\n\
	vdiv	Q, vf00w, vf05x\n\
	\n\
	# calc -T (vf13)\n\
	vsub.xyz vf13,vf00,vf04\n\
	\n\
		# transpose mtmp 3x3 part (using saved regs: vf10-vf12)\n\
		vaddy.x vf07,vf00,vf10\n\
		vaddx.y vf06,vf00,vf11\n\
		\n\
		vaddz.x vf08,vf00,vf10\n\
		vaddx.z vf06,vf00,vf12\n\
		\n\
		vaddz.y vf08,vf00,vf11\n\
		vaddy.z vf07,vf00,vf12\n\
	\n\
	vwaitq\n\
	\n\
	#  mo =  mtmp/|mi|\n\
	vmulq.xyzw vf06,vf06,Q\n\
	vmulq.xyzw vf07,vf07,Q\n\
	vmulq.xyzw vf08,vf08,Q\n\
	\n\
	# -T * mo (vf09)\n\
	vmulax.xyz  ACC,vf06,vf13\n\
	vmadday.xyz  ACC,vf07,vf13\n\
	vmaddz.xyz  vf09,vf08,vf13\n\
	\n\
	# store m0\n\
	sqc2    vf06,0x0(%0)\n\
	sqc2    vf07,0x10(%0)\n\
	sqc2    vf08,0x20(%0)\n\
	sqc2    vf09,0x30(%0)\n\
	\n\
	\n\
	": : "r" (m0) , "r" (m1) : "memory");
    if (ret) EI();
}

static void Vu0SetInit(void)
{
    /* vf29 = (1, 1, 1, 1)  */
    int ret = DI();
    __asm__  __volatile__("\n\
	vmaxw.xyzw vf29, vf00, vf00			# VF29\n\
	");
    if (ret)	EI();
}

static void load_ibm(sceVu0FMATRIX ibm)
{
    int ret = DI();
    __asm__  __volatile__("\n\
	lqc2    vf17,0x0(%0)			# load ibm\n\
	lqc2    vf18,0x10(%0)\n\
	lqc2    vf19,0x20(%0)\n\
	lqc2    vf20,0x30(%0)\n\
	": : "r" (ibm));
    if (ret)	EI();
};

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

static void find_and_load_ibm(Skin_t *s, int ibmfunc, int id)
{
    sceVu0FMATRIX ibm;				/* inverse base matrix  */

    if (ibmfunc!=NO_INVERSE){
	sceVu0FMATRIX *bm;

	bm= searchbasemat(s->shapeh[BM], id);
	if (bm){
	    if (ibmfunc==RT_ONLY_MATRIX){
		Vu0InverseMatrixRT(ibm, *bm);
	    } else{				/*  if (ibmfunc==REGULAR34_MATRIX){ */
		Vu0InverseMatrix34(ibm, *bm);
	    }
	} else Vu0UnitMatrix(ibm);	/* no base matrix found  */
    } else Vu0UnitMatrix(ibm);	/* no inverse matrix use */

    load_ibm(ibm);
}

static void Vu0Weight_Rigid(u_int *vout, sceVu0FVECTOR vin, sceVu0FMATRIX *m, int *idx)
{
    /*
      input:
      %3:  m0=m[idx[0]] vf01-04
      %4:  m1=m[idx[1]] vf05-08
      %5:  m2=m[idx[2]] vf09-12
      %6:  m3=m[idx[3]] vf13-16
      %7:  ibm (inverse base matrix) : vf17-vf20 // set already by load_ibm()
      VIN  vf27 %1
      W   vf28 %2

      work:
          MV0 vf21 (m0*v)
          MV1 vf22 (m1*v)
          MV2 vf23 (m2*v)
          MV3 vf24 (m3*v)

      output:
      VOUT  vf31 %0
    */
    int ret = DI();
    __asm__  __volatile__("\n\
	lqc2    vf27,0x0(%0)			# load vin\n\
	pref	0, 0x0(%1)			# prefetch m0\n\
	vmove.w vf31, vf00			# Vout.w=1.0f\n\
	vmove.w vf27, vf00			# Vin.w=1.0f\n\
	": : "r" (vin), "r" (m[idx[0]]) : "memory");
    __asm__  __volatile__("\n\
	lqc2    vf01,0x0(%0)			# load m0\n\
	lqc2    vf02,0x10(%0)\n\
	lqc2    vf03,0x20(%0)\n\
	lqc2    vf04,0x30(%0)\n\
	": : "r" (m[idx[0]])  : "memory");
    __asm__  __volatile__("\n\
	vmulax.xyz	ACC, vf01, vf27		# mvin= m0*vin\n\
	vmadday.xyz	ACC, vf02, vf27\n\
	vmaddaz.xyz	ACC, vf03, vf27\n\
	vmaddw.xyz	vf31, vf04, vf27\n\
	": :  : "memory");
    __asm__  __volatile__("\n\
	vmulax.xyz	ACC, vf17, vf31		  # vout= ibm*mvin\n\
	vmadday.xyz	ACC, vf18, vf31\n\
	vmaddaz.xyz	ACC, vf19, vf31\n\
	vmaddw.xyz	vf31, vf20, vf31\n\
	": :  : "memory");
    __asm__  __volatile__("\n\
	sqc2    vf31,0x0(%0)			# store vout\n\
	": : "r" (vout) : "memory");
    if (ret)	EI();
}

static void Vu0Weight(u_int *vout, sceVu0FVECTOR vin, sceVu0FMATRIX *m, int *idx, sceVu0FVECTOR w)
{
    /*
      input:
      %3:  m0=m[idx[0]] vf01-04
      %4:  m1=m[idx[1]] vf05-08
      %5:  m2=m[idx[2]] vf09-12
      %6:  m3=m[idx[3]] vf13-16
      %7:  ibm (inverse base matrix) : vf17-vf20 // set already by load_ibm()
      VIN  vf27 %1
      W   vf28 %2

      work:
          MV0 vf21 (m0*v)
          MV1 vf22 (m1*v)
          MV2 vf23 (m2*v)
          MV3 vf24 (m3*v)

      output:
      VOUT  vf31 %0
    */
    int ret = DI();
    __asm__  __volatile__("\n\
	lqc2    vf28,0x0(%2)			# load weight\n\
	lqc2    vf27,0x0(%1)			# load vin\n\
	pref	0, 0x0(%3)			# prefetch m0\n\
	vmove.w vf31, vf00			# Vout.w=1.0f\n\
	vmove.w vf27, vf00			# Vin.w=1.0f\n\
	": : "r" (vout) , "r" (vin), "r" (w) , "r" (m[idx[0]]) 
			  : "memory");
    __asm__  __volatile__("\n\
	lqc2    vf01,0x0(%0)			# load m0\n\
	lqc2    vf02,0x10(%0)\n\
	lqc2    vf03,0x20(%0)\n\
	lqc2    vf04,0x30(%0)\n\
	": : "r" (m[idx[0]])  : "memory");
    __asm__  __volatile__(" pref 0, 0x0(%0) ": : "r" (m[idx[1]])  : "memory"); // prefetch m1
    __asm__  __volatile__("\n\
	vmulax.xyz	ACC, vf01, vf27		# mv0= m0*vin\n\
	vmadday.xyz	ACC, vf02, vf27\n\
	vmaddaz.xyz	ACC, vf03, vf27\n\
	vmaddw.xyz	vf21, vf04, vf27\n\
	": :  : "memory");
    __asm__  __volatile__("\n\
	lqc2    vf05,0x0(%0)			# load m1\n\
	lqc2    vf06,0x10(%0)\n\
	lqc2    vf07,0x20(%0)\n\
	lqc2    vf08,0x30(%0)\n\
	": : "r" (m[idx[1]])  : "memory");
    __asm__  __volatile__(" pref 0, 0x0(%0) ": : "r" (m[idx[2]])  : "memory"); // prefetch m2
    __asm__  __volatile__("\n\
	vmulax.xyz	ACC, vf05, vf27		# mv1= m1*vin\n\
	vmadday.xyz	ACC, vf06, vf27\n\
	vmaddaz.xyz	ACC, vf07, vf27\n\
	vmaddw.xyz	vf22, vf08, vf27\n\
	": :  : "memory");
    __asm__  __volatile__("\n\
	lqc2    vf09,0x0(%0)			# load m2\n\
	lqc2    vf10,0x10(%0)\n\
	lqc2    vf11,0x20(%0)\n\
	lqc2    vf12,0x30(%0)\n\
	": : "r" (m[idx[2]])  : "memory");
    __asm__  __volatile__(" pref 0, 0x0(%0) ": : "r" (m[idx[3]]) : "memory"); // prefetch m3
    __asm__  __volatile__("\n\
	vmulax.xyz	ACC, vf09, vf27		# mv2= m2*vin\n\
	vmadday.xyz	ACC, vf10, vf27\n\
	vmaddaz.xyz	ACC, vf11, vf27\n\
	vmaddw.xyz	vf23, vf12, vf27\n\
	": :  : "memory");
    __asm__  __volatile__("\n\
	lqc2    vf13,0x0(%0)			# load m3\n\
	lqc2    vf14,0x10(%0)\n\
	lqc2    vf15,0x20(%0)\n\
	lqc2    vf16,0x30(%0)\n\
	": : "r" (m[idx[3]])  : "memory");
    __asm__  __volatile__("\n\
	vmulax.xyz	ACC, vf13, vf27		# mv3= m3*vin\n\
	vmadday.xyz	ACC, vf14, vf27\n\
	vmaddaz.xyz	ACC, vf15, vf27\n\
	vmaddw.xyz	vf24, vf16, vf27\n\
	": :  : "memory");
    __asm__  __volatile__("\n\
	vmulax.xyz	ACC, vf21, vf28		# mvin = mv0*w0\n\
	vmadday.xyz	ACC, vf22, vf28		# + mv1*w1\n\
	vmaddaz.xyz	ACC, vf23, vf28		# + mv2*w2\n\
	vmaddw.xyz	vf31, vf24, vf28	# + mv3*w3\n\
	": :  : "memory");
    __asm__  __volatile__("\n\
	vmulax.xyz	ACC, vf17, vf31		  # vout= ibm*mvin\n\
	vmadday.xyz	ACC, vf18, vf31\n\
	vmaddaz.xyz	ACC, vf19, vf31\n\
	vmaddw.xyz	vf31, vf20, vf31\n\
	": :  : "memory");
    __asm__  __volatile__("\n\
	sqc2    vf31,0x0(%0)			# store vout\n\
	": : "r" (vout) : "memory");
    if (ret) EI();
}


/* vout = ibm * normalize( m0*vin*w0 + m1*vin*w1 + m2*vin*w2 + m3*vin*w3 ) */
static void Vu0WeightNormal_Normalize(u_int *vout, sceVu0FVECTOR vin)
{
    /*
      already transfered by Vu0Weight()
      m0=m[idx[0]] vf01-vf04
      m1=m[idx[1]] vf05-vf08
      m2=m[idx[2]] vf09-vf12
      m3=m[idx[3]] vf13-vf16
      ibm (inverse base matrix) vf17-vf20
      W   vf28 %2

      input:
      VIN  vf27 %0

      work:
      MV0 vf21 (m0*v)
      MV1 vf22 (m1*v)
      MV2 vf23 (m2*v)
      MV3 vf24 (m3*v)
      tmp-vout vf30
      length(normal) vf25
      output:
      VOUT  vf31 %0
    */
    int ret = DI();
    __asm__  __volatile__("\n\
	lqc2    vf27,0x0(%0)			# load vin\n\
	vmove.w vf27, vf00			# Vin.w=1.0f\n\
	": : "r" (vin) : "memory");
    __asm__  __volatile__("\n\
	vmulax.xyz	ACC, vf01, vf27		# mv0= m0*vin\n\
	vmadday.xyz	ACC, vf02, vf27\n\
	vmaddz.xyz	vf21, vf03, vf27\n\
	\n\
	": :  : "memory");
    __asm__  __volatile__("\n\
	vmulax.xyz	ACC, vf01, vf27		# mv1= m1*vin\n\
	vmadday.xyz	ACC, vf02, vf27\n\
	vmaddz.xyz	vf22, vf03, vf27\n\
	": :  : "memory");
    __asm__  __volatile__("\n\
	vmulax.xyz	ACC, vf05, vf27		# mv2= m2*vin\n\
	vmadday.xyz	ACC, vf06, vf27\n\
	vmaddz.xyz	vf23, vf07, vf27\n\
	": :  : "memory");
    __asm__  __volatile__("\n\
	vmulax.xyz	ACC, vf09, vf27		# mv3= m3*vin\n\
	vmadday.xyz	ACC, vf10, vf27\n\
	vmaddz.xyz	vf24, vf11, vf27\n\
	": :  : "memory");
    __asm__  __volatile__("\n\
	vmulax.xyz	ACC, vf21, vf28		# normal(vf30)= mv0*w0\n\
	vmadday.xyz	ACC, vf22, vf28		# + mv1*w1\n\
	vmaddaz.xyz	ACC, vf23, vf28		# + mv2*w2\n\
	vmaddw.xyz	vf30, vf24, vf28	# + mv3*w3\n\
	": :  : "memory");
    __asm__  __volatile__("\n\
# normalize\n\
	vmul.xyz	vf25, vf30, vf30	# vf25= vf30^2\n\
	vadday.x	ACC, vf25, vf25		# ACC.x=vf25.y+vf25.x\n\
	vmaddz.x	vf25, vf29, vf25	# vf25.x=ACC.x+(1.0*vf25.z)  (vf29=(1,1,1,1)\n\
	\n\
	vrsqrt 		Q, vf00w, vf25x		# Q= 1/vf25.x\n\
	pref	0, 0x0(%0)			# prefetch vout\n\
	\n\
	vsub.w vf31, vf00, vf00			# Vout.w=0.0f\n\
	\n\
	vmulax.xyz	ACC, vf17, vf30		# vf30= ibm*vf30\n\
	vmadday.xyz	ACC, vf18, vf30\n\
	vmaddz.xyz	vf30, vf19, vf30\n\
	\n\
	vwaitq\n\
	\n\
	vmulq.xyz	vf31, vf30, Q		# vout(vf31)= vf30 * Q\n\
	sqc2    vf31,0x0(%0)			# store vout\n\
	": : "r" (vout) : "memory");
    if (ret) EI();
}

/* vout = ibm * m0*vin */
static void Vu0WeightNormal(u_int *vout, sceVu0FVECTOR vin)
{
    /*
      already transfered by Vu0Weight()
      m0=m[idx[0]] vf01-vf04
      ibm (inverse base matrix) vf17-vf20

      input:
      VIN  vf27 %0

      work:
      MV0 vf21 (m0*v)
      tmp-vout vf30
    */
    int ret = DI();
    __asm__  __volatile__("\n\
	lqc2    vf27,0x0(%0)			# load vin\n\
	vmove.w vf27, vf00			# Vin.w=1.0f\n\
	": : "r" (vin) : "memory");
    __asm__  __volatile__("\n\
	pref	0, 0x0(%0)			# prefetch vout\n\
	": : "r" (vout): "memory");
    __asm__  __volatile__("\n\
	vmulax.xyz	ACC, vf01, vf27		# mv0= m0*vin\n\
	vmadday.xyz	ACC, vf02, vf27\n\
	vmaddz.xyz	vf30, vf03, vf27\n\
	": :  : "memory");
    __asm__  __volatile__("\n\
	vsub.w vf31, vf00, vf00			# Vout.w=0.0f\n\
	\n\
	vmulax.xyz	ACC, vf17, vf30		# vout.xyz= ibm*mv0\n\
	vmadday.xyz	ACC, vf18, vf30\n\
	vmaddz.xyz	vf31, vf19, vf30\n\
	\n\
	sqc2    vf31,0x0(%0)			# store vout\n\
	": : "r" (vout): "memory");
    if (ret) EI();
}


static sceHiErr vu0_skin(Skin_t *s)
{
    int newshapeid, oldshapeid;
    int ibmfunc;
    sceHiPlugSkinData_t	*dat;
    sceVu0FMATRIX *mtx;
    union {
	float* fp;
	u_int **fvpp;
    } vaddr;
    int normalize;
    int calnormal;
    int i;
    int rigid;

    switch ((s->skinh[DATA]->func)&0xffff){
    case VN_WITH_NORMALIZE:
	normalize=1; 
	calnormal=1; 
	break;
    case VN_WITHOUT_NORMALIZE: 
	normalize=0; 
	calnormal=1; 
	break;
    case VONLY:
	normalize=0; 
	calnormal=0; 
	break;
    default:
	return _hip_err(_INVALID_SKIN_FUNCTION);
    }


    ibmfunc= ( (s->skinh[DATA]->func)>>16);
    switch (ibmfunc){				/* error check only */
    case RT_ONLY_MATRIX:
    case REGULAR34_MATRIX:
    case NO_INVERSE:
	break;

    default:
	return _hip_err(_INVALID_SKIN_FUNCTION);
    }

    {
	int ret = DI();
	Vu0SetInit();

	mtx = (sceVu0FMATRIX *)(s->skinh[LW]+1);
	dat = (sceHiPlugSkinData_t *)(s->skinh[DATA]+1);

	oldshapeid= -1;

	for(i=0;i<s->skinh[DATA]->num;i++){
	    newshapeid= dat->id[0];
	    if (newshapeid!=oldshapeid){
		/* set ibm (invert base matrix) to vf-register */
		find_and_load_ibm(s, ibmfunc, newshapeid); 
	    }
	    oldshapeid=newshapeid;

	    rigid=0;
	    if (dat->weight[0]==1.0f) rigid=1;

	    vaddr.fp= &dat->vertex[3];
	    if (rigid){
		Vu0Weight_Rigid(*vaddr.fvpp, dat->vertex, mtx, dat->matrix);
	    } else{
		Vu0Weight(*vaddr.fvpp, dat->vertex, mtx, dat->matrix, dat->weight);
	    }

	    if (calnormal){
		vaddr.fp= &dat->normal[3];

		if (rigid || (normalize==0)){
		    Vu0WeightNormal(*vaddr.fvpp, dat->normal);
		} else{
		    Vu0WeightNormal_Normalize(*vaddr.fvpp, dat->normal);
		}
	    }

	    dat++;
	    if (ret) EI();
	}
    }

    return SCE_HIG_NO_ERR;
}


/********************************************************/
/*		Skin Calc				*/
/********************************************************/
static sceHiErr skin_calc(Skin_t *s)
{
    sceVu0FMATRIX *m[3];		/* LB,LW,BW matrix */
    int num;				/* num of skin unit */
    int *id;				/* BW matrix id from BASEMATRIX */
    int i;


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
	return vu0_skin(s);
    }
    return SCE_HIG_NO_ERR;
}

static  sceHiErr vu0_skin_init(sceHiPlug *p, Skin_t *s)
{
    int i;
    union {
	sceVu0FVECTOR **fpp;
	float *fp;
    } addr;
    sceHiPlugShapeHead_t *th;
    sceHiPlugSkinData_t	*dat;
    sceHiErr err;
    static const sceHiType shape_data_t= {
	SCE_HIP_COMMON,SCE_HIP_FRAMEWORK,SCE_HIP_SHAPE,SCE_HIG_DATA_STATUS,SCE_HIP_SHAPE_DATA,SCE_HIP_REVISION
    };


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
    int i;


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
	return vu0_skin_init(p, s);
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
