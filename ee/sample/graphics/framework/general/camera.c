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
 *                          Name : camera.c
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000  
*/

/*******************************************************
 * 概要
 *	Frame Work内でのCamera Operationの為の関数群
 *
 *	Frame Workでは 一つのカメラしか持たない設計になっています
 *	そのカメラはfwCameraData_t型のグローバル変数
 *	fwCameraDataValで参照できます。
 *	この中の値を変更したのち、fwCameraMakeMatrix()を
 *	呼出すことによって、適切なカメラデータが作成されます。
 *******************************************************/

#include <eekernel.h>
#include <libvu0.h>
#include "defines.h"
#include "data.h"
#include "frame_t.h"
#include "micro.h"
#include "object.h"
#include "camera.h"
#include "pad.h"

/***************************************************
 * Global Variables
 ***************************************************/
fwCameraData_t	fwCameraDataVal;

/***************************************************
 * Static Variables
 ***************************************************/
static sceVu0FVECTOR _fwCameraClipVolVal = { 1000.0f, 1000.0f, 1.0f, 65535.0f};
static fwCameraPos_t	_fwCameraPosVal;
static fwCameraPos_t	_fwCameraPosInitVal = {
    { 0.0f, 0.0f, -30.0f, 0.0f },		// pos
    { 0.0f, 0.0f, 1.0f, 1.0f },			// zdir
    { 0.0f, 1.0f, 0.0f, 1.0f },			// ydir
    { 0.0f, 0.0f, 0.0f, 0.0f },			// rot
};

/***************************************************
 * Static Function declarations
 **************************************************/
static void _fwCameraCtrlFromPAD(void);
inline static void _fwCameraSetPos(fwCameraData_t *, fwCameraPos_t *);
inline static void _fwCameraSetClipVolume(fwCameraData_t *, sceVu0FVECTOR);
inline static void _fwCameraSetClipScreen(fwCameraData_t *, float, float, float, float, float, float, float, float, float);

static void _fwCameraSetViewScreenClip(sceVu0FMATRIX, sceVu0FMATRIX, sceVu0FMATRIX, sceVu0FVECTOR, float, float, float, float, float, float, float, float, float);

inline static void _fwCameraSetMatrix(fwCameraData_t *);
inline static void _fwCameraSetRotMatrix(sceVu0FMATRIX , fwCameraPos_t *);

static void _fwCameraSetClipData(fwCameraData_t *, float , float , float , float);
/***************************************************
 * function definitions
 ***************************************************/

/*
 * fwCameraInit()
 *	Camera CLASSの初期化
 *	アプリケーションで一回だけ呼出すこと
 */
#define CLIP_ALPHA 300 
void fwCameraInit()
{
    _fwCameraPosVal = _fwCameraPosInitVal;
    _fwCameraSetPos(&fwCameraDataVal, &_fwCameraPosVal);
    _fwCameraSetClipVolume(&fwCameraDataVal, _fwCameraClipVolVal);
    _fwCameraSetClipScreen(&fwCameraDataVal, 512.0f, 1.0f, 0.47f,
			        2048.0f, 2048.0f,
				5.0f, 16777000.0f,
				1.0f, 65536.0f);
    _fwCameraSetClipData(&fwCameraDataVal, SCREEN_WIDTH+CLIP_ALPHA, SCREEN_HEIGHT * 2.0f + CLIP_ALPHA, 1.0f, 65536.0f);
    _fwCameraSetMatrix(&fwCameraDataVal);
}

/*
 * fwCameraCtrl()
 *	Camera Modeでのカメラコントロールを行います
 *	Camera Modeの Exec関数から呼出してください
 */
void fwCameraCtrl(void)
{
    /* いずれ dataに応じてauto movingするカメラ(オーサリングカメラが必要？ */
    _fwCameraCtrlFromPAD();
}

/*
 * fwCameraSet()
 *	現在のカメラデータを objectに反映します
 *	すべての表示オブジェクトに対して毎フレーム行ってください
 */
void fwCameraSet(fwObject_t *obj)
{
    /* 現在のPositionデータを、カメラデータにApplyします */
    _fwCameraSetPos(&fwCameraDataVal, &_fwCameraPosVal);
    /* 現在のカメラデータから カメラMatrixを作り出します */
    _fwCameraSetMatrix(&fwCameraDataVal);

    /* 各matrixを Micro Dataにセットします */
    fwMicroSetWorldScreen(obj->data->micro_data, fwCameraDataVal.world_screen);
    fwMicroSetWorldClip(obj->data->micro_data, fwCameraDataVal.world_clip);
    fwMicroSetClipScreen(obj->data->micro_data, fwCameraDataVal.clip_screen);
}

/*
 * _fwCameraCtrlFromPAD()
 *	PADを使ったカメラ操作を制御します
 */
static void _fwCameraCtrlFromPAD(void)
{
    float	dr = 0.5f / 180.0f * PI;
    float	dt = 2.0f;

    if (fwPadStateVal.LdownOn) {
	_fwCameraPosVal.rot[0] +=dr; 
	if (_fwCameraPosVal.rot[0]>PI)
	    _fwCameraPosVal.rot[0] -= 2.0f*PI; 
    }
    if (fwPadStateVal.LupOn) {
	_fwCameraPosVal.rot[0] -=dr;
	if (_fwCameraPosVal.rot[0]<-PI)
	    _fwCameraPosVal.rot[0] += 2.0f*PI; 
    }
    if (fwPadStateVal.LrightOn) {
	_fwCameraPosVal.rot[1] -=dr;
	if (_fwCameraPosVal.rot[1]<-PI)
	    _fwCameraPosVal.rot[1] += 2.0f*PI; 
    }
    if (fwPadStateVal.LleftOn) {
	_fwCameraPosVal.rot[1] +=dr;
	if (_fwCameraPosVal.rot[1]>PI)
	    _fwCameraPosVal.rot[1] -= 2.0f*PI; 
    }
    if (fwPadStateVal.L1On) {
	_fwCameraPosVal.rot[2] -=dr;
	if (_fwCameraPosVal.rot[2]<-PI)
	    _fwCameraPosVal.rot[2] += 2.0f*PI; 
    }
    if (fwPadStateVal.L2On) {
	_fwCameraPosVal.rot[2]+=dr;
	if (_fwCameraPosVal.rot[2]>PI)
	    _fwCameraPosVal.rot[2] -= 2.0f*PI; 
    }
    if (fwPadStateVal.RdownOn)
	_fwCameraPosVal.pos[1] +=dt;
    if (fwPadStateVal.RupOn)
	_fwCameraPosVal.pos[1] -= dt;
    if (fwPadStateVal.RrightOn)
	_fwCameraPosVal.pos[0] += dt;
    if (fwPadStateVal.RleftOn)
	_fwCameraPosVal.pos[0] -= dt;
    if (fwPadStateVal.R1On)
	_fwCameraPosVal.pos[2] += dt;
    if (fwPadStateVal.R2On)
	_fwCameraPosVal.pos[2] -= dt;
    
    if (fwPadStateVal.SelectSwitch)
	_fwCameraPosVal = _fwCameraPosInitVal;
}

/*
 * _fwCameraSetPos(cam, pos)
 *	CameraデータcamのPositionをposにします
 *	frameworkでは Cameraは１個しかありませんので
 *	camはつねに&fwCameraDataValです。
 */
inline static void _fwCameraSetPos(fwCameraData_t *cam, fwCameraPos_t *pos)
{
    cam->cp = *pos;
}

/*
 * _fwCameraSetClipVolume(cam, cvol)
 *	CameraデータcamのClip Volumeをcvolにします
 *	frameworkでは Cameraは１個しかありませので
 *	camはつねに&fwCameraDataValです。
 */
inline static void _fwCameraSetClipVolume(fwCameraData_t *cam, sceVu0FVECTOR cvol)
{
    sceVu0CopyVector((cam)->clip_volume, cvol);
}

/*
 * _fwCameraSetClipScreen(cam, sdist, xasp, yasp, gsx, gsy, zmin, zmax, near, far)
 *	CameraデータcamのClip Screen設定をします
 *	_fwCameraSetViewScreenClip()のWrapperになっています
 *		cam	: Camera Data
 *     		sdist	: screen distance
 *		xasp	: screen X aspect
 *		yasp	: screen Y aspect
 *		gsx	: GS X size
 *		gsy	: GS Y size
 *		zmin	: Z minimam
 *		zmax	: Z maximum
 *		near	: near clip
 *		far	: far clip
 */
inline static void _fwCameraSetClipScreen(fwCameraData_t *cam, float sdist, float xasp, float yasp, float gsx, float gsy, float zmin, float zmax, float near, float far)
{
    _fwCameraSetViewScreenClip(cam->view_screen, cam->view_clip, cam->clip_screen, cam->clip_volume, sdist, xasp, yasp, gsx, gsy, zmin, zmax, near, far);
}

static void _fwCameraSetViewScreenClip(sceVu0FMATRIX m, sceVu0FMATRIX mc,
			     sceVu0FMATRIX mcs,
			     sceVu0FVECTOR clip_vol,
			     float scrz, float ax, float ay, 
			     float cx, float cy, 
			     float zmin, float zmax, 
			     float nearz, float farz)
{
	float	az, cz;
	float   gsx, gsy;
	sceVu0FMATRIX	mt;		

	gsx = nearz*clip_vol[0]/scrz;
	gsy = nearz*clip_vol[1]/scrz;

	cz = (-zmax * nearz + zmin * farz) / (-nearz + farz);
	az  = farz * nearz * (-zmin + zmax) / (-nearz + farz);

	//     | scrz    0  0 0 |
	// m = |    0 scrz  0 0 | 
	//     |    0    0  0 1 |
	//     |    0    0  1 0 |
	sceVu0UnitMatrix(m);
	m[0][0] = scrz;	m[1][1] = scrz;
	m[2][2] = 0.0f;	m[3][3] = 0.0f;
	m[3][2] = 1.0;	m[2][3] = 1.0;

	//     | ax  0  0 cx |
	// m = |  0 ay  0 cy | 
	//     |  0  0 az cz |
	//     |  0  0  0  1 |
	sceVu0UnitMatrix(mt);
	mt[0][0] = ax;	mt[1][1] = ay;	mt[2][2] = az;
	mt[3][0] = cx;	mt[3][1] = cy;	mt[3][2] = cz;
	sceVu0MulMatrix(m, mt, m);

	
	//     |2n/2gsx    0        0           0      |
	// m = |   0    2n/2gsy     0           0      |
	//     |   0       0   (f+n)/(f-n) -2f*n/(f-n) |
	//     |   0       0        1           0      |
	sceVu0UnitMatrix(mc);
	mc[0][0] = 2.0f * nearz / (gsx-(-gsx));
	mc[1][1] = 2.0f * nearz / (gsy-(-gsy));
	mc[2][2] = (farz + nearz) / (farz-nearz);
	mc[3][2] = -2*(farz * nearz) / (farz-nearz);
	mc[2][3] = 1.0f;	mc[3][3] = 0.0f;

	//     |scrz*ax*gsx/n    0            0               cx      |
	// m = |   0        scrz*ay*gsy/n     0               cy      |
	//     |   0             0      (-zmax+zmin)/2  (zmin+zmax)/2 |
	//     |   0             0            0               1       |
	sceVu0UnitMatrix(mcs);
	mcs[0][0] = scrz*ax*gsx/nearz;
	mcs[1][1] = scrz*ay*gsy/nearz;
	mcs[2][2] = (-zmax+zmin)/2.0f;
	mcs[3][2] = (zmax+zmin)/2.0f;
	mcs[3][0] = cx;	mcs[3][1] = cy;	
		mcs[3][3] = 1.0f;	

	return;
}

/*
 * _fwCameraSetMatrix(cam)
 *	CameraデータcamのMatrix(world_view/world_screen/world_clip)を
 *	計算・設定します
 */
inline static void _fwCameraSetMatrix(fwCameraData_t *cam)
{
    /* world => camera local matrix を world_viewへ */
    _fwCameraSetRotMatrix(cam->world_view, &cam->cp);
    /* 透視変換matrixを world_screenへ */
    sceVu0MulMatrix(cam->world_screen, cam->view_screen, cam->world_view);
    /* CLIP matrixを world_clipへ */
    sceVu0MulMatrix(cam->world_clip, cam->view_clip, cam->world_view);
}

/*
 * _fwCameraSetRotMatrix
 *	PositionデータcpによるMatrixを計算して mに返す
 */
inline static void _fwCameraSetRotMatrix(sceVu0FMATRIX m, fwCameraPos_t *cp)
{
    sceVu0FVECTOR	pos;

    /* 
       上は 以前のframeworkの物です。
       どちらのコードでも動きますが、意味的に
       「World座標 => Camera Local座標 が カメラマトリクス」
       という意味での動作は 下です。（また下の方が効率がよい。
       上はわざわざカメラ座標・注視点ベクトル・カメラ上方ベクトル
       に直したのち、sceVu0CameraMatrix()でrotation計算をしている）

       カメラのデータが 位置と注視点 で扱うようになったら
       sceVu0CameraMatrix()を使った方がよいと思われます。

       ゲームなどでは 位置と注視点でのカメラなどが多いと
       思われますので 一応上のタイプもコードとして残しておきます
    */
#if 0
    sceVu0FMATRIX	work;
    sceVu0FVECTOR	vert, dir;

    sceVu0UnitMatrix(work);
    sceVu0RotMatrixX(work, work, cp->rot[0]);
    sceVu0RotMatrixY(work, work, cp->rot[1]);
    sceVu0RotMatrixZ(work, work, cp->rot[2]);
    sceVu0ApplyMatrix(dir, work, cp->zdir);
    sceVu0ApplyMatrix(vert, work, cp->ydir);
    sceVu0ApplyMatrix(pos, work, cp->pos);
    sceVu0CameraMatrix(m ,pos, dir, vert);
#else
    sceVu0UnitMatrix(m);
    sceVu0RotMatrixZ(m, m, - cp->rot[2]);
    sceVu0RotMatrixY(m, m, - cp->rot[1]);
    sceVu0RotMatrixX(m, m, - cp->rot[0]);
    pos[0] = - cp->pos[0];
    pos[1] = - cp->pos[1];
    pos[2] = - cp->pos[2];
    sceVu0TransMatrix(m, m, pos);
#endif
}

static void _fwCameraSetClipData(fwCameraData_t *cam, float sw, float sh, float sn, float sf)
{
    cam->screen_w = sw;
    cam->screen_h = sh;
    cam->near = sn;
    cam->far = sf;
}
