   	.include "utils.i"
	.include "math.i"
	.include "fwLights.i"   ; for lightning macros
	.include "fw.i"		; for basic  macros


	.init_vf_all
	.init_vi_all
	
	.name vclspot

        --enter								; entry point for vcl
	--endenter   					


;**********Micro Init-Block *****************
	fwLocalScreen
	fwLoadLocalWorldRT
	mInvertRT _sce_worldLocal,_sce_localWorldRT
	fwGetScale _sce_scale,_sce_worldLocal,_sce_localWorld
	fwCalcNLightVect \&SPOT_LIGHT_VECT0,\&SPOT_LIGHT_VECT1,\&SPOT_LIGHT_VECT2,_sce_spotNLightVect
	fwCalcLocalLightPos \&SPOT_LIGHT_POS0,\&SPOT_LIGHT_POS1,\&SPOT_LIGHT_POS2,_sce_spotLocalLightPos,_sce_spotLightPos
	fwStoreLightAngle _sce_spotLightPos,100
	fwLoadLightColor \&SPOT_LIGHT_COLOR0,\&SPOT_LIGHT_COLOR1,\&SPOT_LIGHT_COLOR2,_sce_spotLightColor
	fwStoreLightConst _sce_spotLightConst,_sce_spotLightColor,101
	fwInitClip
;********************************************

	--cont
 	--cont					 


;******************* Buffer Init-Block *********
SPOT_START:
	XTOP 		_sce_in_p
	ILW.x 		_sce_packsize, 10(VI00)		; inbuf size
	IADD   	 	_sce_pXkick,_sce_in_p,_sce_packsize
	IADDIU 	 	_sce_out_p,_sce_pXkick,0
;***********************************************

;******************** GifTag Init-Block ********
SPOT_GIFTAG:
	fwInitGifTag
	fwSetVertexCounter
	fwInitTriangleTriangleStrip
	fwStoreFanSwitch 118

	iaddi		_sce_bfcSwitch,vi00,0	
	iaddiu		_sce_bfcMask,vi00,0x00
	ibeq 		_sce_triangleMask,_sce_prim,SPOT_vertex_loop
	iaddiu		_sce_bfcMask,vi00,0x20
	ibeq		_sce_triangleStripMask,_sce_prim,SPOT_vertex_loop
	isub		_sce_noCull,_sce_prim,_sce_triangleMask
	ibltz		_sce_noCull,NOCULL_spot	

SPOT_fan:
	iaddiu		_sce_bfcMask,vi00,0x00
	fwInitFan
	fwStoreFanSwitch 118
	fwStoreFirstPoint 119
;*************************************************************

;******************** Vertex Loop-Block 0 ***********************
SPOT_vertex_loop:

     	--LoopCS	3,3


	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex
	lq 		_sce_normal,_sce_vert_nXYZ(_sce_in_p)				; load normal 
   	lq.xyz		_sce_ST,_sce_vert_ST(_sce_in_p)       				; load st
	lq 		_sce_vColor,_sce_vert_RGBA(_sce_in_p)				; load color

 	fwRTP "_sce_prim_XYZF2"
	fwST  "_sce_prim_ST"
	fwClip
	fwLoadFanSwitch 118
	fwLoadFirstPoint 119
	fwSwitchOldPointFirstPoint
	fwBFC
	fwWriteADC "_sce_prim_XYZF2"
	mScaleVectorXYZ _sce_vrt,_sce_vrt,_sce_scale
	fwLoadLightAngle 100,_sce_spotLightAngle ; spotLightAngle
	fwLoadLightConst 101,_sce_spotLightConst ;spotLightConst	
	fwLoadLightColor  \&SPOT_LIGHT_COLOR0,\&SPOT_LIGHT_COLOR1,\&SPOT_LIGHT_COLOR2,_sce_spotLightColor
	fwLoadLightAmbientColor \&AMBIENT_LIGHT_COLOR
	fwSpotLight "SCE_VERTEXC","SCE_AMBIENT",_sce_diffFact
	fwCalcColorSum "SCE_VERTEXC","SCE_CLAMP",_sce_vColor,_sce_diffFact,_sce_vColor  ;use _sce_vColor as output to save alpha
	fwWriteColor "_sce_prim_RGBAQ",_sce_vColor


; increment pointers, decrement counter

  	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, SPOT_vertex_loop   
;**************************************************************************************	
SPOT_done:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,SPOT_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		SPOT_START        
	
NOCULL_spot:	
	sub.w   _sce_storeVer,vf00,vf00                      ; screenVer.w=0
        fwInitPointLineLineStrip                  ;init. switches for point/line/linestrip clipping
;******************** Vertex Loop-Block 1 ***********************
NOCULL_spot_loop:

     	--LoopCS	1,1


	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex
	lq 		_sce_normal,_sce_vert_nXYZ(_sce_in_p)				; load normal 
   	lq.xyz		_sce_ST,_sce_vert_ST(_sce_in_p)       				; load st
	lq 		_sce_vColor,_sce_vert_RGBA(_sce_in_p)				; load color

 	fwRTP "_sce_prim_XYZF2" "basic"
	fwST  "_sce_prim_ST"
	fwClip "POINT_LINE_LINESTRIP"
	iaddiu	_sce_adcFlag,vi01,0x7fff
	fwWriteADC "_sce_prim_XYZF2"
	mScaleVectorXYZ _sce_vrt,_sce_vrt,_sce_scale
	fwLoadLightAngle 100,_sce_spotLightAngle ; spotLightAngle
	fwLoadLightConst 101,_sce_spotLightConst ;spotLightConst	
	fwLoadLightColor  \&SPOT_LIGHT_COLOR0,\&SPOT_LIGHT_COLOR1,\&SPOT_LIGHT_COLOR2,_sce_spotLightColor
	fwLoadLightAmbientColor \&AMBIENT_LIGHT_COLOR
	fwSpotLight "SCE_VERTEXC","SCE_AMBIENT",_sce_diffFact
	fwCalcColorSum "SCE_VERTEXC","SCE_CLAMP",_sce_vColor,_sce_diffFact,_sce_vColor  ;use _sce_vColor as output to save alpha

	fwWriteColor "_sce_prim_RGBAQ",_sce_vColor


; increment pointers, decrement counter

  	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, NOCULL_spot_loop   
;*************************************************************	
NOCULL_spot_done:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,SPOT_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		SPOT_START        













; -------------------------------------------------------------------

.END











