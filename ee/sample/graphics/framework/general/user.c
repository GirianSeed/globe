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
 *                          Name : user.c
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000    
 */

/***************************************************
 * 概要
 *	User Application側固有に書き換えるであろう
 *	部分を一部抽出
 *	今後、application依存部分をこちらに移す予定
 *	(sourceの再利用性を高めるため)
 *
 *	実際のapplicationではこれらは書き換えられ、
 *	もしくは 削除されると思われるものを中心に
 *	抜き出してある
 ***************************************************/

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

/*
  micro.dsmで定義されているマイクロコード(packet)の各種データ
*/
extern u_int UserMicro_base[] __attribute__((section(".vudata")));
extern u_int UserMicro_cull[] __attribute__((section(".vudata")));
extern u_int UserMicro_pointl[] __attribute__((section(".vudata")));
extern u_int UserMicro_spotl[] __attribute__((section(".vudata")));
extern u_int UserMicro_fog[] __attribute__((section(".vudata")));
extern u_int UserMicro_anti[] __attribute__((section(".vudata")));

static const char *_UserMicroName[]={  // for DrawStatus
    "basic", "cull", "point light", "spot light", "fog", "AA1-polygon AntiAlias"
};

static fwMicroTable_t	_UserMicroTblVal[] =
{ 
    { UserMicro_base,	FW_MATTR_NONE },	/* BASE */
    { UserMicro_cull,	FW_MATTR_NONE },	/* CULL */
    { UserMicro_pointl,	FW_MATTR_NONE },	/* POINT */
    { UserMicro_spotl,	FW_MATTR_NONE },	/* SPOT */
    { UserMicro_fog,	FW_MATTR_FGE },		/* FOG */
    { UserMicro_anti,	FW_MATTR_ANTI }		/* ANTI */
};

static int	_UserMicroTblNumVal = sizeof(_UserMicroTblVal) / sizeof(fwMicroTable_t);

/*
  UserInit
  	User定義部分の初期化をするための関数
	現在
		・ micro code table
	を設定することを行っている
*/
void UserInit(void)
{
    /* Micro Code Tableの設定 */
    fwMicroSetTable(_UserMicroTblVal, _UserMicroTblNumVal);
}

/*
  UserMain
  	User定義部分の実行を行うための関数
	現在
		・ micro code nameのinformationへの登録
	を行っている
*/
void UserMain(void)
{
    /* Information へ current micro codeのnameをセット */
    fwInfoStatusVal.micro_name = _UserMicroName[fwMicroGetCurrent()];
}
