/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                    Basic Sample Program
 *                        Version 0.30
 *            S-JIS  
 *
 *      Copyright (C) 1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.10          May,6,1999	shinohara
 */

#include<libvu0.h>

void sceVu0RotTransPersNClipCol(u_long128 *v0,
     sceVu0FMATRIX m0, sceVu0FVECTOR *v1, sceVu0FVECTOR *nv, sceVu0FVECTOR *st,
     sceVu0FVECTOR *c, sceVu0FMATRIX ll, sceVu0FMATRIX lc, int n)
{
	__asm__ volatile (
		"lqc2	vf4,0x0(%1)\n"		// set local_world matrix[0]
		"lqc2	vf5,0x10(%1)\n"		// set local_world matrix[1]
		"lqc2	vf6,0x20(%1)\n"		// set local_world matrix[2]
		"lqc2	vf7,0x30(%1)\n"		// set local_world matrix[3]

		"lqc2	$vf17,0x0(%6)\n"	// set local_light matirix[0
		"lqc2	$vf18,0x10(%6)\n"	// set local_light matirix[1
		"lqc2	$vf19,0x20(%6)\n"	// set local_light matirix[2

		"lqc2	$vf21,0x0(%7)\n"	// set light_color matrix[0]
		"lqc2	$vf22,0x10(%7)\n"	// set light_color matrix[1]
		"lqc2	$vf23,0x20(%7)\n"	// set light_color matrix[2]
		"lqc2	$vf20,0x30(%7)\n"	// set light_color matrix[3]


	"_rotTPNCC_loop:\n"
		"lqc2	vf8,0x0(%2)\n"		// load XYZ
		"lqc2	$vf24,0x0(%4)\n"	// load NORMAL
		"lqc2	$vf25,0x0(%5)\n"	// load COLOR
		"lqc2	$vf27,0x0(%8)\n"	// load ST

		"vmulax.xyzw     ACC, vf4,vf8\n"
		"vmadday.xyzw    ACC, vf5,vf8\n"
		"vmaddaz.xyzw    ACC, vf6,vf8\n"
		"vmaddw.xyzw      vf12,vf7,vf8\n"
		"vdiv    Q,vf0w,vf12w\n"
		"vwaitq\n"
		"vmulq.xyzw	vf12,vf12,Q\n"
		"vftoi4.xyzw	vf13,vf12\n"

		"vmulax.xyzw    ACC, $vf17,$vf24\n"
		"vmadday.xyzw   ACC, $vf18,$vf24\n"
		"vmaddz.xyzw     $vf24,$vf19,$vf24\n"

		"vmaxx.xyz	$vf24,$vf24,$vf0\n"

		"vmulax.xyzw    ACC, $vf21,$vf24\n"
		"vmadday.xyzw   ACC, $vf22,$vf24\n"
		"vmaddaz.xyzw   ACC, $vf23,$vf24\n"
		"vmaddw.xyzw     $vf24,$vf20,$vf0\n"

		"vmul.xyzw	$vf26,$vf24,$vf25\n"

		"vmulq.xyz	$vf28,$vf27,Q\n"


		"vmaxx.xyz	$vf26,$vf26,$vf0\n"
		"lui	$2,0x437f\n"
		"ctc2	$2,$vi21\n"
		"vnop\n"
		"vnop\n"
		"vminii.xyz	$vf26,$vf26,I\n"
		"vftoi0.xyzw	$vf26,$vf26\n"


		"sqc2	$vf28,0x0(%0)\n"	// store STQ 
		"addi	%0,0x10\n"
		"sqc2	$vf26,0x0(%0)\n"	// store RGBA 
		"addi	%0,0x10\n"
		"sqc2	vf13,0x0(%0)\n"		// store XYZ 
		"addi	%0,0x10\n"

		"addi	%3,-1\n"
		"addi	%2,0x10\n"
		"addi	%4,0x10\n"
		"addi	%5,0x10\n"
		"addi	%8,0x10\n"
		"bne	$0,%3,_rotTPNCC_loop\n"

		: : "r"(v0), "r"(m0), "r"(v1), "r"(n), "r"(nv), "r"(c), "r"(ll), "r"(lc), "r"(st)
		: "$2"
	);
}

// ==========================================================================
// sceVu0RotCameraMatrix    
//      p:  original camera position (in the world)
//      zd: original eye direction
//      yd: original vertical direction
//      rot: rotation vector
//      direction : real eye direction
//      vertical  : real veritical direction
//      position  : real camera position
// 
void sceVu0RotCameraMatrix(sceVu0FMATRIX m, sceVu0FVECTOR p, sceVu0FVECTOR zd,
						   sceVu0FVECTOR yd, sceVu0FVECTOR rot)
{
	sceVu0FMATRIX work;
	sceVu0FVECTOR direction, vertical, position;

	sceVu0UnitMatrix(work);
	sceVu0RotMatrixX(work,work,rot[0]);
	sceVu0RotMatrixY(work,work,rot[1]);
	sceVu0RotMatrixZ(work,work,rot[2]);
	sceVu0ApplyMatrix(direction, work, zd);
	sceVu0ApplyMatrix(vertical, work, yd);
	sceVu0ApplyMatrix(position, work, p);
	sceVu0CameraMatrix(m, position, direction, vertical);
}

