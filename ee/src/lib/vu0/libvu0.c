/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.7
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
#include<eeregs.h>
#include<stdio.h>

static void _sceVu0ecossin(float t);

/* mathematical functions */

//==========================================================================
// sceVu0ApplyMatrix		�٥��ȥ�˥ޥȥ�å�����軻���롣
// ����
// 	void sceVu0ApplyMatrix(sceVu0FVECTOR v0, sceVu0FMATRIX m0, sceVu0FVECTOR v1)
// ����
// 	v1		����:�٥��ȥ�
// 	m0		����:�ޥȥ�å���
// 	v0		����:�٥��ȥ�
// 
// ����
// 	�ޥȥ�å���m�˥٥��ȥ�v0�򱦤���軻���ơ�v1��Ϳ����
// 
// 		v0=m0*v1
// �֤���
// 	�ʤ�
//-------------------------------------------------------------------------- 	

void sceVu0ApplyMatrix(sceVu0FVECTOR v0, sceVu0FMATRIX m0,sceVu0FVECTOR v1)
{
	asm __volatile__("
	lqc2    vf4,0x0(%1)
	lqc2    vf5,0x10(%1)
	lqc2    vf6,0x20(%1)
	lqc2    vf7,0x30(%1)
	lqc2    vf8,0x0(%2)
	vmulax.xyzw	ACC,   vf4,vf8
	vmadday.xyzw	ACC,   vf5,vf8
	vmaddaz.xyzw	ACC,   vf6,vf8
	vmaddw.xyzw	vf9,vf7,vf8
	sqc2    vf9,0x0(%0)
	": : "r" (v0) , "r" (m0) ,"r" (v1));
}


//==========================================================================
// 
// sceVu0MulMatrix		���ĥޥȥ�å������Ѥ����
// ����
// 	void sceVu0MulMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FMATRIX m2)
// ����
// 	m1,m2		����:�ޥȥ�å���
// 	m0		����:�ޥȥ�å���
// ����
// 	�ޥȥ�å���m1�˥ޥȥ�å���m2�򱦤���軻���ơ�m0��Ϳ����
// 
// 		m0=m1*m2
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	

void sceVu0MulMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1,sceVu0FMATRIX m2)
{

	asm __volatile__("
	lqc2    vf4,0x0(%2)
	lqc2    vf5,0x10(%2)
	lqc2    vf6,0x20(%2)
	lqc2    vf7,0x30(%2)
	li    $7,4
_loopMulMatrix:
	lqc2    vf8,0x0(%1)
	vmulax.xyzw	ACC,   vf4,vf8
	vmadday.xyzw	ACC,   vf5,vf8
	vmaddaz.xyzw	ACC,   vf6,vf8
	vmaddw.xyzw	vf9,vf7,vf8
	sqc2    vf9,0x0(%0)
	addi    $7,-1
	addi    %1,0x10
	addi    %0,0x10
	bne    $0,$7,_loopMulMatrix
	":: "r" (m0), "r" (m2), "r" (m1) : "$7");
}

//==========================================================================
// 
// sceVu0OuterProduct		���ĤΥ٥��ȥ�γ��Ѥ����
// ����
// 	void sceVu0OuterProduct(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
// ����
// 	v1,v2		����:�٥��ȥ�
// 	v0		����:�٥��ȥ�
// ����
// 	�٥��ȥ�v1,v2�γ��Ѥ����v0��Ϳ����
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	
 
void sceVu0OuterProduct(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
{
	asm __volatile__
	("
	lqc2    vf4,0x0(%1)
	lqc2    vf5,0x0(%2)
	vopmula.xyz	ACC,vf4,vf5
	vopmsub.xyz	vf6,vf5,vf4
	vsub.w vf6,vf6,vf6		#vf6.xyz=0;
	sqc2    vf6,0x0(%0)
	": : "r" (v0) , "r" (v1) ,"r" (v2));
}

//==========================================================================
// 
// sceVu0InnerProduct		���ĤΥ٥��ȥ�����Ѥ����
// ����
// 	float sceVu0InnerProduct(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
// ����
// 	v0,v1		����:�٥��ȥ�
// ����
// 	�٥��ȥ�v0,v1�����Ѥ����
// �֤���
// 	����
// 
//-------------------------------------------------------------------------- 	

float sceVu0InnerProduct(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
	register float ret;

	asm __volatile__("
	lqc2    vf4,0x0(%1)
	lqc2    vf5,0x0(%2)
	vmul.xyz vf5,vf4,vf5
	vaddy.x vf5,vf5,vf5
	vaddz.x vf5,vf5,vf5
	qmfc2   $2 ,vf5
	mtc1    $2,%0
	": "=f" (ret) :"r" (v0) ,"r" (v1) :"$2" );
	return ret;
}

//==========================================================================
// sceVu0Normalize		�٥��ȥ����������Ԥ�
// ����
// 	void sceVu0Normalize(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
// ����
// 	v1		����:�٥��ȥ�
// 	v0		����:�٥��ȥ�
// ����
// 	�٥��ȥ�v0����������Ԥ�v1��Ϳ����
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	
void sceVu0Normalize(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
#if 0
	asm __volatile__("
	lqc2    vf4,0x0(%1)
	vmul.xyz vf5,vf4,vf4
	vaddy.x vf5,vf5,vf5
	vaddz.x vf5,vf5,vf5

	vrsqrt Q,vf0w,vf5x
	vsub.xyzw vf6,vf0,vf0		#vf6.xyzw=0;
	vaddw.xyzw vf6,vf6,vf4		#vf6.w=vf4.w;
	vwaitq

	vmulq.xyz  vf6,vf4,Q
	sqc2    vf6,0x0(%0)
	": : "r" (v0) , "r" (v1));

#else   //rsqrt bug 

        asm __volatile__("
        lqc2    vf4,0x0(%1)
        vmul.xyz vf5,vf4,vf4
        vaddy.x vf5,vf5,vf5
        vaddz.x vf5,vf5,vf5

        vsqrt Q,vf5x
        vwaitq
        vaddq.x vf5x,vf0x,Q
        vnop
        vnop
        vdiv    Q,vf0w,vf5x
        vsub.xyzw vf6,vf0,vf0           #vf6.xyzw=0;
        vwaitq

        vmulq.xyz  vf6,vf4,Q
        sqc2    vf6,0x0(%0)
        ": : "r" (v0) , "r" (v1));
#endif
}


//==========================================================================
// 
// sceVu0TransposeMatrix	�ޥȥ�å�����ž�ֹ�������
// ����
// 	void sceVu0TransposeMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1)
// ����
// 	m1		����:�ޥȥ�å���
// 	m0		����:�ޥȥ�å���
// ����
// 	�ޥȥ�å���m1��ž�ֹ������m0��Ϳ����
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	

void sceVu0TransposeMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1)
{
	asm __volatile__("
	lq $8,0x0000(%1)
	lq $9,0x0010(%1)
	lq $10,0x0020(%1)
	lq $11,0x0030(%1)

	pextlw     $12,$9,$8
	pextuw     $13,$9,$8
	pextlw     $14,$11,$10
	pextuw     $15,$11,$10

	pcpyld     $8,$14,$12
	pcpyud     $9,$12,$14
	pcpyld     $10,$15,$13
	pcpyud     $11,$13,$15

	sq $8,0x0000(%0)
	sq $9,0x0010(%0)
	sq $10,0x0020(%0)
	sq $11,0x0030(%0)
	": : "r" (m0) , "r" (m1));
}


//==========================================================================
//sceVu0InversMatrix	�չ��� (4x4) ����롣�ʹ������§�Ȳ��ꤹ���
// ����
//	void sceVu0InversMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1);
// ����
// 	m1		����:�ޥȥ�å���(��ž & ʿ�԰�ư)
// 	m0		����:�ޥȥ�å���
// ����
// 	�ޥȥ�å���m1�εչ������ơ��ޥȥ�å���m0��Ϳ����
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	

void sceVu0InversMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1)
{
	asm __volatile__("
	lq $8,0x0000(%1)
	lq $9,0x0010(%1)
	lq $10,0x0020(%1)
	lqc2 vf4,0x0030(%1)

	vmove.xyzw vf5,vf4
	vsub.xyz vf4,vf4,vf4		#vf4.xyz=0;
	vmove.xyzw vf9,vf4
	qmfc2    $11,vf4

	#ž��
	pextlw     $12,$9,$8
	pextuw     $13,$9,$8
	pextlw     $14,$11,$10
	pextuw     $15,$11,$10
	pcpyld     $8,$14,$12
	pcpyud     $9,$12,$14
	pcpyld     $10,$15,$13

	qmtc2    $8,vf6
	qmtc2    $9,vf7
	qmtc2    $10,vf8

	#����
	vmulax.xyz	ACC,   vf6,vf5
	vmadday.xyz	ACC,   vf7,vf5
	vmaddz.xyz	vf4,vf8,vf5
	vsub.xyz	vf4,vf9,vf4

	sq $8,0x0000(%0)
	sq $9,0x0010(%0)
	sq $10,0x0020(%0)
	sqc2 vf4,0x0030(%0)
	": : "r" (m0) , "r" (m1):"$8","$9","$10","$11","$12","$13","$14","$15");

}


//==========================================================================
//sceVu0DivVector	v0 = v1/q  (DIV + MULq)
// ����
//	void sceVu0DivVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float q)
// ����
// 	v1		����:�٥��ȥ�
// 	q		����:�����顼
// 	v0		����:�٥��ȥ�
// ����
// 	�٥��ȥ�v1�򥹥��顼q�ǽ������ơ��٥��ȥ�v0��Ϳ����
// �֤���
// 	�ʤ�
//
//-------------------------------------------------------------------------- 	
void sceVu0DivVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float q)
{
	asm __volatile__("
	lqc2    vf4,0x0(%1)
	mfc1    $8,%2
	qmtc2    $8,vf5
	vdiv    Q,vf0w,vf5x
	vwaitq
	vmulq.xyzw      vf4,vf4,Q
	sqc2    vf4,0x0(%0)
	": : "r" (v0) , "r" (v1), "f" (q):"$8");
}

//==========================================================================
//sceVu0DivVectorXYZ	v0.xyz = v1/q (w ����¸����������)
// ����
//	void sceVu0DivVectorXYZ(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float q)
// ����
// 	v1		����:�٥��ȥ�
// 	q		����:�����顼
// 	v0		����:�٥��ȥ�
// ����
// 	�٥��ȥ�v1��x,y�����Ǥ򥹥���q�ǽ������ơ��٥��ȥ�v0��Ϳ����
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	

void sceVu0DivVectorXYZ(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float q)
{
	asm __volatile__("
	lqc2    vf4,0x0(%1)
	mfc1    $8,%2
	qmtc2    $8,vf5

	vdiv    Q,vf0w,vf5x
	vwaitq
	vmulq.xyz      vf4,vf4,Q

	sqc2    vf4,0x0(%0)
	": : "r" (v0) , "r" (v1), "f" (q) : "$8");
}

//==========================================================================
//sceVu0InterVector	���� 
// ����
//	void sceVu0InterVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2, float t)
// ����
// 	v1,v2		����:�٥��ȥ�
//	t		����:������	
// 	v0		����:�٥��ȥ�
// ����
// 	// v0 = v1*t + v2*(1-t) ������)
// �֤���
// 	�ʤ�
//
//-------------------------------------------------------------------------- 	
void sceVu0InterVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2, float t)
{
	asm __volatile__("
	lqc2    vf4,0x0(%1)
	lqc2    vf5,0x0(%2)
	mfc1    $8,%3
	qmtc2    $8,vf6

        vaddw.x    vf7,vf0,vf0	#vf7.x=1;
        vsub.x    vf8,vf7,vf6	#vf8.x=1-t;
	vmulax.xyzw	ACC,   vf4,vf6
	vmaddx.xyzw	vf9,vf5,vf8

	sqc2    vf9,0x0(%0)
	": : "r" (v0) , "r" (v1), "r" (v2), "f" (t) : "$8");
}

//==========================================================================
//sceVu0AddVector	4����û� (ADD/xyzw)
// ����
//	void sceVu0AddVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
// ����
// 	v1,v2		����:�٥��ȥ�
// 	v0		����:�٥��ȥ�
// ����
// 	�٥��ȥ�v1�γ����Ǥȥ٥��ȥ�v2�γ����Ǥ�ơ��û����롣
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	
void sceVu0AddVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
{
	asm __volatile__("
	lqc2    vf4,0x0(%1)
	lqc2    vf5,0x0(%2)
	vadd.xyzw	vf6,vf4,vf5
	sqc2    vf6,0x0(%0)
	": : "r" (v0) , "r" (v1), "r" (v2));
}

//==========================================================================
//sceVu0SubVector		4���󸺻� (SUB/xyzw)
// ����
//	void sceVu0SubVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
// ����
// 	v1,v2		����:�٥��ȥ�
// 	v0		����:�٥��ȥ�
// ����
// 	�٥��ȥ�v1�γ����Ǥȥ٥��ȥ�v2�γ����Ǥ�ơ��������롣
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	

void sceVu0SubVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
{
	asm __volatile__("
	lqc2    vf4,0x0(%1)
	lqc2    vf5,0x0(%2)
	vsub.xyzw	vf6,vf4,vf5
	sqc2    vf6,0x0(%0)
	": : "r" (v0) , "r" (v1), "r" (v2));
}

//==========================================================================
//sceVu0MulVector	4�����ѻ���MUL/xyzw: ���ѤǤϤʤ����Ȥ���ա�
// ����
//	void sceVu0MulVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
// ����
// 	v1,v2		����:�٥��ȥ�
// 	v0		����:�٥��ȥ�
// ����
// 	�٥��ȥ�v1�γ����Ǥȥ٥��ȥ�v2�γ����Ǥ�ơ��軻���롣
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	

void sceVu0MulVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
{
	asm __volatile__("
	lqc2    vf4,0x0(%1)
	lqc2    vf5,0x0(%2)
	vmul.xyzw	vf6,vf4,vf5
	sqc2    vf6,0x0(%0)
	": : "r" (v0) , "r" (v1), "r" (v2));
}

//==========================================================================
//sceVu0ScaleVector	�����顼�ͤ�軻���� (MULx/xyzw)
// ����
//	void sceVu0ScaleVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float t)
// ����
// 	v1		����:�٥��ȥ�
// 	t		����:�����顼
// 	v0		����:�٥��ȥ�
// ����
// 	�٥��ȥ�v1�˥����顼t��軻���ƥ٥��ȥ�v0��Ϳ����
// �֤���
// 	�ʤ�
//
//-------------------------------------------------------------------------- 	

void sceVu0ScaleVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float t)
{
	asm __volatile__("
	lqc2    vf4,0x0(%1)
	mfc1    $8,%2
	qmtc2    $8,vf5
	vmulx.xyzw	vf6,vf4,vf5
	sqc2    vf6,0x0(%0)
	": : "r" (v0) , "r" (v1), "f" (t):"$8");
}

//==========================================================================
//sceVu0TransMatrix	�����ʿ�԰�ư
// ����
//	void sceVu0TransMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FVECTOR tv);
// ����
// 	tv		����:��ư�٥��ȥ�
// 	m1		����:�ޥȥ�å���
// 	m0		����:�ޥȥ�å���
// ����
// 	Ϳ����줿�٥��ȥ�ʬ���ޥȥ�å�����ʿ�԰�ư����
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	
void sceVu0TransMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FVECTOR tv)
{
	asm __volatile__("
	lqc2    vf4,0x0(%2)
	lqc2    vf5,0x30(%1)

	lq    $7,0x0(%1)
	lq    $8,0x10(%1)
	lq    $9,0x20(%1)
	vadd.xyz	vf5,vf5,vf4
	sq    $7,0x0(%0)
	sq    $8,0x10(%0)
	sq    $9,0x20(%0)
	sqc2    vf5,0x30(%0)
	": : "r" (m0) , "r" (m1), "r" (tv):"$7","$8","$9");
}

//==========================================================================
//sceVu0CopyVector		�����ʣ�� (lq �� sq ���ȹ礻��alignment check �Ѥ�)
// ����
//	void sceVu0CopyVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1);
// ����
// 	m1		����:�ޥȥ�å���
// 	m0		����:�ޥȥ�å���
// ����
// 	�ޥȥ�å���m1��ޥȥ�å���m0�˥��ԡ����롣
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	

void sceVu0CopyVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
	asm __volatile__("
	lq    $6,0x0(%1)
	sq    $6,0x0(%0)
	": : "r" (v0) , "r" (v1):"$6");
}


//==========================================================================
//sceVu0CopyMatrix		�����ʣ�� (lq �� sq ���ȹ礻��alignment check �Ѥ�)
// ����
//	void sceVu0CopyMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1);
// ����
// 	m1		����:�ޥȥ�å���
// 	m0		����:�ޥȥ�å���
// ����
// 	�ޥȥ�å���m1��ޥȥ�å���m0�˥��ԡ����롣
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	

void sceVu0CopyMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1)
{
	asm __volatile__("
	lq    $6,0x0(%1)
	lq    $7,0x10(%1)
	lq    $8,0x20(%1)
	lq    $9,0x30(%1)
	sq    $6,0x0(%0)
	sq    $7,0x10(%0)
	sq    $8,0x20(%0)
	sq    $9,0x30(%0)
	": : "r" (m0) , "r" (m1):"$6","$7","$8","$9");
}

//==========================================================================
// sceVu0FTOI4Vector	FTOI4
// ����
//	void sceVu0FTOI4Vector(sceVu0IVECTOR v0, sceVu0FVECTOR v1);
// ����
// 	v1		����:�٥��ȥ�
// 	v0		����:�٥��ȥ�
// ����
// 	Ϳ����줿�٥��ȥ�����Ǥ��������Ѵ�����
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	

void sceVu0FTOI4Vector(sceVu0IVECTOR v0, sceVu0FVECTOR v1)
{
	asm __volatile__("
	lqc2    vf4,0x0(%1)
	vftoi4.xyzw	vf5,vf4
	sqc2    vf5,0x0(%0)
	": : "r" (v0) , "r" (v1));
}

//==========================================================================
//sceVu0FTOI0Vector 	FTOI0
// ����
//	void sceVu0FTOI0Vector(sceVu0IVECTOR v0, sceVu0FVECTOR v1);
// ����
// 	v1		����:�٥��ȥ�
// 	v0		����:�٥��ȥ�
// ����
// 	Ϳ����줿�٥��ȥ�����Ǥ��������Ѵ�����
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	
void sceVu0FTOI0Vector(sceVu0IVECTOR v0, sceVu0FVECTOR v1)
{
	asm __volatile__("
	lqc2    vf4,0x0(%1)
	vftoi0.xyzw	vf5,vf4
	sqc2    vf5,0x0(%0)
	": : "r" (v0) , "r" (v1));
}


//==========================================================================
// sceVu0ITOF4Vector	ITOF4
// ����
//	void sceVu0ITOF4Vector(sceVu0FVECTOR v0, sceVu0IVECTOR v1);
// ����
// 	v1		����:�٥��ȥ�
// 	v0		����:�٥��ȥ�
// ����
// 	Ϳ����줿�٥��ȥ�����Ǥ��������Ѵ�����
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	

void sceVu0ITOF4Vector(sceVu0FVECTOR v0, sceVu0IVECTOR v1)
{
	asm __volatile__("
	lqc2    vf4,0x0(%1)
	vitof4.xyzw	vf5,vf4
	sqc2    vf5,0x0(%0)
	": : "r" (v0) , "r" (v1));
}

//==========================================================================
//sceVu0ITOF0Vector 	ITOF0
// ����
//	void sceVu0ITOF0Vector(sceVu0FVECTOR v0, sceVu0IVECTOR v1);
// ����
// 	v1		����:�٥��ȥ�
// 	v0		����:�٥��ȥ�
// ����
// 	Ϳ����줿�٥��ȥ�����Ǥ��������Ѵ�����
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	
void sceVu0ITOF0Vector(sceVu0FVECTOR v0, sceVu0IVECTOR v1)
{
	asm __volatile__("
	lqc2    vf4,0x0(%1)
	vitof0.xyzw	vf5,vf4
	sqc2    vf5,0x0(%0)
	": : "r" (v0) , "r" (v1));
}

//==========================================================================
//sceVu0UnitMatrix	ñ�̹��� 
// ����
//	void sceVu0UnitMatrix(sceVu0FMATRIX m0);
// ����
// 	m1		����:�ޥȥ�å���
// 	m0		����:�ޥȥ�å���
// ����
// 	Ϳ����줿�����ñ�̹�����Ѵ�����
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	

void sceVu0UnitMatrix(sceVu0FMATRIX m0)
{
	asm __volatile__("
	vsub.xyzw	vf4,vf0,vf0 #vf4.xyzw=0;
	vadd.w	vf4,vf4,vf0
	vmr32.xyzw	vf5,vf4
	vmr32.xyzw	vf6,vf5
	vmr32.xyzw	vf7,vf6
	sqc2    vf4,0x30(%0)
	sqc2    vf5,0x20(%0)
	sqc2    vf6,0x10(%0)
	sqc2    vf7,0x0(%0)
	": : "r" (m0));
}


//==========================================================================
// ����
//	void _sceVu0ecossin(float x)
// ����
// ����
// 	sin,cos�����
//	ͭ���ϰ�:-��/2 �� ��/2
//	RotMatrix?�ǤΤ߻��Ѳ�
// �֤���
// 
//-------------------------------------------------------------------------- 	
	asm ("
	.data
	.align 4
	S5432:
	.word	0x362e9c14, 0xb94fb21f, 0x3c08873e, 0xbe2aaaa4
	.text
	");


static void _sceVu0ecossin(float t)
{	
	asm __volatile__("
	la	$8,S5432	# S5-S2 �η����� VF05 ��ž��
	lqc2	vf05,0x0($8)	#
	vmr32.w vf06,vf06	# VF06.x(v)��VF06.w �˥��ԡ�
	vaddx.x vf04,vf00,vf06	# VF06.x(v)��VF04.x �˥��ԡ�
	vmul.x vf06,vf06,vf06	# VF06.x �򼫾褷�� v^2 �ˤ���
	vmulx.yzw vf04,vf04,vf00# VF04.yzw=0
	vmulw.xyzw vf08,vf05,vf06# S2-S5 �� VF06.w(v) �򤫤���
	vsub.xyzw vf05,vf00,vf00 #0,0,0,0 |x,y,z,w
	vmulx.xyzw vf08,vf08,vf06# VF06.x(v^2) �򤫤���
	vmulx.xyz vf08,vf08,vf06# VF06.x(v^2) �򤫤���
	vaddw.x vf04,vf04,vf08	# s+=k2
	vmulx.xy vf08,vf08,vf06	# VF06.x(v^2) �򤫤���
	vaddz.x vf04,vf04,vf08	# s+=z
	vmulx.x vf08,vf08,vf06	# VF06.x(v^2) �򤫤���
	vaddy.x vf04,vf04,vf08	# s+=y
	vaddx.x vf04,vf04,vf08	# s+=x (sin ��λ)

	vaddx.xy vf04,vf05,vf04	# .xy=s �ɲ� 

	vmul.x vf07,vf04,vf04	# VF07.x=s*s
	vsubx.w vf07,vf00,vf07	# VF07.w = 1-s*s

	vsqrt Q,vf07w		# Q= ��|1-s*s| (cos ��λ)
	vwaitq

	vaddq.x vf07,vf00,Q     # vf07.x=c

	bne	$7,$0,_ecossin_01
	vaddx.x vf04,vf05,vf07	# VF04.x=s
	b	_ecossin_02
_ecossin_01:	
	vsubx.x vf04,vf05,vf07	# VF04.x=s
_ecossin_02:	

	": : :"$6","$7","$8","$f0");
}

//==========================================================================
//sceVu0RotMatrixZ	�ڼ����濴�Ȥ�������β�ž 
// ����
//	void sceVu0RotMatrixZ(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float rz);
// ����
// 	rz		����:��ž��(ͭ���ϰ�:-�� �� ��)
// 	m1		����:�ޥȥ�å���
// 	m0		����:�ޥȥ�å���
// ����
// 	��ž��rt���ؼ����濴�Ȥ�����ž�ޥȥ�å�������ơ�
//	�ޥȥ�å���m1�˺�¦����軻���ơ����η�̤�ޥȥ�å���
//	m0��Ϳ����
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	
void sceVu0RotMatrixZ(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float rz)
{
	asm __volatile__("
	mtc1	$0,$f0
	c.olt.s %2,$f0
	li.s    $f0,1.57079637050628662109e0	
	bc1f    _RotMatrixZ_01
	add.s   %2,$f0,%2			#rx=rx+��/2
	li 	$7,1				#cos(rx)=sin(rx+��/2)
	j	_RotMatrixZ_02
_RotMatrixZ_01:
	sub.s   %2,$f0,%2			#rx=��/2-rx
	move	$7,$0
_RotMatrixZ_02:

        mfc1    $8,%2
        qmtc2    $8,vf6
	move	$6,$31	# ra ��¸ (�����ϥ����å���Ȥ��٤�)

	jal	_sceVu0ecossin	# sin(roll), cos(roll)
	move	$31,$6	# ra ����
			#vf05:0,0,0,0 |x,y,z,w
	vmove.xyzw vf06,vf05
	vmove.xyzw vf07,vf05
	vmove.xyzw vf09,vf00
	vsub.xyz vf09,vf09,vf09 	#0,0,0,1 |x,y,z,w
	vmr32.xyzw vf08,vf09	#0,0,1,0 |x,y,z,w

	vsub.zw vf04,vf04,vf04 #vf04.zw=0 s,c,0,0 |x,y,z,w
	vaddx.y vf06,vf05,vf04 #vf06 0,s,0,0 |x,y,z,w
	vaddy.x vf06,vf05,vf04 #vf06 c,s,0,0 |x,y,z,w
	vsubx.x vf07,vf05,vf04 #vf07 -s,0,0,0 |x,y,z,w
	vaddy.y vf07,vf05,vf04 #vf07 -s,c,0,0 |x,y,z,w

	li    $7,4
	_loopRotMatrixZ:
	lqc2    vf4,0x0(%1)
	vmulax.xyzw	ACC,   vf6,vf4
	vmadday.xyzw	ACC,   vf7,vf4
	vmaddaz.xyzw	ACC,   vf8,vf4
	vmaddw.xyzw	vf5,  vf9,vf4
	sqc2    vf5,0x0(%0)
	addi    $7,-1
	addi    %1,0x10
	addi    %0,0x10
	bne    $0,$7,_loopRotMatrixZ
	": : "r" (m0) , "r" (m1), "f" (rz):"$6","$7","$8","$f0");
}


//==========================================================================
//sceVu0RotMatrixX	�ؼ����濴�Ȥ�������β�ž 
// ����
//	void sceVu0RotMatrixX(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float ry);
// ����
// 	rx		����:��ž��(ͭ���ϰ�:-�� �� ��)
// 	m1		����:�ޥȥ�å���
// 	m0		����:�ޥȥ�å���
// ����
// 	��ž��rt���ؼ����濴�Ȥ�����ž�ޥȥ�å�������ơ�
//	�ޥȥ�å���m1�˺�¦����軻���ơ����η�̤�ޥȥ�å���
//	m0��Ϳ����
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	
void sceVu0RotMatrixX(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float rx)
{
	asm __volatile__("
	mtc1	$0,$f0
	c.olt.s %2,$f0
	li.s    $f0,1.57079637050628662109e0	
	bc1f    _RotMatrixX_01
	add.s   %2,$f0,%2			#rx=rx+��/2
	li 	$7,1				#cos(rx)=sin(rx+��/2)
	j	_RotMatrixX_02
_RotMatrixX_01:
	sub.s   %2,$f0,%2			#rx=��/2-rx
	move	$7,$0
_RotMatrixX_02:

	mfc1    $8,%2
	qmtc2    $8,$vf6
	move	$6,$31	# ra ��¸ (�����ϥ����å���Ȥ��٤�)
	jal	_sceVu0ecossin	# sin(roll), cos(roll)
	move	$31,$6	# ra ����
			#vf05:0,0,0,0 |x,y,z,w
	vmove.xyzw vf06,vf05	
	vmove.xyzw vf07,vf05
	vmove.xyzw vf08,vf05
	vmove.xyzw vf09,vf05
	vaddw.x vf06,vf05,vf00 #vf06 1,0,0,0 |x,y,z,w
	vaddw.w vf09,vf05,vf00 #vf09 0,0,0,1 |x,y,z,w

	vsub.zw vf04,vf04,vf04 #vf04.zw=0 s,c,0,0 |x,y,z,w
	vaddx.z vf07,vf05,vf04 #vf07.zw=0 0,0,s,0 |x,y,z,w
	vaddy.y vf07,vf05,vf04 #vf07.zw=0 0,c,s,0 |x,y,z,w
	vsubx.y vf08,vf05,vf04 #vf08.zw=0 0,-s,0,0 |x,y,z,w
	vaddy.z vf08,vf05,vf04 #vf08.zw=0 0,-s,c,0 |x,y,z,w

	li    $7,4
	_loopRotMatrixX:
	lqc2    vf4,0x0(%1)
	vmulax.xyzw	ACC,   vf6,vf4
	vmadday.xyzw	ACC,   vf7,vf4
	vmaddaz.xyzw	ACC,   vf8,vf4
	vmaddw.xyzw	vf5,  vf9,vf4
	sqc2    vf5,0x0(%0)
	addi    $7,-1
	addi    %1,0x10
	addi    %0,0x10
	bne    $0,$7,_loopRotMatrixX
	": : "r" (m0) , "r" (m1), "f" (rx):"$6","$7","$8","$f0");
}
//==========================================================================
//sceVu0RotMatrixY	�ټ����濴�Ȥ�������β�ž 
// ����
//	void sceVu0RotMatrixY(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float ry);
// ����
// 	ry		����:��ž��(ͭ���ϰ�:-�� �� ��)
// 	m1		����:�ޥȥ�å���
// 	m0		����:�ޥȥ�å���
// ����
// 	��ž��rt���ټ����濴�Ȥ�����ž�ޥȥ�å�������ơ�
//	�ޥȥ�å���m1�˺�¦����軻���ơ����η�̤�ޥȥ�å���
//	m0��Ϳ����
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	
void sceVu0RotMatrixY(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float ry)
{
	asm __volatile__("
	mtc1	$0,$f0
	c.olt.s %2,$f0
	li.s    $f0,1.57079637050628662109e0	
	bc1f    _RotMatrixY_01
	add.s   %2,$f0,%2			#rx=rx+��/2
	li 	$7,1				#cos(rx)=sin(rx+��/2)
	j	_RotMatrixY_02
_RotMatrixY_01:
	sub.s   %2,$f0,%2			#rx=��/2-rx
	move	$7,$0
_RotMatrixY_02:

	mfc1    $8,%2
	qmtc2    $8,vf6
	move	$6,$31	# ra ��¸ (�����ϥ����å���Ȥ��٤�)
	jal	_sceVu0ecossin	# sin(roll), cos(roll)
	move	$31,$6	# ra ����
			#vf05:0,0,0,0 |x,y,z,w
	vmove.xyzw vf06,vf05   
	vmove.xyzw vf07,vf05
	vmove.xyzw vf08,vf05
	vmove.xyzw vf09,vf05
	vaddw.y vf07,vf05,vf00 #vf07 0,1,0,0 |x,y,z,w
	vaddw.w vf09,vf05,vf00 #vf09 0,0,0,1 |x,y,z,w

	vsub.zw vf04,vf04,vf04 #vf04.zw=0 s,c,0,0 |x,y,z,w
	vsubx.z vf06,vf05,vf04 #vf06 0,0,-s,0 |x,y,z,w
	vaddy.x vf06,vf05,vf04 #vf06 c,0,-s,0 |x,y,z,w
	vaddx.x vf08,vf05,vf04 #vf08 s,0,0,0 |x,y,z,w
	vaddy.z vf08,vf05,vf04 #vf08 s,0,c,0 |x,y,z,w

	li    $7,4
	_loopRotMatrixY:
	lqc2    vf4,0x0(%1)
	vmulax.xyzw	ACC,   vf6,vf4
	vmadday.xyzw	ACC,   vf7,vf4
	vmaddaz.xyzw	ACC,   vf8,vf4
	vmaddw.xyzw	vf5,  vf9,vf4
	sqc2    vf5,0x0(%0)
	addi    $7,-1
	addi    %1,0x10
	addi    %0,0x10
	bne    $0,$7,_loopRotMatrixY
	": : "r" (m0) , "r" (m1), "f" (ry):"$6","$7","$8","$f0");
}

//==========================================================================
//sceVu0RotMatrixY	����β�ž 
// ����
//	void sceVu0RotMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FVECTOR rot);
// ����
// 	m0		����:�ޥȥ�å���
// 	m1		����:�ޥȥ�å���
// 	rot		����:x,y,z���β�ž��(ͭ���ϰ�:-�� �� ��)
// ����
//	Z�����濴�Ȥ����ž�ޥȥ�å�����rot[2]���顢Y�����濴�Ȥ����ž��
//	�ȥ�å�����rot[1]���顢X�����濴�Ȥ����ž�ޥȥ�å�����rot[0]����
//	���줾���ᡢ��˥ޥȥ�å���m1�˺�¦����軻���ơ����η�̤�ޥ�
//	��å���m0���֤��ޤ���
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	
void sceVu0RotMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FVECTOR rot)
{
	sceVu0RotMatrixZ(m0, m1, rot[2]);
	sceVu0RotMatrixY(m0, m0, rot[1]);
	sceVu0RotMatrixX(m0, m0, rot[0]);
}

//==========================================================================
//sceVu0ClampVector	�٥��ȥ�Υ�����
// ����
//	void sceVu0ClampVector(sceVu0FVECTOR v0, sceVu0FFVECTOR v1, float min, float max)
// ����
// 	v0		����:�٥��ȥ�
// 	v1		����:�٥��ȥ�
// 	min		����:�Ǿ���
// 	max		����:������
// ����
//	�٥��ȥ�v1�γ����Ǥ򡢺Ǿ���min��������max�ǥ����פ�����̤�٥�
//	�ȥ�v0���֤��ޤ���
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	
void sceVu0ClampVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float min, float max)
{
	asm __volatile__("
        mfc1    $8,%2
        mfc1    $9,%3
	lqc2    vf6,0x0(%1)
        qmtc2    $8,vf4
        qmtc2    $9,vf5
	vmaxx.xyzw $vf06,$vf06,$vf04
	vminix.xyzw $vf06,$vf06,$vf05
	sqc2    vf6,0x0(%0)
	": : "r" (v0) , "r" (v1), "f" (min), "f" (max):"$8","$9");
}

//==========================================================================
//sceVu0CameraMatrix	���ɥӥ塼���������
// ����
//	void sceVu0CameraMatrix(sceVu0FMATRIX m, sceVu0FVECTOR p, sceVu0FVECTOR zd, sceVu0FVECTOR yd);
// ����
// 	m		����:�ޥȥ�å���(�ӥ塼���ɺ�ɸ)
// 	p		����:�ޥȥ�å���(����)
// 	zd		����:�ޥȥ�å���(����)
// 	yd		����:�ޥȥ�å���(��ľ����)
// ����
//	����p��(0,0,0)�ˡ�����zd��(0,0,1)�ˡ���ľ����yd��(0,1,0)�ˤ��Ѵ���
//	��褦�ʹ�����ᡢm���֤��ޤ���
// �֤���
// 	�ʤ�
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
//sceVu0NormalLightMatrix         �Ρ��ޥ�饤�ȹ��������
// ����
//	void sceVu0NormalLightMatrix(sceVu0FMATRIX m, sceVu0FVECTOR l0, sceVu0FVECTOR l1, sceVu0FVECTOR l2);
// ����
// 	m		����:�ޥȥ�å���
// 	l0		����:�ޥȥ�å���(����0������)
// 	l1		����:�ޥȥ�å���(����1������)
// 	l2		����:�ޥȥ�å���(����2������)
// ����
//	����l0������l1������l2����Ρ��ޥ�饤�ȹ�����ᡢm���֤��ޤ���
// �֤���
// 	�ʤ�
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
//sceVu0LightColorMatrix		�饤�ȥ��顼���������
// ����
//	void sceVu0LightColorMatrix(sceVu0FMATRIX m,  sceVu0FVECTOR c0, sceVu0FVECTOR c1, sceVu0FVECTOR c2, sceVu0FVECTOR a)
// ����
// 	m		����:�ޥȥ�å���
// 	c0		����:�٥��ȥ�(������0)
// 	c1		����:�٥��ȥ�(������1)
// 	c2		����:�٥��ȥ�(������2)
// 	a		����:�٥��ȥ�(����ӥ����)
// ����
//	������c0,c1,c2�ȴĶ���a����饤�ȥ��顼������ᡢm���֤��ޤ���
// �֤���
// 	�ʤ�
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
//sceVu0ViewScreenMatrix          �ӥ塼�����꡼����������
// ����
//	void sceVu0ViewScreenMatrix(sceVu0FMATRIX m, float scrz, float ax, float ay, float cx, float cy, float zmin,  float zmax,  float nearz, float farz)
// ����
// 	m		����:�ޥȥ�å���
//	scrz            ����:(�����꡼��ޤǤε�Υ)
//	ax              ����:(�����������ڥ�����)
//	ay              ����:(�����������ڥ�����)
//	cx              ����:(�����꡼����濴�غ�ɸ)
//	cy              ����:(�����꡼����濴�ٺ�ɸ)
//	zmin            ����:(�ڥХåե��Ǿ���)
//	zmax            ����:(�ڥХåե�������)
//	nearz           ����:(�˥�����å��̤Σ�)
//	farz            ����:(�ե�������å��̤Σ�)
// ����
//	���ꤵ�줿�ƥѥ�᡼���˽��äƥӥ塼�����꡼�������ᡢm���֤���
//	����
// �֤���
// 	�ʤ�
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
// sceVu0DropShadowMatrix          �ɥ�åץ���ɥ��ͱƹ��������
// ����
//        void sceVu0DropShadowMatrix(sceVu0FMATRIX m, sceVu0FVECTOR lp, float a, float b, float c, int mode);
// ����
// 	�ʤ�
//	m               ����:�ޥȥ�å���
//	lp              ����:�٥��ȥ�(�����ΰ���)
//	a               ����:�Ƥ������
//	b               ����:�Ƥ������
//	c               ����:�Ƥ������
//	mode            ����:�����μ���
//			0: ʿ�Ը���
//			1: ������
// ����
//	ax+by+cz=1��ɽ�������ʿ�̤�lp��mode�ǻ��ꤵ����������αƤ����
//	���������ᡢm���֤��ޤ���
// �֤���
// 	�ʤ�
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
//sceVu0RotTransPersN	Ʃ���Ѵ�
// ����
//	void sceVu0RotTransPersN(sceVu0IVECTOR *v0, sceVu0FMATRIX m0, sceVu0FVECTOR v1, int n, int mode);
// ����
// 	v0		����:�٥��ȥ�(�����꡼���ɸ)
// 	m0		����:�ޥȥ�å���
// 	v1		����:�٥��ȥ�
// 	n		����:ĺ����
// 	mode		����:v0[2],v0[3]���ͤ��ڤ��ؤ�
// 			     0:32bit���ʤ����꾮����(28.4)
// 			     1:32bit���ʤ����꾮����(32.0)
// ����
// 	Ϳ����줿n�Ĥ�ĺ���򥹥��꡼���ɸ��Ʃ���Ѵ����롣
//	����v0���ͤϡ�v0[0],v0[1]�ϡ�32bit����դ����꾮����(28.4)��
//	v0[2],v0[3]��mode=1�ΤȤ��ϡ�32bit���ʤ����꾮����(28.4)��
//	mode=0�ΤȤ��ϡ�32bit���ʤ����꾮����(32.0)���֤���
//	GIF��PACKED�⡼�ɤ�XYZF2,XYZF3���Ѥ�����ˤϡ�mode=0�ˤ�
//	��ȡ�PACK�������������ڤ�Ф����Τ�ͭ�ѤǤ��롣
//	
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	

void sceVu0RotTransPersN(sceVu0IVECTOR *v0, sceVu0FMATRIX m0, sceVu0FVECTOR *v1, int n, int mode)
{
	asm __volatile__("
	lqc2	vf4,0x0(%1)
	lqc2	vf5,0x10(%1)
	lqc2	vf6,0x20(%1)
	lqc2	vf7,0x30(%1)
_rotTPN_loop:
	lqc2	vf8,0x0(%2)
	vmulax.xyzw     ACC, vf4,vf8
	vmadday.xyzw    ACC, vf5,vf8
	vmaddaz.xyzw    ACC, vf6,vf8
	vmaddw.xyzw      vf9,vf7,vf8
	vdiv    Q,vf0w,vf9w
	vwaitq
	vmulq.xyz	vf9,vf9,Q
	vftoi4.xyzw	vf10,vf9
	beqz	%4,_rotTPN
	vftoi0.zw	vf10,vf9	
_rotTPN:
	sqc2	vf10,0x0(%0)

	addi	%3,-1
	addi	%2,0x10
	addi	%0,0x10
	bne	$0,%3,_rotTPN_loop
	": : "r" (v0) , "r" (m0) ,"r" (v1), "r" (n) ,"r" (mode));
}


//==========================================================================
//sceVu0RotTransPers	Ʃ���Ѵ�
// ����
//	void sceVu0RotTransPers(sceVu0IVECTOR v0, sceVu0FMATRIX m0, sceVu0FVECTOR v1);
// ����
// 	v0		����:�٥��ȥ�(�����꡼���ɸ)
// 	m0		����:�ޥȥ�å���
// 	v1		����:�٥��ȥ�
// 	mode		����:v0[2],v0[3]���ͤ��ڤ��ؤ�
// 			     0:32bit���ʤ����꾮����(28.4)
// 			     1:32bit���ʤ����꾮����(32.0)
// ����
// 	Ϳ����줿ĺ���򥹥��꡼���ɸ��Ʃ���Ѵ����롣
//	����v0���ͤϡ�v0[0],v0[1]�ϡ�32bit����դ����꾮����(28.4)��
//	v0[2],v0[3]��mode=1�ΤȤ��ϡ�32bit���ʤ����꾮����(28.4)��
//	mode=0�ΤȤ��ϡ�32bit���ʤ����꾮����(32.0)���֤���
//	GIF��PACKED�⡼�ɤ�XYZF2,XYZF3���Ѥ�����ˤϡ�mode=0�ˤ�
//	��ȡ�PACK�������������ڤ�Ф����Τ�ͭ�ѤǤ��롣
//	
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	

void sceVu0RotTransPers(sceVu0IVECTOR v0, sceVu0FMATRIX m0, sceVu0FVECTOR v1, int mode)
{
	asm __volatile__("
	lqc2	vf4,0x0(%1)
	lqc2	vf5,0x10(%1)
	lqc2	vf6,0x20(%1)
	lqc2	vf7,0x30(%1)
	lqc2	vf8,0x0(%2)
	vmulax.xyzw     ACC, vf4,vf8
	vmadday.xyzw    ACC, vf5,vf8
	vmaddaz.xyzw    ACC, vf6,vf8
	vmaddw.xyzw      vf9,vf7,vf8
	vdiv    Q,vf0w,vf9w
	vwaitq
	vmulq.xyz	vf9,vf9,Q
	vftoi4.xyzw	vf10,vf9
	beqz	%3,_rotTP
	vftoi0.zw	vf10,vf9	
_rotTP:
	sqc2	vf10,0x0(%0)

	": : "r" (v0) , "r" (m0) ,"r" (v1), "r" (mode));
}

//==========================================================================
// 
// sceVu0CopyVectorXYZ	�٥��ȥ��ʣ��
// ����
//	void sceVu0CopyVectorXYZ(sceVu0FVECTOR v0,sceVu0FVECTOR v1);
// ����
//	v0		����:�٥��ȥ�
//	v1		����:�٥��ȥ�
// ����
//	�٥��ȥ�v1��x,y,z���Ǥ�٥��ȥ�v0�˥��ԡ����ޤ���
//	�٥��ȥ�v0��w���Ǥϡ����Τޤޥ٥��ȥ�v0���֤��� 
// �֤���
// 	�ʤ�
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
// sceVu0InterVector	���ޥ٥��ȥ������
// ����
//	void sceVu0InterVector(sceVu0FVECTOR v0,sceVu0FVECTOR v1,sceVu0FVECTOR v2, float t)
// ����
//	v0		����:�٥��ȥ�
//	v1.v2		����:�٥��ȥ�
//	t		����:���ޥѥ�᡼��
// ����
//	�٥��ȥ�v1,v2����ӥѥ�᡼��t�������ޥ٥��ȥ���ᡢv0���֤��ޤ���
//	����ɽ������ȼ��ΤȤ���Ǥ���
//	 v0 = v1*t + v2*(1-t)
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	
void sceVu0InterVectorXYZ(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2, float r)
{
	asm __volatile__("
	lqc2    vf4,0x0(%1)
	lqc2    vf5,0x0(%2)
	mfc1    $8,%3
	qmtc2    $8,vf6

	vmove.w	vf9,vf4
	vaddw.x    vf7,vf0,vf0	#vf7.x=1;
	vsub.x    vf8,vf7,vf6	#vf8.x=1-t;
	vmulax.xyz	ACC,   vf4,vf6
	vmaddx.xyz	vf9,vf5,vf8

	sqc2    vf9,0x0(%0)
	": : "r" (v0) , "r" (v1), "r" (v2), "f" (r) : "$8");
}

//==========================================================================
// 
// sceVu0ScaleVectorXYZ	�����顼�ͤȥ٥��ȥ�ξ軻 (MULx/xyz)
// ����
//	void sceVu0ScaleVector(sceVu0FVECTOR v0,sceVu0FVECTOR v1,float t)
// ����
// 	�ʤ�
//	v0		����:�٥��ȥ�
//	v1		����:�٥��ȥ�
//	t		����:�����顼
// ����
//	�٥��ȥ�v1��x,y,z���Ǥ˥����顼t��軻������̤�٥��ȥ�v0���֤���
//	�����٥��ȥ�v0��w���Ǥϡ����Τޤޥ٥��ȥ�v0���֤��ޤ���
// �֤���
// 	�ʤ�
// 
//-------------------------------------------------------------------------- 	
void  sceVu0ScaleVectorXYZ(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float s)
{
	asm __volatile__("
	lqc2    vf4,0x0(%1)
	mfc1    $8,%2
	qmtc2    $8,vf5
	vmulx.xyz	vf4,vf4,vf5
	sqc2    vf4,0x0(%0)
	": : "r" (v0) , "r" (v1), "f" (s):"$8");
}

//==========================================================================
// 
// sceVu0ClipScreen	ĺ���Σǣ������ϰϤΥ���åԥ�
// ����
// 	int sceVu0ClipScreen(sceVu0FVECTOR v0)
// ����
// 	v0		����:�٥��ȥ�
// ����
// 	ĺ���٥��ȥ�v0���ǣ������ϰϤ����äƤ��뤫��Ĵ�٤롣
// �֤���
// 	0:ĺ��v0���ǣ������ϰϤ����äƤ��롣
// 
//-------------------------------------------------------------------------- 	

int sceVu0ClipScreen(sceVu0FVECTOR v0)
{
	register int ret;

	asm __volatile__("
	vsub.xyzw        vf04,vf00,vf00  #(0,0,0,0);
	li     %0,0x4580000045800000
	lqc2    vf07,0x0(%1)

	qmtc2  %0,vf06
	ctc2    $0,$vi16                #���ơ������ե饰�Υ��ꥢ

	vsub.xyw        vf05,vf07,vf04  #(z,ZBz,PPy,PPx)-(0,0,0,0);
	vsub.xy        vf05,vf06,vf07  #(Zmax,0,Ymax,Xmax) -(z,ZBz,PPy,PPx)

	vnop				
	vnop				
	vnop				
	vnop				
	vnop				
        cfc2    %0,$vi16                #���ơ������ե饰��READ
	andi	%0,%0,0xc0

	":"=r"(ret): "r" (v0));
	return ret;
}

//==========================================================================
// 
// sceVu0ClipScreen3	��ĺ���Σǣ������ϰϤΥ���åԥ�
// ����
// 	int sceVu0ClipScreen3(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
// ����
// 	v0,v1,v2		����:�٥��ȥ�
// ����
// 	ĺ���٥��ȥ�v0,v1,v2�����٤ƣǣ������ϰϤ����äƤ��뤫��Ĵ�٤롣
// �֤���
// 	0:���٤Ƥ�ĺ�����ǣ������ϰϤ����äƤ��롣
// 
//-------------------------------------------------------------------------- 	
 
int sceVu0ClipScreen3(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
{
	register int ret;

	asm __volatile__("
	vsub.xyzw        vf04,vf00,vf00  #(0,0,0,0);
	li     %0,0x4580000045800000
	lqc2    vf06,0x0(%1)
	lqc2    vf08,0x0(%2)
	lqc2    vf09,0x0(%3)

	qmtc2  %0,vf07
	ctc2    $0,$vi16                #���ơ������ե饰�Υ��ꥢ

	vsub.xyw        vf05,vf06,vf04  #(z,ZBz,PPy,PPx)-(0,0,0,0);
	vsub.xy        vf05,vf07,vf06  #(Zmax,0,Ymax,Xmax) -(z,ZBz,PPy,PPx)
	vsub.xyw        vf05,vf08,vf04  #(z,ZBz,PPy,PPx)-(0,0,0,0);
	vsub.xy        vf05,vf07,vf08  #(Zmax,0,Ymax,Xmax) -(z,ZBz,PPy,PPx)
	vsub.xyw        vf05,vf09,vf04  #(z,ZBz,PPy,PPx)-(0,0,0,0);
	vsub.xy        vf05,vf07,vf09  #(Zmax,0,Ymax,Xmax) -(z,ZBz,PPy,PPx)

	vnop				
	vnop				
	vnop				
	vnop				
	vnop				
	cfc2    %0,$vi16                #���ơ������ե饰��READ
	andi	%0,%0,0xc0

	":"=r"(ret): "r" (v0), "r" (v1), "r" (v2) );
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
// sceVu0ClipAll           ɽ���ϰϤˤ�륯��åԥ󥰸���
// ����
// 	int sceVu0ClipAll(sceVu0FVECTOR minv, sceVu0FVECTOR maxv, sceVu0FVECTOR ms,sceVu0FVECTOR *vm,int n)
// ����
//	minv			����:ɽ���ϰϤκǾ���
//	maxv			����:ɽ���ϰϤκ�����
//	ms			����:�ޥȥ�å����ʥ�ǥ�ݥ����꡼���
//	vm			����:ĺ���٥��ȥ�Υݥ���
//	n			����:ĺ����
// ����
//	vm��n�ǻ��ꤵ���n�Ĥ�ĺ�������٤�ɽ���ϰϤ����äƤ��ʤ����ɤ�����
//	Ĵ�٤ޤ���
// �֤���
//	���٤Ƥ�ĺ����ɽ���ϰϤ����äƤ��ʤ��Ȥ���1���֤��ޤ���
//
//-------------------------------------------------------------------------- 	
int sceVu0ClipAll(sceVu0FVECTOR minv, sceVu0FVECTOR maxv, sceVu0FMATRIX ms, sceVu0FVECTOR *vm, int n)
{
	register int ret;

	asm __volatile__("

	lqc2    vf8,0x0(%4)
	lqc2    vf4,0x0(%3)
	lqc2    vf5,0x10(%3)
	lqc2    vf6,0x20(%3)
	lqc2    vf7,0x30(%3)

	lqc2    vf9,0x0(%1)	#minv
	lqc2    vf10,0x0(%2)	#maxv
	lqc2    vf11,0x0(%1)	#minv
	lqc2    vf12,0x0(%2)	#maxv

loop_clip_all:
	vmulax.xyzw	ACC,vf4,vf8
	vmadday.xyzw	ACC,vf5,vf8
	vmaddaz.xyzw	ACC,vf6,vf8
	vmaddw.xyzw	vf8,vf7,vf8

	vmulw.xyz	vf11,vf9,vf8
	vmulw.xyz	vf12,vf10,vf8

	vnop				
	vnop				
	ctc2    $0,$vi16                #���ơ������ե饰�Υ��ꥢ
	vsub.xyw       vf11,vf8,vf11  #(z,ZBz,PPy,PPx)-(Zmin,0,Ymin,Xmin);
	vsub.xyw       vf12,vf12,vf8  #(Zmax,0,Ymax,Xmax) -(z,ZBz,PPy,PPx)
	vmove.w	vf11,vf9
	vmove.w	vf12,vf10
	vnop				
	addi	%4,0x10
	lqc2    vf8,0x0(%4)
	addi	%5,-1
	cfc2    %0,$vi16                #���ơ������ե饰��READ
	andi	%0,$2,0xc0
	beqz	%0,end_clip_all

	bne	$0,%5,loop_clip_all

	addi	%0,$0,1
end_clip_all:

	":"=r"(ret): "r" (minv),"r" (maxv),"r" (ms),"r" (vm),"r" (n)  );
	return ret;
}

//==========================================================================
// 
// sceVu0Reset	VU0,VIF0�Υꥻ�å�
// ����
// 	void sceVu0Reset(void);
// ����
// 	�ʤ�
// ����
// 	vu0�����VIF0���������ޤ���
// �֤���
// 	�ʤ�
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

	*VIF0_MARK=0;
	*VIF0_ERR=0;
	*VIF0_FBRST = 1; /* VIF1 reset */
	__asm__ __volatile__("
	cfc2 $8, $vi28
	ori  $8, $8, 0x0002
	ctc2 $8, $vi28
	sync.p
	":::"$8"); /* VU1 reset */
	*VIF0_FIFO = *(u_long128 *)&(init_vif_regs[0]);
	*VIF0_FIFO = *(u_long128 *)&(init_vif_regs[4]);
}

