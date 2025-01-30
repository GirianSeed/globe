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
 *                          Name : anime.c
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000  
*/

#include <eekernel.h>
#include <malloc.h>
#include "defines.h"
#include "data.h"
#include "frame_t.h"
#include "micro.h"
#include "object.h"
#include "hrchy.h"
#include "anime.h"

/*******************************************************
 * constant
 *******************************************************/
#define SIZIDX 2
#define NUMIDX 3
#define QWSIZE 4

/*******************************************************
 * static function declarations
 *******************************************************/
static fwAnimeFrame_t* _fwAnimeInitData(fwAnimeFrame_t *, u_int *, u_int *, u_int *, u_int *);
static void _fwAnimeSet(fwAnimeFrame_t *);
static u_int _fwAnimeGetKeypoint(u_int, u_int *, u_int);
static float _fwAnimeCalcLinear(float, float, float);

/***************************************************
 * function definitions
 ***************************************************/

/*
 * fwAnimeInit(data)
 *	es2rawが吐いたデータのアドレスdataから
 *	animation dataの為の Anime CLASS Instance
 *	（fwAnimeFrame_t型）を作成して返します
 *	作成失敗は NULLを返します
 */
fwAnimeFrame_t* fwAnimeInit(fwDataHead_t *data)
{
    fwAnimeFrame_t	*anime_frame;

    anime_frame = (fwAnimeFrame_t *)memalign(16, sizeof(fwAnimeFrame_t));
    if (anime_frame == NULL)
	return NULL;
    anime_frame = _fwAnimeInitData(anime_frame, data->anime_data,
				   data->keyframe,
				   data->keyvalue,
				   data->hrchy_data);
    if (anime_frame == NULL)
	return NULL;

    return anime_frame;
}

/*
 * fwAnimePreCalc(af)
 *	Anime CLASS afのPre Calcを行う
 */
void fwAnimePreCalc(fwAnimeFrame_t *af)
{
    af->frame++;
    _fwAnimeSet(af);
}

/*
 * fwAnimePostCalc(af)
 *	Anime CLASS afのPost Calcを行う
 */
void fwAnimePostCalc(fwAnimeFrame_t *af)
{
}

/*
 * fwAnimeFinish(af)
 *	Anime CLASS afを消す
 */
void fwAnimeFinish(fwAnimeFrame_t *af)
{
    free((u_int *)Paddr(af->keyframe));
    free((u_int *)Paddr(af->keyvalue));
    free((u_int *)Paddr(af));
}

/*
 * _fwAnimeInitData(af, anmP, frmP, valP, hrcP)
 *	Anime CLASS afのデータ初期化を行う
 *		anmP : anime data pointer
 *		frmP : key frame data pointer
 *		valP : key value data pointer
 *		hrcP : hierarchy data pointer
 */
static fwAnimeFrame_t* _fwAnimeInitData(fwAnimeFrame_t *af,
					u_int *anmP,
					u_int *frmP,
					u_int *valP,
					u_int *hrcP)
{
    int		i;
    int		num;

    /* Animation Data */
    af->frame	= 0;
    af->num	= anmP[NUMIDX];
    af->anime	= (fwAnime_t *)(anmP + QWSIZE);
    af->hrchy	= (fwHrchy_t *)(hrcP + QWSIZE);

    /* KeyFrame Data */
    num		= frmP[NUMIDX];
    af->keyframe= (u_int **)memalign(16, sizeof(u_int *) * num);
    if (af->keyframe == NULL)
	return NULL;
    frmP += QWSIZE;
    for (i = 0; i < num; i++) {
	af->keyframe[i] = (u_int *)(frmP + QWSIZE);
	frmP += frmP[SIZIDX] + QWSIZE;
    }

    /* KeyValue Data */
    num = valP[NUMIDX];
    af->keyvalue = (float **)memalign(16, sizeof(u_int *) * num);
    valP += QWSIZE;
    for (i = 0; i < num; i++) {
	af->keyvalue[i] = (float *)(valP + QWSIZE);
	valP += valP[SIZIDX] + QWSIZE;
    }

    return af;
}

/*
 * _fwAnimeSet(af)
 *	Anime CLASS afの アニメーション計算を行い、
 *	階層構造のtrans/rotを書き換える
 */
static void _fwAnimeSet(fwAnimeFrame_t *af)
{
    u_int	nkey, hier;
    u_int	frm, key, end;
    u_int	*kf;
    float	*kv;
    float	t;
    float	a, b;
    float	f;
    u_int	i, j;

    for (i = 0; i < af->num; i++) {
	nkey = af->anime[i].nkey;
	hier = af->anime[i].hierarchy;
	kf = af->keyframe[af->anime[i].keyframe];
	kv = af->keyvalue[af->anime[i].keyvalue];
	end = kf[nkey - 1];

	frm = (u_int) (af->frame % end);
	key = _fwAnimeGetKeypoint(frm, kf, nkey);

	t = (float) (frm - kf[key]) / (float)(kf[key + 1] - kf[key]);

	/* trans */
	for (j = 0; j < 3; j++) {
	    a = kv[key * 8 + j];
	    b = kv[(key + 1) *8 + j];
	    f = _fwAnimeCalcLinear(t, a, b);
	    af->hrchy[hier].trans[j] = f;
	}
	af->hrchy[hier].trans[3] = 0.0f;

	/* rot */
	for (j = 0; j < 3; j++) {
	    a = kv[key * 8 + 4 + j];
	    b = kv[(key + 1) * 8 + 4 + j];
	    f = _fwAnimeCalcLinear(t, a, b);
	    af->hrchy[hier].rot[j] = f;
	}
	af->hrchy[hier].rot[3] = 0.0f;
    }
}

/*
 * _fwAnimeGetKeypoint(frm, *kf, nkey)
 *	Anime CLASS afの kfで示されるkey frame table(要素はnkey個)
 *	の中で frmが存在する場所(index)を得る
 */
static u_int _fwAnimeGetKeypoint(u_int frm, u_int *kf, u_int nkey)
{
    u_int	i;
    u_int	ret;

    ret = 0;
    for (i = 0; i < nkey; i++) {
	if ((frm >= kf[i]) && (frm <= kf[i + 1])) {
	    ret = i;
	    break;
	}
    }
    return ret;
}

/*
 * _fwAnimeCalcLinear(t, a, b)
 *	Linear Animationの値を計算する
 */
static float _fwAnimeCalcLinear(float t, float a, float b)
{
    float f;

    f = (1.0f - t) * a + t * b;
    return f;
}
