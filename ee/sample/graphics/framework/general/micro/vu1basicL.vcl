   	.include "utils.i"
	.include "math.i
	.include "fwLights.i"   ; for lightning macros
	.include "fw.i"		; for basic  macros


	.init_vf_all
	.init_vi_all
	
	.name vclbasicL

        --enter								; entry point for vcl
	--endenter   					

;**********Micro Init-Block *****************

	fwLocalScreen

	fwCalcNLightVect \&DIR_LIGHT_VECT0,\&DIR_LIGHT_VECT1,\&DIR_LIGHT_VECT2,_sce_dirNLightVect
;********************************************


	--cont
 	--cont					 


;******************* Buffer Init-Block *********
LBASIC_START:
	XTOP 		_sce_in_p
	ILW.x 		_sce_packsize, 10(VI00)		; inbuf size
	IADD   	 	_sce_pXkick,_sce_in_p,_sce_packsize
	IADDIU 	 	_sce_out_p,_sce_pXkick,0

;***********************************************

;******************** GifTag Init-Block ********
LBASIC_GIFTAG:

	fwInitGifTag
	fwSetVertexCounter
	fwLoadLightColor  \&DIR_LIGHT_COLOR0,\&DIR_LIGHT_COLOR1,\&DIR_LIGHT_COLOR2,_sce_dirLightColor
	fwLoadLightAmbientColor \&AMBIENT_LIGHT_COLOR 

	sub.w	_sce_storeVer,vf00,vf00			; screenVer.w=0		
;*************************************************************

;******************** Vertex Loop-Block ***********************
LBASIC_vertex_loop:

     	--LoopCS	1,1


	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex
	lq 		_sce_normal,_sce_vert_nXYZ(_sce_in_p)				; load vertex
   	lq.xyz		_sce_ST,_sce_vert_ST(_sce_in_p)       				; load st
	lq 		_sce_vColor,\&MATERIAL_DIFFUSE(VI00)					; load diffuse color
	lq		_sce_aColor,\&MATERIAL_AMBIENT(VI00)
       

	fwRTP "_sce_prim_XYZF2" "basic"
	fwST "_sce_prim_ST"
	fwDirLight "SCE_LAMBERT","SCE_AMBIENT",_sce_diffFact
	fwCalcColorSum "SCE_LAMBERT","SCE_CLAMP",_sce_vColor,_sce_diffFact,_sce_vColor,_sce_aColor  ;use _sce_vColor as output to save alpha
	fwWriteColor "_sce_prim_RGBAQ",_sce_vColor
 

; increment pointers, decrement counter

  	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, LBASIC_vertex_loop   
;**************************************************************************************	
LBASIC_done:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,LBASIC_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		LBASIC_START        
	












; -------------------------------------------------------------------

.END











