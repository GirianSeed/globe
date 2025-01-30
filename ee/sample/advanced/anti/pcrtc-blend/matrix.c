/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *      Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*
*
*	VU & GS test data generator
*
*/
#include <stdlib.h>
#include <stdio.h>
#include <eekernel.h>
#include <eeregs.h>
#include <math.h>
#include <libgraph.h>
#include <libdma.h>
#include <libpkt.h>
#include <libvu0.h>
#include <libdev.h>
#include <sifdev.h>
#include <eestruct.h>

#include "base.h"

#define ERR_STOP while(1)

void view2swmtx(float view[3],float target[3],float swmx[4][4]);
void set_llmtx(float lv[4], float wlm[4][4], float llm[4][4]);
void psmtx2plmtx(float psm[4][4], float swm[4][4], float pwm[4][4], float wlm[4][4], float plm[4][4]);

inline void Mul_Matrix_Matrix(sceVu0FMATRIX a,sceVu0FMATRIX b, sceVu0FMATRIX m);
inline void Mul_Matrix_Vector(sceVu0FMATRIX m,sceVu0FVECTOR a, sceVu0FVECTOR b);
inline void Transpose_Matrix(sceVu0FMATRIX s, sceVu0FMATRIX d);
inline void Vector_Normal(sceVu0FVECTOR v0);


void set_llmtx(float lv[4], float wlm[4][4], float llm[4][4])
{
       	float  lwm[4][4]__attribute__((aligned(16)));  /* Inverse wlmtx (Rotation) */
	float  ldir[4]	__attribute__((aligned(16)));
	/* Local World Rotation Matrix for Light */
	Transpose_Matrix(wlm, lwm);   // Transpose wlm->lwm
	lwm[0][3]=0.0f;	lwm[1][3]=0.0f;	lwm[2][3]=0.0f;	lwm[3][3]= 1.0f; 
	/* Set Local Light */
	Mul_Matrix_Vector(lwm,lv,ldir);
	llm[0][0]=ldir[0];
	llm[1][0]=ldir[1];
	llm[2][0]=ldir[2];
	
}

void psmtx2plmtx(float psm[4][4], float swm[4][4], float pwm[4][4], float wlm[4][4], float plm[4][4])
{
	/*Prim/World matrix*/
	 Mul_Matrix_Matrix(psm,swm,pwm);
        /*Prim/Local Matrix*/
         Mul_Matrix_Matrix(pwm,wlm,plm);
}

		
void view2swmtx(float view[3],float target[3],float swmx[4][4])
{
	float	v[4] __attribute__((aligned(16)));
	float	bn;

	if(target[0]==view[0]&&target[1]==view[1]&&target[2]==view[2]){
        	swmx[0][0]= 1.0f;
        	swmx[1][0]= 0.0f;
        	swmx[2][0]= 0.0f;
        	swmx[3][0]= 0.0f;

        	swmx[0][1]= 0.0f;
        	swmx[1][1]= 1.0f;
        	swmx[2][1]= 0.0f;
        	swmx[3][1]= 0.0f;

        	swmx[0][2]= 0.0f;
        	swmx[1][2]= 0.0f;
        	swmx[2][2]= 1.0f;
        	swmx[3][2]= 1.0f;

        	swmx[0][3]= 0.0f;
        	swmx[1][3]= 0.0f;
        	swmx[2][3]= 0.0f;
        	swmx[3][3]= 1.0f;
		ERR_STOP;
	}

	v[0]= target[0]-view[0];	
	v[1]= target[1]-view[1];	
	v[2]= target[2]-view[2];

	Vector_Normal(v);
	bn= sqrtf(v[0]*v[0]+v[2]*v[2]);

	if(bn==0){
		//kprintf("Singular point of view2mtx: ON Y-axis\n");
       		swmx[0][0]= 1.0f;
        	swmx[1][0]= 0.0f;
        	swmx[2][0]= 0.0f;
        	swmx[3][0]= 0.0f;

        	swmx[0][1]= 0.0f;
        	swmx[1][1]= 1.0f;
        	swmx[2][1]= 0.0f;
        	swmx[3][1]= 0.0f;

        	swmx[0][2]= 0.0f;
        	swmx[1][2]= 0.0f;
        	swmx[2][2]= 1.0f;
        	swmx[3][2]= sqrtf((target[1]-view[1])*(target[1]-view[1]));

        	swmx[0][3]= 0.0f;
        	swmx[1][3]= 0.0f;
        	swmx[2][3]= 0.0f;
        	swmx[3][3]= 1.0f;
		ERR_STOP;
	}

        swmx[0][0]= v[2]/bn;
        swmx[1][0]= 0.0f;
        swmx[2][0]= -v[0]/bn;
        swmx[3][0]= 0.0f;

        swmx[0][1]= -v[1]*v[0]/bn;
        swmx[1][1]= (v[2]*v[2]+v[0]*v[0])/bn;
        swmx[2][1]= -v[1]*v[2]/bn;
        swmx[3][1]= 0.0f;

        swmx[0][2]= v[0];
        swmx[1][2]= v[1];
        swmx[2][2]= v[2];
        swmx[3][2]= sqrtf((target[0]-view[0])*(target[0]-view[0])
                          +(target[1]-view[1])*(target[1]-view[1])
                          +(target[2]-view[2])*(target[2]-view[2]));

        swmx[0][3]= 0.0f;
        swmx[1][3]= 0.0f;
        swmx[2][3]= 0.0f;
        swmx[3][3]= 1.0f;
}

inline void Mul_Matrix_Matrix(sceVu0FMATRIX a,sceVu0FMATRIX b, sceVu0FMATRIX m)
{
	__asm__ __volatile__(
	"lqc2    vf4,0x0(%0)\n"		//load ( a[0][0],a[0][1],a[0][2],a[0][3])
	"lqc2    vf5,0x10(%0)\n"		//load ( a[1][0],a[1][1],a[1][2],a[1][3])
	"lqc2    vf6,0x20(%0)\n"		//load ( a[2][0],a[2][1],a[2][2],a[2][3])
	"lqc2    vf7,0x30(%0)\n"		//load ( a[3][0],a[3][1],a[3][2],a[3][3])
	"lqc2    vf8,0x0(%1)\n"		//load ( b[i][0],b[i][1],b[i][2],b[i][3])  i=0;
	"vmulax.xyzw	ACC,   vf4,vf8x\n"	//  a[0][0]*b[i][0]  a[0][1]*b[i][0]  a[0][2]*b[i][0]  a[0][2]*b[i][0]
	"vmadday.xyzw	ACC,   vf5,vf8y\n"	// +a[1][0]*b[i][1] +a[1][1]*b[i][1] +a[1][2]*b[i][1] +a[1][2]*b[i][1]
	"vmaddaz.xyzw	ACC,   vf6,vf8z\n"	// +a[2][0]*b[i][2] +a[2][1]*b[i][2] +a[2][2]*b[i][2] +a[2][2]*b[i][2]
	"vmaddw.xyzw	vf12,vf7,vf8w\n"	// +a[3][0]*b[i][3] +a[3][1]*b[i][3] +a[3][2]*b[i][3] +a[3][2]*b[i][3]
	"sqc2    vf12,0x0(%2)\n"		//save ( m[i][0],m[i][1],m[i][2],m[i][3])
	"lqc2    vf8,0x10(%1)\n"
	"vmulax.xyzw	ACC,   vf4,vf8\n"
	"vmadday.xyzw	ACC,   vf5,vf8\n"
	"vmaddaz.xyzw	ACC,   vf6,vf8\n"
	"vmaddw.xyzw	vf12,vf7,vf8\n"
	"sqc2    vf12,0x10(%2)\n"
	"lqc2    vf8,0x20(%1)\n"
	"vmulax.xyzw	ACC,   vf4,vf8\n"
	"vmadday.xyzw	ACC,   vf5,vf8\n"
	"vmaddaz.xyzw	ACC,   vf6,vf8\n"
	"vmaddw.xyzw	vf12,vf7,vf8\n"
	"sqc2    vf12,0x20(%2)\n"
	"lqc2    vf8,0x30(%1)\n"
	"vmulax.xyzw	ACC,   vf4,vf8\n"
	"vmadday.xyzw	ACC,   vf5,vf8\n"
	"vmaddaz.xyzw	ACC,   vf6,vf8\n"
	"vmaddw.xyzw	vf12,vf7,vf8\n"
	"sqc2    vf12,0x30(%2)\n"
	: : "r" (a) , "r" (b) ,"r" (m) : "memory");

/*****************************************************************************************
 int i;

	for(i=0;i<4;i++){
		m[i][0]= a[0][0]*b[i][0]+a[1][0]*b[i][1]+a[2][0]*b[i][2]+a[3][0]*b[i][3];
		m[i][1]= a[0][1]*b[i][0]+a[1][1]*b[i][1]+a[2][1]*b[i][2]+a[3][1]*b[i][3];
		m[i][2]= a[0][2]*b[i][0]+a[1][2]*b[i][1]+a[2][2]*b[i][2]+a[3][2]*b[i][3];
		m[i][3]= a[0][3]*b[i][0]+a[1][3]*b[i][1]+a[2][3]*b[i][2]+a[3][3]*b[i][3];
	}

 in Vu0:
	for(i=0;i<4;i++){
	   a[0][0]*b[i][0]  a[0][1]*b[i][0]  a[0][2]*b[i][0]  a[0][3]*b[i][0];
	  +a[1][0]*b[i][1] +a[1][1]*b[i][1] +a[1][2]*b[i][1] +a[1][3]*b[i][1];
	  +a[2][0]*b[i][2] +a[2][1]*b[i][2] +a[2][2]*b[i][2] +a[2][3]*b[i][2];
	  +a[3][0]*b[i][3] +a[3][1]*b[i][3] +a[3][2]*b[i][3] +a[3][3]*b[i][3];
	 X:m[i][0]	  Y:m[i][1]	   Z:m[i][2]        W:m[i][3]
	}
*******************************************************************************************/
}

inline void Mul_Matrix_Vector(sceVu0FMATRIX m,sceVu0FVECTOR a, sceVu0FVECTOR b)
{
	__asm__ __volatile__(
	"lqc2    vf4,0x0(%0)\n"
	"lqc2    vf5,0x10(%0)\n"
	"lqc2    vf6,0x20(%0)\n"
	"lqc2    vf7,0x30(%0)\n"
	"lqc2    vf8,0x0(%1)\n"
	"vmulax.xyzw	ACC,   vf4,vf8\n"
	"vmadday.xyzw	ACC,   vf5,vf8\n"
	"vmaddaz.xyzw	ACC,   vf6,vf8\n"
	"vmaddw.xyzw	vf12,vf7,vf8\n"
	"sqc2    vf12,0x0(%2)\n"
	: : "r" (m) , "r" (a) ,"r" (b) : "memory");

/*************************************************************************
{
	b[0]= m[0][0]*a[0] + m[1][0]*a[1] + m[2][0]*a[2] + m[3][0]*a[3];
	b[1]= m[0][1]*a[0] + m[1][1]*a[1] + m[2][1]*a[2] + m[3][1]*a[3];
	b[2]= m[0][2]*a[0] + m[1][2]*a[1] + m[2][2]*a[2] + m[3][2]*a[3];
	b[3]= m[0][3]*a[0] + m[1][3]*a[1] + m[2][3]*a[2] + m[3][3]*a[3];
}
in Vu0:
	   m[0][0]*a[0]	   m[0][1]*a[1]	   m[0][2]*a[2]	   m[0][3]*a[3];
	  +m[1][0]*a[0]	  +m[1][1]*a[1]	  +m[1][2]*a[2]	  +m[1][3]*a[3];
	  +m[2][0]*a[0]	  +m[2][1]*a[1]	  +m[2][2]*a[2]	  +m[2][3]*a[3];
	  +m[3][0]*a[0]	  +m[3][1]*a[1]	  +m[3][2]*a[2]	  +m[3][3]*a[3];
	 X:b[0]		 Y:b[1]		 Z:b[2]	 	 W:b[3] 
**************************************************************************/
}

inline void Transpose_Matrix(sceVu0FMATRIX s, sceVu0FMATRIX d)
{
	__asm__ __volatile__(
	"lq $8,0x0000(%0)\n"
	"lq $9,0x0010(%0)\n"
	"lq $10,0x0020(%0)\n"
	"lq $11,0x0030(%0)\n"

	"pextlw     $12,$9,$8\n"
	"pextuw     $13,$9,$8\n"
	"pextlw     $14,$11,$10\n"
	"pextuw     $15,$11,$10\n"

	"pcpyld     $8,$14,$12\n"
	"pcpyud     $9,$12,$14\n"
	"pcpyld     $10,$15,$13\n"
	"pcpyud     $11,$13,$15\n"

	"sq $8,0x0000(%1)\n"
	"sq $9,0x0010(%1)\n"
	"sq $10,0x0020(%1)\n"
	"sq $11,0x0030(%1)\n"
	: : "r" (s) , "r" (d) : "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "memory");
}


inline void Vector_Normal(sceVu0FVECTOR v0)
{
	__asm__ __volatile__(
	"lqc2    vf4,0x0(%0)\n"
	"vmul.xyz vf5,vf4,vf4\n"
	"vaddy.x vf5,vf5,vf5\n"
	"vaddz.x vf5,vf5,vf5\n"

	"vrsqrt Q,vf0w,vf5x\n"
	"vsub.xyzw vf7,vf0,vf0\n"		//vf7.xyzw=0;
	"vaddw.xyzw vf7,vf7,vf4\n"		//vf7.w=vf4.w;
	"vwaitq\n"

	"vmulq.xyz  vf7,vf4,Q\n"
	"sqc2    vf7,0x0(%0)\n"
	: : "r" (v0) : "memory");
/*****************************************************************
float abs;
        abs= sqrtf(vct[0]*vct[0]+vct[1]*vct[1]+vct[2]*vct[2]);
        vct[0] /=abs;
        vct[1] /=abs;
        vct[2] /=abs;
******************************************************************/
}
