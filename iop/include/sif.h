/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/*
 *                     I/O Processor Library 
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        sif.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.4.0
 */

#ifndef _SIF_H_DEFS
#define _SIF_H_DEFS

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

/* flag bit asigned */
#define SIF_INIT	(1 << 0)
#define SIF_INIT2	(1 << 1)
#define SIF_CMD_FLG	(1 << 1)

#define SIF_DBG_1	(1 << 2)
#define SIF_DBG_2	(1 << 3)
#define SIF_DBG_3	(1 << 4)

/* send direction & mode */
#define SIF_FROM_IO	0x0
#define SIF_TO_IO	0x1
#define SIF_FROM_IOP	0x0
#define SIF_TO_IOP	0x1
#define SIF_FROM_EE	0x0
#define SIF_TO_EE	0x1

#define SIF_DMA_INT	0x2
#define SIF_DMA_INT_I	0x2
#define SIF_DMA_INT_O	0x4

#define SIF_DMA_SPR	0x8

#define SIF_DMA_BSN	0x10
#define SIF_DMA_TAG	0x20

/* for ELF loader */
extern void sceSifInitS(void);	 /* IOP only */
extern void sceSifAcceptData(void); /* EE only */

extern void sceSifInit(void);
extern int  sceSifCheckInit(void);  /* IOP only */

extern void sceSifSetDChain(void);

typedef struct {
	unsigned int	data;
	unsigned int	addr;
	unsigned int	size;
	unsigned int	mode;
} sceSifDmaData;

extern unsigned int sceSifSetDma(sceSifDmaData *sdd, int len);
extern int sceSifDmaStat(unsigned int id);


#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif /* SIF_H_DEFS */

/* End of File */
