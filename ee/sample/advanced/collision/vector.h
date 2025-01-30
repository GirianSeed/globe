/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/*
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          Name : vector.h 
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            0.10    May 22, 2000   Eric Larsen  Initial
 *            0.11    Nov 28, 2001     eder      
 */                                              
                       
#ifndef VECTORMATH_H
#define VECTORMATH_H

#include <stdio.h>
#include <libvu0.h>

//------------------------------------------------------------------------------------------------------------------
//  CPU+FPU routines
//------------------------------------------------------------------------------------------------------------------

inline
static
void
CpuCopyVector(sceVu0FVECTOR result, sceVu0FVECTOR vector)
{
  asm volatile ("
                lq $2, 0(%1);
                sq $2, 0(%0);
                "
                :
                : "r" (&result[0]), "r" (&vector[0])
                : "cc", "$2", "memory");
}

inline
static
void
CpuSubVector(sceVu0FVECTOR result, sceVu0FVECTOR vector0, sceVu0FVECTOR vector1)
{
  asm volatile ("
                lwc1 $f1, 0(%1);
                lwc1 $f4, 0(%2);
                lwc1 $f2, 4(%1);
                lwc1 $f5, 4(%2);
                lwc1 $f3, 8(%1);
                lwc1 $f6, 8(%2);
                sub.s $f1, $f1, $f4;
                sub.s $f2, $f2, $f5;
                sub.s $f3, $f3, $f6;
                swc1 $f1, 0(%0);
                swc1 $f2, 4(%0);
                swc1 $f3, 8(%0);
                "
                :
                : "r" (&result[0]), "r" (&vector0[0]), "r" (&vector1[0])
                : "cc", "$f1", "$f2", "$f3", "$f4", "$f5", "$f6", "memory"
                );
}

inline
static
void
CpuAddVector(sceVu0FVECTOR result, sceVu0FVECTOR vector0, sceVu0FVECTOR vector1)
{
  asm volatile ("
                lwc1 $f1, 0(%1);
                lwc1 $f4, 0(%2);
                lwc1 $f2, 4(%1);
                lwc1 $f5, 4(%2);
                lwc1 $f3, 8(%1);
                lwc1 $f6, 8(%2);
                add.s $f1, $f1, $f4;
                add.s $f2, $f2, $f5;
                add.s $f3, $f3, $f6;
                swc1 $f1, 0(%0);
                swc1 $f2, 4(%0);
                swc1 $f3, 8(%0);
                "
                :
                : "r" (&result[0]), "r" (&vector0[0]), "r" (&vector1[0])
                : "cc", "$f1", "$f2", "$f3", "$f4", "$f5", "$f6", "memory"
                );
}

inline
static
void
CpuScaleVector(sceVu0FVECTOR result, sceVu0FVECTOR vector, float scale)
{
  asm volatile ("
                lwc1 $f1, 0(%1);
                lwc1 $f2, 4(%1);
                lwc1 $f3, 8(%1);
                mul.s $f1, $f1, %2;
                mul.s $f2, $f2, %2;
                mul.s $f3, $f3, %2;
                swc1 $f1, 0(%0);
                swc1 $f2, 4(%0);
                swc1 $f3, 8(%0);
                "
                :
                : "r" (result), "r" (vector), "f" (scale)
                : "cc", "$f1", "$f2", "$f3", "memory"
                );
}

inline
static
void
CpuCrossProduct(sceVu0FVECTOR result, sceVu0FVECTOR vector0, sceVu0FVECTOR vector1)
{
  asm volatile ("
                lwc1 $f2, 4(%1);
                lwc1 $f6, 8(%2);
                lwc1 $f3, 8(%1);
                lwc1 $f5, 4(%2);
                lwc1 $f1, 0(%1);
                lwc1 $f4, 0(%2);
                mula.s $f2, $f6;
                msub.s $f7, $f3, $f5;
                mula.s $f3, $f4;
                msub.s $f8, $f1, $f6;
                mula.s $f1, $f5;
                msub.s $f9, $f2, $f4;
                swc1 $f7, 0(%0);
                swc1 $f8, 4(%0);
                swc1 $f9, 8(%0);
                "
                :
                : "r" (result), "r" (vector0), "r" (vector1)
                : "cc", "$f1", "$f2", "$f3", "$f4", "$f5", "$f6", "$f7", "$f8", "$f9", "memory"
                );
}

inline
static
float
CpuDotProduct(sceVu0FVECTOR vector0, sceVu0FVECTOR vector1)
{
  float dot;
  asm volatile ("
                lwc1 $f1, 0(%1);
                lwc1 $f2, 4(%1);
                lwc1 $f3, 8(%1);
                lwc1 $f4, 0(%2);
                lwc1 $f5, 4(%2);
                lwc1 $f6, 8(%2);
                mula.s $f1, $f4;
                madda.s $f2, $f5;
                madd.s %0, $f3, $f6;
                "
                : "=f" (dot)
                : "r" (&vector0[0]), "r" (&vector1[0])
                : "cc", "$f1", "$f2", "$f3", "$f4", "$f5", "$f6");
  return dot;
}

inline
static
void
CpuTriNormal(sceVu0FVECTOR normal, sceVu0FVECTOR vertex0, sceVu0FVECTOR vertex1, sceVu0FVECTOR vertex2)
{
  asm volatile ("
                lwc1 $f1, 0(%1);
                lwc1 $f2, 4(%1);
                lwc1 $f3, 8(%1);
                lwc1 $f4, 0(%2);
                lwc1 $f5, 4(%2);
                lwc1 $f6, 8(%2);
                lwc1 $f7, 0(%3);
                lwc1 $f8, 4(%3);
                lwc1 $f9, 8(%3);
                sub.s $f4, $f4, $f1;
                sub.s $f5, $f5, $f2;
                sub.s $f6, $f6, $f3;
                sub.s $f7, $f7, $f1;
                sub.s $f8, $f8, $f2;
                sub.s $f9, $f9, $f3;
                mula.s $f5, $f9;
                msub.s $f1, $f6, $f8
                mula.s $f6, $f7;
                msub.s $f2, $f4, $f9;
                mula.s $f4, $f8;
                msub.s $f3, $f5, $f7;
                swc1 $f1, 0(%0);
                swc1 $f2, 4(%0);
                swc1 $f3, 8(%0);
                "
                :
                : "r" (&normal[0]), "r" (&vertex0[0]), "r" (&vertex1[0]), "r" (&vertex2[0])
                : "cc", "$f1", "$f2", "$f3", "$f4", "$f5", "$f6", "$f7", "$f8", "$f9", "memory");
}

//------------------------------------------------------------------------------------------------------------------
//  VU0 macrocode routines
//------------------------------------------------------------------------------------------------------------------
inline
static
void
Vu0NegVector(sceVu0FVECTOR result, sceVu0FVECTOR vector)
{
  asm __volatile__("
   lqc2     vf4,0x0(%0)
   vsub.xyz vf5,vf0,vf4
   sqc2     vf5,0x0(%1)   
   ": : "r" (vector), "r" (result));		
}


inline
static
void
Vu0CopyVector(sceVu0FVECTOR result, sceVu0FVECTOR vector)
{
  asm volatile ("
                lqc2 vf01, 0x00(%1);
                sqc2 vf01, 0x00(%0);
                "
                :
                : "r" (result), "r" (vector)
                : "cc", "memory");
}

inline
static
void
Vu0CopyMatrix(sceVu0FMATRIX result, sceVu0FMATRIX matrix)
{
  asm volatile ("
                lqc2 vf01, 0x00(%1);
                lqc2 vf02, 0x10(%1);
                lqc2 vf03, 0x20(%1);
                lqc2 vf04, 0x30(%1);
                sqc2 vf01, 0x00(%0);
                sqc2 vf02, 0x10(%0);
                sqc2 vf03, 0x20(%0);
                sqc2 vf04, 0x30(%0);
                "
                :
                : "r" (&result[0][0]), "r" (&matrix[0][0])
                : "cc", "memory");
}

inline
static
void
Vu0SubVector(sceVu0FVECTOR result, sceVu0FVECTOR vector0, sceVu0FVECTOR vector1)
{
  asm volatile ("
                lqc2 vf01, 0(%1);
                lqc2 vf02, 0(%2);
                vsub.xyz vf01, vf01, vf02;
                sqc2 vf01, 0(%0);
                "
                :
                : "r" (&result[0]), "r" (&vector0[0]), "r" (&vector1[0])
                : "cc", "memory");
}

inline
static
void
Vu0AddVector(sceVu0FVECTOR result, sceVu0FVECTOR vector0, sceVu0FVECTOR vector1)
{
  asm volatile ("
                lqc2 vf01, 0(%1);
                lqc2 vf02, 0(%2);
                vadd.xyz vf01, vf01, vf02;
                sqc2 vf01, 0(%0);
                "
                :
                : "r" (&result[0]), "r" (&vector0[0]), "r" (&vector1[0])
                : "cc", "memory");
}

inline
static
void
Vu0ScaleVector(sceVu0FVECTOR result, sceVu0FVECTOR vector, float scale)
{
  asm volatile ("
                lqc2 vf01, 0(%1);
                qmtc2 %2, vf02;
                vmulx.xyz vf01, vf01, vf02x;
                sqc2 vf01, 0(%0);
                "
                :
                : "r" (&result[0]), "r" (&vector[0]), "r" (scale)
                : "cc", "memory");
}

inline
static
void
Vu0TransformVector(sceVu0FVECTOR result, sceVu0FMATRIX matrix, sceVu0FVECTOR vector)
{
  // assumes w element of vector is 1.0

  asm volatile ("
                lqc2 vf05, 0x00(%2);
                lqc2 vf01, 0x00(%1);
                lqc2 vf02, 0x10(%1);
                lqc2 vf03, 0x20(%1);
                lqc2 vf04, 0x30(%1);
                vmulax ACC, vf01, vf05x;
                vmadday ACC, vf02, vf05y;
                vmaddaz ACC, vf03, vf05z;
                vmaddw.xyz vf05, vf04, vf00w;
                sqc2 vf05, 0(%0);
                "
                :
                : "r" (&result[0]), "r" (&matrix[0][0]), "r" (&vector[0])
                : "cc", "memory");
}

inline
static
void
Vu0RotateVector(sceVu0FVECTOR result, sceVu0FMATRIX matrix, sceVu0FVECTOR vector)
{
  // same as Vu0TransformVector, but doesn't add the translation

  asm volatile ("
                lqc2 vf04, 0x00(%2);
                lqc2 vf01, 0x00(%1);
                lqc2 vf02, 0x10(%1);
                lqc2 vf03, 0x20(%1);
                vmulax ACC, vf01, vf04x;
                vmadday ACC, vf02, vf04y;
                vmaddz.xyz vf04, vf03, vf04z;
                sqc2 vf04, 0x00(%0);
                "
                :
                : "r" (&result[0]), "r" (&matrix[0][0]), "r" (&vector[0])
                : "cc", "memory");
}

inline
static
void
Vu0CrossProduct(sceVu0FVECTOR result, sceVu0FVECTOR vector0, sceVu0FVECTOR vector1)
{
  asm volatile ("
                lqc2 vf01, 0x00(%1);
                lqc2 vf02, 0x00(%2);
                vopmula ACC, vf01, vf02;
                vopmsub vf01, vf02, vf01;
                sqc2 vf01, 0x00(%0);
                "
                :
                : "r" (&result[0]), "r" (&vector0[0]), "r" (&vector1[0])
                : "cc", "memory");
}

inline
static
float
Vu0DotProduct(sceVu0FVECTOR vector0, sceVu0FVECTOR vector1)
{
  float dot;
  asm volatile ("
                lqc2 vf01, 0x00(%1);
                lqc2 vf02, 0x00(%2);
                vmul.xyz vf01, vf01, vf02;
                vaddw.x vf02x, vf00x, vf00w;
                vadday.x ACCx, vf01x, vf01y;
                vmaddz.x vf01x, vf02x, vf01z;
                qmfc2 %0, $vf01;
                "
                : "=r" (dot)
                : "r" (&vector0[0]), "r" (&vector1[0])
                : "cc");
  return dot;
}

inline
static
void
Vu0NormalizeVector(sceVu0FVECTOR result, sceVu0FVECTOR vector)
{
  asm volatile ("
                .set noreorder
                lqc2 vf01, 0(%1);
                vmul.xyz vf02, vf01, vf01;
                vaddw.x vf03x, vf00x, vf00w;
                vadday.x ACCx, vf02x, vf02y;
                vmaddz.x vf02x, vf03x, vf02z;
                vrsqrt Q, vf00w, vf02x;
                vwaitq;
                vmulq.xyz vf01, vf01, Q;
                sqc2 vf01, 0(%0);
                .set reorder
                "
                :
                : "r" (&result[0]), "r" (&vector[0])
                : "cc");
}

inline
static
void
Vu0InvertMatrix(sceVu0FMATRIX result, sceVu0FMATRIX matrix)
{
  // Transposes the coordinate frame axes; negates the translation and converts it into the
  // coordinates of the transposed axes. This inverts a transform as long as the 3x3 is
  // orthonormal.

  asm volatile ("
                lqc2 vf01, 0x00(%1);                  # Load matrix
                lqc2 vf02, 0x10(%1);
                lqc2 vf03, 0x20(%1);
                lqc2 vf04, 0x30(%1);
                vaddx.y vf05y, vf00y, vf02x
                vmove.zw vf07zw, vf03zw               # Compute Rinv:
                vaddx.z vf05z, vf00z, vf03x
                vmr32.x vf06x, vf01
                vaddy.z vf06z, vf00z, vf03y
                vmove.xw vf05xw, vf01xw
                vaddz.x vf07x, vf00x, vf01z
                vmove.yw vf06yw, vf02yw
                vnop
                vmr32.y vf07y, vf02
                vadda ACC, vf00, vf00
                vmove.w vf08w, vf04w                  # Compute Rinv * -T
                vmsubax ACC, vf05, vf04x
                vmsubay ACC, vf06, vf04y
                vmsubz.xyz vf08, vf07, vf04z
                sqc2 vf05, 0x00(%0);
                sqc2 vf06, 0x10(%0);
                sqc2 vf07, 0x20(%0);
                sqc2 vf08, 0x30(%0);
                "
                :
                : "r" (&result[0][0]), "r" (&matrix[0][0])
                : "cc", "memory");
}

inline
static
void
Vu0InvertRotation(sceVu0FMATRIX result, sceVu0FMATRIX matrix)
{
  // Same as Vu0InvertMatrix, put doesn't transform or store a translation

  asm volatile ("
                lqc2 vf01, 0x00(%1);
                lqc2 vf02, 0x10(%1);
                lqc2 vf03, 0x20(%1);
                vaddx.y vf05y, vf00y, vf02x;
                vaddx.z vf05z, vf00z, vf03x;
                vaddy.x vf06x, vf00x, vf01y;
                vaddy.z vf06z, vf00z, vf03y;
                vaddz.x vf03x, vf00x, vf01z;
                vaddz.y vf03y, vf00y, vf02z;
                vadd.yz vf01yz, vf00yz, vf05yz;
                vadd.xz vf02xz, vf00xz, vf06xz;
                sqc2 vf01, 0x00(%0);
                sqc2 vf02, 0x10(%0);
                sqc2 vf03, 0x20(%0);
                "
                :
                : "r" (&result[0][0]), "r" (&matrix[0][0])
                : "cc", "memory");
}

inline
static
void
Vu0TriNormal(sceVu0FVECTOR normal, sceVu0FVECTOR vertex0, sceVu0FVECTOR vertex1, sceVu0FVECTOR vertex2)
{
  asm volatile ("
                lqc2 vf01, 0(%1);
                lqc2 vf02, 0(%2);
                lqc2 vf03, 0(%3);
                vsub vf02, vf02, vf01;
                vsub vf03, vf03, vf01;
                vopmula ACC, vf02, vf03;
                vopmsub vf01, vf03, vf02;
                sqc2 vf01, 0(%0);
                "
                :
                : "r" (&normal[0]), "r" (&vertex0[0]), "r" (&vertex1[0]), "r" (&vertex2[0])
                : "cc", "memory");
}

inline
static
int Vu0CompareVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
	int ret;

	asm __volatile__("
	lqc2    vf4,0x0(%1)
	lqc2    vf5,0x0(%2)
      vsub.xyz vf1,vf4,vf5
      vnop
      vnop
      vnop
      vnop 
      cfc2 %0,$vi17
      andi %0,%0,0xe
	": "=&r" (ret) :"r" (v0) ,"r" (v1) :"$2" );
 	if (ret==0xe) { return 1;} else  return 0;
}

#endif
