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
 *                          Name : tex2d.h
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *             2.1     Nov,17,2000
 */

#ifndef _TEX2D_H_
#define _TEX2D_H_

extern fwTex2dFrame_t* fwTex2dInit(fwDataHead_t *);
extern void fwTex2dPreCalc(fwTex2dFrame_t *);
extern void fwTex2dPostCalc(fwTex2dFrame_t *);
extern void fwTex2dFinish(fwTex2dFrame_t *);

#endif /* ! _TEX2D_H_ */
