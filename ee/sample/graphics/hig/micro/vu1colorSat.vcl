   	.include "utils.i"
	.include "math.i
	.include "fwLights.i"   ; for lightning macros
	.include "fw.i"		; for basic  macros


	.init_vf_all
	.init_vi_all
	
	.name vclbasic_cs

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
BASICD_START:
	XTOP 		_sce_in_p
	ILW.x 		_sce_packsize, 10(VI00)		; inbuf size
	IADD   	 	_sce_pXkick,_sce_in_p,_sce_packsize
	IADDIU 	 	_sce_out_p,_sce_pXkick,0
;***********************************************

;******************** GifTag Init-Block ********
BASICD_GIFTAG:

	fwInitGifTag
	fwSetVertexCounter
	fwLoadLightColor  \&DIR_LIGHT_COLOR0,\&DIR_LIGHT_COLOR1,\&DIR_LIGHT_COLOR2,_sce_dirLightColor
	fwLoadLightAmbientColor \&AMBIENT_LIGHT_COLOR 

	sub.w	_sce_storeVer,vf00,vf00			; screenVer.w=0		
        fwInitAllGsPrimClip                  ;init. switches for point/line/linestrip/triangle/trianglestrip/fan clipping
;*************************************************************

;******************** Vertex Loop-Block ***********************
BASICD_vertex_loop:

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
	fwCalcColorSum "SCE_VERTEXC","SCE_SAT_CLAMP",_sce_vColor,_sce_diffFact,_sce_vColor  ;use _sce_vColor as output to save alpha
	
	FTOI0.xyz 	     _sce_vColor, _sce_vColor
	FTOI0.w 	     _sce_alphaC,_sce_alphaC
	SQ.xyz _sce_vColor,_sce_prim_RGBAQ(_sce_out_p)
	SQ.w   _sce_alphaC,_sce_prim_RGBAQ(_sce_out_p)

; increment pointers, decrement counter

  	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, BASICD_vertex_loop   
;**************************************************************************************

BASICD_done:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,BASICD_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		BASICD_START        
	












; -------------------------------------------------------------------

.END











