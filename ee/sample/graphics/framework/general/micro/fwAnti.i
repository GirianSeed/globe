;macros for framwork anti-AA1

; anti-pirmitiv structure

	struct	_sce_aa_prim 			; aa framework primitive
	vud	_sce_aprim_PRMOD
	vud	_sce_aprim_ST
	vud	_sce_aprim_RGBAQ
	vud	_sce_aprim_XYZF2
	endst	_sce_aprim_sizeof

.macro fwInitAnti

	iaddiu		_sce_prmod_Addr,VI00,0x001b 	; address value
	iaddiu		_sce_minusMask2,vi00,0x80	; -x test
	
.endm


.macro fwAnti prim
	sub.xyz		_sce_nDelta,_sce_vrt,_sce_oVer

	OPMULA.xyz 	ACC,_sce_oDelta,_sce_nDelta      
	OPMSUB.xyz 	_sce_polygonNormal,_sce_nDelta,_sce_oDelta 		;outer prod.

	MULy.xyz 	_sce_oDelta,_sce_nDelta,_sce_fanSwitch	
	MOVE.xyz 	_sce_oVer,_sce_vrt		

	getRcpLengP_z	_sce_polygonNormal,_sce_oneOverPNLength

	MULz.xyz 	_sce_polygonNormal,_sce_polygonNormal,_sce_oneOverPNLength	
	SUB.xyz 	_sce_cameraDist,_sce_vrt,_sce_antiParam

	getRcpLengQ_x	_sce_cameraDist,_sce_oneOverCdist

	MUL.xyz 	_sce_angle,_sce_cameraDist,_sce_polygonNormal
	MULAx.x	 	ACC,_sce_oneOverCdist,_sce_angle
	MADDAy.x 	ACC,_sce_oneOverCdist,_sce_angle
	MADDz.x 	_sce_angle,_sce_oneOverCdist,_sce_angle
	ABS.x 		_sce_angle,_sce_angle				; absolute value
	SUBw.x 		_sce_angle,_sce_angle,_sce_antiParam  	       		; antiParam.w is threshold value
	FMAND 		_sce_resultFlag, _sce_minusMask2	     		; 0x80 is -x.. so angle<threshold
	isub		_sce_resultFlag,_sce_minusMask2,_sce_resultFlag	; reverse it , now 0x80 iff angle>threshold
	ior		_sce_prmod_Data,_sce_attribute,_sce_resultFlag		; now has AA1 set
	isw.z 		_sce_prmod_Addr,\prim(_sce_out_p)		; set ADDRESS = prmod AD = 0x1b
	isw.x 		_sce_prmod_Data,\prim(_sce_out_p) 		; set DATA = prmod value
.endm



.macro fwAntiBFC prmod
	sub.xyz		_sce_nDelta,_sce_vrt,_sce_oVer

	OPMULA.xyz 	ACC,_sce_oDelta,_sce_nDelta      
	OPMSUB.xyz 	_sce_polygonNormal,_sce_nDelta,_sce_oDelta 		;outer prod.

	MULy.xyz 	_sce_oDelta,_sce_nDelta,_sce_fanSwitch
	MOVE.xyz 	_sce_oVer,_sce_vrt		

	getRcpLengP_z	_sce_polygonNormal,_sce_oneOverPNLength

	MULz.xyz 	_sce_polygonNormal,_sce_polygonNormal,_sce_oneOverPNLength	
	SUB.xyz 	_sce_cameraDist,_sce_antiParam,_sce_vrt

	getRcpLengQ_x	_sce_cameraDist,_sce_oneOverCdist

	MUL.xyz 	_sce_angle,_sce_cameraDist,_sce_polygonNormal
	MULAx.x	 	ACC,_sce_oneOverCdist,_sce_angle
	MADDAy.x 	ACC,_sce_oneOverCdist,_sce_angle
	MADDz.x 	_sce_angle,_sce_oneOverCdist,_sce_angle


        fmand           _sce_bfcBit,_sce_minusMask2
	isub		_sce_bfcBit,_sce_bfcBit,_sce_bfcSwitch 
	iand		_sce_bfcBit,_sce_bfcBit,_sce_minusMask2
        iadd            _sce_isClip,_sce_bfcBit,vi01
        iaddiu          _sce_adcFlag,_sce_isClip,0x7fff
  

	ABS.x 		_sce_angle,_sce_angle				; absolute value
	SUBw.x 		_sce_angle,_sce_angle,_sce_antiParam  	       		; antiParam.w is threshold value
	FMAND 		_sce_resultFlag, _sce_minusMask2	     		; 0x80 is -x.. so angle<threshold
	isub		_sce_resultFlag,_sce_minusMask2,_sce_resultFlag	; reverse it , now 0x80 iff angle>threshold
	ior		_sce_prmod_Data,_sce_attribute,_sce_resultFlag		; now has AA1 set
	isw.z 		_sce_prmod_Addr,\prmod(_sce_out_p)		; set ADDRESS = prmod AD = 0x1b
	isw.x 		_sce_prmod_Data,\prmod(_sce_out_p) 		; set DATA = prmod value
	iaddiu		_sce_bfcSwitch,_sce_bfcSwitch,0x20
	iand		_sce_bfcSwitch,_sce_bfcSwitch,_sce_bfcMask
.endm
