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
 *                          Name : model.h
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            0.40      Jun,21,2000    kaneko
 */

#ifndef _MODEL_H_
#define _MODEL_H_

#define MAX_MODEL	10
typedef u_int	fwModelID_t;

extern void fwModelInit(void);
extern void fwModelCtrl(void);
extern void fwModelExit(void);
extern int fwModelIsActive(fwModelID_t);
extern fwObject_t* fwModelGetDataAddr(fwModelID_t);

#endif /* _MODEL_H_ */
