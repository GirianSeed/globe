/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *        Emotion Engine Library Sample Program
 *
 *                         - libgp: filtering -
 *
 *                         Version 1.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            camera.h
 *                     simple 2D filtering w/ libgp
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            Sep,20,2001      aoki
 */

#ifndef _CAMERA_H_
#define _CAMERA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <libvu0.h>


typedef struct {
    sceVu0FVECTOR	color;		/* R/G/B */
    sceVu0FVECTOR	vector;		/* X/Y/Z */
    sceVu0FVECTOR	point;		/* X/Y/Z */
    float		intensity;	/* for Point/Spot */
    float		angle;		/* for Spot */
} _light_t;

typedef struct {
    sceVu0FVECTOR pos;
    sceVu0FVECTOR zdir;
    sceVu0FVECTOR ydir;
    sceVu0FVECTOR rot;
} CameraPos;

typedef struct{
    CameraPos cp;
    sceVu0FVECTOR clip_volume;
    sceVu0FMATRIX world_view;
    sceVu0FMATRIX view_screen;
    sceVu0FMATRIX view_clip;
    sceVu0FMATRIX world_screen;
    sceVu0FMATRIX world_clip;
    sceVu0FMATRIX clip_screen;

    /* clip data for new micro */
    float	screen_w;
    float	screen_h;
    float	near;
    float	far;
}CAMERA_DAT;

extern void light_set_one(int gp, int num, _light_t *lt);
extern void light_init(void);
extern void light_set(u_int *micro_data_addr);

extern void camera_init(void);
extern void camera_set(u_int *micro_data_addr);

extern void Vu1MicroSetWorldScreen(u_int *, sceVu0FMATRIX);
extern void Vu1MicroSetWorldClip(u_int *, sceVu0FMATRIX);
extern void Vu1MicroSetClipScreen(u_int *, sceVu0FMATRIX);
extern void Camera_SetCamera_fromCameraPos(CAMERA_DAT *cam, CameraPos *cam_pos);

extern void sceVu0RotCameraMatrix(sceVu0FMATRIX m, CameraPos *cam_pos);

extern void SetVu0ViewScreenClipMatrix(sceVu0FMATRIX m, sceVu0FMATRIX mc,
				       sceVu0FMATRIX mcs,
				       sceVu0FVECTOR clip_vol,
				       float scrz, float ax, float ay, 
				       float cx, float cy, 
				       float zmin, float zmax, 
				       float nearz, float farz);

extern void Camera_SetClipScreen(CAMERA_DAT *cam, float screen_dist, 
			  float xaspect, float yaspect, 
			  float gs_xsize, float gs_ysize,
			  float zmini, float zmax, 
			  float nearz, float farz);

extern void Camera_SetClipData(CAMERA_DAT *cam, float sw, float sh, float sn, float sf);

#define Camera_SetRotate(cam, cam_rot) \
  sceVu0CopyVector((cam)->cp.rot, cam_rot);

#define Camera_SetPosition(cam, cam_pos) \
  sceVu0CopyVector((cam)->cp.pos, cam_pos);

#define Camera_SetClipVolume(cam, clip_vol) \
  sceVu0CopyVector((cam)->clip_volume, clip_vol);

#define Camera_SetDirection(cam, cam_dir, cam_ver) \
  sceVu0CopyVector((cam)->cp.zdir, cam_dir); \
  sceVu0CopyVector((cam)->cp.ydir, cam_ver); 


#define Camera_SetMatrix(cam) \
  sceVu0RotCameraMatrix((cam)->world_view, &(cam)->cp); \
  sceVu0MulMatrix((cam)->world_screen,(cam)->view_screen,(cam)->world_view); \
  sceVu0MulMatrix((cam)->world_clip,(cam)->view_clip,(cam)->world_view);

#define Camera_SetScreen(cam, screen_dist, \
		      xaspect, yaspect, \
		      gs_xsize, gs_ysize, \
		      zmini, zmax, \
		      nearz, farz) \
  sceVu0ViewScreenMatrix((cam)->view_screen, \
			   screen_dist, xaspect, yaspect, \
			   gs_xsize, gs_ysize, zmini, zmax, \
			   nearz, farz); 

#ifdef __cplusplus
}
#endif

#endif /* !_CAMERA_H_ */
