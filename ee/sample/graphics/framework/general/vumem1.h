/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *               Graphics Framework Sample Program
 *                  
 *              ---- Scene Viewer Version 0.40 ----
 * 
 *      Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          Name : vumem1.h
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            0.40      Jun,21,2000    kaneko
 */
#ifndef _VUMEM1_H_

#define  VUMEM1_OVERFLOW 1
#define  VUMEM1_OK 0

extern void fwVumem1SetParam(u_int isize, u_int osize);
extern void fwVumem1Reset();
extern int fwVumem1CheckRest(u_int isize, u_int osize);
extern int fwVumem1Rest(int ioffset, int i_per_vertex, int ooffset, int o_per_vertex);
extern int fwVumem1IRest();
extern int fwVumem1ORest();
extern u_int fwVumem1GetIptr(u_int isize);
extern u_int fwVumem1GetOptr(u_int osize);
extern int fwVumem1ISize();
extern int fwVumem1OSize();

#endif //_VUMEM1_H_
