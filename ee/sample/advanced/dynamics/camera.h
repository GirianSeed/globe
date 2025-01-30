/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
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

extern void perspective(sceHiPlugMicroData_t *md, sceVu0FVECTOR scr, sceVu0FVECTOR win, sceVu0FVECTOR dep);
extern void viewset(sceVu0FVECTOR pos, sceVu0FVECTOR rot, sceVu0FVECTOR tag, sceVu0FVECTOR up);
extern void viewget(sceVu0FVECTOR pos, sceVu0FVECTOR rot, sceVu0FVECTOR tag, sceVu0FVECTOR up);
extern void cameractrl(void);
extern void cameraview(sceHiPlugMicroData_t *md);
extern void camerainit(sceHiPlugMicroData_t *md);
extern void lookatview(sceHiPlugMicroData_t *md);
extern void pilotview(sceHiPlugMicroData_t *md);
extern void polarview(sceHiPlugMicroData_t *md);

#ifdef __cplusplus
}
#endif
