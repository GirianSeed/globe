   	.include "utils.i"
	.include "math.i
	.include "fwLights.i"   ; for lightning macros
	.include "fw.i"		; for basic  macros

;****************************************************************************
;	My Micros
;****************************************************************************
.macro	mySkin bonep
	lq	bonem, 0(\bonep)
	mulax	ACC, bonem, _skin_weight
	lq	bonem, 4(\bonep)
	madday	ACC, bonem, _skin_weight
	lq	bonem, 8(\bonep)
	maddaz	ACC, bonem, _skin_weight
	lq	bonem, 12(\bonep)
	maddw	localworld[0], bonem, _skin_weight

	lq	bonem, 1(\bonep)
	mulax	ACC, bonem, _skin_weight
	lq	bonem, 5(\bonep)
	madday	ACC, bonem, _skin_weight
	lq	bonem, 9(\bonep)
	maddaz	ACC, bonem, _skin_weight
	lq	bonem, 13(\bonep)
	maddw	localworld[1], bonem, _skin_weight

	lq	bonem, 2(\bonep)
	mulax	ACC, bonem, _skin_weight
	lq	bonem, 6(\bonep)
	madday	ACC, bonem, _skin_weight
	lq	bonem, 10(\bonep)
	maddaz	ACC, bonem, _skin_weight
	lq	bonem, 14(\bonep)
	maddw	localworld[2], bonem, _skin_weight

	lq	bonem, 3(\bonep)
	mulax	ACC, bonem, _skin_weight
	lq	bonem, 7(\bonep)
	madday	ACC, bonem, _skin_weight
	lq	bonem, 11(\bonep)
	maddaz	ACC, bonem, _skin_weight
	lq	bonem, 15(\bonep)
	maddw	localworld[3], bonem, _skin_weight

	;; sum of weights == 0 then wsum = 1 , == 1 then wsum = 0
	esum		P, _skin_weight
	mfp.w		_wsum, P
	sub.w		_wsum, vf00, _wsum

	;; result vertex
	mMulVecByMat	_org_vrt, _sce_vrt, _sce_localWorld
	mulw		_org_vrt, _org_vrt, _wsum

	mMulVecByMat	_sce_vrt, _sce_vrt, localworld
	add		_sce_vrt, _sce_vrt, _org_vrt

	;; result lightrot matrix (localworld w/o scale)
	mulw	_sce_lightRot[0], _sce_lightRot[0], _wsum
	mulw	_sce_lightRot[1], _sce_lightRot[1], _wsum
	mulw	_sce_lightRot[2], _sce_lightRot[2], _wsum

	add	_sce_lightRot[0], _sce_lightRot[0], localworld[0]
	add	_sce_lightRot[1], _sce_lightRot[1], localworld[1]
	add	_sce_lightRot[2], _sce_lightRot[2], localworld[2]
.endm

.macro	myRTP prim	; worldscreen * vertex
	mulax		ACC,_sce_worldScreen[0],_sce_vrt
	madday		ACC,_sce_worldScreen[1],_sce_vrt
	maddaz		ACC,_sce_worldScreen[2],_sce_vrt
	maddw		_sce_screenVer,_sce_worldScreen[3],vf00

	div 		Q,vf00w,_sce_screenVerw
	mulq.xyz	_sce_screenVer,_sce_screenVer,Q
	ftoi4.xzy	_sce_storeVer, _sce_screenVer
	sq.xyzw		_sce_storeVer,\prim(_sce_out_p) 			; XYZF2,3
.endm

.macro myCalcNLightVect light_type
	mMulVecByMat \light_type[0],_sce_lightRot[0],_sce_lightVect
	mMulVecByMat \light_type[1],_sce_lightRot[1],_sce_lightVect
	mMulVecByMat \light_type[2],_sce_lightRot[2],_sce_lightVect
.endm

;****************************************************************************
	.init_vf_all
	.init_vi_all
	
	.name vclskin

        --enter								; entry point for vcl
	--endenter   				
	
;**********Micro Init-Block *****************

	fwLocalScreen
	iaddiu	_sce_bone_p, vi00, 72
;;	fwLoadNLightVect \&DIR_LIGHT_VECT0,\&DIR_LIGHT_VECT1,\&DIR_LIGHT_VECT2,_sce_dirNLightVect
	fwCalcNLightVect \&DIR_LIGHT_VECT0,\&DIR_LIGHT_VECT1,\&DIR_LIGHT_VECT2,_sce_dirNLightVect

;********************************************


	--cont
 	--cont					 


;******************* Buffer Init-Block *********
SKIN_START:
	XTOP 		_sce_in_p
	ILW.x 		_sce_packsize, 10(VI00)		; inbuf size
	IADD   	 	_sce_pXkick,_sce_in_p,_sce_packsize
	IADDIU 	 	_sce_out_p,_sce_pXkick,0
;***********************************************

;******************** GifTag Init-Block ********
SKIN_GIFTAG:

	fwInitGifTag
	fwSetVertexCounter
	fwLoadLightColor  \&DIR_LIGHT_COLOR0,\&DIR_LIGHT_COLOR1,\&DIR_LIGHT_COLOR2,_sce_dirLightColor
	fwLoadLightAmbientColor \&AMBIENT_LIGHT_COLOR 

	sub.w	_sce_storeVer,vf00,vf00			; screenVer.w=0

;*************************************************************

;******************** Vertex Loop-Block ***********************
SKIN_vertex_loop:

     	--LoopCS	1,1

	lq		_sce_vrt,_sce_vert_XYZ(_sce_in_p)
	lq		_sce_normal,_sce_vert_nXYZ(_sce_in_p)
   	lq		_sce_ST,_sce_vert_ST(_sce_in_p)
	lq		_sce_vColor,_sce_vert_RGBA(_sce_in_p)

	; get 4 weights
	addw.x		_skin_weight, vf00, _sce_vrt
	addw.y		_skin_weight, vf00, _sce_normal
	lq.zw		_skin_weight, _sce_vert_ST(_sce_in_p)

	; Vw = Nw = STz = 1.0
	move.w		_sce_vrt, vf00
	move.w		_sce_normal, vf00
	addw.z		_sce_ST, vf00, vf00

	mySkin _sce_bone_p
	myRTP "_sce_prim_XYZF2"
	fwST "_sce_prim_ST"

	myCalcNLightVect _sce_dirNLightVect
	fwDirLight "SCE_VERTEXC","SCE_AMBIENT",_sce_diffFact
	fwCalcColorSum "SCE_VERTEXC","SCE_CLAMP",_sce_vColor,_sce_diffFact,_sce_vColor  ;use _sce_vColor as output to save alpha
	fwWriteColor "_sce_prim_RGBAQ",_sce_vColor
 

; increment pointers, decrement counter

  	iaddiu		_sce_out_p,_sce_out_p,_sce_prim_sizeof
   	iaddiu		_sce_in_p,_sce_in_p,_sce_vert_sizeof
	iaddi		_sce_nVerts,_sce_nVerts,-1

	ibne  		_sce_nVerts,vi00, SKIN_vertex_loop   
;**************************************************************************************

SKIN_done:

	mtir 		_sce_eopFlag,_sce_gifTagx
	ibgez 		_sce_eopFlag,SKIN_GIFTAG

	xgkick 		_sce_pXkick  

	--cont

        B 		SKIN_START        
	












; -------------------------------------------------------------------

.END
