/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/*
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: shape.c,v 1.22.8.3 2002/03/04 05:44:07 aoki Exp $	*/
#include <eekernel.h>
#include <eestruct.h>
#include <stdlib.h>
#include <stdio.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>

#define PADDR_MASK 0x0fffffff
#define Paddr(x) ((u_int)(x)&PADDR_MASK)
#define QWSIZE (4)
#define MATRIXSIZE (8 * QWSIZE)

static const sceVu0FMATRIX m[2] = {
    {{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}}, 
    {{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}}
};

typedef enum {
    _POINT	= 0,
    _LINE	= 1,
    _LINE_STRIP	= 2,
    _TRIANGLE	= 3,
    _TRIANGLE_STRIP	= 4,
    _TRIANGLE_FAN	= 5,
    _SPRITE	= 6
} _primitive_t;

/************************************************/
/*		Structure			*/
/************************************************/

typedef struct{
  u_int	*data;
  u_int	*env;
}Tex2d;

typedef struct {
    u_int		id;
    u_int		primitive;	/* primitive data */
    u_int		vertex_num;
    qword		*vertex; 	/* x/y/z/1.0 array */
    qword		*normal;       	/* x/y/z/1.0 array */
    qword		*st;		/* s/t/1.0/0.0 array */
    qword		*color;		/* r/g/b/a/ array */
    int			lastflag;	/* if last geometry in maerial, be 1 */
} _geometry_t;

typedef struct {
    u_int		id;
    u_int		geometry_num;
    _geometry_t		*geometries;
    u_int		tex_id;
    u_int		tex_num;
    u_int		tag_nloop;	/* number of registers */
    qword		*giftags;	/* giftag and register data */
    u_int		material_data_num;
    qword		*material_datas;
} _material_t;

/* attributeを守るように */
typedef struct {
    sceVu0IVECTOR	data[2];	/* vumem 10,11 */

    u_int		id;
    u_int		material_num;
    _material_t		*materials;
} _shape_t;

typedef struct {
    int		shape_id;
    u_int	data;		/* matrix data */
} _matrix_t;

typedef struct {
    u_int	reserved0;
    u_int	reserved1;
    u_int	flags;
    int		shapeid;
    sceVu0FMATRIX	local;
    sceVu0FMATRIX	light;
} Matrix_t;

enum {
    MASTER_CHAIN_IN_STATIC_O	= (1 << 0),
};

typedef struct{
    u_int		flags;		/* flags */

    u_int		*rawdata;	/* shape raw data address */
    Matrix_t		*matdata;	/* basematrix raw data address */
    int 		my_id;		/* shape frame packet ID */
    int	 		*id;		/* object packetID */    
    int 		shape_num;	/* number of shape in this data */
    _shape_t 		*shape;       	/* shape data array */
    int			matrix_num;	/* number of basematrix num in this data */
    _matrix_t		*matrixs;
    Tex2d		*tex2d;
} SHAPE_FRAME;

static void _decode_shape_data(SHAPE_FRAME *, u_int *);
static void _decode_basematrix_data(SHAPE_FRAME *, u_int *);

/***************************************************
 * Local Errors
 ***************************************************/
typedef enum {
    _NO_HEAP,
    _SHAPE_PACKET_MAKING_ERR,
    _PLGBLK_STACK_BROKEN,
    _MATIDX_IS_ILLEAGAL,
    _VU1_OVERFLOW
} _local_err;

static const sceHiErr _local2higerr[] = {
    SCE_HIG_NO_HEAP,
    SCE_HIG_FAILURE,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_VALUE,
    SCE_HIG_NO_HEAP
};

static const char *_shape_err_mes[] = {
    "HiP Shape : can't allocate memory from HiG Heap\n",
    "HiP Shape : shape packet making failure\n",
    "HiP Shape : the plugin's stack is broken(maybe didnt do INIT_PROCESS)\n",
    "HiP Shape : the matrix ID value is illeagal\n",
    "HiP Shape : VU1 Memory overflow\n"
};

/* error handling */
static sceHiErr _hip_shape_err(_local_err err)
{
    sceHiErrState.mes = _shape_err_mes[err];
    return _local2higerr[err];
}

/****************************************************************/
/* VU1 Memory management					*/
/****************************************************************/
#define  VUMEM1_OVERFLOW 1
#define  VUMEM1_OK 0

static u_int vumem1isize;
static u_int vumem1osize;
static u_int vumem1irest;
static u_int vumem1orest;
static u_int vumem1iptr;
static u_int vumem1optr;
static u_int vumem1istart;
static u_int vumem1ostart;

static void Vumem1SetParam(u_int, u_int);
static void Vumem1Reset(void);
static int Vumem1CheckRest(u_int isize, u_int osize);
static int Vumem1Rest(int ioffset, int i_per_vertex, int ooffset, int o_per_vertex);
#if 0
static int Vumem1IRest();
static int Vumem1ORest();
static int Vumem1OSize();
#endif
static u_int Vumem1GetIptr(u_int isize);
static u_int Vumem1GetOptr(u_int osize);
static int Vumem1ISize();

typedef union {
    u_long	ul[2];
    qword	qw;    
} GifTag_t;

/* GS register設定用buffer */
/*	
	1. PRMODECONT設定(= 1)	2 qwrod
	2. PRIM REG設定		2 qword
	3. PRMODECONT設定(= 0)	2 qword
  を行う
  PRIM AttributeはVU1で行う
*/
GifTag_t	_gs_set[6];

/* VU1 mem managerの初期化 */
static void Vumem1SetParam(u_int isize, u_int osize)
{
    vumem1isize=isize;
    vumem1osize=osize;
    vumem1istart=0;				/* unpackR を使うときには 0 */
    vumem1ostart=vumem1istart+isize;

    Vumem1Reset();
}

/* VU1 mem managerのリセット(初期値に戻す) */
static void Vumem1Reset()
{
    vumem1iptr=vumem1istart;
    vumem1optr=vumem1ostart;
    vumem1irest=vumem1isize;
    vumem1orest=vumem1osize;
}

/* VU1 mem のバッファチェック */
static int Vumem1CheckRest(u_int isize, u_int osize)
{
    if (vumem1irest<isize) return VUMEM1_OVERFLOW;
    if (vumem1orest<osize) return VUMEM1_OVERFLOW;
    return VUMEM1_OK;
}

static int Vumem1Rest(int ioffset, int i_per_vertex, int ooffset, int o_per_vertex)
{
    int imax, omax;
    imax=(vumem1irest-ioffset)/i_per_vertex;
    omax=(vumem1orest-ooffset)/o_per_vertex;
    if (imax>omax) return omax;
    else return imax;
}

/* In Bufferの残りを返す */
#if 0
static int Vumem1IRest()
{
    return vumem1irest;
}

/* Out Bufferの残りを返す */
static int Vumem1ORest()
{
    return vumem1orest;
}
#endif
/* In Bufferのサイズを返す */
static int Vumem1ISize()
{
    return vumem1isize;
}

/* Out Bufferのサイズを返す */
#if 0 
static int Vumem1OSize()
{
    return vumem1osize;
}
#endif

/* In Bufferから isize分の領域を割り当ててもらって そこへのpointerを返す */
static u_int Vumem1GetIptr(u_int isize)
{
    u_int iptr;

    iptr=vumem1iptr;
    if (isize>0){
	if (Vumem1CheckRest(isize, 0)!=VUMEM1_OK) {
		return 0xffffffff;
	}
	vumem1irest-=isize;
	vumem1iptr+=isize;
    }
    return iptr;
}

/* Out Bufferから osize分の領域を割り当ててもらって そこへのpointerを返す */
static u_int Vumem1GetOptr(u_int osize)
{
    u_int optr;

    optr=vumem1optr;
    if (osize>0){
	if (Vumem1CheckRest(0, osize)!=VUMEM1_OK) {
		return 0xffffffff;
	}
	vumem1orest-=osize;
	vumem1optr+=osize;
    }
    return optr;
}

/****************************************************************/
/*			Shape Load				*/
/****************************************************************/
/*
  sf	: shape frame
  data	: shape data
*/
static void _decode_shape_data(SHAPE_FRAME *sf, u_int *data)
{
    _shape_t	*shape;
    _material_t	*material;
    _geometry_t	*geometry;
    int		i, j, k;

    sf->rawdata 	= data;
    sf->my_id		= 0;
    sf->id		= NULL;
    sf->shape_num	= data[3];
    sf->shape		= (_shape_t *) sceHiMemAlign(16, sizeof(_shape_t) * sf->shape_num);
    data +=  QWSIZE;	/* data block header is 1 qword */    

    /* shape data decode */
    for (i = 0; i < sf->shape_num; i++) {
	shape = &(sf->shape[i]);
	shape->id		= data[0];
	shape->material_num	= data[3];
	shape->materials	= (_material_t *) sceHiMemAlign(16, sizeof(_material_t) * sf->shape[i].material_num);
	data += QWSIZE;	/* shape header is 1 qword */

	/* material data decode */
	for (j = 0; j < shape->material_num; j++) {
	    material = &(shape->materials[j]);
	    material->id		= data[0];
	    material->geometry_num	= data[1];
	    material->tex_id		= data[2];
	    material->tex_num		= data[3];
	    material->material_data_num	= data[7];	/* qwrod unit */
	    material->giftags		= (qword *) &(data[8]);
	    material->tag_nloop		= ((sceHiGsGiftag *)material->giftags)->nloop;	    
	    material->geometries	= (_geometry_t *) sceHiMemAlign(16, sizeof(_geometry_t) * material->geometry_num);
	    material->material_datas	= (qword *)(data + (1 + 2 + material->tag_nloop) * QWSIZE);

	    /* material header 1 qword */
	    /* material info 2 qword */
	    /* material data = material_data_num + tag_nloop * QWSIZE */
	    data += (1 + 2 + material->material_data_num + material->tag_nloop) * QWSIZE;

	    /* goemetry data decode */
	    for (k = 0; k < material->geometry_num; k++) {
		geometry = &(material->geometries[k]);
		geometry->id		= data[0];
		geometry->primitive	= data[2];
		geometry->vertex_num	= data[3];
		/* vertex start data[4] */
		/* 1 vertex/normal/st/color : 4 float */
		/* order.... n*vertex/n*normal/n*st/n*color */
		geometry->vertex	= (qword *) &(data[4 + 4 * 0 * geometry->vertex_num]);
		geometry->normal	= (qword *) &(data[4 + 4 * 1 * geometry->vertex_num]);
		geometry->st		= (qword *) &(data[4 + 4 * 2 * geometry->vertex_num]);
		geometry->color		= (qword *) &(data[4 + 4 * 3 * geometry->vertex_num]);

		if (k == material->geometry_num - 1)
		    geometry->lastflag = 1;
		else
		    geometry->lastflag = 0;

		data += QWSIZE + geometry->vertex_num * 4 * QWSIZE;
	    }	    
	}
    }
}

/*
  sf	: shape frame
  data	: basematrix data

  DO _decode_shape_data ahead by all means.
*/
static void _decode_basematrix_data(SHAPE_FRAME *sf, u_int *data)
{
    _matrix_t	*matrix;
    int		i;

    if (data == NULL) {
	sf->matdata = NULL;

	sf->matrix_num	= sf->shape_num;
	sf->matrixs	= (_matrix_t *) sceHiMemAlign(16, sizeof(_matrix_t) * sf->matrix_num);
	for (i = 0; i < sf->matrix_num; i++) {
	    matrix = (_matrix_t *)&(sf->matrixs[i]);
	    matrix->shape_id = i;
	    matrix->data = (u_int)m;	/* default matrix */
	}
    } else {
	sf->matdata = (Matrix_t *)(data + QWSIZE);
    
	sf->matrix_num	= data[3];
	sf->matrixs	= (_matrix_t *) sceHiMemAlign(16, sizeof(_matrix_t) * sf->matrix_num);    
	data += QWSIZE;

	for (i = 0; i < sf->matrix_num; i++) {
	    matrix = (_matrix_t *)&(sf->matrixs[i]);      
	    matrix->shape_id = (int)data[3];
	    matrix->data = (u_int)&(data[4]);
	    data += MATRIXSIZE + QWSIZE;
	}
    }
}

/****************************************************************/
/*			Shape Packet				*/
/****************************************************************/

/* 
  ShapeGetGiftag()
  	Shapeで設定するGiftag部分の作成
	ここでは マイクロコードに依存せず、
	Shape形状のみに依存する部分に関する
	giftagを作成する
	すなわち
		NLOOP/EOP/PRIM
	の設定である
	PRIMの設定は Primitive/Attributeに分けて作成される
	Primitive自体はそのまま使用されるがAttributeは
	MicroCode依存部分(ex. FOG)などがあるため
	ここで作成されたものとmicro pluginで作成されたものが
	ORされて実際に使用される

	返値とVU1 Micro Cdeでの動作（括弧内で説明しているもの）
		giftag[0] : EOP | NLOOP	(OutbufferのGiftag部分に直接Copy)
		giftag[1] : Primitive	(PRIMレジスタに直接いれられる)
		prim_reg  : Attribute	(Micro Plug作成dataとORされPRMODEレジスタに)

       01.01.16
       	gs service functionsの設定を有効にするように変更
 */
static void ShapeGetGiftag(qword giftag, u_int *prim_reg, u_int attribute, u_int packsize, int eop)
{
    union {
	/* 上位64bitはshape pluginでは一切touchしない */
	/* Micro Pluginが 作成する */
	u_long128	ul128;
	u_long		ul64[2];
	u_int		ui32[4];
    } Giftag;

    u_int	prim, type, shade, tex, nloop = 0;
    sceHiGsGeneral	*gen;

    prim	= 0x03ff & attribute;
    type	= 0x0007 & attribute;
    shade	= 0x0001;		/* Gouraud */
    tex		= (attribute >> 4) & 0x01;    
    gen		= sceHiGsGeneralStatus();

    /* Primitive種によって loop回数が違う可能性がある(?)*/
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
    /* 1st clear Attribute by Mask */
    prim = prim & GS_PRIM_PRIM_M;
    if (shade && gen->prmode.IIP)
	prim |= (u_long) 1 << GS_PRIM_IIP_O;
    if (tex && gen->prmode.TME)
	prim |= (u_long) 1 << GS_PRIM_TME_O;
/*
    prim |= (u_long) shade << GS_PRIM_IIP_O;
    prim |= (u_long) tex   << GS_PRIM_TME_O;
*/
    *prim_reg = prim & (~GS_PRIM_PRIM_M); /* & ~(GS_PRIM_PRIM_M); attribute */

    /* making tag */
    Giftag.ul128 = 0;	/* clear */
    Giftag.ui32[0] = nloop;
    Giftag.ui32[1] = type; /* primitive type */
    if (eop)
	Giftag.ui32[0] |= 0x8000;

    giftag[0] = Giftag.ui32[0]; giftag[1] = Giftag.ui32[1];
    giftag[2] = Giftag.ui32[2]; giftag[3] = Giftag.ui32[3];
}

#define NUMIDX 3
/*
 *
 */
static const qword __attribute__ ((aligned(16))) giftex01 = {0x00008001,0x10000000,0x0000000e,0x0};

static u_int *getTex01(u_int *data, int texID)
{
  u_int *currR = data;
  u_int	num;
  u_int i;
  u_int	texelsize, clutsize;

  num = currR[NUMIDX];	/* num of texture */
  currR += QWSIZE;
  for(i=0;i<num;i++){
      if(texID == i){
	  return currR;
	  break;
      }
      currR += QWSIZE;
      texelsize = currR[0];
      clutsize = currR[1];
      currR += QWSIZE;
      currR += texelsize;
      currR += clutsize;
  }
  return NULL;
}
static sceHiGsGiftag *getPackedGiftag(u_int *data, int id)
{
  int	num;
  int	i;
  sceHiGsGiftag	*giftag;

  num = (int)*(data + NUMIDX);
  if(id >= num) return NULL;

  giftag = (sceHiGsGiftag *)(data + QWSIZE);
  for(i=0;i<id;i++){
    giftag = giftag + giftag->nloop + 1;
  }

  return giftag;
}

/* 
  primitive typeによって 最小pack vertex数が違う
*/
static inline int _get_pack_unit(_primitive_t prim_type)
{
    int	unit = 0;

    switch (prim_type) {
      case _POINT:
	unit = 4;
	break;
      case _LINE:
	unit = 4;
	break;
      case _LINE_STRIP:
	unit = 3;
	break;
      case _TRIANGLE:
	unit = 3;
	break;
      case _TRIANGLE_STRIP:
	unit = 4;
	break;
      case _TRIANGLE_FAN:
	unit = 4;
	break;
      case _SPRITE:
	unit = 1;
	break;
    }

    return unit;
}

/* 
  bufferサイズと残りのサイズ、最小パックサイズからpackする量を決定する
  また、パケットの区切とするべき状態であるならば*eop == 1にする
*/
static inline u_int _calc_pack_Vnum(u_int rest, int cycle, int *eop)
{
#define MIN_PACKNUM	(5)	/* 最低pack vertex数 */
#define	IN_OFFSET	(2)	/* send１回当りのgiftagが使用するinbufferの量 qword unit */
#define	IN_PER_VERTEX	(4)	/* vertex１コ当りのinbufferの量 qword unit */
#define OUT_OFFSET	(3)	/* send１回当りのgiftagが使用するoutbufferの量 qword unit */
#define OUT_PER_VERTEX	(4)	/* vertex１コ当りのoutbufferの量 qword unit */
    u_int	pack;
    u_int	buf;	/* Vu1 Memoryに入りきる最大のvertex数 */

    /* VU1 memoryに入りきる最大vertex数を得る */
    buf = Vumem1Rest(IN_OFFSET, IN_PER_VERTEX, OUT_OFFSET, OUT_PER_VERTEX);

    /* pack数はcycleの倍数でなくてはならない */
    pack = buf - (buf % cycle);

    /* 残り頂点数が Vu1 memoryに入るきる量よりすくなければ */
    if (rest <= pack)
	pack = rest; /* 全部入れる */

    /* 入れたのち vu1 memoryに十分なあきがなければEOP */
    *eop = 0;
    if (buf - pack < MIN_PACKNUM)
	*eop = 1;

    return pack;
}

/* 
  primitive種によっては、逆戻り(後方移動)させなければならない
  こともある
*/
static inline u_int _calc_backward_Vnum(_primitive_t prim_type)
{
    u_int	back = 0;

    switch (prim_type) {
      case _POINT:	
      case _LINE:
	back = 0;
	break;
      case _LINE_STRIP:
	back = 1;
	break;
      case _TRIANGLE:
	back = 0;
	break;
      case _TRIANGLE_STRIP:
	back = 2;
	break;
      case _TRIANGLE_FAN:
	back = 1; /* これに加えて例外処理転送1vertexあり */
	break;
      case _SPRITE:
	back = 0;
	break;
    }
    return back;
}

static sceHiErr ShapeMakeVertexPacket(_geometry_t *geom, u_int start, u_int pack, int eop)
{
    u_int	iptr;	/* in buffer pointer */
    sceHiErr	err;

    /* pack数のvertexデータ */
    iptr = Vumem1GetIptr(4 * pack);
    if (iptr == 0xffffffff)
	return _hip_shape_err(_VU1_OVERFLOW);
    if ((err = sceHiDMAMake_LoadStep((u_int *)(iptr + 0), (u_int *)&(geom->vertex[start]), pack, 1, 3)) != SCE_HIG_NO_ERR)	return err;
    if ((err = sceHiDMAMake_LoadStep((u_int *)(iptr + 1), (u_int *)&(geom->normal[start]), pack, 1, 3)) != SCE_HIG_NO_ERR)	return err;
    if ((err = sceHiDMAMake_LoadStep((u_int *)(iptr + 2), (u_int *)&(geom->st[start]),     pack, 1, 3)) != SCE_HIG_NO_ERR)	return err;
    if ((err = sceHiDMAMake_LoadStep((u_int *)(iptr + 3), (u_int *)&(geom->color[start]),  pack, 1, 3)) != SCE_HIG_NO_ERR)	return err;

    if (eop)
	if ((err = sceHiDMAMake_ContinueMicro()) != SCE_HIG_NO_ERR)	return err;

    return SCE_HIG_NO_ERR;
}

static sceHiErr ShapeMakeVertexGiftag(_geometry_t *geom, u_int pack, int eop)
{
    qword	giftag1;
    qword	giftag2;
    u_int	iptr;	/* in buffer pointer */
    sceHiErr	err;
    
    ShapeGetGiftag(giftag1, &giftag2[0], geom->primitive, pack, eop);
    giftag2[1] = giftag2[2] = giftag2[3] = 0;

    /* giftagなどのdata２qword */
    iptr = Vumem1GetIptr(2);
    if (iptr == 0xffffffff)
	return _hip_shape_err(_VU1_OVERFLOW);
    if ((err = sceHiDMAMake_LumpStart((u_int *)iptr)) != SCE_HIG_NO_ERR)	return err;
    if ((err = sceHiDMAMake_Lump(giftag1)) != SCE_HIG_NO_ERR)			return err;
    if ((err = sceHiDMAMake_Lump(giftag2)) != SCE_HIG_NO_ERR)			return err;
    if ((err = sceHiDMAMake_LumpEnd()) != SCE_HIG_NO_ERR)			return err;

    return SCE_HIG_NO_ERR;
}

static sceHiErr ShapeMakeExceptionTriFan(_geometry_t *geom)
{
    u_int	iptr;	/* in buffer pointer */
    sceHiErr	err;

    iptr = Vumem1GetIptr(4);
    if (iptr == 0xffffffff)
	return _hip_shape_err(_VU1_OVERFLOW);
    if ((err = sceHiDMAMake_LoadPtr((u_int *)(iptr + 0), (u_int *)&(geom->vertex[0]), 1)) != SCE_HIG_NO_ERR)	return err;
    if ((err = sceHiDMAMake_LoadPtr((u_int *)(iptr + 1), (u_int *)&(geom->normal[0]), 1)) != SCE_HIG_NO_ERR)	return err;
    if ((err = sceHiDMAMake_LoadPtr((u_int *)(iptr + 2), (u_int *)&(geom->st[0]),     1)) != SCE_HIG_NO_ERR)	return err;
    if ((err = sceHiDMAMake_LoadPtr((u_int *)(iptr + 3), (u_int *)&(geom->color[0]),  1)) != SCE_HIG_NO_ERR)	return err;
    return SCE_HIG_NO_ERR;
}

static sceHiErr ShapeMakeGeometryPacket(_geometry_t *geom)
{
    u_int		restVnum;	/* 操作残りvertex数 */
    u_int		packVnum;	/* packするvertex数 */
    u_int		start;		/* send開始vertex number */
    sceHiErr		err;
    int			eop;		/* end of packet flag */
    int			cycle;		/* pack最小単位 */
    _primitive_t	prim_type;	/* primitive種 */
    int			optr;

    prim_type = geom->primitive & 0x07;

    /* primitive種別な 最小pack単位を得る */
    cycle = _get_pack_unit(prim_type);

    /* 未転送vertexが存在する限り */	
    for (restVnum = geom->vertex_num, start = 0; restVnum > 0; restVnum -= packVnum, start += packVnum) {

	/* packするvertex数 および EOPを決定する */
	if (prim_type == _TRIANGLE_FAN && start != 0) {
	    /*
	      triangle fanで geometry最初のデータでなければ 例外処理転送1 vertex分
	      restVnumに上乗せした状態で計算する
	    */
	    packVnum = _calc_pack_Vnum(restVnum + 1, cycle, &eop);
	} else
	    packVnum = _calc_pack_Vnum(restVnum, cycle, &eop);

	/* geometryの終わりかどうかでEOPを再決定する */
	if (geom->lastflag == 1)
	    eop = 1;

	/* Geometry Packetはすべて Double Buffer内に作られる */
	if ((err = sceHiDMAMake_DBufStart()) != SCE_HIG_NO_ERR)	return err;

	/* PACKET : giftag  */ 
	if ((err = ShapeMakeVertexGiftag(geom, packVnum, eop)) != SCE_HIG_NO_ERR)	return err;
	optr = Vumem1GetOptr(3);	/* Giftagでoutbufferに3qword使われる */
	if (optr == 0xffffffff)
	    return _hip_shape_err(_VU1_OVERFLOW);

	/* triangle fanの場合、geometryの最初以外は 例外処理転送が存在する */
	/* PACKET: exception triangle fan */	
	if (prim_type == _TRIANGLE_FAN && start != 0) {
	    if ((err = ShapeMakeExceptionTriFan(geom)) != SCE_HIG_NO_ERR)		return err;
	    optr = Vumem1GetOptr(4);	/* exeption tri fanで outbufferに1qword使われる */
	    if (optr == 0xffffffff)
		return _hip_shape_err(_VU1_OVERFLOW);
	    packVnum--;	/* 例外処理転送1vertex分減らしておく */
	}

	/* startからpackVnum分転送する */
	err = ShapeMakeVertexPacket(geom, start, packVnum, eop);
	optr = Vumem1GetOptr(4 * packVnum);	/* packVnum * 4 qwordが outbufferで使われる */
	if (optr == 0xffffffff)
	    return _hip_shape_err(_VU1_OVERFLOW);
	if (err != SCE_HIG_NO_ERR)
	    return err;	    

	/* Double Bufferingを一旦終了させておく */
	if ((err = sceHiDMAMake_DBufEnd()) != SCE_HIG_NO_ERR)	return err;

	/* 終わりでなければbackwardする量を決定する */
	if (packVnum != restVnum) {
	    packVnum -= _calc_backward_Vnum(prim_type);
	}

	/* End of PacketならBufferをresetする */
	if (eop) {
	    Vumem1Reset();
	}
    }

    return SCE_HIG_NO_ERR;
}

static sceHiErr ShapeMakeMaterialPacket(_material_t *mt, Tex2d *tex2d)
{
    int		i;
    u_int	*tex01;
    sceHiGsGiftag	*textag;
    sceHiErr	err;


    /* PACKET: send giftag for texture */
    if ((tex2d->env != NULL) && (mt->tex_num)) {
	textag = getPackedGiftag(tex2d->env, mt->tex_id);
	if (textag != NULL) {
	    if ((err = sceHiDMAMake_WaitMicro()) != SCE_HIG_NO_ERR)					return err;
	    if (mt->tag_nloop)
		if ((err = sceHiDMAMake_LoadGS((u_int *)mt->giftags, mt->tag_nloop + 1)) != SCE_HIG_NO_ERR)		return err;
	    if ((err = sceHiDMAMake_LoadGS((u_int *)textag, textag->nloop + 1)) != SCE_HIG_NO_ERR)	return err;
	}
	/* PACKET: send material data */
	if (mt->material_data_num) {
	    if ((err = sceHiDMAMake_LoadPtr((u_int *) 24, (u_int *)mt->material_datas, mt->material_data_num)) != SCE_HIG_NO_ERR)	return err;
	}
    } else if ((tex2d->data != NULL) && (mt->tex_num)) {
	tex01 = getTex01(tex2d->data, mt->tex_id);
	if (tex01 != NULL) {
	    if ((err = sceHiDMAMake_WaitMicro()) != SCE_HIG_NO_ERR)				return err;
	    if ((err = sceHiDMAMake_LoadGS((u_int *)giftex01, 1)) != SCE_HIG_NO_ERR)	return err;
	    if ((err = sceHiDMAMake_LoadGS((u_int *)tex01, 1)) != SCE_HIG_NO_ERR)		return err;
	}
	/* PACKET: send material data */
	if (mt->material_data_num) {
	    if ((err = sceHiDMAMake_LoadPtr((u_int *) 24, (u_int *)mt->material_datas, mt->material_data_num)) != SCE_HIG_NO_ERR)	return err;
	}
    } else {
	if (mt->material_data_num) {
	    if ((err = sceHiDMAMake_WaitMicro()) != SCE_HIG_NO_ERR)	return err;
	    if ((err = sceHiDMAMake_LoadPtr((u_int *) 24, (u_int *)mt->material_datas, mt->material_data_num)) != SCE_HIG_NO_ERR)	return err;
	}
    }

    /* PACKET: send each geometry */
    Vumem1Reset();
    for (i = 0; i < mt->geometry_num; i++) {
	err = ShapeMakeGeometryPacket(&(mt->geometries[i]));
	if (err != SCE_HIG_NO_ERR)
	    return err;
    }

    return SCE_HIG_NO_ERR;
}

static sceHiErr ShapeMakeShapePacket(_shape_t *sp, Tex2d *tex2d, int *id)
{
    int		i;
    sceHiErr	err;

    sp->data[0][0] = Vumem1ISize();
    sp->data[0][1] = sp->data[0][2] = sp->data[0][3] = 0;
    sp->data[1][0] = 0xdeaddead;
    sp->data[1][1] = sp->data[1][2] = sp->data[1][3] = 0;

    if ((err = sceHiDMAMake_ChainStart()) != SCE_HIG_NO_ERR)			return err;

    /* PACKET:send data to VU */
    if ((err = sceHiDMAMake_LoadPtr((u_int *)10, (u_int *)sp->data, 2)) != SCE_HIG_NO_ERR)	return err;

    /* PACKET:calc view volume */
    if ((err = sceHiDMAMake_ExecMicro()) != SCE_HIG_NO_ERR)				return err;
    if ((err = sceHiDMAMake_ContinueMicro()) != SCE_HIG_NO_ERR)			return err;

    /* PACKET: send each material */
    for (i = 0; i < sp->material_num; i++) {
	err = ShapeMakeMaterialPacket(&(sp->materials[i]), tex2d);
	if (err != SCE_HIG_NO_ERR)
	    return err;
    }
    if ((err = sceHiDMAMake_ChainEnd(id)) != SCE_HIG_NO_ERR)		return err;

    return SCE_HIG_NO_ERR;
}

static sceHiErr ShapeFrameMakeSubPacket(SHAPE_FRAME *sf, Tex2d *tex2d)
{
    int i;
    sceHiErr	err;

    sf->tex2d = tex2d;
    sf->id = (int *)sceHiMemAlign(16, sizeof(int) * sf->shape_num);
    if (sf->id == NULL)
	return _hip_shape_err(_NO_HEAP);

    for (i = 0; i < sf->shape_num; i++) {
	err = ShapeMakeShapePacket(&(sf->shape[i]), tex2d, &(sf->id[i]));
	if (err != SCE_HIG_NO_ERR)
	    return err;
    }
    return SCE_HIG_NO_ERR;
}

static sceHiErr ShapeFrameMakePacket(SHAPE_FRAME *sf)
{
    int	i;
    sceHiErr err;
    Matrix_t	*daddr = sf->matdata;

    /* 全objectを統括する Packet列生成 */
    sceHiDMAMake_DynamicChainStart();
    _gs_set[0].ul[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 1);
    _gs_set[0].ul[1] = (long)SCE_GIF_PACKED_AD;
    _gs_set[1].ul[0] = 0;	/* PRMODCONT = 0 */
    _gs_set[1].ul[1] = (long)SCE_GS_PRMODECONT;
    if ((err = sceHiDMAMake_LoadGS((u_int *)_gs_set, 2)))
	return err;
    /* Matrixの埋め込み&object ID call */
    for (i = 0; i < sf->matrix_num; i++) {
	/* 先頭の第１ビット目で visible/invisible */
	/* ただしマトリクスデータを持たないものは適用不可 */
	if (daddr == NULL || (!(daddr->flags & 0x1))) {

	    if (sf->matrixs[i].shape_id != -1) {
		/*	shape packet	*/
		if ((err = sceHiDMAMake_LoadPtr((u_int *)0, (u_int *)sf->matrixs[i].data, 8)) != SCE_HIG_NO_ERR)	return err;
		if ((err = sceHiDMAMake_CallID(sf->id[sf->matrixs[i].shape_id])) != SCE_HIG_NO_ERR) 		return err;
	    }
	}
	if (daddr != NULL)
	    daddr++;
    }

    /* 最後に PRMODCONT = 1にしておかなければならない・・・ */
    if ((err = sceHiDMAMake_WaitMicro()))	return err;
    _gs_set[2].ul[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 1);
    _gs_set[2].ul[1] = (long)SCE_GIF_PACKED_AD;
    _gs_set[3].ul[0] = 1;	/* PRMODCONT = 1 */
    _gs_set[3].ul[1] = (long)SCE_GS_PRMODECONT;
    if ((err = sceHiDMAMake_LoadGS((u_int *)&(_gs_set[2]), 2)))
	return err;

    if ((err = sceHiDMAMake_DynamicChainEnd()))
	return err;

    return SCE_HIG_NO_ERR;
}

static sceHiErr ShapeFrameMakePacketInStatic(SHAPE_FRAME *sf)
{
    int i;
    sceHiErr err;
    Matrix_t *daddr = sf->matdata;

    sceHiDMAMake_ChainStart();
    _gs_set[0].ul[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 1);
    _gs_set[0].ul[1] = (long) SCE_GIF_PACKED_AD;
    _gs_set[1].ul[0] = 0;
    _gs_set[1].ul[1] = (long) SCE_GS_PRMODECONT;
    if ((err = sceHiDMAMake_LoadGS((u_int *)_gs_set, 2)))
	return err;
    for (i = 0; i < sf->matrix_num; i++) {
	if (daddr == NULL || (!(daddr->flags & MASTER_CHAIN_IN_STATIC_O))) {

	    if (sf->matrixs[i].shape_id != -1) {
		if ((err = sceHiDMAMake_LoadPtr((u_int *)0, (u_int *)sf->matrixs[i].data, 8)) != SCE_HIG_NO_ERR)
		    return err;
		if ((err = sceHiDMAMake_CallID(sf->id[sf->matrixs[i].shape_id])) != SCE_HIG_NO_ERR)
		    return err;
	    }
	}
	if (daddr != NULL)
	    daddr++;
    }

    if ((err = sceHiDMAMake_WaitMicro()))
	return err;
    _gs_set[2].ul[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 1);
    _gs_set[2].ul[1] = (long) SCE_GIF_PACKED_AD;
    _gs_set[3].ul[0] = 1;
    _gs_set[3].ul[1] = (long) SCE_GS_PRMODECONT;
    if ((err = sceHiDMAMake_LoadGS((u_int *)&(_gs_set[2]), 2)))
	return err;
    if ((err = sceHiDMAMake_ChainEnd(&sf->my_id)))
	return err;
    return SCE_HIG_NO_ERR;
}

/************************************************/
/*		Shape Plug			*/
/************************************************/
sceHiErr sceHiPlugShape(sceHiPlug *plug, int process)
{
    sceHiType	type;
    u_int	*data;
    sceHiErr	err;
    SHAPE_FRAME	*shape_frame;
    int		i, j;
    Tex2d	tex2d;
    u_int	*mat;

    switch(process){
      case SCE_HIG_INIT_PROCESS:
	type.repository = SCE_HIP_COMMON;
	type.project = SCE_HIP_FRAMEWORK;
	type.category = SCE_HIP_SHAPE;
	type.id = SCE_HIP_SHAPE_DATA;
	type.status = SCE_HIG_DATA_STATUS;
	type.revision = SCE_HIP_REVISION;

	{
		u_int bufsize = (1024 - 120) / 2;
		u_int isize_per_vtx = 4;
		u_int osize_per_vtx = 5;
		u_int vnum, isize, osize;

		vnum = bufsize / (isize_per_vtx + osize_per_vtx);
		osize = vnum * osize_per_vtx;
		isize = bufsize - osize;
		Vumem1SetParam(isize, osize);
	}
	
	err = sceHiGetData(plug, &data, type);	if(err != SCE_HIG_NO_ERR) return err;

	shape_frame = (SHAPE_FRAME *)sceHiMemAlign(16, sizeof(SHAPE_FRAME)*1);
	if(shape_frame == NULL) return _hip_shape_err(_NO_HEAP);
	shape_frame->flags = 0;
	shape_frame->shape = NULL;
	shape_frame->shape_num = 0;
	_decode_shape_data(shape_frame, (u_int *)data);

	type.id = SCE_HIP_BASEMATRIX;
	err = sceHiGetData(plug, &data, type);
	if(err == SCE_HIG_NO_ERR)	mat = (u_int *)data;
	else			mat = NULL;
	_decode_basematrix_data(shape_frame, mat);

	type.category = SCE_HIP_TEX2D;
	type.id = SCE_HIP_TEX2D_DATA;
	err = sceHiGetData(plug, &data, type);
	if(err == SCE_HIG_NO_ERR)	tex2d.data = (u_int *)data;
	else			tex2d.data = NULL;

	type.id = SCE_HIP_TEX2D_ENV;
	err = sceHiGetData(plug, &data, type);
	if(err == SCE_HIG_NO_ERR)	tex2d.env = (u_int *)data;
	else			tex2d.env = NULL;

	err = ShapeFrameMakeSubPacket(shape_frame, &tex2d);
	if(err != SCE_HIG_NO_ERR) return err;

	if (shape_frame->flags & MASTER_CHAIN_IN_STATIC_O)
	    err = ShapeFrameMakePacketInStatic(shape_frame);

	if(err != SCE_HIG_NO_ERR) return err;

	plug->stack = (u_int)shape_frame;	/* push stack */
	break;

      case SCE_HIG_PRE_PROCESS:
	break;

      case SCE_HIG_POST_PROCESS:
	shape_frame = (SHAPE_FRAME *)plug->stack;	/* pop stack */
	if(shape_frame == NULL) return _hip_shape_err(_PLGBLK_STACK_BROKEN);

	if (!(shape_frame->flags & MASTER_CHAIN_IN_STATIC_O))
	    /* dynamic send */
	    err = ShapeFrameMakePacket(shape_frame);
	else
	    sceHiDMARegist(shape_frame->my_id);
	break;

      case SCE_HIG_END_PROCESS:
	shape_frame = (SHAPE_FRAME *)plug->stack;	/* pop stack */
	if(shape_frame == NULL) return _hip_shape_err(_PLGBLK_STACK_BROKEN);

	/* chain delete */
	for (i = 0; i < shape_frame->shape_num; i++)
		sceHiDMADel_Chain(shape_frame->id[i]);
	if (shape_frame->flags & MASTER_CHAIN_IN_STATIC_O)
	    sceHiDMADel_Chain(shape_frame->my_id);

	/* memory free */
	sceHiMemFree((u_int *)Paddr(shape_frame->matrixs));
	for (i = 0; i < shape_frame->shape_num; i++) {
	    for (j = 0; j < shape_frame->shape[i].material_num; j++) {
		sceHiMemFree((u_int *)Paddr(shape_frame->shape[i].materials[j].geometries));
	    }
	    sceHiMemFree((u_int *)Paddr(shape_frame->shape[i].materials));
	}
	sceHiMemFree((u_int *)Paddr(shape_frame->id));
	sceHiMemFree((u_int *)Paddr(shape_frame->shape));
	sceHiMemFree((u_int *)Paddr(shape_frame));

	plug->stack = NULL;
	plug->args = NULL;
	break;
      default:
	break;
    }

    return SCE_HIG_NO_ERR;
}

/*
 * matrixID  matidxに対応したシェープの表示・非表示設定をする
 */
sceHiErr sceHiPlugShapeInvisible(sceHiPlug *plug, int matidx, int flag)
{
    sceHiErr err;
    SHAPE_FRAME	*sf;

    sf = (SHAPE_FRAME *)plug->stack;
    if (sf == NULL)
	return _hip_shape_err(_PLGBLK_STACK_BROKEN);

    /* change flags in Matrix-Data */
    if (sf->matrix_num <= matidx)
	return _hip_shape_err(_MATIDX_IS_ILLEAGAL);
    flag = ((flag & 0x0001) << 0);
    sf->matdata[matidx].flags &= (~1);
    sf->matdata[matidx].flags |= flag;

    /* if the master chain is static, re-make it */
    if (sf->flags & MASTER_CHAIN_IN_STATIC_O) {
	/* delete it */
	if ((err = sceHiDMADel_Chain(sf->my_id)) != SCE_HIG_NO_ERR)
	    return err;
	/* re-make it */
	if ((err = ShapeFrameMakePacketInStatic(sf)) != SCE_HIG_NO_ERR)
	    return err;
    }

    return SCE_HIG_NO_ERR;
}

/*
 * ShapeパケットのMaster ChainのDynamic/Static切換え等設定をする
 *
 * NOTE: DON'T CALL this function AFTER PRE_PROCESS.
 * -- sequence
 *	sceHiCallPlug(plug, INIT_PROCESS)
 *	.
 *	.
 *	while (1) { // main-loop
 *		.
 *		.
 *	    sceHiPlugShapeMasterChainSetting()	// this function
 *		.
 *		.
 *	    sceHiCallPlug(plug, PRE_PROCESS)
 *	    sceHiCallPlug(plug, POST_PROCESS)
 *	    sceHiDMASend();
 *		.
 *		.
 *	}
 * -- illeagal
 *	while (1) {
 *	    sceHiCallPlug(plug, PRE_PROCESS)
 *	    sceHiCallPlug(plug, POST_PROCESS)
 *	    sceHiPlugShapeMasterChainSetting()	// this function
 *	    sceHiDMASend();
 *	}
 *
 */
sceHiErr sceHiPlugShapeMasterChainSetting(sceHiPlug *plug, int flag)
{
    /* flag ... 0 bit : 0 then Dynamic, 1 then Static */
    sceHiErr err;
    SHAPE_FRAME *sf;

    sf = (SHAPE_FRAME *)plug->stack;
    if (sf == NULL)
	return _hip_shape_err(_PLGBLK_STACK_BROKEN);

    /* check flags */
    if ((sf->flags & MASTER_CHAIN_IN_STATIC_O) != (flag & MASTER_CHAIN_IN_STATIC_O)) {
	if (flag & MASTER_CHAIN_IN_STATIC_O) {
	    /* dynamic -> static */
	    /* note : NOW, none master chain here. just need to make master chain in static */
	    if ((err = ShapeFrameMakePacketInStatic(sf)) != SCE_HIG_NO_ERR)
		return err;
	} else {
	    /* static -> dynamic */
	    /* note : need to delete static chain. the master chain will be made later in POST process*/
	    if ((err = sceHiDMADel_Chain(sf->my_id)) != SCE_HIG_NO_ERR)
		return err;
	}
    }

    /* copy flag */
    sf->flags = flag;

    return SCE_HIG_NO_ERR;
}
