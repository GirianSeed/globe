/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *               Graphics Framework Sample Program
 *                  
 * 
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          Name : micro.h
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            0.40      Jun, 2000      Aoki        Initial
 *            2.1       Nov,17,2000
 */

#ifndef _MICRO_H_
#define _MICRO_H_

/*******************************************************
 * type
 *******************************************************/
typedef enum {
    BASE,
    CULL,
    POINTL,
    SPOTL,
    FOG,
    ANTI,
    MAX_MICRO
} fwMicroType_t;

typedef struct {
    u_int	pre;	/* 0/1 prmode enable */
    u_int	prim;	/* valid for fge, abe, aa1, fst, ctxt, fix(gitag_shapemask‚Æ‘Î‰ž) */
    u_int	nreg;
    u_int	regs0;
    u_int	regs1;
} fwMicroGifParam_t;

typedef struct {
    sceVu0FMATRIX	vector;
    sceVu0FMATRIX	point;
    sceVu0FMATRIX	color;
} fwMicroLight_t;


typedef struct {
    sceVu0FMATRIX	wscr;
    sceVu0FMATRIX	wclp;
    sceVu0FMATRIX	cscr;
    sceVu0FMATRIX	init_data;
    fwMicroLight_t	lightgrp[3];
} fwMicroData_t;

#if 0
typedef u_int *fwMicroTable_t;
#else
enum {
    FW_MATTR_NONE	= 0,
    FW_MATTR_FGE	= (1 << 0),
    FW_MATTR_ANTI	= (1 << 1)
};
typedef struct {
    u_int	*mcode;
    u_int	mattr;
} fwMicroTable_t;
#endif

typedef struct{
    fwMicroType_t	mic_id;
    qword	**mcode;	/* micro code DMA chain */
   qword	*mdata;	/* micro data DMA chain */
    fwMicroGifParam_t *gifparam;
    qword	giftag_on_micro;
    fwMicroData_t	*data;
} fwMicroFrame_t;

#define fwMicroSetWorldScreen(mic, mat)			\
		sceVu0CopyMatrix(((fwMicroData_t *)(mic))->wscr, (mat))

#define fwMicroSetWorldClip(mic, mat)			\
		sceVu0CopyMatrix(((fwMicroData_t *)(mic))->wclp, (mat))

#define fwMicroSetClipScreen(mic, mat)			\
		sceVu0CopyMatrix(((fwMicroData_t *)(mic))->cscr, (mat))

#define fwMicroSetInitData(mic, mat)			\
		sceVu0CopyMatrix(((fwMicroData_t *)(mic))->init_data, (mat))

#define fwMicroSetLightVect(mic, grp, vec0, vec1, vec2) \
		sceVu0NormalLightMatrix(((fwMicroData_t *)(mic))->lightgrp[(grp)].vector, vec0, vec1, vec2); \
		((fwMicroData_t *)(mic))->lightgrp[(grp)].vector[3][3] = 1.0f;

#define fwMicroSetLightColor(mic, grp, col0, col1, col2, col3) \
		sceVu0LightColorMatrix(((fwMicroData_t *)(mic))->lightgrp[(grp)].color, col0, col1, col2, col3);

#define fwMicroSetLightPoint(mic, grp, point0, point1, point2) \
               sceVu0CopyVector(((fwMicroData_t *)(mic))->lightgrp[(grp)].point[FW_LIGHT_0], point0); \
                sceVu0CopyVector(((fwMicroData_t *)(mic))->lightgrp[(grp)].point[FW_LIGHT_1], point1); \
                sceVu0CopyVector(((fwMicroData_t *)(mic))->lightgrp[(grp)].point[FW_LIGHT_2], point2);    

extern sceVu0FVECTOR	fwMicroZeroVector;
extern qword	fwMicroGiftagCurrent;

/*******************************************************
 * functions
 *******************************************************/
extern fwMicroFrame_t* fwMicroInit(fwDataHead_t *);
extern void fwMicroPreCalc(fwMicroFrame_t *);
extern void fwMicroPostCalc(fwMicroFrame_t *, int);
extern void fwMicroFinish(fwMicroFrame_t *);
extern void fwMicroChange(fwMicroType_t);
extern fwMicroType_t fwMicroGetCurrent(void);
extern void fwMicroCtrl(void);

#endif /* !_MICRO_H_ */
