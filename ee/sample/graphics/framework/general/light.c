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
 *                          Name : light.c
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000  
 *
 */

#include <eekernel.h>
#include <libvu0.h>
#include <libgraph.h>
#include "defines.h"
#include "data.h"
#include "frame_t.h"
#include "micro.h"
#include "object.h"
#include "camera.h"
#include "light.h"
#include "pad.h"
#include "gmain.h"
#include "info.h"

/***************************************************
 * constants
 ***************************************************/
/* init_data for micro-code */
#define DUMMY 0.0f
#define LIGHT_CONST 400.0f			// ライトの強さ
#define S_LIGHT_ANGLE 0.12f			// ライトの広がり
#define FOG_END 93784.0f			// fog-far (screen-Z)
#define FOG_BEGIN  323388.0f			// fog near (screen-Z)
#define ANTI_PARAM 0.09f			// AA1 ON/OFF threshold

/***************************************************
 * global variable definitions
 ***************************************************/
fwLight_t	fwLightVal[FW_LGROUP_MAX][FW_LIGHT_MAX];

/***************************************************
 * static variable definitions
 ***************************************************/
static sceVu0FVECTOR _fwLightAmbientVal = { 0.3f, 0.3f, 0.3f, 0.0f };
static float	_fwLightDefaultIntensity = LIGHT_CONST;
static float	_fwLightDefaultSpotAngle = S_LIGHT_ANGLE;
static sceVu0FVECTOR	_fwLightZERO = {0.0f, 0.0f, 0.0f, 0.0f};
static fwLightNumber_t	_fwLightTarget = 0;

/***************************************************
 * static function declarations
 ***************************************************/
static void _fwLightCtrlFromPAD(void);
static void _fwLightDrawCube(sceVu0FVECTOR, sceVu0FVECTOR, sceVu0FVECTOR);

/***************************************************
 * function definitions
 ***************************************************/

/*
 * fwLightInit()
 *	Light CLASSの初期化
 *	アプリケーションで一回だけ呼出すこと
 */
void fwLightInit()
{
    int		i, j;

    for (i = 0; i < FW_LGROUP_MAX; i++) {
	for (j = 0; j < FW_LIGHT_MAX; j++) {
	    sceVu0CopyVector(fwLightVal[i][j].vector, _fwLightZERO);
	    sceVu0CopyVector(fwLightVal[i][j].color,  _fwLightZERO);
	    sceVu0CopyVector(fwLightVal[i][j].point,  _fwLightZERO);
	    fwLightVal[i][j].intensity = _fwLightDefaultIntensity;
	    fwLightVal[i][j].angle     = _fwLightDefaultSpotAngle;
	}
    }
}

/*
 * fwLightCtrl()
 *	PADによるライトのコントロール
 *	frameworkでは Light ModeのExec Functionから呼出される
 */ 
void fwLightCtrl(void)
{
    fwMicroType_t	m;

    m = fwMicroGetCurrent();

    /* いずれ dataに応じて auto movingするライト(オーサリングライト)が必要？ */
    _fwLightCtrlFromPAD();

    /* Draw Light Point Cube */    
#if 1
    switch (m) {
      case POINTL:
	_fwLightDrawCube(fwLightVal[fwMainPointLGroup][_fwLightTarget].color,
			 fwLightVal[fwMainPointLGroup][_fwLightTarget].point,
			 fwLightVal[fwMainPointLGroup][_fwLightTarget].vector);
	break;
      case SPOTL:
	_fwLightDrawCube(fwLightVal[fwMainSpotLGroup][_fwLightTarget].color,
			 fwLightVal[fwMainSpotLGroup][_fwLightTarget].point,
			 fwLightVal[fwMainSpotLGroup][_fwLightTarget].vector);
	break;
      default:
	break;
    }
#endif
}

/*
 * fwLightSet(gp, ln, lt)
 * Light group gp Light Number lnな ライトに lt(vector, color)をセットします
*/
void fwLightSet(fwLightGroup_t gp, fwLightNumber_t ln, fwLight_t *lt)
{
    if (gp > FW_LGROUP_MAX)
	return;
    if (ln > FW_LIGHT_MAX)
	return;
    sceVu0CopyVector(fwLightVal[gp][ln].vector, lt->vector);
    sceVu0CopyVector(fwLightVal[gp][ln].color , lt->color);
    sceVu0CopyVector(fwLightVal[gp][ln].point , lt->point);
}

/*
 * fwLightApply(obj)
 * objにLightをApplyさせます。
 * fwLightSet()で設定されたすべてのライトをアプライします
 */
void fwLightApply(fwObject_t *obj)
{
    sceVu0FVECTOR	vec[3], col[3], pnt[3];
    fwLightGroup_t	gp;

    /*
      ※ 注意
      現在同時には一つの種類の光源タイプしか、マイクロコードがだせない。
      各光源タイプ別に
      	PARRALEL => GROUP 0
	POINT    => GROUP 1
	SPOT     => GROUP 2
      を使うこと
    */
    for (gp = FW_LGROUP_0; gp < FW_LGROUP_MAX; gp++) {
	sceVu0CopyVector(vec[FW_LIGHT_0], fwLightVal[gp][FW_LIGHT_0].vector);
	sceVu0CopyVector(vec[FW_LIGHT_1], fwLightVal[gp][FW_LIGHT_1].vector);
	sceVu0CopyVector(vec[FW_LIGHT_2], fwLightVal[gp][FW_LIGHT_2].vector);
	sceVu0CopyVector(col[FW_LIGHT_0], fwLightVal[gp][FW_LIGHT_0].color);
	sceVu0CopyVector(col[FW_LIGHT_1], fwLightVal[gp][FW_LIGHT_1].color);
	sceVu0CopyVector(col[FW_LIGHT_2], fwLightVal[gp][FW_LIGHT_2].color);
	sceVu0CopyVector(pnt[FW_LIGHT_0], fwLightVal[gp][FW_LIGHT_0].point);
	sceVu0CopyVector(pnt[FW_LIGHT_1], fwLightVal[gp][FW_LIGHT_1].point);
	sceVu0CopyVector(pnt[FW_LIGHT_2], fwLightVal[gp][FW_LIGHT_2].point);
	col[FW_LIGHT_0][3] = fwLightVal[gp][FW_LIGHT_0].intensity;
	col[FW_LIGHT_1][3] = fwLightVal[gp][FW_LIGHT_1].intensity;
	col[FW_LIGHT_2][3] = fwLightVal[gp][FW_LIGHT_2].intensity;
	pnt[FW_LIGHT_0][3] = fwLightVal[gp][FW_LIGHT_0].angle;
	pnt[FW_LIGHT_1][3] = fwLightVal[gp][FW_LIGHT_1].angle;
	pnt[FW_LIGHT_2][3] = fwLightVal[gp][FW_LIGHT_2].angle;

	fwMicroSetLightVect(obj->data->micro_data, gp, vec[0], vec[1], vec[2]);
	fwMicroSetLightColor(obj->data->micro_data, gp, col[0], col[1], col[2], _fwLightAmbientVal);
	fwMicroSetLightPoint(obj->data->micro_data, gp, pnt[0], pnt[1], pnt[2]);
    }
}

static void _fwLightCtrlFromPAD(void)
{
#define MOVE_MIN	(.1f)
#define INFO_X		(3)
#define INFO_Y		(6)
    fwLightGroup_t      gp = 0;
    fwMicroType_t	m;
    static int		cur = 0; /* menu pointer */
    static int		item = 0;	/* item pointer */

    m = fwMicroGetCurrent();
    switch (m) {
      case BASE:
      case CULL:
      case FOG:
      case ANTI:
	gp = FW_LGROUP_0;
	break;
      case SPOTL:
	gp = FW_LGROUP_2;
	break;
      case POINTL:
	gp = FW_LGROUP_1;
	break;
      default:
	break;
    }

    /* PAD操作 */

    /* 対象Lightの入れ換え */
    if (fwPadStateVal.SelectSwitch) {
	++_fwLightTarget;
	if (_fwLightTarget >= FW_LIGHT_MAX)
	    _fwLightTarget = FW_LIGHT_0;
    }

    /* カーソル操作 */
    if (fwPadStateVal.LupSwitch)	cur--;
    if (fwPadStateVal.LdownSwitch)	cur++;
    if (fwPadStateVal.LleftSwitch)	item--;
    if (fwPadStateVal.LrightSwitch)	item++;
    if (cur  < 0)	cur = 0;
    if (cur  > 3)	cur = 3;
    if (item < 0)	item = 0;
    if (cur > 1) {
	item = 0;
    } else {
	if (item > 2)
	    item = 2;
    }

    /* ライト位置 */
    if (fwPadStateVal.RupOn)
	fwLightVal[gp][_fwLightTarget].point[1] -= MOVE_MIN;
    if (fwPadStateVal.RdownOn)
	fwLightVal[gp][_fwLightTarget].point[1] += MOVE_MIN;
    if (fwPadStateVal.RleftOn)
	fwLightVal[gp][_fwLightTarget].point[0] -= MOVE_MIN;
    if (fwPadStateVal.RrightOn)
	fwLightVal[gp][_fwLightTarget].point[0] += MOVE_MIN;
    if (fwPadStateVal.R1On)
	fwLightVal[gp][_fwLightTarget].point[2] -= MOVE_MIN;
    if (fwPadStateVal.R2On)
	fwLightVal[gp][_fwLightTarget].point[2] += MOVE_MIN;

    /* MENU物 */
    switch (cur) {
      case 0: /* COLOR CHANGE */
	if (fwPadStateVal.L1On)
	    fwLightVal[gp][_fwLightTarget].color[item] += 0.01;
	if (fwPadStateVal.L2On)
	    fwLightVal[gp][_fwLightTarget].color[item] -= 0.01;
	break;
      case 1: /* DIRECTION CHANGE */
	if (fwPadStateVal.L1On)
	    fwLightVal[gp][_fwLightTarget].vector[item] += 0.01;
	if (fwPadStateVal.L2On)
	    fwLightVal[gp][_fwLightTarget].vector[item] -= 0.01;
	break;
      case 2: /* INTENSITY CHANGE */
	if (fwPadStateVal.L1On)
	    fwLightVal[gp][_fwLightTarget].intensity += 10.0f;
	if (fwPadStateVal.L2On)
	    fwLightVal[gp][_fwLightTarget].intensity -= 10.0f;
	break;
      case 3: /* ANGLE CHANGE */
	if (fwPadStateVal.L1On)
	    fwLightVal[gp][_fwLightTarget].angle += 0.01;
	if (fwPadStateVal.L2On)
	    fwLightVal[gp][_fwLightTarget].angle -= 0.01;
	break;
    }

    /* check Color/Direction/intensity/angle value */
    if (fwLightVal[gp][_fwLightTarget].color[item] < 0.0f)
	fwLightVal[gp][_fwLightTarget].color[item] = 0.0f;
    if (fwLightVal[gp][_fwLightTarget].color[item] > 1.0f)
	fwLightVal[gp][_fwLightTarget].color[item] = 1.0f;
    if (fwLightVal[gp][_fwLightTarget].intensity < 0.0f)
	fwLightVal[gp][_fwLightTarget].intensity = 0.0f;
    if (fwLightVal[gp][_fwLightTarget].angle > 1.0f)
	fwLightVal[gp][_fwLightTarget].angle = 1.0f;
    if (fwLightVal[gp][_fwLightTarget].angle < 0.0f)
	fwLightVal[gp][_fwLightTarget].angle = 0.0f;

    /* 現在のライト情報の表示 */
    fwInfoDraw(INFO_X, INFO_Y,	   "Light Group %d Number %d", gp, _fwLightTarget);
    fwInfoDraw(INFO_X + 3, INFO_Y + 1, "POINT : [%5.2f, %4.1f, %4.1f]",
	       fwLightVal[gp][_fwLightTarget].point[0],
	       fwLightVal[gp][_fwLightTarget].point[1],
	       fwLightVal[gp][_fwLightTarget].point[2]);
    fwInfoDraw(INFO_X + 3, INFO_Y + 3, "COLOR : [%5.3f, %4.2f, %4.2f]",
	       fwLightVal[gp][_fwLightTarget].color[0],
	       fwLightVal[gp][_fwLightTarget].color[1],
	       fwLightVal[gp][_fwLightTarget].color[2]);
    fwInfoDraw(INFO_X + 3, INFO_Y + 4, "DIR   : [%5.3f, %4.2f, %4.2f]",
	       fwLightVal[gp][_fwLightTarget].vector[0],
	       fwLightVal[gp][_fwLightTarget].vector[1],
	       fwLightVal[gp][_fwLightTarget].vector[2]);
    fwInfoDraw(INFO_X + 3, INFO_Y + 5, "INTENSITY : %5.3f", fwLightVal[gp][_fwLightTarget].intensity);
    fwInfoDraw(INFO_X + 3, INFO_Y + 6, "ANGLE     : %5.3f", fwLightVal[gp][_fwLightTarget].angle);

    fwInfoDraw(INFO_X, INFO_Y + 3 + cur, ">");
    fwInfoDraw(INFO_X + 14 + item * 7, INFO_Y + 2, "V");
}

/*
 * ライト位置を示すCUBEを出す
 *	GSにダイレクトでデータを渡しています
 */
static void _fwLightDrawCube(sceVu0FVECTOR color, sceVu0FVECTOR point, sceVu0FVECTOR rot)
{
union QWdata {
    u_long128	ul128;
    u_long		ul64[2];
    u_int		ui32[4];
    sceVu0FVECTOR	fvect;
};
sceVu0FVECTOR cube[14] = {
                        {-5.0, -5.0, 5.0, 0.0},
                        {-5.0, -5.0, -5.0, 0.0},
                        {-5.0, 5.0, 5.0, 0.0},
                        {-5.0, 5.0, -5.0, 0.0},
                        {5.0, 5.0, -5.0, 0.0},
                        {-5.0, -5.0, -5.0, 0.0},
                        {5.0, -5.0, -5.0, 0.0},
                        {-5.0, -5.0, 5.0, 0.0},
                        {5.0, -5.0, 5.0, 0.0},
                        {-5.0, 5.0, 5.0, 0.0},
                        {5.0, 5.0, 5.0, 0.0},
                        {5.0, 5.0, -5.0, 0.0},
                        {5.0, -5.0, 5.0, 0.0},
                        {5.0, -5.0, -5.0, 0.0}};
    int			j;
    struct {
	union QWdata	buf[29];
	u_int		count;
    } pack __attribute__ ((aligned(16)));

#define LIGHT_CUBE_SIZE	0.1	/* 光源位置を示すキューブの大きさ */
    sceVu0FMATRIX	work;
    sceVu0FVECTOR	v00, v01;
    sceVu0IVECTOR	v02;

    point[3] = 1.0f;

    pack.count = 0;
    pack.buf[pack.count].ul64[0] = SCE_GIF_SET_TAG(14, 1, 1, 0x04, 0, 2);
    pack.buf[pack.count].ul64[1] = 0x0041L;
    pack.count++;
    sceVu0UnitMatrix(work);
/*    sceVu0RotMatrix(work, work, rot);*/
    for (j = 0; j < 14; j++) {
	sceVu0ApplyMatrix(v00, work, cube[j]);
	sceVu0ScaleVector(v00, v00, LIGHT_CUBE_SIZE);
	sceVu0AddVector(v01, v00, point);
	sceVu0RotTransPers(v02, fwCameraDataVal.world_screen, v01, 0);
	pack.buf[pack.count++].ul128 = *((u_long128 *)color);
	pack.buf[pack.count++].ul128 = *((u_long128 *)v02);
    }
    *D2_QWC = pack.count;
    *D2_MADR = (u_int) &pack.buf & 0x0fffffff;
    *D_STAT = 2;
    FlushCache(0);
    *D2_CHCR = 1 | (0 << 2) | (0 << 4) | (0 << 5) | (0 << 6) | (0 << 7) | (1 << 8);
    sceGsSyncPath(0, 0);
}
