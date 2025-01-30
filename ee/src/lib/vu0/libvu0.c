/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                      Emotion Engine Library
 *                          Version  0.10
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        libvu0 - libvu0.c 
 *                       Library for VU0 Macro Code
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.10           Mar,25,1999     shino    
 */

#include"libvu0.h"
#include <eekernel.h>
#include<eeregs.h>
#include<stdio.h>


static void _sceVu0ecossin(float t);

/* mathematical functions */

//==========================================================================
// sceVu0ApplyMatrix		ベクトルにマトリックスを乗算する。
// 形式
// 	void sceVu0ApplyMatrix(sceVu0FVECTOR v0, sceVu0FMATRIX m0, sceVu0FVECTOR v1)
// 引数
// 	v1		入力:ベクトル
// 	m0		入力:マトリックス
// 	v0		出力:ベクトル
// 
// 解説
// 	マトリックスmにベクトルv0を右から乗算して、v1に与える
// 
// 		v0=m0*v1
// 返り値
// 	なし
//-------------------------------------------------------------------------- 	

void sceVu0ApplyMatrix(sceVu0FVECTOR v0, sceVu0FMATRIX m0,sceVu0FVECTOR v1)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lqc2    vf4,0x0(%1)\n\
	lqc2    vf5,0x10(%1)\n\
	lqc2    vf6,0x20(%1)\n\
	lqc2    vf7,0x30(%1)\n\
	lqc2    vf8,0x0(%2)\n\
	vmulax.xyzw	ACC,   vf4,vf8\n\
	vmadday.xyzw	ACC,   vf5,vf8\n\
	vmaddaz.xyzw	ACC,   vf6,vf8\n\
	vmaddw.xyzw	vf9,vf7,vf8\n\
	sqc2    vf9,0x0(%0)\n\
	": : "r" (v0) , "r" (m0) ,"r" (v1): "memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}


//==========================================================================
// 
// sceVu0MulMatrix		２つマトリックスの積を求める
// 形式
// 	void sceVu0MulMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FMATRIX m2)
// 引数
// 	m1,m2		入力:マトリックス
// 	m0		出力:マトリックス
// 解説
// 	マトリックスm1にマトリックスm2を右から乗算して、m0に与える
// 
// 		m0=m1*m2
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	

void sceVu0MulMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1,sceVu0FMATRIX m2)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lqc2    vf4,0x0(%2)\n\
	lqc2    vf5,0x10(%2)\n\
	lqc2    vf6,0x20(%2)\n\
	lqc2    vf7,0x30(%2)\n\
	li    $7,4\n\
_loopMulMatrix:\n\
	lqc2    vf8,0x0(%1)\n\
	vmulax.xyzw	ACC,   vf4,vf8\n\
	vmadday.xyzw	ACC,   vf5,vf8\n\
	vmaddaz.xyzw	ACC,   vf6,vf8\n\
	vmaddw.xyzw	vf9,vf7,vf8\n\
	sqc2    vf9,0x0(%0)\n\
	addi    $7,-1\n\
	addi    %1,0x10\n\
	addi    %0,0x10\n\
	bne    $0,$7,_loopMulMatrix\n\
	":: "r" (m0), "r" (m2), "r" (m1) : "$7", "memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}

//==========================================================================
// 
// sceVu0OuterProduct		２つのベクトルの外積を求める
// 形式
// 	void sceVu0OuterProduct(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
// 引数
// 	v1,v2		入力:ベクトル
// 	v0		出力:ベクトル
// 解説
// 	ベクトルv1,v2の外積を求めてv0に与える
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	
 
void sceVu0OuterProduct(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__
	("\n\
	lqc2    vf4,0x0(%1)\n\
	lqc2    vf5,0x0(%2)\n\
	vopmula.xyz	ACC,vf4,vf5\n\
	vopmsub.xyz	vf6,vf5,vf4\n\
	vsub.w vf6,vf6,vf6		#vf6.xyz=0;\n\
	sqc2    vf6,0x0(%0)\n\
	": : "r" (v0) , "r" (v1) ,"r" (v2) : "memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}

//==========================================================================
// 
// sceVu0InnerProduct		２つのベクトルの内積を求める
// 形式
// 	float sceVu0InnerProduct(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
// 引数
// 	v0,v1		入力:ベクトル
// 解説
// 	ベクトルv0,v1の内積を求める
// 返り値
// 	内積
// 
//-------------------------------------------------------------------------- 	

float sceVu0InnerProduct(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
    register float ret;
#ifdef SCE_VU0_SAFE_CONTEXT
    int r = DI();
#endif
    __asm__ __volatile__("\n\
	lqc2    vf4,0x0(%1)\n\
	lqc2    vf5,0x0(%2)\n\
	vmul.xyz vf5,vf4,vf5\n\
	vaddy.x vf5,vf5,vf5\n\
	vaddz.x vf5,vf5,vf5\n\
	qmfc2   $2 ,vf5\n\
	mtc1    $2,%0\n\
	": "=f" (ret) :"r" (v0) ,"r" (v1) :"$2", "memory" );
#ifdef SCE_VU0_SAFE_CONTEXT
    if (r) EI();
#endif
    return ret;
}

//==========================================================================
// sceVu0Normalize		ベクトルの正規化を行う
// 形式
// 	void sceVu0Normalize(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
// 引数
// 	v1		入力:ベクトル
// 	v0		出力:ベクトル
// 解説
// 	ベクトルv0の正規化を行いv1に与える
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	
void sceVu0Normalize(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
#if 0
    __asm__ __volatile__("\n\
	lqc2    vf4,0x0(%1)\n\
	vmul.xyz vf5,vf4,vf4\n\
	vaddy.x vf5,vf5,vf5\n\
	vaddz.x vf5,vf5,vf5\n\
	\n\
	vrsqrt Q,vf0w,vf5x\n\
	vsub.xyzw vf6,vf0,vf0		#vf6.xyzw=0;\n\
	vaddw.xyzw vf6,vf6,vf4		#vf6.w=vf4.w;\n\
	vwaitq\n\
	\n\
	vmulq.xyz  vf6,vf4,Q\n\
	sqc2    vf6,0x0(%0)\n\
	": : "r" (v0) , "r" (v1) : "memory");

#else   //rsqrt bug 

    __asm__ __volatile__("\n\
        lqc2    vf4,0x0(%1)\n\
        vmul.xyz vf5,vf4,vf4\n\
        vaddy.x vf5,vf5,vf5\n\
        vaddz.x vf5,vf5,vf5\n\
	\n\
        vsqrt Q,vf5x\n\
        vwaitq\n\
        vaddq.x vf5x,vf0x,Q\n\
        vnop\n\
        vnop\n\
        vdiv    Q,vf0w,vf5x\n\
        vsub.xyzw vf6,vf0,vf0           #vf6.xyzw=0;\n\
        vwaitq\n\
	\n\
        vmulq.xyz  vf6,vf4,Q\n\
        sqc2    vf6,0x0(%0)\n\
        ": : "r" (v0) , "r" (v1) : "memory");
#endif
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}


//==========================================================================
// 
// sceVu0TransposeMatrix	マトリックスの転置行列を求める
// 形式
// 	void sceVu0TransposeMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1)
// 引数
// 	m1		入力:マトリックス
// 	m0		出力:マトリックス
// 解説
// 	マトリックスm1の転置行列を求めm0に与える
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	

void sceVu0TransposeMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lq $8,0x0000(%1)\n\
	lq $9,0x0010(%1)\n\
	lq $10,0x0020(%1)\n\
	lq $11,0x0030(%1)\n\
	\n\
	pextlw     $12,$9,$8\n\
	pextuw     $13,$9,$8\n\
	pextlw     $14,$11,$10\n\
	pextuw     $15,$11,$10\n\
	\n\
	pcpyld     $8,$14,$12\n\
	pcpyud     $9,$12,$14\n\
	pcpyld     $10,$15,$13\n\
	pcpyud     $11,$13,$15\n\
	\n\
	sq $8,0x0000(%0)\n\
	sq $9,0x0010(%0)\n\
	sq $10,0x0020(%0)\n\
	sq $11,0x0030(%0)\n\
	": : "r" (m0) , "r" (m1) : "memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}


//==========================================================================
//sceVu0InversMatrix	逆行列 (4x4) を求める。（行列は正則と仮定する）
// 形式
//	void sceVu0InversMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1);
// 引数
// 	m1		入力:マトリックス(回転 & 平行移動)
// 	m0		出力:マトリックス
// 解説
// 	マトリックスm1の逆行列を求めて、マトリックスm0に与える
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	

void sceVu0InversMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lq $8,0x0000(%1)\n\
	lq $9,0x0010(%1)\n\
	lq $10,0x0020(%1)\n\
	lqc2 vf4,0x0030(%1)\n\
	\n\
	vmove.xyzw vf5,vf4\n\
	vsub.xyz vf4,vf4,vf4		#vf4.xyz=0;\n\
	vmove.xyzw vf9,vf4\n\
	qmfc2    $11,vf4\n\
	\n\
	#転置\n\
	pextlw     $12,$9,$8\n\
	pextuw     $13,$9,$8\n\
	pextlw     $14,$11,$10\n\
	pextuw     $15,$11,$10\n\
	pcpyld     $8,$14,$12\n\
	pcpyud     $9,$12,$14\n\
	pcpyld     $10,$15,$13\n\
	\n\
	qmtc2    $8,vf6\n\
	qmtc2    $9,vf7\n\
	qmtc2    $10,vf8\n\
	\n\
	#内積\n\
	vmulax.xyz	ACC,   vf6,vf5\n\
	vmadday.xyz	ACC,   vf7,vf5\n\
	vmaddz.xyz	vf4,vf8,vf5\n\
	vsub.xyz	vf4,vf9,vf4\n\
	\n\
	sq $8,0x0000(%0)\n\
	sq $9,0x0010(%0)\n\
	sq $10,0x0020(%0)\n\
	sqc2 vf4,0x0030(%0)\n\
	": : "r" (m0) , "r" (m1):"$8","$9","$10","$11","$12","$13","$14","$15", "memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}


//==========================================================================
//sceVu0DivVector	v0 = v1/q  (DIV + MULq)
// 形式
//	void sceVu0DivVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float q)
// 引数
// 	v1		入力:ベクトル
// 	q		入力:スカラー
// 	v0		出力:ベクトル
// 解説
// 	ベクトルv1をスカラーqで除算して、ベクトルv0に与える
// 返り値
// 	なし
//
//-------------------------------------------------------------------------- 	
void sceVu0DivVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float q)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lqc2    vf4,0x0(%1)\n\
	mfc1    $8,%2\n\
	qmtc2    $8,vf5\n\
	vdiv    Q,vf0w,vf5x\n\
	vwaitq\n\
	vmulq.xyzw      vf4,vf4,Q\n\
	sqc2    vf4,0x0(%0)\n\
	": : "r" (v0) , "r" (v1), "f" (q):"$8", "memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}

//==========================================================================
//sceVu0DivVectorXYZ	v0.xyz = v1/q (w を保存したい除算)
// 形式
//	void sceVu0DivVectorXYZ(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float q)
// 引数
// 	v1		入力:ベクトル
// 	q		入力:スカラー
// 	v0		出力:ベクトル
// 解説
// 	ベクトルv1のx,y,zの要素をスカラqで除算して、ベクトルv0に与える
//	ベクトルv1のw要素はそのままベクトルv0に返す
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	

void sceVu0DivVectorXYZ(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float q)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lqc2    vf4,0x0(%1)\n\
	mfc1    $8,%2\n\
	qmtc2    $8,vf5\n\
	\n\
	vdiv    Q,vf0w,vf5x\n\
	vwaitq\n\
	vmulq.xyz      vf4,vf4,Q\n\
	\n\
	sqc2    vf4,0x0(%0)\n\
	": : "r" (v0) , "r" (v1), "f" (q) : "$8", "memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}

//==========================================================================
//sceVu0InterVector	内挿 
// 形式
//	void sceVu0InterVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2, float t)
// 引数
// 	v1,v2		入力:ベクトル
//	t		入力:内挿値	
// 	v0		出力:ベクトル
// 解説
// 	// v0 = v1*t + v2*(1-t) （内挿)
// 返り値
// 	なし
//
//-------------------------------------------------------------------------- 	
void sceVu0InterVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2, float t)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lqc2    vf4,0x0(%1)\n\
	lqc2    vf5,0x0(%2)\n\
	mfc1    $8,%3\n\
	qmtc2    $8,vf6\n\
	\n\
        vaddw.x    vf7,vf0,vf0	#vf7.x=1;\n\
        vsub.x    vf8,vf7,vf6	#vf8.x=1-t;\n\
	vmulax.xyzw	ACC,   vf4,vf6\n\
	vmaddx.xyzw	vf9,vf5,vf8\n\
	\n\
	sqc2    vf9,0x0(%0)\n\
	": : "r" (v0) , "r" (v1), "r" (v2), "f" (t) : "$8", "memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}

//==========================================================================
//sceVu0AddVector	4並列加算 (ADD/xyzw)
// 形式
//	void sceVu0AddVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
// 引数
// 	v1,v2		入力:ベクトル
// 	v0		出力:ベクトル
// 解説
// 	ベクトルv1の各要素とベクトルv2の各要素を各々加算する。
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	
void sceVu0AddVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lqc2    vf4,0x0(%1)\n\
	lqc2    vf5,0x0(%2)\n\
	vadd.xyzw	vf6,vf4,vf5\n\
	sqc2    vf6,0x0(%0)\n\
	": : "r" (v0) , "r" (v1), "r" (v2) : "memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}

//==========================================================================
//sceVu0SubVector		4並列減算 (SUB/xyzw)
// 形式
//	void sceVu0SubVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
// 引数
// 	v1,v2		入力:ベクトル
// 	v0		出力:ベクトル
// 解説
// 	ベクトルv1の各要素とベクトルv2の各要素を各々減算する。
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	

void sceVu0SubVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lqc2    vf4,0x0(%1)\n\
	lqc2    vf5,0x0(%2)\n\
	vsub.xyzw	vf6,vf4,vf5\n\
	sqc2    vf6,0x0(%0)\n\
	": : "r" (v0) , "r" (v1), "r" (v2) : "memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}

//==========================================================================
//sceVu0MulVector	4並列積算（MUL/xyzw: 内積ではないことに注意）
// 形式
//	void sceVu0MulVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
// 引数
// 	v1,v2		入力:ベクトル
// 	v0		出力:ベクトル
// 解説
// 	ベクトルv1の各要素とベクトルv2の各要素を各々乗算する。
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	

void sceVu0MulVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lqc2    vf4,0x0(%1)\n\
	lqc2    vf5,0x0(%2)\n\
	vmul.xyzw	vf6,vf4,vf5\n\
	sqc2    vf6,0x0(%0)\n\
	": : "r" (v0) , "r" (v1), "r" (v2) : "memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}

//==========================================================================
//sceVu0ScaleVector	スカラー値を乗算する (MULx/xyzw)
// 形式
//	void sceVu0ScaleVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float t)
// 引数
// 	v1		入力:ベクトル
// 	t		入力:スカラー
// 	v0		出力:ベクトル
// 解説
// 	ベクトルv1にスカラーtを乗算してベクトルv0に与える
// 返り値
// 	なし
//
//-------------------------------------------------------------------------- 	

void sceVu0ScaleVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float t)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lqc2    vf4,0x0(%1)\n\
	mfc1    $8,%2\n\
	qmtc2    $8,vf5\n\
	vmulx.xyzw	vf6,vf4,vf5\n\
	sqc2    vf6,0x0(%0)\n\
	": : "r" (v0) , "r" (v1), "f" (t):"$8", "memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}

//==========================================================================
//sceVu0TransMatrix	行列の平行移動
// 形式
//	void sceVu0TransMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FVECTOR tv);
// 引数
// 	tv		入力:移動ベクトル
// 	m1		入力:マトリックス
// 	m0		出力:マトリックス
// 解説
// 	与えられたベクトル分、マトリックスを平行移動する
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	
void sceVu0TransMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FVECTOR tv)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lqc2    vf4,0x0(%2)\n\
	lqc2    vf5,0x30(%1)\n\
	\n\
	lq    $7,0x0(%1)\n\
	lq    $8,0x10(%1)\n\
	lq    $9,0x20(%1)\n\
	vadd.xyz	vf5,vf5,vf4\n\
	sq    $7,0x0(%0)\n\
	sq    $8,0x10(%0)\n\
	sq    $9,0x20(%0)\n\
	sqc2    vf5,0x30(%0)\n\
	": : "r" (m0) , "r" (m1), "r" (tv):"$7","$8","$9","memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}

//==========================================================================
//sceVu0CopyVector		行列の複写 (lq と sq の組合せ。alignment check 用に)
// 形式
//	void sceVu0CopyVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1);
// 引数
// 	m1		入力:マトリックス
// 	m0		出力:マトリックス
// 解説
// 	マトリックスm1をマトリックスm0にコピーする。
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	

void sceVu0CopyVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lq    $6,0x0(%1)\n\
	sq    $6,0x0(%0)\n\
	": : "r" (v0) , "r" (v1):"$6", "memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}


//==========================================================================
//sceVu0CopyMatrix		行列の複写 (lq と sq の組合せ。alignment check 用に)
// 形式
//	void sceVu0CopyMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1);
// 引数
// 	m1		入力:マトリックス
// 	m0		出力:マトリックス
// 解説
// 	マトリックスm1をマトリックスm0にコピーする。
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	

void sceVu0CopyMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lq    $6,0x0(%1)\n\
	lq    $7,0x10(%1)\n\
	lq    $8,0x20(%1)\n\
	lq    $9,0x30(%1)\n\
	sq    $6,0x0(%0)\n\
	sq    $7,0x10(%0)\n\
	sq    $8,0x20(%0)\n\
	sq    $9,0x30(%0)\n\
	": : "r" (m0) , "r" (m1):"$6","$7","$8","$9","memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}

//==========================================================================
// sceVu0FTOI4Vector	FTOI4
// 形式
//	void sceVu0FTOI4Vector(sceVu0IVECTOR v0, sceVu0FVECTOR v1);
// 引数
// 	v1		入力:ベクトル
// 	v0		出力:ベクトル
// 解説
// 	与えられたベクトルの要素を整数に変換する
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	

void sceVu0FTOI4Vector(sceVu0IVECTOR v0, sceVu0FVECTOR v1)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lqc2    vf4,0x0(%1)\n\
	vftoi4.xyzw	vf5,vf4\n\
	sqc2    vf5,0x0(%0)\n\
	": : "r" (v0) , "r" (v1) : "memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}

//==========================================================================
//sceVu0FTOI0Vector 	FTOI0
// 形式
//	void sceVu0FTOI0Vector(sceVu0IVECTOR v0, sceVu0FVECTOR v1);
// 引数
// 	v1		入力:ベクトル
// 	v0		出力:ベクトル
// 解説
// 	与えられたベクトルの要素を整数に変換する
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	
void sceVu0FTOI0Vector(sceVu0IVECTOR v0, sceVu0FVECTOR v1)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lqc2    vf4,0x0(%1)\n\
	vftoi0.xyzw	vf5,vf4\n\
	sqc2    vf5,0x0(%0)\n\
	": : "r" (v0) , "r" (v1) : "memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}


//==========================================================================
// sceVu0ITOF4Vector	ITOF4
// 形式
//	void sceVu0ITOF4Vector(sceVu0FVECTOR v0, sceVu0IVECTOR v1);
// 引数
// 	v1		入力:ベクトル
// 	v0		出力:ベクトル
// 解説
// 	与えられたベクトルの要素を整数に変換する
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	

void sceVu0ITOF4Vector(sceVu0FVECTOR v0, sceVu0IVECTOR v1)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lqc2    vf4,0x0(%1)\n\
	vitof4.xyzw	vf5,vf4\n\
	sqc2    vf5,0x0(%0)\n\
	": : "r" (v0) , "r" (v1) : "memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}

//==========================================================================
//sceVu0ITOF0Vector 	ITOF0
// 形式
//	void sceVu0ITOF0Vector(sceVu0FVECTOR v0, sceVu0IVECTOR v1);
// 引数
// 	v1		入力:ベクトル
// 	v0		出力:ベクトル
// 解説
// 	与えられたベクトルの要素を整数に変換する
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	
void sceVu0ITOF0Vector(sceVu0FVECTOR v0, sceVu0IVECTOR v1)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lqc2    vf4,0x0(%1)\n\
	vitof0.xyzw	vf5,vf4\n\
	sqc2    vf5,0x0(%0)\n\
	": : "r" (v0) , "r" (v1) : "memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}

//==========================================================================
//sceVu0UnitMatrix	単位行列 
// 形式
//	void sceVu0UnitMatrix(sceVu0FMATRIX m0);
// 引数
// 	m1		入力:マトリックス
// 	m0		出力:マトリックス
// 解説
// 	与えられた行列を単位行列に変換する
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	

void sceVu0UnitMatrix(sceVu0FMATRIX m0)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	vsub.xyzw	vf4,vf0,vf0 #vf4.xyzw=0;\n\
	vadd.w	vf4,vf4,vf0\n\
	vmr32.xyzw	vf5,vf4\n\
	vmr32.xyzw	vf6,vf5\n\
	vmr32.xyzw	vf7,vf6\n\
	sqc2    vf4,0x30(%0)\n\
	sqc2    vf5,0x20(%0)\n\
	sqc2    vf6,0x10(%0)\n\
	sqc2    vf7,0x0(%0)\n\
	": : "r" (m0) : "memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}


//==========================================================================
// 形式
//	void _sceVu0ecossin(float x)
// 引数
// 解説
// 	sin,cosを求める
//	有効範囲:-π/2 ～ π/2
//	RotMatrix?でのみ使用可
// 返り値
// 
//-------------------------------------------------------------------------- 	
	__asm__ ("\n\
	.data\n\
	.align 4\n\
	S5432:\n\
	.word	0x362e9c14, 0xb94fb21f, 0x3c08873e, 0xbe2aaaa4\n\
	.text\n\
	");


static void _sceVu0ecossin(float t)
{
    // Note. 05/21/2003
    //	この関数はstaticである事を踏まえて、呼び出し元が必ず
    //	割込み禁止処理(DI/EI)を行っている事を前提として
    //	同処理を行っていません
    //
    //	またsceVu0RotMatrix*()等では、この関数をinline assemblerで
    //	呼出しているので改変する場合はレジスタの破壊等に気を付けてください
    //	(ローカル変数を作成する場合/C文を挿入する場合等)

    __asm__ __volatile__("\n\
	la	$8,S5432	# S5-S2 の係数を VF05 へ転送	\n\
	lqc2	vf05,0x0($8)	#				\n\
	vmr32.w vf06,vf06	# VF06.x(v)をVF06.w にコピー	\n\
	vaddx.x vf04,vf00,vf06	# VF06.x(v)をVF04.x にコピー	\n\
	vmul.x vf06,vf06,vf06	# VF06.x を自乗して v^2 にする	\n\
	vmulx.yzw vf04,vf04,vf00# VF04.yzw=0			\n\
	vmulw.xyzw vf08,vf05,vf06# S2-S5 に VF06.w(v) をかける	\n\
	vsub.xyzw vf05,vf00,vf00 #0,0,0,0 |x,y,z,w		\n\
	vmulx.xyzw vf08,vf08,vf06# VF06.x(v^2) をかける		\n\
	vmulx.xyz vf08,vf08,vf06# VF06.x(v^2) をかける		\n\
	vaddw.x vf04,vf04,vf08	# s+=k2				\n\
	vmulx.xy vf08,vf08,vf06	# VF06.x(v^2) をかける		\n\
	vaddz.x vf04,vf04,vf08	# s+=z				\n\
	vmulx.x vf08,vf08,vf06	# VF06.x(v^2) をかける		\n\
	vaddy.x vf04,vf04,vf08	# s+=y				\n\
	vaddx.x vf04,vf04,vf08	# s+=x (sin 完了)		\n\
								\n\
	vaddx.xy vf04,vf05,vf04	# .xy=s 追加 			\n\
								\n\
	vmul.x vf07,vf04,vf04	# VF07.x=s*s			\n\
	vsubx.w vf07,vf00,vf07	# VF07.w = 1-s*s		\n\
								\n\
	vsqrt Q,vf07w		# Q= √|1-s*s| (cos 完了)	\n\
	vwaitq							\n\
								\n\
	vaddq.x vf07,vf00,Q     # vf07.x=c			\n\
								\n\
	bne	$7,$0,_ecossin_01				\n\
	vaddx.x vf04,vf05,vf07	# VF04.x=s			\n\
	b	_ecossin_02					\n\
_ecossin_01:							\n\
	vsubx.x vf04,vf05,vf07	# VF04.x=s			\n\
_ecossin_02:							\n\
								\n\
	": : :"$7","$8");
}

//==========================================================================
//sceVu0RotMatrixZ	Ｚ軸を中心とした行列の回転 
// 形式
//	void sceVu0RotMatrixZ(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float rz);
// 引数
// 	rz		入力:回転角(有効範囲:-π ～ π)
// 	m1		入力:マトリックス
// 	m0		出力:マトリックス
// 解説
// 	回転角rtよりＸ軸を中心とした回転マトリックスを求めて、
//	マトリックスm1に左側から乗算して、その結果をマトリックス
//	m0に与える
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	
void sceVu0RotMatrixZ(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float rz)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	mtc1	$0,$f0\n\
	c.olt.s %2,$f0\n\
	li.s    $f0,1.57079637050628662109e0	\n\
	bc1f    _RotMatrixZ_01\n\
	add.s   %2,$f0,%2			#rx=rx+π/2\n\
	li 	$7,1				#cos(rx)=sin(rx+π/2)\n\
	j	_RotMatrixZ_02\n\
_RotMatrixZ_01:\n\
	sub.s   %2,$f0,%2			#rx=π/2-rx\n\
	move	$7,$0\n\
_RotMatrixZ_02:\n\
	\n\
        mfc1    $8,%2\n\
        qmtc2    $8,vf6\n\
	move	$6,$31	# ra 保存 (本当はスタックを使うべき)\n\
	\n\
	jal	_sceVu0ecossin	# sin(roll), cos(roll)\n\
	move	$31,$6	# ra 回復\n\
			#vf05:0,0,0,0 |x,y,z,w\n\
	vmove.xyzw vf06,vf05\n\
	vmove.xyzw vf07,vf05\n\
	vmove.xyzw vf09,vf00\n\
	vsub.xyz vf09,vf09,vf09 	#0,0,0,1 |x,y,z,w\n\
	vmr32.xyzw vf08,vf09	#0,0,1,0 |x,y,z,w\n\
	\n\
	vsub.zw vf04,vf04,vf04 #vf04.zw=0 s,c,0,0 |x,y,z,w\n\
	vaddx.y vf06,vf05,vf04 #vf06 0,s,0,0 |x,y,z,w\n\
	vaddy.x vf06,vf05,vf04 #vf06 c,s,0,0 |x,y,z,w\n\
	vsubx.x vf07,vf05,vf04 #vf07 -s,0,0,0 |x,y,z,w\n\
	vaddy.y vf07,vf05,vf04 #vf07 -s,c,0,0 |x,y,z,w\n\
	\n\
	li    $7,4\n\
	_loopRotMatrixZ:\n\
	lqc2    vf4,0x0(%1)\n\
	vmulax.xyzw	ACC,   vf6,vf4\n\
	vmadday.xyzw	ACC,   vf7,vf4\n\
	vmaddaz.xyzw	ACC,   vf8,vf4\n\
	vmaddw.xyzw	vf5,  vf9,vf4\n\
	sqc2    vf5,0x0(%0)\n\
	addi    $7,-1\n\
	addi    %1,0x10\n\
	addi    %0,0x10\n\
	bne    $0,$7,_loopRotMatrixZ\n\
	": : "r" (m0) , "r" (m1), "f" (rz):"$6","$7","$8","$f0", "memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}


//==========================================================================
//sceVu0RotMatrixX	Ｘ軸を中心とした行列の回転 
// 形式
//	void sceVu0RotMatrixX(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float ry);
// 引数
// 	rx		入力:回転角(有効範囲:-π ～ π)
// 	m1		入力:マトリックス
// 	m0		出力:マトリックス
// 解説
// 	回転角rtよりＸ軸を中心とした回転マトリックスを求めて、
//	マトリックスm1に左側から乗算して、その結果をマトリックス
//	m0に与える
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	
void sceVu0RotMatrixX(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float rx)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	mtc1	$0,$f0\n\
	c.olt.s %2,$f0\n\
	li.s    $f0,1.57079637050628662109e0	\n\
	bc1f    _RotMatrixX_01\n\
	add.s   %2,$f0,%2			#rx=rx+π/2\n\
	li 	$7,1				#cos(rx)=sin(rx+π/2)\n\
	j	_RotMatrixX_02\n\
_RotMatrixX_01:\n\
	sub.s   %2,$f0,%2			#rx=π/2-rx\n\
	move	$7,$0\n\
_RotMatrixX_02:\n\
	\n\
	mfc1    $8,%2\n\
	qmtc2    $8,$vf6\n\
	move	$6,$31	# ra 保存 (本当はスタックを使うべき)\n\
	jal	_sceVu0ecossin	# sin(roll), cos(roll)\n\
	move	$31,$6	# ra 回復\n\
			#vf05:0,0,0,0 |x,y,z,w\n\
	vmove.xyzw vf06,vf05\n\
	vmove.xyzw vf07,vf05\n\
	vmove.xyzw vf08,vf05\n\
	vmove.xyzw vf09,vf05\n\
	vaddw.x vf06,vf05,vf00 #vf06 1,0,0,0 |x,y,z,w\n\
	vaddw.w vf09,vf05,vf00 #vf09 0,0,0,1 |x,y,z,w\n\
	\n\
	vsub.zw vf04,vf04,vf04 #vf04.zw=0 s,c,0,0 |x,y,z,w\n\
	vaddx.z vf07,vf05,vf04 #vf07.zw=0 0,0,s,0 |x,y,z,w\n\
	vaddy.y vf07,vf05,vf04 #vf07.zw=0 0,c,s,0 |x,y,z,w\n\
	vsubx.y vf08,vf05,vf04 #vf08.zw=0 0,-s,0,0 |x,y,z,w\n\
	vaddy.z vf08,vf05,vf04 #vf08.zw=0 0,-s,c,0 |x,y,z,w\n\
	\n\
	li    $7,4\n\
	_loopRotMatrixX:\n\
	lqc2    vf4,0x0(%1)\n\
	vmulax.xyzw	ACC,   vf6,vf4\n\
	vmadday.xyzw	ACC,   vf7,vf4\n\
	vmaddaz.xyzw	ACC,   vf8,vf4\n\
	vmaddw.xyzw	vf5,  vf9,vf4\n\
	sqc2    vf5,0x0(%0)\n\
	addi    $7,-1\n\
	addi    %1,0x10\n\
	addi    %0,0x10\n\
	bne    $0,$7,_loopRotMatrixX\n\
	": : "r" (m0) , "r" (m1), "f" (rx):"$6","$7","$8","$f0","memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}
//==========================================================================
//sceVu0RotMatrixY	Ｙ軸を中心とした行列の回転 
// 形式
//	void sceVu0RotMatrixY(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float ry);
// 引数
// 	ry		入力:回転角(有効範囲:-π ～ π)
// 	m1		入力:マトリックス
// 	m0		出力:マトリックス
// 解説
// 	回転角rtよりＹ軸を中心とした回転マトリックスを求めて、
//	マトリックスm1に左側から乗算して、その結果をマトリックス
//	m0に与える
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	
void sceVu0RotMatrixY(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float ry)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	mtc1	$0,$f0\n\
	c.olt.s %2,$f0\n\
	li.s    $f0,1.57079637050628662109e0	\n\
	bc1f    _RotMatrixY_01\n\
	add.s   %2,$f0,%2			#rx=rx+π/2\n\
	li 	$7,1				#cos(rx)=sin(rx+π/2)\n\
	j	_RotMatrixY_02\n\
_RotMatrixY_01:\n\
	sub.s   %2,$f0,%2			#rx=π/2-rx\n\
	move	$7,$0\n\
_RotMatrixY_02:\n\
	\n\
	mfc1    $8,%2\n\
	qmtc2    $8,vf6\n\
	move	$6,$31	# ra 保存 (本当はスタックを使うべき)\n\
	jal	_sceVu0ecossin	# sin(roll), cos(roll)\n\
	move	$31,$6	# ra 回復\n\
			#vf05:0,0,0,0 |x,y,z,w\n\
	vmove.xyzw vf06,vf05\n\
	vmove.xyzw vf07,vf05\n\
	vmove.xyzw vf08,vf05\n\
	vmove.xyzw vf09,vf05\n\
	vaddw.y vf07,vf05,vf00 #vf07 0,1,0,0 |x,y,z,w\n\
	vaddw.w vf09,vf05,vf00 #vf09 0,0,0,1 |x,y,z,w\n\
	\n\
	vsub.zw vf04,vf04,vf04 #vf04.zw=0 s,c,0,0 |x,y,z,w\n\
	vsubx.z vf06,vf05,vf04 #vf06 0,0,-s,0 |x,y,z,w\n\
	vaddy.x vf06,vf05,vf04 #vf06 c,0,-s,0 |x,y,z,w\n\
	vaddx.x vf08,vf05,vf04 #vf08 s,0,0,0 |x,y,z,w\n\
	vaddy.z vf08,vf05,vf04 #vf08 s,0,c,0 |x,y,z,w\n\
	\n\
	li    $7,4\n\
	_loopRotMatrixY:\n\
	lqc2    vf4,0x0(%1)\n\
	vmulax.xyzw	ACC,   vf6,vf4\n\
	vmadday.xyzw	ACC,   vf7,vf4\n\
	vmaddaz.xyzw	ACC,   vf8,vf4\n\
	vmaddw.xyzw	vf5,  vf9,vf4\n\
	sqc2    vf5,0x0(%0)\n\
	addi    $7,-1\n\
	addi    %1,0x10\n\
	addi    %0,0x10\n\
	bne    $0,$7,_loopRotMatrixY\n\
	": : "r" (m0) , "r" (m1), "f" (ry):"$6","$7","$8","$f0","memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}

//==========================================================================
//sceVu0RotMatrixY	行列の回転 
// 形式
//	void sceVu0RotMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FVECTOR rot);
// 引数
// 	m0		出力:マトリックス
// 	m1		入力:マトリックス
// 	rot		入力:x,y,z軸の回転角(有効範囲:-π ～ π)
// 解説
//	Z軸を中心とする回転マトリックスをrot[2]から、Y軸を中心とする回転マ
//	トリックスをrot[1]から、X軸を中心とする回転マトリックスをrot[0]から
//	それぞれ求め、順にマトリックスm1に左側から乗算して、その結果をマト
//	リックスm0に返します。
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	
void sceVu0RotMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FVECTOR rot)
{
    sceVu0RotMatrixZ(m0, m1, rot[2]);
    sceVu0RotMatrixY(m0, m0, rot[1]);
    sceVu0RotMatrixX(m0, m0, rot[0]);
}

//==========================================================================
//sceVu0ClampVector	ベクトルのクランプ
// 形式
//	void sceVu0ClampVector(sceVu0FVECTOR v0, sceVu0FFVECTOR v1, float min, float max)
// 引数
// 	v0		出力:ベクトル
// 	v1		入力:ベクトル
// 	min		入力:最小値
// 	max		入力:最大値
// 解説
//	ベクトルv1の各要素を、最小値min、最大値maxでクランプし、結果をベク
//	トルv0に返します。
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	
void sceVu0ClampVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float min, float max)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
        mfc1    $8,%2\n\
        mfc1    $9,%3\n\
	lqc2    vf6,0x0(%1)\n\
        qmtc2    $8,vf4\n\
        qmtc2    $9,vf5\n\
	vmaxx.xyzw $vf06,$vf06,$vf04\n\
	vminix.xyzw $vf06,$vf06,$vf05\n\
	sqc2    vf6,0x0(%0)\n\
	": : "r" (v0) , "r" (v1), "f" (min), "f" (max):"$8","$9","memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}

//==========================================================================
//sceVu0CameraMatrix	ワールドビュー行列の生成
// 形式
//	void sceVu0CameraMatrix(sceVu0FMATRIX m, sceVu0FVECTOR p, sceVu0FVECTOR zd, sceVu0FVECTOR yd);
// 引数
// 	m		出力:マトリックス(ビューワールド座標)
// 	p		入力:マトリックス(視点)
// 	zd		入力:マトリックス(視線)
// 	yd		入力:マトリックス(垂直方向)
// 解説
//	視点pを(0,0,0)に、視線zdを(0,0,1)に、垂直方向ydを(0,1,0)にと変換す
//	るような行列を求め、mに返します。
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	
//  yake world screen matrix
//	p:  camera position (in the world)
//	zd: eye direction
//	yd: vertical direction
//
//	zd = zd/|zd|
//	xd = zd x yd; xd = xd/|xd|	(wing direction)
//	yd = yd x xd; yd = yd/|yd|	(real veritical direction)
//	
//	        | xd0 yd0 zd0 p0 |
//	M = inv | xd1 yd1 zd1 p1 |
//	        | xd2 yd2 zd2 p2 |
//	        |   0   0   0  1 |
//-------------------------------------------------------------------------- 	

void sceVu0CameraMatrix(sceVu0FMATRIX m, sceVu0FVECTOR p, sceVu0FVECTOR zd, sceVu0FVECTOR yd)
{
	sceVu0FMATRIX	m0;
	sceVu0FVECTOR	xd;

	sceVu0UnitMatrix(m0);			
	sceVu0OuterProduct(xd, yd, zd);
	sceVu0Normalize(m0[0], xd);
	sceVu0Normalize(m0[2], zd);
	sceVu0OuterProduct(m0[1], m0[2], m0[0]);
	sceVu0TransMatrix(m0, m0, p);
	sceVu0InversMatrix(m, m0);
}

//==========================================================================
//sceVu0NormalLightMatrix         ノーマルライト行列の生成
// 形式
//	void sceVu0NormalLightMatrix(sceVu0FMATRIX m, sceVu0FVECTOR l0, sceVu0FVECTOR l1, sceVu0FVECTOR l2);
// 引数
// 	m		出力:マトリックス
// 	l0		入力:マトリックス(光源0の方向)
// 	l1		入力:マトリックス(光源1の方向)
// 	l2		入力:マトリックス(光源2の方向)
// 解説
//	光源l0、光源l1、光源l2からノーマルライト行列を求め、mに返します。
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	
// make normal light matrix
//	l0-l2:	light direction
//	    | l0x l0y l0z 0 |   | 10r 10g 10b 0 |
//	M = | l1x l1y l1z 0 | = | 11r 11g 11b 0 |
//	    | l2x l2y l2z 0 |   | 12r 12g 12b 0 |
//	    |   0   0   0 1 |   |   0   0   0 1 |
//-------------------------------------------------------------------------- 	

void sceVu0NormalLightMatrix(sceVu0FMATRIX m, sceVu0FVECTOR l0, sceVu0FVECTOR l1, sceVu0FVECTOR l2)
{
        sceVu0FVECTOR  t;
        sceVu0ScaleVector(t, l0, -1); sceVu0Normalize(m[0], t);
        sceVu0ScaleVector(t, l1, -1); sceVu0Normalize(m[1], t);
        sceVu0ScaleVector(t, l2, -1); sceVu0Normalize(m[2], t);

	m[3][0] = m[3][1] = m[3][2] = 0.0f;
	m[3][3] = 1.0f;

	sceVu0TransposeMatrix(m, m);
}

//==========================================================================
//sceVu0LightColorMatrix		ライトカラー行列の生成
// 形式
//	void sceVu0LightColorMatrix(sceVu0FMATRIX m,  sceVu0FVECTOR c0, sceVu0FVECTOR c1, sceVu0FVECTOR c2, sceVu0FVECTOR a)
// 引数
// 	m		出力:マトリックス
// 	c0		入力:ベクトル(光源色0)
// 	c1		入力:ベクトル(光源色1)
// 	c2		入力:ベクトル(光源色2)
// 	a		入力:ベクトル(アンビエント)
// 解説
//	光源色c0,c1,c2と環境色aからライトカラー行列を求め、mに返します。
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	
// make normal light matrix
//	c0-c2:	light color
//	a:	ambient color
//
//	    | c0x c1x c2x ax |   | c0r c1r c2r ar |
//	M = | c0y c1y c2y ay | = | c0g c1g c2g ag |
//	    | c0z c1z c2z az |   | c0b c1b c2b ab |
//	    |   0   0   0  0 |   |   0   0   0  0 |
//-------------------------------------------------------------------------- 	
void sceVu0LightColorMatrix(sceVu0FMATRIX m, sceVu0FVECTOR c0, sceVu0FVECTOR c1, sceVu0FVECTOR c2, sceVu0FVECTOR a)
{
	sceVu0CopyVector(m[0], c0);
	sceVu0CopyVector(m[1], c1);
	sceVu0CopyVector(m[2], c2);
	sceVu0CopyVector(m[3],  a);
}

//==========================================================================
//sceVu0ViewScreenMatrix          ビュースクリーン行列の生成
// 形式
//	void sceVu0ViewScreenMatrix(sceVu0FMATRIX m, float scrz, float ax, float ay, float cx, float cy, float zmin,  float zmax,  float nearz, float farz)
// 引数
// 	m		出力:マトリックス
//	scrz            入力:(スクリーンまでの距離)
//	ax              入力:(Ｘ方向アスペクト比)
//	ay              入力:(Ｙ方向アスペクト比)
//	cx              入力:(スクリーンの中心Ｘ座標)
//	cy              入力:(スクリーンの中心Ｙ座標)
//	zmin            入力:(Ｚバッファ最小値)
//	zmax            入力:(Ｚバッファ最大値)
//	nearz           入力:(ニアクリップ面のＺ)
//	farz            入力:(ファークリップ面のＺ)
// 解説
//	指定された各パラメータに従ってビュースクリーン行列を求め、mに返しま
//	す。
// 返り値
// 	なし
//-------------------------------------------------------------------------- 	
// make view-screen matrix
//	scrz:		distance to screen
//	ax, ay:		aspect ratio
//	cx, cy:		center
//	zmin, zmax:	Z-buffer range
//	nearz, farz:	near, far edge
//-------------------------------------------------------------------------- 	
void sceVu0ViewScreenMatrix(sceVu0FMATRIX m, float scrz, float ax, float ay, 
	       float cx, float cy, float zmin, float zmax, float nearz, float farz)
{
	float	az, cz;
	sceVu0FMATRIX	mt;

	cz = (-zmax * nearz + zmin * farz) / (-nearz + farz);
	az  = farz * nearz * (-zmin + zmax) / (-nearz + farz);

	//     | scrz    0  0 0 |
	// m = |    0 scrz  0 0 | 
	//     |    0    0  0 1 |
	//     |    0    0  1 0 |
	sceVu0UnitMatrix(m);
	m[0][0] = scrz;
	m[1][1] = scrz;
	m[2][2] = 0.0f;
	m[3][3] = 0.0f;
	m[3][2] = 1.0f;
	m[2][3] = 1.0f;

	//     | ax  0  0 cx |
	// m = |  0 ay  0 cy | 
	//     |  0  0 az cz |
	//     |  0  0  0  1 |
	sceVu0UnitMatrix(mt);
	mt[0][0] = ax;
	mt[1][1] = ay;
	mt[2][2] = az;
	mt[3][0] = cx;
	mt[3][1] = cy;
	mt[3][2] = cz;

	sceVu0MulMatrix(m, mt, m);
	return;
}

//==========================================================================
// 
// sceVu0DropShadowMatrix          ドロップシャドウ射影行列の生成
// 形式
//        void sceVu0DropShadowMatrix(sceVu0FMATRIX m, sceVu0FVECTOR lp, float a, float b, float c, int mode);
// 引数
// 	なし
//	m               出力:マトリックス
//	lp              入力:ベクトル(光源の位置)
//	a               入力:影の投影面
//	b               入力:影の投影面
//	c               入力:影の投影面
//	mode            入力:光源の種類
//			0: 平行光源
//			1: 点光源
// 解説
//	ax+by+cz=1で表現される平面にlpとmodeで指定される光源からの影を投影
//	する行列を求め、mに返します。
// 返り値
// 	なし
//-------------------------------------------------------------------------- 	
// drop shadow 
//	p:		light source postion
//	a, b, c:	shadow surface (ax+by+cz = 1)
//
//	    | 1 0 0 px |   | d 0 0 0 |   | 1 0 0 -px |
//	M = | 0 1 0 py | * | 0 d 0 0 | * | 0 1 0 -py |
//	    | 0 0 1 pz |   | 0 0 d 0 |   | 0 0 1 -pz |
//	    | 0 0 0  1 |   | a b c 0 |   | 0 0 0   1 |
//
//	    | ax+d bx   cx   -x |			
//	  = | ay   by+d cy   -y |			
//	    | az   bz   cz+d -z |			
//	    | a    b    c    d-1| 
//		(d = 1-(ax+by+cz))			
//
// drop shadow (paralle light
//	d:	light direction
//	a,b,c:	shadow surface
//
//	(x,y,z) =  (pt, qt, rt), then t -> infinity
//
//	    | ap-n bp   cp   -p |
//	M = | aq   bp-n cp   -q |
//	    | ar   bp   cp-n -r |
//	    |  0   0   0    -n  | (n = ap+bq+cr)
//-------------------------------------------------------------------------- 	

void sceVu0DropShadowMatrix(sceVu0FMATRIX m, sceVu0FVECTOR lp, float a, float b, float c, int mode)
{
    if (mode) {	// spot light
	float x = lp[0], y = lp[1], z = lp[2];
	float d = (float)1-(a*x+b*y+c*z);

	m[0][0] = a*x+d, m[1][0] = b*x,   m[2][0] = c*x,   m[3][0] = -x;
	m[0][1] = a*y,   m[1][1] = b*y+d, m[2][1] = c*y,   m[3][1] = -y;
	m[0][2] = a*z,   m[1][2] = b*z,   m[2][2] = c*z+d, m[3][2] = -z;
	m[0][3] = a,     m[1][3] = b,     m[2][3] = c,     m[3][3] = d-(float)1;
    }
    else {		// parallel light
	float p  = lp[0], q = lp[1], r = lp[2];
	float n  = a*p+b*q+c*r;
	float nr = -(float)1.0/n;

	m[0][0] = nr*(a*p-n), m[1][0] = nr*(b*p),   m[2][0] = nr*(c*p),   m[3][0] = nr*(-p);
	m[0][1] = nr*(a*q),   m[1][1] = nr*(b*q-n), m[2][1] = nr*(c*q),   m[3][1] = nr*(-q);
	m[0][2] = nr*(a*r),   m[1][2] = nr*(b*r),   m[2][2] = nr*(c*r-n), m[3][2] = nr*(-r);
	m[0][3] = (float)0,          m[1][3] = (float)0,          m[2][3] =(float) 0,          m[3][3] = nr*(-n);
    }
}



//==========================================================================
//sceVu0RotTransPersN	透視変換
// 形式
//	void sceVu0RotTransPersN(sceVu0IVECTOR *v0, sceVu0FMATRIX m0, sceVu0FVECTOR v1, int n, int mode);
// 引数
// 	v0		出力:ベクトル(スクリーン座標)
// 	m0		入力:マトリックス
// 	v1		入力:ベクトル
// 	n		入力:頂点数
// 	mode		入力:v0[2],v0[3]の値の切り替え
// 			     0:32bit符号なし固定小数点(28.4)
// 			     1:32bit符号なし固定小数点(32.0)
// 解説
// 	与えられたn個の頂点をスクリーン座標に透視変換する。
//	出力v0の値は、v0[0],v0[1]は、32bit符号付き固定小数点(28.4)、
//	v0[2],v0[3]はmode=1のときは、32bit符号なし固定小数点(28.4)、
//	mode=0のときは、32bit符号なし固定小数点(32.0)を返す。
//	GIFのPACKEDモードでXYZF2,XYZF3を用いる場合には、mode=0にす
//	ると、PACK時に整数部が切り出されるので有用である。
//	
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	

void sceVu0RotTransPersN(sceVu0IVECTOR *v0, sceVu0FMATRIX m0, sceVu0FVECTOR *v1, int n, int mode)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lqc2	vf4,0x0(%1)\n\
	lqc2	vf5,0x10(%1)\n\
	lqc2	vf6,0x20(%1)\n\
	lqc2	vf7,0x30(%1)\n\
_rotTPN_loop:\n\
	lqc2	vf8,0x0(%2)\n\
	vmulax.xyzw     ACC, vf4,vf8\n\
	vmadday.xyzw    ACC, vf5,vf8\n\
	vmaddaz.xyzw    ACC, vf6,vf8\n\
	vmaddw.xyzw      vf9,vf7,vf8\n\
	vdiv    Q,vf0w,vf9w\n\
	vwaitq\n\
	vmulq.xyz	vf9,vf9,Q\n\
	vftoi4.xyzw	vf10,vf9\n\
	beqz	%4,_rotTPN\n\
	vftoi0.zw	vf10,vf9	\n\
_rotTPN:\n\
	sqc2	vf10,0x0(%0)\n\
	\n\
	addi	%3,-1\n\
	addi	%2,0x10\n\
	addi	%0,0x10\n\
	bne	$0,%3,_rotTPN_loop\n\
	": : "r" (v0) , "r" (m0) ,"r" (v1), "r" (n) ,"r" (mode):"memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}


//==========================================================================
//sceVu0RotTransPers	透視変換
// 形式
//	void sceVu0RotTransPers(sceVu0IVECTOR v0, sceVu0FMATRIX m0, sceVu0FVECTOR v1);
// 引数
// 	v0		出力:ベクトル(スクリーン座標)
// 	m0		入力:マトリックス
// 	v1		入力:ベクトル
// 	mode		入力:v0[2],v0[3]の値の切り替え
// 			     0:32bit符号なし固定小数点(28.4)
// 			     1:32bit符号なし固定小数点(32.0)
// 解説
// 	与えられた頂点をスクリーン座標に透視変換する。
//	出力v0の値は、v0[0],v0[1]は、32bit符号付き固定小数点(28.4)、
//	v0[2],v0[3]はmode=1のときは、32bit符号なし固定小数点(28.4)、
//	mode=0のときは、32bit符号なし固定小数点(32.0)を返す。
//	GIFのPACKEDモードでXYZF2,XYZF3を用いる場合には、mode=0にす
//	ると、PACK時に整数部が切り出されるので有用である。
//	
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	

void sceVu0RotTransPers(sceVu0IVECTOR v0, sceVu0FMATRIX m0, sceVu0FVECTOR v1, int mode)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lqc2	vf4,0x0(%1)\n\
	lqc2	vf5,0x10(%1)\n\
	lqc2	vf6,0x20(%1)\n\
	lqc2	vf7,0x30(%1)\n\
	lqc2	vf8,0x0(%2)\n\
	vmulax.xyzw     ACC, vf4,vf8\n\
	vmadday.xyzw    ACC, vf5,vf8\n\
	vmaddaz.xyzw    ACC, vf6,vf8\n\
	vmaddw.xyzw      vf9,vf7,vf8\n\
	vdiv    Q,vf0w,vf9w\n\
	vwaitq\n\
	vmulq.xyz	vf9,vf9,Q\n\
	vftoi4.xyzw	vf10,vf9\n\
	beqz	%3,_rotTP\n\
	vftoi0.zw	vf10,vf9	\n\
_rotTP:\n\
	sqc2	vf10,0x0(%0)\n\
	\n\
	": : "r" (v0) , "r" (m0) ,"r" (v1), "r" (mode):"memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}

//==========================================================================
// 
// sceVu0CopyVectorXYZ	ベクトルの複写
// 形式
//	void sceVu0CopyVectorXYZ(sceVu0FVECTOR v0,sceVu0FVECTOR v1);
// 引数
//	v0		出力:ベクトル
//	v1		入力:ベクトル
// 解説
//	ベクトルv1のx,y,z要素をベクトルv0にコピーします。
//	ベクトルv0のw要素は、そのままベクトルv0に返す。 
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	
void  sceVu0CopyVectorXYZ(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
    v0[0]=v1[0];
    v0[1]=v1[1];
    v0[2]=v1[2];
}


//==========================================================================
// 
// sceVu0InterVectorXYZ	内挿ベクトルの生成
// 形式
//	void sceVu0InterVectorXYZ(sceVu0FVECTOR v0,sceVu0FVECTOR v1,sceVu0FVECTOR v2, float t)
// 引数
//	v0		出力:ベクトル
//	v1.v2		入力:ベクトル
//	t		入力:内挿パラメータ
// 解説
//	ベクトルv1,v2およびパラメータtから内挿ベクトルを求め、v0に返します。
//	式で表現すると次のとおりです。
//	 v0 = v1*t + v2*(1-t)
//	ベクトルv1のw要素は、そのままベクトルv0に返します
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	
void sceVu0InterVectorXYZ(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2, float r)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lqc2    vf4,0x0(%1)\n\
	lqc2    vf5,0x0(%2)\n\
	mfc1    $8,%3\n\
	qmtc2    $8,vf6\n\
	\n\
	vmove.w	vf9,vf4\n\
	vaddw.x    vf7,vf0,vf0	#vf7.x=1;\n\
	vsub.x    vf8,vf7,vf6	#vf8.x=1-t;\n\
	vmulax.xyz	ACC,   vf4,vf6\n\
	vmaddx.xyz	vf9,vf5,vf8\n\
	\n\
	sqc2    vf9,0x0(%0)\n\
	": : "r" (v0) , "r" (v1), "r" (v2), "f" (r) : "$8","memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}

//==========================================================================
// 
// sceVu0ScaleVectorXYZ	スカラー値とベクトルの乗算 (MULx/xyz)
// 形式
//	void sceVu0ScaleVector(sceVu0FVECTOR v0,sceVu0FVECTOR v1,float t)
// 引数
// 	なし
//	v0		出力:ベクトル
//	v1		入力:ベクトル
//	t		入力:スカラー
// 解説
//	ベクトルv1のx,y,z要素にスカラーtを乗算し、結果をベクトルv0に返しま
//	す。ベクトルv1のw要素は、そのままベクトルv0に返します。
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	
void  sceVu0ScaleVectorXYZ(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float s)
{
#ifdef SCE_VU0_SAFE_CONTEXT
    int ret = DI();
#endif
    __asm__ __volatile__("\n\
	lqc2    vf4,0x0(%1)\n\
	mfc1    $8,%2\n\
	qmtc2    $8,vf5\n\
	vmulx.xyz	vf4,vf4,vf5\n\
	sqc2    vf4,0x0(%0)\n\
	": : "r" (v0) , "r" (v1), "f" (s):"$8","memory");
#ifdef SCE_VU0_SAFE_CONTEXT
    if (ret) EI();
#endif
}

//==========================================================================
// 
// sceVu0ClipScreen	頂点のＧＳ描画範囲のクリッピング
// 形式
// 	int sceVu0ClipScreen(sceVu0FVECTOR v0)
// 引数
// 	v0		入力:ベクトル
// 解説
// 	頂点ベクトルv0がＧＳ描画範囲に入っているかを調べる。
// 返り値
// 	0:頂点v0がＧＳ描画範囲に入っている。
// 
//-------------------------------------------------------------------------- 	

int sceVu0ClipScreen(sceVu0FVECTOR v0)
{
    register int ret;

#ifdef SCE_VU0_SAFE_CONTEXT
    int r = DI();
#endif
    __asm__ __volatile__("\n\
	vsub.xyzw        vf04,vf00,vf00  #(0,0,0,0);\n\
	li     %0,0x4580000045800000\n\
	lqc2    vf07,0x0(%1)\n\
	\n\
	qmtc2  %0,vf06\n\
	ctc2    $0,$vi16                #ステータスフラグのクリア\n\
	\n\
	vsub.xyw        vf05,vf07,vf04  #(z,ZBz,PPy,PPx)-(0,0,0,0);\n\
	vsub.xy        vf05,vf06,vf07  #(Zmax,0,Ymax,Xmax) -(z,ZBz,PPy,PPx)\n\
	\n\
	vnop			        \n\
	vnop				\n\
	vnop				\n\
	vnop				\n\
	vnop				\n\
        cfc2    %0,$vi16                #ステータスフラグをREAD\n\
	andi	%0,%0,0xc0		\n\
	\n\
	":"=&r"(ret): "r" (v0));
#ifdef SCE_VU0_SAFE_CONTEXT
    if (r) EI();
#endif
    return ret;
}

//==========================================================================
// 
// sceVu0ClipScreen3	３頂点のＧＳ描画範囲のクリッピング
// 形式
// 	int sceVu0ClipScreen3(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
// 引数
// 	v0,v1,v2		入力:ベクトル
// 解説
// 	頂点ベクトルv0,v1,v2がすべてＧＳ描画範囲に入っているかを調べる。
// 返り値
// 	0:すべての頂点がＧＳ描画範囲に入っている。
// 
//-------------------------------------------------------------------------- 	
 
int sceVu0ClipScreen3(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
{
    register int ret;

#ifdef SCE_VU0_SAFE_CONTEXT
    int r = DI();
#endif
    __asm__ __volatile__("\n\
	vsub.xyzw        vf04,vf00,vf00  #(0,0,0,0);\n\
	li     %0,0x4580000045800000\n\
	lqc2    vf06,0x0(%1)\n\
	lqc2    vf08,0x0(%2)\n\
	lqc2    vf09,0x0(%3)\n\
	\n\
	qmtc2  %0,vf07\n\
	ctc2    $0,$vi16                #ステータスフラグのクリア\n\
	\n\
	vsub.xyw        vf05,vf06,vf04  #(z,ZBz,PPy,PPx)-(0,0,0,0);\n\
	vsub.xy        vf05,vf07,vf06  #(Zmax,0,Ymax,Xmax) -(z,ZBz,PPy,PPx)\n\
	vsub.xyw        vf05,vf08,vf04  #(z,ZBz,PPy,PPx)-(0,0,0,0);\n\
	vsub.xy        vf05,vf07,vf08  #(Zmax,0,Ymax,Xmax) -(z,ZBz,PPy,PPx)\n\
	vsub.xyw        vf05,vf09,vf04  #(z,ZBz,PPy,PPx)-(0,0,0,0);\n\
	vsub.xy        vf05,vf07,vf09  #(Zmax,0,Ymax,Xmax) -(z,ZBz,PPy,PPx)\n\
	\n\
	vnop				\n\
	vnop				\n\
	vnop				\n\
	vnop				\n\
	vnop				\n\
	cfc2    %0,$vi16                #ステータスフラグをREAD\n\
	andi	%0,%0,0xc0		\n\
	\n\
	":"=&r"(ret): "r" (v0), "r" (v1), "r" (v2) );
#ifdef SCE_VU0_SAFE_CONTEXT
    if (r) EI();
#endif
    return ret;
}

#define UNDER_X		 1
#define UNDER_Y		 2
#define UNDER_W		 4
#define OVER_X		 8
#define OVER_Y		16
#define OVER_W		32
//==========================================================================
//
// sceVu0ClipAll           表示範囲によるクリッピング検査
// 形式
// 	int sceVu0ClipAll(sceVu0FVECTOR minv, sceVu0FVECTOR maxv, sceVu0FVECTOR ms,sceVu0FVECTOR *vm,int n)
// 引数
//	minv			入力:表示範囲の最小値
//	maxv			入力:表示範囲の最大値
//	ms			入力:マトリックス（モデル－スクリーン）
//	vm			入力:頂点ベクトルのポインタ
//	n			入力:頂点数
// 解説
//	vmとnで指定されるn個の頂点がすべて表示範囲に入っていないかどうかを
//	調べます。
// 返り値
//	すべての頂点が表示範囲に入っていないときは1を返します。
//
//-------------------------------------------------------------------------- 	
int sceVu0ClipAll(sceVu0FVECTOR minv, sceVu0FVECTOR maxv, sceVu0FMATRIX ms, sceVu0FVECTOR *vm, int n)
{
    register int ret;

#ifdef SCE_VU0_SAFE_CONTEXT
    int r = DI();
#endif
    __asm__ __volatile__("\n\
	\n\
	lqc2    vf8,0x0(%4)\n\
	lqc2    vf4,0x0(%3)\n\
	lqc2    vf5,0x10(%3)\n\
	lqc2    vf6,0x20(%3)\n\
	lqc2    vf7,0x30(%3)\n\
	\n\
	lqc2    vf9,0x0(%1)	#minv\n\
	lqc2    vf10,0x0(%2)	#maxv\n\
	lqc2    vf11,0x0(%1)	#minv\n\
	lqc2    vf12,0x0(%2)	#maxv\n\
	\n\
loop_clip_all:\n\
	vmulax.xyzw	ACC,vf4,vf8\n\
	vmadday.xyzw	ACC,vf5,vf8\n\
	vmaddaz.xyzw	ACC,vf6,vf8\n\
	vmaddw.xyzw	vf8,vf7,vf8\n\
	\n\
	vmulw.xyz	vf11,vf9,vf8\n\
	vmulw.xyz	vf12,vf10,vf8\n\
	\n\
	vnop				\n\
	vnop				\n\
	ctc2    $0,$vi16                #ステータスフラグのクリア\n\
	vsub.xyw       vf11,vf8,vf11  #(z,ZBz,PPy,PPx)-(Zmin,0,Ymin,Xmin);\n\
	vsub.xyw       vf12,vf12,vf8  #(Zmax,0,Ymax,Xmax) -(z,ZBz,PPy,PPx)\n\
	vmove.w	vf11,vf9\n\
	vmove.w	vf12,vf10\n\
	vnop			\n\
	addi	%4,0x10	\n\
	lqc2    vf8,0x0(%4)\n\
	addi	%5,-1\n\
	cfc2    %0,$vi16                #ステータスフラグをREAD\n\
	andi	%0,%0,0xc0\n\
	beqz	%0,end_clip_all\n\
	\n\
	bne	$0,%5,loop_clip_all\n\
	\n\
	addi	%0,$0,1\n\
end_clip_all:\n\
	\n\
	":"=&r"(ret): "r" (minv),"r" (maxv),"r" (ms),"r" (vm),"r" (n)  );
#ifdef SCE_VU0_SAFE_CONTEXT
    if (r) EI();
#endif
    return ret;
}

//==========================================================================
// 
// sceVu0Reset	VU0,VIF0のリセット
// 形式
// 	void sceVu0Reset(void);
// 引数
// 	なし
// 解説
// 	vu0およびVIF0を初期化します。
// 返り値
// 	なし
// 
//-------------------------------------------------------------------------- 	
void sceVpu0Reset(void)
{
    static  u_int init_vif_regs[8] __attribute__((aligned (16))) = {
	0x01000404, /*  STCYCL          */
	0x20000000, /*  STMASK          */
	0x00000000, /*  DATA            */
	0x05000000, /*  STMOD           */
	0x04000000, /*  ITOP            */
	0x00000000, /*  NOP             */
	0x00000000, /*  NOP             */
	0x00000000, /*  NOP             */
    };

    {
#ifdef SCE_VU0_SAFE_CONTEXT
	int ret = DI();
#endif
	*VIF0_MARK=0;
	*VIF0_ERR=0;
	*VIF0_FBRST = 1; /* VIF1 reset */
	__asm__ __volatile__("\n\
	cfc2 $8, $vi28\n\
	ori  $8, $8, 0x0002\n\
	ctc2 $8, $vi28\n\
	sync.p\n\
	":::"$8"); /* VU1 reset */
	*VIF0_FIFO = *(u_long128 *)&(init_vif_regs[0]);
	*VIF0_FIFO = *(u_long128 *)&(init_vif_regs[4]);
#ifdef SCE_VU0_SAFE_CONTEXT
	if (ret) EI();
#endif
    }
}

