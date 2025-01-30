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
 *                          Name : pad.c
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000      
 *
 */

/*******************************************************
 * 概要
 *	Frame Work内での Pad Operationの為の関数群
 *
 *	Frame Work内では 一つのパッドしか持たせてません。
 *	ただし、ここの関数のちょっとの改造で複数に対応できます。
 *	また fwPadState_t型の変数のビットメンバは
 *		～On
 *		～Switch
 *	がありますが、～Onは 押されている間常に１となるビット
 *	～Switchは 押された瞬間だけ１となるビットを意味します。
 *******************************************************/

#include <eekernel.h>
#include <libpad.h>
#include "pad.h"

/*******************************************************
 * Global Variables (substance definition)
 *******************************************************/
fwPadState_t	fwPadStateVal;

/*******************************************************
 * Static Variables
 *******************************************************/
u_long128	_fwPadBufVal[scePadDmaBufferMax] __attribute__((aligned(64)));

/*******************************************************
 * fwPadInit
 *	Padの初期化
 *******************************************************/
void fwPadInit(void)
{
    scePadInit(0);
    scePadPortOpen(0, 0, _fwPadBufVal);
}

/*******************************************************
 * fwPadCheck
 *	Pad Dataの作成（毎フレーム呼ぶこと）
 *	fwPadState_tの並びは libpad.hでの定義に則した
 *	並びになっており、短純なbit操作で得ることができる
 *	様になっているが、処理系依存なコードであることに
 *	注意
 *******************************************************/
void fwPadCheck(void)
{
    static u_short	oldpad = 0;
    u_short		pad;
    u_char		rdata[32];
    u_int		data;
    
    if (scePadRead(0, 0, rdata) > 0) 
		pad = 0xffff ^ ((rdata[2] << 8) | rdata[3]);
    else
	pad = 0;    

    data = ((u_int)pad) << 16;
    data |= pad & (~oldpad);
    fwPadStateVal = *((fwPadState_t *)&data);
    oldpad = pad;
}
