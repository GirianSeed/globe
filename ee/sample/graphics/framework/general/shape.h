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
 *                          Name : shape.h
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000      
 */

#ifndef _SHAPE_H_
#define _SHAPE_H_

#define fwShapeSetLocalWorld(shape, mat)			\
		sceVu0CopyMatrix((shape)->local_world, (mat))

#define fwShapeSetLightRot(shape, mat)				\
		sceVu0CopyMatrix((shape)->light_rot, (mat))

extern fwShapeFrame_t* fwShapeInit(fwDataHead_t *);
extern void fwShapePreCalc(fwShapeFrame_t *);
extern void fwShapePostCalc(fwShapeFrame_t *);
extern void fwShapeFinish(fwShapeFrame_t *);

#endif /* !_SHAPE_H_ */
