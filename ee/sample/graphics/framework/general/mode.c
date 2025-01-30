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
 *                          Name : mode.c
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000   
 *
 */

#include <eekernel.h>
#include <stdio.h>
#include <libvu0.h>
#include "defines.h"
#include "data.h"
#include "frame_t.h"
#include "micro.h"
#include "object.h"
#include "packbuf.h"
#include "pad.h"
#include "camera.h"	/* for Camera Mode */
#include "light.h"
#include "mode.h"
#include "info.h"
#include "micro.h"
#include "gmain.h"
#include "model.h"

/*******************************************************
 * �T�v
 *	Frame Work�ł̊e���[�h�𓝍����Ĉ������߂̊֐��Q
 *	�e���[�h�� mode_tbl.h�Œ�`����A
 *		_fwModeInit<MODE-NAME>
 *		_fwModeExec<MODE-NAME>
 *		_fwModeExit<MODE-NAME>
 *	�Ƃ������̂� ������/���s/�I���֐���mode.c��
 *	static�Ɏ���
 *******************************************************/

/*******************************************************
 * MODE TABLE DEFINES
 *******************************************************/
typedef void	_fwModeFunc_t(void);

#define DEFMODE(name)	#name,
static const char *mode_name[] = {
#include "mode_tbl.h"
};
#undef DEFMODE

#define DEFMODE(name)	static _fwModeFunc_t _fwModeInit##name;
#include "mode_tbl.h"
#undef DEFMODE

#define DEFMODE(name)	static _fwModeFunc_t _fwModeExec##name;
#include "mode_tbl.h"
#undef DEFMODE

#define DEFMODE(name)	static _fwModeFunc_t _fwModeExit##name;
#include "mode_tbl.h"
#undef DEFMODE

#define DEFMODE(name)	static _fwModeFunc_t _fwModeInfo##name;
#include "mode_tbl.h"
#undef DEFMODE

#define DEFMODE(name)	_fwModeInit##name,
static _fwModeFunc_t *_fwModeInitFuncVal[MODE_MAX] = {
#include "mode_tbl.h"
};
#undef DEFMODE

#define DEFMODE(name)	_fwModeExec##name,
static _fwModeFunc_t *_fwModeExecFuncVal[MODE_MAX] = {
#include "mode_tbl.h"
};
#undef DEFMODE

#define DEFMODE(name)	_fwModeInfo##name,
static _fwModeFunc_t *_fwModeInfoFuncVal[MODE_MAX] = {
#include "mode_tbl.h"
};
#undef DEFMODE

#define DEFMODE(name)	_fwModeExit##name,
static _fwModeFunc_t *_fwModeExitFuncVal[MODE_MAX] = {
#include "mode_tbl.h"
};
#undef DEFMODE


#define MODE_INFO_X	3
#define MODE_INFO_Y	15
#define MODE_MENU_X	3
#define MODE_MENU_Y	7

/*******************************************************
 * Global Variables
 *******************************************************/

/*******************************************************
 * Static Variables
 *******************************************************/
static int	_fwModeNumVal;		/* current mode number */

/*******************************************************
 * fwModeInit
 *	Mode�̏�����
 *	����: mode	�f�t�H���g�Ō���郂�[�h
 *******************************************************/
void fwModeInit(fwModeType_t mode)
{
    fwModeType_t m;

    _fwModeNumVal = mode;
    for (m = 0; m < MODE_MAX; m++)
	_fwModeInitFuncVal[m]();
}

/*******************************************************
 * fwModeExec
 *	Mode�̎��s
 *******************************************************/
void fwModeExec(void)
{
    static int	disp_info = 1;

    /* ���[�h���̂̃Z�b�g */
    sprintf(fwInfoStatusVal.mode, mode_name[_fwModeNumVal]);

    /* L1�������Ȃ��� R1�������� Mode Info��on/off */
    if (fwPadStateVal.L1On && fwPadStateVal.R1Switch)
	disp_info = !disp_info;

    /* ���[�h�֐��̎��s */
    _fwModeExecFuncVal[_fwModeNumVal]();

    /* �C���t�H���[�V�����iKey Config�Ȃǂ̕\���j*/
    if (disp_info)
	_fwModeInfoFuncVal[_fwModeNumVal]();
}

/*******************************************************
 * fwModeExit
 *	Mode�I������
 *******************************************************/
void fwModeExit(void)
{
    fwModeType_t	m;

    for (m = 0; m < MODE_MAX; m++)
	_fwModeExitFuncVal[m]();
}

/*******************************************************
 * fwModeChange
 *	Mode�̕ύX
 *	�ύX��i�� �ďo�����Ɉς˂��Ă���
 *******************************************************/
void fwModeChange(fwModeType_t mode)
{
    _fwModeNumVal = mode;
}

/*******************************************************
 * Camear Mode
 *******************************************************/
static void _fwModeInitCamera(void)
{
}

static void _fwModeExecCamera(void)
{
    fwCameraCtrl();
}

static void _fwModeExitCamera(void)
{
}

static void _fwModeInfoCamera(void)
{
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y,     "Change MODE       : Start");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 1, "------------------------");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 2, "Cam Mv Up/Down    : Rup/Rdown");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 3, "Cam Mv Left/Right : Rleft/Rright");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 4, "Cam Mv Front/Back : R1/R2");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 5, "Cam Rot X         : Lup/Ldown");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 6, "Cam Rot Y         : Lleft/Lright");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 7, "Cam Rot Z         : L1/L2");
}

/*******************************************************
 * Light Mode
 *******************************************************/
static void _fwModeInitLight(void)
{
}

static void _fwModeExecLight(void)
{
    fwLightCtrl();
}

static void _fwModeExitLight(void)
{
}

static void _fwModeInfoLight(void)
{
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y,     "Change MODE       : Start");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 1, "------------------------");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 2, "Change Light      : Select");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 3, "Light Mv Up/Down  : Rup/Rdon");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 4, "Light Mv Lft/Rght : Rleft/Rright");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 5, "Light Mv Frnt/Bck : L1/L2");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 6, "Item Cursor Move  : Lup/Ldown Lleft/Lright");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 7, "Item Value Change : L1/L2");
}

/*******************************************************
 * Micro Mode
 *******************************************************/
static void _fwModeInitMicro(void)
{
}

static void _fwModeExecMicro(void)
{
    fwMicroType_t	m;

    m = fwMicroGetCurrent();
    if (fwPadStateVal.LupSwitch)
	m++;
    if (fwPadStateVal.LdownSwitch)
	m--;
    if ((int)m < 0)	/* enum�^�� unsigned */
	m = MAX_MICRO - 1;
    if (m >= MAX_MICRO)
	m = 0;
    fwMicroChange(m);

    fwMicroCtrl();
}

static void _fwModeExitMicro(void)
{
}

static void _fwModeInfoMicro(void)
{
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y,     "Change MODE       : Start");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 1, "------------------------");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 2, "Change Micro : Lup/Ldown");
}

/*******************************************************
 * Model Mode
 *******************************************************/
static void _fwModeInitModel(void)
{
    fwModelInit();
}

static void _fwModeExecModel(void)
{
    fwModelCtrl();
}

static void _fwModeExitModel(void)
{
    fwModelExit();
}

static void _fwModeInfoModel(void)
{
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y,     "Change MODE       : Start");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 1, "------------------------");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 2, "Change Visible    : Select");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 3, "Cursor Mv Up/Down : Lup/Ldown");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 4, "Cursor Mv Lft/Rgt : Lleft/Lright");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 5, "Mdl Mv Up/Down    : Rup/Rdown");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 6, "Mdl Mv Left/Right : Rleft/Rrifht");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 7, "Mdl Mv Front/Back : R1/R2");
    fwInfoDraw(MODE_INFO_X, MODE_INFO_Y + 8, "Angle Value Change: L1/L2");
}
