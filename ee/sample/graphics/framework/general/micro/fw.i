; framework vertex structure

	struct	_sce_fwVertex  		; framework input vertex
	vud	_sce_vert_XYZ 
	vud	_sce_vert_nXYZ
	vud	_sce_vert_ST
	vud	_sce_vert_RGBA
	endst	_sce_vert_sizeof


	struct	_sce_prim 			; framework primitive
	vud	_sce_prim_ST
	vud	_sce_prim_RGBAQ
	vud	_sce_prim_XYZF2
	endst	_sce_prim_sizeof


.macro	fwLocalScreen
	sub		_sce_oldDelta,vf00,vf00 	;init: removed by vcl if not used
	sub		_sce_oldPoint,vf00,vf00
	sub		_sce_oDelta,vf00,vf00
   	sub		_sce_oVer,vf00,vf00
	fcset 0

	lq _sce_localWorld[0], 0(VI00)					;0 local_world
	lq _sce_localWorld[1], 1(VI00)					;1
 	lq _sce_localWorld[2], 2(VI00)
	lq _sce_localWorld[3], 3(VI00)

        lq _sce_worldScreen[0],16(VI00);16world_screen
        lq _sce_worldScreen[1],17(VI00);17
        lq _sce_worldScreen[2],18(VI00);18
        lq _sce_worldScreen[3],19(VI00);19

	mMulVecByMat _sce_localScreen[0],_sce_localWorld[0],_sce_worldScreen
	mMulVecByMat _sce_localScreen[1],_sce_localWorld[1],_sce_worldScreen
	mMulVecByMat _sce_localScreen[2],_sce_localWorld[2],_sce_worldScreen
	mMulVecByMat _sce_localScreen[3],_sce_localWorld[3],_sce_worldScreen

.endm

.macro fwLocalView
	lq _sce_worldView[0], 20(VI00)				
	lq _sce_worldView[1], 21(VI00)					
 	lq _sce_worldView[2], 22(VI00)
	lq _sce_worldView[3], 23(VI00)

	mMulVecByMat _sce_localView[0],_sce_localWorld[0],_sce_worldView
	mMulVecByMat _sce_localView[1],_sce_localWorld[1],_sce_worldView
	mMulVecByMat _sce_localView[2],_sce_localWorld[2],_sce_worldView
	mMulVecByMat _sce_localView[3],_sce_localWorld[3],_sce_worldView

.endm

.macro fwRTP prim type="default"
;writes storeVert to xyz for "default" and xyzw for "basic"
 	 mulax		ACC,_sce_localScreen[0],_sce_vrt
	 madday		ACC,_sce_localScreen[1],_sce_vrt
	 maddaz		ACC,_sce_localScreen[2],_sce_vrt
	 maddw		_sce_screenVer,_sce_localScreen[3],vf00

	 div 		q,vf00w,_sce_screenVerw
         MULq.xyz	_sce_screenVer,_sce_screenVer,Q
       	 FTOI4.xzy	_sce_storeVer, _sce_screenVer  
	 .AIF "\type" EQ "basic"
	   ;basic		
	   SQ		_sce_storeVer,\prim(_sce_out_p) 			; XYZF2,3
	 .AELSE
	   ;default
       	   SQ.xyz 	_sce_storeVer,\prim(_sce_out_p);			; XYZF2,3
	 .AENDI
.endm


.macro fwST prim
	;check if Q has the right value!!
  	 MULq.xyz 	_sce_ST,_sce_ST,Q		
  	 SQ.xyz		_sce_ST,\prim(_sce_out_p)
.endm


.macro fwInitClip 
	 loi		2048.0			
	 addi.xy		_sce_clip_offsets,vf00,i 		; clip_offsets.xy = GS centre for recenter operation
	 LQ.wz _sce_clip_offsets,30(VI00)
	 LQ.xy _sce_clip_scale,30(VI00)
.endm

.macro fwClip 
	 ;screen-space
	 sub.xy		_sce_c_ver,_sce_screenVer,_sce_clip_offsets			; centre X,Y
	 subw.z		_sce_c_ver,_sce_clip_offsets,_sce_screenVer			; center
	 mul.xy		_sce_c_ver,_sce_c_ver,_sce_clip_scale
	 clipw.xyz	_sce_c_ver,_sce_clip_offsets
	 fcand		vi01,0x003ffff
.endm

.macro fwBFC param="default"
	.AIF "\param" EQ "local"
		iaddiu		_sce_minusMask2,vi00,0x80                      ; calculate innerprod\
		sub.xyz		_sce_nDelta,_sce_vrt,_sce_oVer		       ; eyeVect*polygonNormal
		OPMULA.xyz 	ACC,_sce_oDelta,_sce_nDelta      
		OPMSUB.xyz 	_sce_polygonNormal,_sce_nDelta,_sce_oDelta 		;outer prod.
		MULy.xyz 	_sce_oDelta,_sce_nDelta,_sce_fanSwitch
		MOVE.xyz 	_sce_oVer,_sce_vrt		
		SUB.xyz 	_sce_cameraDist,_sce_localCamera,_sce_vrt
		MUL.xyz   _sce_angle,_sce_cameraDist,_sce_polygonNormal
		rinit 	R,vf00x  						;get 1 in _sce_one[xyz]
		rget	_sce_one,R
		MULAx.x         ACC,_sce_one,_sce_angle
		MADDAy.x        ACC,_sce_one,_sce_angle
		MADDz.x		_sce_angle,_sce_one,_sce_angle
        	fmand           _sce_bfcBit,_sce_minusMask2
		isub		_sce_bfcBit,_sce_bfcBit,_sce_bfcSwitch 
		iand		_sce_bfcBit,_sce_bfcBit,_sce_minusMask2
        	iadd            _sce_isClip,_sce_bfcBit,vi01
        	iaddiu          _sce_adcFlag,_sce_isClip,0x7fff
		iaddiu		_sce_bfcSwitch,_sce_bfcSwitch,0x20		
		iand		_sce_bfcSwitch,_sce_bfcSwitch,_sce_bfcMask
	.AELSE
		iaddiu 		_sce_minusMask, VI00, 0x0020			; calculate
		sub.xyz		_sce_newDelta,_sce_screenVer,_sce_oldPoint	; ploygonNormal 
		opmula.xyz	ACC,_sce_newDelta,_sce_oldDelta			; in screen_space
		opmsub.xyz	_sce_bfcDir,_sce_oldDelta,_sce_newDelta
		fmand 		_sce_bfcBit,_sce_minusMask
		isub 		_sce_bfcBit,_sce_bfcBit,_sce_bfcSwitch
		iand		_sce_bfcBit,_sce_bfcBit,_sce_minusMask
		iadd 		_sce_isClip,_sce_bfcBit,vi01	
		iaddiu		_sce_adcFlag,_sce_isClip,0x7fff
		move.xyz	_sce_oldPoint,_sce_screenVer				; point->oldPoint	
		muly.xyz	_sce_oldDelta,_sce_newDelta,_sce_fanSwitch  			; newDelta->-oldDelta for triangle/strip(fanSwitch.y=-1) but +oldDelta for fan(fanSwitch.y=1)
		iaddiu		_sce_bfcSwitch,_sce_bfcSwitch,0x20
		iand		_sce_bfcSwitch,_sce_bfcSwitch,_sce_bfcMask
	.AENDI
.endm

.macro fwWriteADC prim
	ISW.w _sce_adcFlag,\prim(_sce_out_p)
.endm

.macro fwInitFan param="default
	addw.z   _sce_fanSwitch,vf00,vf00
	subw.w   _sce_fanSwitch,vf00,vf00
	addw.y   _sce_fanSwitch,vf00,vf00
	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex
	.AIF "\param" EQ"local" 
	move.xyz	_sce_firstPoint,_sce_vrt
	.AELSE
	mulax		ACC,_sce_localScreen[0],_sce_vrt
	madday		ACC,_sce_localScreen[1],_sce_vrt
	maddaz		ACC,_sce_localScreen[2],_sce_vrt
	maddw		_sce_firstPoint,_sce_localScreen[3],vf00
	div 		q,vf00w,_sce_firstPointw
        MULq.xyz	_sce_firstPoint,_sce_firstPoint,Q
	.AENDI
.endm

.macro fwInitTriangleTriangleStrip
	move.zw	 _sce_fanSwitch,vf00
	subw.y   _sce_fanSwitch,vf00,vf00
.endm

.macro fwStoreFirstPoint address
	sq.xyz _sce_firstPoint,\address(VI00)
.endm

.macro fwLoadFirstPoint address
	lq.xyz _sce_firstPoint,\address(VI00)
.endm

.macro fwStoreFanSwitch address
	sq.yzw   _sce_fanSwitch,\address(VI00)
.endm	

.macro fwLoadFanSwitch address
	lq.yzw 	 _sce_fanSwitch,\address(VI00)
.endm

.macro fwSwitchOldPointFirstPoint param="default"
	.AIF "\param" EQ"local" 
	MULAw.xyz ACC,_sce_oVer,_sce_fanSwitch
	MADDz.xyz _sce_oVer,_sce_firstPoint,_sce_fanSwitch 
	.AELSE
	MULAw.xyz ACC,_sce_oldPoint,_sce_fanSwitch	; oldPoint=A*oldPoint+B*firstPoint (A..fanSwitch.w,B..fanSwitch.z)
	MADDz.xyz _sce_oldPoint,_sce_firstPoint,_sce_fanSwitch ; triangle/strip: A=1,B=0 fan: A=0,B=1
 	.AENDI
.endm

.macro fwInitGifTag
        LQ      _sce_gifTag1, 8(VI00)        ; load giftag for PRIM/PRMOD
        ILW.y   _sce_primitive, 0(_sce_in_p)      ; load primitive by shape
        ILW.z   _sce_attrMicro, 29(VI00)     ; load attribute by micro
        ILW.x   _sce_attrShape, 1(_sce_in_p)      ; load attribute by shape
        IOR     _sce_attribute, _sce_attrMicro, _sce_attrShape ; make attribute
        LQ      _sce_gifTag2, 9(VI00)        ; load giftag for Vertex
        ILW.x   _sce_nloop, 0(_sce_in_p)          ; load eop/nloop
        LQ      _sce_gifTag, 0(_sce_in_p)         ; for eop check 
        mfir    _sce_clearF,vi00             ; 0 clear qword
        IADDIU  _sce_prmod, VI00, 0x1b       ; prmod register = 0x1b
 
        SQ      _sce_gifTag1, 0(_sce_out_p)
        SQ.yzw  _sce_clearF, 1(_sce_out_p)
        SQ.yzw  _sce_clearF, 2(_sce_out_p)
        ISW.x   _sce_primitive, 1(_sce_out_p)
        ISW.x   _sce_attribute, 2(_sce_out_p)
        ISW.z   _sce_prmod, 2(_sce_out_p)
        SQ.yzw  _sce_gifTag2, 3(_sce_out_p)
        ISW.x   _sce_nloop, 3(_sce_out_p)
 
        IADDIU  _sce_in_p, _sce_in_p, 2
        IADDIU  _sce_out_p, _sce_out_p, 4
	iaddiu	_sce_primMask,VI00,0x0007		
        IADDI   _sce_triangleMask, VI00, 0x0003
	iaddiu	_sce_triangleStripMask,VI00,0x0004
        IAND    _sce_prim, _sce_primitive, _sce_primMask
.endm

.macro fwSetVertexCounter
	iaddiu		_sce_mask,vi00,0x7fff
        IAND    _sce_nVerts, _sce_nloop, _sce_mask   
.endm

.macro fwLocalCamera 
	LQ.xyz _sce_worldCamera,28(VI00)
	mMakeLocal _sce_localCamera,_sce_worldCamera,_sce_worldLocal
.endm

.macro fwStoreLocalCamera address
	 SQ.xyz _sce_worldCamera,\address(VI00)
.endm

.macro fwLoadLocalCamera address
	 LQ.xyz _sce_worldCamera,\address(VI00)
.endm


