    	.include "utils.i"
	.include "math.i"
	.include "fwLights.i"   ; for lightning macros
	.include "fw.i"		; for basic  macros


	.init_vf_all
	.init_vi_all
	
	.name vclrefractR

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
	; load ref. mapping parameters
	LQ.y _sce_RefInde,31(VI00)
	LQ.zw _sce_RefZoom,31(VI00)
;********************************************

	--cont
 	--cont					 


;******************* Buffer Init-Block *********
refractR_START:
	XTOP 		_sce_in_p
	ILW.x 		_sce_packsize, 10(VI00)		; inbuf size
	IADD   	 	_sce_pXkick,_sce_in_p,_sce_packsize
	IADDIU 	 	_sce_out_p,_sce_pXkick,0
;***********************************************

;******************** GifTag Init-Block ********
refractR_GIFTAG:
	fwInitGifTag "TME_ON"
	fwSetVertexCounter
        fwInitAllGsPrimClip                  ;init. switches for point/line/linestrip/triangle/trianglestrip/fan clipping
;*************************************************************

;******************** Vertex Loop-Block 0 ***********************
refractR_vertex_loop:

     	--LoopCS	3,3


	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex
	lq 		_sce_normal,_sce_vert_nXYZ(_sce_in_p)				; load normal 

        mScaleVectorXYZ _sce_vrt,_sce_vrt,_sce_scale 
	fwSphereRefractST  _sce_localCamera,_sce_vrt

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

	ibne  		_sce_nVerts,vi00, refractR_vertex_loop   
;**************************************************************************************	
refractR_done_ver:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,refractR_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		refractR_START        

 


; -------------------------------------------------------------------

.END











