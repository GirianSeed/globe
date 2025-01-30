   	.include "utils.i"
	.include "math.i
	.include "fwLights.i"   ; for lightning macros
	.include "fw.i"		; for basic  macros


	.init_vf_all
	.init_vi_all
	
	.name lightmstq_culling

        --enter								; entry point for vcl
	--endenter   				
	
;**********Micro Init-Block *****************

	fwLocalScreen

        fwLoadLocalWorldRT
        mInvertRT _sce_worldLocal,_sce_localWorldRT          

	;for back-face projection cancellation
	LQ.xyz _sce_zdir,\&DIR_LIGHT_VECT0(VI00)	
	mulax.xyz	acc, _sce_worldLocal[0],_sce_zdir
	madday.xyz	acc, _sce_worldLocal[1],_sce_zdir
	maddz.xyz	_sce_localZdir, _sce_worldLocal[2],_sce_zdir
	getRcpLengP_z	_sce_localZdir,_sce_oneOverNorm
	MULz.xyz	_sce_localZdir,_sce_localZdir,_sce_oneOverNormz
	LOI -1.0
	MULI.xyz	_sce_localZdir,_sce_localZdir,I
	LQ.xyz 	_sce_light_color,\&DIR_LIGHT_COLOR0(VI00)
	LOI 255.0
	MULI.xyz  _sce_light_color,_sce_light_color,I
	LOI 128.0
	MULI.w	_sce_light_color,vf00,I
	FTOI0 	write_color, _sce_light_color
;********************************************
	lq _worldLightm[0], 20(VI00)
	lq _worldLightm[1], 21(VI00)
	lq _worldLightm[2], 22(VI00)
	lq _worldLightm[3], 23(VI00)

	mMulVecByMat _localLightm[0],_sce_localWorld[0],_worldLightm
	mMulVecByMat _localLightm[1],_sce_localWorld[1],_worldLightm
	mMulVecByMat _localLightm[2],_sce_localWorld[2],_worldLightm
	mMulVecByMat _localLightm[3],_sce_localWorld[3],_worldLightm

	iaddiu 	_sce_minus,vi00,0x0080	
	fwInitClip
	--cont
 	--cont					 


;******************* Buffer Init-Block *********
LIGHTMSTQC_START:
	XTOP 		_sce_in_p
	ILW.x 		_sce_packsize, 10(VI00)		; inbuf size
	IADD   	 	_sce_pXkick,_sce_in_p,_sce_packsize
	IADDIU 	 	_sce_out_p,_sce_pXkick,0

;***********************************************

;******************** GifTag Init-Block ********
LIGHTMSTQC_GIFTAG:

	fwInitGifTag "TME_ON"
	fwSetVertexCounter
	fwInitTriangleTriangleStrip
	fwStoreFanSwitch 118



	iaddi		_sce_bfcSwitch,vi00,0	
	iaddiu		_sce_bfcMask,vi00,0x00 
	ibeq 		_sce_triangleMask,_sce_prim,LIGHTMSTQC_vertex_loop
	iaddiu		_sce_bfcMask,vi00,0x20
	ibeq		_sce_triangleStripMask,_sce_prim,LIGHTMSTQC_vertex_loop


;*************************************************************

LIGHTMSTQC_fan:
	iaddiu		_sce_bfcMask,vi00,0x00
	fwInitFan
	fwStoreFanSwitch 118
	fwStoreFirstPoint 119

;******************** Vertex Loop-Block ***********************
LIGHTMSTQC_vertex_loop:

     	--LoopCS	1,1


	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex
	lq 		_sce_normal,_sce_vert_nXYZ(_sce_in_p)				; load normal

	lq 		_sce_vColor,_sce_vert_RGBA(_sce_in_p)				; load color

;*************************************************************
;			Calc Lightm ST
;*************************************************************

	rinit 	R,vf00x
	rget	_sce_one,R
	
	MUL.xyz		_sce_innerPtemp,_sce_localZdir,_sce_normal
	ADDAy.x		ACC,_sce_innerPtemp,_sce_innerPtemp
	MADDz.x 	_LdotN,_sce_one,_sce_innerPtemp


	
	ADDw.x	_lightmBF, _LdotN, vf00						; lightm backface flag
	FTOI0.x	_lightmBF, _lightmBF
	ITOF0.x	_lightmBF, _lightmBF
        MINIw.x	_lightmBF, _lightmBF, vf00					; L.N > 0 : BF=1 , L.N < 0 : BF=0

	mMulVecByMat	_sce_ST, _sce_vrt, _localLightm					; lightmST = LightmMatrix * vertex
	addw.z		_sce_ST,vf00,vf00						; STz = 1.0


	fwRTP "_sce_prim_XYZF2" 

	;projection/perspective correction
	mulq.xyz _sce_ST,_sce_ST,Q
	maxx.w   _sce_ST,_sce_ST,vf00
	mulw.z 	_sce_ST,_sce_ST,_sce_ST	
	MULx.xyz	_sce_ST, _sce_ST, _lightmBF					; if BF=0 then ST=0
	SQ.xyz 	_sce_ST,_sce_prim_ST(_sce_out_p)

	fwClip "TRIANGLE_TRIANGLESTRIP_FAN"
	fwLoadFanSwitch 118
	fwLoadFirstPoint 119
	fwSwitchOldPointFirstPoint
	fwBFC
	fwWriteADC "_sce_prim_XYZF2"
 	SQ 		write_color, _sce_prim_RGBAQ(_sce_out_p)

; increment pointers, decrement counter

  	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, LIGHTMSTQC_vertex_loop   
;**************************************************************************************

LIGHTMSTQC_done:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,LIGHTMSTQC_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		LIGHTMSTQC_START        
	












; -------------------------------------------------------------------

.END











