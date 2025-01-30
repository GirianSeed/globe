    	.include "utils.i"
	.include "math.i"
	.include "fwLights.i"   ; for lightning macros
	.include "fw.i"		; for basic  macros


	.init_vf_all
	.init_vi_all
	
	.name vclreflectS

        --enter								; entry point for vcl
	--endenter   					

;**********Micro Init-Block *****************
	fwLocalScreen
        fwLocalView
        fwLoadLocalWorldRT
        mInvertRT _sce_worldLocal,_sce_localWorldRT
        fwGetScale _sce_scale,_sce_worldLocal,_sce_localWorld   
	fwLocalCamera
	fwInitClip 
	; get refl.mapping parameters
	LQ.z _sce_RefZoom,31(VI00)	
;********************************************

	--cont
 	--cont					 


;******************* Buffer Init-Block *********
reflectS_START:
	XTOP 		_sce_in_p
	ILW.x 		_sce_packsize, 10(VI00)		; inbuf size
	IADD   	 	_sce_pXkick,_sce_in_p,_sce_packsize
	IADDIU 	 	_sce_out_p,_sce_pXkick,0
;***********************************************

;******************** GifTag Init-Block ********
reflectS_GIFTAG:
	fwInitGifTag
	fwSetVertexCounter
        fwInitAllGsPrimClip                  ;init. switches for point/line/linestrip/triangle/trianglestrip/fan clipping
;*************************************************************

;******************** Vertex Loop-Block 0 ***********************
reflectS_vertex_loop:

     	--LoopCS	3,3


	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex
	lq 		_sce_normal,_sce_vert_nXYZ(_sce_in_p)				; load normal 

        mScaleVectorXYZ _sce_vrt,_sce_vrt,_sce_scale
        fwSphereReflectST  _sce_localCamera,_sce_vrt,"static" 

        lq              _sce_vrt,_sce_vert_XYZ(_sce_in_p)                               ; reload unscaled vertex  
	fwRTP "_sce_prim_XYZF2"
	fwST  "_sce_prim_ST"
	fwClip "POINT_LINE_LINESTRIP_TRIANGLE_TRIANGLESTRIP_FAN"
	iaddiu	_sce_adcFlag,vi01,0x7fff
	fwWriteADC "_sce_prim_XYZF2"


; increment pointers, decrement counter

  	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, reflectS_vertex_loop   
;**************************************************************************************	
reflectS_done_ver:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,reflectS_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		reflectS_START        

 


; -------------------------------------------------------------------

.END











