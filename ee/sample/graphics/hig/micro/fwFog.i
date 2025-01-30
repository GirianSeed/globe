; macros for framework-fog

.macro fwInitFog
	iaddiu		_sce_adcMask,vi00,0x7fff
	iaddiu		_sce_adcMask,_sce_adcMask,1		; 0x8000
.endm


.macro fwFog 
;fog calculation
      	 LQ _sce_fogConst,29(VI00)
         SUBy.z _sce_fogEnd,_sce_screenVer,_sce_fogConst;
     	 MULx.z _sce_fogConst,_sce_fogEnd,_sce_fogConst;	 
         MAXx.z _sce_fogConst,_sce_fogConst,vf00
         MINIw.z _sce_fogConst,_sce_fogConst,vf00
         LOI 4080			; shift left and mult with 255
         MULi.z _sce_fogConst,_sce_fogConst,I
      	 FTOI0.z _sce_fogConst,_sce_fogConst
      	 MTIR _sce_fogFlag,_sce_fogConstz
.endm

 
.macro fwWriteADC_Fog prim
	IAND _sce_adcFlag,_sce_adcFlag,_sce_adcMask
	IOR _sce_adcFog,_sce_adcFlag,_sce_fogFlag
	ISW.w _sce_adcFog,\prim(_sce_out_p)  
.endm
