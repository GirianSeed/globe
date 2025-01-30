/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0.2
 $Id: vcl_sml.i,v 1.1 2002/10/24 13:28:29 aoki Exp $
 */
/* 
 * Emotion Engine Library Sample
 *
 * Copyright (C) 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * VCLSML - VCL Standard Macros Library
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2002-01-17      Geoff Audy
 *
 *  Note: Some macros generate the following temporary variables:
 * 	      vclsmlftemp:   Temporary float register
 * 	      vclsmlitemp:   Temporary integer register
 */
;--------------------------------------------------------------------
; MatrixLoad - Load "matrix" from VU mem location "vumemlocation" +
; "offset"
;--------------------------------------------------------------------
   .macro   MatrixLoad  matrix,offset,vumemlocation
   lq             \matrix[0], \offset+0(\vumemlocation)
   lq             \matrix[1], \offset+1(\vumemlocation)
   lq             \matrix[2], \offset+2(\vumemlocation)
   lq             \matrix[3], \offset+3(\vumemlocation)
   .endm

;--------------------------------------------------------------------
; MatrixSave - Save "matrix" to VU mem location "vumemlocation" +
; "offset"
;--------------------------------------------------------------------
   .macro   MatrixSave  matrix,offset,vumemlocation
   sq             \matrix[0], \offset+0(\vumemlocation)
   sq             \matrix[1], \offset+1(\vumemlocation)
   sq             \matrix[2], \offset+2(\vumemlocation)
   sq             \matrix[3], \offset+3(\vumemlocation)
   .endm

;--------------------------------------------------------------------
; MatrixMultiply - Multiply 2 matrices, "matleft" and "matright", and
; output the result in "matresult".  Dont forget matrix multipli-
; cations arent commutative, i.e. left X right wont give you the
; same result as right X left.
;
; Note: ACC register is modified
;--------------------------------------------------------------------
   .macro   MatrixMultiply   matresult,matleft,matright
   mul            acc,           \matright[0], \matleft[0][x]
   madd           acc,           \matright[1], \matleft[0][y]
   madd           acc,           \matright[2], \matleft[0][z]
   madd           \matresult[0], \matright[3], \matleft[0][w]

   mul            acc,           \matright[0], \matleft[1][x]
   madd           acc,           \matright[1], \matleft[1][y]
   madd           acc,           \matright[2], \matleft[1][z]
   madd           \matresult[1], \matright[3], \matleft[1][w]

   mul            acc,           \matright[0], \matleft[2][x]
   madd           acc,           \matright[1], \matleft[2][y]
   madd           acc,           \matright[2], \matleft[2][z]
   madd           \matresult[2], \matright[3], \matleft[2][w]

   mul            acc,           \matright[0], \matleft[3][x]
   madd           acc,           \matright[1], \matleft[3][y]
   madd           acc,           \matright[2], \matleft[3][z]
   madd           \matresult[3], \matright[3], \matleft[3][w]
   .endm

;--------------------------------------------------------------------
; LocalizeLightMatrix - Transform the light matrix "lightmatrix" into
; local space, as described by "matrix", and output the result in
; "locallightmatrix"
;
; Note: ACC register is modified
;--------------------------------------------------------------------
   .macro   LocalizeLightMatrix locallightmatrix,matrix,lightmatrix
   mul            acc,                  \lightmatrix[0], \matrix[0][x]
   madd           acc,                  \lightmatrix[1], \matrix[0][y]
   madd           acc,                  \lightmatrix[2], \matrix[0][z]
   madd           \locallightmatrix[0], \lightmatrix[3], \matrix[0][w]

   mul            acc,                  \lightmatrix[0], \matrix[1][x]
   madd           acc,                  \lightmatrix[1], \matrix[1][y]
   madd           acc,                  \lightmatrix[2], \matrix[1][z]
   madd           \locallightmatrix[1], \lightmatrix[3], \matrix[1][w]

   mul            acc,                  \lightmatrix[0], \matrix[2][x]
   madd           acc,                  \lightmatrix[1], \matrix[2][y]
   madd           acc,                  \lightmatrix[2], \matrix[2][z]
   madd           \locallightmatrix[2], \lightmatrix[3], \matrix[2][w]

   move           \locallightmatrix[3], \lightmatrix[3]
   .endm

;--------------------------------------------------------------------
; MatrixMultiplyVertex - Multiply "matrix" by "vertex", and output
; the result in "vertexresult"
;
; Note: Apply rotation, scale and translation
; Note: ACC register is modified
;--------------------------------------------------------------------
   .macro   MatrixMultiplyVertex vertexresult,matrix,vertex
   mul            acc,           \matrix[0], \vertex[x]
   madd           acc,           \matrix[1], \vertex[y]
   madd           acc,           \matrix[2], \vertex[z]
   madd           \vertexresult, \matrix[3], \vertex[w]
   .endm

;--------------------------------------------------------------------
; MatrixMultiplyVector - Multiply "matrix" by "vector", and output
; the result in "vectorresult"
;
; Note: Apply rotation and scale, but no translation
; Note: ACC register is modified
;--------------------------------------------------------------------
   .macro   MatrixMultiplyVector vectorresult,matrix,vector
   mul            acc,           \matrix[0], \vector[x]
   madd           acc,           \matrix[1], \vector[y]
   madd           \vectorresult, \matrix[2], \vector[z]
   .endm

;--------------------------------------------------------------------
; VectorLoad - Load "vector" from VU mem location "vumemlocation" +
; "offset"
;--------------------------------------------------------------------
   .macro   VectorLoad  vector,offset,vumemlocation
   lq             \vector, \offset(\vumemlocation)
   .endm

;--------------------------------------------------------------------
; VectorSave - Save "vector" to VU mem location "vumemlocation" +
; "offset"
;--------------------------------------------------------------------
   .macro   VectorSave  vector,offset,vumemlocation
   sq             \vector, \offset(\vumemlocation)
   .endm

;--------------------------------------------------------------------
; VectorAdd - Add 2 vectors, "vector1" and "vector2" and output the
; result in "vectorresult"
;--------------------------------------------------------------------
   .macro   VectorAdd   vectorresult,vector1,vector2
   add            \vectorresult, \vector1, \vector2
   .endm

;--------------------------------------------------------------------
; VectorSub - Subtract "vector2" from "vector1", and output the
; result in "vectorresult"
;--------------------------------------------------------------------
   .macro   VectorAdd   vectorresult,vector1,vector2
   sub            \vectorresult, \vector1, \vector2
   .endm

;--------------------------------------------------------------------
; VertexLoad - Load "vertex" from VU mem location "vumemlocation" +
; "offset"
;--------------------------------------------------------------------
   .macro   VertexLoad  vertex,offset,vumemlocation
   lq             \vertex, \offset(\vumemlocation)
   .endm

;--------------------------------------------------------------------
; VertexSave - Save "vertex" to VU mem location "vumemlocation" +
; "offset"
;--------------------------------------------------------------------
   .macro   VertexSave  vertex,offset,vumemlocation
   sq             \vertex, \offset(\vumemlocation)
   .endm

;--------------------------------------------------------------------
; VertexPersCorr - Apply perspective correction onto "vertex" and
; output the result in "vertexoutput"
;
; Note: Q register is modified
;--------------------------------------------------------------------
   .macro   VertexPersCorr vertexoutput,vertex
   div            q, vf00[w], \vertex[w]
;   mul.xyz        \vertexoutput, \vertex, q
   mul            \vertexoutput, \vertex, q
   .endm

;--------------------------------------------------------------------
; VertexPersCorrST - Apply perspective correction onto "vertex" and
; "st", and output the result in "vertexoutput" and "stoutput"
;
; Note: Q register is modified
;--------------------------------------------------------------------
   .macro   VertexPersCorrST vertexoutput,stoutput,vertex,st
   div            q,             vf00[w], \vertex[w]
   mul.xyz        \vertexoutput, \vertex, q
   move.w         \vertexoutput, \vertex
;   mul            \vertexoutput, \vertex, q
   mul            \stoutput,     \st,     q
   .endm

;--------------------------------------------------------------------
; VertexFPtoGsXYZ2 - Convert an XYZW, floating-point vertex to GS
; XYZ2 format (ADC bit isnt set)
;--------------------------------------------------------------------
   .macro   VertexFpToGsXYZ2  outputxyz,vertex
   ftoi4.xy       \outputxyz,    \vertex
   ftoi0.z        \outputxyz,    \vertex
   mfir           \outputxyz[w], vi00
   .endm

;--------------------------------------------------------------------
; VertexFPtoGsXYZ2Adc - Convert an XYZW, floating-point vertex to GS
; XYZ2 format (ADC bit is set)
;--------------------------------------------------------------------
   .macro   VertexFpToGsXYZ2Adc  outputxyz,vertex
   ftoi4.xy       \outputxyz, \vertex
   ftoi0.z        \outputxyz, \vertex
   ftoi15.w       \outputxyz, vf00
   .endm

;--------------------------------------------------------------------
; VertexFpToGsXYZF2 - Convert an XYZF, floating-point vertex to GS
; XYZF2 format (ADC bit isnt set)
;--------------------------------------------------------------------
   .macro   VertexFpToGsXYZF2 outputxyz,vertex
   ftoi4          \outputxyz, \vertex
   .endm

;--------------------------------------------------------------------
; VertexFpToGsXYZF2Adc - Convert an XYZF, floating-point vertex to GS
; XYZF2 format (ADC bit is set)
;--------------------------------------------------------------------
   .macro   VertexFpToGsXYZF2Adc outputxyz,vertex
   ftoi4          \outputxyz,    \vertex
   mtir           vclsmlitemp,   \outputxyz[w]
   iaddiu         vclsmlitemp,   0x7FFF
   iaddi          vclsmlitemp,   1
   mfir           \outputxyz[w], vclsmlitemp
   .endm

;--------------------------------------------------------------------
; ColorFPtoGsRGBAQ - Convert an RGBA, floating-point color to GS
; RGBAQ format
;--------------------------------------------------------------------
   .macro   ColorFPtoGsRGBAQ  outputrgba,color
   ftoi0          \outputrgba, \color
   .endm

;--------------------------------------------------------------------
; ColorGsRGBAQtoFP - Convert an RGBA, GS RGBAQ format to floating-
; point color
;--------------------------------------------------------------------
   .macro   ColorGsRGBAQtoFP  outputrgba,color
   itof0          \outputrgba, \color
   .endm

;--------------------------------------------------------------------
; CreateGsPRIM - Create a GS-packed-format PRIM command, according to
; a specified immediate value "prim"
;
; Note: Meant more for debugging purposes than for a final solution
;--------------------------------------------------------------------
   .macro   CreateGsPRIM   outputprim,prim
   iaddiu         vclsmlitemp, vi00, \prim
   mfir           \outputprim, vclsmlitemp
   .endm

;--------------------------------------------------------------------
; CreateGsRGBA - Create a GS-packed-format RGBA command, according to
; specified immediate values "r", "g", "b" and "a" (integer 0-255)
;
; Note: Meant more for debugging purposes than for a final solution
;--------------------------------------------------------------------
   .macro   CreateGsRGBA   outputrgba,r,g,b,a
   iaddiu         vclsmlitemp, vi00, \r
   mfir           \outputrgba[x], vclsmlitemp
   iaddiu         vclsmlitemp, vi00, \g
   mfir           \outputrgba[y], vclsmlitemp
   iaddiu         vclsmlitemp, vi00, \b
   mfir           \outputrgba[z], vclsmlitemp
   iaddiu         vclsmlitemp, vi00, \a
   mfir           \outputrgba[w], vclsmlitemp
   .endm

;--------------------------------------------------------------------
; CreateGsSTQ - Create a GS-packed-format STQ command, according to
; specified immediate values "s", "t" and "q" (floats)
;
; Note: I register is modified
; Note: Meant more for debugging purposes than for a final solution
;--------------------------------------------------------------------
   .macro   CreateGsSTQ   outputstq,s,t,q
   loi            \s
   add            \outputstq[x], vf00[x], i
   loi            \t
   add            \outputstq[y], vf00[y], i
   loi            \q
   add            \outputstq[z], vf00[z], i
   .endm

;--------------------------------------------------------------------
; CreateGsUV - Create a GS-packed-format VU command, according to
; specified immediate values "u" and "v" (integer -32768 - 32768,
; with 4 LSB as precision)
;
; Note: Meant more for debugging purposes than for a final solution
;--------------------------------------------------------------------
   .macro   CreateGsUV   outputuv,u,v
   iaddiu         vclsmlitemp, vi00, \u
   mfir           \outputuv[x], vclsmlitemp
   iaddiu         vclsmlitemp, vi00, \v
   mfir           \outputuv[y], vclsmlitemp
   .endm

;--------------------------------------------------------------------
; CreateGsRGBA - Create a GS-packed-format RGBA command, according to
; a specified immediate value "fog" (integer 0-255)
;
; Note: Meant more for debugging purposes than for a final solution
;--------------------------------------------------------------------
   .macro   CreateGsFOG   outputfog,fog
   iaddiu         vclsmlitemp, vi00, \fog * 16
   mfir           \outputfog[w], vclsmlitemp
   .endm

;--------------------------------------------------------------------
; CreateGifTag - Create a packed-mode giftag, according to specified
; immediate values.  Currently only support up to 4 registers.
;
; Note: I register is modified
; Note: Definitely meant for debugging purposes, NOT for a final
; solution
;--------------------------------------------------------------------
; MIGHT NOT BE IMPLEMENTABLE AFTER ALL (AT LEAST NOT UNTIL VCL EVALUATES CONSTANTS!)
; THAT WOULD HAVE BEEN KINDA COOL...  DAMN.  --GEOFF
;   .macro   CreateGifTag   outputgiftag,nloop,prim,nreg,reg1,reg2,reg3,reg4
;   iaddiu         vclsmlitemp, vi00, \nloop + 0x8000
;   mfir.x         \outputgiftag, vclsmlitemp
;   loi            0x00004000 + (\prim * 0x8000) + (\nreg * 0x10000000)
;   add.y          \outputgiftag, vf00, i
;   iaddiu         vclsmlitemp, vi00, \reg1 + (\reg2 * 16) + (\reg3 * 256) + (\reg4 * 4096)
;   mfir.z         \outputgiftag, vclsmlitemp
;   .endm

;--------------------------------------------------------------------
; VectorDotProduct - Calculate the dot product of "vector1" and
; "vector2", and output to "dotproduct"[x]
;--------------------------------------------------------------------
   .macro   VectorDotProduct  dotproduct,vector1,vector2
   mul.xyz        \dotproduct, \vector1,    \vector2
   add.x          \dotproduct, \dotproduct, \dotproduct[y]
   add.x          \dotproduct, \dotproduct, \dotproduct[z]
   .endm

;--------------------------------------------------------------------
; VectorCrossProduct - Calculate the cross product of "vector1" and
; "vector2", and output to "vectoroutput"
;
; Note: ACC register is modified
;--------------------------------------------------------------------
   .macro   VectorCrossProduct  vectoroutput,vector1,vector2
   opmula.xyz     ACC,           \vector1, \vector2
   opmsub.xyz     \vectoroutput, \vector2, \vector1
   sub.w          \vectoroutput, vf00,     vf00
   .endm

;--------------------------------------------------------------------
; VectorNormalize - Bring the length of "vector" to 1.f, and output
; it to "vectoroutput"
;
; Note: Q register is modified
;--------------------------------------------------------------------
   .macro   VectorNormalize   vecoutput,vector
   mul.xyz        vclsmlftemp,   \vector,     \vector
   add.x          vclsmlftemp,   vclsmlftemp, vclsmlftemp[y]
   add.x          vclsmlftemp,   vclsmlftemp, vclsmlftemp[z]
   rsqrt          q,             vf00[w],     vclsmlftemp[x]
   sub.w          \vectoroutput, vf00,        vf00
   mul.xyz        \vectoroutput, \vector,     q
   .endm

;--------------------------------------------------------------------
; VertexLightAmb - Apply ambient lighting "ambientrgba" to a vertex
; of color "vertexrgba", and output the result in "outputrgba"
;--------------------------------------------------------------------
   .macro   VertexLightAmb rgbaout,vertexrgba,ambientrgba
   mul            \rgbaout, \vertexrgba, \ambientrgba
   .endm

;--------------------------------------------------------------------
; VertexLightDir3 - Apply up to 3 directional lights contained in a
; light matrix "lightmatrix" to a vertex of color "vertexrgba" and
; having a normal "vertexnormal", and output the result in
; "outputrgba"
;
; Note: ACC register is modified
;--------------------------------------------------------------------
   .macro   VertexLightDir3   rgbaout,vertexrgba,vertexnormal,lightcolors,lightnormals
   mul            acc,      \lightnormals[0], \vertexnormal[x]
   madd           acc,      \lightnormals[1], \vertexnormal[y]
   madd           acc,      \lightnormals[2], \vertexnormal[z]
   madd           \rgbaout, \lightnormals[3], \vertexnormal[w] ; Here "rgbaout" is the dot product for the 3 lights
   max            \rgbaout, \rgbaout,         vf00[x]          ; Here "rgbaout" is the dot product for the 3 lights
   mul            acc,      \lightcolors[0],  \rgbaout[x]
   madd           acc,      \lightcolors[1],  \rgbaout[y]
   madd           \rgbaout, \lightcolors[2],  \rgbaout[z]      ; Here "rgbaout" is the light applied on the vertex
   mul            \rgbaout, \vertexrgba,      \rgbaout         ; Here "rgbaout" is the amount of light reflected by the vertex
   .endm

;--------------------------------------------------------------------
; VertexLightDir3Amb - Apply up to 3 directional lights, plus an
; ambient light contained in a light matrix "lightmatrix" to a vertex
; of color "vertexrgba" and having a normal "vertexnormal", and
; output the result in "outputrgba"
;
; Note: ACC register is modified
;--------------------------------------------------------------------
   .macro   VertexLightDir3Amb   rgbaout,vertexrgba,vertexnormal,lightcolors,lightnormals
   mul            acc,      \lightnormals[0], \vertexnormal[x]
   madd           acc,      \lightnormals[1], \vertexnormal[y]
   madd           acc,      \lightnormals[2], \vertexnormal[z]
   madd           \rgbaout, \lightnormals[3], \vertexnormal[w] ; Here "rgbaout" is the dot product for the 3 lights
   max            \rgbaout, \rgbaout,         vf00[x]          ; Here "rgbaout" is the dot product for the 3 lights
   mul            acc,      \lightcolors[0],  \rgbaout[x]
   madd           acc,      \lightcolors[1],  \rgbaout[y]
   madd           acc,      \lightcolors[2],  \rgbaout[z]
   madd           \rgbaout, \lightcolors[3],  \rgbaout[w]      ; Here "rgbaout" is the light applied on the vertex
   mul.xyz        \rgbaout, \vertexrgba,      \rgbaout         ; Here "rgbaout" is the amount of light reflected by the vertex
   .endm

;--------------------------------------------------------------------
; FogSetup - Set up fog "fogparams", by specifying "nearfog" and
; "farfog".  "fogparams" will afterward be ready to be used by fog-
; related macros, like "VertexFogLinear" for example.
;
; Note: I register is modified
;--------------------------------------------------------------------
   .macro   FogSetup fogparams,nearfogz,farfogz
   sub            \fogparams, vf00,          vf00           ; Set XYZW to 0
   loi            \farfogz                                  ;
   add.w          \fogparams, \fogparams,    i              ; fogparam[w] is farfogz
   loi            \nearfogz
   add.z          \fogparams, \fogparams,    \fogparams[w]
   sub.z          \fogparams, \fogparams,    i
   loi            255.0
   add.xy         \fogparams, \fogparams,    i              ; fogparam[y] is 255.0
   sub.x          \fogparams, \fogparams,    vf00[w]        ; fogparam[x] is 254.0
   div            q,          \fogparams[y], \fogparams[z]
   sub.z          \fogparams, \fogparams,    \fogparams
   add.z          \fogparams, \fogparams,    q              ; fogparam[z] is 255.f / (farfogz - nearfogz)
   .endm

;--------------------------------------------------------------------
; VertexFogLinear - Apply fog "fogparams" to a vertex "xyzw", and
; output the result in "xyzfoutput". "xyzw" [w] is assumed to be
; the distance from the camera.  "fogparams" must contain farfogz in
; [w], and (255.f / (farfogz - nearfogz)) in [z]. "xyzfoutputf" [w]
; will contain a float value between 0.0 and 255.0, inclusively.
;--------------------------------------------------------------------
   .macro   VertexFogLinear   xyzfoutput,xyzw,fogparams
   move.xyz       \xyzfoutput, \xyzw                        ; XYZ part won't be modified
   sub.w          \xyzfoutput, \fogparams,  \xyzw[w]        ; fog = (farfogz - z) * 255.0 /
   mul.w          \xyzfoutput, \xyzfoutput, \fogparams[z]   ;       (farfogz - nearfogz)
   max.w          \xyzfoutput, \xyzfoutput, vf00[x]         ; Clamp fog values outside the range 0.0-255.0
   mini.w         \xyzfoutput, \xyzfoutput, \fogparams[y]   ;
   .endm

;--------------------------------------------------------------------
; VertexFogRemove - Remove any effect of fog to "xyzf".  "fogparams"
; [x] must be set to 254.0.  "xyzf" will be modified directly.
;--------------------------------------------------------------------
   .macro   VertexFogRemove   xyzf,fogparams
   add.w          \xyzf, vf00, \fogparams[x]                ; xyzw[w] = 1.0 + 254.0 = 255.0 = no fog
   .endm

;--------------------------------------------------------------------
; PushInteger1 - Push "integer1" on "stackptr"
;
; Note: "stackptr" is updated
;--------------------------------------------------------------------
   .macro   PushInteger1   stackptr,integer1
   isubiu         \stackptr, \stackptr, 1
   iswr.x         \integer1, (\stackptr)
   .endm

;--------------------------------------------------------------------
; PushInteger2 - Push "integer1" and "integer2" on "stackptr"
;
; Note: "stackptr" is updated
;--------------------------------------------------------------------
   .macro   PushInteger2   stackptr,integer1,integer2
   isubiu         \stackptr, \stackptr, 1
   iswr.x         \integer1, (\stackptr)
   iswr.y         \integer2, (\stackptr)
   .endm

;--------------------------------------------------------------------
; PushInteger3 - Push "integer1", "integer2" and "integer3" on
; "stackptr"
;
; Note: "stackptr" is updated
;--------------------------------------------------------------------
   .macro   PushInteger3   stackptr,integer1,integer2,integer3
   isubiu         \stackptr, \stackptr, 1
   iswr.x         \integer1, (\stackptr)
   iswr.y         \integer2, (\stackptr)
   iswr.z         \integer3, (\stackptr)
   .endm

;--------------------------------------------------------------------
; PushInteger4 - Push "integer1", "integer2", "integer3" and
; "integer4" on "stackptr"
;
; Note: "stackptr" is updated
;--------------------------------------------------------------------
   .macro   PushInteger4   stackptr,integer1,integer2,integer3,integer4
   isubiu         \stackptr, \stackptr, 1
   iswr.x         \integer1, (\stackptr)
   iswr.y         \integer2, (\stackptr)
   iswr.z         \integer3, (\stackptr)
   iswr.w         \integer4, (\stackptr)
   .endm

;--------------------------------------------------------------------
; PopInteger1 - Pop "integer1" on "stackptr"
;
; Note: "stackptr" is updated
;--------------------------------------------------------------------
   .macro   PopInteger1   stackptr,integer1
   ilwr.x         \integer1, (\stackptr)
   iaddiu         \stackptr, \stackptr, 1
   .endm

;--------------------------------------------------------------------
; PopInteger2 - Pop "integer1" and "integer2" on "stackptr"
;
; Note: "stackptr" is updated
;--------------------------------------------------------------------
   .macro   PopInteger2   stackptr,integer1,integer2
   ilwr.y         \integer2, (\stackptr)
   ilwr.x         \integer1, (\stackptr)
   iaddiu         \stackptr, \stackptr, 1
   .endm

;--------------------------------------------------------------------
; PopInteger3 - Pop "integer1", "integer2" and "integer3" on
; "stackptr"
;
; Note: "stackptr" is updated
;--------------------------------------------------------------------
   .macro   PopInteger3   stackptr,integer1,integer2,integer3
   ilwr.z         \integer3, (\stackptr)
   ilwr.y         \integer2, (\stackptr)
   ilwr.x         \integer1, (\stackptr)
   iaddiu         \stackptr, \stackptr, 1
   .endm

;--------------------------------------------------------------------
; PopInteger4 - Pop "integer1", "integer2", "integer3" and
; "integer4" on "stackptr"
;
; Note: "stackptr" is updated
;--------------------------------------------------------------------
   .macro   PopInteger4   stackptr,integer1,integer2,integer3,integer4
   ilwr.w         \integer4, (\stackptr)
   ilwr.z         \integer3, (\stackptr)
   ilwr.y         \integer2, (\stackptr)
   ilwr.x         \integer1, (\stackptr)
   iaddiu         \stackptr, \stackptr, 1
   .endm

;--------------------------------------------------------------------
; PushMatrix - Push "matrix" onto the "stackptr"
;
; Note: "stackptr" is updated
;--------------------------------------------------------------------
   .macro   PushMatrix  stackptr,matrix
   sq             \matrix[0], -1(\stackptr)
   sq             \matrix[1], -2(\stackptr)
   sq             \matrix[2], -3(\stackptr)
   sq             \matrix[3], -4(\stackptr)
   iaddi          \stackptr, \stackptr, -4
   .endm

;--------------------------------------------------------------------
; PopMatrix - Pop "matrix" out of the "stackptr"
;
; Note: "stackptr" is updated
;--------------------------------------------------------------------
   .macro   PopMatrix   stackptr,matrix
   lq             \matrix[0], 0(\stackptr)
   lq             \matrix[1], 1(\stackptr)
   lq             \matrix[2], 2(\stackptr)
   lq             \matrix[3], 3(\stackptr)
   iaddi          \stackptr, \stackptr, 4
   .endm

;--------------------------------------------------------------------
; PushVector - Push "vector" onto the "stackptr"
;
; Note: "stackptr" is updated
;--------------------------------------------------------------------
   .macro   PushVector  stackptr,vector
   sqd            \vector, (--\stackptr)
   .endm

;--------------------------------------------------------------------
; PopVector - Pop "vector" out of the "stackptr"
;
; Note: "stackptr" is updated
;--------------------------------------------------------------------
   .macro   PopVector   stackptr,vector
   lqi            \vector, (\stackptr++)
   .endm

;--------------------------------------------------------------------
; PushVertex - Push "vector" onto the "stackptr"
;
; Note: "stackptr" is updated
;--------------------------------------------------------------------
   .macro   PushVertex  stackptr,vertex
   sqd            \vertex, (--\stackptr)
   .endm

;--------------------------------------------------------------------
; PopVertex - Pop "vertex" out of the "stackptr"
;
; Note: "stackptr" is updated
;--------------------------------------------------------------------
   .macro   PopVertex   stackptr,vertex
   lqi            \vertex, (\stackptr++)
   .endm



;--------------------------------------------------------------------
; Name Here - Description here
;
; Note:
;--------------------------------------------------------------------
   .macro   TriangleClip   fanoutput,xyzw
   .endm
