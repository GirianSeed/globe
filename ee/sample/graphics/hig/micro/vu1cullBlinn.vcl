    	.include "utils.i"
	.include "math.i"
	.include "fwLights.i"   ; for lightning macros
	.include "fw.i"		; for basic  macros


	.init_vf_all
	.init_vi_all
	
	.name vclcullS

        --enter								; entry point for vcl
	--endenter   					

;**********Micro Init-Block *****************
	fwLocalScreen
	fwLoadLocalWorldRT
	mInvertRT _sce_worldLocal,_sce_localWorldRT
	fwGetScale _sce_scale,_sce_worldLocal,_sce_localWorld
	fwLocalCamera
	fwCalcNLightVect \&DIR_LIGHT_VECT0,\&DIR_LIGHT_VECT1,\&DIR_LIGHT_VECT2,_sce_dirNLightVect
	fwStoreNLightVect 103,104,105,_sce_dirNLightVect
	fwInitClip 
;********************************************

	--cont
 	--cont					 


;******************* Buffer Init-Block *********
SCULL_START:
	XTOP 		_sce_in_p
	ILW.x 		_sce_packsize, 10(VI00)		; inbuf size
	IADD   	 	_sce_pXkick,_sce_in_p,_sce_packsize
	IADDIU 	 	_sce_out_p,_sce_pXkick,0
;***********************************************

;******************** GifTag Init-Block ********
SCULL_GIFTAG:
	fwInitGifTag
	fwSetVertexCounter
	fwLoadLightColor  \&DIR_LIGHT_COLOR0,\&DIR_LIGHT_COLOR1,\&DIR_LIGHT_COLOR2,_sce_dirLightColor
	fwLoadLightAmbientColor \&AMBIENT_LIGHT_COLOR
	fwInitTriangleTriangleStrip
	fwStoreFanSwitch 118

	iaddi		_sce_bfcSwitch,vi00,0	
	iaddiu		_sce_bfcMask,vi00,0x00 
	ibeq 		_sce_triangleMask,_sce_prim,SCULL_vertex_loop
	iaddiu		_sce_bfcMask,vi00,0x20
	ibeq		_sce_triangleStripMask,_sce_prim,SCULL_vertex_loop
	isub		_sce_noCull,_sce_prim,_sce_triangleMask
	ibltz		_sce_noCull,SNOCULL_cull	

SCULL_fan:
	iaddiu		_sce_bfcMask,vi00,0x00
	fwInitFan
	fwStoreFanSwitch 118
	fwStoreFirstPoint 119
;*************************************************************

;******************** Vertex Loop-Block 0 ***********************
SCULL_vertex_loop:

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
	fwLoadNLightVect 103,104,105,_sce_dirNLightVect
 	fwDirLight "SCE_BLINN","SCE_AMBIENT",_sce_diffFact,_sce_specFact
	fwLoadMaterialColor _sce_dColor,_sce_sColor,_sce_eColor,_sce_aColor,\&MATERIAL_DIFFUSE,\&MATERIAL_SPECULAR,\&MATERIAL_EMISSION,\&MATERIAL_AMBIENT
	fwCalcColorSum "SCE_BLINN","SCE_CLAMP",_sce_dColor,_sce_diffFact,_sce_dColor,_sce_aColor,_sce_eColor,_sce_specFact,_sce_sColor  ;use _sce_dColor as output to save alpha
	fwWriteColor "_sce_prim_RGBAQ",_sce_dColor


; increment pointers, decrement counter

  	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, SCULL_vertex_loop   
;**************************************************************************************	
SCULL_done_ver:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,SCULL_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		SCULL_START        

 
SNOCULL_cull:
	sub.w   _sce_storeVer,vf00,vf00                      ; screenVer.w=0          
        fwInitPointLineLineStrip                  ;init. switches for point/line/linestrip clipping
;******************** Vertex Loop-Block 1 ***********************
SNOCULL_loop:
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
	fwLoadNLightVect 103,104,105,_sce_dirNLightVect	
 	fwDirLight "SCE_BLINN","SCE_AMBIENT",_sce_diffFact,_sce_specFact
	fwLoadMaterialColor _sce_dColor,_sce_sColor,_sce_aColor,_sce_eColor,\&MATERIAL_DIFFUSE,\&MATERIAL_SPECULAR,\&MATERIAL_AMBIENT,\&MATERIAL_EMISSION
	fwCalcColorSum "SCE_BLINN","SCE_CLAMP",_sce_dColor,_sce_diffFact,_sce_dColor,_sce_aColor,_sce_eColor,_sce_specFact,_sce_sColor  ;use _sce_dColor as output to save alpha
	fwWriteColor "_sce_prim_RGBAQ",_sce_dColor


; increment pointers, decrement counter

  	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, SNOCULL_loop   
;*************************************************************		
SNOCULL_done:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,SCULL_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		SCULL_START        


; -------------------------------------------------------------------

.END











