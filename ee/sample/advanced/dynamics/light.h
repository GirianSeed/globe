/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
*/
/*
 *
 *	Copyright (C) 2002 Sony Computer Entertainment Inc.
 *							All Right Reserved
 *
 */
#ifdef __cplusplus
extern "C" {
#endif

extern void lgpini(void);
extern void lgpreg(void);
extern void lgpdel(void);

extern void lightload(sceHiPlugMicroData_t *md, sceVu0FVECTOR dir, sceVu0FVECTOR pos, sceVu0FVECTOR col, int lt, int ln);
extern void lightstore(sceHiPlugMicroData_t *md, sceVu0FVECTOR dir, sceVu0FVECTOR pos, sceVu0FVECTOR col, int lt, int ln);
extern void lightcopy(sceHiPlugMicroData_t *dst, sceHiPlugMicroData_t *src, int lt);
extern void lightctrl(sceHiPlugMicroData_t *md, int lt);
extern void lightinit(sceHiPlugMicroData_t *md);

enum {DIR_LIGHT,POINT_LIGHT,SPOT_LIGHT,MAX_LIGHT};

#ifdef __cplusplus
}
#endif
