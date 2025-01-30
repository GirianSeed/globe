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
 *                          Name : light.h
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000
 *
 */             
#ifndef _LIGHT_H_
#define _LIGHT_H_

typedef enum {
    FW_LGROUP_0	= 0,		/* 現在 並行光源固定 */
    FW_LGROUP_1 = 1,		/* 現在 点光源固定 */
    FW_LGROUP_2 = 2,		/* 現在 スポットライト固定 */
    FW_LGROUP_MAX = 3
} fwLightGroup_t;

typedef enum {
    FW_LIGHT_0	= 0,
    FW_LIGHT_1	= 1,
    FW_LIGHT_2	= 2,		/* 並行光源の場合 環境光 */
    FW_LIGHT_MAX = 3
} fwLightNumber_t;

typedef struct {
    sceVu0FVECTOR	color;		/* R/G/B */
    sceVu0FVECTOR	vector;		/* X/Y/Z */
    sceVu0FVECTOR	point;		/* X/Y/Z */
    float		intensity;	/* 強さ*/
    float		angle;		/* Spot LightのAngle */
} fwLight_t;

extern fwLight_t	fwLightVal[FW_LGROUP_MAX][FW_LIGHT_MAX];
extern void fwLightInit();
extern void fwLightCtrl(void);
extern void fwLightApply(fwObject_t *);
extern void fwLightSet(fwLightGroup_t, fwLightNumber_t, fwLight_t *);
extern void fwLightOn(fwLightGroup_t, fwLightNumber_t);
extern void fwLightOff(fwLightGroup_t, fwLightNumber_t);

#endif /* !_LIGHT_H_*/

