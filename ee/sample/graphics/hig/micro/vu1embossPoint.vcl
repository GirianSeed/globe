   	.include "utils.i"
	.include "math.i
	.include "fwLights.i"   ; for lightning macros
	.include "fw.i"		; for basic  macros


	.init_vf_all
	.init_vi_all
	
	.name vclbump

        --enter								; entry point for vcl
	--endenter   				
	
;**********Micro Init-Block *****************

	fwLocalScreen
	fwLoadLocalWorldRT
	mInvertRT _sce_worldLocal,_sce_localWorldRT
	fwGetScale _sce_scale,_sce_worldLocal,_sce_localWorld
	fwCalcLocalLightPos \&POINT_LIGHT_POS0,\&POINT_LIGHT_POS1,\&POINT_LIGHT_POS2,_sce_pointLocalLightPos,_sce_pointLightPos
	fwLoadLightColor \&POINT_LIGHT_COLOR0,\&POINT_LIGHT_COLOR1,\&POINT_LIGHT_COLOR2,_sce_pointLightColor
	fwStoreLightConst _sce_pointLightConst,_sce_pointLightColor,100
	fwInitClip
;********************************************


	--cont
 	--cont					 


;******************* Buffer Init-Block *********
BUMP_START:
	XTOP 		_sce_in_p
	ILW.x 		_sce_packsize, 10(VI00)		; inbuf size
	IADD   	 	_sce_pXkick,_sce_in_p,_sce_packsize
	IADDIU 	 	_sce_out_p,_sce_pXkick,0
;***********************************************

;******************** GifTag Init-Block ********
BUMP_GIFTAG:


	fwInitGifTag
	fwSetVertexCounter
	fwLoadLightColor \&POINT_LIGHT_COLOR0,\&POINT_LIGHT_COLOR1,\&POINT_LIGHT_COLOR2,_sce_pointLightColor
	fwLoadLightAmbientColor \&AMBIENT_LIGHT_COLOR
	fwLoadLightConst 100, _sce_pointLightConst ;pointLightConst

        fwInitTriangleTriangleStrip
        fwInitFan

	ibeq 		_sce_triangleMask,_sce_prim,BUMP_vertex_loop
	b               BUMP_stripfan


;*************************************************************

;******************** Vertex Loop-Block ***********************
BUMP_vertex_loop:

     	--LoopCS	1,1

	lq 		_sce_vrt0,_sce_vert_XYZ(_sce_in_p)				; load vertex
   	lq.xyz		_sce_ST0,_sce_vert_ST(_sce_in_p)       				; load st
	lq 		_sce_vrt1,_sce_vert_XYZ+4(_sce_in_p)				; load vertex
	lq.xyz		_sce_ST1,_sce_vert_ST+4(_sce_in_p)       		        ; load st
	lq 		_sce_vrt2,_sce_vert_XYZ+8(_sce_in_p)				; load vertex
	lq.xyz		_sce_ST2,_sce_vert_ST+8(_sce_in_p)       			; load st
	fwCalcSTSxT _sce_local_tex


	; 1st vertex
	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex
   	lq.xyz		_sce_ST,_sce_vert_ST(_sce_in_p)       				; load st
	lq 		_sce_vColor,_sce_vert_RGBA(_sce_in_p)				; load color
	lq 		_sce_normal,_sce_vert_nXYZ(_sce_in_p)				; load normal

	mScaleVectorXYZ _sce_vrt,_sce_vrt,_sce_scale
	fwPointLight "SCE_VERTEXC","SCE_NOAMBIENT",_sce_diffFact
	fwCalcColorSum "SCE_VERTEXC","SCE_CLAMP",_sce_vColor,_sce_diffFact,_sce_vColor  ;use _sce_vColor as output to save alpha
	fwWriteColor "_sce_prim_RGBAQ",_sce_vColor

	; use _sce_lightVect1 from fwPointLight-macro
	MULx.xyz 	_sce_light,_sce_lightVect1,_sce_oneOverNorm	
	fwCalcEmbossShift _sce_shift,_sce_local_tex,_sce_light
	ADD.xy      _sce_ST,_sce_ST,_sce_shift

	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; reload unscaled vertex
	fwRTP "_sce_prim_XYZF2" 
	fwST "_sce_prim_ST"
	fwClip "TRIANGLE"
	iaddiu	_sce_adcFlag,vi01,0x7fff
	fwWriteADC "_sce_prim_XYZF2"
	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof

	; 2nd vertex
	lq 		_sce_vrt,_sce_vert_XYZ+4(_sce_in_p)				; load vertex
   	lq.xyz		_sce_ST,_sce_vert_ST+4(_sce_in_p)       			; load st
	lq 		_sce_vColor,_sce_vert_RGBA+4(_sce_in_p)				; load color
	lq 		_sce_normal,_sce_vert_nXYZ+4(_sce_in_p)				; load norma

	mScaleVectorXYZ _sce_vrt,_sce_vrt,_sce_scale
	fwPointLight "SCE_VERTEXC","SCE_NOAMBIENT",_sce_diffFact
	fwCalcColorSum "SCE_VERTEXC","SCE_CLAMP",_sce_vColor,_sce_diffFact,_sce_vColor  ;use _sce_vColor as output to save alpha
	fwWriteColor "_sce_prim_RGBAQ",_sce_vColor

	; use _sce_lightVect1 from fwPointLight-macro
	MULx.xyz 	_sce_light,_sce_lightVect1,_sce_oneOverNorm	
	fwCalcEmbossShift _sce_shift,_sce_local_tex,_sce_light
	ADD.xy      _sce_ST,_sce_ST,_sce_shift

	lq 		_sce_vrt,_sce_vert_XYZ+4(_sce_in_p)				; reload unscaled vertex
	fwRTP "_sce_prim_XYZF2" 
	fwST "_sce_prim_ST"
	fwClip "TRIANGLE"
	iaddiu	_sce_adcFlag,vi01,0x7fff
	fwWriteADC "_sce_prim_XYZF2"
	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof

	; 3rd vertex
	lq 		_sce_vrt,_sce_vert_XYZ+8(_sce_in_p)				; load vertex
   	lq.xyz		_sce_ST,_sce_vert_ST+8(_sce_in_p)       			; load st
	lq 		_sce_vColor,_sce_vert_RGBA+8(_sce_in_p)				; load color
	lq 		_sce_normal,_sce_vert_nXYZ+8(_sce_in_p)				; load normal

	mScaleVectorXYZ _sce_vrt,_sce_vrt,_sce_scale
	fwPointLight "SCE_VERTEXC","SCE_NOAMBIENT",_sce_diffFact
	fwCalcColorSum "SCE_VERTEXC","SCE_CLAMP",_sce_vColor,_sce_diffFact,_sce_vColor  ;use _sce_vColor as output to save alpha
	fwWriteColor "_sce_prim_RGBAQ",_sce_vColor

	; use _sce_lightVect1 from fwPointLight-macro
	MULx.xyz 	_sce_light,_sce_lightVect1,_sce_oneOverNorm	
	fwCalcEmbossShift _sce_shift,_sce_local_tex,_sce_light
	ADD.xy      _sce_ST,_sce_ST,_sce_shift

	lq 		_sce_vrt,_sce_vert_XYZ+8(_sce_in_p)				; reload unscaled vertex
	fwRTP "_sce_prim_XYZF2"
	fwST "_sce_prim_ST"
	fwClip "TRIANGLE"
	iaddiu	_sce_adcFlag,vi01,0x7fff
	fwWriteADC "_sce_prim_XYZF2"
   	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof

; increment pointers, decrement counter
   	iaddiu		_sce_in_p,_sce_in_p,3*_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-3

	ibne  		_sce_nVerts,vi00, BUMP_vertex_loop   
;**************************************************************************************

BUMP_done:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,BUMP_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		BUMP_START        
	

BUMP_stripfan:
	lq 		_sce_vrt0,_sce_vert_XYZ(_sce_in_p)				; load vertex
   	lq.xyz		_sce_ST0,_sce_vert_ST(_sce_in_p)       				; load st

	lq 		_sce_vrt1,_sce_vert_XYZ+4(_sce_in_p)				; load vertex
	lq.xyz		_sce_ST1,_sce_vert_ST+4(_sce_in_p)       		        ; load st

	lq 		_sce_vrt2,_sce_vert_XYZ+8(_sce_in_p)				; load vertex
	lq.xyz		_sce_ST2,_sce_vert_ST+8(_sce_in_p)       			; load st

	fwCalcSTSxT _sce_local_tex
							
;******************** Vertex Loop-Block 0 ***********************
bump_start_loop:

     	--LoopCS	3,3


	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex
	lq 		_sce_normal,_sce_vert_nXYZ(_sce_in_p)				; load normal
   	lq.xyz		_sce_ST,_sce_vert_ST(_sce_in_p)       				; load st
	lq 		_sce_vColor,_sce_vert_RGBA(_sce_in_p)				; load color


	mScaleVectorXYZ _sce_vrt,_sce_vrt,_sce_scale
	fwPointLight "SCE_VERTEXC","SCE_NOAMBIENT",_sce_diffFact
	fwCalcColorSum "SCE_VERTEXC","SCE_CLAMP",_sce_vColor,_sce_diffFact,_sce_vColor  ;use _sce_vColor as output to save alpha
	fwWriteColor "_sce_prim_RGBAQ",_sce_vColor

	; use _sce_lightVect1 from fwPointLight-macro
	MULx.xyz 	_sce_light,_sce_lightVect1,_sce_oneOverNorm	
	fwCalcEmbossShift _sce_shift,_sce_local_tex,_sce_light
	ADD.xy      _sce_ST,_sce_ST,_sce_shift

	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; reload unscaled vertex
	fwRTP "_sce_prim_XYZF2" 
	fwST "_sce_prim_ST"
	fwClip "TRIANGLESTRIP_FAN"
	iaddiu	_sce_adcFlag,vi01,0x7fff
	fwWriteADC "_sce_prim_XYZF2"

; increment pointers, decrement counter

  	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, bump_start_loop   
;**************************************************************************************


	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,BUMP_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		BUMP_START        






; -------------------------------------------------------------------

.END











