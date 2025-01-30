/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: mathfunc.h,v 1.1 2002/10/18 12:26:13 aoki Exp $
 */
/* 
 * Emotion Engine Library Sample Program
 *
 * Copyright (C) 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * scis_at - mathfunc.h
 *	
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2002-10-08      aoki        
 */
#include <libvu0.h>

#ifdef __cplusplus
extern "C" {
#endif
void sceVu0RotCameraMatrix(sceVu0FMATRIX m, sceVu0FVECTOR p, sceVu0FVECTOR zd,
				  sceVu0FVECTOR yd, sceVu0FVECTOR rot);
void cal_matrix(void);


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


#ifdef __cplusplus
}
#endif
