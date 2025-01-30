/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
*/
/*
 *
 *	Copyright (C) 2002 Sony Computer Entertainment Inc.
 *							All Right Reserved
 *
 */
#include <eekernel.h>
#include <malloc.h>
#include <eeregs.h>
#include <libvu0.h>
#include <libgp.h>
#include <libhig.h>
#include <libhip.h>

/***********************************************
Usage:
	void MatrixDispSet(sceHiPlugShapeHead_t *sh, sceHiPlugMicroData_t *md)
	sh: basematrix header
	md: micro data


sceHiPlug *shapeP;
sceHiPlug *microP;
const sceHiType bmtype={ // base_matrix
	SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_SHAPE, 
	SCE_HIG_DATA_STATUS, SCE_HIP_BASEMATRIX, SCE_HIP_REVISION
};
sceHiPlugMicroData_t *microD;
sceHiPlugShapeHead_t *skinmh;


    // initialize
    mh=sceHiPlugShapeGetHead(shapeP, bmtype);
    microD = sceHiPlugMicroGetData(microP);

    // per draw
    MatrixDispSet(mh, microD);

 ***********************************************/



/************************************************/
/*	matrix-disp gp init/regist/delete		*/
/************************************************/
static sceGpPrimR	*MatrixDisp=NULL;	/* matrix-disp gp packet */
#define NLINE	3
static sceVu0IVECTOR MatrixDispc[NLINE*2] = {		/* colors */
    {0,0,0,128},    {255,0,0,128},
    {0,0,0,128},    {0,255,0,128},
    {0,0,0,128},    {0,0,255,128},
};

static void MatrixDispIni(void)
{
    const u_int type = SCE_GP_PRIM_R | SCE_GP_LINE_G;
    int i;
    int index;

    /*	libgp packet	*/
    MatrixDisp = memalign(16, sizeof(u_long128) * sceGpChkPacketSize(type, NLINE));
    sceGpInitPacket(MatrixDisp, type, NLINE);

    for(i=0;i<NLINE*2;i++){
	index = sceGpIndexRgbaLineG(i);
	sceGpSetRgba(MatrixDisp, index, MatrixDispc[i][0], MatrixDispc[i][1], MatrixDispc[i][2], MatrixDispc[i][3]);
    }
}

void MatrixDispEnd(void)
{
    if (MatrixDisp) {
	free(MatrixDisp);
	MatrixDisp=NULL;
    }
}

/************************************************/
/*	matrix-disp gp data set			*/
/************************************************/
#define AXISLEN (1.7f)
static sceVu0FVECTOR MatrixDispv[NLINE*2] = {		/* vertices */
    {0.0f,0.0f,0.0f,1.0f},{AXISLEN,0.0f,0.0f,1.0f},
    {0.0f,0.0f,0.0f,1.0f},{0.0f,AXISLEN,0.0f,1.0f},
    {0.0f,0.0f,0.0f,1.0f},{0.0f,0.0f,AXISLEN,1.0f},
};




void MatrixDispSet(sceHiPlugShapeHead_t *sh, sceHiPlugMicroData_t *md)
{
    int index;
    int i,j;
    sceVu0IVECTOR v;
    sceVu0FMATRIX m;
    sceHiPlugShapeMatrix_t *mh;

    if (MatrixDisp==NULL) MatrixDispIni();

    for (j=0; j<sh->top.num; j++){
	mh=sceHiPlugShapeGetMatrix(sh, j);
	sceVu0MulMatrix(m, md->wscreen, mh->local);

	for(i=0;i<NLINE*2;i++){
	    sceVu0RotTransPers(v, m, MatrixDispv[i], 1);
	    index = sceGpIndexXyzfLineG(i);
	    sceGpSetXyz(MatrixDisp, index, v[0], v[1], v[2]);
	}

	sceHiDMAMake_DynamicChainStart();
	sceHiDMAMake_WaitMicro();
	sceHiDMAMake_LoadGSLump((u_int*)(&MatrixDisp->giftag1), MatrixDisp->dmanext.qwc);
	sceHiDMAMake_DynamicChainEnd();

    }
}
