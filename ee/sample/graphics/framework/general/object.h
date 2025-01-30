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
 *                          Name : object.h
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000
 *
 */             

#ifndef _OBJECT_H_
#define _OBJECT_H_

typedef struct {
    fwDataHead_t 	*data;
    fwMicroFrame_t	*micro;
    fwShapeFrame_t	*shape;
    fwTex2dFrame_t	*tex2d;
    fwHrchyFrame_t	*hrchy;
    fwAnimeFrame_t	*anime;
    fwShareFrame_t	*share;
} fwObject_t;

extern fwPackbuf_t	*fwObjectPBufVal;

extern fwObject_t* fwObjectCreate(fwDataHead_t *);
extern void fwObjectDelete(fwObject_t *);
extern void fwObjectPreCalc(fwObject_t *);
extern void fwObjectPostCalc(fwObject_t *);

#endif /* !_OBJECT_H_*/
