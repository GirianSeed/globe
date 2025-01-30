	; -------------------- Macros ------------------------------------

	.macro	mMulVecByMat output, vector, matBaseName
	mulax	acc, \matBaseName[0], \vector
	madday	acc, \matBaseName[1], \vector
	maddaz	acc, \matBaseName[2], \vector
	maddw	\output, \matBaseName[3], \vector
	.endm


	.macro mTransposeXYZMatrix output
	MR32.w \output[2],\output[1]
	MOVE.y temp\@,\output[1]
	MR32.xy \output[1],\output[0]
	MR32.xzw temp\@, \output[2]
	MR32.xy \output[2],\output[1]
	MR32.yzw \output[0],temp\@
	MOVE.y \output[1],temp\@
	MR32.z \output[1],\output[0]
	.endm

	.macro mInvertRT output, matRT
; the rotation-part is going to be transposed and the translation-part is output[3]=R^(-1)*t
	move.xyz \output[0],\matRT[0]
	move.xyz \output[1],\matRT[1]
	move.xyz \output[2],\matRT[2]
        mTransposeXYZMatrix \output
	mulax.xyz	acc, \output[0], \matRT[3]
	madday.xyz	acc, \output[1], \matRT[3]
	maddz.xyz	\output[3], \output[2], \matRT[3]
.endm

 	.macro mMakeLocal localVect, worldVect, matWorldLocal
	mulax.xyz	acc, \matWorldLocal[0],\worldVect
	madday.xyz	acc, \matWorldLocal[1],\worldVect
	maddz.xyz	\localVect, \matWorldLocal[2],\worldVect
	sub.xyz  	\localVect,\localVect,\matWorldLocal[3] ; worldLocal[3]= -translation !!
	.endm

	.macro mLocalLightPos output, worldLightPos, matWorldLocal
	;mInvertRT _sce_worldLocal,\matLocalWorld
	mMakeLocal \output[0],\worldLightPos[0],\matWorldLocal
	mMakeLocal \output[1],\worldLightPos[1],\matWorldLocal
	mMakeLocal \output[2],\worldLightPos[2],\matWorldLocal
	.endm



	; --------------------------------

	.macro	getRcpLengQ_x 	vec,out
	mul.xyz		tmp\@,\vec,\vec
	mulax.w		acc,vf00,tmp\@
	madday.w	acc,vf00,tmp\@
	maddz.w		tmp\@,vf00,tmp\@
	rsqrt		q,vf00w,tmp\@w
	addq.x		\out,vf00,q
	.endm		


	.macro	getRcpLengQ_y 	vec,out
	mul.xyz		tmp\@,\vec,\vec
	mulax.w		acc,vf00,tmp\@
	madday.w	acc,vf00,tmp\@
	maddz.w		tmp\@,vf00,tmp\@
	rsqrt		q,vf00w,tmp\@w
	addq.y		\out,vf00,q
	.endm		


	.macro	getRcpLengQ_z 	vec,out
	mul.xyz		tmp\@,\vec,\vec
	mulax.w		acc,vf00,tmp\@
	madday.w	acc,vf00,tmp\@
	maddz.w		tmp\@,vf00,tmp\@
	rsqrt		q,vf00w,tmp\@w
	addq.z		\out,vf00,q
	.endm		

	; --------------------------------

	.macro	getRcpLengP_x 	vec,out
	erleng 	P,\vec	
       	mfp.x	\out,P
	.endm

	.macro	getRcpLengP_y 	vec,out
	erleng 	P,\vec	
       	mfp.y	\out,P
	.endm

	.macro	getRcpLengP_z 	vec,out
	erleng 	P,\vec	
       	mfp.z	\out,P
	.endm

	; --------------------------------

;	1/sum_of_squares

	; --------------------------------

	.macro	getSaddQ_x 	vec,out
	
	mul.xyz		tmp\@,\vec,\vec
	mulax.w		acc,vf00,tmp\@
	madday.w	acc,vf00,tmp\@
	maddz.w		tmp\@,vf00,tmp\@
	div		q,vf00w,tmp\@w
	addq.x		\out,vf00,q
	
	.endm		


	; --------------------------------

	.macro	getSaddQ_y 	vec,out
	
	mul.xyz		tmp\@,\vec,\vec
	mulax.w		acc,vf00,tmp\@
	madday.w	acc,vf00,tmp\@
	maddz.w		tmp\@,vf00,tmp\@
	div		q,vf00w,tmp\@w
	addq.y		\out,vf00,q
	
	.endm		


	.macro	getSaddQ_z 	vec,out
	
	mul.xyz		tmp\@,\vec,\vec
	mulax.w		acc,vf00,tmp\@
	madday.w	acc,vf00,tmp\@
	maddz.w		tmp\@,vf00,tmp\@	
	div		q,vf00w,tmp\@w
	addq.z		\out,vf00,q
	
	.endm		


	; --------------------------------


	.macro	getSaddP_x 	vec,out
	ersadd 	P,\vec
	mfp.x	\out,p
	.endm

	.macro	getSaddP_y 	vec,out
	ersadd 	P,\vec
	mfp.y	\out,p
	.endm


	.macro	getSaddP_z 	vec,out
	ersadd 	P,\vec
	mfp.z	\out,p
	.endm


