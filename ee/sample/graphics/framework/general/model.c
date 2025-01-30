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
 *                          Name : model.c
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            0.40      Jun,21,2000    kaneko
 */

#include <eekernel.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sifdev.h>
#include "defines.h"
#include "frame_t.h"
#include "data.h"
#include "micro.h"
#include "object.h"
#include "model.h"
#include "info.h"
#include "pad.h"
#include "camera.h"
#include "light.h"

/*******************************************************
 * �T�v
 *	MODEL MODE�Ɋւ���֐��Q�������܂�
 *	�����ł�MODEL�Ƃ� es2raw�œf���ꂽ��̃o�C�i���P�ʂł�
 *	MODEL(object)�̎��ł�
 *	MODEL MODE�łł��邱�Ƃ͈ȉ��̒ʂ�ł�
 *		1. �z�X�g�}�V�������MODEL�f�[�^�̃��[�h
 *		2. ���[�h����MODEL��Visible/Invisible�̐ݒ�
 *		3. Model�̈ړ�(Hierarcy Root���ړ�)
 *******************************************************/

typedef struct {
    unsigned	visible		: 1;	/* if visible then 1 */
    unsigned	loaded		: 1;	/* if loaded then 1 */
} fwModelManage_t;

typedef struct {
    fwModelManage_t	flags;
    fwObject_t		*objdata;
} fwModel_t;

static fwModel_t	*_fwModel;

static const char *_fwModelFname[] = {
    "data/dino.bin",
    "data/logo.bin",
    "data/APEWALK39.bin",
    "data/shuttle.bin",
    "data/earth.bin",
    "data/mbox.bin"
};
static int _fwModelnbFile = sizeof(_fwModelFname) / sizeof(char *);

static fwObject_t* _fwModelLoad(const char *);

void fwModelInit(void)
{
    _fwModel = (fwModel_t *)calloc(sizeof(fwModel_t) * MAX_MODEL, 1);
}

void fwModelCtrl(void)
{
#define INFO_X	(3)
#define INFO_Y	(6)
    int		i;
    static int	cursor = 0;
    static int	ang_cursor = 0;
    fwHrchyFrame_t	*hf = NULL;

    /* move model-cursor (Lup/Ldown)*/
    if (fwPadStateVal.LupSwitch)
	--cursor;
    if (fwPadStateVal.LdownSwitch)
	++cursor;
    if (cursor < 0)
	cursor = 0;
    if (cursor > _fwModelnbFile - 1)
	cursor = _fwModelnbFile - 1;

    /* move angle-cursor (Lleft/Lright) */
    if (fwPadStateVal.LleftSwitch)
	--ang_cursor;
    if (fwPadStateVal.LrightSwitch)
	++ang_cursor;
    if (ang_cursor < 0)
	ang_cursor = 0;
    if (ang_cursor > 2)
	ang_cursor = 2;

    /* change visible (Lleft/Lright)*/
    /* �������f�����܂����[�h����Ă��Ȃ������烍�[�h���� */
    if (fwPadStateVal.SelectSwitch) /* Off */
	_fwModel[cursor].flags.visible = !_fwModel[cursor].flags.visible;
    if (_fwModel[cursor].flags.visible) {
	if (!_fwModel[cursor].flags.loaded) {
	    /* data load */
	    fwInfoDraw(5, 12, "**** NOW LOADING DATA ****");
	    _fwModel[cursor].objdata = _fwModelLoad(_fwModelFname[cursor]);
	    if (_fwModel[cursor].objdata == NULL) {
		/* load failed */
		_fwModel[cursor].flags.loaded = _fwModel[cursor].flags.visible = 0;
	    } else {
		_fwModel[cursor].flags.loaded = 1;
	    }
	}
    }
	
    /* change model coordinate */
    if (fwModelIsActive(cursor)) {
	hf = _fwModel[cursor].objdata->hrchy;
	if (fwPadStateVal.RleftOn)
	    hf->root_trans[0] -= 1.0f;
	if (fwPadStateVal.RrightOn)
	    hf->root_trans[0] += 1.0f;
	if (fwPadStateVal.RupOn)
	    hf->root_trans[1] -= 1.0f;
	if (fwPadStateVal.RdownOn)
	    hf->root_trans[1] += 1.0f;
	if (fwPadStateVal.R1On)
	    hf->root_trans[2] -= 1.0f;
	if (fwPadStateVal.R2On)
	    hf->root_trans[2] += 1.0f;
	if (fwPadStateVal.L1On)
	    hf->root_rot[ang_cursor] += 0.01f;
	if (fwPadStateVal.L2On)
	    hf->root_rot[ang_cursor] -= 0.01f;

	/* check rotation angle */
	if (hf->root_rot[ang_cursor] > PI)
	    hf->root_rot[ang_cursor] -= 2.0f * PI;
	if (hf->root_rot[ang_cursor] < -PI)
	    hf->root_rot[ang_cursor] += 2.0f * PI;
    }

    /* information */
    for (i = 0; i < _fwModelnbFile; i++)
	fwInfoDraw(INFO_X + 3, INFO_Y + i, "%3s : %s",
		   _fwModel[i].flags.visible ? "On" : "Off", _fwModelFname[i]);
    fwInfoDraw(INFO_X, INFO_Y + cursor, ">");
    if (hf) {
	fwInfoDraw(INFO_X + 3, INFO_Y + _fwModelnbFile, "POS [%7.2f, %7.2f, %7.2f]",
		   hf->root_trans[0], hf->root_trans[1], hf->root_trans[2]);
	fwInfoDraw(INFO_X + 3, INFO_Y + _fwModelnbFile + 2, 
		   "ANG [%7.4f, %7.4f, %7.4f]",
		   hf->root_rot[0],
		   hf->root_rot[1],
		   hf->root_rot[2]);
	fwInfoDraw(INFO_X + 11 + ang_cursor * 9, INFO_Y + _fwModelnbFile + 1, "V");
    }
}

void fwModelExit(void)
{
    fwModelID_t	id;

    for (id = 0; id < MAX_MODEL; id++) {
	if (fwModelIsActive(id)) {
	    fwObjectDelete(_fwModel[id].objdata);
	}
    }
    free(_fwModel);
}

/*
  �C��ID�̃��f�������݂���Visible��ԂȂ�1��Ԃ�
  
*/
int fwModelIsActive(fwModelID_t id)
{
    if (id >= MAX_MODEL)
	return 0;
    if (_fwModel[id].flags.loaded && _fwModel[id].flags.visible)
	return 1;
    return 0;
}

/*
  �C��ID�̃��f���̃f�[�^�̃A�h���X��Ԃ�
*/
fwObject_t* fwModelGetDataAddr(fwModelID_t id)
{
    return _fwModel[id].objdata;
}

/*
  Model�̃��[�h
*/
fwObject_t* _fwModelLoad(const char *fname)
{
    int		i;
    fwObject_t	*obj;
    char	file[256];
    size_t	size;
    u_int	*dptr;
    int		fd;

    sprintf(file, "host0:%s", fname);
    if ((fd = sceOpen(file, SCE_RDONLY)) < 0) {
	printf("Can't open file %s\n", fname);
	return NULL;
    }
    size = sceLseek(fd, 0, SCE_SEEK_END);
    sceLseek(fd, 0, SCE_SEEK_SET);
    dptr = (u_int *) memalign(64, size);
    if (dptr == NULL) {
	printf("Can't allocate memory for file reading %s\n", fname);
	return NULL;
    }
    if (sceRead(fd, (u_int *)dptr, size) < 0) {
	printf("Cnat' read file %s\n", fname);
	return NULL;
    }
    FlushCache(0);
    sceClose(fd);

    /* 
       dptr������ fwDataHead_t�^�̃f�[�^�̓I�t�Z�b�g�f�[�^�ɂȂ��Ă���
       �̂� �������������
       ������ NULL���͂����Ă���ꍇ������̂Œ���
    */
    for (i = 0; i < sizeof(fwDataHead_t)/sizeof(u_int *); i++) {
	if (dptr[i] != (u_int)NULL)
	    dptr[i] += (u_int)dptr;
    }
    obj = fwObjectCreate((fwDataHead_t *)dptr);
    return obj;
}
