/* SCE CONFIDENTIAL
   "PlayStation 2" Programmer Tool Runtime Library Release 3.0.2
*/
/*
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: quat.h,v 1.5 2003/04/17 12:18:54 xkazama Exp $	*/

#ifndef _LIB_VU0_H_
#include <libvu0.h>
#endif

static inline void add_quat(sceVu0FVECTOR result,sceVu0FVECTOR a,sceVu0FVECTOR b)
{
    __asm__ __volatile__ ("\n\
                lqc2 vf01, 0(%1)\n\
                lqc2 vf02, 0(%2)\n\
                vadd vf03,vf02,vf01\n\
                sqc2 vf03, 0(%0)\n\
                "
		  :
		  : "r" (&result[0]), "r" (&a[0]), "r" (&b[0])
		  : "memory");
}

static inline void sub_quat(sceVu0FVECTOR result,sceVu0FVECTOR a,sceVu0FVECTOR b)
{
    __asm__ __volatile__ ("\n\
                lqc2 vf01, 0(%1)\n\
                lqc2 vf02, 0(%2)\n\
                vsub vf03,vf01,vf02\n\
                sqc2 vf03, 0(%0)\n\
                "
		  :
		  : "r" (&result[0]), "r" (&a[0]), "r" (&b[0])
		  : "memory");
}

static inline void scale_quat(sceVu0FVECTOR result,sceVu0FVECTOR a,float scale)
{
    __asm__ __volatile__ ("\n\
                lqc2 vf01, 0(%1)\n\
                qmtc2 %2, vf02\n\
                vmulx.xyzw vf03,vf01,vf02x\n\
                sqc2 vf03, 0(%0)\n\
                "
		  :
		  : "r" (&result[0]), "r" (&a[0]), "r" (scale)
		  : "memory");
}

static inline void mul_quat(sceVu0FVECTOR result,sceVu0FVECTOR a,sceVu0FVECTOR b)
{
    __asm__ __volatile__ ("\n\
                lqc2 vf01, 0(%1)\n\
                lqc2 vf02, 0(%2)\n\
		\n\
                vmul vf03,vf01,vf02\n\
                vopmula ACC,vf01,vf02\n\
                vmaddaw.xyz ACC,vf01,vf02\n\
                vmaddaw.xyz ACC,vf02,vf01\n\
                vopmsub.xyz vf01,vf02,vf01\n\
                vsubaz.w ACC,vf03,vf03\n\
                vmsubay.w ACC,vf00,vf03\n\
                vmsubx.w vf01,vf00,vf03\n\
		\n\
                sqc2 vf01, 0(%0)\n\
                "
		  :
		  : "r" (&result[0]), "r" (&a[0]), "r" (&b[0])
		  : "memory");
}
  
static inline void conjugate_quat(sceVu0FVECTOR result,sceVu0FVECTOR a)
{
    __asm__ __volatile__ ("\n\
                lqc2 vf01, 0(%1)\n\
                vsub.xyz vf01,vf00,vf01\n\
                sqc2 vf01, 0(%0)\n\
                "
		  :
		  : "r" (&result[0]), "r" (&a[0])
		  : "memory");
}

static inline void normalize_quat(sceVu0FVECTOR result,sceVu0FVECTOR a)
{
    __asm__ __volatile__ ("\n\
                 lqc2 vf01, 0(%1)\n\
		\n\
                 vmul vf30,vf01,vf01\n\
                 vmulax.w ACC,vf00,vf30\n\
                 vmadday.w ACC,vf00,vf30\n\
                 vmaddaz.w ACC,vf00,vf30\n\
                 vmaddw.w  vf03,vf00,vf30\n\
		\n\
                 vrsqrt Q,vf00w,vf03w\n\
                 vwaitq\n\
                 vmulq  vf03,vf01,Q\n\
		\n\
                 sqc2 vf03,0(%0)\n\
                "
		  :
		  : "r" (&result[0]), "r" (&a[0])
		  : "memory");
}

static inline void quat_to_matrix(sceVu0FMATRIX matrix,sceVu0FVECTOR q)
{
    float d_qxqx=2.0f*q[0]*q[0];
    float d_qyqy=2.0f*q[1]*q[1];
    float d_qzqz=2.0f*q[2]*q[2];
 
    float d_qxqy=2.0f*q[0]*q[1];
    float d_qxqz=2.0f*q[0]*q[2];
 
    float d_qyqz=2.0f*q[1]*q[2];
 
    float d_qwqx=2.0f*q[3]*q[0];
    float d_qwqy=2.0f*q[3]*q[1];
    float d_qwqz=2.0f*q[3]*q[2];
 
    matrix[0][0]=1.0f-d_qyqy-d_qzqz;
    matrix[0][1]=d_qxqy+d_qwqz;
    matrix[0][2]=d_qxqz-d_qwqy;
    matrix[0][3]=0.0f;
 
    matrix[1][0]=d_qxqy-d_qwqz;
    matrix[1][1]=1.0f-d_qxqx-d_qzqz;
    matrix[1][2]=d_qyqz+d_qwqx;
    matrix[1][3]=0.0f;
 
    matrix[2][0]=d_qxqz+d_qwqy;
    matrix[2][1]=d_qyqz-d_qwqx;
    matrix[2][2]=1.0f-d_qxqx-d_qyqy;
    matrix[2][3]=0.0f;
 
    matrix[3][0]=0.0f;
    matrix[3][1]=0.0f;
    matrix[3][2]=0.0f;
    matrix[3][3]=1.0f;
}

static inline float my_sqrtf(float arg)
{
    float result;
    __asm__ __volatile__ ("sqrt.s %0,%1" : "=f" (result) : "f" (arg));
    return result;
}

static inline float my_rsqrtf(float a, float b)
{
    /*	c = a/sqrt(b)	*/
    float c;
    __asm__ __volatile__  ("rsqrt.s %0,%1,%2" : "=f" (c) : "f" (a), "f" (b));
    return c;
}

static inline void matrix_to_quat(sceVu0FVECTOR q, sceVu0FMATRIX m)
{
    int i,j,k;

    float tr = m[0][0]+m[1][1]+m[2][2]+1.0f;
    float qw;

    if(tr>=1.0f){
	q[3] = 0.5f * my_sqrtf(tr);
	qw = my_rsqrtf(0.5f, tr);
	q[0] = (m[1][2]-m[2][1]) * qw;
	q[1] = (m[2][0]-m[0][2]) * qw;
	q[2] = (m[0][1]-m[1][0]) * qw;
    }
    else{
	if (m[0][0]>m[1][1]) i=0; else i=1;
	if (m[2][2]>m[i][i]) i=2;
   
	j=(i+1)%3;
	k=(j+1)%3;

	tr = m[i][i]-m[j][j]-m[k][k]+1.0f;
	q[i] = 0.5f * my_sqrtf(tr);
	qw = my_rsqrtf(0.5f, tr);
	q[j] = (m[j][i]+m[i][j]) * qw;
	q[k] = (m[k][i]+m[i][k]) * qw;
	q[3] = (m[j][k]-m[k][j]) * qw;
    }
}

static inline void vectovec_quat(sceVu0FVECTOR q, sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
    sceVu0FVECTOR c;
    float d,s;

    sceVu0Normalize(v0, v0);
    sceVu0Normalize(v1, v1);
    sceVu0OuterProduct(c, v0, v1);
    d = (1.0f + sceVu0InnerProduct(v0, v1)) * 2.0f;
    q[3] = 0.5f * my_sqrtf(d);
    s = my_rsqrtf(1.0f, d);
    q[0] = c[0] * s;
    q[1] = c[1] * s;
    q[2] = c[2] * s;
}

static inline void axisangle_to_quat(sceVu0FVECTOR q, sceVu0FVECTOR v, float a)
{
    float s,c;

    s = sinf(a*0.5f);
    c = cosf(a*0.5f);

    q[0] = v[0]*s;
    q[1] = v[1]*s;
    q[2] = v[2]*s;
    q[3] = c;
}

static inline float dot_quat(sceVu0FVECTOR a, sceVu0FVECTOR b)
{
    float dot;

    __asm__ __volatile__ ("\n\
                 lqc2 vf02, 0(%1)\n\
                 lqc2 vf03, 0(%2)\n\
		\n\
                 vmul.xyzw vf01, vf02, vf03\n\
                 vaddy.x   vf01, vf01, vf01\n\
                 vaddz.x   vf01, vf01, vf01\n\
                 vaddw.x   vf01, vf01, vf01\n\
		\n\
                 qmfc2 %0, $vf01\n\
                "
		  : "=r" (dot)
		  : "r" (&a[0]), "r" (&b[0])
		  : "memory");

    return dot;
}

static inline float norm_quat(sceVu0FVECTOR a)
{
    float norm;

    __asm__ __volatile__ ("\n\
                 lqc2 vf02, 0(%1)\n\
		\n\
                 vmul.xyzw vf01, vf02, vf02\n\
                 vaddy.x   vf01, vf01, vf01\n\
                 vaddz.x   vf01, vf01, vf01\n\
                 vaddw.x   vf01, vf01, vf01\n\
		\n\
                 qmfc2 %0, $vf01\n\
                "
		  : "=r" (norm)
		  : "r" (&a[0])
		  : "memory");

    return norm;
}
