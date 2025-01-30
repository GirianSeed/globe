
   	.include "utils.i"
	.include "math.i" 
	.include "fwLights.i"   ; infludes for lights
	.include "fwAnti.i"     ; includes for AA1-anti
	.include "fw.i"		; includes for framework


	
	.init_vf_all
	.init_vi_all
	
	.name vclantiL

        --enter								; entry point for vcl
	--endenter   					

;**********Micro Init-Block *****************
	fwLocalScreen
	fwCalcNLightVect \&DIR_LIGHT_VECT0,\&DIR_LIGHT_VECT1,\&DIR_LIGHT_VECT2,_sce_dirNLightVect
	fwInitClip
	fwInitAnti  
	mInvertRT _sce_worldLocal,_sce_localWorld
	LQ.xyz _sce_worldCamera,28(VI00);
	mMakeLocal _sce_antiParam,_sce_worldCamera,_sce_worldLocal	
	LQ.w _sce_antiParam,28(VI00)   	; threshold for angle to camera test (xyz is cam-pos)
	SQ.xyzw _sce_antiParam,104(VI00)
;********************************************

	--cont
 	--cont					 


;******************* Buffer Init-Block *********
LANTI_START:
	XTOP 		_sce_in_p
	ILW.x 		_sce_packsize, 10(VI00)		; inbuf size
	IADD   	 	_sce_pXkick,_sce_in_p,_sce_packsize
	IADDIU 	 	_sce_out_p,_sce_pXkick,0
;***********************************************

;******************** GifTag Init-Block ********
LANTI_GIFTAG:
	fwInitGifTag
	fwSetVertexCounter
	fwLoadLightColor  \&DIR_LIGHT_COLOR0,\&DIR_LIGHT_COLOR1,\&DIR_LIGHT_COLOR2,_sce_dirLightColor
	fwLoadLightAmbientColor \&AMBIENT_LIGHT_COLOR
	fwInitTriangleTriangleStrip 
	fwStoreFanSwitch 118

	iaddi		_sce_bfcSwitch,vi00,0	
	iaddiu		_sce_bfcMask,vi00,0x00
	ibeq 		_sce_triangleMask,_sce_prim,LANTI_vertex_loop
	iaddiu		_sce_bfcMask,vi00,0x20
	ibeq            _sce_triangleStripMask,_sce_prim,LANTI_vertex_loop
	isub		_sce_noCull,_sce_prim,_sce_triangleMask
	ibltz		_sce_noCull,LNOCULL_anti

LANTI_fan:
	iaddiu		_sce_bfcMask,vi00,0x00
	fwInitFan "local"
	fwStoreFanSwitch 118
	fwStoreFirstPoint 119
;*************************************************************

;******************** Vertex Loop-Block 0 ***********************

LANTI_vertex_loop:
	
    	--LoopCS	3,3
	--LoopExtra     2 

	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex
	lq 		_sce_normal,_sce_vert_nXYZ(_sce_in_p)				; load vertex
   	lq.xyz		_sce_ST,_sce_vert_ST(_sce_in_p)       				; load st
	lq 		_sce_vColor,\&MATERIAL_DIFFUSE(VI00)				; load color
	lq		_sce_aColor,\&MATERIAL_AMBIENT(VI00)

 	fwRTP "_sce_aprim_XYZF2"
	fwST  "_sce_aprim_ST"
	fwClip
	fwLoadFanSwitch 118
	fwLoadFirstPoint 119
	fwSwitchOldPointFirstPoint "local"

	fwAntiBFC  "_sce_aprim_PRMOD"
	fwWriteADC "_sce_aprim_XYZF2"	
	fwDirLight "SCE_LAMBERT","SCE_NOAMBIENT",_sce_diffFact
	fwCalcColorSum "SCE_LAMBERT","SCE_CLAMP",_sce_vColor,_sce_diffFact,_sce_vColor,_sce_aColor  ;use _sce_vColor as output to save alpha
	fwWriteColor "_sce_aprim_RGBAQ",  _sce_vColor

  	iaddiu		_sce_out_p,_sce_out_p,_sce_aprim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, LANTI_vertex_loop   
;**************************************************************************************	
LANTI_done:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,LANTI_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		LANTI_START        
	
LNOCULL_anti:
	sub.w   _sce_storeVer,vf00,vf00                      ; screenVer.w=0  
 
;******************** Vertex Loop-Block 1 ***********************
LNOCULL_anti_loop:
	
    	--LoopCS	1,1


	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex
	lq 		_sce_normal,_sce_vert_nXYZ(_sce_in_p)				; load vertex
   	lq.xyz		_sce_ST,_sce_vert_ST(_sce_in_p)       				; load st
	lq 		_sce_vColor,\&MATERIAL_DIFFUSE(VI00)				; load color
	lq		_sce_aColor,\&MATERIAL_AMBIENT(VI00)

 	fwRTP "_sce_aprim_XYZF2" "basic"
	fwST  "_sce_aprim_ST"
	fwClip
	iaddiu	_sce_adcFlag,vi01,0x7fff
	fwWriteADC "_sce_prim_XYZF2"
	ior		_sce_prmod_Data,_sce_attribute,_sce_minusMask2		; now has AA1 set 
	isw.z 		_sce_prmod_Addr,_sce_aprim_PRMOD(_sce_out_p)		; set ADDRESS = prmod AD = 0x1b
	isw.x 		_sce_prmod_Data,_sce_aprim_PRMOD(_sce_out_p) 		; set DATA = prmod value

	fwDirLight "SCE_LAMBERT","SCE_NOAMBIENT",_sce_diffFact
	fwCalcColorSum "SCE_LAMBERT","SCE_CLAMP",_sce_vColor,_sce_diffFact,_sce_vColor,_sce_aColor  ;use _sce_vColor as output to save alpha
	fwWriteColor "_sce_aprim_RGBAQ", _sce_vColor

  	iaddiu		_sce_out_p,_sce_out_p,_sce_aprim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, LNOCULL_anti_loop   
;*************************************************************	
	
LNOCULL_anti_done:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,LANTI_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		LANTI_START        












; -------------------------------------------------------------------

.END











