/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4.2
 */
/*
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: tex2d.c,v 1.14 2001/05/21 05:53:52 kaneko Exp $	*/
#include <eekernel.h>
#include <stdlib.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>

#define PADDR_MASK 0x0fffffff
#define Paddr(x) ((u_int)(x)&PADDR_MASK)

#define QWSIZE	4
#define NUMIDX	3
#define INIT_TBP 0x1a40		/* default tbp for safety */
#define INIT_CBP 0x4000		/* default cbp for safety */
#define RESIDENT 2		/* first texture transfer flag */

#define SET_QW(qw, three, two, one, zero)  \
        (qw)[3]=three; (qw)[2]=two; (qw)[1]=one; (qw)[0]=zero;

/************************************************/
/*		Structure			*/
/************************************************/
typedef struct{
    qword GIFtag;
    qword Bitblt;
    qword Trxpos;
    qword Trxreg;
    qword Trxdir;
} TextTransRegs __attribute__ ((aligned(16)));

typedef struct _texflush{
    qword GIFtexflush;		/* flush giftag */
    qword Texflush;		/* flush regs */
}Texflush __attribute__ ((aligned(16)));

typedef struct{
    TextTransRegs texTransRegs;	/* texel transfer gs regs */
    TextTransRegs clutTransRegs;/* clut transfer gs regs */
    qword texGIFtag;		/* texel giftag */
    qword clutGIFtag;		/* clut giftag */
    u_int *texpData;		/* texel data ptr */
    u_int texBp;		/* TBP (texel base addr at GS) */
    u_int texGsSize;		/* blocks (aligned page break) */
    u_int texTransSize; 	/* qwords */
    u_int *clutpData;		/* clut data ptr */
    u_int clutBp;		/* CBP (clut base addr at GS) */
    u_int clutGsSize;		/* blocks (aligned page break) */
    u_int clutTransSize;	/* qwords */
}Image __attribute__ ((aligned(16)));

typedef struct{
    int 	num;		/* num of texture image */
    Image 	*image;		/* upper image array */
    int		resident;	/* resident flag */
    int		id;		/* pbuf id */
    sceHiGsMemTbl	*tbl;	/* resident auto mem */
}TEXTURE_FRAME;

/***************************************************
 * Local Errors
 ***************************************************/
typedef enum {
    _NO_HEAP,
    _PLGBLK_STACK_BROKEN,
    _NO_GS_HEAP,
} _local_err;

static const sceHiErr _local2higerr[] = {
    SCE_HIG_NO_HEAP,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_NO_HEAP,
};

static const char *_tex2d_err_mes[] = {
    "HiP Tex2d : can't malloc HiG heap\n",
    "HiP Tex2d : plugin stack is null\n",
    "HiP Tex2d : can't malloc Gs heap\n"
};

/* error handling */
static sceHiErr _hip_tex2d_err(_local_err err)
{
    sceHiErrState.mes = _tex2d_err_mes[err];
    return _local2higerr[err];
}

/************************************************/
/*	       	Packed from Gs addr		*/
/************************************************/
static u_long *getPackedData(sceHiGsPacked *p, u_char addr)
{
    int i,n;

    n = p->giftag->nloop;

    for(i=0;i<n;i++){
	if(addr == p->packed[i].addr)
	    return (u_long *)&(p->packed[i]);
    }

    return NULL;
}

/************************************************/
/*		Texture Load			*/
/************************************************/
static sceHiErr Vu1TexInit(TEXTURE_FRAME *tex, u_int *data, u_int *env, sceHiGsMemTbl *tbl)
{
    int i;
    Image *image;
    u_int *currR = data;
    u_int texWidth, texHeight;
    u_int psm;
    u_int tbw;
    u_int clutWidth, clutHeight;
    u_int cpsm=0;
    u_int cbw=0;
    sceGsTex0	*tex01 = NULL;
    u_long	*addr = NULL;
    sceHiGsPacked	pkd = {NULL, NULL};
    u_int		miplevel = 0;

    u_int		tbp,cbp;

    tex->num = *(currR + NUMIDX);
    tex->image = (Image *)sceHiMemAlign(16, sizeof(Image)*tex->num);
    if(tex->image == NULL){
	return _hip_tex2d_err(_NO_HEAP);
    }
    image = tex->image;

    currR += QWSIZE;

    if(env != NULL){
	pkd.giftag = (sceHiGsGiftag *)(env + QWSIZE);
	pkd.packed = (sceHiGsPacked_t *)(env + QWSIZE*2);
    }
    else{
	pkd.giftag = NULL;
	pkd.packed = NULL;
    }

    /*	Base Pointer	*/
    if(tbl != NULL){
	tbp = tbl->addr/SCE_HIGS_BLOCK_ALIGN;
	cbp = (tbl->addr+tbl->size)/SCE_HIGS_BLOCK_ALIGN;
    }
    else{
	tbp = INIT_TBP;
	cbp = INIT_CBP;
    }

    for(i=0;i<tex->num;i++){

	miplevel = *(currR + NUMIDX);
	if((i)&&(pkd.giftag)&&(!miplevel)){
	    pkd.giftag = (sceHiGsGiftag *)(pkd.giftag + pkd.giftag->nloop + 1);
	    pkd.packed = (sceHiGsPacked_t *)(pkd.giftag + 1);
	}

	tex01 = (sceGsTex0 *)currR;
	addr = (u_long *)(currR + 2);
	*addr = SCE_GS_TEX0_1;

	psm= tex01->PSM;

	currR+=QWSIZE;

	image[i].texTransSize= (currR[0]+3)/4;
	texHeight = currR[2] >> 16;
	texWidth = currR[2]& 0xffff;
	if ( (psm==SCE_GS_PSMT8) || (psm==SCE_GS_PSMT4)){
	    tbw= 2*((texWidth+127)/128);
	} else{
	    tbw= (texWidth+63)/64;
	}
	image[i].texGsSize = sceHiGsPageSize(texWidth, texHeight, psm)/SCE_HIGS_BLOCK_ALIGN;
	image[i].texBp = tbp;
	tbp += image[i].texGsSize;
	if(tbp > cbp) tbp = cbp;

	tex01->TBP0 = image[i].texBp;
	tex01->TBW = tbw;

	image[i].clutTransSize = (currR[1]+3)/4;

	if (image[i].clutTransSize){
	    cpsm= tex01->CPSM;
	    clutHeight = currR[3] >> 16;
	    clutWidth = currR[3]& 0xffff;
	    cbw=1;
	    image[i].clutGsSize = sceHiGsBlockSize(clutWidth, clutHeight, cpsm)/SCE_HIGS_BLOCK_ALIGN;
	    cbp -= image[i].clutGsSize;
	    if(cbp < tbp) cbp = tbp;
	    image[i].clutBp = cbp;

	    tex01->CBP = image[i].clutBp;
	    tex01->CLD = 2;
	} else{
	    clutWidth = 0;
	    clutHeight = 0;
	    image[i].clutGsSize=0;    
	}

	if(pkd.giftag){
	    sceGsTex0		*tex0 = NULL;
	    sceGsMiptbp1	*tbp1 = NULL;
	    sceGsMiptbp2	*tbp2 = NULL;

	    switch(miplevel){
	      case 0:
		  tex0 = (sceGsTex0 *)getPackedData(&pkd, SCE_GS_TEX0_1);
		  if(tex0 != NULL){
		      tex0->TBP0 = tex01->TBP0;
		      tex0->TBW = tex01->TBW;
		      tex0->CBP = tex01->CBP;
		      tex0->CLD = tex01->CLD;
		  }
		  break;

	      case 1:
		  tbp1 = (sceGsMiptbp1 *)getPackedData(&pkd, SCE_GS_MIPTBP1_1);
		  if(tbp1 != NULL){
		      tbp1->TBP1 = tex01->TBP0;
		      tbp1->TBW1 = tex01->TBW;
		  }
		  break;
	      case 2:
		  tbp1 = (sceGsMiptbp1 *)getPackedData(&pkd, SCE_GS_MIPTBP1_1);
		  if(tbp1 != NULL){
		      tbp1->TBP2 = tex01->TBP0;
		      tbp1->TBW2 = tex01->TBW;
		  }
		  break;
	      case 3:
		  tbp1 = (sceGsMiptbp1 *)getPackedData(&pkd, SCE_GS_MIPTBP1_1);
		  if(tbp1 != NULL){
		      tbp1->TBP3 = tex01->TBP0;
		      tbp1->TBW3 = tex01->TBW;
		  }
		  break;
	      case 4:
		  tbp2 = (sceGsMiptbp2 *)getPackedData(&pkd, SCE_GS_MIPTBP2_1);
		  if(tbp2 != NULL){
		      tbp2->TBP4 = tex01->TBP0;
		      tbp2->TBW4 = tex01->TBW;
		  }
		  break;
	      case 5:
		  tbp2 = (sceGsMiptbp2 *)getPackedData(&pkd, SCE_GS_MIPTBP2_1);
		  if(tbp2 != NULL){
		      tbp2->TBP5 = tex01->TBP0;
		      tbp2->TBW5 = tex01->TBW;
		  }
		  break;
	      case 6:
		  tbp2 = (sceGsMiptbp2 *)getPackedData(&pkd, SCE_GS_MIPTBP2_1);
		  if(tbp2 != NULL){
		      tbp2->TBP6 = tex01->TBP0;
		      tbp2->TBW6 = tex01->TBW;
		  }
		  break;
	    }
	}

	/* GIFtag and GSregs for texel host->local transfer */
	SET_QW(image[i].texTransRegs.GIFtag, 0x0, 0x0000000e, 0x10000000, 0x00008004 );
	SET_QW(image[i].texTransRegs.Bitblt, 0x0, 0x00000050, 
	       ((psm<<(GS_BITBLTBUF_DPSM_O-32))
		|(tbw<<(GS_BITBLTBUF_DBW_O-32))
		| (image[i].texBp<<(GS_BITBLTBUF_DBP_O-32))),
	       0x00000000 );
	SET_QW(image[i].texTransRegs.Trxpos, 0x0, 0x00000051, 0x00000000, 0x00000000 );
	SET_QW(image[i].texTransRegs.Trxreg, 0x0, 0x00000052, texHeight, texWidth);
	SET_QW(image[i].texTransRegs.Trxdir, 0x0, 0x00000053, 0x00000000, 0x00000000 );

	/* GIFtag and GSregs for clut host->local transfer */
	if (image[i].clutTransSize>0){
	    SET_QW(image[i].clutTransRegs.GIFtag, 0x0, 0x0000000e, 0x10000000, 0x00008004 );
	    SET_QW(image[i].clutTransRegs.Bitblt, 0x0, 0x00000050, 
		   ((cpsm<<(GS_BITBLTBUF_DPSM_O-32))
		    |(cbw<<(GS_BITBLTBUF_DBW_O-32))
		    | (image[i].clutBp<<(GS_BITBLTBUF_DBP_O-32))),
		   0x00000000 );
	    SET_QW(image[i].clutTransRegs.Trxpos, 0x0, 0x00000051, 0x00000000, 0x00000000 );
	    SET_QW(image[i].clutTransRegs.Trxreg, 0x0, 0x00000052, clutHeight, clutWidth);
	    SET_QW(image[i].clutTransRegs.Trxdir, 0x0, 0x00000053, 0x00000000, 0x00000000 );
	}

	currR += 4;

	image[i].texpData = currR;

	/* GIFtag for texel data transfer to GS */
	SET_QW(image[i].texGIFtag, 0x0, 0x0, 0x08000000, image[i].texTransSize|0x8000 );

	currR += image[i].texTransSize*4;

	/* GIFtag for clut data transfer to GS */
	if (image[i].clutTransSize){
	    image[i].clutpData = currR;
	    SET_QW(image[i].clutGIFtag, 0x0, 0x0, 0x08000000, image[i].clutTransSize|0x8000 );

	    currR += image[i].clutTransSize*4;
	}
    }

    return SCE_HIG_NO_ERR;
}

static const Texflush __attribute__ ((aligned(16))) texflush = {
    {0x00008001, 0x10000000, 0x0000000e, 0x0},
    {0x00000000, 0x00000000, 0x0000003f, 0x0}
};
/************************************************/
/*		Texture Packet			*/
/************************************************/
static u_int Vu1ResidentTextureMakePacket(TEXTURE_FRAME *tex)
{
    Image *image;
    int texID;
    int	id;

    if (tex != NULL) {
	sceHiDMAMake_ChainStart();
	for (texID = 0; texID < tex->num; texID++) {
	    image = tex->image;
	    sceHiDMAMake_WaitMicro();
	    sceHiDMAMake_LoadGS(image[texID].texTransRegs.GIFtag, 5);
	    sceHiDMAMake_LoadGS(image[texID].texGIFtag, 1);
	    sceHiDMAMake_LoadGS(image[texID].texpData, image[texID].texTransSize);
	    if (image[texID].clutTransSize > 0) {
		sceHiDMAMake_LoadGS(image[texID].clutTransRegs.GIFtag, 5);
		sceHiDMAMake_LoadGS(image[texID].clutGIFtag, 1);
		sceHiDMAMake_LoadGS(image[texID].clutpData, image[texID].clutTransSize);
	    }
	    sceHiDMAMake_LoadGS((int *)&texflush, 2);
	}
	sceHiDMAMake_ChainEnd(&id);
    }

    return id;
}

/************************************************/
/*		Tex2D Plug			*/
/************************************************/
sceHiErr sceHiPlugTex2D(sceHiPlug *plug, int process)
{
    sceHiType	type;
    u_int	*data, *env;
    sceHiErr	err;
    TEXTURE_FRAME	*tex_frame;
    sceHiGsMemTbl	*tbl = NULL;

    switch(process){
      case SCE_HIG_INIT_PROCESS:
	  type.repository = SCE_HIP_COMMON;
	  type.project = SCE_HIP_FRAMEWORK;
	  type.category = SCE_HIP_TEX2D;
	  type.status = SCE_HIG_DATA_STATUS;
	  type.revision = SCE_HIP_REVISION;
	  type.id = SCE_HIP_TEX2D_DATA;

	  err = sceHiGetData(plug, &data, type);	if(err != SCE_HIG_NO_ERR) return err;
	  type.id = SCE_HIP_TEX2D_ENV;
	  err = sceHiGetData(plug, &env, type);
	  if(err != SCE_HIG_NO_ERR)	env = NULL;

	  tex_frame = (TEXTURE_FRAME *)sceHiMemAlign(16, sizeof(TEXTURE_FRAME));
	  if (tex_frame == NULL)	return _hip_tex2d_err(_NO_HEAP);

	  if(plug->args != NULL){
	      tex_frame->resident = ((sceHiPlugTex2dInitArg_t *)plug->args)->resident ? RESIDENT : FALSE;
	      tbl = ((sceHiPlugTex2dInitArg_t *)plug->args)->tbl;
	  }
	  else{
	      tex_frame->resident = FALSE;
	      tbl = NULL;
	  }

	  if((tex_frame->resident == RESIDENT)&&(tbl == NULL)){
	      tbl = sceHiGsMemAlloc(SCE_HIGS_BLOCK_ALIGN, sceHiPlugTex2DSize(plug));
	      if(tbl == NULL)	return _hip_tex2d_err(_NO_GS_HEAP);
	      tex_frame->tbl = tbl;	/* for free */
	  }
	  else{
	      tex_frame->tbl = NULL;
	  }

	  err = Vu1TexInit(tex_frame, data, env, tbl);
	  if(err != SCE_HIG_NO_ERR) return err;
	  tex_frame->id = Vu1ResidentTextureMakePacket(tex_frame);

	  plug->stack = (u_int)tex_frame;
	  plug->args = NULL;
	  break;

      case SCE_HIG_POST_PROCESS:
	  tex_frame = (TEXTURE_FRAME *)plug->stack;
	  if(tex_frame == NULL) return _hip_tex2d_err(_PLGBLK_STACK_BROKEN);

	  if(tex_frame->resident == RESIDENT){
	      sceHiDMARegist(tex_frame->id);
	      tex_frame->resident = TRUE;
	  }
	  else if(tex_frame->resident == FALSE){
	      sceHiDMARegist(tex_frame->id);
	  }
	  break;

      case SCE_HIG_END_PROCESS:
	  tex_frame = (TEXTURE_FRAME *)plug->stack;
	  if(tex_frame == NULL) return _hip_tex2d_err(_PLGBLK_STACK_BROKEN);
	  sceHiDMADel_Chain(tex_frame->id);
	  if(tex_frame->tbl != NULL)	sceHiGsMemFree(tex_frame->tbl);
	  sceHiMemFree((u_int *)Paddr(tex_frame->image));
	  sceHiMemFree((u_int *)Paddr(tex_frame));
	  plug->stack = NULL;
	  plug->args = NULL;
	  break;

      default:
	  break;
    }

    return SCE_HIG_NO_ERR;
}

size_t sceHiPlugTex2DSize(sceHiPlug *plug)
{
    sceHiErr	err;
    sceHiType	type;
    u_int	*data;
    u_int	*curr;
    size_t	size;
    int		num,i;
    sceGsTex0	*tex0;
    u_int	w,h,psm;
    u_int	texelsize,clutsize;

    type.repository = SCE_HIP_COMMON;
    type.project = SCE_HIP_FRAMEWORK;
    type.category = SCE_HIP_TEX2D;
    type.status = SCE_HIG_DATA_STATUS;
    type.revision = SCE_HIP_REVISION;
    type.id = SCE_HIP_TEX2D_DATA;
    err = sceHiGetData(plug, &data, type);
    if(err != SCE_HIG_NO_ERR) return 0;

    curr = data;

    num = *(curr + NUMIDX);
    curr += QWSIZE;

    size = 0;
    for(i=0;i<num;i++){
	tex0 = (sceGsTex0 *)curr;
	curr += QWSIZE;
	texelsize = curr[0];
	clutsize = curr[1];
	if(texelsize){
	    w = curr[2] & 0xffff;
	    h = curr[2] >> 16;
	    psm = tex0->PSM;
	    size += sceHiGsPageSize(w,h,psm);
	}
	if(clutsize){
	    w = curr[3] & 0xffff;
	    h = curr[3] >> 16;
	    psm = tex0->CPSM;
	    size += sceHiGsBlockSize(w,h,psm);
	}
	curr += QWSIZE;
	curr += texelsize+clutsize;
    }

    return size;
}
