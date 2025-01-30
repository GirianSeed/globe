   	.include "utils.i"
	.include "math.i"
	.include "fwLights.i"   ; includes for lightning macros
	.include "fw.i"		; includes for basic  macros


	.init_vf_all
	.init_vi_all
	
	.name vclpointS

        --enter								; entry point for vcl
	--endenter   					

;**********Micro Init-Block *****************
	fwLocalScreen
	fwLoadLocalWorldRT
	mInvertRT _sce_worldLocal,_sce_localWorldRT
	fwGetScale _sce_scale,_sce_worldLocal,_sce_localWorld
	fwLocalCamera
	fwCalcLocalLightPos \&POINT_LIGHT_POS0,\&POINT_LIGHT_POS1,\&POINT_LIGHT_POS2,_sce_pointLocalLightPos,_sce_pointLightPos
	fwLoadLightColor \&POINT_LIGHT_COLOR0,\&POINT_LIGHT_COLOR1,\&POINT_LIGHT_COLOR2,_sce_pointLightColor
	fwStoreLightConst _sce_pointLightConst,_sce_pointLightColor,100
	fwInitClip 
;********************************************
	--cont
 	--cont					 


;******************* Buffer Init-Block *********
SPOINT_START:
	XTOP 		_sce_in_p
	ILW.x 		_sce_packsize, 10(VI00)		; inbuf size
	IADD   	 	_sce_pXkick,_sce_in_p,_sce_packsize
	IADDIU 	 	_sce_out_p,_sce_pXkick,0
;***********************************************

;******************** GifTag Init-Block ********
SPOINT_GIFTAG:
	fwInitGifTag
	fwSetVertexCounter	
	fwLoadLightColor  \&POINT_LIGHT_COLOR0,\&POINT_LIGHT_COLOR1,\&POINT_LIGHT_COLOR2,_sce_pointLightColor
	fwLoadLightAmbientColor \&AMBIENT_LIGHT_COLOR
	fwLoadLightConst 100, _sce_pointLightConst ;pointLightConst
	fwInitTriangleTriangleStrip
	fwStoreFanSwitch 118


	iaddi		_sce_bfcSwitch,vi00,0	
	iaddiu		_sce_bfcMask,vi00,0x00
	ibeq 		_sce_triangleMask,_sce_prim,SPOINT_vertex_loop
	iaddiu		_sce_bfcMask,vi00,0x20
	ibeq            _sce_triangleStripMask,_sce_prim,SPOINT_vertex_loop
	isub		_sce_noCull,_sce_prim,_sce_triangleMask
	ibltz		_sce_noCull,SNOCULL_point

SPOINT_fan:
	iaddiu		_sce_bfcMask,vi00,0x00
	fwInitFan
	fwStoreFanSwitch 118
	fwStoreFirstPoint 119
;*************************************************************

;******************** Vertex Loop-Block 0 ***********************
SPOINT_vertex_loop:

     	--LoopCS	3,3


	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex
	lq 		_sce_normal,_sce_vert_nXYZ(_sce_in_p)				; load normal 
   	lq.xyz		_sce_ST,_sce_vert_ST(_sce_in_p)       				; load st

 	fwRTP "_sce_prim_XYZF2"
	fwST  "_sce_prim_ST"
	fwClip
	fwLoadFanSwitch 118
	fwLoadFirstPoint 119
	fwSwitchOldPointFirstPoint
	fwBFC
	fwWriteADC "_sce_prim_XYZF2"
	
	mScaleVectorXYZ _sce_vrt,_sce_vrt,_sce_scale
	fwPointLight "SCE_PHONG","SCE_AMBIENT",_sce_diffFact,_sce_specFact
	fwLoadMaterialColor _sce_dColor,_sce_sColor,_sce_eColor,_sce_aColor,\&MATERIAL_DIFFUSE,\&MATERIAL_SPECULAR,\&MATERIAL_EMISSION,\&MATERIAL_AMBIENT
	fwCalcColorSum "SCE_PHONG","SCE_CLAMP",_sce_dColor,_sce_diffFact,_sce_dColor,_sce_aColor,_sce_eColor,_sce_specFact,_sce_sColor  ;use _sce_dColor as output to save alpha
	fwWriteColor "_sce_prim_RGBAQ",_sce_dColor


; increment pointers, decrement counter

  	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, SPOINT_vertex_loop   
;**************************************************************************************	
SPOINT_done:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,SPOINT_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		SPOINT_START        
	
SNOCULL_point:
	sub.w   _sce_storeVer,vf00,vf00                      ; screenVer.w=0  
        fwInitPointLineLineStrip                  ;init. switches for point/line/linestrip clipping
;******************** Vertex Loop-Block 1 ***********************

SNOCULL_point_loop:
     	--LoopCS	1,1


	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex
	lq 		_sce_normal,_sce_vert_nXYZ(_sce_in_p)				; load normal 
   	lq.xyz		_sce_ST,_sce_vert_ST(_sce_in_p)       				; load st

 	fwRTP "_sce_prim_XYZF2" "basic"
	fwST  "_sce_prim_ST"
	fwClip "POINT_LINE_LINESTRIP"
	iaddiu	_sce_adcFlag,vi01,0x7fff
	fwWriteADC "_sce_prim_XYZF2"

	mScaleVectorXYZ _sce_vrt,_sce_vrt,_sce_scale
	fwPointLight "SCE_PHONG","SCE_AMBIENT",_sce_diffFact,_sce_specFact
	fwLoadMaterialColor _sce_dColor,_sce_sColor,_sce_eColor,_sce_aColor,\&MATERIAL_DIFFUSE,\&MATERIAL_SPECULAR,\&MATERIAL_EMISSION,\&MATERIAL_AMBIENT
	fwCalcColorSum "SCE_PHONG","SCE_CLAMP",_sce_dColor,_sce_diffFact,_sce_dColor,_sce_aColor,_sce_eColor,_sce_specFact,_sce_sColor  ;use _sce_dColor as output to save alpha
	fwWriteColor "_sce_prim_RGBAQ",_sce_dColor


; increment pointers, decrement counter

  	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1
;*************************************************************

	ibne  		_sce_nVerts,vi00, SNOCULL_point_loop   
	
SNOCULL_done_point:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,SPOINT_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		SPOINT_START        












; -------------------------------------------------------------------

.END











