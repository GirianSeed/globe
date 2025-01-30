/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                    Basic Sample Program
 *                        Version 0.30
 *                            EUC
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.10          Jun,16,1999	shinohara
 */

#include<libvu0.h>

// ==========================================================================
// sceVu0RotTransPersColN
// 座標変換、透視変換、光源計算、テクスチャ座標変換
// 
//      v0:  Vertex
//      m0:  Local-Screen Matrix
//      v1:  Vertex
//      nv:  Normal Vector
//      st:  
//      c :  object color
//      ll:  Local-Light Matrix
//      lc:  Light-Color Matrix
//      n :  number of vertexs
// 

void sceVu0RotTransPersColN(u_long128 *v0, sceVu0FMATRIX m0, sceVu0FVECTOR *v1, sceVu0FVECTOR *nv,
		sceVu0FVECTOR *st,sceVu0FVECTOR *c,sceVu0FMATRIX ll,sceVu0FMATRIX lc, int n)
{
	__asm__ __volatile__("\n\
	lqc2	vf4,0x0(%1)	#set local_world matrix[0]\n\
	lqc2	vf5,0x10(%1)	#set local_world matrix[1]\n\
	lqc2	vf6,0x20(%1)	#set local_world matrix[2]\n\
	lqc2	vf7,0x30(%1)	#set local_world matrix[3]\n\
	\n\
	lqc2	$vf17,0x0(%6)	#set local_light matirix[0]\n\
	lqc2	$vf18,0x10(%6)	#set local_light matirix[1]\n\
	lqc2	$vf19,0x20(%6)	#set local_light matirix[2]\n\
	\n\
	lqc2	$vf21,0x0(%7)	#set light_color matrix[0]\n\
	lqc2	$vf22,0x10(%7)	#set light_color matrix[1]\n\
	lqc2	$vf23,0x20(%7)	#set light_color matrix[2]\n\
	lqc2	$vf20,0x30(%7)	#set light_color matrix[3]\n\
	\n\
	\n\
_rotTPNCC_loop:\n\
	lqc2	vf8,0x0(%2)	#load XYZ\n\
	lqc2	$vf24,0x0(%4)	#load NORMAL\n\
	lqc2	$vf25,0x0(%5)	#load COLOR\n\
	lqc2	$vf27,0x0(%8)	#load ST\n\
	\n\
	vmulax.xyzw     ACC, vf4,vf8\n\
	vmadday.xyzw    ACC, vf5,vf8\n\
	vmaddaz.xyzw    ACC, vf6,vf8\n\
	vmaddw.xyzw      vf12,vf7,vf8\n\
	vdiv    Q,vf0w,vf12w\n\
	vwaitq\n\
	vmulq.xyzw	vf12,vf12,Q\n\
	vftoi4.xyzw	vf13,vf12\n\
	\n\
	vmulax.xyzw    ACC, $vf17,$vf24\n\
	vmadday.xyzw   ACC, $vf18,$vf24\n\
	vmaddz.xyzw     $vf24,$vf19,$vf24\n\
	\n\
	vmaxx.xyz	$vf24,$vf24,$vf0\n\
	\n\
	vmulax.xyzw    ACC, $vf21,$vf24\n\
	vmadday.xyzw   ACC, $vf22,$vf24\n\
	vmaddaz.xyzw   ACC, $vf23,$vf24\n\
	vmaddw.xyzw     $vf24,$vf20,$vf0\n\
	\n\
	vmul.xyzw	$vf26,$vf24,$vf25\n\
	\n\
	vmulq.xyz	$vf28,$vf27,Q\n\
	\n\
	\n\
	vmaxx.xyz	$vf26,$vf26,$vf0	\n\
	lui	$2,0x437f\n\
	ctc2	$2,$vi21\n\
	vnop\n\
	vnop\n\
	vminii.xyz	$vf26,$vf26,I\n\
	vftoi0.xyzw	$vf26,$vf26\n\
	\n\
	\n\
	sqc2	$vf28,0x0(%0)		#store STQ \n\
	addi	%0,0x10\n\
	sqc2	$vf26,0x0(%0)		#store RGBA \n\
	addi	%0,0x10\n\
	sqc2	vf13,0x0(%0)		#store XYZ \n\
	addi	%0,0x10\n\
	\n\
	addi	%3,-1\n\
	addi	%2,0x10\n\
	addi	%4,0x10\n\
	addi	%5,0x10\n\
	addi	%8,0x10\n\
	bne	$0,%3,_rotTPNCC_loop\n\
	\n\
	": : "r" (v0) , "r" (m0) ,"r" (v1), "r" (n) ,"r" (nv) ,"r" (c), "r" (ll) ,"r" (lc),"r"(st) :"$2","$9", "memory");
}

// ==========================================================================
// sceVu0RotTransPersClipGsColN
// 座標変換、透視変換、光源計算、テクスチャ座標変換、GS描画領域クリッピング
// 
//      v0:  Vertex
//      m0:  Local-Screen Matrix
//      v1:  Vertex
//      nv:  Normal Vector
//      st:  
//      c :  object color
//      ll:  Local-Light Matrix
//      lc:  Light-Color Matrix
//      n :  number of vertexs
// 
void sceVu0RotTransPersClipGsColN(u_long128 *v0, sceVu0FMATRIX m0, sceVu0FVECTOR *v1, sceVu0FVECTOR *nv,
		sceVu0FVECTOR *st,sceVu0FVECTOR *c,sceVu0FMATRIX ll,sceVu0FMATRIX lc, int n)
{
	__asm__ __volatile__("\n\
	lqc2	vf4,0x0(%1)	#set local_world matrix[0]\n\
	lqc2	vf5,0x10(%1)	#set local_world matrix[1]\n\
	lqc2	vf6,0x20(%1)	#set local_world matrix[2]\n\
	lqc2	vf7,0x30(%1)	#set local_world matrix[3]\n\
	\n\
	lqc2	$vf17,0x0(%6)	#set local_light matirix[0]\n\
	lqc2	$vf18,0x10(%6)	#set local_light matirix[1]\n\
	lqc2	$vf19,0x20(%6)	#set local_light matirix[2]\n\
	\n\
	lqc2	$vf21,0x0(%7)	#set light_color matrix[0]\n\
	lqc2	$vf22,0x10(%7)	#set light_color matrix[1]\n\
	lqc2	$vf23,0x20(%7)	#set light_color matrix[2]\n\
	lqc2	$vf20,0x30(%7)	#set light_color matrix[3]\n\
	\n\
	# GS CLIP\n\
	li     $2,0x4580000045800000\n\
	qmtc2  $2,vf29\n\
	li     $2,0x8000\n\
	ctc2    $2,$vi01\n\
	li      $9,0x0\n\
	\n\
	\n\
_rotTPNCGC_loop:\n\
	lqc2	vf8,0x0(%2)	#load XYZ\n\
	lqc2	$vf24,0x0(%4)	#load NORMAL\n\
	lqc2	$vf25,0x0(%5)	#load COLOR\n\
	lqc2	$vf27,0x0(%8)	#load ST\n\
	\n\
	vmulax.xyzw     ACC, vf4,vf8\n\
	vmadday.xyzw    ACC, vf5,vf8\n\
	vmaddaz.xyzw    ACC, vf6,vf8\n\
	vmaddw.xyzw      vf12,vf7,vf8\n\
	vdiv    Q,vf0w,vf12w\n\
	vwaitq\n\
	vmulq.xyz	vf12,vf12,Q\n\
	vftoi4.xyzw	vf13,vf12\n\
	\n\
	#GS CLIP\n\
	vnop\n\
	vnop\n\
	ctc2    $0,$vi16                #clear status flag\n\
	vsub.xyw        vf0,vf12,vf00  #(z,ZBz,PPy,PPx)-(1,0,0,0);\n\
	vsub.xy        vf0,vf29,vf12  #(Zmax,0,Ymax,Xmax) -(z,ZBz,PPy,PPx)\n\
	vnop                            \n\
	vnop                            \n\
	vnop				\n\
	vnop				\n\
	sll     $9,1			\n\
	andi    $9,$9,6\n\
	cfc2    $2,$vi16                #read status flag\n\
	andi    $2,$2,0xc0\n\
	beqz    $2,_rotTPNCGC_skip1\n\
	ori     $9,$9,1\n\
	_rotTPNCGC_skip1:\n\
	beqz    $9,_rotTPNCGC_skip2\n\
	vmfir.w $vf13,$vi01\n\
	_rotTPNCGC_skip2:\n\
	\n\
	\n\
	vmulax.xyzw    ACC, $vf17,$vf24\n\
	vmadday.xyzw   ACC, $vf18,$vf24\n\
	vmaddz.xyzw     $vf24,$vf19,$vf24\n\
	\n\
	vmaxx.xyz	$vf24,$vf24,$vf0\n\
	\n\
	vmulax.xyzw    ACC, $vf21,$vf24\n\
	vmadday.xyzw   ACC, $vf22,$vf24\n\
	vmaddaz.xyzw   ACC, $vf23,$vf24\n\
	vmaddw.xyzw     $vf24,$vf20,$vf0\n\
	\n\
	vmul.xyzw	$vf26,$vf24,$vf25\n\
	\n\
	vmulq.xyz	$vf28,$vf27,Q\n\
	\n\
	\n\
	vmaxx.xyz	$vf26,$vf26,$vf0	\n\
	lui	$2,0x437f\n\
	ctc2	$2,$vi21\n\
	vnop\n\
	vnop\n\
	vminii.xyz	$vf26,$vf26,I\n\
	vftoi0.xyzw	$vf26,$vf26\n\
	\n\
	\n\
	sqc2	$vf28,0x0(%0)		#store STQ \n\
	addi	%0,0x10	\n\
	sqc2	$vf26,0x0(%0)		#store RGBA \n\
	addi	%0,0x10\n\
	sqc2	vf13,0x0(%0)		#store XYZ \n\
	addi	%0,0x10\n\
	\n\
	addi	%3,-1\n\
	addi	%2,0x10\n\
	addi	%4,0x10\n\
	addi	%5,0x10\n\
	addi	%8,0x10\n\
	bne	$0,%3,_rotTPNCGC_loop\n\
	\n\
	": : "r" (v0) , "r" (m0) ,"r" (v1), "r" (n) ,"r" (nv) ,"r" (c), "r" (ll) ,"r" (lc),"r"(st) :"$2","$9", "memory");
}


// ==========================================================================
// sceVu0RotCameraMatrix    
// ワールド座標系に対するカメラ座標系を定義(回転有)
// 
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

// ==========================================================================
// sceVu0ViewClipMatrix
// View-Clip MATRIX を生成する
// 
//      vc:  View-Clip Matrix
//      scrw: screen width
//      scrh: screen height
//      scrz: screen depth
//      zmin: near clip surface
//      zmax: far clip surface
// 

void sceVu0ViewClipMatrix(sceVu0FMATRIX vc, float scrw, float scrh, float scrz, float zmin, float zmax)
{
	
        //      | 2*z/w  0    0             0        |
        // vc = |   0  2*z/h  0             0        |
        //      |   0    0  (f+n)/(f-n) -2*f*n/(f-n) |
        //      |   0    0    1             0        |

	sceVu0UnitMatrix(vc);
	vc[0][0]=2*scrz/scrw;
	vc[1][1]=2*scrz/scrh;
	vc[2][2]=(zmax+zmin)/(zmax-zmin);
	vc[3][2]=-2*zmax*zmin/(zmax-zmin);
	vc[2][3]=1.0f;
	vc[3][3]=0.0f;
}

// ==========================================================================
// sceVu0ClipBBox
// クリップ座標系で与えられたバウンダリボックスをクリップ
// 
//      lc:  Local-Clip Matrix
//      v: BoundaryBox Vertexs
// 

int sceVu0ClipBBox(sceVu0FMATRIX lc, sceVu0FVECTOR *v)
{
        register int ret;

        __asm__  __volatile__("\n\
	\n\
        lqc2    vf4,0x0(%1)\n\
        lqc2    vf5,0x10(%1)\n\
        lqc2    vf6,0x20(%1)\n\
        lqc2    vf7,0x30(%1)\n\
        li      %0,0x3f\n\
        li      $8,8\n\
loop_clip_all:\n\
        lqc2    vf8,0x0(%2)\n\
	addi	%2,0x10\n\
	\n\
        vmulax.xyzw     ACC,vf4,vf8\n\
        vmadday.xyzw    ACC,vf5,vf8\n\
        vmaddaz.xyzw    ACC,vf6,vf8\n\
        vmaddw.xyzw     vf9,vf7,vf8\n\
	\n\
	vclipw.xyz vf9xyz,vf9w\n\
	vnop\n\
	vnop\n\
	vnop\n\
	vnop\n\
	vnop\n\
	cfc2    $3,$vi18                #READ CLIP flag\n\
	and	%0,%0,$3\n\
	\n\
        beqz    %0,end_clip_all\n\
        addi    $8,$8,-1\n\
	\n\
        bne     $0,$8,loop_clip_all\n\
        addi    %0,$0,1\n\
end_clip_all:\n\
	\n\
        ":"=&r"(ret): "r" (lc),"r" (v)  : "$2","$3","$8");

	return ret;
}

