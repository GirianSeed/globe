   	.include "utils.i"
	.include "math.i"
	.include "fwFog.i"
	.include "fwLights.i"   ; for lightning macros
	.include "fw.i"		; for basic  macros


	.init_vf_all
	.init_vi_all
	
	.name vclfogL

        --enter								; entry point for vcl
	--endenter   					

;**********Micro Init-Block *****************

	fwLocalScreen
	fwCalcNLightVect \&DIR_LIGHT_VECT0,\&DIR_LIGHT_VECT1,\&DIR_LIGHT_VECT2,_sce_dirNLightVect
	fwInitClip 
;********************************************
	--cont
 	--cont					 


;******************* Buffer Init-Block *********
LFOG_START:
	XTOP 		_sce_in_p
	ILW.x 		_sce_packsize, 10(VI00)		; inbuf size
	IADD   	 	_sce_pXkick,_sce_in_p,_sce_packsize
	IADDIU 	 	_sce_out_p,_sce_pXkick,0
	fwInitFog
;***********************************************

;******************** GifTag Init-Block ********
LFOG_GIFTAG:
	fwInitGifTag
	fwSetVertexCounter
	fwLoadLightColor  \&DIR_LIGHT_COLOR0,\&DIR_LIGHT_COLOR1,\&DIR_LIGHT_COLOR2,_sce_dirLightColor
	fwLoadLightAmbientColor \&AMBIENT_LIGHT_COLOR
	fwInitTriangleTriangleStrip
	fwStoreFanSwitch 118

	iaddi		_sce_bfcSwitch,vi00,0	
	iaddiu		_sce_bfcMask,vi00,0x00
	ibeq 		_sce_triangleMask,_sce_prim,LFOG_vertex_loop
	iaddiu		_sce_bfcMask,vi00,0x20
	ibeq            _sce_triangleStripMask,_sce_prim,LFOG_vertex_loop
	isub		_sce_noCull,_sce_prim,_sce_triangleMask
	ibltz		_sce_noCull,LNOCULL_fog

LFOG_fan:
	iaddiu		_sce_bfcMask,vi00,0x00
	fwInitFan
	fwStoreFanSwitch 118
	fwStoreFirstPoint 119

;*************************************************************

;******************** Vertex Loop-Block 0 ***********************	
LFOG_vertex_loop:

     	--LoopCS	3,3


	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex
	lq 		_sce_normal,_sce_vert_nXYZ(_sce_in_p)				; load vertex
   	lq.xyz		_sce_ST,_sce_vert_ST(_sce_in_p)       				; load st
	lq 		_sce_vColor,\&MATERIAL_DIFFUSE(VI00)				; load color
	lq		_sce_aColor,\&MATERIAL_AMBIENT(VI00)

 	fwRTP "_sce_prim_XYZF2"
	fwST  "_sce_prim_ST"
	fwClip
	fwLoadFanSwitch 118
	fwLoadFirstPoint 119
	fwSwitchOldPointFirstPoint
	fwBFC
	fwFog
	fwWriteADC_Fog "_sce_prim_XYZF2"
	fwDirLight "SCE_LAMBERT","SCE_NOAMBIENT",_sce_diffFact
	fwCalcColorSum "SCE_LAMBERT","SCE_CLAMP",_sce_vColor,_sce_diffFact,_sce_vColor,_sce_aColor  ;use _sce_vColor as output to save alpha
	fwWriteColor "_sce_prim_RGBAQ",_sce_vColor


; increment pointers, decrement counter

  	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, LFOG_vertex_loop   
;**************************************************************************************	
LFOG_done:
 
	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,LFOG_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		LFOG_START        
	
LNOCULL_fog:
	
;******************** Vertex Loop-Block 1 ***********************
LNOCULL_fog_loop:

     	--LoopCS	1,1


	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex
	lq 		_sce_normal,_sce_vert_nXYZ(_sce_in_p)				; load vertex
   	lq.xyz		_sce_ST,_sce_vert_ST(_sce_in_p)       				; load st
	lq 		_sce_vColor,\&MATERIAL_DIFFUSE(VI00)				; load color
	lq		_sce_aColor,\&MATERIAL_AMBIENT(VI00)

 	fwRTP "_sce_prim_XYZF2"
	fwST  "_sce_prim_ST"
	fwClip
	iaddiu	_sce_adcFlag,vi01,0x7fff
	fwFog
	fwWriteADC_Fog "_sce_prim_XYZF2"
	fwDirLight "SCE_LAMBERT","SCE_NOAMBIENT",_sce_diffFact
	fwCalcColorSum "SCE_LAMBERT","SCE_CLAMP",_sce_vColor,_sce_diffFact,_sce_vColor,_sce_aColor  ;use _sce_vColor as output to save alpha
	fwWriteColor "_sce_prim_RGBAQ",_sce_vColor


; increment pointers, decrement counter

  	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, LNOCULL_fog_loop   
;*************************************************************		
LNOCULL_fog_done:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,LFOG_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		LFOG_START        












; -------------------------------------------------------------------

.END











