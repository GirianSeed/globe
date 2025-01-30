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
 *                          Name : info.h
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000  
 *
 */

#ifndef _INFO_H_
#define _INFO_H_

/***************************************************
 * type definitions
 ***************************************************/
typedef struct {
    char	*filename;
    const char	*micro_name;
    int		frame;
    int		vertNum;
    float	transTex;
    int		cpu_count;
    int		dma_count;
    char	mode[64];
} fwInfoStatus_t;

/***************************************************
 * external variable declarations
 ***************************************************/
extern fwInfoStatus_t fwInfoStatusVal;

/***************************************************
 * external function declarations
 ***************************************************/
extern void fwInfoInit(void);
extern void fwInfoExit(void);
extern void fwInfoDispStatus(void);
extern void fwInfoDraw(int, int, const char *, ...);

#endif /* !_INFO_H_ */
