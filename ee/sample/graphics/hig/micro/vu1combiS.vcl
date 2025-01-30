
   	.include "utils.i"
	.include "math.i"
	.include "fwLights.i"
	.include "fwFog.i"
	.include "fwAnti.i"
	.include "fw.i"		; includes for framework
	
	.init_vf_all
	.init_vi_all
	
	.name vclcombiS

        --enter								; entry point for vcl
	--endenter   	
				
;Pre-Matrix-Calculations
	fwLocalScreen
	fwLoadLocalWorldRT
	mInvertRT _sce_worldLocal,_sce_localWorldRT
	fwGetScale _sce_scale,_sce_worldLocal,_sce_localWorld
	fwLocalCamera
	SQ.xyz _sce_localCamera,104(VI00)

	fwCalcNLightVect \&DIR_LIGHT_VECT0,\&DIR_LIGHT_VECT1,\&DIR_LIGHT_VECT2,_sce_dirNLightVect
	fwStoreNLightVect 101,102,103,_sce_dirNLightVect
	
	
	fwCalcLocalLightPos \&POINT_LIGHT_POS0,\&POINT_LIGHT_POS1,\&POINT_LIGHT_POS2,_sce_pointLocalLightPos,_sce_pointLightPos
	SQ.xyz _sce_pointLocalLightPos[0],117(VI00)
	SQ.xyz _sce_pointLocalLightPos[1],118(VI00)
	SQ.xyz _sce_pointLocalLightPos[2],119(VI00)
	fwLoadLightColor \&POINT_LIGHT_COLOR0,\&POINT_LIGHT_COLOR1,\&POINT_LIGHT_COLOR2,_sce_pointLightColor
	fwStoreLightConst _sce_pointLightConst,_sce_pointLightColor,113
	fwInitClip
	--cont
 	--cont					 



ALL_START:
	XTOP 		_sce_in_p
	ILW.x 		_sce_packsize, 10(VI00)		; inbuf size
	IADD   	 	_sce_pXkick,_sce_in_p,_sce_packsize
	IADDIU 	 	_sce_out_p,_sce_pXkick,0
	loi 		255.0
ALL_GIFTAG:

	fwInitGifTag
	fwSetVertexCounter
	fwInitTriangleTriangleStrip
	fwInitAnti
 

	iaddi		_sce_bfcSwitch,vi00,0	
	iaddiu		_sce_bfcMask,vi00,0x00 
	ibeq 		_sce_triangleMask,_sce_prim,ALL_vertex_loop
	iaddiu		_sce_bfcMask,vi00,0x20
	ibeq		_sce_triangleStripMask,_sce_prim,ALL_vertex_loop


ALL_vertex_loop:
	
    	--LoopCS	3,3

	fwInitFog

	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex
	lq 		_sce_normal,_sce_vert_nXYZ(_sce_in_p)				; load normal 
   	lq.xyz		_sce_ST,_sce_vert_ST(_sce_in_p)       				; load st

	
	fwLoadLightColor  \&DIR_LIGHT_COLOR0,\&DIR_LIGHT_COLOR1,\&DIR_LIGHT_COLOR2,_sce_dirLightColor
	fwLoadLightColor  \&POINT_LIGHT_COLOR0,\&POINT_LIGHT_COLOR1,\&POINT_LIGHT_COLOR2,_sce_pointLightColor
	fwLoadLightAmbientColor \&AMBIENT_LIGHT_COLOR

;************************* RTP *******************************
 	fwRTP "_sce_aprim_XYZF2"
;*************************************************************


;************************** ST *******************************
	fwST  "_sce_aprim_ST"
;*************************************************************

;************************** Clip *****************************
	fwClip "TRIANGLE_TRIANGLESTRIP"
;*************************************************************

	
;********************** ANTI +BFC ****************************
	LQ.w _sce_antiParam,28(VI00)
	LQ.xyz _sce_antiParam,104(VI00)
	fwAntiBFC "_sce_aprim_PRMOD"  
;*************************************************************

;********** calc _sce_eyeVect with global varibles ************
	MULx.xyz _sce_eyeVect,_sce_cameraDist,_sce_oneOverCdist
;**************************************************************

;*********************** FOG **	*******************************
	fwFog
	fwWriteADC_Fog "_sce_aprim_XYZF2"
;*************************************************************

	mScaleVectorXYZ _sce_vrt,_sce_vrt,_sce_scale
 	LQ.xyz _sce_localCamera,104(VI00)

;****************** DIR LIGHT ***************************
	fwLoadNLightVect 101,102,103,_sce_dirNLightVect
	fwDirLight "SCE_PHONG","SCE_NOAMBIENT",_sce_diffpaColor,_sce_specpaColor,"nocalc"
;*************************************************************

	LQ.xyz _sce_localCamera,104(VI00)

;******************** POINT LIGHT ****************************
 	LQ.xyz _sce_pointLocalLightPos[0],117(VI00)
	LQ.xyz _sce_pointLocalLightPos[1],118(VI00)
	LQ.xyz _sce_pointLocalLightPos[2],119(VI00)

	fwLoadLightConst 113, _sce_pointLightConst
	fwPointLight "SCE_PHONG","SCE_NOAMBIENT",_sce_diffpoColor,_sce_specpoColor,"nocalc"

;***********************ADD Colors ***************************
	ADD.xyz _sce_diffvColor,_sce_diffpaColor,_sce_diffpoColor
	ADD.xyz _sce_specvColor,_sce_specpaColor,_sce_specpoColor
	fwLoadMaterialColor _sce_dColor,_sce_sColor,_sce_eColor,_sce_aColor,\&MATERIAL_DIFFUSE,\&MATERIAL_SPECULAR,\&MATERIAL_EMISSION,\&MATERIAL_AMBIENT
	fwCalcColorSum  "SCE_PHONG","SCE_CLAMP",_sce_dColor,_sce_diffvColor,_sce_dColor,_sce_aColor,_sce_specvColor,_sce_sColor,_sce_eColor
	fwWriteColor "_sce_aprim_RGBAQ",  _sce_dColor
;*************************************************************

; increment pointers, decrement counter

  	iaddiu		_sce_out_p,_sce_out_p,_sce_aprim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, ALL_vertex_loop   
	
ALL_done:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,ALL_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		ALL_START        


; -------------------------------------------------------------------
.END
