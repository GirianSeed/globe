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
 *                          Name : micro.c
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000    
 */

/*******************************************************
 * 概要
 *	Frame Workの マイクロコード関係の関数群
 *	マイクロコードパケットの生成や、マイクロコードに
 *	渡すデータの書き換えなどを行う
 *******************************************************/

#include <eekernel.h>
#include <libvu0.h>
#include <libgraph.h>
#include "defines.h"
#include "packbuf.h"
#include "frame_t.h"
#include "data.h"
#include "micro.h"
#include "malloc.h"
#include "micro.h"
#include "info.h"
#include "object.h"
#include "camera.h"
#include "pad.h"

qword fwMicroGiftagCurrent;

/*
  _fwMicroSettingData
  fwMicroSetInitDataでセットするデータ

  cameraX, Y, Z: camera global position
  fogA = 1 / (fog_begin - fog_end)
  fogB = fog_end
  n, f : screen near/far
  sw/sh : screen width/height

  { cameraX, cameraY, cameraZ, anti-cutoff },
  { fogA,    fogB,    PRIM     ----------  },
  { ((n+f)/2)/sw, ((n+f)/2)/sh, (n+f)/2, (f-n)/2 },
  { ------- --------  -------  ----------  }

*/
static union {
    sceVu0FMATRIX	mat;
    u_int		ui[4][4];
} _fwMicroSettingData;

static union {
    u_long	ul[2];
    u_int	ui[4];
} _giftags[2] __attribute__((align(64)));

static float		_fwMicroAntiCutoff = 0.09f;
static float		_fwMicroFogBegin = 1298388.0f;
static float		_fwMicroFogEnd = 93784.0f;

static fwMicroFrame_t	*_fwMicroFrameCur = NULL;

static void _fwMicroCtrlAnti(void);
static void _fwMicroCtrlFog(void);

static qword* _fwMicroMakeCODEPacket(fwMicroFrame_t *, u_int *, int);
static qword* _fwMicroMakeDATAPacket(fwMicroFrame_t *, u_int *);

static fwMicroType_t _fwMicroCurrentVal;

static fwMicroTable_t 	*_fwMicroTblVal;
static u_int		_fwMicroTblNumVal;

/*
 * fwMicroSetTable
 *	fwMicro～()で使用されるmicro codeを指定する
 */
void fwMicroSetTable(fwMicroTable_t *tbl, u_int tblnum)
{
    _fwMicroTblVal = tbl;
    _fwMicroTblNumVal = tblnum;
}

/*
 * fwMicroInit(data)
 *	es2rawが吐いたデータのアドレスdataから
 *	micro code/dataの為の Micro CLASS Instance
 *	（fwMicroFrame_t型）を作成して返します
 *	作成失敗は NULLを返します
 */
fwMicroFrame_t* fwMicroInit(fwDataHead_t *data)
{
    fwMicroFrame_t	*mf;
    int			i;

    mf = (fwMicroFrame_t *) memalign(16, sizeof(fwMicroFrame_t));
    mf->mcode = (qword **) malloc(sizeof(qword *) * _fwMicroTblNumVal);
    for (i = 0; i < _fwMicroTblNumVal; i++) {
	mf->mcode[i] = _fwMicroMakeCODEPacket(mf, _fwMicroTblVal[i].mcode, 0);
    }
    mf->mdata = _fwMicroMakeDATAPacket(mf, data->micro_data);
    mf->data = (fwMicroData_t *)data->micro_data;

    /* 最初のマイクロ登録なら Change Microしておく */
    if (_fwMicroFrameCur == NULL) {
	_fwMicroFrameCur = mf;
	fwMicroChange(BASE);
    }

    /* giftag making */
    _giftags[0].ul[0] = SCE_GIF_SET_TAG(1, 0, 0, 0, 0, 2);
    _giftags[0].ul[1] = (SCE_GIF_PACKED_AD << (4 * 0)) | (SCE_GIF_PACKED_AD << (4 * 1));

    return mf;
}

void fwMicroPreCalc(fwMicroFrame_t *mf)
{
    sceVu0FVECTOR	pos;
    sceVu0FMATRIX	work;
    u_int		nreg, tag2, tag3, flg, prim;
    float		fPn, fSn, sw, sh; /* FarPlusNear, FarSubNear, ScreenW, ScreenH */

    sceVu0UnitMatrix(work);
    sceVu0RotMatrixX(work, work, fwCameraDataVal.cp.rot[0]);
    sceVu0RotMatrixY(work, work, fwCameraDataVal.cp.rot[1]);
    sceVu0RotMatrixZ(work, work, fwCameraDataVal.cp.rot[2]);
    sceVu0ApplyMatrix(pos, work, fwCameraDataVal.cp.pos);

    sceVu0CopyVector(_fwMicroSettingData.mat[0], pos);
    _fwMicroSettingData.mat[0][3] = _fwMicroAntiCutoff;
    _fwMicroSettingData.mat[1][0] = 1 / (_fwMicroFogBegin - _fwMicroFogEnd);
    _fwMicroSettingData.mat[1][1] = _fwMicroFogEnd;

    fPn = fwCameraDataVal.far + fwCameraDataVal.near;
    fSn = fwCameraDataVal.far - fwCameraDataVal.near;
    sw  = fwCameraDataVal.screen_w;
    sh  = fwCameraDataVal.screen_h;
    _fwMicroSettingData.mat[2][0] = fSn / sw;
    _fwMicroSettingData.mat[2][1] = fSn / sh;
    _fwMicroSettingData.mat[2][2] = fPn / 2.0f;
    _fwMicroSettingData.mat[2][3] = fSn / 2.0f;

    /* default value */
    nreg = 3;
    tag2 = (SCE_GS_ST << (4 * 0)) | (SCE_GS_RGBAQ << (4 * 1)) | (SCE_GS_XYZF2 << (4 * 2));
    tag3 = 0;
    flg = SCE_GIF_PACKED;
    prim = 0;

    if (_fwMicroTblVal[_fwMicroCurrentVal].mattr & FW_MATTR_FGE) {
	prim |= GS_PRIM_FGE_M;
    }
    if (_fwMicroTblVal[_fwMicroCurrentVal].mattr & FW_MATTR_ANTI) {
	nreg = 4;
	tag2 = (SCE_GIF_PACKED_AD << (4 * 0)) | (SCE_GS_ST << (4 * 1)) | (SCE_GS_RGBAQ << (4 * 2)) | (SCE_GS_XYZF2 << (4 * 3));
    }

    _giftags[1].ui[3] = tag3;
    _giftags[1].ui[2] = tag2;
    _giftags[1].ui[1] = (nreg << GIF_TAG1_NREG_O) | (flg << GIF_TAG1_FLG_O);
    _fwMicroSettingData.ui[1][2] = prim;

    fwMicroSetInitData(mf->data, _fwMicroSettingData.mat);
}

void fwMicroPostCalc(fwMicroFrame_t *mf, int flg)
{
    if (flg) {
	fwPackbufAddCall(fwPackbufKickBufVal.pCurr, mf->mcode[_fwMicroCurrentVal]);
	fwPackbufKickBufVal.pCurr++;
	fwPackbufAddCall(fwPackbufKickBufVal.pCurr, mf->mdata);
	fwPackbufKickBufVal.pCurr++;
    } else {
	fwPackbufAddCall(fwPackbufKickBufVal.pCurr, mf->mdata);
	fwPackbufKickBufVal.pCurr++;
    }
}

void fwMicroFinish(fwMicroFrame_t *mf)
{
}

void fwMicroChange(fwMicroType_t m)
{
    fwMicroFrame_t	*mf;

    _fwMicroCurrentVal = m;
    mf = _fwMicroFrameCur;
}

void fwMicroCtrl(void)
{
    /* 現在のマイクロの種類に応じて動作が違う */
    switch (_fwMicroCurrentVal) {
      case BASE:
      case CULL:
      case SPOTL:
      case POINTL:
	break;
      case ANTI:
	_fwMicroCtrlAnti();
	break;
      case FOG:
	_fwMicroCtrlFog();
	break;
      default:
	break;
    }
}

fwMicroType_t fwMicroGetCurrent(void)
{
    return _fwMicroCurrentVal;
}

/*
 * _fwMicroCtrlAnti()
 *	ANTI Microの場合のPAD Control
 */
static void _fwMicroCtrlAnti(void)
{
#define INFO_X	(3)
#define INFO_Y	(6)
    if (fwPadStateVal.RupOn)
	_fwMicroAntiCutoff += 0.005f;
    if (fwPadStateVal.RdownOn)
	_fwMicroAntiCutoff -= 0.005f;
    if (_fwMicroAntiCutoff < 0.09f)
	_fwMicroAntiCutoff = 0.09f;
    if (_fwMicroAntiCutoff > 2.0f)
	_fwMicroAntiCutoff = 2.0f;
    fwInfoDraw(INFO_X, INFO_Y, "ANTI CUTOFF : %8.3f", _fwMicroAntiCutoff);
}

/*
 * _fwMicroCtrlFog()
 *	FOG Microの場合のPAD Control
 */
static void _fwMicroCtrlFog(void)
{
#define INFO_X	(3)
#define INFO_Y	(6)
#define MIN_MOVE	(5000.0f)

    if (fwPadStateVal.L1On)
	if (_fwMicroFogBegin - MIN_MOVE > _fwMicroFogEnd)
	    _fwMicroFogBegin -= MIN_MOVE;
    if (fwPadStateVal.L2On)
	_fwMicroFogBegin += MIN_MOVE;
    if (fwPadStateVal.R1On)
	_fwMicroFogEnd   -= MIN_MOVE;
    if (fwPadStateVal.R2On)
	if (_fwMicroFogEnd + MIN_MOVE < _fwMicroFogBegin)
	_fwMicroFogEnd   += MIN_MOVE;

    fwInfoDraw(INFO_X, INFO_Y    , "FOG BEGIN   : %8.3f", _fwMicroFogBegin);
    fwInfoDraw(INFO_X, INFO_Y + 1, "FOG END     : %8.3f", _fwMicroFogEnd);
}

/*
 * _fwMicroMakePacket(mf, atom, worksize, data)
 *	Micro Code/DataをDMAで送るパケット列の生成
 *	mf	: Micro CLASS Instance
 *	atom	: Micro Code Load Packet Chain Address
 *	worksize: VU1のwork size
 */
static qword* _fwMicroMakeCODEPacket(fwMicroFrame_t *mf, u_int *atom, int worksize)
{
    qword	*cur, *ret;

    ret = cur = fwPackbufVal.pCurr;

    /* Micro Codeをロードするパケット列をCALLする */
    CALL_NOP(*cur, (u_int) atom, 0); cur++;

    /* VU Mem1の Double Buffer設定 */
    RET_BASE_OFFSET(*cur, 120, (1024 - 120 - worksize) / 2); cur++;

    fwPackbufVal.pCurr = cur;
    return ret;
}

static qword* _fwMicroMakeDATAPacket(fwMicroFrame_t *mf, u_int *data)
{
    qword	*cur, *ret;

    ret = cur = fwPackbufVal.pCurr;

    /* Micro Dataの転送要求 */
    REF_STCYCL_UNPACK(*cur, (u_int)data, 16, 52, 1, 1); cur++;
    REF_STCYCL_UNPACK(*cur, (u_int)_giftags, 8, 2, 1, 1); cur++;
    RET_NOP(*cur, 0); cur++;

    fwPackbufVal.pCurr = cur;

    return ret;
}
