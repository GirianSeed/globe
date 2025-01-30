/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 */

#include <libvu0.h>
extern void sceVu0RotCameraMatrix(sceVu0FMATRIX m, sceVu0FVECTOR p, sceVu0FVECTOR zd,
				  sceVu0FVECTOR yd, sceVu0FVECTOR rot);



extern sceVu0FVECTOR camera_p;
extern sceVu0FVECTOR camera_zd;
extern sceVu0FVECTOR camera_yd;
extern sceVu0FVECTOR camera_rot;
extern sceVu0FVECTOR light0;
extern sceVu0FVECTOR light1;
extern sceVu0FVECTOR light2;

extern sceVu0FVECTOR color0 ;
extern sceVu0FVECTOR color1 ;
extern sceVu0FVECTOR color2 ;

extern sceVu0FVECTOR ambient;

extern sceVu0FVECTOR obj_trans;
extern sceVu0FVECTOR obj_rot;

extern void cal_matrix(void);
