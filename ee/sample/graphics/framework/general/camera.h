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
 *                          Name : camera.h
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000 
 */

#ifndef _CAMERA_H_
#define _CAMERA_H_

/*******************************************************
 * type declarations
 *******************************************************/
typedef struct {
    sceVu0FVECTOR	pos;
    sceVu0FVECTOR	zdir;
    sceVu0FVECTOR	ydir;
    sceVu0FVECTOR	rot;
} fwCameraPos_t;

typedef struct {
    fwCameraPos_t	cp;
    sceVu0FVECTOR	clip_volume;
    sceVu0FMATRIX	world_view;
    sceVu0FMATRIX	view_screen;
    sceVu0FMATRIX	view_clip;
    sceVu0FMATRIX	world_screen;
    sceVu0FMATRIX	world_clip;
    sceVu0FMATRIX	clip_screen;

    /* clip data for new micro */
    float		screen_w;	/* screen width */
    float		screen_h;	/* screen height */
    float		near;		/* near screen */
    float		far;		/* far screen */
} fwCameraData_t;

typedef struct {
    float		fovy;
    float		aspect;
    float		near;
    float		far;
    float		position[4];
    float		interest[4];
    float		up[4];
} fwCameraInfo_t;

/*******************************************************
 * Macro declarations
 *******************************************************/
#define fwCameraSet2Micro(mic)				\
		fwMicroSetWorldScreen((mic), fwCameraDataVal.world_screen); \
		fwMicroSetWorldClip((mic), fwCameraDataVal.world_clip); \
		fwMicroSetClipScreen((mic), fwCameraDataVal.clip_screen)

/*******************************************************
 * external variable declarations
 *******************************************************/
extern fwCameraData_t	fwCameraDataVal;

/*******************************************************
 * external function declarations
 *******************************************************/
extern void fwCameraInit(void);
extern void fwCameraCtrl(void);
extern void fwCameraSet(fwObject_t *);

#endif /* !_CAMERA_H_ */
