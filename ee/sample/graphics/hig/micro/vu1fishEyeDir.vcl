    	.include "utils.i"
	.include "math.i"
	.include "fwLights.i"   ; for lightning macros
	.include "fw.i"		; for basic  macros


	.init_vf_all
	.init_vi_all
	
	.name vclfish

        --enter								; entry point for vcl
	--endenter   					

;**********Micro Init-Block *****************
	fwLocalScreen
        fwLocalView
	fwCalcNLightVect \&DIR_LIGHT_VECT0,\&DIR_LIGHT_VECT1,\&DIR_LIGHT_VECT2,_sce_dirNLightVect
	fwStoreNLightVect 103,104,105,_sce_dirNLightVect
	LQ.x _sce_Zoom,30(VI00)
	LQ.yz _sce_fishZfact,30(VI00)
;********************************************

	--cont
 	--cont					 


;******************* Buffer Init-Block *********
fish_START:
	XTOP 		_sce_in_p
	ILW.x 		_sce_packsize, 10(VI00)		; inbuf size
	IADD   	 	_sce_pXkick,_sce_in_p,_sce_packsize
	IADDIU 	 	_sce_out_p,_sce_pXkick,0
;***********************************************

;******************** GifTag Init-Block ********
fish_GIFTAG:
	fwInitGifTag
	fwSetVertexCounter
	fwLoadLightColor  \&DIR_LIGHT_COLOR0,\&DIR_LIGHT_COLOR1,\&DIR_LIGHT_COLOR2,_sce_dirLightColor
	fwLoadLightAmbientColor \&AMBIENT_LIGHT_COLOR
	fwInitTriangleTriangleStrip
	fwStoreFanSwitch 118

	iaddi		_sce_bfcSwitch,vi00,0	
	iaddiu		_sce_bfcMask,vi00,0x00 
	ibeq 		_sce_triangleMask,_sce_prim,fish_vertex_loop
	iaddiu		_sce_bfcMask,vi00,0x20
	ibeq		_sce_triangleStripMask,_sce_prim,fish_vertex_loop
	isub		_sce_noCull,_sce_prim,_sce_triangleMask
	ibltz		_sce_noCull,NoCullfish	


fish_fan:
	iaddiu		_sce_bfcMask,vi00,0x00
	fwInitFan "noclip"
	fwStoreFanSwitch 118
	fwStoreFirstPoint 119
;*************************************************************

;******************** Vertex Loop-Block 0 ***********************
fish_vertex_loop:

     	--LoopCS	3,3


	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex
	lq 		_sce_normal,_sce_vert_nXYZ(_sce_in_p)				; load normal
   	lq.xyz		_sce_ST,_sce_vert_ST(_sce_in_p)       				; load st
	lq              _sce_vColor,_sce_vert_RGBA(_sce_in_p)
                                     

	fwFishEye 	"_sce_prim_XYZF2",_sce_vrt
	fwLoadFanSwitch 118 
	fwLoadFirstPoint 119
	fwSwitchOldPointFirstPoint
	fwBFC
	fwWriteADC "_sce_prim_XYZF2"

	fwLoadNLightVect 103,104,105,_sce_dirNLightVect
 	fwDirLight "SCE_VERTEXC","SCE_AMBIENT",_sce_diffFact
	fwCalcColorSum "SCE_VERTEXC","SCE_CLAMP",_sce_vColor,_sce_diffFact,_sce_vColor   ;use _sce_dColor as output to save alpha
	fwWriteColor "_sce_prim_RGBAQ",_sce_vColor


; increment pointers, decrement counter

  	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, fish_vertex_loop   
;**************************************************************************************	
fish_done_ver:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,fish_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		fish_START        

 
NoCullfish:
	sub.w   _sce_storeVer,vf00,vf00                      ; screenVer.w=0          

;******************** Vertex Loop-Block 1 ***********************
NoCullfish_loop:
     	--LoopCS	1,1


	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex
	lq 		_sce_normal,_sce_vert_nXYZ(_sce_in_p)				; load normal 
   	lq.xyz		_sce_ST,_sce_vert_ST(_sce_in_p)       				; load st
	lq              _sce_vColor,_sce_vert_RGBA(_sce_in_p)

	fwFishEye	"_sce_prim_XYZF2",_sce_vrt
	iaddiu	_sce_adcFlag,vi01,0x7fff
	fwWriteADC "_sce_prim_XYZF2"

	fwLoadNLightVect 103,104,105,_sce_dirNLightVect
 	fwDirLight "SCE_VERTEXC","SCE_AMBIENT",_sce_diffFact
	fwCalcColorSum "SCE_VERTEXC","SCE_CLAMP",_sce_vColor,_sce_diffFact,_sce_vColor   ;use _sce_dColor as output to save alpha
	fwWriteColor "_sce_prim_RGBAQ",_sce_vColor


; increment pointers, decrement counter

  	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, NoCullfish_loop   
;*************************************************************		
NoCullfish_done:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,fish_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		fish_START        


; -------------------------------------------------------------------

.END











