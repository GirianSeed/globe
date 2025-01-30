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
 *                          Name : object.c
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000
 *
 */
#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libdma.h>
#include <libvu0.h>
#include <sifdev.h>
#include <string.h>
#include <malloc.h>
#include <math.h>


#include "defines.h"
#include "packbuf.h"
#include "data.h"
#include "frame_t.h"
#include "micro.h"
#include "object.h"
#include "camera.h"
#include "hrchy.h"
#include "anime.h"
#include "micro.h"
#include "light.h"
#include "gmain.h"
#include "pad.h"
#include "mode.h"
#include "info.h"
#include "shape.h"
#include "tex2d.h"
#include "share.h"

fwObject_t* fwObjectCreate(fwDataHead_t *data)
{
    fwObject_t	*obj;

    obj = (fwObject_t *) calloc(sizeof(fwObject_t), 1);
    obj->data = data;
    if (data->micro_data)
	obj->micro = fwMicroInit(data);
    if (data->tex2d_data)
	obj->tex2d = fwTex2dInit(data);
    if (data->shape_data)
	obj->shape = fwShapeInit(data);
    if (data->hrchy_data)
	obj->hrchy = fwHrchyInit(data);
    if (data->anime_data)
	obj->anime = fwAnimeInit(data);
    if (data->share_data)
	obj->share = fwShareInit(data);
    return obj;
}

void fwObjectDelete(fwObject_t *obj)
{
    if (obj->micro)
	fwMicroFinish(obj->micro);
    if (obj->tex2d)
	fwTex2dFinish(obj->tex2d);
    if (obj->shape)
	fwShapeFinish(obj->shape);
    if (obj->hrchy)
	fwHrchyFinish(obj->hrchy);
    if (obj->anime)
	fwAnimeFinish(obj->anime);
     if (obj->share)
	fwShareFinish(obj->share);
}

void fwObjectPreCalc(fwObject_t *obj)
{
    if (obj->micro)
	fwMicroPreCalc(obj->micro);
    if (obj->tex2d)
	fwTex2dPreCalc(obj->tex2d);
    if (obj->shape)
	fwShapePreCalc(obj->shape);
    if (obj->hrchy)
	fwHrchyPreCalc(obj->hrchy);
    if (obj->anime)
	fwAnimePreCalc(obj->anime);
    if (obj->share)
	fwSharePreCalc(obj->share);
}

void fwObjectPostCalc(fwObject_t *obj)
{
    if (obj->micro)
	fwMicroPostCalc(obj->micro, 1);
    if (obj->tex2d)
	fwTex2dPostCalc(obj->tex2d);
    if (obj->shape)
	fwShapePostCalc(obj->shape);
    if (obj->hrchy)
	fwHrchyPostCalc(obj->hrchy);
    if (obj->anime)
	fwAnimePostCalc(obj->anime);
    if (obj->share)
	fwSharePostCalc(obj->share);
}
