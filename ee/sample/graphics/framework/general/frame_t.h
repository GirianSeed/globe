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
 *                          Name : frame_t.h
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            0.30      Apr,4,2000     thatake     Beta0
 *            2.1       Nov,17,2000    
 */
#ifndef _FRAME_T_H_
#define _FRAME_T_H_

#include <libvu0.h>
#include "packbuf.h"

/*******************************************************
 * type declarations
 *******************************************************/

/***********
 * Shape CLASS
 ***********/
typedef struct{
    sceVu0IVECTOR	giftag0;	/* vu1mem 8 */
    sceVu0IVECTOR	giftag1;	/* vu1mem 9 */
    sceVu0IVECTOR	prim0;		/* vu1mem 10 */
    sceVu0IVECTOR	prim1;		/* vu1mem 11 */
    u_int	*pRawData;
    int		vertNum;
    int		texNum;
    int		mateNum;
    int		geomNum;
    u_int	texID;
    u_int	matID;
} fwShape_t;

typedef struct{
    qword	*my_pack;	/* shape frameÇÃpacket */
    qword	**pack;		/* â∫Ç…êUÇÁÇ≥Ç™Ç¡ÇƒÇ¢ÇÈ packet */
    int		num;
    fwShape_t *shape;
    u_int	*pPackData;
    u_int	VertexTotal;
} fwShapeFrame_t;

/***********
 * Tex2d CLASS
 ***********/
typedef struct{
    qword GIFtag;
    qword Bitblt;
    qword Trxpos;
    qword Trxreg;
    qword Trxdir;
} fwTex2dTextTransRegs_t;

typedef struct{
    qword Tex01;
    fwTex2dTextTransRegs_t texTransRegs;
    fwTex2dTextTransRegs_t clutTransRegs;
    qword texGIFtag;
    qword clutGIFtag;
    u_int *texpData;
    u_int texBp;	  /* TBP (texel base addr at GS) */
    u_int texGsSize;	  /* blocks (aligned page break) */
    u_int texTransSize;   /* qwords */
    u_int *clutpData;
    u_int clutBp;	   /* CBP (clut base addr at GS) */
    u_int clutGsSize;      /* blocks */
    u_int clutTransSize;   /* qwords */
} fwTex2dImage_t;

typedef struct{
    int num;
    fwTex2dImage_t *image;
    u_int*  pResidentPackData;
    qword *pack;
    float	TransSizeTotal;
} fwTex2dFrame_t;

/***********
 * Hrchy CLASS
 ***********/
typedef struct	_Hierarchy{
    sceVu0FVECTOR	trans;
    sceVu0FVECTOR	rot;
    sceVu0FVECTOR	scale;
    int			shape;		/* not use */
    int			parent;
    int			child;
    int			sibling;
} fwHrchy_t;

typedef	struct _Matrix{
    u_int		rorder;		/* not use */
    u_int		reserved0;
    u_int		reserved1;
    int			shape;
    sceVu0FMATRIX	local_matrix;
    sceVu0FMATRIX	light_matrix;
} fwHrchyMat_t;

typedef struct{
    int			num;
    fwHrchy_t 		*hrc;
    fwHrchyMat_t	*mat;
    u_int		padding;
    sceVu0FVECTOR	root_trans;
    sceVu0FVECTOR	root_rot;
} fwHrchyFrame_t;

/***********
 * Anime CLASS
 ***********/
typedef struct _Animation{
  int	hierarchy;
  int	nkey;
  int	keyframe;
  int   keyvalue;
} fwAnime_t;

typedef struct {
    fwAnime_t	*anime;
    fwHrchy_t	*hrchy;
    u_int	**keyframe;
    float	**keyvalue;
    int		num;
    u_int	frame;
    u_int	padding0;
    u_int	padding1;
} fwAnimeFrame_t;

/***********
 * Share CLASS
 ***********/
typedef struct {
    u_int	bv, lv;		/* shared vertex info */
    u_int	bn, ln;		/* shared normal info */
} fwShareDvn_t;

typedef struct {
    int		srcidx;		/* src index array */
    int		dstidx;		/* dst index array */
    int		geomID;		/* vertex array ptr */
    int		num;		/* num of vertex index */
} fwShare_t;

typedef struct {
    int		nsrcv, ndstv, nsrcn, ndstn;
    sceVu0FVECTOR	*srcv, *srcn, *dstv, *dstn;
    int		*vidx, *nidx;
    int		shapev, shapen;
    int		nsharevertex, nsharenormal;
    fwShare_t	*sharevertex, *sharenormal;
    fwShareDvn_t	*sharedvn;
    int		nsharedvn;
    fwHrchyMat_t	*mtx;
    int		nmtx;
    sceVu0FVECTOR	**pGeomVertex;
    sceVu0FVECTOR	**pGeomNormal;
    u_int	pad0, pad1;
} fwShareFrame_t;

#endif /* !_FRAME_T_H_ */
