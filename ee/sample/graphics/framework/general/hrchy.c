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
 *                          Name : hrchy.c
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            0.40      Jun,21,2000    kaneko
 *            2.1       Nov,17,2000
 */

#include <eekernel.h>
#include <stdio.h>
#include <eeregs.h>
#include <libvu0.h>
#include <malloc.h>
#include "defines.h"
#include "data.h"
#include "frame_t.h"
#include "hrchy.h"

/***************************************************
 * constans definitions
 ***************************************************/
#define STACKSIZE	20
#define QWSIZE		4

/***************************************************
 * static variable definitions
 ***************************************************/
static int _fwHrchyCurrentVal = 0;
static int _fwHrchyCurRotVal = 0;           
static sceVu0FMATRIX _fwHrchyCurMatrixVal = { {1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0} };
static sceVu0FMATRIX _fwHrchyCurRotMatrixVal =  { {1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0} };
static sceVu0FMATRIX _fwHrchyStackMatrixVal[STACKSIZE];
static sceVu0FMATRIX _fwHrchyStackRotMatrixVal[STACKSIZE];
static sceVu0FVECTOR _fwHrchyZeroVector = {0.0f, 0.0f, 0.0f, 0.0f};

/***************************************************
 * static function declarations
 ***************************************************/
static void _fwHrchyInitData(fwHrchyFrame_t *, u_int *, u_int *);
static void _fwHrchySet(fwHrchyFrame_t *);
static void _fwHrchySetMatrix(fwHrchyFrame_t *, int);
static inline void _fwHrchyPushRotMatrix(sceVu0FMATRIX);
static inline void _fwHrchyPopRotMatrix(sceVu0FMATRIX);
static inline void _fwHrchyLoadRotMatrix(sceVu0FMATRIX);
static inline void _fwHrchyPopMatrix(sceVu0FMATRIX);
static inline void _fwHrchyPushMatrix(sceVu0FMATRIX);
static inline void _fwHrchyLoadMatrix(sceVu0FMATRIX);

/***************************************************
 * function definitions
 ***************************************************/

/*
 * fwHrchyInit(data)
 *	es2rawが吐いたデータのアドレスdataから
 *	hierarchy dataの為の Hrchy CLASS Instance
 *	（fwHrchyFrame_t型）を作成して返します
 *	作成失敗はNULLを返します
 */
fwHrchyFrame_t* fwHrchyInit(fwDataHead_t *data)
{
    fwHrchyFrame_t	*hrchy_frame;

    hrchy_frame = (fwHrchyFrame_t *)memalign(16, sizeof(fwHrchyFrame_t));
    if (hrchy_frame == NULL)
	return NULL;
    _fwHrchyInitData(hrchy_frame, data->hrchy_data, data->basematrix);
    return hrchy_frame;
}

/*
 * fwHrchyPreCalc(hf)
 *	Hrchy CLASS hfのPre Calcを行う
 */
void fwHrchyPreCalc(fwHrchyFrame_t *hf)
{
    _fwHrchySet(hf);
}

/*
 * fwHrchyPostCalc(hf)
 *	Hrchy CLASS hfのPost Calcを行う
 */
void fwHrchyPostCalc(fwHrchyFrame_t *hf)
{
}

/*
 * fwHrchyFinish(hf)
 *	Hrchy CLASS hfを消す
 */
void fwHrchyFinish(fwHrchyFrame_t *hf)
{
    free((u_int *)Paddr(hf));
}

/*
 * _fwHrchyInitData(hf, hrcP, matP)
 *	Hrchy CLASS hfのデータ初期化を行います
 *		hrcP	: hierarchy data pointer
 *		matP	: hierarchy matrix data pointer
 */

static void _fwHrchyInitData(fwHrchyFrame_t *hf, u_int *hrcP, u_int *matP)
{
    hf->num = hrcP[3];
    hf->hrc = (fwHrchy_t *)(hrcP + QWSIZE);
    hf->mat = (fwHrchyMat_t *)(matP + QWSIZE);
    sceVu0CopyVector(hf->root_trans, _fwHrchyZeroVector);
    sceVu0CopyVector(hf->root_rot,   _fwHrchyZeroVector);
}

/*
 * _fwHrchySet(hf)
 *	Hrchy CLASSの階層によるマトリクスなどの計算をします
 */
static void _fwHrchySet(fwHrchyFrame_t *hf)
{
    int		i;
    fwHrchy_t	*hrc;
    sceVu0FMATRIX	mat;

    for (i = 0; i < hf->num; i++) {
	hrc = (fwHrchy_t *)(hf->hrc + i);
	/*
	  自分に親がいなければROOT Hierarchyとする
	*/
	if (hrc->parent == -1) {
	    /* 
	       Hrchy CLASS Instance全体の座標（すなわちモデル座標）
	       を、あらかじめスタックにもっておく
	    */
	    sceVu0UnitMatrix(mat);
	    sceVu0RotMatrixY(mat, mat, hf->root_rot[1]);
	    sceVu0RotMatrixX(mat, mat, hf->root_rot[0]);
	    sceVu0RotMatrixZ(mat, mat, hf->root_rot[2]);
	    _fwHrchyPushRotMatrix(mat);
	    sceVu0TransMatrix(mat, mat, hf->root_trans);
	    /*
	      モデル座標をスタックからだす
	    */
	    _fwHrchyPushMatrix(mat);
	    _fwHrchySetMatrix(hf, i);
	    _fwHrchyPopMatrix(mat);
	    _fwHrchyPopRotMatrix(mat);
	}
    }
}

/*
 * _fwHrchySetMatrix(hf, id)
 *	Hrchy CLASS hfのidが示す階層のマトリクスを計算する
 */
static void _fwHrchySetMatrix(fwHrchyFrame_t *hf, int id)
{
    int		i;
    static sceVu0FMATRIX	m2;
    static sceVu0FMATRIX	m0;
    fwHrchy_t	*hrc;
    fwHrchyMat_t	*mat;
    sceVu0FVECTOR	rot;

    hrc = (fwHrchy_t *)(hf->hrc + id);
    mat = (fwHrchyMat_t *)(hf->mat + id);
    {
	sceVu0FVECTOR	v;
	int		npi;

	sceVu0CopyVector(rot, hrc->rot);
	sceVu0ScaleVectorXYZ(v, rot, 1.0f/PI);
	for (i = 0; i < 3; i++) {
	    npi = (int) v[i];
	    if (npi < 0)
		npi -= 1;
	    else
		npi += 1;
	    npi /= 2;
	    if (npi != 0)
		rot[i] -= npi * (2.0f * PI);
	}
    }
    {
	sceVu0FMATRIX	m1;

	sceVu0UnitMatrix(m1);
	sceVu0RotMatrixY(m1, m1, rot[1]);
	sceVu0RotMatrixX(m1, m1, rot[0]);
	sceVu0RotMatrixZ(m1, m1, rot[2]);
	_fwHrchyLoadRotMatrix(m2);
	sceVu0MulMatrix(m2, m2, m1);
	sceVu0TransMatrix(m1, m1, hrc->trans);
	_fwHrchyLoadMatrix(m0);
	sceVu0MulMatrix(m0, m0, m1);
    }
    sceVu0CopyMatrix(mat->local_matrix, m0);
    sceVu0CopyMatrix(mat->light_matrix, m2);

    for (i = 0; i < hf->num; i++) {
	hrc = (fwHrchy_t *)(hf->hrc + i);
	if ((hrc->parent != -1) && (hrc->parent == id)) {
	    _fwHrchyPushRotMatrix(m2);
	    _fwHrchyPushMatrix(m0);
	    _fwHrchySetMatrix(hf, i);
	    _fwHrchyPopMatrix(m0);
	    _fwHrchyPopRotMatrix(m2);
	}
    }
}

/*
 * _fwHrchyPushRotMatrix(m)
 *	mのマトリクスをRotMatrixとしてプッシュする
 */
static inline void _fwHrchyPushRotMatrix(sceVu0FMATRIX m)
{
    sceVu0CopyMatrix(_fwHrchyStackRotMatrixVal[_fwHrchyCurRotVal], m);
    sceVu0CopyMatrix(_fwHrchyCurRotMatrixVal, m);
    _fwHrchyCurRotVal++;
}

/*
 * _fwHrchyPopRotMatrix
 *	mのマトリクスをRotMatrixとしてポップする
 */
static inline void _fwHrchyPopRotMatrix(sceVu0FMATRIX m)
{
    _fwHrchyCurRotVal--;
    if (_fwHrchyCurrentVal < 0)
	return;
    sceVu0CopyMatrix(m, _fwHrchyStackRotMatrixVal[_fwHrchyCurRotVal]);
    if (! _fwHrchyCurRotVal)
	sceVu0UnitMatrix(_fwHrchyCurRotMatrixVal);
    else
	sceVu0CopyMatrix(_fwHrchyCurRotMatrixVal, m);
}

/*
 * Pushされているマトリクスの一番上のマトリクスをロードする
 */
static inline void _fwHrchyLoadRotMatrix(sceVu0FMATRIX m)
{
    sceVu0CopyMatrix(m, _fwHrchyCurRotMatrixVal);
}

/*
 * PushされているマトリクスをPopする
 */
static inline void _fwHrchyPopMatrix(sceVu0FMATRIX m)
{
    _fwHrchyCurrentVal--;
    if (_fwHrchyCurrentVal < 0)
	return;
    sceVu0CopyMatrix(m, _fwHrchyStackMatrixVal[_fwHrchyCurrentVal]);
    if (!_fwHrchyCurrentVal)
	sceVu0UnitMatrix(_fwHrchyCurMatrixVal);
    else
	sceVu0CopyMatrix(_fwHrchyCurMatrixVal, m);
}

/*
 * MatrixをPushする
 */
static inline void _fwHrchyPushMatrix(sceVu0FMATRIX m)
{
    sceVu0CopyMatrix(_fwHrchyStackMatrixVal[_fwHrchyCurrentVal], m);
    sceVu0CopyMatrix(_fwHrchyCurMatrixVal, m);
    _fwHrchyCurrentVal++;
    if (_fwHrchyCurrentVal >= STACKSIZE)
	return;
}

/*
 * Pushされているmatrixをロードする
 */
static inline void _fwHrchyLoadMatrix(sceVu0FMATRIX m)
{
    sceVu0CopyMatrix(m, _fwHrchyCurMatrixVal);
}
