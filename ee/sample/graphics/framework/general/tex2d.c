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
 *                          Name : tex2d.c
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *             2.1      Nov,17,2000
 */

/*******************************************************
 * äTóv
 *	Frame WorkÇÃtexture dataä÷åWÇàµÇ§ä÷êîåQ
 *******************************************************/

#include <eekernel.h>
#include <stdio.h>
#include <libvu0.h>
#include <libgraph.h>
#include <malloc.h>

#include "defines.h"
#include "data.h"
#include "frame_t.h"
#include "info.h"

/********************************

    +-----------------------+INIT_TBP
    |  resident texture  #0 |  ^
    |                       |  |
    +-----------------------+  |
    |  resident texture  #1 |  | TEXEL_RES_SIZE
    |                       |  |
    +-----------------------+  |
    |     :                 |  |
    |     :                 |  |
    |     :                 |  v
    +-----------------------+ ---
    |  non-resident texel   |
    |                       |
    +-----------------------+ 
    |  (blank)              |
    |                       |
    +-----------------------+ (2048 word align)
    |  non-resident clut    |
    +-----------------------+
    | (blank for alignment) |
    +-----------------------+ ---  
    |     :                 |  ^
    |     :                 |  |
    +-----------------------+  |
    |   resident CLUT  #1   |  | CLUT_RES_SIZE
    +-----------------------+  |
    |   resident CLUT  #0   |  v
    +-----------------------+INIT_CBP
    ********************************/

/* initial texel base point (forward) */
#define INIT_TBP 0x1a40
/* initial CLUT base point (backward) */
#define INIT_CBP 0x4000

/* GS size for resident texture */
#define TEXEL_RES_SIZE 0x1000
#define CLUT_RES_SIZE 0x40


#define FW_GS_ALIGN_ANY (-1)
#define FW_GS_ALIGN_PAGE (1)

typedef struct _texflush{
    qword GIFtexflush;
    qword Texflush;
} _fwTex2dTexflush_t;

static int _fwTex2dInitData(fwTex2dFrame_t *, u_int *);
static u_int _fwTex2dGetClutGsSize(u_int, u_int);
static u_int _fwTex2dGetTexGsSize(u_int, u_int, u_int);
static u_int _fwTex2dGetCbp(u_int, int, int);
static u_int _fwTex2dGetTbp(u_int, int);
static inline void _fwTex2dResetCbp(void);
static inline void _fwTex2dResetTbp(void);
static qword* _fwTex2dMakeTexturePacket(fwTex2dFrame_t *tex);

static _fwTex2dTexflush_t _fwTex2dFlush = {
    {0x00008001, 0x10000000, 0x0000000e, 0x0},
    {0x00000000, 0x00000000, 0x0000003f, 0x0}
};
static u_int _fwTex2dTbpVal = INIT_TBP;
static u_int _fwTex2dCbpVal = INIT_CBP;

fwTex2dFrame_t* fwTex2dInit(fwDataHead_t *data)
{
    fwTex2dFrame_t	*tex2d_frame;

    _fwTex2dResetTbp();
    _fwTex2dResetCbp();
    tex2d_frame = (fwTex2dFrame_t *)memalign(16, sizeof(fwTex2dFrame_t));
    if (tex2d_frame == NULL)
	return NULL;

    if (!_fwTex2dInitData(tex2d_frame, data->tex2d_data))
	return NULL;
    tex2d_frame->pack = _fwTex2dMakeTexturePacket(tex2d_frame);
#if 0
    fwInfoStatusVal.transTex += tex2d_frame->TransSizeTotal;
#endif
    return tex2d_frame;
}

void fwTex2dPreCalc(fwTex2dFrame_t *tf)
{
}

void fwTex2dPostCalc(fwTex2dFrame_t *tf)
{
    fwPackbufAddCall(fwPackbufKickBufVal.pCurr, tf->pack);
    fwPackbufKickBufVal.pCurr++;
}

void fwTex2dFinish(fwTex2dFrame_t *tf)
{
#if 0
    fwInfoStatusVal.transTex -= tf->TransSizeTotal;
#endif
    free((u_int *)Paddr(tf->image));
    free((u_int *)Paddr(tf));
}

static int _fwTex2dInitData(fwTex2dFrame_t *tex, u_int *data)
{
  int i;
  fwTex2dImage_t *image;
  u_int *currR = data;
  u_int texWidth, texHeight;
  u_int psm;
  u_int tbw;
  u_int clutWidth, clutHeight;
  u_int cpsm=0;
  u_int cbw=0;
  u_int	*tex01;		/* force resident texture and tex01 register */

  tex->num = *(currR + 3);
  tex->image = (fwTex2dImage_t *)memalign(16, sizeof(fwTex2dImage_t)*tex->num);
  if(tex->image == NULL){
    return 0;
  }
  image = tex->image;
  tex->TransSizeTotal = 0.0f;

  currR += 4;

  for(i=0;i<tex->num;i++){
    
    /* read and set whether this texture is resident */
    tex01 = currR;			/* force resident texture */

    /* read image data and set GS Tex01 reg */
    image[i].Tex01[0] = currR[0];
    image[i].Tex01[1] = currR[1]; 
    image[i].Tex01[2]=  SCE_GS_TEX0_1;
    image[i].Tex01[3] = 0;
    psm= (image[i].Tex01[0]>>GS_TEX0_PSM_O) & 0x3f;

    currR+=4;

    image[i].texTransSize= (currR[0]+3)/4;
    texHeight = currR[2] >> 16;
    texWidth = currR[2]& 0xffff;
    if ( (psm==SCE_GS_PSMT8) || (psm==SCE_GS_PSMT4)){
	tbw= 2*((texWidth+127)/128);
    } else{
	tbw= (texWidth+63)/64;
    }

    image[i].texGsSize= _fwTex2dGetTexGsSize(tbw, psm, texHeight);
    image[i].texBp = _fwTex2dGetTbp(image[i].texGsSize, 1);				/* force resident */

    image[i].Tex01[0]&= ~((u_int)(GS_TEX0_TBP0_M|GS_TEX0_TBW_M));
    image[i].Tex01[0]|= (image[i].texBp << GS_TEX0_TBP0_O);
    image[i].Tex01[0]|= (tbw<< GS_TEX0_TBW_O);

    image[i].clutTransSize = (currR[1]+3)/4;

    if (image[i].clutTransSize){
	cpsm= (image[i].Tex01[1]>>(GS_TEX0_CPSM_O-32)) & 0xf;
	clutHeight = currR[3] >> 16;
	clutWidth = currR[3]& 0xffff;
	cbw=1;

	image[i].clutGsSize= _fwTex2dGetClutGsSize(psm, cpsm);
	image[i].clutBp = _fwTex2dGetCbp(image[i].clutGsSize, FW_GS_ALIGN_ANY, 1);	/* force resident */

	image[i].Tex01[1]&= ~((GS_TEX0_CBP_M|GS_TEX0_CLD_M) >> 32);
	image[i].Tex01[1]|= (image[i].clutBp<<(GS_TEX0_CBP_O-32));
	image[i].Tex01[1]|= (2<<(GS_TEX0_CLD_O-32));
    } else{
	clutWidth = 0;
	clutHeight = 0;
	image[i].clutGsSize=0;    
    }

    COPY_QW((u_int)tex01, (u_int)image[i].Tex01);      /* force tex01 register */

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

  return 1;
}

static u_int _fwTex2dGetClutGsSize(u_int psm, u_int cpsm)
{
	switch (psm) {
	  case SCE_GS_PSMCT32:
	  case SCE_GS_PSMCT24:
	  case SCE_GS_PSMCT16:
	  case SCE_GS_PSMCT16S:
		return 0;
	  case SCE_GS_PSMT8H:
	  case SCE_GS_PSMT8:
		switch (cpsm) {
		  case SCE_GS_PSMCT32:	return 4;
		  default:		return 2;
		}
		break;
	  case SCE_GS_PSMT4:
	  case SCE_GS_PSMT4HH:
	  case SCE_GS_PSMT4HL:
		return 1;
	  default:
		return 0;
	}
}

static u_int _fwTex2dGetTexGsSize(u_int bw, u_int psm, u_int height)
{
    int pixel_per_word=0;
    u_int size = 0;
    int	page_width = 0;
    int page_height = 0;

    switch (psm){
    case SCE_GS_PSMCT32: 
	page_width = 64;
	page_height = 32;
	pixel_per_word = 1;
	break;
    case SCE_GS_PSMCT24:
	page_width = 64;
	page_height = 32;
	pixel_per_word = 1;
	break;
    case SCE_GS_PSMT8H:
	page_width = 64;
	page_height = 32;
	pixel_per_word = 1;
	break;
    case SCE_GS_PSMT4HH:
	page_width = 64;
	page_height = 32;
	pixel_per_word = 1;
	break;
    case SCE_GS_PSMT4HL:
	page_width = 64;
	page_height = 32;
	pixel_per_word=1; 
	break;
    case SCE_GS_PSMCT16:
	page_width = 64;
	page_height = 64;
	pixel_per_word=2;
	break;
    case SCE_GS_PSMCT16S:
	page_width = 64;
	page_height = 64;
	pixel_per_word = 2;
	break;
    case SCE_GS_PSMT8:
	page_width = 128;
	page_height = 64;
	pixel_per_word=4;
	break;
    case SCE_GS_PSMT4:
	page_width = 128;
	page_height = 128;
	pixel_per_word=8; 
	break;
    default:
	return 0;
	break;
    }
    size = ((bw * 64) + ((bw * 64) % page_width)) * (height + (height % page_height));
    size /= pixel_per_word;
    size /= 64;		/* tbp is word/64 */

    return size;
}

static u_int _fwTex2dGetCbp(u_int gssize, int align, int resident)
{
	u_int v;

	if (resident) {
		switch (align) {
		  case FW_GS_ALIGN_ANY:
			_fwTex2dCbpVal -= gssize;
			v = _fwTex2dCbpVal;
			break;
		  case FW_GS_ALIGN_PAGE:
			_fwTex2dCbpVal -= gssize;
			_fwTex2dCbpVal = 32 * (_fwTex2dCbpVal / 32);
			v = _fwTex2dCbpVal;
			break;
		  default:
			return 0;
		}
		if (_fwTex2dCbpVal < INIT_CBP-CLUT_RES_SIZE) {
			v = INIT_CBP - CLUT_RES_SIZE;
		}
	} else {
		v = INIT_CBP - CLUT_RES_SIZE - gssize;
		v = 32 * (v / 32);
	}
	return v;
}

static u_int _fwTex2dGetTbp(u_int gssize, int resident)
{
    u_int v;
    
    if (resident){
	v = _fwTex2dTbpVal;
	_fwTex2dTbpVal += gssize;
	_fwTex2dTbpVal =32*((_fwTex2dTbpVal+31)/32);			/* page break align (2048/64=32) */

	if (_fwTex2dTbpVal>INIT_TBP+TEXEL_RES_SIZE){
/*	    ErrPrintf("getTbp: too many resident texels\n");*/
	    v=INIT_TBP+TEXEL_RES_SIZE-gssize;
	}
    } else{
	v=INIT_TBP+TEXEL_RES_SIZE;
    }

    return v;
}

static inline void _fwTex2dResetCbp(void)
{
    _fwTex2dCbpVal = INIT_CBP;
}

static inline void _fwTex2dResetTbp(void)
{
    _fwTex2dTbpVal = INIT_TBP;
}

static qword* _fwTex2dMakeTexturePacket(fwTex2dFrame_t *tex)
{
    fwTex2dImage_t	*image;
    int			texID;
    qword		*cur, *ret;

    ret = cur = fwPackbufVal.pCurr;

    if (tex != NULL) {
	for (texID = 0; texID < tex->num; texID++) {
	    image = tex->image;
	    CNT_FLUSH(*cur, 0); cur++;
	    REF_DIRECT(*cur, (u_int)image[texID].texTransRegs.GIFtag, 5); cur++;
	    REF_DIRECT(*cur, (u_int)image[texID].texGIFtag, 1); cur++;
	    REF_DIRECT(*cur, (u_int)image[texID].texpData, image[texID].texTransSize); cur++;
	    tex->TransSizeTotal += image[texID].texTransSize / 65536.0f;
	    if (image[texID].clutTransSize > 0) {
		REF_DIRECT(*cur, (u_int)image[texID].clutTransRegs.GIFtag, 5); cur++;
		REF_DIRECT(*cur, (u_int)image[texID].clutGIFtag, 1); cur++;
		REF_DIRECT(*cur, (u_int)image[texID].clutpData, image[texID].clutTransSize); cur++;
		tex->TransSizeTotal += image[texID].clutTransSize / 65536.0f;
	    }
	    REF_DIRECT(*cur, (u_int)&_fwTex2dFlush, 2); cur++;
	}
    }
    RET_NOP(*cur, 0); cur++;
    fwPackbufVal.pCurr = cur;

    return ret;
}
