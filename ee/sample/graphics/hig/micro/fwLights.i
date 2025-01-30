; mem-layout for fwlights

AMBIENT_LIGHT_COLOR    .assigna 43

DIR_LIGHT_COLOR0  .assigna 40
DIR_LIGHT_COLOR1  .assigna 41
DIR_LIGHT_COLOR2  .assigna 42
DIR_LIGHT_VECT0   .assigna 32
DIR_LIGHT_VECT1   .assigna 33
DIR_LIGHT_VECT2   .assigna 34

POINT_LIGHT_COLOR0  .assigna 52
POINT_LIGHT_COLOR1  .assigna 53
POINT_LIGHT_COLOR2  .assigna 54
POINT_LIGHT_POS0    .assigna 48
POINT_LIGHT_POS1    .assigna 49
POINT_LIGHT_POS2    .assigna 50


SPOT_LIGHT_COLOR0  .assigna 64
SPOT_LIGHT_COLOR1  .assigna 65
SPOT_LIGHT_COLOR2  .assigna 66
SPOT_LIGHT_VECT0   .assigna 56
SPOT_LIGHT_VECT1   .assigna 57
SPOT_LIGHT_VECT2   .assigna 58
SPOT_LIGHT_POS0    .assigna 60
SPOT_LIGHT_POS1    .assigna 61
SPOT_LIGHT_POS2    .assigna 62

MATERIAL_DIFFUSE   .assigna 24
MATERIAL_SPECULAR  .assigna 25
MATERIAL_EMISSION  .assigna 26
MATERIAL_AMBIENT   .assigna 27

.macro fwCalcNLightVect l1, l2, l3,light_type
	LQ _sce_lightVect[0],\l1(VI00)
	LQ _sce_lightVect[1],\l2(VI00)
	LQ _sce_lightVect[2],\l3(VI00)
	MOVE _sce_lightVect[3],vf00

	.AIF "\light_type" EQ "_sce_spotNLightVect"
	  ; invert light_vectors 
	  SUB.xyz _sce_lightVect[0],vf00,_sce_lightVect[0]
	  SUB.xyz _sce_lightVect[1],vf00,_sce_lightVect[1]
	  SUB.xyz _sce_lightVect[2],vf00,_sce_lightVect[2]
	.AENDI

	LQ _sce_lightRot[0],4(VI00)
	LQ _sce_lightRot[1],5(VI00)
	LQ _sce_lightRot[2],6(VI00)


	mMulVecByMat \light_type[0],_sce_lightRot[0],_sce_lightVect
	mMulVecByMat \light_type[1],_sce_lightRot[1],_sce_lightVect
	mMulVecByMat \light_type[2],_sce_lightRot[2],_sce_lightVect
.endm

.macro fwLoadNLightVect l1,l2,l3,light_type
	LQ \light_type[0],\l1(VI00)
	LQ \light_type[1],\l2(VI00)
	LQ \light_type[2],\l3(VI00)
.endm

.macro fwStoreNLightVect l1,l2,l3,light_type
	SQ \light_type[0],\l1(VI00)
	SQ \light_type[1],\l2(VI00)
	SQ \light_type[2],\l3(VI00)
.endm

.macro fwCalcLocalLightPos l1,l2,l3,light_type,source
	LQ \source[0],\l1(VI00)
	LQ \source[1],\l2(VI00)
	LQ \source[2],\l3(VI00)
	mLocalLightPos \light_type,\source,_sce_worldLocal
.endm

.macro fwStoreLightPos l1,l2,l3,light_type
	SQ \light_type[0],\l1(VI00)
	SQ \light_type[1],\l2(VI00)
	SQ \light_type[2],\l3(VI00)
.endm

.macro fwLoadLightPos l1,l2,l3,light_type
	LQ \light_type[0],\l1(VI00)
	LQ \light_type[1],\l2(VI00)
	LQ \light_type[2],\l3(VI00)
.endm
	
.macro fwLoadLightColor l1,l2,l3,light_type
	LQ.xyzw 	\light_type[0],\l1(VI00)
	LQ.xyzw 	\light_type[1],\l2(VI00)
	LQ.xyzw 	\light_type[2],\l3(VI00)
.endm

.macro fwStoreLightColor l1,l2,l3,light_type
	LQ.xyz 	\light_type[0],\l1(VI00)
	LQ.xyz 	\light_type[1],\l2(VI00)
	LQ.xyz 	\light_type[2],\l3(VI00)
.endm

.macro fwLoadLightAmbientColor l1
	LQ.xyz 	_sce_lightAmbientColor,\l1(VI00)
.endm

.macro fwStoreLightConst light_type,source,store
	addw.x \light_type, VF00, \source[0]
	addw.y \light_type, VF00, \source[1]
	addw.z \light_type, VF00, \source[2]
	SQ.xyz \light_type,\store(VI00)
.endm

.macro fwStoreLightAngle source,store
	addw.x _sce_spotlightAngle, VF00, \source[0]
	addw.y _sce_spotlightAngle, VF00, \source[1]
	addw.z _sce_spotlightAngle, VF00, \source[2]
	SQ.xyz _sce_spotlightAngle,\store(VI00)
.endm

.macro  fwLoadLightConst load,light_type
	LQ.xyz \light_type,\load(VI00)
.endm

.macro fwLoadLightAngle load,light_type
	LQ.xyz \light_type,\load(VI00)
.endm


.macro 	fwDirLight shading_type,ambient_calc,diffout,specout="def",calcEyeVect="calc"
  	 MULAx		ACC,_sce_dirNLightVect[0],_sce_normalx	    			; dot products
	 MADDAy 	ACC,_sce_dirNLightVect[1],_sce_normaly		 
	 MADDz 		_sce_colorCoef,_sce_dirNLightVect[2], _sce_normalz		 
	.AIF "\shading_type" EQ "SCE_VERTEXC"
		.AIF "\ambient_calc" EQ "SCE_AMBIENT"
	 	   MAXx  		_sce_colorCoef,_sce_colorCoef,VF00       			; cut under 0.0f
	 	   MULAx.xyz 	ACC,_sce_dirLightColor[0],_sce_colorCoefx	
	 	   MADDAy.xyz 	ACC,_sce_dirLightColor[1],_sce_colorCoefy
 		   MADDAz.xyz 	ACC,_sce_dirLightColor[2],_sce_colorCoefz	
	 	   MADDw.xyz 	\diffout,_sce_lightAmbientColor,VF00w	;ambient
		.AELSE
	 	  MAXx  		_sce_colorCoef,_sce_colorCoef,VF00       			; cut under 0.0f
	          MULAx.xyz 	ACC,_sce_dirLightColor[0],_sce_colorCoefx	
	          MADDAy.xyz 	ACC,_sce_dirLightColor[1],_sce_colorCoefy
		  MADDz.xyz 	\diffout,_sce_dirLightColor[2],_sce_colorCoefz
		.AENDI 		       
	.AELSE
	   .AIF "\shading_type" EQ "SCE_PHONG"
		fwSpecularBF    _sce_bfCheck,_sce_colorCoef
		MAXx  		_sce_colorCoef,_sce_colorCoef,VF00       			; cut under 0.0f
		MULAx.xyz 	ACC,_sce_dirLightColor[0],_sce_colorCoefx	
	 	MADDAy.xyz 	ACC,_sce_dirLightColor[1],_sce_colorCoefy
		MADDz.xyz 	\diffout,_sce_dirLightColor[2],_sce_colorCoefz		
	  ;reflection vector calc.	 	 
	 	ADD.xyz		_sce_refVectCoef,_sce_colorCoef,_sce_colorCoef
 	 	MULx.xyz	_sce_refVect1,_sce_normal,_sce_refVectCoef
	 	MULy.xyz	_sce_refVect2,_sce_normal,_sce_refVectCoef
	 	MULz.xyz 	_sce_refVect3,_sce_normal,_sce_refVectCoef
		mTransposeXYZMatrix _sce_dirNLightVect
		SUB.xyz	_sce_refVect1,_sce_refVect1,_sce_dirNLightVect[0]
		SUB.xyz	_sce_refVect2,_sce_refVect2,_sce_dirNLightVect[1]
		SUB.xyz 	_sce_refVect3,_sce_refVect3,_sce_dirNLightVect[2]
	   	 .AIF "\calcEyeVect" EQ "calc"	 
	   		fwGetEyeVectorP  _sce_eyeVect,_sce_localCamera,_sce_vrt
		 .AENDI	 
		 fwGetSpecularCosine _sce_specFact,_sce_eyeVect,_sce_refVect1,_sce_refVect2,_sce_refVect3
		 fwRemoveSpecularBF _sce_specFact,_sce_bfCheck
		 fwShininess _sce_specFact
; now intensity calc	 
	 	 MULAx.xyz 	ACC,_sce_dirLightColor[0], _sce_specFactx	
	 	 MADDAy.xyz 	ACC,_sce_dirLightColor[1],_sce_specFacty	
	 	 MADDz.xyz 	\specout,_sce_dirLightColor[2],_sce_specFactz	
	   .AELSE
	       .AIF "\shading_type" EQ "SCE_LAMBERT"
	 	  MAXx  		_sce_colorCoef,_sce_colorCoef,VF00       			; cut under 0.0f
	 	  MULAx.xyz 	ACC,_sce_dirLightColor[0],_sce_colorCoefx	
	 	  MADDAy.xyz 	ACC,_sce_dirLightColor[1],_sce_colorCoefy
		  MADDz.xyz 	\diffout,_sce_dirLightColor[2],_sce_colorCoefz
	        .AELSE
 		  .AIF "\shading_type" EQ "SCE_BLINN"
		    MAXx  	_sce_colorCoef,_sce_colorCoef,VF00	
		    MULAx.xyz 	ACC,_sce_dirLightColor[0],_sce_colorCoefx	
	 	    MADDAy.xyz 	ACC,_sce_dirLightColor[1],_sce_colorCoefy
		    MADDz.xyz 	\diffout,_sce_dirLightColor[2],_sce_colorCoefz		
	        ;reflection vector calc.	 	 	   
		    .AIF "\calcEyeVect" EQ "calc"	 
	   		fwGetEyeVectorP  _sce_eyeVect,_sce_localCamera,_sce_vrt
		    .AENDI
		    mTransposeXYZMatrix _sce_dirNLightVect
		    ADD.xyz	_sce_refVect1,_sce_eyeVect,_sce_dirNLightVect[0]
		    ADD.xyz	_sce_refVect2,_sce_eyeVect,_sce_dirNLightVect[1]
		    ADD.xyz     _sce_refVect3,_sce_eyeVect,_sce_dirNLightVect[2]
		    fwBlinnCalc _sce_specFact,_sce_refVect1,_sce_refVect2,_sce_refVect3
		; now intensity calc	 
	 	    MULAx.xyz 	ACC,_sce_dirLightColor[0],_sce_specFactx	
	 	    MADDAy.xyz 	ACC,_sce_dirLightColor[1],_sce_specFacty	
	 	    MADDz.xyz 	\specout,_sce_dirLightColor[2],_sce_specFactz
	          .AENDI
	        .AENDI
           .AENDI
	.AENDI
	
.endm


.macro fwBlinnCalc output,refvect1,refvect2,refvect3

	        getRcpLengQ_x	\refvect1,_sce_oneRefOverNorm
	        getRcpLengQ_y	\refvect2,_sce_oneRefOverNorm
       	 	getRcpLengP_z	\refvect3,_sce_oneRefOverNorm

	  	MUL.xyz 	_sce_cosineFact1,\refvect1,_sce_normal
	 	MUL.xyz 	_sce_cosineFact2,\refvect2,_sce_normal
	 	MUL.xyz 	_sce_cosineFact3,\refvect3,_sce_normal

		MULAx.x 	ACC,_sce_oneRefOverNorm,_sce_cosineFact1
		MADDAy.x 	ACC,_sce_oneRefOverNorm,_sce_cosineFact1
	 	MADDz.x 	_sce_specFact,_sce_oneRefOverNorm,_sce_cosineFact1
	 
	 	MULAx.y 	ACC,_sce_oneRefOverNorm,_sce_cosineFact2
	 	MADDAy.y 	ACC,_sce_oneRefOverNorm,_sce_cosineFact2
	 	MADDz.y 	_sce_specFact,_sce_oneRefOverNorm, _sce_cosineFact2	 
	 
	 	MULAx.z 	ACC,_sce_oneRefOverNorm,_sce_cosineFact3
	 	MADDAy.z 	ACC,_sce_oneRefOverNorm,_sce_cosineFact3
	 	MADDz.z 	_sce_specFact,_sce_oneRefOverNorm,_sce_cosineFact3
		fwShininess _sce_specFact
.endm

.macro fwSpecularBF bf,coef 
;backface check 	
	 ADDw.xyz	\bf,\coef,vf00
	 FTOI0.xyz      \bf,\bf
	 ITOF0.xyz	\bf,\bf
         MINIw.xyz      \bf,\bf,vf00 
.endm

.macro fwGetEyeVectorP  eye,camera,vertex
	 SUB.xyz	\eye,\camera,\vertex
       	 getRcpLengP_z	\eye,_sce_oneOverCameraNorm
	 MULz.xyz	\eye,\eye,_sce_oneOverCameraNormz
.endm


.macro fwFishEye	prim,vertex	
	mulax.xyz	ACC,_sce_localView[0],\vertex
	madday.xyz	ACC,_sce_localView[1],\vertex
	maddaz.xyz	ACC,_sce_localView[2],\vertex
	maddw.xyz       _sce_fishVie,_sce_localView[3],\vertex

	getRcpLengQ_z	_sce_fishVie,_sce_oneOverCameraNorm
	MULz.xyz 	_sce_fishVie,_sce_fishVie,_sce_oneOverCameraNormz
	
	SUBw.w		_sce_viewClipTestw,vf00,vf00	
 	CLIPw.xyz	_sce_fishVie,_sce_viewClipTestw
	FCAND		vi01,0x0000020


	; stretch to texture
	MULx.xy _sce_screenVer,_sce_fishVie,_sce_Zoom

	LOI 2048.0
	ADDI.xy	_sce_screenVer,_sce_screenVer,I

	; for z-buffer
        MOVE.z  _sce_screenVer,_sce_oneOverCameraNormz
	MULz.z _sce_screenVer,_sce_screenVer,_sce_fishZfact
	ADDy.z _sce_screenVer,_sce_screenVer,_sce_fishZfact
	
        FTOI4.xyz	_sce_storeFish,_sce_screenVer	
	SQ.xyz 	_sce_storeFish,\prim(_sce_out_p)
	SQ.xyz _sce_ST,_sce_prim_ST(_sce_out_p)	
.endm


.macro fwSphereReflectST  camera,vertex,mode="default"
	fwGetEyeVectorQ   _sce_eyeVect,\vertex,\camera
	rinit 	R,vf00x
	rget	_sce_one,R
	
	MUL.xyz	_sce_innerPtemp,_sce_eyeVect,_sce_normal
	ADDAy.x	ACC,_sce_innerPtemp,_sce_innerPtemp
	MADDz.x 	_sce_innerP,_sce_one,_sce_innerPtemp

	ADDx.x		temp,_sce_innerP,_sce_innerP
	MULx.xyz	_sce_refInd,_sce_normal,temp
	SUB.xyz		_sce_refInd,_sce_refInd,_sce_eyeVect
	 
	mulax.xyz	ACC,_sce_localView[0],_sce_refInd
	madday.xyz	ACC,_sce_localView[1],_sce_refInd
	maddz.xyz	_sce_viewRefInd,_sce_localView[2],_sce_refInd

	.AIF "\mode" EQ "static"	 
	    ADDw.z		_sce_viewRefInd,_sce_viewRefInd,VF00	
	.AELSE	 
	    ADDw.z _sce_viewRefInd,_sce_viewRefInd,_sce_RefZoom
	.AENDI

	MULz.xy  _sce_viewRefInd,_sce_viewRefInd,_sce_RefZoom

	eleng		P,_sce_viewRefInd
	mfp.w		_sce_m,P
	ADDw.w		_sce_m2,_sce_m,_sce_m
	DIV 		Q,vf00w,_sce_m2w
		
	; copy 1 to ST.z to be save
	ADDw.z		_sce_ST,vf00,vf00
	MULQ.xy		_sce_ST,_sce_viewRefInd,Q
	LOI 0.5
	ADDI.xy		_sce_ST,_sce_ST,I
.endm

.macro fwSphereRefractST  camera,vertex,mode="default"
	fwGetEyeVectorQ   _sce_eyeVect,\camera,\vertex
	rinit 	R,vf00x
	rget	_sce_one,R
	
	MUL.xyz	_sce_innerPtemp,_sce_eyeVect,_sce_normal
	ADDAy.x	ACC,_sce_innerPtemp,_sce_innerPtemp
	MADDz.x 	_sce_innerP,_sce_one,_sce_innerPtemp

	ADDx.x		temp,_sce_innerP,_sce_innerP
	; refract coef   	
	MULy.xyz	_sce_normalRefract,_sce_normal,_sce_RefInde

	MULx.xyz	_sce_refInd,_sce_normalRefract,temp
	SUB.xyz		_sce_refInd,_sce_refInd,_sce_eyeVect
	 
	mulax.xyz	ACC,_sce_localView[0],_sce_refInd
	madday.xyz	ACC,_sce_localView[1],_sce_refInd
	maddz.xyz	_sce_viewRefInd,_sce_localView[2],_sce_refInd
	
	.AIF "\mode" EQ "static"	 
	    ADDw.z		_sce_viewRefInd,_sce_viewRefInd,VF00	
	    SUB.y		_sce_viewRefInd,VF00,_sce_viewRefInd	
	.AELSE
	    ADDw.z _sce_viewRefInd,_sce_viewRefInd,_sce_RefZoom
	    SUB.xy  _sce_viewRefInd,vf00,_sce_viewRefInd
	.AENDI
 
	MULz.xy  _sce_viewRefInd,_sce_viewRefInd,_sce_RefZoom

	eleng		P,_sce_viewRefInd
	mfp.w		_sce_m,P
	ADDw.w		_sce_m2,_sce_m,_sce_m
	DIV 		Q,vf00w,_sce_m2w
		
	; copy 1 to ST.z to be save
	ADDw.z		_sce_ST,vf00,vf00
	MULQ.xy		_sce_ST,_sce_viewRefInd,Q
	LOI 0.5
	ADDI.xy		_sce_ST,_sce_ST,I
.endm




.macro fwGetEyeVectorQ  eye,camera,vertex
	 SUB.xyz	\eye,\camera,\vertex
       	 getRcpLengQ_z	\eye,_sce_oneOverCameraNorm
	 MULz.xyz	\eye,\eye,_sce_oneOverCameraNormz
.endm

.macro fwGetSpecularCosine specFact,eye,ref1,ref2,ref3

	 MUL.xyz 	_sce_cosineFact1,\ref1,\eye
	 MUL.xyz 	_sce_cosineFact2,\ref2,\eye
	 MUL.xyz 	_sce_cosineFact3,\ref3,\eye


	 rinit 	R,vf00x
	 rget	_sce_one,R

	 ADDAy.x	ACC,_sce_cosineFact1,_sce_cosineFact1
	 MADDz.x 	\specFact,_sce_one,_sce_cosineFact1
	 
	 ADDAx.y 	ACC, _sce_cosineFact2, _sce_cosineFact2
	 MADDz.y 	\specFact,_sce_one, _sce_cosineFact2	 
	 
	 ADDAy.z 	ACC, _sce_cosineFact3, _sce_cosineFact3
	 MADDx.z 	\specFact,_sce_one,_sce_cosineFact3
.endm

.macro  fwRemoveSpecularBF spec,bf
	 MUL.xyz \spec,\spec,\bf
.endm

.macro  fwShininess spec
; shininess calc	 
	 LQ.w 		_sce_shininess,\&MATERIAL_SPECULAR(VI00)
	 MULw.xyz	_sce_mt,\spec,_sce_shininess
	 SUBw.xyz	_sce_mt,_sce_mt,_sce_shininess
	 SUB.xyz	_sce_temp69,_sce_mt,\spec
 	 DIV		Q,vf00w,_sce_temp69x
	 ADDQ.x		_sce_temp69,vf00,Q
	 DIV 		Q,vf00w,_sce_temp69y
	 ADDQ.y		_sce_temp69,vf00,Q
	 ERCPR		P,_sce_temp69z
	 MFP.z		_sce_temp69,P
	 MUL.xyz	\spec,\spec,_sce_temp69
	 SUB.xyz        \spec,vf00,\spec
	 MAXx.xyz      \spec,\spec,vf00
.endm


.macro fwPointLight  shading_type,ambient_calc,diffout,specout="def",calcEyeVect="calc"

	 SUB.xyz 	_sce_lightVect1,_sce_pointLocalLightPos[0],_sce_vrt
	 SUB.xyz 	_sce_lightVect2,_sce_pointLocalLightPos[1],_sce_vrt
	 SUB.xyz 	_sce_lightVect3,_sce_pointLocalLightPos[2],_sce_vrt

	 getRcpLengQ_x	_sce_lightVect1,_sce_oneOverNorm
	 getRcpLengQ_y	_sce_lightVect2,_sce_oneOverNorm
       	 getRcpLengP_z	_sce_lightVect3,_sce_oneOverNorm	 	 

	.AIF "\shading_type" EQ "SCE_PHONG"
	 	MULx.xyz 	_sce_lightVect1,_sce_lightVect1,_sce_oneOverNorm
	 	MULy.xyz        _sce_lightVect2,_sce_lightVect2,_sce_oneOverNorm
	 	MULz.xyz	_sce_lightVect3,_sce_lightvect3,_sce_oneOverNorm
	 	MUL.xyz 	_sce_normInnerProduct1,_sce_lightVect1,_sce_normal
	 	MUL.xyz 	_sce_normInnerProduct2,_sce_lightVect2,_sce_normal
	 	MUL.xyz 	_sce_normInnerProduct3,_sce_lightVect3,_sce_normal	 	 
		MUL.xyz 	_sce_oneOverNormSq,_sce_oneOverNorm,_sce_oneOverNorm
	 
	 	rinit 	R,vf00x	; get 1.0 in _sce_one[xyzw]
	 	rget	_sce_one,R
	 	ADDAy.x 	ACC, _sce_normInnerProduct1,_sce_normInnerProduct1
	 	MADDz.x 	_sce_refVectCoef,_sce_one,_sce_normInnerProduct1	 
	 
	 	ADDAx.y 	ACC, _sce_normInnerProduct2,_sce_normInnerProduct2
	 	MADDz.y 	_sce_refVectCoef,_sce_one,_sce_normInnerProduct2	 	 

	 	ADDAy.z 	ACC, _sce_normInnerProduct3,_sce_normInnerProduct3
	 	MADDx.z 	_sce_refVectCoef,_sce_one,_sce_normInnerProduct3
	 	MUL.xyz 	_sce_lightCoef,_sce_refVectCoef,_sce_oneOverNormSq
	.AELSE	 
           .AIF "\shading_type" EQ "SCE_BLINN"
	 	MULx.xyz 	_sce_lightVect1,_sce_lightVect1,_sce_oneOverNorm
	 	MULy.xyz        _sce_lightVect2,_sce_lightVect2,_sce_oneOverNorm
	 	MULz.xyz	_sce_lightVect3,_sce_lightvect3,_sce_oneOverNorm
	 	MUL.xyz 	_sce_normInnerProduct1,_sce_lightVect1,_sce_normal
	 	MUL.xyz 	_sce_normInnerProduct2,_sce_lightVect2,_sce_normal
	 	MUL.xyz 	_sce_normInnerProduct3,_sce_lightVect3,_sce_normal	 	 
		MUL.xyz 	_sce_oneOverNormSq,_sce_oneOverNorm,_sce_oneOverNorm	 
	 	rinit 	R,vf00x	; get 1.0 in _sce_one[xyzw]
	 	rget	_sce_one,R

	 	ADDAy.x 	ACC, _sce_normInnerProduct1,_sce_normInnerProduct1
	 	MADDz.x 	_sce_refVectCoef,_sce_one,_sce_normInnerProduct1	 

	 	ADDAx.y 	ACC, _sce_normInnerProduct2,_sce_normInnerProduct2
	 	MADDz.y 	_sce_refVectCoef,_sce_one,_sce_normInnerProduct2
	 	 
	 	ADDAy.z 	ACC, _sce_normInnerProduct3,_sce_normInnerProduct3
	 	MADDz.z 	_sce_refVectCoef,_sce_one,_sce_normInnerProduct3

	 	MUL.xyz 	_sce_lightCoef,_sce_refVectCoef,_sce_oneOverNormSq
           .AELSE           	
	 	MUL.xyz 	_sce_innerProduct1,_sce_lightVect1,_sce_normal
	 	MUL.xyz 	_sce_innerProduct2,_sce_lightVect2,_sce_normal
	 	MUL.xyz 	_sce_innerProduct3,_sce_lightVect3,_sce_normal	 
	 	MULx.xyz 	_sce_normInnerProduct1,_sce_innerProduct1,_sce_oneOverNorm
	 	MULy.xyz 	_sce_normInnerProduct2,_sce_innerProduct2,_sce_oneOverNorm
	 	MULz.xyz 	_sce_normInnerProduct3,_sce_innerProduct3,_sce_oneOverNorm
		MUL.xyz 	_sce_oneOverNormSq,_sce_oneOverNorm,_sce_oneOverNorm
	 	MULAx.x 	ACC, _sce_oneOverNormSq, _sce_normInnerProduct1
	 	MADDAy.x 	ACC, _sce_oneOverNormSq, _sce_normInnerProduct1
	 	MADDz.x 	_sce_lightCoef,_sce_oneOverNormSq,_sce_normInnerProduct1
		MULAx.y 	ACC, _sce_oneOverNormSq, _sce_normInnerProduct2
	 	MADDAy.y 	ACC, _sce_oneOverNormSq,_sce_normInnerProduct2
	 	MADDz.y 	_sce_lightCoef,_sce_oneOverNormSq,_sce_normInnerProduct2	 	 
		MULAx.z 	ACC, _sce_oneOverNormSq, _sce_normInnerProduct3
	 	MADDAy.z 	ACC, _sce_oneOverNormSq,_sce_normInnerProduct3
	 	MADDz.z 	_sce_lightCoef,_sce_oneOverNormSq,_sce_normInnerProduct3
           .AENDI
	.AENDI

	 MUL.xyz 	_sce_lightCoef,_sce_lightCoef,_sce_pointLightConst ; 3 different lights!
	 MAXx.xyz 	_sce_lightCoef,_sce_lightCoef,VF00	 
	 MULAx.xyz 	ACC,_sce_pointLightColor[0],_sce_lightCoefx	
	 MADDAy.xyz 	ACC,_sce_pointLightColor[1],_sce_lightCoefy	
	.AIF "\shading_type" EQ "SCE_VERTEXC"	 	 
	      .AIF "\ambient_calc" EQ "SCE_AMBIENT"
		MADDAz.xyz 	ACC,_sce_pointLightColor[2],_sce_lightCoefz	
	 	MADDw.xyz	\diffout, _sce_lightAmbientColor, VF00w ;ambient
	      .AELSE
		MADDz.xyz 	\diffout,_sce_pointLightColor[2],_sce_lightCoefz
	      .AENDI
	.AELSE
           .AIF  "\shading_type" EQ "SCE_PHONG"
		MADDz.xyz 	\diffout,_sce_pointLightColor[2],_sce_lightCoefz
	 	fwSpecularBF 	_sce_bfCheck,_sce_refVectCoef 
;reflection vector calc
	 	ADD.xyz		_sce_refVectCoef,_sce_refVectCoef,_sce_refVectCoef
 	 	MULx.xyz	_sce_refVect1,_sce_normal,_sce_refVectCoef
	 	MULy.xyz	_sce_refVect2,_sce_normal,_sce_refVectCoef
	 	MULz.xyz 	_sce_refVect3,_sce_normal,_sce_refVectCoef
	 	SUB.xyz		_sce_refVect1,_sce_refVect1,_sce_lightVect1
	 	SUB.xyz		_sce_refVect2,_sce_refVect2,_sce_lightVect2
	 	SUB.xyz 	_sce_refVect3,_sce_refVect3,_sce_lightVect3
		.AIF "\calcEyeVect" EQ "calc"	 
	   		fwGetEyeVectorP  _sce_eyeVect,_sce_localCamera,_sce_vrt
		.AENDI	 
		fwGetSpecularCosine _sce_specFact,_sce_eyeVect,_sce_refVect1,_sce_refVect2,_sce_refVect3
		fwRemoveSpecularBF _sce_specFact,_sce_bfCheck
		fwShininess 	_sce_specFact
; now intensity calc
	 	MUL.xyz 	_sce_specFact,_sce_specFact,_sce_pointLightConst ; 3 different lights!
	 	MUL.xyz         _sce_specFact,_sce_specFact,_sce_oneOverNormSq	 
	 	MULAx.xyz 	ACC,_sce_pointLightColor[0], _sce_specFactx	
	 	MADDAy.xyz 	ACC,_sce_pointLightColor[1],_sce_specFacty	
	 	MADDz.xyz 	\specout,_sce_pointLightColor[2],_sce_specFactz	
	   .AELSE
	       .AIF "\shading_type" EQ "SCE_LAMBERT"
		  MADDz.xyz 	\diffout,_sce_pointLightColor[2],_sce_lightCoefz
	       .AELSE
		  .AIF "\shading_type" EQ "SCE_BLINN"
		    MADDz.xyz 	\diffout,_sce_pointLightColor[2],_sce_lightCoefz		
	        ;reflection vector calc.	 	 	   
		    .AIF "\calcEyeVect" EQ "calc"	 
	   		fwGetEyeVectorP  _sce_eyeVect,_sce_localCamera,_sce_vrt
		    .AENDI
		    ADD.xyz	_sce_refVect1,_sce_eyeVect,_sce_lightVect1
		    ADD.xyz	_sce_refVect2,_sce_eyeVect,_sce_lightVect2
		    ADD.xyz     _sce_refVect3,_sce_eyeVect,_sce_lightVect3
		    fwBlinnCalc _sce_specFact,_sce_refVect1,_sce_refVect2,_sce_refVect3
     	 	    MUL.xyz 	_sce_specFact,_sce_specFact,_sce_pointLightConst ; 3 different lights!
	 	    MUL.xyz         _sce_specFact,_sce_specFact,_sce_oneOverNormSq	 
	 	    MULAx.xyz 	ACC,_sce_pointLightColor[0], _sce_specFactx	
	 	    MADDAy.xyz 	ACC,_sce_pointLightColor[1],_sce_specFacty	
	 	    MADDz.xyz 	\specout,_sce_pointLightColor[2],_sce_specFactz	
	          .AENDI
               .AENDI
	   .AENDI		
	.AENDI
.endm

.macro fwSpotLight shading_type,ambient_calc,diffout,specout="def",calcEyeVect="calc"

	SUB.xyz _sce_lightVect[0],_sce_spotLocalLightPos[0],_sce_vrt
	SUB.xyz _sce_lightVect[1],_sce_spotLocalLightPos[1],_sce_vrt
	SUB.xyz _sce_lightVect[2],_sce_spotLocalLightPos[2],_sce_vrt


	.AIF "\shading_type" EQ "SCE_PHONG"	 
		getRcpLengQ_x	_sce_lightVect[0],_sce_oneOverNorm
		getRcpLengQ_y	_sce_lightVect[1],_sce_oneOverNorm
       		getRcpLengP_z	_sce_lightVect[2],_sce_oneOverNorm	 	 
	
		MUL.xyz 	_sce_oneOverNormSq,_sce_oneOverNorm,_sce_oneOverNorm
	.AENDI

	.AIF "\shading_type" EQ "SCE_BLINN"	 
		getRcpLengQ_x	_sce_lightVect[0],_sce_oneOverNorm
		getRcpLengQ_y	_sce_lightVect[1],_sce_oneOverNorm
       		getRcpLengP_z	_sce_lightVect[2],_sce_oneOverNorm	 	 
	
		MUL.xyz 	_sce_oneOverNormSq,_sce_oneOverNorm,_sce_oneOverNorm
	.AENDI

	.AIF "\shading_type" EQ "SCE_VERTEXC"
		getSaddQ_x _sce_lightVect[0],_sce_oneOverNormSq
		getSaddQ_y _sce_lightVect[1],_sce_oneOverNormSq
		getSaddP_z _sce_lightVect[2],_sce_oneOverNormSq
	.AENDI
	
	.AIF "\shading_type" EQ "SCE_LAMBERT"
		getSaddQ_x _sce_lightVect[0],_sce_oneOverNormSq
		getSaddQ_y _sce_lightVect[1],_sce_oneOverNormSq
		getSaddP_z _sce_lightVect[2],_sce_oneOverNormSq
	.AENDI	

	
	mTransposeXYZMatrix _sce_lightVect
	
	MULAx.x ACC,_sce_lightVect[0],_sce_spotNlightVect[0]
	MADDAx.x ACC,_sce_lightVect[1],_sce_spotNlightVect[1]
	MADDx.x _sce_innerProduct,_sce_lightVect[2],_sce_spotNlightVect[2]
	MULAy.y ACC,_sce_lightVect[0],_sce_spotNlightVect[0]
	MADDAy.y ACC,_sce_lightVect[1],_sce_spotNlightVect[1]
	MADDy.y _sce_innerProduct,_sce_lightVect[2],_sce_spotNlightVect[2]
	MULAz.z ACC,_sce_lightVect[0],_sce_spotNlightVect[0]
	MADDAz.z ACC,_sce_lightVect[1],_sce_spotNlightVect[1]
	MADDz.z _sce_innerProduct,_sce_lightVect[2],_sce_spotNlightVect[2]
	 
	MINIx.xyz _sce_innerProduct,_sce_innerProduct,VF00	
	MUL.xyz _sce_innerProductSq,_sce_innerProduct,_sce_innerProduct
	MUL.xyz _sce_cosSq,_sce_innerProductSq,_sce_oneOverNormSq
	SUB.xyz _sce_intensityCoef,_sce_cosSq,_sce_spotlightAngle
	MUL.xyz _sce_intensityCoef,_sce_intensityCoef,_sce_spotLightConst
	MULAx.xyz ACC,_sce_lightVect[0],_sce_normal
	MADDAy.xyz ACC,_sce_lightVect[1],_sce_normal
	MADDz.xyz _sce_innerLightNormal,_sce_lightVect[2],_sce_normal
       .AIF "\shading_type" EQ "SCE_PHONG"
		MUL.xyz	_sce_refVectCoef,_sce_innerLightNormal,_sce_oneOverNorm
       .AENDI
	MUL.xyz _sce_innerLightNormal,_sce_innerLightNormal,_sce_oneOverNormSq
	MAX.xyz _sce_innerLightNormal,_sce_innerLightNormal,VF00x
	MUL.xyz _sce_intensityCoef,_sce_intensityCoef,_sce_innerLightNormal
	MAXx.xyz _sce_intensityCoef,_sce_intensityCoef,VF00x
	MULAx.xyz ACC,_sce_spotLightColor[0], _sce_intensityCoefx	
	MADDAy.xyz ACC,_sce_spotLightColor[1],_sce_intensityCoefy	
       .AIF "\shading_type" EQ "SCE_VERTEXC"
           .AIF "\ambient_calc" EQ "SCE_AMBIENT"
	     MADDAz.xyz ACC,_sce_spotLightColor[2],_sce_intensityCoefz
	     MADDw.xyz \diffout, _sce_lightAmbientColor, VF00w
	   .AELSE
	     MADDz.xyz \diffout,_sce_spotLightColor[2],_sce_intensityCoefz
	   .AENDI
       .AELSE
	   .AIF "\shading_type" EQ "SCE_PHONG"
	        MADDz.xyz \diffout,_sce_spotLightColor[2],_sce_intensityCoefz
		fwSpecularBF _sce_bfCheck,_sce_refVectCoef 
;reflection vector calc.
	 	ADD.xyz		_sce_refVectCoef,_sce_refVectCoef,_sce_refVectCoef
 	 	MULx.xyz	_sce_refVect1,_sce_normal,_sce_refVectCoef
	 	MULy.xyz	_sce_refVect2,_sce_normal,_sce_refVectCoef
	 	MULz.xyz 	_sce_refVect3,_sce_normal,_sce_refVectCoef
	 	mTransposeXYZMatrix _sce_lightVect
	 	MULx.xyz 	_sce_lightVect[0],_sce_lightVect[0],_sce_oneOverNorm
	 	MULy.xyz  	_sce_lightVect[1],_sce_lightVect[1],_sce_oneOverNorm
	 	MULz.xyz	_sce_lightVect[2],_sce_lightVect[2],_sce_oneOverNorm
	 	SUB.xyz		_sce_refVect1,_sce_refVect1,_sce_lightVect[0]
	 	SUB.xyz		_sce_refVect2,_sce_refVect2,_sce_lightVect[1]
	 	SUB.xyz 	_sce_refVect3,_sce_refVect3,_sce_lightVect[2]
	   	.AIF "\calcEyeVect" EQ "calc"	 
	   		fwGetEyeVectorP  _sce_eyeVect,_sce_localCamera,_sce_vrt
		.AENDI	 
		fwGetSpecularCosine _sce_specFact,_sce_eyeVect,_sce_refVect1,_sce_refVect2,_sce_refVect3
		fwRemoveSpecularBF _sce_specFact,_sce_bfCheck
		fwShininess _sce_specFact
; now intensity calc
	 	MUL.xyz 	_sce_specFact,_sce_specFact,_sce_intensityCoef ; 3 different lights
 	 	MULAx.xyz 	ACC,_sce_spotLightColor[0], _sce_specFactx	
	 	MADDAy.xyz 	ACC,_sce_spotLightColor[1],_sce_specFacty	
	 	MADDz.xyz 	\specout,_sce_spotLightColor[2],_sce_specFactz	
	   .AELSE
	       .AIF "\shading_type" EQ "SCE_LAMBERT"
		  MADDz.xyz \diffout,_sce_spotLightColor[2],_sce_intensityCoefz
	       .AELSE
		  .AIF "\shading_type" EQ "SCE_BLINN"
		    MADDz.xyz 	\diffout,_sce_spotLightColor[2],_sce_intensityCoefz		
	        ;reflection vector calc.	 	 	   
		    .AIF "\calcEyeVect" EQ "calc"	 
	   		fwGetEyeVectorP  _sce_eyeVect,_sce_localCamera,_sce_vrt
		    .AENDI	 
		    mTransposeXYZMatrix _sce_lightVect
	 	    MULx.xyz 	_sce_lightVect[0],_sce_lightVect[0],_sce_oneOverNorm
	 	    MULy.xyz  	_sce_lightVect[1],_sce_lightVect[1],_sce_oneOverNorm
	 	    MULz.xyz	_sce_lightVect[2],_sce_lightVect[2],_sce_oneOverNorm
		    ADD.xyz	_sce_refVect1,_sce_eyeVect,_sce_lightVect[0]
		    ADD.xyz	_sce_refVect2,_sce_eyeVect,_sce_lightVect[1]
		    ADD.xyz     _sce_refVect3,_sce_eyeVect,_sce_lightVect[2]
		    fwBlinnCalc _sce_specFact,_sce_refVect1,_sce_refVect2,_sce_refVect3
	 	    MUL.xyz 	_sce_specFact,_sce_specFact,_sce_intensityCoef ; 3 different lights
 	 	    MULAx.xyz 	ACC,_sce_spotLightColor[0], _sce_specFactx	
	 	    MADDAy.xyz 	ACC,_sce_spotLightColor[1],_sce_specFacty	
	 	    MADDz.xyz 	\specout,_sce_spotLightColor[2],_sce_specFactz	
	          .AENDI
               .AENDI
           .AENDI
       .AENDI
.endm

.macro fwLoadMaterialColor diffColor,specColor,emissionColor,ambientColor,l1,l2,l3,l4
	LQ.xyzw \diffColor,\l1(VI00)
	LQ.xyz  \specColor,\l2(VI00)
	LQ.xyz  \emissionColor,\l3(VI00)
	LQ.xyz  \ambientColor, \l4(VI00)
.endm 

.macro fwCalcColorSum shading_type,color_clamp,out,df,dColor,aColor="defa",eColor="defe",sp="defd",sColor="defs" 
       .AIF "\shading_type" EQ "SCE_BLINN"
	 	MULA.xyz 	ACC,\sp,\sColor
		MADDA.xyz	ACC,_sce_lightAmbientColor,\aColor
		MADDAw.xyz	ACC,\eColor,vf00w
	        MADD.xyz	\out,\df,\dColor
	     .AIF "\color_clamp" EQ "SCE_CLAMP"
	 	LOI 255
	 	MINII.xyz 	\out,\out,I
	     .AENDI
	     .AIF "\color_clamp" EQ "SCE_SAT_CLAMP"
		MAXy.x _sce_satur,\out,\out
		MAXz.x _sce_satur,_sce_satur,\out
		LOI 255.0
		ADDI.x _sce_maxCol,vf00,I
		DIV   Q,_sce_maxColx,_sce_saturx
		ADDQ.y	_sce_satur,vf00,Q
		MINIw.y  _sce_satur,_sce_satur,vf00
		MULy.xyz \out,\out,_sce_satur
	     .AENDI
      .AENDI
      .AIF "\shading_type" EQ "SCE_VERTEXC"
	     .AIF "\color_clamp" EQ "SCE_CLAMP"
		MINIw.xyz \df,\df,VF00w		
	     .AENDI
		MUL.xyz \out, \df,\dColor
	     .AIF "\color_clamp" EQ "SCE_SAT_CLAMP"
		MAXy.x _sce_satur,\out,\out
		MAXz.x _sce_satur,_sce_satur,\out
		LOI 255.0
		ADDI.x _sce_maxCol,vf00,I
		DIV   Q,_sce_maxColx,_sce_saturx
		ADDQ.y	_sce_satur,vf00,Q
		MINIw.y  _sce_satur,_sce_satur,vf00
		MULy.xyz \out,\out,_sce_satur
	    .AENDI
      .AELSE
	.AIF "\shading_type" EQ "SCE_PHONG"
	 	MULA.xyz 	ACC,\sp,\sColor
		MADDA.xyz	ACC,_sce_lightAmbientColor,\aColor
		MADDAw.xyz	ACC,\eColor,vf00w
	        MADD.xyz	\out,\df,\dColor
	     .AIF "\color_clamp" EQ "SCE_CLAMP"
	 	LOI 255
	        MINII.xyz 	\out,\out,I
	     .AENDI
	     .AIF "\color_clamp" EQ "SCE_SAT_CLAMP"
		MAXy.x _sce_satur,\out,\out
		MAXz.x _sce_satur,_sce_satur,\out
		LOI 255.0
		ADDI.x _sce_maxCol,vf00,I
		DIV   Q,_sce_maxColx,_sce_saturx
		ADDQ.y	_sce_satur,vf00,Q
		MINIw.y  _sce_satur,_sce_satur,vf00
		MULy.xyz \out,\out,_sce_satur
 	     .AENDI
	.AELSE 
	  .AIF "\shading_type" EQ "SCE_LAMBERT"
	   	MULA.xyz	ACC,_sce_lightAmbientColor,\aColor
		MADDAw.xyz	ACC,\eColor,vf00w
		MADD.xyz	\out,\df,\dColor
	     .AIF "\color_clamp" EQ "SCE_CLAMP"
	 	LOI 255
	 	MINII.xyz 	\out,\out,I
	     .AENDI
	     .AIF "\color_clamp" EQ "SCE_SAT_CLAMP
		MAXy.x _sce_satur,\out,\out
		MAXz.x _sce_satur,_sce_satur,\out
		LOI 255.0
		ADDI.x _sce_maxCol,vf00,I
		DIV   Q,_sce_maxColx,_sce_saturx
		ADDQ.y	_sce_satur,vf00,Q
		MINIw.y  _sce_satur,_sce_satur,vf00
		MULy.xyz \out,\out,_sce_satur
	     .AENDI
	  .AENDI
        .AENDI
      .AENDI
.endm


.macro fwWriteColor prim, color
	 FTOI0 		\color, \color		
	 SQ 		\color, \prim(_sce_out_p)
.endm

	
.macro fwCalcSTSxT local_tex

	; calulate S,T,SxT vector for bump-mapping
	; e.g.: plane equation: Ax+Bs+Ct+D=0
	;     -> dX/dS = -B/A

	SUB.xyz	_sce_posDiff1,_sce_vrt1,_sce_vrt0
	SUB.xyz _sce_posDiff2,_sce_vrt2,_sce_vrt0
	SUB.xy	_sce_stDiff1,_sce_ST1,_sce_ST0
	SUB.xy	_sce_stDiff2,_sce_ST2,_sce_ST0

	ADDy.z	_sce_stDiff1,vf00,_sce_stDiff1
	ADDx.y	_sce_stDiff1,vf00,_sce_stDiff1
	ADDy.z	_sce_stDiff2,vf00,_sce_stDiff2
	ADDx.y	_sce_stDiff2,vf00,_sce_stDiff2

	; calc. bum_normal1=(A1,B1,C1,D1)
	; A1*x+B1*s+C1*t+D1=0

	MOVE.x	_sce_xPosST1,_sce_posDiff1
	MOVE.x	_sce_xPosST2,_sce_posDiff2
	MOVE.yz	_sce_xPosST1,_sce_stDiff1
	MOVE.yz	_sce_xPosST2,_sce_stDiff2
	OPMULA.xyz 	ACC,_sce_xPosST1,_sce_xPosST2      	
	OPMSUB.xyz 	_sce_bumpNormal1,_sce_xPosST2,_sce_xPosST1	

	; calc. bum_normal2=(A2,B2,C2,D2)
	; A2*y+B2*s+C2*t+D2=0
	ADDy.x	_sce_xPosST1,vf00,_sce_posDiff1
	ADDy.x  _sce_xPosST2,vf00,_sce_posDiff2
	OPMULA.xyz 	ACC,_sce_xPosST1,_sce_xPosST2      	
	OPMSUB.xyz 	_sce_bumpNormal2,_sce_xPosST2,_sce_xPosST1	


	; calc. bum_normal3=(A3,B3,C3,D3)
	; A3*z+B3*s+C3*t+D3=0
	ADDz.x	_sce_xPosST1,vf00,_sce_posDiff1
	ADDz.x  _sce_xPosST2,vf00,_sce_posDiff2
	OPMULA.xyz 	ACC,_sce_xPosST1,_sce_xPosST2      	
	OPMSUB.xyz 	_sce_bumpNormal3,_sce_xPosST2,_sce_xPosST1	

	; bumdiv = (1/A1 1/A2 1/A3 -)
	DIV	Q,vf00w,_sce_bumpNormal1x
	ADDQ.x 	_sce_bumpDiv,vf00,Q
	DIV	Q,vf00w,_sce_bumpNormal2x
	ADDQ.y 	_sce_bumpDiv,vf00,Q
	DIV	Q,vf00w,_sce_bumpNormal3x
	ADDQ.z 	_sce_bumpDiv,vf00,Q

	; now calc. S and T Vector from normals
	ADDy.x _sce_SVector,vf00,_sce_bumpNormal1
	ADDy.y _sce_SVector,vf00,_sce_bumpNormal2
	ADDy.z _sce_SVector,vf00,_sce_bumpNormal3
	ADDz.x _sce_TVector,vf00,_sce_bumpNormal1
	ADDz.y _sce_TVector,vf00,_sce_bumpNormal2
	ADDz.z _sce_TVector,vf00,_sce_bumpNormal3

	MUL.xyz _sce_SVector,_sce_SVector,_sce_bumpDiv
	MUL.xyz	_sce_TVector,_sce_TVector,_sce_bumpDiv
	
	; norm. S,T vectors
	getRcpLengP_x	_sce_SVector,_sce_STNorm
	getRcpLengP_y	_sce_TVector,_sce_STNorm
	MULx.xyz \local_tex[0],_sce_SVector,_sce_STNorm
	MULy.xyz \local_tex[1],_sce_TVector,_sce_STNorm	

	;calc SxT vector
	OPMULA.xyz 	ACC,_sce_SVector,_sce_TVector   	
	OPMSUB.xyz 	\local_tex[2],_sce_TVector,_sce_SVector	

	; transpose matrix 
	mTransposeXYZMatrix \local_tex
.endm

.macro fwCalcEmbossShift bumpShift,local_tex,light_vect
	mulax.xyz	ACC,\local_tex[0],\light_vect
	madday.xyz	ACC,\local_tex[1],\light_vect
	maddz.xyz	_sce_bumpLightPos,\local_tex[2],\light_vect
	LQ.x   	_sce_bumpParam,31(VI00)
	MULx.xy	\bumpShift,_sce_bumpLightPos,_sce_bumpParam
.endm
