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
 *                          Name : gmain.h
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            0.30      Apr,4,2000     thatake     Beta0
 *            0.40      Jun,16,2000     	   +animation, clut
 *            2.1       Nov,17,2000
 */

#ifndef _GMAIN_H_
#define _GMAIN_H_

extern fwObject_t *fwMainObjectVal;

extern fwLightNumber_t	fwMainParaLNumber;
extern fwLightNumber_t	fwMainSpotLNumber;
extern fwLightNumber_t	fwMainPointLNumber;
extern fwLightGroup_t	fwMainParaLGroup;
extern fwLightGroup_t	fwMainSpotLGroup;
extern fwLightGroup_t	fwMainPointLGroup;

/* global functions in gmain.c */
extern void fwInit(void);
extern void fwMain(u_int  frame);
extern void fwFinish();
extern void fwSync(u_int frame, int odev);
extern fwObject_t* fwMainReadModel(fwDataHead_t *data);
extern void DrawStrings(int x, int y, char *fmt, ...);

#endif /* !_GMAIN_H_ */
