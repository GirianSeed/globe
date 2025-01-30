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
// sceVu0ApplyMatrix		�x�N�g���Ƀ}�g���b�N�X����Z����B
// �`��
// 	void sceVu0ApplyMatrix(sceVu0FVECTOR v0, sceVu0FMATRIX m0, sceVu0FVECTOR v1)
// ����
// 	v1		����:�x�N�g��
// 	m0		����:�}�g���b�N�X
// 	v0		�o��:�x�N�g��
// 
// ���
// 	�}�g���b�N�Xm�Ƀx�N�g��v0���E�����Z���āAv1�ɗ^����
// 
// 		v0=m0*v1
// �Ԃ�l
// 	�Ȃ�
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
// sceVu0MulMatrix		�Q�}�g���b�N�X�̐ς����߂�
// �`��
// 	void sceVu0MulMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FMATRIX m2)
// ����
// 	m1,m2		����:�}�g���b�N�X
// 	m0		�o��:�}�g���b�N�X
// ���
// 	�}�g���b�N�Xm1�Ƀ}�g���b�N�Xm2���E�����Z���āAm0�ɗ^����
// 
// 		m0=m1*m2
// �Ԃ�l
// 	�Ȃ�
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
// sceVu0OuterProduct		�Q�̃x�N�g���̊O�ς����߂�
// �`��
// 	void sceVu0OuterProduct(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
// ����
// 	v1,v2		����:�x�N�g��
// 	v0		�o��:�x�N�g��
// ���
// 	�x�N�g��v1,v2�̊O�ς����߂�v0�ɗ^����
// �Ԃ�l
// 	�Ȃ�
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
// sceVu0InnerProduct		�Q�̃x�N�g���̓��ς����߂�
// �`��
// 	float sceVu0InnerProduct(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
// ����
// 	v0,v1		����:�x�N�g��
// ���
// 	�x�N�g��v0,v1�̓��ς����߂�
// �Ԃ�l
// 	����
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
// sceVu0Normalize		�x�N�g���̐��K�����s��
// �`��
// 	void sceVu0Normalize(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
// ����
// 	v1		����:�x�N�g��
// 	v0		�o��:�x�N�g��
// ���
// 	�x�N�g��v0�̐��K�����s��v1�ɗ^����
// �Ԃ�l
// 	�Ȃ�
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
// sceVu0TransposeMatrix	�}�g���b�N�X�̓]�u�s������߂�
// �`��
// 	void sceVu0TransposeMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1)
// ����
// 	m1		����:�}�g���b�N�X
// 	m0		�o��:�}�g���b�N�X
// ���
// 	�}�g���b�N�Xm1�̓]�u�s�������m0�ɗ^����
// �Ԃ�l
// 	�Ȃ�
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
//sceVu0InversMatrix	�t�s�� (4x4) �����߂�B�i�s��͐����Ɖ��肷��j
// �`��
//	void sceVu0InversMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1);
// ����
// 	m1		����:�}�g���b�N�X(��] & ���s�ړ�)
// 	m0		�o��:�}�g���b�N�X
// ���
// 	�}�g���b�N�Xm1�̋t�s������߂āA�}�g���b�N�Xm0�ɗ^����
// �Ԃ�l
// 	�Ȃ�
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
	#�]�u\n\
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
	#����\n\
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
// �`��
//	void sceVu0DivVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float q)
// ����
// 	v1		����:�x�N�g��
// 	q		����:�X�J���[
// 	v0		�o��:�x�N�g��
// ���
// 	�x�N�g��v1���X�J���[q�ŏ��Z���āA�x�N�g��v0�ɗ^����
// �Ԃ�l
// 	�Ȃ�
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
//sceVu0DivVectorXYZ	v0.xyz = v1/q (w ��ۑ����������Z)
// �`��
//	void sceVu0DivVectorXYZ(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float q)
// ����
// 	v1		����:�x�N�g��
// 	q		����:�X�J���[
// 	v0		�o��:�x�N�g��
// ���
// 	�x�N�g��v1��x,y,z�̗v�f���X�J��q�ŏ��Z���āA�x�N�g��v0�ɗ^����
//	�x�N�g��v1��w�v�f�͂��̂܂܃x�N�g��v0�ɕԂ�
// �Ԃ�l
// 	�Ȃ�
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
//sceVu0InterVector	���} 
// �`��
//	void sceVu0InterVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2, float t)
// ����
// 	v1,v2		����:�x�N�g��
//	t		����:���}�l	
// 	v0		�o��:�x�N�g��
// ���
// 	// v0 = v1*t + v2*(1-t) �i���})
// �Ԃ�l
// 	�Ȃ�
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
//sceVu0AddVector	4������Z (ADD/xyzw)
// �`��
//	void sceVu0AddVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
// ����
// 	v1,v2		����:�x�N�g��
// 	v0		�o��:�x�N�g��
// ���
// 	�x�N�g��v1�̊e�v�f�ƃx�N�g��v2�̊e�v�f���e�X���Z����B
// �Ԃ�l
// 	�Ȃ�
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
//sceVu0SubVector		4���񌸎Z (SUB/xyzw)
// �`��
//	void sceVu0SubVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
// ����
// 	v1,v2		����:�x�N�g��
// 	v0		�o��:�x�N�g��
// ���
// 	�x�N�g��v1�̊e�v�f�ƃx�N�g��v2�̊e�v�f���e�X���Z����B
// �Ԃ�l
// 	�Ȃ�
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
//sceVu0MulVector	4����ώZ�iMUL/xyzw: ���ςł͂Ȃ����Ƃɒ��Ӂj
// �`��
//	void sceVu0MulVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
// ����
// 	v1,v2		����:�x�N�g��
// 	v0		�o��:�x�N�g��
// ���
// 	�x�N�g��v1�̊e�v�f�ƃx�N�g��v2�̊e�v�f���e�X��Z����B
// �Ԃ�l
// 	�Ȃ�
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
//sceVu0ScaleVector	�X�J���[�l����Z���� (MULx/xyzw)
// �`��
//	void sceVu0ScaleVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float t)
// ����
// 	v1		����:�x�N�g��
// 	t		����:�X�J���[
// 	v0		�o��:�x�N�g��
// ���
// 	�x�N�g��v1�ɃX�J���[t����Z���ăx�N�g��v0�ɗ^����
// �Ԃ�l
// 	�Ȃ�
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
//sceVu0TransMatrix	�s��̕��s�ړ�
// �`��
//	void sceVu0TransMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FVECTOR tv);
// ����
// 	tv		����:�ړ��x�N�g��
// 	m1		����:�}�g���b�N�X
// 	m0		�o��:�}�g���b�N�X
// ���
// 	�^����ꂽ�x�N�g�����A�}�g���b�N�X�𕽍s�ړ�����
// �Ԃ�l
// 	�Ȃ�
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
//sceVu0CopyVector		�s��̕��� (lq �� sq �̑g�����Balignment check �p��)
// �`��
//	void sceVu0CopyVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1);
// ����
// 	m1		����:�}�g���b�N�X
// 	m0		�o��:�}�g���b�N�X
// ���
// 	�}�g���b�N�Xm1���}�g���b�N�Xm0�ɃR�s�[����B
// �Ԃ�l
// 	�Ȃ�
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
//sceVu0CopyMatrix		�s��̕��� (lq �� sq �̑g�����Balignment check �p��)
// �`��
//	void sceVu0CopyMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1);
// ����
// 	m1		����:�}�g���b�N�X
// 	m0		�o��:�}�g���b�N�X
// ���
// 	�}�g���b�N�Xm1���}�g���b�N�Xm0�ɃR�s�[����B
// �Ԃ�l
// 	�Ȃ�
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
// �`��
//	void sceVu0FTOI4Vector(sceVu0IVECTOR v0, sceVu0FVECTOR v1);
// ����
// 	v1		����:�x�N�g��
// 	v0		�o��:�x�N�g��
// ���
// 	�^����ꂽ�x�N�g���̗v�f�𐮐��ɕϊ�����
// �Ԃ�l
// 	�Ȃ�
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
// �`��
//	void sceVu0FTOI0Vector(sceVu0IVECTOR v0, sceVu0FVECTOR v1);
// ����
// 	v1		����:�x�N�g��
// 	v0		�o��:�x�N�g��
// ���
// 	�^����ꂽ�x�N�g���̗v�f�𐮐��ɕϊ�����
// �Ԃ�l
// 	�Ȃ�
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
// �`��
//	void sceVu0ITOF4Vector(sceVu0FVECTOR v0, sceVu0IVECTOR v1);
// ����
// 	v1		����:�x�N�g��
// 	v0		�o��:�x�N�g��
// ���
// 	�^����ꂽ�x�N�g���̗v�f�𐮐��ɕϊ�����
// �Ԃ�l
// 	�Ȃ�
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
// �`��
//	void sceVu0ITOF0Vector(sceVu0FVECTOR v0, sceVu0IVECTOR v1);
// ����
// 	v1		����:�x�N�g��
// 	v0		�o��:�x�N�g��
// ���
// 	�^����ꂽ�x�N�g���̗v�f�𐮐��ɕϊ�����
// �Ԃ�l
// 	�Ȃ�
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
//sceVu0UnitMatrix	�P�ʍs�� 
// �`��
//	void sceVu0UnitMatrix(sceVu0FMATRIX m0);
// ����
// 	m1		����:�}�g���b�N�X
// 	m0		�o��:�}�g���b�N�X
// ���
// 	�^����ꂽ�s���P�ʍs��ɕϊ�����
// �Ԃ�l
// 	�Ȃ�
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
// �`��
//	void _sceVu0ecossin(float x)
// ����
// ���
// 	sin,cos�����߂�
//	�L���͈�:-��/2 �` ��/2
//	RotMatrix?�ł̂ݎg�p��
// �Ԃ�l
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
    //	���̊֐���static�ł��鎖�𓥂܂��āA�Ăяo�������K��
    //	�����݋֎~����(DI/EI)���s���Ă��鎖��O��Ƃ���
    //	���������s���Ă��܂���
    //
    //	�܂�sceVu0RotMatrix*()���ł́A���̊֐���inline assembler��
    //	�ďo���Ă���̂ŉ��ς���ꍇ�̓��W�X�^�̔j�󓙂ɋC��t���Ă�������
    //	(���[�J���ϐ����쐬����ꍇ/C����}������ꍇ��)

    __asm__ __volatile__("\n\
	la	$8,S5432	# S5-S2 �̌W���� VF05 �֓]��	\n\
	lqc2	vf05,0x0($8)	#				\n\
	vmr32.w vf06,vf06	# VF06.x(v)��VF06.w �ɃR�s�[	\n\
	vaddx.x vf04,vf00,vf06	# VF06.x(v)��VF04.x �ɃR�s�[	\n\
	vmul.x vf06,vf06,vf06	# VF06.x �����悵�� v^2 �ɂ���	\n\
	vmulx.yzw vf04,vf04,vf00# VF04.yzw=0			\n\
	vmulw.xyzw vf08,vf05,vf06# S2-S5 �� VF06.w(v) ��������	\n\
	vsub.xyzw vf05,vf00,vf00 #0,0,0,0 |x,y,z,w		\n\
	vmulx.xyzw vf08,vf08,vf06# VF06.x(v^2) ��������		\n\
	vmulx.xyz vf08,vf08,vf06# VF06.x(v^2) ��������		\n\
	vaddw.x vf04,vf04,vf08	# s+=k2				\n\
	vmulx.xy vf08,vf08,vf06	# VF06.x(v^2) ��������		\n\
	vaddz.x vf04,vf04,vf08	# s+=z				\n\
	vmulx.x vf08,vf08,vf06	# VF06.x(v^2) ��������		\n\
	vaddy.x vf04,vf04,vf08	# s+=y				\n\
	vaddx.x vf04,vf04,vf08	# s+=x (sin ����)		\n\
								\n\
	vaddx.xy vf04,vf05,vf04	# .xy=s �ǉ� 			\n\
								\n\
	vmul.x vf07,vf04,vf04	# VF07.x=s*s			\n\
	vsubx.w vf07,vf00,vf07	# VF07.w = 1-s*s		\n\
								\n\
	vsqrt Q,vf07w		# Q= ��|1-s*s| (cos ����)	\n\
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
//sceVu0RotMatrixZ	�y���𒆐S�Ƃ����s��̉�] 
// �`��
//	void sceVu0RotMatrixZ(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float rz);
// ����
// 	rz		����:��]�p(�L���͈�:-�� �` ��)
// 	m1		����:�}�g���b�N�X
// 	m0		�o��:�}�g���b�N�X
// ���
// 	��]�prt���w���𒆐S�Ƃ�����]�}�g���b�N�X�����߂āA
//	�}�g���b�N�Xm1�ɍ��������Z���āA���̌��ʂ��}�g���b�N�X
//	m0�ɗ^����
// �Ԃ�l
// 	�Ȃ�
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
	add.s   %2,$f0,%2			#rx=rx+��/2\n\
	li 	$7,1				#cos(rx)=sin(rx+��/2)\n\
	j	_RotMatrixZ_02\n\
_RotMatrixZ_01:\n\
	sub.s   %2,$f0,%2			#rx=��/2-rx\n\
	move	$7,$0\n\
_RotMatrixZ_02:\n\
	\n\
        mfc1    $8,%2\n\
        qmtc2    $8,vf6\n\
	move	$6,$31	# ra �ۑ� (�{���̓X�^�b�N���g���ׂ�)\n\
	\n\
	jal	_sceVu0ecossin	# sin(roll), cos(roll)\n\
	move	$31,$6	# ra ��\n\
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
//sceVu0RotMatrixX	�w���𒆐S�Ƃ����s��̉�] 
// �`��
//	void sceVu0RotMatrixX(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float ry);
// ����
// 	rx		����:��]�p(�L���͈�:-�� �` ��)
// 	m1		����:�}�g���b�N�X
// 	m0		�o��:�}�g���b�N�X
// ���
// 	��]�prt���w���𒆐S�Ƃ�����]�}�g���b�N�X�����߂āA
//	�}�g���b�N�Xm1�ɍ��������Z���āA���̌��ʂ��}�g���b�N�X
//	m0�ɗ^����
// �Ԃ�l
// 	�Ȃ�
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
	add.s   %2,$f0,%2			#rx=rx+��/2\n\
	li 	$7,1				#cos(rx)=sin(rx+��/2)\n\
	j	_RotMatrixX_02\n\
_RotMatrixX_01:\n\
	sub.s   %2,$f0,%2			#rx=��/2-rx\n\
	move	$7,$0\n\
_RotMatrixX_02:\n\
	\n\
	mfc1    $8,%2\n\
	qmtc2    $8,$vf6\n\
	move	$6,$31	# ra �ۑ� (�{���̓X�^�b�N���g���ׂ�)\n\
	jal	_sceVu0ecossin	# sin(roll), cos(roll)\n\
	move	$31,$6	# ra ��\n\
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
//sceVu0RotMatrixY	�x���𒆐S�Ƃ����s��̉�] 
// �`��
//	void sceVu0RotMatrixY(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float ry);
// ����
// 	ry		����:��]�p(�L���͈�:-�� �` ��)
// 	m1		����:�}�g���b�N�X
// 	m0		�o��:�}�g���b�N�X
// ���
// 	��]�prt���x���𒆐S�Ƃ�����]�}�g���b�N�X�����߂āA
//	�}�g���b�N�Xm1�ɍ��������Z���āA���̌��ʂ��}�g���b�N�X
//	m0�ɗ^����
// �Ԃ�l
// 	�Ȃ�
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
	add.s   %2,$f0,%2			#rx=rx+��/2\n\
	li 	$7,1				#cos(rx)=sin(rx+��/2)\n\
	j	_RotMatrixY_02\n\
_RotMatrixY_01:\n\
	sub.s   %2,$f0,%2			#rx=��/2-rx\n\
	move	$7,$0\n\
_RotMatrixY_02:\n\
	\n\
	mfc1    $8,%2\n\
	qmtc2    $8,vf6\n\
	move	$6,$31	# ra �ۑ� (�{���̓X�^�b�N���g���ׂ�)\n\
	jal	_sceVu0ecossin	# sin(roll), cos(roll)\n\
	move	$31,$6	# ra ��\n\
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
//sceVu0RotMatrixY	�s��̉�] 
// �`��
//	void sceVu0RotMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FVECTOR rot);
// ����
// 	m0		�o��:�}�g���b�N�X
// 	m1		����:�}�g���b�N�X
// 	rot		����:x,y,z���̉�]�p(�L���͈�:-�� �` ��)
// ���
//	Z���𒆐S�Ƃ����]�}�g���b�N�X��rot[2]����AY���𒆐S�Ƃ����]�}
//	�g���b�N�X��rot[1]����AX���𒆐S�Ƃ����]�}�g���b�N�X��rot[0]����
//	���ꂼ�ꋁ�߁A���Ƀ}�g���b�N�Xm1�ɍ��������Z���āA���̌��ʂ��}�g
//	���b�N�Xm0�ɕԂ��܂��B
// �Ԃ�l
// 	�Ȃ�
// 
//-------------------------------------------------------------------------- 	
void sceVu0RotMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FVECTOR rot)
{
    sceVu0RotMatrixZ(m0, m1, rot[2]);
    sceVu0RotMatrixY(m0, m0, rot[1]);
    sceVu0RotMatrixX(m0, m0, rot[0]);
}

//==========================================================================
//sceVu0ClampVector	�x�N�g���̃N�����v
// �`��
//	void sceVu0ClampVector(sceVu0FVECTOR v0, sceVu0FFVECTOR v1, float min, float max)
// ����
// 	v0		�o��:�x�N�g��
// 	v1		����:�x�N�g��
// 	min		����:�ŏ��l
// 	max		����:�ő�l
// ���
//	�x�N�g��v1�̊e�v�f���A�ŏ��lmin�A�ő�lmax�ŃN�����v���A���ʂ��x�N
//	�g��v0�ɕԂ��܂��B
// �Ԃ�l
// 	�Ȃ�
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
//sceVu0CameraMatrix	���[���h�r���[�s��̐���
// �`��
//	void sceVu0CameraMatrix(sceVu0FMATRIX m, sceVu0FVECTOR p, sceVu0FVECTOR zd, sceVu0FVECTOR yd);
// ����
// 	m		�o��:�}�g���b�N�X(�r���[���[���h���W)
// 	p		����:�}�g���b�N�X(���_)
// 	zd		����:�}�g���b�N�X(����)
// 	yd		����:�}�g���b�N�X(��������)
// ���
//	���_p��(0,0,0)�ɁA����zd��(0,0,1)�ɁA��������yd��(0,1,0)�ɂƕϊ���
//	��悤�ȍs������߁Am�ɕԂ��܂��B
// �Ԃ�l
// 	�Ȃ�
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
//sceVu0NormalLightMatrix         �m�[�}�����C�g�s��̐���
// �`��
//	void sceVu0NormalLightMatrix(sceVu0FMATRIX m, sceVu0FVECTOR l0, sceVu0FVECTOR l1, sceVu0FVECTOR l2);
// ����
// 	m		�o��:�}�g���b�N�X
// 	l0		����:�}�g���b�N�X(����0�̕���)
// 	l1		����:�}�g���b�N�X(����1�̕���)
// 	l2		����:�}�g���b�N�X(����2�̕���)
// ���
//	����l0�A����l1�A����l2����m�[�}�����C�g�s������߁Am�ɕԂ��܂��B
// �Ԃ�l
// 	�Ȃ�
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
//sceVu0LightColorMatrix		���C�g�J���[�s��̐���
// �`��
//	void sceVu0LightColorMatrix(sceVu0FMATRIX m,  sceVu0FVECTOR c0, sceVu0FVECTOR c1, sceVu0FVECTOR c2, sceVu0FVECTOR a)
// ����
// 	m		�o��:�}�g���b�N�X
// 	c0		����:�x�N�g��(�����F0)
// 	c1		����:�x�N�g��(�����F1)
// 	c2		����:�x�N�g��(�����F2)
// 	a		����:�x�N�g��(�A���r�G���g)
// ���
//	�����Fc0,c1,c2�Ɗ��Fa���烉�C�g�J���[�s������߁Am�ɕԂ��܂��B
// �Ԃ�l
// 	�Ȃ�
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
//sceVu0ViewScreenMatrix          �r���[�X�N���[���s��̐���
// �`��
//	void sceVu0ViewScreenMatrix(sceVu0FMATRIX m, float scrz, float ax, float ay, float cx, float cy, float zmin,  float zmax,  float nearz, float farz)
// ����
// 	m		�o��:�}�g���b�N�X
//	scrz            ����:(�X�N���[���܂ł̋���)
//	ax              ����:(�w�����A�X�y�N�g��)
//	ay              ����:(�x�����A�X�y�N�g��)
//	cx              ����:(�X�N���[���̒��S�w���W)
//	cy              ����:(�X�N���[���̒��S�x���W)
//	zmin            ����:(�y�o�b�t�@�ŏ��l)
//	zmax            ����:(�y�o�b�t�@�ő�l)
//	nearz           ����:(�j�A�N���b�v�ʂ̂y)
//	farz            ����:(�t�@�[�N���b�v�ʂ̂y)
// ���
//	�w�肳�ꂽ�e�p�����[�^�ɏ]���ăr���[�X�N���[���s������߁Am�ɕԂ���
//	���B
// �Ԃ�l
// 	�Ȃ�
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
// sceVu0DropShadowMatrix          �h���b�v�V���h�E�ˉe�s��̐���
// �`��
//        void sceVu0DropShadowMatrix(sceVu0FMATRIX m, sceVu0FVECTOR lp, float a, float b, float c, int mode);
// ����
// 	�Ȃ�
//	m               �o��:�}�g���b�N�X
//	lp              ����:�x�N�g��(�����̈ʒu)
//	a               ����:�e�̓��e��
//	b               ����:�e�̓��e��
//	c               ����:�e�̓��e��
//	mode            ����:�����̎��
//			0: ���s����
//			1: �_����
// ���
//	ax+by+cz=1�ŕ\������镽�ʂ�lp��mode�Ŏw�肳����������̉e�𓊉e
//	����s������߁Am�ɕԂ��܂��B
// �Ԃ�l
// 	�Ȃ�
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
//sceVu0RotTransPersN	�����ϊ�
// �`��
//	void sceVu0RotTransPersN(sceVu0IVECTOR *v0, sceVu0FMATRIX m0, sceVu0FVECTOR v1, int n, int mode);
// ����
// 	v0		�o��:�x�N�g��(�X�N���[�����W)
// 	m0		����:�}�g���b�N�X
// 	v1		����:�x�N�g��
// 	n		����:���_��
// 	mode		����:v0[2],v0[3]�̒l�̐؂�ւ�
// 			     0:32bit�����Ȃ��Œ菬���_(28.4)
// 			     1:32bit�����Ȃ��Œ菬���_(32.0)
// ���
// 	�^����ꂽn�̒��_���X�N���[�����W�ɓ����ϊ�����B
//	�o��v0�̒l�́Av0[0],v0[1]�́A32bit�����t���Œ菬���_(28.4)�A
//	v0[2],v0[3]��mode=1�̂Ƃ��́A32bit�����Ȃ��Œ菬���_(28.4)�A
//	mode=0�̂Ƃ��́A32bit�����Ȃ��Œ菬���_(32.0)��Ԃ��B
//	GIF��PACKED���[�h��XYZF2,XYZF3��p����ꍇ�ɂ́Amode=0�ɂ�
//	��ƁAPACK���ɐ��������؂�o�����̂ŗL�p�ł���B
//	
// �Ԃ�l
// 	�Ȃ�
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
//sceVu0RotTransPers	�����ϊ�
// �`��
//	void sceVu0RotTransPers(sceVu0IVECTOR v0, sceVu0FMATRIX m0, sceVu0FVECTOR v1);
// ����
// 	v0		�o��:�x�N�g��(�X�N���[�����W)
// 	m0		����:�}�g���b�N�X
// 	v1		����:�x�N�g��
// 	mode		����:v0[2],v0[3]�̒l�̐؂�ւ�
// 			     0:32bit�����Ȃ��Œ菬���_(28.4)
// 			     1:32bit�����Ȃ��Œ菬���_(32.0)
// ���
// 	�^����ꂽ���_���X�N���[�����W�ɓ����ϊ�����B
//	�o��v0�̒l�́Av0[0],v0[1]�́A32bit�����t���Œ菬���_(28.4)�A
//	v0[2],v0[3]��mode=1�̂Ƃ��́A32bit�����Ȃ��Œ菬���_(28.4)�A
//	mode=0�̂Ƃ��́A32bit�����Ȃ��Œ菬���_(32.0)��Ԃ��B
//	GIF��PACKED���[�h��XYZF2,XYZF3��p����ꍇ�ɂ́Amode=0�ɂ�
//	��ƁAPACK���ɐ��������؂�o�����̂ŗL�p�ł���B
//	
// �Ԃ�l
// 	�Ȃ�
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
// sceVu0CopyVectorXYZ	�x�N�g���̕���
// �`��
//	void sceVu0CopyVectorXYZ(sceVu0FVECTOR v0,sceVu0FVECTOR v1);
// ����
//	v0		�o��:�x�N�g��
//	v1		����:�x�N�g��
// ���
//	�x�N�g��v1��x,y,z�v�f���x�N�g��v0�ɃR�s�[���܂��B
//	�x�N�g��v0��w�v�f�́A���̂܂܃x�N�g��v0�ɕԂ��B 
// �Ԃ�l
// 	�Ȃ�
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
// sceVu0InterVectorXYZ	���}�x�N�g���̐���
// �`��
//	void sceVu0InterVectorXYZ(sceVu0FVECTOR v0,sceVu0FVECTOR v1,sceVu0FVECTOR v2, float t)
// ����
//	v0		�o��:�x�N�g��
//	v1.v2		����:�x�N�g��
//	t		����:���}�p�����[�^
// ���
//	�x�N�g��v1,v2����уp�����[�^t������}�x�N�g�������߁Av0�ɕԂ��܂��B
//	���ŕ\������Ǝ��̂Ƃ���ł��B
//	 v0 = v1*t + v2*(1-t)
//	�x�N�g��v1��w�v�f�́A���̂܂܃x�N�g��v0�ɕԂ��܂�
// �Ԃ�l
// 	�Ȃ�
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
// sceVu0ScaleVectorXYZ	�X�J���[�l�ƃx�N�g���̏�Z (MULx/xyz)
// �`��
//	void sceVu0ScaleVector(sceVu0FVECTOR v0,sceVu0FVECTOR v1,float t)
// ����
// 	�Ȃ�
//	v0		�o��:�x�N�g��
//	v1		����:�x�N�g��
//	t		����:�X�J���[
// ���
//	�x�N�g��v1��x,y,z�v�f�ɃX�J���[t����Z���A���ʂ��x�N�g��v0�ɕԂ���
//	���B�x�N�g��v1��w�v�f�́A���̂܂܃x�N�g��v0�ɕԂ��܂��B
// �Ԃ�l
// 	�Ȃ�
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
// sceVu0ClipScreen	���_�̂f�r�`��͈͂̃N���b�s���O
// �`��
// 	int sceVu0ClipScreen(sceVu0FVECTOR v0)
// ����
// 	v0		����:�x�N�g��
// ���
// 	���_�x�N�g��v0���f�r�`��͈͂ɓ����Ă��邩�𒲂ׂ�B
// �Ԃ�l
// 	0:���_v0���f�r�`��͈͂ɓ����Ă���B
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
	ctc2    $0,$vi16                #�X�e�[�^�X�t���O�̃N���A\n\
	\n\
	vsub.xyw        vf05,vf07,vf04  #(z,ZBz,PPy,PPx)-(0,0,0,0);\n\
	vsub.xy        vf05,vf06,vf07  #(Zmax,0,Ymax,Xmax) -(z,ZBz,PPy,PPx)\n\
	\n\
	vnop			        \n\
	vnop				\n\
	vnop				\n\
	vnop				\n\
	vnop				\n\
        cfc2    %0,$vi16                #�X�e�[�^�X�t���O��READ\n\
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
// sceVu0ClipScreen3	�R���_�̂f�r�`��͈͂̃N���b�s���O
// �`��
// 	int sceVu0ClipScreen3(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
// ����
// 	v0,v1,v2		����:�x�N�g��
// ���
// 	���_�x�N�g��v0,v1,v2�����ׂĂf�r�`��͈͂ɓ����Ă��邩�𒲂ׂ�B
// �Ԃ�l
// 	0:���ׂĂ̒��_���f�r�`��͈͂ɓ����Ă���B
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
	ctc2    $0,$vi16                #�X�e�[�^�X�t���O�̃N���A\n\
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
	cfc2    %0,$vi16                #�X�e�[�^�X�t���O��READ\n\
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
// sceVu0ClipAll           �\���͈͂ɂ��N���b�s���O����
// �`��
// 	int sceVu0ClipAll(sceVu0FVECTOR minv, sceVu0FVECTOR maxv, sceVu0FVECTOR ms,sceVu0FVECTOR *vm,int n)
// ����
//	minv			����:�\���͈͂̍ŏ��l
//	maxv			����:�\���͈͂̍ő�l
//	ms			����:�}�g���b�N�X�i���f���|�X�N���[���j
//	vm			����:���_�x�N�g���̃|�C���^
//	n			����:���_��
// ���
//	vm��n�Ŏw�肳���n�̒��_�����ׂĕ\���͈͂ɓ����Ă��Ȃ����ǂ�����
//	���ׂ܂��B
// �Ԃ�l
//	���ׂĂ̒��_���\���͈͂ɓ����Ă��Ȃ��Ƃ���1��Ԃ��܂��B
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
	ctc2    $0,$vi16                #�X�e�[�^�X�t���O�̃N���A\n\
	vsub.xyw       vf11,vf8,vf11  #(z,ZBz,PPy,PPx)-(Zmin,0,Ymin,Xmin);\n\
	vsub.xyw       vf12,vf12,vf8  #(Zmax,0,Ymax,Xmax) -(z,ZBz,PPy,PPx)\n\
	vmove.w	vf11,vf9\n\
	vmove.w	vf12,vf10\n\
	vnop			\n\
	addi	%4,0x10	\n\
	lqc2    vf8,0x0(%4)\n\
	addi	%5,-1\n\
	cfc2    %0,$vi16                #�X�e�[�^�X�t���O��READ\n\
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
// sceVu0Reset	VU0,VIF0�̃��Z�b�g
// �`��
// 	void sceVu0Reset(void);
// ����
// 	�Ȃ�
// ���
// 	vu0�����VIF0�����������܂��B
// �Ԃ�l
// 	�Ȃ�
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

