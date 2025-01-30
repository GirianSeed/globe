   	.include "utils.i"
	.include "math.i
	.include "fwLights.i"   ; for lightning macros
	.include "fw.i"		; for basic  macros



.macro  myLocalScreen
        sub             _sce_oldDelta,vf00,vf00         ;init: removed by vcl if not used
        sub             _sce_oldPoint,vf00,vf00
        sub             _sce_oDelta,vf00,vf00
        sub             _sce_oVer,vf00,vf00
        fcset 0
 
        lq localWorld[0], 0(VI00)                                  ;0 local_world
        lq localWorld[1], 1(VI00)                                  ;1
        lq localWorld[2], 2(VI00)
        lq localWorld[3], 3(VI00)
 
        lq _sce_worldScreen[0],16(VI00);16world_screen
        lq _sce_worldScreen[1],17(VI00);17
        lq _sce_worldScreen[2],18(VI00);18
        lq _sce_worldScreen[3],19(VI00);19

	lq root_mat[0],68(VI00)
        lq root_mat[1],69(VI00) 
        lq root_mat[2],70(VI00) 
        lq root_mat[3],71(VI00) 

        mMulVecByMat _sce_localWorld[0],localWorld[0],root_mat
        mMulVecByMat _sce_localWorld[1],localWorld[1],root_mat
        mMulVecByMat _sce_localWorld[2],localWorld[2],root_mat
        mMulVecByMat _sce_localWorld[3],localWorld[3],root_mat
 
        mMulVecByMat _sce_localScreen[0],_sce_localWorld[0],_sce_worldScreen
        mMulVecByMat _sce_localScreen[1],_sce_localWorld[1],_sce_worldScreen
        mMulVecByMat _sce_localScreen[2],_sce_localWorld[2],_sce_worldScreen
        mMulVecByMat _sce_localScreen[3],_sce_localWorld[3],_sce_worldScreen
 
.endm             


.macro myCalcNLightVect l1, l2, l3,light_type
        LQ _sce_lightVect[0],\l1(VI00)
        LQ _sce_lightVect[1],\l2(VI00)
        LQ _sce_lightVect[2],\l3(VI00)
        MOVE _sce_lightVect[3],vf00
 
        .AIF "\light_type" EQ "_sce_spotNLightVect"
          ; invert light_vectors
          SUB.xyz _sce_lightVect[0],vf00,_sce_lightVect[0]
          SUB.xyz _sce_lightVect[1],vf00,_sce_lightVect[1]
          SUB.xyz _sce_lightVect[2],vf00,_sce_lightVect[2]
        .AENDI
 
        LQ lightRot[0],4(VI00)
        LQ lightRot[1],5(VI00)
        LQ lightRot[2],6(VI00)
       
        mMulVecByMat _sce_lightRot[0],lightRot[0],root_mat
        mMulVecByMat _sce_lightRot[1],lightRot[1],root_mat
        mMulVecByMat _sce_lightRot[2],lightRot[2],root_mat
        
 
        mMulVecByMat \light_type[0],_sce_lightRot[0],_sce_lightVect
        mMulVecByMat \light_type[1],_sce_lightRot[1],_sce_lightVect
        mMulVecByMat \light_type[2],_sce_lightRot[2],_sce_lightVect
.endm                       


	.init_vf_all
	.init_vi_all
	
	.name vclcloneclip

        --enter								; entry point for vcl
	--endenter   				
	
;**********Micro Init-Block *****************

	myLocalScreen

	myCalcNLightVect \&DIR_LIGHT_VECT0,\&DIR_LIGHT_VECT1,\&DIR_LIGHT_VECT2,_sce_dirNLightVect
	fwInitClip 
;********************************************


	--cont
 	--cont					 


;******************* Buffer Init-Block *********
CLONEC_START:
	XTOP 		_sce_in_p
	ILW.x 		_sce_packsize, 10(VI00)		; inbuf size
	IADD   	 	_sce_pXkick,_sce_in_p,_sce_packsize
	IADDIU 	 	_sce_out_p,_sce_pXkick,0
;***********************************************

;******************** GifTag Init-Block ********
CLONEC_GIFTAG:

	fwInitGifTag
	fwSetVertexCounter
	fwLoadLightColor  \&DIR_LIGHT_COLOR0,\&DIR_LIGHT_COLOR1,\&DIR_LIGHT_COLOR2,_sce_dirLightColor
	fwLoadLightAmbientColor \&AMBIENT_LIGHT_COLOR 

	sub.w	_sce_storeVer,vf00,vf00			; screenVer.w=0		
        fwInitAllGsPrimClip                   ;init. switches for point/line/linestrip/triangle/trianglestrip/fan clipping
;*************************************************************

;******************** Vertex Loop-Block ***********************
CLONEC_vertex_loop:

     	--LoopCS	1,1


	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex
	lq 		_sce_normal,_sce_vert_nXYZ(_sce_in_p)				; load normal 
   	lq.xyz		_sce_ST,_sce_vert_ST(_sce_in_p)       				; load st
	lq 		_sce_vColor,_sce_vert_RGBA(_sce_in_p)				; load color
       
	MOVE.w _sce_alphaC,_sce_vColor
	fwRTP "_sce_prim_XYZF2" "basic"
	fwST "_sce_prim_ST"
	fwClip "POINT_LINE_LINESTRIP_TRIANGLE_TRIANGLESTRIP_FAN"
	iaddiu	_sce_adcFlag,vi01,0x7fff
	fwWriteADC "_sce_prim_XYZF2"
	fwDirLight "SCE_VERTEXC","SCE_AMBIENT",_sce_diffFact
	fwCalcColorSum "SCE_VERTEXC","SCE_CLAMP",_sce_vColor,_sce_diffFact,_sce_vColor  ;use _sce_vColor as output to save alpha
	
	FTOI0.xyz 	     _sce_vColor, _sce_vColor
	FTOI0.w 	     _sce_alphaC,_sce_alphaC
	SQ.xyz _sce_vColor,_sce_prim_RGBAQ(_sce_out_p)
	SQ.w   _sce_alphaC,_sce_prim_RGBAQ(_sce_out_p)

; increment pointers, decrement counter

  	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, CLONEC_vertex_loop   
;**************************************************************************************

CLONEC_done:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,CLONEC_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		CLONEC_START        
	












; -------------------------------------------------------------------

.END











