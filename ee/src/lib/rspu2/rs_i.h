/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/* 
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                       librspu2 - rspu2cmd.h
 *                    internal headder for librspu2
 *
 *   Version   Date            Design    Log
 *  --------------------------------------------------------------------
 *   0.30      Jun.17.1999     morita    first checked in
 */

typedef struct {
    int		mode;	
    int		voice_bit;
    int		status;	
    int		opt;
} SpuEECBData;

//--- magic number
#define sce_SPU_DEV   0x8011
#define sce_SPUST_DEV 0x8012
#define sce_SPUST_CB  0x8013

//#define PRINTF(x) printf x
#define PRINTF(x) 
