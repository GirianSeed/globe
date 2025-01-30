/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0.2
 $Id: scissor.vcl,v 1.3 2004/04/20 12:22:59 aoki Exp $
 */
/* 
 * Emotion Engine Library Sample
 *
 * Copyright (C) 2003 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * scissor.vcl - VCL code for scissoring
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2002-10-22      Geoff Audy
 * 
 * Based on original code from T. Hatake (December 1999) and modifi-
 * cations from David Coombes (August 2001)
 * 
 */

   .include "vcl_sml.i"

;   .name          scissoringvu1
;   .mpg           0
   .syntax        new

   .init_vf_all
   .init_vi_all

L2W_OFFSET              .assign     0
L2S_OFFSET              .assign     0           ; Note that Local-to-Screen matrix will overwrite Local-to-World matrix in VUMEM
DIRLIGHTNORMAL_OFFSET   .assign     4
W2S_OFFSET              .assign     8
W2C_OFFSET              .assign     12
DIRLIGHTCOLOR_OFFSET    .assign     16
C2S_OFFSET              .assign     20

STACK_OFFSET            .assign     1023        ; (This was 1024 before, which was conflicting with the dummy XGKick...)

CLIPFAN_OFFSET          .assign     30

CLIP_WORK_BUF_0         .assign     61
CLIP_WORK_BUF_1         .assign     101

INBUF_SIZE              .assign     204         ; Max NbrVerts (51 * 4)
DUMMY_XGKICK_BUF        .assign     1023

   --enter
   --endenter

   ;=====================================================================================
   ; Initialization: Set the stack pointer, compute the L2S matrix, L2Clip Matrix,
   ; local light natrix, and load the light colors
   ;=====================================================================================
   IADDIU               StackPtr, VI00, STACK_OFFSET

   MatrixLoad           W2S_Matrix,  W2S_OFFSET, VI00
   MatrixLoad           L2W_Matrix,  L2W_OFFSET, VI00

   MatrixMultiply       L2S_Matrix, L2W_Matrix, W2S_Matrix
   MatrixSave           L2S_Matrix, L2S_OFFSET, VI00

   MatrixLoad           W2C_Matrix,  W2C_OFFSET, VI00
   MatrixMultiply       L2C_Matrix, L2W_Matrix, W2C_Matrix

   MatrixLoad           temp_Matrix, DIRLIGHTNORMAL_OFFSET, VI00
   LocalizeLightMatrix  LightNormals, L2W_Matrix, temp_Matrix

   MatrixLoad           LightColors, DIRLIGHTCOLOR_OFFSET, VI00


   fcset                0

   --cont

LOADPRIM:

   ;=====================================================================================
   ; Point "InputDataPtr" to the start of the incoming data
   ;=====================================================================================
   XTOP                 InputDataPtr

   ;=====================================================================================
   ; Load the giftag, and then vrite back out to the clip output space
   ;=====================================================================================
   LQ                   GifTag, 0(InputDataPtr)           ; load Triangle Fan Tag
   SQ                   GifTag, CLIPFAN_OFFSET(VI00)      ; save Triangle Fan Tag

   ;=====================================================================================
   ; If the result of ANDing 3 with the giftag's NLOOP (number of verts) is 3, the model
   ; is assumed to be a collection of triangles. Otherwise it's assumed it's a strip (-1)
   ;=====================================================================================
   ILWR.x               PrimitiveType, (InputDataPtr)x
   IADDI                Mask, VI00, 3
   IAND                 PrimitiveType, PrimitiveType, Mask

   IADDI                ClipTrigger, VI00, -1

   IBNE                 Mask, PrimitiveType, DONE

TRIANGLE:
   IADDIU               ClipTrigger, VI00, 3

DONE:
   --cont

   ;=====================================================================================
   ; This is where we'll start the processing of a batch of vertices
   ;=====================================================================================
START:
   XTOP                 InputDataPtr

   ;=====================================================================================
   ; Get the number of verts (bit 0-14) from the giftag
   ;=====================================================================================
   IADDIU               Mask, VI00, 0x7fff
   ILWR.x               NbrVerts, (InputDataPtr)x
   IAND                 NbrVerts, NbrVerts, Mask

   ;=====================================================================================
   ; Load the giftag
   ;=====================================================================================
   LQ                   GifTag, 0(InputDataPtr)

   ;=====================================================================================
   ; Point "InputDataPtr" to the real input vtx buffer (containing STQ, RGBA, NML and XYZF)
   ;=====================================================================================
   IADDIU               InputDataPtr, InputDataPtr, 1

   ;=====================================================================================
   ; These are the clip flag results for the last 3 verts.
   ; Initialize them all to be -2.
   ;=====================================================================================
;   IADDI                ClipFlag1, VI00, -2     ;;;;;;; No need to do this...
   IADDI                ClipFlag2, VI00, -2
   IADDI                ClipFlag3, VI00, -2

   ;=====================================================================================
   ; Output buffer offset is fixed so that even when NbrVerts is increased, new input
   ; data from VIF won't overwrite the previous output buffer, which may not have been
   ; sent via PATH1 yet. This pointer will be what we use to XGKICK non-clipped geometry.
   ; data_start +           (where the data starts)
   ; input buffer size +    (fixed = max nloop * XYZ/NML/RGBA/STQ)
   ; = where the output buffer can go
   ;=====================================================================================
   IADDIU               XGKickPtr, InputDataPtr, INBUF_SIZE

   ;=====================================================================================
   ; Write the giftag at the start of the output buffer
   ;=====================================================================================
   SQ                   GifTag, 0(XGKickPtr)

   ;=====================================================================================
   ; Compute the vertex output buffer pointer (containing STQ, RGBA and XYZF2)
   ;=====================================================================================
   IADDIU               OutputDataPtr, XGKickPtr, 2

   ;=====================================================================================
   ; These are the clipping space version of the last 3 verts.
   ; Initialize them at (0,0,0,1), which is sure to be within range (not clipped)
   ;=====================================================================================
;   MOVE                 CSVertex1, VF00     ;;;;;;; No need to do this...
   MOVE                 CSVertex2, VF00
   MOVE                 CSVertex3, VF00

LOOP:
   ;=====================================================================================
   ; Load the vertex
   ;=====================================================================================
   VertexLoad           Vertex, 0, InputDataPtr
   VectorLoad           Normal, 1, InputDataPtr
   VectorLoad           Color,  2, InputDataPtr
   VectorLoad           STQ,    3, InputDataPtr

   ;=====================================================================================
   ; Do the rot trans pers on the vertex, perspective-correct STQ, convert the vertex to
   ; GIF XYZ2 format, then finally save to the output buffer
   ;=====================================================================================
   MatrixMultiplyVertex VertexTransform, L2S_Matrix, Vertex
   VertexPersCorrST     VertexTransform, STQTransform, VertexTransform, STQ
   VertexFPToGsXYZF2    VertexTransform, VertexTransform
   VertexSave           VertexTransform,  1, OutputDataPtr
   VectorSave           STQTransform,    -1, OutputDataPtr

   ;=====================================================================================
   ; Lit the vertex with directional and ambient lights, convert to GIF RGBAQ format,
   ; then save to the output buffer
   ;=====================================================================================
   VertexLightDir3Amb   ColorTransform, Color, Normal, LightColors, LightNormals
   ColorFPToGsRGBAQ     ColorTransform, ColorTransform
   VectorSave           ColorTransform, 0, OutputDataPtr

   ;=====================================================================================
   ; Push the clipping-space vertex queue down 1
   ;=====================================================================================
   MOVE                 CSVertex1, CSVertex2
   MOVE                 CSVertex2, CSVertex3

   ;=====================================================================================
   ; Apply the world to clip matrix to the vertex (push onto above stack)
   ;=====================================================================================
   MatrixMultiplyVertex CSVertex3, L2C_Matrix, Vertex

   ;=====================================================================================
   ; Perform clipping judgement on the the previous 3 vertices.  sets the clipping flags
   ;=====================================================================================
   CLIPw.xyz            CSVertex1, CSVertex1
   CLIPw.xyz            CSVertex2, CSVertex2
   CLIPw.xyz            CSVertex3, CSVertex3

   ;=====================================================================================
   ; Pushing the clipping flag result down 1
   ;=====================================================================================
   IADD                 ClipFlag1, VI00, ClipFlag2
   IADD                 ClipFlag2, VI00, ClipFlag3

   ;=====================================================================================
   ; AND clipping flags with 0x3f (current judgement) and store result in VI01.
   ; VI01 will be 0 if the vertex is within the clip volume
   ;=====================================================================================
   FCAND                VI01, 0x3f

   ;=====================================================================================
   ; store result in the clip result stack
   ;=====================================================================================
   IADD                 ClipFlag3, VI00, VI01

STRIPCLIP:
   ;=====================================================================================
   ; ClipTrigger is 1, 2 or 3 if we're dealing with triangles or -1 for strips.  There's
   ; no need to run the code immediately following if we got a strip
   ;=====================================================================================
   IBLTZ                ClipTrigger, CHECK

TRIANGLE_CLIP_ONLY:
   ;=====================================================================================
   ; If it's a triangle rather than a strip, you only need to perform the clipping check
   ; every 3 verts (i.e. when ClipTrigger == 0)
   ;=====================================================================================
   IADDI                ClipTrigger, ClipTrigger, -1
   IBGTZ                ClipTrigger, AFTER_SCISSORING

   ;=====================================================================================
   ; Reset the ClipTrigger
   ;=====================================================================================
   IADDIU               ClipTrigger, VI00, 3

CHECK:
   ;=====================================================================================
   ; If all 3 verts are inside (i.e., All clip results = 0), we don't need to perform
   ; scissoring.
   ;=====================================================================================
   IADD                 ClipFlag1, ClipFlag1, ClipFlag2
   IADD                 ClipFlag1, ClipFlag1, ClipFlag3
   IBLEZ                ClipFlag1, AFTER_SCISSORING

CLIPPING_VERTEX:
   ;=====================================================================================
   ; Find out if we got one of those triangles that don't need to be displayed at all
   ;=====================================================================================

   ;=====================================================================================
   ; 0xfdf7df = 111111 011111 011111 011111
   ; FCOR returns 1 if all fields in result are 1, or 0 if they are not all 1
   ; VI01 will return 1 if all 3 verts are clipped by the -Z plane, in which case we
   ; don't have to display it at all
   ;=====================================================================================
   FCOR                 VI01, 0xfdf7df
   IBNE                 VI01, VI00, COMPLETELY_OUTSIDE

   ;=====================================================================================
   ; Ditto but this time fefbef = 111111 101111 101111 101111 (+Z)
   ;=====================================================================================
   FCOR                 VI01, 0xfefbef
   IBNE                 VI01, VI00, COMPLETELY_OUTSIDE

   ;=====================================================================================
   ; Ditto but this time ff7df7 = 111111 110111 110111 110111 (-Y)
   ;=====================================================================================
   FCOR                 VI01, 0xff7df7
   IBNE                 VI01, VI00, COMPLETELY_OUTSIDE

   ;=====================================================================================
   ; Ditto but this time ffbefb = 111111 111011 111011 111011 (+Y)
   ;=====================================================================================
   FCOR                 VI01, 0xffbefb
   IBNE                 VI01, VI00, COMPLETELY_OUTSIDE

   ;=====================================================================================
   ; Ditto but this time ffdf7d = 111111 111101 111101 111101 (-X)
   ;=====================================================================================
   FCOR                 VI01, 0xffdf7d
   IBNE                 VI01, VI00, COMPLETELY_OUTSIDE

   ;=====================================================================================
   ; Ditto but this time ffefbe = 111111 111110 111110 111110 (+X)
   ;=====================================================================================
   FCOR                 VI01, 0xffefbe
   IBNE                 VI01, VI00, COMPLETELY_OUTSIDE

   ;=====================================================================================
   ; If we get here, it means we have to scissor
   ;=====================================================================================

   ;=====================================================================================
   ; Save context
   ;=====================================================================================
   PushVertex           StackPtr, CSVertex1
   PushVertex           StackPtr, CSVertex2
   PushVertex           StackPtr, CSVertex3
   PushInteger4         StackPtr, XGKickPtr, InputDataPtr,  ClipFlag1, ClipFlag2
   PushInteger4         StackPtr, ClipFlag3, OutputDataPtr, NbrVerts,  ClipTrigger

   ;-----------------------------------------------------------------------------------------------------------------------------------
   ;=====================================================================================
   ; Setup pointers to clipping work buffers
   ;=====================================================================================
   IADDIU               ClipWorkBuf0, VI00, CLIP_WORK_BUF_0
   IADDIU               ClipWorkBuf1, VI00, CLIP_WORK_BUF_1

   ;=====================================================================================
   ; Calculate the average vertex of the 3 clipping-space verts
   ;=====================================================================================
   ADD                  AverageVertex, CSVertex1,     CSVertex2
   ADD                  AverageVertex, AverageVertex, CSVertex3
   LOI                  0.3333333
   MUL                  AverageVertex, AverageVertex, I

   ;=====================================================================================
   ; To get rid of the cracks generated by our piss-poor scissoring algorithm, scale the
   ; triangle up 1% in relation to the average vertex.
   ;=====================================================================================
   SUB                  ScaledVertex1, CSVertex1, AverageVertex
   SUB                  ScaledVertex2, CSVertex2, AverageVertex
   SUB                  ScaledVertex3, CSVertex3, AverageVertex

   LOI                  \(1.01)

   MUL                  ACC, AverageVertex, VF00[w]
   MADD                 ScaledVertex1, ScaledVertex1, I

   MUL                  ACC, AverageVertex, VF00[w]
   MADD                 ScaledVertex2, ScaledVertex2, I

   MUL                  ACC, AverageVertex, VF00[w]
   MADD                 ScaledVertex3, ScaledVertex3, I

   ;=====================================================================================
   ; Store the 3 verts in the clipping work buffer.  The first vertex is added at the end
   ; to avoid having to wrap around the vertex index when we'll scissor lines.
   ;=====================================================================================
   VertexSave           ScaledVertex1, 0, ClipWorkBuf0
   VertexSave           ScaledVertex2, 3, ClipWorkBuf0
   VertexSave           ScaledVertex3, 6, ClipWorkBuf0
   VertexSave           ScaledVertex1, 9, ClipWorkBuf0

   ;=====================================================================================
   ; Load the STQs from the original data and store them in the clipping work buffer.
   ;=====================================================================================
   VectorLoad           TempSTQ1, -5, InputDataPtr
   VectorLoad           TempSTQ2, -1, InputDataPtr
   VectorLoad           TempSTQ3,  3, InputDataPtr

   VectorSave           TempSTQ1,  2, ClipWorkBuf0
   VectorSave           TempSTQ2,  5, ClipWorkBuf0
   VectorSave           TempSTQ3,  8, ClipWorkBuf0
   VectorSave           TempSTQ1, 11, ClipWorkBuf0

   ;=====================================================================================
   ; Only execute the following code snippet if we're processing triangles
   ; (ClipTrigger >= 0).
   ;=====================================================================================
   IBLTZ                ClipTrigger, STRIP_COLOR

   ;=====================================================================================
   ; Triangle only: Load 3 colors, convert them to float, and store them in the clipping
   ; work buffer.
   ;=====================================================================================
   VectorLoad           TempColor1, -6, OutputDataPtr
   VectorLoad           TempColor2, -3, OutputDataPtr
   VectorLoad           TempColor3,  0, OutputDataPtr

   ColorGsRGBAQtoFP     TempColor1, TempColor1
   ColorGsRGBAQtoFP     TempColor2, TempColor2
   ColorGsRGBAQtoFP     TempColor3, TempColor3

   VectorSave           TempColor1,  1, ClipWorkBuf0
   VectorSave           TempColor2,  4, ClipWorkBuf0
   VectorSave           TempColor3,  7, ClipWorkBuf0
   VectorSave           TempColor1, 10, ClipWorkBuf0

   B                    PLANE_START

STRIP_COLOR:
   ;=====================================================================================
   ; Strip only: Load 1 color, convert it to fixed, and store it to the clipping work
   ; buffer.
   ;=====================================================================================
   VectorLoad           TempColor1, 0, OutputDataPtr

   ColorGsRGBAQtoFP     TempColor1, TempColor1

   VectorSave           TempColor1,  1, ClipWorkBuf0
   VectorSave           TempColor1,  4, ClipWorkBuf0
   VectorSave           TempColor1,  7, ClipWorkBuf0
   VectorSave           TempColor1, 10, ClipWorkBuf0

PLANE_START:
   ;=====================================================================================
   ; Useful for clipping: A zero vector (0.f, 0.f, 0.f, 0.f)
   ;=====================================================================================
   SUB                  Vector0000,       VF00, VF00

   ;=====================================================================================
   ; Scissor against the Z- plane
   ;=====================================================================================
   IADDIU               PreviousClipFlag, VI00, 0x800
   IADDIU               CurrentClipFlag,  VI00, 0x020
   IADDIU               NbrRotates,       VI00, 2
   IADDIU               ClipWorkBuf0,     VI00, CLIP_WORK_BUF_0
   IADDIU               ClipWorkBuf1,     VI00, CLIP_WORK_BUF_1
   IADDIU               NbrVerts,         VI00, 3
   IADDIU               NewNbrVerts,      VI00, 0
   SUB.x                PlaneSign,        VF00, VF00[w]     ; Negative

   ; Load a vertex from the buffer
   LQI                  NextVertex, (ClipWorkBuf0++)
   LQI                  NextColor,  (ClipWorkBuf0++)
   LQI                  NextSTQ,    (ClipWorkBuf0++)

LOOP_Z_MINUS:
   ; Interpolate the vertex values at the plane
   BAL                  RetAddr2, SCISSOR_INTERPOLATION

   ; Repeat until we have no more vertices
   ISUBIU               NbrVerts, NbrVerts, 1
   IBNE                 NbrVerts, VI00, LOOP_Z_MINUS

   ; Copy the triangle fan's first vertex to the end (in clipping work buffer 1)
   IADDIU               ClipWorkBuf0, VI00, CLIP_WORK_BUF_1
   BAL                  RetAddr2, SAVE_LAST_LOOP

   ; If all verts got clipped out, stop
   IBEQ                 NewNbrVerts, VI00, SCISSOR_END

   ;=====================================================================================
   ; Scissor against the Z+ plane
   ;=====================================================================================
   IADDIU               PreviousClipFlag, VI00, 0x400
   IADDIU               CurrentClipFlag,  VI00, 0x010
   IADDIU               NbrRotates,       VI00, 2
   IADDIU               ClipWorkBuf0,     VI00, CLIP_WORK_BUF_1
   IADDIU               ClipWorkBuf1,     VI00, CLIP_WORK_BUF_0
   IADD                 NbrVerts,         VI00, NewNbrVerts
   IADDIU               NewNbrVerts,      VI00, 0
   ADD.x                PlaneSign,        VF00, VF00[w]     ; Positive

   ; Load a vertex from the buffer
   LQI                  NextVertex, (ClipWorkBuf0++)
   LQI                  NextColor,  (ClipWorkBuf0++)
   LQI                  NextSTQ,    (ClipWorkBuf0++)

LOOP_Z_PLUS:
   ; Interpolate the vertex values at the plane
   BAL                  RetAddr2, SCISSOR_INTERPOLATION

   ; Repeat until we have no more vertices
   ISUBIU               NbrVerts, NbrVerts, 1
   IBNE                 NbrVerts, VI00, LOOP_Z_PLUS

   ; Copy the triangle fan's first vertex to the end (in clipping work buffer 0)
   IADDIU               ClipWorkBuf0, VI00, CLIP_WORK_BUF_0
   BAL                  RetAddr2, SAVE_LAST_LOOP

   ; If all verts got clipped out, stop
   IBEQ                 NewNbrVerts, VI00, SCISSOR_END

   ;=====================================================================================
   ; Scissor against the X- plane
   ;=====================================================================================
   IADDIU               PreviousClipFlag, VI00, 0x080
   IADDIU               CurrentClipFlag,  VI00, 0x002
   IADDIU               NbrRotates,       VI00, 0
   IADDIU               ClipWorkBuf0,     VI00, CLIP_WORK_BUF_0
   IADDIU               ClipWorkBuf1,     VI00, CLIP_WORK_BUF_1
   IADD                 NbrVerts,         VI00, NewNbrVerts
   IADDIU               NewNbrVerts,      VI00, 0
   SUB.x                PlaneSign,        VF00, VF00[w]     ; Negative

   ; Load a vertex from the buffer
   LQI                  NextVertex, (ClipWorkBuf0++)
   LQI                  NextColor,  (ClipWorkBuf0++)
   LQI                  NextSTQ,    (ClipWorkBuf0++)

LOOP_X_MINUS:
   ; Interpolate the vertex values at the plane
   BAL                  RetAddr2, SCISSOR_INTERPOLATION

   ; Repeat until we have no more vertices
   ISUBIU               NbrVerts, NbrVerts, 1
   IBNE                 NbrVerts, VI00, LOOP_X_MINUS

   ; Copy the triangle fan's first vertex to the end (in clipping work buffer 1)
   IADDIU               ClipWorkBuf0, VI00, CLIP_WORK_BUF_1
   BAL                  RetAddr2, SAVE_LAST_LOOP

   ; If all verts got clipped out, stop
   IBEQ                 NewNbrVerts, VI00, SCISSOR_END

   ;=====================================================================================
   ; Scissor against the X+ plane
   ;=====================================================================================
   IADDIU               PreviousClipFlag, VI00, 0x040
   IADDIU               CurrentClipFlag,  VI00, 0x001
   IADDIU               NbrRotates,       VI00, 0
   IADDIU               ClipWorkBuf0,     VI00, CLIP_WORK_BUF_1
   IADDIU               ClipWorkBuf1,     VI00, CLIP_WORK_BUF_0
   IADD                 NbrVerts,         VI00, NewNbrVerts
   IADDIU               NewNbrVerts,      VI00, 0
   ADD.x                PlaneSign,        VF00, VF00[w]     ; Positive

   ; Load a vertex from the buffer
   LQI                  NextVertex, (ClipWorkBuf0++)
   LQI                  NextColor,  (ClipWorkBuf0++)
   LQI                  NextSTQ,    (ClipWorkBuf0++)

LOOP_X_PLUS:
   ; Interpolate the vertex values at the plane
   BAL                  RetAddr2, SCISSOR_INTERPOLATION

   ; Repeat until we have no more vertices
   ISUBIU               NbrVerts, NbrVerts, 1
   IBNE                 NbrVerts, VI00, LOOP_X_PLUS

   ; Copy the triangle fan's first vertex to the end (in clipping work buffer 0)
   IADDIU               ClipWorkBuf0, VI00, CLIP_WORK_BUF_0
   BAL                  RetAddr2, SAVE_LAST_LOOP

   ; If all verts got clipped out, stop
   IBEQ                 NewNbrVerts, VI00, SCISSOR_END

   ;=====================================================================================
   ; Scissor against the Y- plane
   ;=====================================================================================
   IADDIU               PreviousClipFlag, VI00, 0x200
   IADDIU               CurrentClipFlag,  VI00, 0x008
   IADDIU               NbrRotates,       VI00, 1
   IADDIU               ClipWorkBuf0,     VI00, CLIP_WORK_BUF_0
   IADDIU               ClipWorkBuf1,     VI00, CLIP_WORK_BUF_1
   IADD                 NbrVerts,         VI00, NewNbrVerts
   IADDIU               NewNbrVerts,      VI00, 0
   SUB.x                PlaneSign,        VF00, VF00[w]     ; Negative

   ; Load a vertex from the buffer
   LQI                  NextVertex, (ClipWorkBuf0++)
   LQI                  NextColor,  (ClipWorkBuf0++)
   LQI                  NextSTQ,    (ClipWorkBuf0++)

LOOP_Y_MINUS:
   ; Interpolate the vertex values at the plane
   BAL                  RetAddr2, SCISSOR_INTERPOLATION

   ; Repeat until we have no more vertices
   ISUBIU               NbrVerts, NbrVerts, 1
   IBNE                 NbrVerts, VI00, LOOP_Y_MINUS

   ; Copy the triangle fan's first vertex to the end (in clipping work buffer 1)
   IADDIU               ClipWorkBuf0, VI00, CLIP_WORK_BUF_1
   BAL                  RetAddr2, SAVE_LAST_LOOP

   ; If all verts got clipped out, stop
   IBEQ                 NewNbrVerts, VI00, SCISSOR_END

   ;=====================================================================================
   ; Scissor against the Y+ plane
   ;=====================================================================================
   IADDIU               PreviousClipFlag, VI00, 0x100
   IADDIU               CurrentClipFlag,  VI00, 0x004
   IADDIU               NbrRotates,       VI00, 1
   IADDIU               ClipWorkBuf0,     VI00, CLIP_WORK_BUF_1
   IADDIU               ClipWorkBuf1,     VI00, CLIP_WORK_BUF_0
   IADD                 NbrVerts,         VI00, NewNbrVerts
   IADDIU               NewNbrVerts,      VI00, 0
   ADD.x                PlaneSign,        VF00, VF00[w]     ; Positive

   ; Load a vertex from the buffer
   LQI                  NextVertex, (ClipWorkBuf0++)
   LQI                  NextColor,  (ClipWorkBuf0++)
   LQI                  NextSTQ,    (ClipWorkBuf0++)

LOOP_Y_PLUS:
   ; Interpolate the vertex values at the plane
   BAL                  RetAddr2, SCISSOR_INTERPOLATION

   ; Repeat until we have no more vertices
   ISUBIU               NbrVerts, NbrVerts, 1
   IBNE                 NbrVerts, VI00, LOOP_Y_PLUS

   ; Copy the triangle fan's first vertex to the end (in clipping work buffer 0)
   IADDIU               ClipWorkBuf0, VI00, CLIP_WORK_BUF_0
   BAL                  RetAddr2, SAVE_LAST_LOOP

   ; If all verts got clipped out, stop
   IBEQ                 NewNbrVerts, VI00, SCISSOR_END

   ;=====================================================================================
   ; If we get here, we gotta draw the triangle fan
   ;=====================================================================================
   IADDIU               ClipWorkBuf0, VI00, CLIP_WORK_BUF_0
   IADDIU               ClipWorkBuf1, VI00, CLIP_WORK_BUF_1
   IADD                 NbrVerts,     VI00, NewNbrVerts

   IADDIU               DummyXGKickPtr, VI00, DUMMY_XGKICK_BUF
   IADDIU               XGKickPtr, VI00, CLIPFAN_OFFSET
   IADD                 InputDataPtr, VI00, ClipWorkBuf0
   IADDIU               OutputDataPtr, XGKickPtr, 2

   ; Clear the GifTag's NLOOP to 0 (for a dummy XGKick stall)
   IADDIU               Mask, VI00, 0x7fff
   IADDIU               Mask, Mask, 0x01
   ISW.x                Mask, 0(DummyXGKickPtr)

   XGKICK               DummyXGKickPtr

   MatrixLoad           C2S_Matrix, C2S_OFFSET, VI00

   ; Set the GifTag EOP bit to 1 and NLOOP to the number of vertices
   IADDIU               Mask, NbrVerts, 0x7fff
   IADDIU               Mask, Mask, 0x01
   ISW.x                Mask, 0(XGKickPtr)

LOOP2:
   ;=====================================================================================
   ; Load a clip-space vertex, apply clip-to-screen matrix, perspective-correct it (and
   ; its STQ), convert it to GS format and then save it
   ;=====================================================================================
   VertexLoad           Vertex, 0, InputDataPtr
   VectorLoad           Color,  1, InputDataPtr
   VectorLoad           STQ,    2, InputDataPtr

   MatrixMultiplyVertex VertexTransform, C2S_Matrix, Vertex
   VertexPersCorrST     VertexTransform, STQTransform, VertexTransform, STQ
   VertexFPtoGSXYZF2    VertexTransform, VertexTransform

   VertexSave           VertexTransform,  1, OutputDataPtr
   VectorSave           STQTransform,    -1, OutputDataPtr

   ;=====================================================================================
   ; Convert the color to GS format and output it
   ;=====================================================================================
   ColorFPtoGSRGBAQ     Color, Color
   VectorSave           Color, 0, OutputDataPtr

   ;=====================================================================================
   ; Next vertex...
   ;=====================================================================================
   IADDIU               InputDataPtr, InputDataPtr, 3
   IADDIU               OutputDataPtr, OutputDataPtr, 3

   IADDI                NbrVerts, NbrVerts, -1
   IBNE                 NbrVerts, VI00, LOOP2

   ; --- send result to GIF and stop ---
   XGKICK               XGKickPtr

SCISSOR_END:
;-----------------------------------------------------------------------------------------------------------------------------------

   ;=====================================================================================
   ; Restore context
   ;=====================================================================================
   PopInteger4          StackPtr, ClipFlag3, OutputDataPtr, NbrVerts, ClipTrigger
   PopInteger4          StackPtr, XGKickPtr, InputDataPtr, ClipFlag1, ClipFlag2
   PopVertex            StackPtr, CSVertex3
   PopVertex            StackPtr, CSVertex2
   PopVertex            StackPtr, CSVertex1

COMPLETELY_OUTSIDE:
   ;=====================================================================================
   ; We don't need to draw this triangle after all...  Set the ADC bit (0x8000)
   ;=====================================================================================
   IADDIU               Mask, VI00, 0x7fff
   IADDI                Mask, Mask, 1
   ISW.w                Mask, 1(OutputDataPtr)

AFTER_SCISSORING:

   ;=====================================================================================
   ; We don't need scissoring.  All 3 vertices are inside
   ;=====================================================================================

   ;=====================================================================================
   ; Next vertex...
   ;=====================================================================================
   IADDIU               InputDataPtr, InputDataPtr, 4
   IADDIU               OutputDataPtr, OutputDataPtr, 3

   IADDI                NbrVerts, NbrVerts, -1
   IBNE                 NbrVerts, VI00, LOOP

   ;=====================================================================================
   ; We're ready to kick the poly packet
   ;=====================================================================================
   XGKICK               XGKickPtr

   ;=====================================================================================
   ; We're done with this batch
   ;=====================================================================================
   B                    DONE

;--------------------------------------------------------------
; Add first element to end of the list for triangle fan
;--------------------------------------------------------------
SAVE_LAST_LOOP:
   VertexLoad           TempVertex, 0, ClipWorkBuf0
   VectorLoad           TempColor,  1, ClipWorkBuf0
   VectorLoad           TempSTQ,    2, ClipWorkBuf0

   VertexSave           TempVertex, 0, ClipWorkBuf1
   VectorSave           TempColor,  1, ClipWorkBuf1
   VectorSave           TempSTQ,    2, ClipWorkBuf1
   iaddiu               ClipWorkBuf1, ClipWorkBuf1, 3

   JR                   RetAddr2

;--------------------------------------------------------------------------------------------------
; Scissor a line going from "CurrentVertex" to "NextVertex" to a clipping plane.
;--------------------------------------------------------------------------------------------------
; Input:  "CurrentVertex" and "NextVertex" is the line
;         "ClipWorkBuf0" must point on the input vertex buffer
;         "ClipWorkBuf1" must point on the output vertex buffer
;         "NewNbrVerts" is the current number of output vertices
; Output: "ClipWorkBuf0" will be incremented to the next input vertex
;         "ClipWorkBuf1" will contain new verts, and will be incremented to the next output vertex
;         "NewNbrVerts" will be incremented according to 'ClipWorkBuf1'
;--------------------------------------------------------------------------------------------------
SCISSOR_INTERPOLATION:
   ;----------------------------------------
   ; The next vertex becomes the current one
   ;----------------------------------------
   ADD                  CurrentVertex, Vector0000, NextVertex
   ADD                  CurrentColor,  Vector0000, NextColor
   ADD                  CurrentSTQ,    Vector0000, NextSTQ

   ;---------------------------------------
   ; Update the indices for the next vertex
   ;---------------------------------------
   LQI                  NextVertex, (ClipWorkBuf0++)
   LQI                  NextColor,  (ClipWorkBuf0++)
   LQI                  NextSTQ,    (ClipWorkBuf0++)

   ;----------------------------------------------------------
   ; Test if the 2 vertices are on different side of the plane
   ;----------------------------------------------------------
   CLIPw.xyz            CurrentVertex, CurrentVertex[w]
   CLIPw.xyz            NextVertex, NextVertex[w]
   FCGET                VI01

   ;------------------------------------------------------
   ; If the result is 0, then the first vertex is "inside"
   ;------------------------------------------------------
   IAND                 ClipFlag1, VI01, PreviousClipFlag
   IBEQ                 ClipFlag1, VI00, CUR_IN

CUR_OUT:
   ;------------------------------------------------------
   ; The first vertex is "outside".  Check the second one.
   ;------------------------------------------------------
   IAND                 ClipFlag1, VI01, CurrentClipFlag
   IBEQ                 ClipFlag1, VI00, CO_NEXT_IN

CO_NEXT_OUT:
   ;------------------------------------------------------------------
   ; Both vertices are "outside".  Don't need to interpolate anything.
   ;------------------------------------------------------------------
;   B                    SCISSOR_INTERPOLATION_END
   JR                   RetAddr2

CO_NEXT_IN:
   ;---------------------------------------------------------------------
   ; The first vertex is "outside", and the second is "inside".  Find the
   ; point of intersection, and store it in the clipping work buffer.
   ;---------------------------------------------------------------------
   BAL                  RetAddr1, INTERPOLATE

   SQI                  NewVertex, (ClipWorkBuf1++)
   SQI                  NewColor,  (ClipWorkBuf1++)
   SQI                  NewSTQ,    (ClipWorkBuf1++)

   IADDIU               NewNbrVerts, NewNbrVerts, 1

;   B                    SCISSOR_INTERPOLATION_END
   JR                   RetAddr2

CUR_IN:
   ;-----------------------------
   ; The first vertex is "inside"
   ;-----------------------------
   IAND                 ClipFlag1, VI01, CurrentClipFlag
   IBEQ                 ClipFlag1, VI00, CI_NEXT_IN

CI_NEXT_OUT:
   ;-----------------------------------------------------------
   ; The first vertex is "inside", and the second is "outside".
   ; Find the point of intersection, and store both the first
   ; and new vertices in the clipping work buffer.
   ;-----------------------------------------------------------
   BAL                  RetAddr1, INTERPOLATE

   SQI                  CurrentVertex, (ClipWorkBuf1++)
   SQI                  CurrentColor,  (ClipWorkBuf1++)
   SQI                  CurrentSTQ,    (ClipWorkBuf1++)

   SQI                  NewVertex, (ClipWorkBuf1++)
   SQI                  NewColor,  (ClipWorkBuf1++)
   SQI                  NewSTQ,    (ClipWorkBuf1++)

   IADDIU               NewNbrVerts, NewNbrVerts, 2

;   B                    SCISSOR_INTERPOLATION_END
   JR                   RetAddr2

   ;---------------------------------------------------------------------
   ; Both vertices are "inside". Don't need to interpolate anything.  But
   ; still need to store the first vertex in the clipping work buffer.
   ;---------------------------------------------------------------------
CI_NEXT_IN:
   SQI                  CurrentVertex, (ClipWorkBuf1++)
   SQI                  CurrentColor,  (ClipWorkBuf1++)
   SQI                  CurrentSTQ,    (ClipWorkBuf1++)

   IADDIU               NewNbrVerts, NewNbrVerts, 1

   ;-------------------
   ; Scissoring is over
   ;-------------------
SCISSOR_INTERPOLATION_END:
   JR                   RetAddr2

;--------------------------------------------------------------------------------------------------
; Calculate the point of insersection between a clip plane and a line going from "CurrentVertex" to
; "NextVertex" (in clipping space). The intersection point is returned in "NewVertex".
;--------------------------------------------------------------------------------------------------
; Input:  "CurrentVertex" and "NextVertex" is the line
;         "PlaneSign" is the sign of the clipping plane
;         "NbrRotate" is 0 for X, 1 for Y, and 2 for Z plane
; Output: "NewVertex" is the intersection point
;--------------------------------------------------------------------------------------------------
INTERPOLATE:
   ;---------------------------------------------------------------------------------
   ; Find the distance of the current vertex to the clipping plane. In clip space the
   ; extents of the clipping regions are all vf17w for the clip test so the clipping
   ; plane in clip space is simply vf17w * the sign of the plane (vf30x)
   ;---------------------------------------------------------------------------------
   MUL.w                Temp1, CurrentVertex, PlaneSign[x]        ; Temp1 = CurrentVertex - (sign * CurrentVertex[w])
   SUB                  Temp1, CurrentVertex, Temp1[w]            ;

   ;-----------------------------------
   ; Same thing but for NextVertex
   ;-----------------------------------
   MUL.w                Temp2, NextVertex, PlaneSign[x]           ; Temp1 = NextVertex - (sign * NextVertex[w])
   SUB                  Temp2, NextVertex, Temp2[w]               ;

   ;-------------------------------------------
   ; Rotate the vertices around until the field
   ; we are checking against is in the x field
   ;-------------------------------------------
   IADD                 NbrRotations, VI00, NbrRotates

LOOP_ROT:
   IBEQ                 NbrRotations, VI00, LOOP_ROT_END

   MR32                 Temp1, Temp1
   MR32                 Temp2, Temp2

   ISUBIU               NbrRotations, NbrRotations, 1
   B                    LOOP_ROT

LOOP_ROT_END:
   ;---------------------------------------------------------
   ; Calculate the difference between Current and Next vertex
   ;---------------------------------------------------------
   SUB.x                Difference, Temp2, Temp1

   ;---------------------------------------------------------------
   ;ok so now divide current by the ratio between current and previous vectors
   ;and get the abs version
   ;q = (current.x - current.w * cplane)/(next.x-next.x * cplane)
   ;---------------------------------------------------------------
   DIV                  Q, Temp1[x], Difference[x]
   ADD.x                Answer, vf00, Q
   ABS.x                Answer, Answer          ; ans = |(c-cw)/((n-nw)+(c-cw))|

   ;-------------------------------------------------------------------
   ; NewVertex = ((NextVertex - CurrentVertex) * Ratio) + CurrentVertex
   ;-------------------------------------------------------------------
   SUB                  NewVertex, NextVertex, CurrentVertex
   SUB                  NewColor,  NextColor,  CurrentColor
   SUB                  NewSTQ,    NextSTQ,    CurrentSTQ

   MUL                  NewVertex, NewVertex, Answer[x]
   MUL                  NewColor,  NewColor,  Answer[x]
   MUL                  NewSTQ,    NewSTQ,    Answer[x]

   ADD                  NewVertex, NewVertex, CurrentVertex
   ADD                  NewColor,  NewColor,  CurrentColor
   ADD                  NewSTQ,    NewSTQ,    CurrentSTQ

   ;-----------------------------------
   ; Done
   ;-----------------------------------
   JR                   RetAddr1

   --exit
   --endexit

   .end


