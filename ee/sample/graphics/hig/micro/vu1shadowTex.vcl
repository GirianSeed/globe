   	.include "utils.i"
	.include "math.i
	.include "fwLights.i"   ; for lightning macros
	.include "fw.i"		; for basic  macros


	.init_vf_all
	.init_vi_all
	
	.name shadowtex 

        --enter								; entry point for vcl
	--endenter   				
	
;**********Micro Init-Block *****************

	fwLocalScreen
	;shadowColor
	LOI 255.0
	ADDI.xyz  _sce_shadowColor,vf00,I
	LOI 128.0
	MULI.w	_sce_shadowColor,vf00,I
	FTOI0 	write_color, _sce_shadowColor		
	--cont
 	--cont					 


;******************* Buffer Init-Block *********
SHADOWTEX_START:
	XTOP 		_sce_in_p
	ILW.x 		_sce_packsize, 10(VI00)		; inbuf size
	IADD   	 	_sce_pXkick,_sce_in_p,_sce_packsize
	IADDIU 	 	_sce_out_p,_sce_pXkick,0
;***********************************************

;******************** GifTag Init-Block ********
SHADOWTEX_GIFTAG:

	fwInitGifTag "TME_OFF"
	fwSetVertexCounter
	sub.w	_sce_storeVer,vf00,vf00			; screenVer.w=0		
;*************************************************************

;******************** Vertex Loop-Block ***********************
SHADOWTEX_vertex_loop:

     	--LoopCS	1,1

	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex


	fwRTP "_sce_prim_XYZF2" "basic"
	SQ 		write_color, _sce_prim_RGBAQ(_sce_out_p)

; increment pointers, decrement counter

  	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, SHADOWTEX_vertex_loop   
;**************************************************************************************

SHADOWTEX_done:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,SHADOWTEX_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		SHADOWTEX_START        
	












; -------------------------------------------------------------------

.END











