/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/* 
 *                      Emotion Engine Library
 *                          Version 0.10
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                       libdma - dmac.h
 *                 internal definition of libdma
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.09            October.07.1998 suzu        initial
 *      0.10            March.25.1999   suzu        use inline function
 */


/* EE kernel */
#include <eekernel.h>
#include <eeregs.h>


/* 128bit data type */
typedef u_int qword[4];

/* pseudo divice size */
#define DSIZE	0x2000

/* DMA channel */
#define D_VIF0		0
#define D_VIF1		1
#define D_GIF		2
#define D_fromIPU	3
#define D_toIPU		4
#define D_SIF0		5
#define D_SIF1		6
#define D_SIF2		7
#define D_fromSPR	8
#define D_toSPR		9

/* d_ctrl */
#define DCTRL_DMAE		(1<<0)
#define DCTRL_RELE		(1<<1)
#define DCTRL_MFD		(3<<2)		
#define DCTRL_STS		(3<<4)		
#define DCTRL_STD		(3<<6)		
#define DCTRL_RCYC		(3<<8)

#define clearDMAE(x)	(x &= ~DCTRL_DMAE)
#define clearRELE(x)	(x &= ~DCTRL_RELE)

#define setDMAE(x)	(x |= DCTRL_DMAE)
#define setRELE(x)	(x |= DCTRL_RELE)
#define setMFD(x,v)	(x =  ((x)&~DCTRL_MFD )|((v)<<2))
#define setSTS(x,v)	(x =  ((x)&~DCTRL_STS )|((v)<<4))
#define setSTD(x,v)	(x =  ((x)&~DCTRL_STD )|((v)<<6))
#define setRCYC(x,v)	(x =  ((x)&~DCTRL_RCYC)|((v)<<8))

#define getDMAE(x)	(((x)>> 0)&0x0001)
#define getSTS(x)	(((x)>> 4)&0x0003)
#define getSTD(x)	(((x)>> 6)&0x0003)

/* d_sqwc */
#define DSQWC_SQWC		(0xffff<< 0)
#define DSQWC_TQWC		(0xffff<<16)
#define setSQWC(x,v)	(x =  ((x)&~DSQWC_SQWC)|((v)<< 0))
#define setTQWC(x,v)	(x =  ((x)&~DSQWC_TQWC)|((v)<<16))

/* pseudo DMAC */
typedef struct {
	u_int	chcr, p0[3];
	u_int	madr, p1[3];
	u_int	qwc,  p2[3];
	u_int	tadr, p3[3];
	u_int	asr0, p4[3];
	u_int	asr1, p5[3];
	u_int	p6[4];	
	u_int	p7[4];
	u_int	sadr, p8[3];
} DCH;

/* S.Chain DMATag */	
#define DT_REFE		((u_int)0x1000)
#define DT_CNT		((u_int)0x1000)
#define DT_NEXT		((u_int)0x2000)
#define DT_REF		((u_int)0x3000)
#define DT_REFS		((u_int)0x4000)
#define DT_CALL		((u_int)0x5000)
#define DT_RET		((u_int)0x6000)
#define DT_END		((u_int)0x7000)

#define DT_IREFE	(DT_REFE|0x8000)
#define DT_ICNT		(DT_CNT |0x8000)
#define DT_INEXT	(DT_NEXT|0x8000)
#define DT_IREF		(DT_REF |0x8000)
#define DT_IREFS	(DT_REFS|0x8000)
#define DT_ICALL	(DT_CALL|0x8000)
#define DT_IRET		(DT_RET |0x8000)
#define DT_IEND		(DT_END |0x8000)

/* D.Chain DMATag */	
#define DT_DSTS		0x0000
#define DT_DST		0x1000

#define DT_IDSTS	(DT_DSTS|0x8000)
#define DT_IDST		(DT_DST |0x8000)

/* CHCR register mapping */
#define DC_STR		(1<<8)		/* start bit */
#define DC_TIE		(1<<7)		/* interrupt enable */
#define DC_TTE		(1<<6)		/* tag transfer enabel */
#define DC_ASP		(3<<4)		/* stack pointer */
#define DC_MOD		(3<<2)		/* 1:normal 2:chain 3:interleave */
#define DC_DIR		(1<<0)		/* 0:from_memory 1:to_memory */
#define DC_TAG		(0xffff<<16)

#define clearSTR(x)	(x &= ~DC_STR)
#define clearTIE(x)	(x &= ~DC_TIE)
#define clearTTE(x)	(x &= ~DC_TTE)
#define clearDIR(x)	(x &= ~DC_DIR)

#define setSTR(x)	(x |= DC_STR)
#define setTIE(x)	(x |= DC_TIE)
#define setTTE(x)	(x |= DC_TTE)
#define setDIR(x)	(x |= DC_DIR)
#define setTAG(x,v)	(x =  ((x)&~DC_TAG)|((v)<<16))
#define setASP(x,v)	(x =  ((x)&~DC_ASP)|((v)<< 4))
#define setMOD(x,v)	(x =  ((x)&~DC_MOD)|((v)<< 2))
	
#define getTAGID(x)	(((x)>>16)&0x7000)
#define getTAG(x)	(((x)>>16)&0xffff)
#define getPCE(x)	(((x)>>26)&0x0003)
#define getASP(x)	(((x)>> 4)&0x0003)
#define getMOD(x)	(((x)>> 2)&0x0003)
#define getSTR(x)	(((x)>> 8)&0x0001)
#define getTIE(x)	(((x)>> 7)&0x0001)
#define getTTE(x)	(((x)>> 6)&0x0001)
#define getDIR(x)	(((x)>> 0)&0x0001)


