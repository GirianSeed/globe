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
 * 概要
 *	Frame Work内の Shape関係(shape/material/vertex)
 *	を扱う関数群
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
 *	es2rawが吐いたデータのアドレスdataから
 *	shape dataの為のShape CLASS Instance
 *	（fwShapeFrame_t型）を作成して返す
 *	作成失敗は NULLを返します
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
 *	sfで示されるShape CLASS Instanceが保持している
 *	Packet列を Kick登録する
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
 *	sfで示されるShape CLASS Instanceを表示するための
 *	Packet列を生成する
 *	sf	: Shape CLASS Instance
 *	tex	: Tex2d Data
 *	mat	: Basematrix Data
 */
static void _fwShapeMakeFramePacket(fwShapeFrame_t *sf, u_int *tex, u_int *mat)
{
    int		i, num;
    qword	*cur;

    /* Basematrixがなくても表示できるようにしておく */ 
    if (mat == NULL)
	num = sf->num;
    else
	num = mat[3];
    sf->pack = (qword **)malloc(sizeof(qword *) * sf->num);

    /* Shape Dataが保持している全てのShapeのパケット列を作る */
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
	/* Matrixがなかった場合 */
	for (i = 0; i < sf->num; i++) {
	    REF_STCYCL_UNPACK(*cur, (u_int)_fwShapeDefaultMatrix, 0, 8, 1, 1); cur++;
	    fwPackbufAddCall(cur, sf->pack[i]); cur++;
	    sf->VertexTotal += sf->shape[i].vertNum;
	}
    } else {
	/* Matrixがあった場合 */
	int	idc;
	mat += QWSIZE;
	for (i = 0; i < num; i++) {
	    idc = mat[3];
	    mat += QWSIZE;
	    if (idc != -1) {
		/* idc != -1はそのShapeがマトリクスを持っていることを
		   意味する */

		/* matrixをロードするDMA packet */
		REF_STCYCL_UNPACK(*cur, (u_int) mat, 0, 8, 1, 1); cur++;

		/* 該当Shapeのパケット列をCALLする DMA packet */
		fwPackbufAddCall(cur, sf->pack[idc]); cur++;

		/* sf->VertexTotalはこの ShapeDataが持つ全頂点数 */
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
 *	shapeで示される一つのshapeを表示するためのPacket列を生成
 */
static qword* _fwShapeMakeShapePacket(fwShape_t *shape, u_int *tex)
{
    u_int	*rdata;
    int		i;
    qword	*cur, *ret;   

    /* rdata : Shapeのデータ */
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
    /* shape dataにあるすべてのmaterialを表示するためのpacket列の生成 */
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
 *	rで示される場所にあるshapeのmaterialデータを
 *	表示するためのpacket列を生成する
 *	作成後、次のデータへのポインタを返す
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

    /* texture indexの設定 */
    if ((tex != NULL) && texNum) {
	tex01 = _fwShapeGetTex01(shape, tex, texID);
	if (tex01 != NULL) {
	    CNT_FLUSH(*cur, 0); cur++;
	    REF_DIRECT(*cur, (u_int)_fwShapeGiftex01, 1); cur++;
	    REF_DIRECT(*cur, (u_int)tex01, 1); cur++;
	}
    }

    /* material内のすべてのGeometryの表示パケットを作る */
    shape->geomNum += geomNum;
    fwVumem1Reset();	/* 各Geonetry毎にVu Mem1は Resetされる(Double Switcing)*/
    for (k = 0; k < geomNum; k++) {
	shape->vertNum += *(read + 3);
	/* このmaterial最後のGeometryかどうか？ */
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
 * rからにある geometryデータの生成を行う
 * lastflagは それがmaterial最後のgeometryデータであることを示す
 */
static u_int* _fwShapeMakeGeomPacket(u_int *r, int lastflag)
{
#define MIN_PACKNUM	5	/* 最少 Pack 頂点数 */
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
	bufrest = fwVumem1Rest(2, 4, 3, 4); /* Vu1の残り量を返す */
	packNum = bufrest - (bufrest % cycle); /* packできる頂点数 */
	/* 
	   このgeometry表示の為の残り頂点数が pack頂点数よりすくなければ
	   pack数は 残り頂点数
	*/
	if (rest_vNum <= packNum) {
	    packNum = rest_vNum;
	    if (lastflag)
		eop = 1;
	}
	/*
	  最少パック頂点数の量 以下しかVu mem1の残りがなければ
	  これ以上詰め込まない
	  	(eop = 1のときは materialの途中でも Vu1 mem double buffer
		switch : 'VIF code Vu1-Continue'を行う)
	*/
	if (bufrest - packNum < MIN_PACKNUM)
	    eop = 1;
	/*
	  パック数で指定された頂点数の頂点を表示するための
	  packet列生成
	*/
	read = _fwShapeMakeVertexPacket(read, vNum, packNum, prim, eop);

	/* 残りの頂点数を再設定 */
	switch (prim & 0x07) {
	  case 2:
	    read -= 1 * 4;
	    rest_vNum -= (packNum - 1);
	    break;
	  case 0:
	  case 1:
	  case 3:	/* TRIANGLE */
	    rest_vNum -= packNum;	/* パックした量だけ頂点数は減る */
	    break;
	  case 4:	/* TRIANGLE STRIP */
	  case 5:
	/* 
	   Triangle Stripの場合パックした量のうち２頂点は次に使われるこ
	   とに注意
	*/
	    read -= 2 * 4;
	    rest_vNum -= (packNum - 2);
	    break;
	}
	/*
	  VIF Continueコードを使ったらBuffer Switchされるので
	   VU1 Memoryの設定をリセットする
	*/
	if (eop)
	    fwVumem1Reset();
    }

    /* read pointerを進める */
    read = r + 4 + geomSize;
    return read;
}

/*
 * v_num_onceで示されるパック数の頂点表示packet列を生成
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

    /* GIF TAGを作る */
    _fwShapeGetGiftag(giftag, &primitive[0], prim, v_num_once, eop);
    primitive[1] = primitive[2] = primitive[3] = 0x00;

    cur = fwPackbufVal.pCurr;
    /* 
       Micro CLASSで作成されたfwMicroGiftagCurrentで
       store modeや store rowを設定する
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
    /* 各頂点データを送るpacket列を生成 */
    REF_UNPACKR(*cur, (u_int)curr_ver, (iptr + 0), v_num_once);    cur++;
    REF_UNPACKR(*cur, (u_int)curr_nor, (iptr + 1), v_num_once);    cur++;
    REF_UNPACKR(*cur, (u_int)curr_st,  (iptr + 2), v_num_once);    cur++;
    REF_UNPACKR(*cur, (u_int)curr_col, (iptr + 3), v_num_once);    cur++;

    /* eopが設定されていたら Buffer Switching */
    if (eop) {
	CNT_MSCNT(*cur, 0); cur++;
    }

    fwPackbufVal.pCurr = cur;

    read += 4 * v_num_once;
    fwVumem1GetOptr(4 * v_num_once + 3);
    return read;
}

/*
 * GIF TAGの生成
 *	Shapeで設定するGiftag部分の生成
 *	ここではmicro codeに依存せず、
 *	shape形状のみに依存する部分に関する
 *	giftagを設定します
 *	すなわち
 *		NLOOP|EOP|PRIM
 *	の設定です
 *	PRIMの設定あhPrimitive/Attributeに分けて作成されます
 *	Attributeは Micro Cdoe依存部分があるので(ex. FOG)、
 *	ここで作成されたものと micro側で作成されたものがORされて
 *	実際には使用されます
 *		giftag[0] : EOP | NLOOP
 *		giftag[1] : Primitive
 *		prim_reg : Attribute
 */
static void _fwShapeGetGiftag(qword giftag, u_int *prim_reg, u_int attribute, u_int packsize, int eop)
{
    union {
	/* 上位64bitはshape pluginでは一切touchしない */
	/* Micro Pluginが 作成する */
	u_long128	ul128;
	u_long		ul64[2];
	u_int		ui32[4];
    } Giftag;

    u_int	prim, type, shade, tex, nloop;

    prim	= 0x03ff & attribute;
    type	= 0x0007 & attribute;
    shade	= 0x0001;		/* Gouraud */
    tex		= (attribute >> 4) & 0x01;    

    /* Primitive種によって loop回数が違う可能性がある*/
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
