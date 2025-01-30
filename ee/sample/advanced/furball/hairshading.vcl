   	.include "utils.i"
	.include "math.i
	.include "fwLights.i"   ; for lightning macros
	.include "fw.i"		; for basic  macros

.macro myClip 
 
         ;screen-space
         sub.xy         _sce_c_ver,_sce_screenVer,_sce_clip_offsets                     ; centre X,Y
         subw.z         _sce_c_ver,_sce_clip_offsets,_sce_screenVer                     ; center
         mul.xy         _sce_c_ver,_sce_c_ver,_sce_clip_scale
         clipw.xyz      _sce_c_ver,_sce_clip_offsets
 
         fcand         vi01,0xfff
.endm
.macro  myDirLight diffout
         MULAx          ACC,_sce_dirNLightVect[0],_sce_normalx                          ; dot products
         MADDAy         ACC,_sce_dirNLightVect[1],_sce_normaly
         MADDz          _sce_colorCoef,_sce_dirNLightVect[2], _sce_normalz

         MAXx                 _sce_colorCoef,_sce_colorCoef,VF00                              ; cut under 0.0f

         MULAx.xyz    ACC,_sce_dirLightColor[0],_sce_colorCoefx
         MADDAy.xyz   ACC,_sce_dirLightColor[1],_sce_colorCoefy
         MADDAz.xyz   ACC,_sce_dirLightColor[2],_sce_colorCoefz
         MADDw.xyz    \diffout,_sce_lightAmbientColor,VF00w   ;ambient
.endm

.macro myRTP prim type="default"
;writes storeVert to xyz for "default" and xyzw for "basic"
          ;default
          mulax         ACC,_sce_worldScreen[0],_sce_vrt
          madday        ACC,_sce_worldScreen[1],_sce_vrt
          maddaz        ACC,_sce_worldScreen[2],_sce_vrt
          maddw         _sce_screenVer,_sce_worldScreen[3],vf00
         div            q,vf00w,_sce_screenVerw
         MULq.xyz       _sce_screenVer,_sce_screenVer,Q
         FTOI4.xzy      _sce_storeVer, _sce_screenVer
         .AIF "\type" EQ "basic"
           ;basic
           SQ           _sce_storeVer,\prim(_sce_out_p)                         ; XYZF2,3
         .AELSE
           ;default
           SQ.xyz       _sce_storeVer,\prim(_sce_out_p);                        ; XYZF2,3
         .AENDI
.endm                  

	.init_vf_all
	.init_vi_all
	
	.name hairshading 

        --enter								; entry point for vcl
	--endenter   				
	
;**********Micro Init-Block *****************

        lq _sce_worldScreen[0],16(VI00);16world_screen
        lq _sce_worldScreen[1],17(VI00);17
        lq _sce_worldScreen[2],18(VI00);18
        lq _sce_worldScreen[3],19(VI00);19 
         
        lq translate, 3(VI00) 

        LQ _sce_dirNLightVect[0],32(VI00)
        LQ _sce_dirNLightVect[1],33(VI00)
        LQ _sce_dirNLightVect[2],34(VI00)   

	fwInitClip 
        fcset 0      
;********************************************


	--cont
 	--cont					 


;******************* Buffer Init-Block *********
PARTICLEC_START:
	XTOP 		_sce_in_p
	ILW.x 		_sce_packsize, 10(VI00)		; inbuf size
	IADD   	 	_sce_pXkick,_sce_in_p,_sce_packsize
	IADDIU 	 	_sce_out_p,_sce_pXkick,0
;***********************************************

;******************** GifTag Init-Block ********
PARTICLEC_GIFTAG:

	fwInitGifTag
	fwSetVertexCounter
	fwLoadLightColor  \&DIR_LIGHT_COLOR0,\&DIR_LIGHT_COLOR1,\&DIR_LIGHT_COLOR2,_sce_dirLightColor
	fwLoadLightAmbientColor \&AMBIENT_LIGHT_COLOR 

	sub.w	_sce_storeVer,vf00,vf00			; screenVer.w=0		
;*************************************************************

;******************** Vertex Loop-Block ***********************
PARTICLEC_vertex_loop:

     	--LoopCS	1,1


	lq 		_sce_vrt,_sce_vert_XYZ(_sce_in_p)				; load vertex
   	lq.xyz		_sce_ST,_sce_vert_ST(_sce_in_p)       				; load st
	lq 		_sce_vColor,_sce_vert_RGBA(_sce_in_p)				; load color

        sub.xyzw        normal,_sce_vrt,translate  
        getRcpLengP_x   normal,length
        MULx.xyz         _sce_normal,normal,length
        
	MOVE.w _sce_alphaC,_sce_vColor
	myRTP "_sce_prim_XYZF2" "basic"
	fwST "_sce_prim_ST"
	myClip
	iaddiu	_sce_adcFlag,vi01,0x7fff
	fwWriteADC "_sce_prim_XYZF2"
	myDirLight _sce_diffFact
	fwCalcColorSum "SCE_VERTEXC","SCE_CLAMP",_sce_vColor,_sce_diffFact,_sce_vColor  ;use _sce_vColor as output to save alpha
	
	FTOI0.xyz 	     _sce_vColor, _sce_vColor
	FTOI0.w 	     _sce_alphaC,_sce_alphaC
	SQ.xyz _sce_vColor,_sce_prim_RGBAQ(_sce_out_p)
	SQ.w   _sce_alphaC,_sce_prim_RGBAQ(_sce_out_p)

; increment pointers, decrement counter

  	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, PARTICLEC_vertex_loop   
;**************************************************************************************

PARTICLEC_done:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,PARTICLEC_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		PARTICLEC_START        
	












; -------------------------------------------------------------------

.END











