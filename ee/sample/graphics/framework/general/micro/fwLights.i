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
		.AENDI
           .AENDI
	.AENDI
	
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

	 MULAx.x 	ACC, _sce_one,_sce_cosineFact1
	 MADDAy.x 	ACC, _sce_one,_sce_cosineFact1
	 MADDz.x 	\specFact,_sce_one,_sce_cosineFact1
	 
	 MULAx.y 	ACC, _sce_one, _sce_cosineFact2
	 MADDAy.y 	ACC, _sce_one, _sce_cosineFact2
	 MADDz.y 	\specFact,_sce_one, _sce_cosineFact2	 
	 
	 MULAx.z 	ACC, _sce_one, _sce_cosineFact3
	 MADDAy.z 	ACC, _sce_one, _sce_cosineFact3
	 MADDz.z 	\specFact,_sce_one,_sce_cosineFact3
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
	 	MULAx.x 	ACC, _sce_one, _sce_normInnerProduct1
	 	MADDAy.x 	ACC, _sce_one,_sce_normInnerProduct1
	 	MADDz.x 	_sce_refVectCoef,_sce_one,_sce_normInnerProduct1	 
	 	MULAx.y 	ACC, _sce_one, _sce_normInnerProduct2
	 	MADDAy.y 	ACC, _sce_one,_sce_normInnerProduct2
	 	MADDz.y 	_sce_refVectCoef,_sce_one,_sce_normInnerProduct2	 	 
	 	MULAx.z 	ACC, _sce_one, _sce_normInnerProduct3
	 	MADDAy.z 	ACC, _sce_one,_sce_normInnerProduct3
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
	.AELSE
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

.macro fwCalcColorSum shading_type,color_clamp,out,df,dColor,aColor="defa",sp="defd",sColor="defs",eColor="defe"	
      .AIF "\shading_type" EQ "SCE_VERTEXC"
	     .AIF "\color_clamp" EQ "SCE_CLAMP"
		MINIw.xyz \df,\df,VF00w		
	     .AENDI
		MUL.xyz \out, \df,\dColor
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
	.AELSE 
	  .AIF "\shading_type" EQ "SCE_LAMBERT"
	   	MULA.xyz	ACC,_sce_lightAmbientColor,\aColor
		MADD.xyz	\out,\df,\dColor
	     .AIF "\color_clamp" EQ "SCE_CLAMP"
	 	LOI 255
	 	MINII.xyz 	\out,\out,I
	     .AENDI
	  .AENDI
        .AENDI
      .AENDI
.endm


.macro fwWriteColor prim, color
	 FTOI0 		\color, \color		
	 SQ 		\color, \prim(_sce_out_p)
.endm

