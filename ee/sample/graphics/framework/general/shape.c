/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *               Graphics Framework Sample Program
 * 
 *      Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          Name : shape.c
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000      
 */

/*******************************************************
 * �T�v
 *	Frame Work���� Shape�֌W(shape/material/vertex)
 *	�������֐��Q
 *******************************************************/

#include <eekernel.h>
#include <stdio.h>
#include <libvu0.h>
#include <libgraph.h>
#include <malloc.h>
#include "defines.h"
#include "data.h"
#include "frame_t.h"
#include "micro.h"
#include "packbuf.h"
#include "object.h"
#include "vumem1.h"
#include "shape.h"
#include "info.h"

#define QWSIZE		(4)
#define MATRIXSIZE	(8 * QWSIZE)

static qword _fwShapeGiftex01 = {0x00008001,0x10000000,0x0000000e,0x0};

u_int	_fwShapeGiftagMask[] = {
    GIF_TAG0_NLOOP_M | GIF_TAG0_EOP_M,
    ((GS_PRIM_PRIM_M | GS_PRIM_IIP_M | GS_PRIM_TME_M) << GIF_TAG1_PRIM_O) | GIF_TAG1_FLG_M,
    0,
    0
};

static sceVu0FMATRIX _fwShapeDefaultMatrix[2] = {
    {{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}}, 
    {{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}}
};

typedef union {
    u_long	ul[2];
    qword	qw;    
} GifTag_t;
GifTag_t	_gs_set[6] __attribute__((aligned(64)));

static void _fwShapeMakeFramePacket(fwShapeFrame_t *, u_int *, u_int *);
static qword* _fwShapeMakeShapePacket(fwShape_t *, u_int *);
static u_int* _fwShapeMakeMaterialPacket(fwShape_t *, u_int *, u_int *);
static u_int* _fwShapeMakeGeomPacket(u_int *, int);
static u_int* _fwShapeMakeVertexPacket(u_int *, int, int, u_int, int);
static void _fwShapeGetGiftag(qword, u_int *, u_int, u_int, int);
static inline void _fwShapeSetGiftag(fwShape_t *, int, sceVu0IVECTOR);
static inline void _fwShapeSetPrim(fwShape_t *, int, sceVu0IVECTOR);
static fwShapeFrame_t* _fwShapeInitData(fwShapeFrame_t *, u_int *);
static u_int* _fwShapeGetTex01(fwShape_t *, u_int *, int);

/*
 * fwShapeInit(data)
 *	es2raw���f�����f�[�^�̃A�h���Xdata����
 *	shape data�ׂ̈�Shape CLASS Instance
 *	�ifwShapeFrame_t�^�j���쐬���ĕԂ�
 *	�쐬���s�� NULL��Ԃ��܂�
 */
fwShapeFrame_t* fwShapeInit(fwDataHead_t *data)
{
    fwShapeFrame_t	*shape_frame;

    shape_frame = (fwShapeFrame_t *)memalign(16, sizeof(fwShapeFrame_t));
    if (shape_frame == NULL)
	return NULL;
    shape_frame->shape = NULL;
    shape_frame->num = 0;
    shape_frame = _fwShapeInitData(shape_frame, data->shape_data);
    if (shape_frame == NULL)
	return NULL;
    _fwShapeMakeFramePacket(shape_frame, data->tex2d_data, data->basematrix);

    return shape_frame;
}

void fwShapePreCalc(fwShapeFrame_t *sf)
{
}

/*
 * fwShapePostCalc(sf)
 *	sf�Ŏ������Shape CLASS Instance���ێ����Ă���
 *	Packet��� Kick�o�^����
 */
void fwShapePostCalc(fwShapeFrame_t *sf)
{
    fwPackbufAddCall(fwPackbufKickBufVal.pCurr, sf->my_pack);
    fwPackbufKickBufVal.pCurr++;
}

void fwShapeFinish(fwShapeFrame_t *sf)
{
    free((u_int *)Paddr(sf->shape));
    free((u_int *)Paddr(sf));
}

/*
 * _fwShapeMakeFramePacket(sf, tex, mat)
 *	sf�Ŏ������Shape CLASS Instance��\�����邽�߂�
 *	Packet��𐶐�����
 *	sf	: Shape CLASS Instance
 *	tex	: Tex2d Data
 *	mat	: Basematrix Data
 */
static void _fwShapeMakeFramePacket(fwShapeFrame_t *sf, u_int *tex, u_int *mat)
{
    int		i, num;
    qword	*cur;

    /* Basematrix���Ȃ��Ă��\���ł���悤�ɂ��Ă��� */ 
    if (mat == NULL)
	num = sf->num;
    else
	num = mat[3];
    sf->pack = (qword **)malloc(sizeof(qword *) * sf->num);

    /* Shape Data���ێ����Ă���S�Ă�Shape�̃p�P�b�g������ */
    for (i = 0; i < sf->num; i++) {
	sf->pack[i] = _fwShapeMakeShapePacket(&sf->shape[i], tex);
    }

    cur = fwPackbufVal.pCurr;
    sf->my_pack = cur;
    sf->VertexTotal = 0;    

    _gs_set[0].ul[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 1);
    _gs_set[0].ul[1] = (long) SCE_GIF_PACKED_AD;
    _gs_set[1].ul[0] = 0; /* PRMODCONT = 0 */
    _gs_set[1].ul[1] = (long) SCE_GS_PRMODECONT;
    REF_DIRECT(*cur, (u_int) &(_gs_set[0]), 2); cur++;

    if (mat == NULL) {
	/* Matrix���Ȃ������ꍇ */
	for (i = 0; i < sf->num; i++) {
	    REF_STCYCL_UNPACK(*cur, (u_int)_fwShapeDefaultMatrix, 0, 8, 1, 1); cur++;
	    fwPackbufAddCall(cur, sf->pack[i]); cur++;
	    sf->VertexTotal += sf->shape[i].vertNum;
	}
    } else {
	/* Matrix���������ꍇ */
	int	idc;
	mat += QWSIZE;
	for (i = 0; i < num; i++) {
	    idc = mat[3];
	    mat += QWSIZE;
	    if (idc != -1) {
		/* idc != -1�͂���Shape���}�g���N�X�������Ă��邱�Ƃ�
		   �Ӗ����� */

		/* matrix�����[�h����DMA packet */
		REF_STCYCL_UNPACK(*cur, (u_int) mat, 0, 8, 1, 1); cur++;

		/* �Y��Shape�̃p�P�b�g���CALL���� DMA packet */
		fwPackbufAddCall(cur, sf->pack[idc]); cur++;

		/* sf->VertexTotal�͂��� ShapeData�����S���_�� */
		sf->VertexTotal += sf->shape[i].vertNum;
	    }
	    mat += MATRIXSIZE;
	}
    }

    CNT_FLUSH(*cur, 0); cur++;
    _gs_set[2].ul[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 1);
    _gs_set[2].ul[1] = (long) SCE_GIF_PACKED_AD;
    _gs_set[3].ul[0] = 1; /* PRMODCONT = 1 */
    _gs_set[3].ul[1] = (long) SCE_GS_PRMODECONT;
    REF_DIRECT(*cur, (u_int) &(_gs_set[2]), 2); cur++;

    RET_NOP(*cur, 0); cur++;
    fwPackbufVal.pCurr = cur;
}

/*
 * _fwShapeMakeShapePacket(shape, tex)
 *	shape�Ŏ��������shape��\�����邽�߂�Packet��𐶐�
 */
static qword* _fwShapeMakeShapePacket(fwShape_t *shape, u_int *tex)
{
    u_int	*rdata;
    int		i;
    qword	*cur, *ret;   

    /* rdata : Shape�̃f�[�^ */
    rdata = shape->pRawData;

    shape->prim0[0] = fwVumem1ISize();
    shape->prim1[0] = 0xdeaddead;	/* not used */

    /* skip header */
    rdata += 4;

    ret = cur = fwPackbufVal.pCurr;

    REF_UNPACK(*cur, (u_int) shape->prim0, 10, 4); cur++;
    DMA_CNT(*cur, 0);
    ADD_MSCAL(*cur, 2, 0);
    ADD_MSCNT(*cur, 3);
    cur++;

    fwPackbufVal.pCurr = cur;
    /* shape data�ɂ��邷�ׂĂ�material��\�����邽�߂�packet��̐��� */
    for (i = 0; i < shape->mateNum; i++) {
	rdata = _fwShapeMakeMaterialPacket(shape, rdata, tex);
    }

    cur = fwPackbufVal.pCurr;
    RET_NOP(*cur, 0); cur++;
    fwPackbufVal.pCurr = cur;

    return ret;
}

/*
 * _fwShapeMakeMaterialPacket(shape, r, tex)
 *	r�Ŏ������ꏊ�ɂ���shape��material�f�[�^��
 *	�\�����邽�߂�packet��𐶐�����
 *	�쐬��A���̃f�[�^�ւ̃|�C���^��Ԃ�
 */
static u_int* _fwShapeMakeMaterialPacket(fwShape_t *shape, u_int *r, u_int *tex)
{
    u_int	*read = r;
    int		geomNum = *(read + 1);
    int		texID	= *(read + 2);
    int		texNum	= *(read + 3);
    u_int	*tex01;
    int		k;
    int		lastgeom;
    qword	*cur;

    cur = fwPackbufVal.pCurr;
    read += 12;

    /* texture index�̐ݒ� */
    if ((tex != NULL) && texNum) {
	tex01 = _fwShapeGetTex01(shape, tex, texID);
	if (tex01 != NULL) {
	    CNT_FLUSH(*cur, 0); cur++;
	    REF_DIRECT(*cur, (u_int)_fwShapeGiftex01, 1); cur++;
	    REF_DIRECT(*cur, (u_int)tex01, 1); cur++;
	}
    }

    /* material���̂��ׂĂ�Geometry�̕\���p�P�b�g����� */
    shape->geomNum += geomNum;
    fwVumem1Reset();	/* �eGeonetry����Vu Mem1�� Reset�����(Double Switcing)*/
    for (k = 0; k < geomNum; k++) {
	shape->vertNum += *(read + 3);
	/* ����material�Ō��Geometry���ǂ����H */
	if (k == geomNum -1)
	    lastgeom = 1;
	else
	    lastgeom = 0;

	fwPackbufVal.pCurr = cur;
	read = _fwShapeMakeGeomPacket(read, lastgeom);
	cur = fwPackbufVal.pCurr;
    }
    fwPackbufVal.pCurr = cur;

    return read;
}

/*
 * r����ɂ��� geometry�f�[�^�̐������s��
 * lastflag�� ���ꂪmaterial�Ō��geometry�f�[�^�ł��邱�Ƃ�����
 */
static u_int* _fwShapeMakeGeomPacket(u_int *r, int lastflag)
{
#define MIN_PACKNUM	5	/* �ŏ� Pack ���_�� */
    u_int	*read		= r;
    u_int	geomSize 	= *(read + 1);
    u_int	vNum		= *(read + 3);
    u_int	rest_vNum	= *(read + 3);
    u_int	prim		= *(read + 2) & 0x3ff;
    int		eop;
    int		cycle		= 0;
    int		bufrest, packNum;

    read += 4;
    while (rest_vNum > 2) {
	eop = 0;
	switch (prim & 0x07) {
	  case 0:
	  case 1:
	    cycle = 4;
	    break;
	  case 2:
	  case 3:	/* TRIANGLE */
	    cycle = 3;
	    break;
	  case 4:	/* TRIANGLE STRIP */
	  case 5:
	    cycle = 4;
	    break;
	}
	bufrest = fwVumem1Rest(2, 4, 3, 4); /* Vu1�̎c��ʂ�Ԃ� */
	packNum = bufrest - (bufrest % cycle); /* pack�ł��钸�_�� */
	/* 
	   ����geometry�\���ׂ̈̎c�蒸�_���� pack���_����肷���Ȃ����
	   pack���� �c�蒸�_��
	*/
	if (rest_vNum <= packNum) {
	    packNum = rest_vNum;
	    if (lastflag)
		eop = 1;
	}
	/*
	  �ŏ��p�b�N���_���̗� �ȉ�����Vu mem1�̎c�肪�Ȃ����
	  ����ȏ�l�ߍ��܂Ȃ�
	  	(eop = 1�̂Ƃ��� material�̓r���ł� Vu1 mem double buffer
		switch : 'VIF code Vu1-Continue'���s��)
	*/
	if (bufrest - packNum < MIN_PACKNUM)
	    eop = 1;
	/*
	  �p�b�N���Ŏw�肳�ꂽ���_���̒��_��\�����邽�߂�
	  packet�񐶐�
	*/
	read = _fwShapeMakeVertexPacket(read, vNum, packNum, prim, eop);

	/* �c��̒��_�����Đݒ� */
	switch (prim & 0x07) {
	  case 2:
	    read -= 1 * 4;
	    rest_vNum -= (packNum - 1);
	    break;
	  case 0:
	  case 1:
	  case 3:	/* TRIANGLE */
	    rest_vNum -= packNum;	/* �p�b�N�����ʂ������_���͌��� */
	    break;
	  case 4:	/* TRIANGLE STRIP */
	  case 5:
	/* 
	   Triangle Strip�̏ꍇ�p�b�N�����ʂ̂����Q���_�͎��Ɏg���邱
	   �Ƃɒ���
	*/
	    read -= 2 * 4;
	    rest_vNum -= (packNum - 2);
	    break;
	}
	/*
	  VIF Continue�R�[�h���g������Buffer Switch�����̂�
	   VU1 Memory�̐ݒ�����Z�b�g����
	*/
	if (eop)
	    fwVumem1Reset();
    }

    /* read pointer��i�߂� */
    read = r + 4 + geomSize;
    return read;
}

/*
 * v_num_once�Ŏ������p�b�N���̒��_�\��packet��𐶐�
 */
static u_int* _fwShapeMakeVertexPacket(u_int *r, int vnum, int v_num_once, u_int prim, int eop)
{
    static qword	giftag;
    static qword	primitive;
    u_int	*read		= r;
    float	*rawdata	= (float *)read;
    float	*curr_ver	= rawdata;
    float	*curr_nor	= rawdata + vnum * 4 * 1;
    float	*curr_st	= rawdata + vnum * 4 * 2;
    float	*curr_col	= rawdata + vnum * 4 * 3;
    u_int	iptr;
    qword	*cur;

    /* GIF TAG����� */
    _fwShapeGetGiftag(giftag, &primitive[0], prim, v_num_once, eop);
    primitive[1] = primitive[2] = primitive[3] = 0x00;

    cur = fwPackbufVal.pCurr;
    /* 
       Micro CLASS�ō쐬���ꂽfwMicroGiftagCurrent��
       store mode�� store row��ݒ肷��
    */
    iptr = fwVumem1GetIptr(2);
#if 1
    CNT_UNPACKR(*cur, iptr, 1); cur++;
    ADD_QWDATA(*cur, giftag); cur++;
    CNT_UNPACKR(*cur, iptr+1, 1); cur++;
    ADD_QWDATA(*cur, primitive); cur++;
#else
    REF_UNPACKR(*cur, (u_int)giftag, iptr, 1); cur++;
    REF_UNPACKR(*cur, (u_int)primitive, iptr + 1, 1); cur++;
#endif
    CNT_STCYCL(*cur, 0, 1, 4); cur++;
    iptr = fwVumem1GetIptr(4 * v_num_once);
    /* �e���_�f�[�^�𑗂�packet��𐶐� */
    REF_UNPACKR(*cur, (u_int)curr_ver, (iptr + 0), v_num_once);    cur++;
    REF_UNPACKR(*cur, (u_int)curr_nor, (iptr + 1), v_num_once);    cur++;
    REF_UNPACKR(*cur, (u_int)curr_st,  (iptr + 2), v_num_once);    cur++;
    REF_UNPACKR(*cur, (u_int)curr_col, (iptr + 3), v_num_once);    cur++;

    /* eop���ݒ肳��Ă����� Buffer Switching */
    if (eop) {
	CNT_MSCNT(*cur, 0); cur++;
    }

    fwPackbufVal.pCurr = cur;

    read += 4 * v_num_once;
    fwVumem1GetOptr(4 * v_num_once + 3);
    return read;
}

/*
 * GIF TAG�̐���
 *	Shape�Őݒ肷��Giftag�����̐���
 *	�����ł�micro code�Ɉˑ������A
 *	shape�`��݂̂Ɉˑ����镔���Ɋւ���
 *	giftag��ݒ肵�܂�
 *	���Ȃ킿
 *		NLOOP|EOP|PRIM
 *	�̐ݒ�ł�
 *	PRIM�̐ݒ肠hPrimitive/Attribute�ɕ����č쐬����܂�
 *	Attribute�� Micro Cdoe�ˑ�����������̂�(ex. FOG)�A
 *	�����ō쐬���ꂽ���̂� micro���ō쐬���ꂽ���̂�OR�����
 *	���ۂɂ͎g�p����܂�
 *		giftag[0] : EOP | NLOOP
 *		giftag[1] : Primitive
 *		prim_reg : Attribute
 */
static void _fwShapeGetGiftag(qword giftag, u_int *prim_reg, u_int attribute, u_int packsize, int eop)
{
    union {
	/* ���64bit��shape plugin�ł͈��touch���Ȃ� */
	/* Micro Plugin�� �쐬���� */
	u_long128	ul128;
	u_long		ul64[2];
	u_int		ui32[4];
    } Giftag;

    u_int	prim, type, shade, tex, nloop;

    prim	= 0x03ff & attribute;
    type	= 0x0007 & attribute;
    shade	= 0x0001;		/* Gouraud */
    tex		= (attribute >> 4) & 0x01;    

    /* Primitive��ɂ���� loop�񐔂��Ⴄ�\��������*/
    switch (type) {
      case 0:	/* point */
      case 1:	/* line */
      case 2:	/* line strip */
      case 3:	/* triangle */
      case 4:	/* triangle strip */
      case 5:	/* triangle fan */
      case 6:	/* sprite */       
	nloop = packsize;
	break;
      case 7: /* ? */
	break;
    }

    /* making primitive attribute(prim include Primitive/Attribute) */
    prim |= (u_long) shade << GS_PRIM_IIP_O;
    prim |= (u_long) tex   << GS_PRIM_TME_O;
    *prim_reg = prim & (~GS_PRIM_PRIM_M); /* & ~(GS_PRIM_PRIM_M); /* attribute */

    /* making tag */
    Giftag.ul128 = 0;	/* clear */
    Giftag.ui32[0] = nloop;
    Giftag.ui32[1] = type; /* primitive type */
    if (eop)
	Giftag.ui32[0] |= 0x8000;

    giftag[0] = Giftag.ui32[0]; giftag[1] = Giftag.ui32[1];
    giftag[2] = Giftag.ui32[2]; giftag[3] = Giftag.ui32[3];
}

static inline void _fwShapeSetGiftag(fwShape_t *shape, int field, sceVu0IVECTOR vec)
{
    switch (field) {
      case 0:
	COPY_QW((u_int)shape->giftag0, (u_int)vec);
	break;
      case 1:
	COPY_QW((u_int)shape->giftag1, (u_int)vec);
	break;
    }
}

static inline void _fwShapeSetPrim(fwShape_t *shape, int field, sceVu0IVECTOR vec)
{
    switch (field) {
      case 0:
	COPY_QW((u_int)shape->prim0, (u_int)vec);
	break;
      case 1:
	COPY_QW((u_int)shape->prim1, (u_int)vec);
	break;
    }
}

static fwShapeFrame_t* _fwShapeInitData(fwShapeFrame_t *sf, u_int *data)
{
    int		i;
    fwShape_t	*shape;
    u_int	*currR = data;
    sceVu0FMATRIX	unit;
    sceVu0IVECTOR	ivect = {0, 0, 0, 0};
    u_int	size;

    sf->num = currR[3];
    sf->shape = (fwShape_t *)memalign(16, sizeof(fwShape_t) * sf->num);
    if (sf->shape == NULL)
	return NULL;
    sf->pPackData = NULL;
    currR += 4;
    for (i = 0; i < sf->num; i++) {
	shape = &sf->shape[i];
	shape->pRawData = currR;
	size = currR[1];
	shape->mateNum = currR[3];
	sceVu0UnitMatrix(unit);
	_fwShapeSetGiftag(shape, 0, ivect);
	_fwShapeSetGiftag(shape, 1, ivect);
	_fwShapeSetPrim(shape, 0, ivect);
	_fwShapeSetPrim(shape, 1, ivect);
	shape->vertNum = 0;
	shape->texNum = 0;
	shape->geomNum = 0;
	shape->texID = 0;
	shape->matID = 0;
	currR += (size + 4);
    }
    return sf;
}

static u_int *_fwShapeGetTex01(fwShape_t *shape, u_int *data, int texID)
{
  u_int *currR = data;
  u_int	num;
  u_int i;
  u_int	texelsize, clutsize;

  num = currR[3];	/* num of texture */
  currR += 4;
  for(i=0;i<num;i++){
      if(texID == i){
	  return currR;
	  break;
      }
      currR += 4;
      texelsize = currR[0];
      clutsize = currR[1];
      currR += 4;
      currR += texelsize;
      currR += clutsize;
  }
  return NULL;
}
