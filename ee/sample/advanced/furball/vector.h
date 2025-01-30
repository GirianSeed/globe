/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
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
  __asm__ volatile (
                "lq $2, 0(%1)\n"
                "sq $2, 0(%0)\n"
                :
                : "r" (&result[0]), "r" (&vector[0])
                :  "$2", "memory");
}

inline
static
void
CpuSubVector(sceVu0FVECTOR result, sceVu0FVECTOR vector0, sceVu0FVECTOR vector1)
{
  __asm__ volatile (
                "lwc1 $f1, 0(%1)\n"
                "lwc1 $f4, 0(%2)\n"
                "lwc1 $f2, 4(%1)\n"
                "lwc1 $f5, 4(%2)\n"
                "lwc1 $f3, 8(%1)\n"
                "lwc1 $f6, 8(%2)\n"
                "sub.s $f1, $f1, $f4\n"
                "sub.s $f2, $f2, $f5\n"
                "sub.s $f3, $f3, $f6\n"
                "swc1 $f1, 0(%0)\n"
                "swc1 $f2, 4(%0)\n"
                "swc1 $f3, 8(%0)\n"
                :
                : "r" (&result[0]), "r" (&vector0[0]), "r" (&vector1[0])
                :  "$f1", "$f2", "$f3", "$f4", "$f5", "$f6", "memory"
                );
}

inline
static
void
CpuAddVector(sceVu0FVECTOR result, sceVu0FVECTOR vector0, sceVu0FVECTOR vector1)
{
  __asm__ volatile (
                "lwc1 $f1, 0(%1)\n"
                "lwc1 $f4, 0(%2)\n"
                "lwc1 $f2, 4(%1)\n"
                "lwc1 $f5, 4(%2)\n"
                "lwc1 $f3, 8(%1)\n"
                "lwc1 $f6, 8(%2)\n"
                "add.s $f1, $f1, $f4\n"
                "add.s $f2, $f2, $f5\n"
                "add.s $f3, $f3, $f6\n"
                "swc1 $f1, 0(%0)\n"
                "swc1 $f2, 4(%0)\n"
                "swc1 $f3, 8(%0)\n"
                :
                : "r" (&result[0]), "r" (&vector0[0]), "r" (&vector1[0])
                :  "$f1", "$f2", "$f3", "$f4", "$f5", "$f6", "memory"
                );
}

inline
static
void
CpuScaleVector(sceVu0FVECTOR result, sceVu0FVECTOR vector, float scale)
{
  __asm__ volatile (
                "lwc1 $f1, 0(%1)\n"
                "lwc1 $f2, 4(%1)\n"
                "lwc1 $f3, 8(%1)\n"
                "mul.s $f1, $f1, %2\n"
                "mul.s $f2, $f2, %2\n"
                "mul.s $f3, $f3, %2\n"
                "swc1 $f1, 0(%0)\n"
                "swc1 $f2, 4(%0)\n"
                "swc1 $f3, 8(%0)\n"
                :
                : "r" (result), "r" (vector), "f" (scale)
                :  "$f1", "$f2", "$f3", "memory"
                );
}

inline
static
void
CpuCrossProduct(sceVu0FVECTOR result, sceVu0FVECTOR vector0, sceVu0FVECTOR vector1)
{
  __asm__ volatile (
                "lwc1 $f2, 4(%1)\n"
                "lwc1 $f6, 8(%2)\n"
                "lwc1 $f3, 8(%1)\n"
                "lwc1 $f5, 4(%2)\n"
                "lwc1 $f1, 0(%1)\n"
                "lwc1 $f4, 0(%2)\n"
                "mula.s $f2, $f6\n"
                "msub.s $f7, $f3, $f5\n"
                "mula.s $f3, $f4\n"
                "msub.s $f8, $f1, $f6\n"
                "mula.s $f1, $f5\n"
                "msub.s $f9, $f2, $f4\n"
                "swc1 $f7, 0(%0)\n"
                "swc1 $f8, 4(%0)\n"
                "swc1 $f9, 8(%0)\n"
                :
                : "r" (result), "r" (vector0), "r" (vector1)
                :  "$f1", "$f2", "$f3", "$f4", "$f5", "$f6", "$f7", "$f8", "$f9", "memory"
                );
}

inline
static
float
CpuDotProduct(sceVu0FVECTOR vector0, sceVu0FVECTOR vector1)
{
  float dot;
  __asm__ volatile (
                "lwc1 $f1, 0(%1)\n"
                "lwc1 $f2, 4(%1)\n"
                "lwc1 $f3, 8(%1)\n"
                "lwc1 $f4, 0(%2)\n"
                "lwc1 $f5, 4(%2)\n"
                "lwc1 $f6, 8(%2)\n"
                "mula.s $f1, $f4\n"
                "madda.s $f2, $f5\n"
                "madd.s %0, $f3, $f6\n"
                : "=f" (dot)
                : "r" (&vector0[0]), "r" (&vector1[0])
                :  "$f1", "$f2", "$f3", "$f4", "$f5", "$f6");
  return dot;
}

inline
static
void
CpuTriNormal(sceVu0FVECTOR normal, sceVu0FVECTOR vertex0, sceVu0FVECTOR vertex1, sceVu0FVECTOR vertex2)
{
  __asm__ volatile (
                "lwc1 $f1, 0(%1)\n"
                "lwc1 $f2, 4(%1)\n"
                "lwc1 $f3, 8(%1)\n"
                "lwc1 $f4, 0(%2)\n"
                "lwc1 $f5, 4(%2)\n"
                "lwc1 $f6, 8(%2)\n"
                "lwc1 $f7, 0(%3)\n"
                "lwc1 $f8, 4(%3)\n"
                "lwc1 $f9, 8(%3)\n"
                "sub.s $f4, $f4, $f1\n"
                "sub.s $f5, $f5, $f2\n"
                "sub.s $f6, $f6, $f3\n"
                "sub.s $f7, $f7, $f1\n"
                "sub.s $f8, $f8, $f2\n"
                "sub.s $f9, $f9, $f3\n"
                "mula.s $f5, $f9\n"
                "msub.s $f1, $f6, $f8\n"
                "mula.s $f6, $f7\n"
                "msub.s $f2, $f4, $f9\n"
                "mula.s $f4, $f8\n"
                "msub.s $f3, $f5, $f7\n"
                "swc1 $f1, 0(%0)\n"
                "swc1 $f2, 4(%0)\n"
                "swc1 $f3, 8(%0)\n"
                :
                : "r" (&normal[0]), "r" (&vertex0[0]), "r" (&vertex1[0]), "r" (&vertex2[0])
                :  "$f1", "$f2", "$f3", "$f4", "$f5", "$f6", "$f7", "$f8", "$f9", "memory");
}

//------------------------------------------------------------------------------------------------------------------
//  VU0 macrocode routines
//------------------------------------------------------------------------------------------------------------------
inline
static
void
Vu0NegVector(sceVu0FVECTOR result, sceVu0FVECTOR vector)
{
  __asm__ volatile (
   "lqc2     vf4,0x0(%0)\n"
   "vsub.xyz vf5,vf0,vf4\n"
   "sqc2     vf5,0x0(%1)   \n"
   : : "r" (vector), "r" (result));		
}


inline
static
void
Vu0CopyVector(sceVu0FVECTOR result, sceVu0FVECTOR vector)
{
  __asm__ volatile (
                "lqc2 vf01, 0x00(%1)\n"
                "sqc2 vf01, 0x00(%0)\n"
                :
                : "r" (result), "r" (vector)
                :  "memory");
}

inline
static
void
Vu0CopyMatrix(sceVu0FMATRIX result, sceVu0FMATRIX matrix)
{
  __asm__ volatile (
                "lqc2 vf01, 0x00(%1)\n"
                "lqc2 vf02, 0x10(%1)\n"
                "lqc2 vf03, 0x20(%1)\n"
                "lqc2 vf04, 0x30(%1)\n"
                "sqc2 vf01, 0x00(%0)\n"
                "sqc2 vf02, 0x10(%0)\n"
                "sqc2 vf03, 0x20(%0)\n"
                "sqc2 vf04, 0x30(%0)\n"
                :
                : "r" (&result[0][0]), "r" (&matrix[0][0])
                :  "memory");
}

inline
static
void
Vu0SubVector(sceVu0FVECTOR result, sceVu0FVECTOR vector0, sceVu0FVECTOR vector1)
{
  __asm__ volatile (
                "lqc2 vf01, 0(%1)\n"
                "lqc2 vf02, 0(%2)\n"
                "vsub.xyz vf01, vf01, vf02\n"
                "sqc2 vf01, 0(%0)\n"
                :
                : "r" (&result[0]), "r" (&vector0[0]), "r" (&vector1[0])
                :  "memory");
}

inline
static
void
Vu0AddVector(sceVu0FVECTOR result, sceVu0FVECTOR vector0, sceVu0FVECTOR vector1)
{
  __asm__ volatile (
                "lqc2 vf01, 0(%1)\n"
                "lqc2 vf02, 0(%2)\n"
                "vadd.xyz vf01, vf01, vf02\n"
                "sqc2 vf01, 0(%0)\n"
                :
                : "r" (&result[0]), "r" (&vector0[0]), "r" (&vector1[0])
                :  "memory");
}

inline
static
void
Vu0ScaleVector(sceVu0FVECTOR result, sceVu0FVECTOR vector, float scale)
{
  __asm__ volatile (
                "lqc2 vf01, 0(%1)\n"
                "qmtc2 %2, vf02\n"
                "vmulx.xyz vf01, vf01, vf02x\n"
                "sqc2 vf01, 0(%0)\n"
                :
                : "r" (&result[0]), "r" (&vector[0]), "r" (scale)
                :  "memory");
}

inline
static
void
Vu0TransformVector(sceVu0FVECTOR result, sceVu0FMATRIX matrix, sceVu0FVECTOR vector)
{
  // assumes w element of vector is 1.0

  __asm__ volatile (
                "lqc2 vf05, 0x00(%2)\n"
                "lqc2 vf01, 0x00(%1)\n"
                "lqc2 vf02, 0x10(%1)\n"
                "lqc2 vf03, 0x20(%1)\n"
                "lqc2 vf04, 0x30(%1)\n"
                "vmulax ACC, vf01, vf05x\n"
                "vmadday ACC, vf02, vf05y\n"
                "vmaddaz ACC, vf03, vf05z\n"
                "vmaddw.xyz vf05, vf04, vf00w\n"
                "sqc2 vf05, 0(%0)\n"
                :
                : "r" (&result[0]), "r" (&matrix[0][0]), "r" (&vector[0])
                :  "memory");
}

inline
static
void
Vu0RotateVector(sceVu0FVECTOR result, sceVu0FMATRIX matrix, sceVu0FVECTOR vector)
{
  // same as Vu0TransformVector, but doesn't add the translation

  __asm__ volatile (
                "lqc2 vf04, 0x00(%2)\n"
                "lqc2 vf01, 0x00(%1)\n"
                "lqc2 vf02, 0x10(%1)\n"
                "lqc2 vf03, 0x20(%1)\n"
                "vmulax ACC, vf01, vf04x\n"
                "vmadday ACC, vf02, vf04y\n"
                "vmaddz.xyz vf04, vf03, vf04z\n"
                "sqc2 vf04, 0x00(%0)\n"
                :
                : "r" (&result[0]), "r" (&matrix[0][0]), "r" (&vector[0])
                :  "memory");
}

inline
static
void
Vu0CrossProduct(sceVu0FVECTOR result, sceVu0FVECTOR vector0, sceVu0FVECTOR vector1)
{
  __asm__ volatile (
                "lqc2 vf01, 0x00(%1)\n"
                "lqc2 vf02, 0x00(%2)\n"
                "vopmula ACC, vf01, vf02\n"
                "vopmsub vf01, vf02, vf01\n"
                "sqc2 vf01, 0x00(%0)\n"
                :
                : "r" (&result[0]), "r" (&vector0[0]), "r" (&vector1[0])
                :  "memory");
}

inline
static
float
Vu0DotProduct(sceVu0FVECTOR vector0, sceVu0FVECTOR vector1)
{
  float dot;
  __asm__ volatile (
                "lqc2 vf01, 0x00(%1)\n"
                "lqc2 vf02, 0x00(%2)\n"
                "vmul.xyz vf01, vf01, vf02\n"
                "vaddw.x vf02x, vf00x, vf00w\n"
                "vadday.x ACCx, vf01x, vf01y\n"
                "vmaddz.x vf01x, vf02x, vf01z\n"
                "qmfc2 %0, $vf01\n"
                : "=r" (dot)
                : "r" (&vector0[0]), "r" (&vector1[0])
                );
  return dot;
}

inline
static
void
Vu0NormalizeVector(sceVu0FVECTOR result, sceVu0FVECTOR vector)
{
  __asm__ volatile (
                ".set noreorder\n"
                "lqc2 vf01, 0(%1)\n"
                "vmul.xyz vf02, vf01, vf01\n"
                "vaddw.x vf03x, vf00x, vf00w\n"
                "vadday.x ACCx, vf02x, vf02y\n"
                "vmaddz.x vf02x, vf03x, vf02z\n"
                "vrsqrt Q, vf00w, vf02x\n"
                "vwaitq\n"
                "vmulq.xyz vf01, vf01, Q\n"
                "sqc2 vf01, 0(%0)\n"
                ".set reorder\n"
                :
                : "r" (&result[0]), "r" (&vector[0])
                );
}

inline
static
void
Vu0InvertMatrix(sceVu0FMATRIX result, sceVu0FMATRIX matrix)
{
  // Transposes the coordinate frame axes; negates the translation and converts it into the
  // coordinates of the transposed axes. This inverts a transform as long as the 3x3 is
  // orthonormal.

  __asm__ volatile (
                "lqc2 vf01, 0x00(%1)\n"                  // Load matrix
                "lqc2 vf02, 0x10(%1)\n"
                "lqc2 vf03, 0x20(%1)\n"
                "lqc2 vf04, 0x30(%1)\n"
                "vaddx.y vf05y, vf00y, vf02x\n"
                "vmove.zw vf07zw, vf03zw\n"               // Compute Rinv:
                "vaddx.z vf05z, vf00z, vf03x\n"
                "vmr32.x vf06x, vf01\n"
                "vaddy.z vf06z, vf00z, vf03y\n"
                "vmove.xw vf05xw, vf01xw\n"
                "vaddz.x vf07x, vf00x, vf01z\n"
                "vmove.yw vf06yw, vf02yw\n"
                "vnop\n"
                "vmr32.y vf07y, vf02\n"
                "vadda ACC, vf00, vf00\n"
                "vmove.w vf08w, vf04w\n"                  // Compute Rinv * -T
                "vmsubax ACC, vf05, vf04x\n"
                "vmsubay ACC, vf06, vf04y\n"
                "vmsubz.xyz vf08, vf07, vf04z\n"
                "sqc2 vf05, 0x00(%0)\n"
                "sqc2 vf06, 0x10(%0)\n"
                "sqc2 vf07, 0x20(%0)\n"
                "sqc2 vf08, 0x30(%0)\n"
                :
                : "r" (&result[0][0]), "r" (&matrix[0][0])
                :  "memory");
}

inline
static
void
Vu0InvertRotation(sceVu0FMATRIX result, sceVu0FMATRIX matrix)
{
  // Same as Vu0InvertMatrix, put doesn't transform or store a translation

  __asm__ volatile (
                "lqc2 vf01, 0x00(%1)\n"
                "lqc2 vf02, 0x10(%1)\n"
                "lqc2 vf03, 0x20(%1)\n"
                "vaddx.y vf05y, vf00y, vf02x\n"
                "vaddx.z vf05z, vf00z, vf03x\n"
                "vaddy.x vf06x, vf00x, vf01y\n"
                "vaddy.z vf06z, vf00z, vf03y\n"
                "vaddz.x vf03x, vf00x, vf01z\n"
                "vaddz.y vf03y, vf00y, vf02z\n"
                "vadd.yz vf01yz, vf00yz, vf05yz\n"
                "vadd.xz vf02xz, vf00xz, vf06xz\n"
                "sqc2 vf01, 0x00(%0)\n"
                "sqc2 vf02, 0x10(%0)\n"
                "sqc2 vf03, 0x20(%0)\n"
                :
                : "r" (&result[0][0]), "r" (&matrix[0][0])
                :  "memory");
}

inline
static
void
Vu0TriNormal(sceVu0FVECTOR normal, sceVu0FVECTOR vertex0, sceVu0FVECTOR vertex1, sceVu0FVECTOR vertex2)
{
  __asm__ volatile (
                "lqc2 vf01, 0(%1)\n"
                "lqc2 vf02, 0(%2)\n"
                "lqc2 vf03, 0(%3)\n"
                "vsub vf02, vf02, vf01\n"
                "vsub vf03, vf03, vf01\n"
                "vopmula ACC, vf02, vf03\n"
                "vopmsub vf01, vf03, vf02\n"
                "sqc2 vf01, 0(%0)\n"
                :
                : "r" (&normal[0]), "r" (&vertex0[0]), "r" (&vertex1[0]), "r" (&vertex2[0])
                :  "memory");
}

inline
static
int Vu0CompareVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
	int ret;

 __asm__ __volatile__(
	"lqc2    vf4,0x0(%1)\n"
	"lqc2    vf5,0x0(%2)\n"
      "vsub.xyz vf1,vf4,vf5\n"
      "vnop\n"
      "vnop\n"
      "vnop\n"
      "vnop \n"
      "cfc2 %0,$vi17\n"
      "andi %0,%0,0xe\n"
	: "=&r" (ret) :"r" (v0) ,"r" (v1) :"$2" );
 	if (ret==0xe) { return 1;} else  return 0;
}

#endif
