[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 2000 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program: Texture mapping 
+ Anti-aliasing with LINE (one-pass version)


<Description>
This program adds an anti-aliasing function using AA1 LINE to the sample program that performs specular brightness calculations and texture mapping.

Backface clipping is added to the AA1 LINE display so that smoothing can be applied cleanly even for a CUBE or other object that is not smooth.

<Keypoint>
In the case of refmap-onepass-AA1line, antialiasing was applied to smooth shapes, so no AA1line hidden surface elimination was required.

If the outline of a hidden surface of a shape such as a cube is traced using AA1line, the color of the hidden surface will be alpha blended, and an artifact will be generated. To prevent this, make sure that AA1line does not draw around back-facing surfaces.


<Files>

buff0.dsm:  	Packet for setting up texture transfer
buff1.dsm:  	Packet for setting up TEX0_1 register
r.vsm:  	Microcode for specular, reflection mapping 
		(not optimized)
r_opt.vsm:  	Microcode for specular, reflection mapping 
		(optimized)
main.c:  	Main function
mathfunc.c:  	Various math functions
mathfunc.h:  	Definitions of various parameters
add_object.c:  	Curved surface data generation program
packet.dsm:  	Packet data flowing to path1/path2
matrix.c:  	Function for setting various types of matrices
reftex.dsm:  	Texture for reflection mapping

<Run method>

        % make:  Compile sample
        % make run:  Run sample

        The following method can also be used 
	to run the program after compilation.
        % dsedb
        > run main.elf

<Controller Operation Methods>

up, down, left, right  (direction keys):  Rotate camera up, down, left and right
Triangle, Cross, Square, Circle buttons:  Rotate light up, down, left and right
L1 button:  Switch anti-aliasing ON/OFF
L2 button:  Toggle TFX: Modulate -> Decal -> Highlight -> Highlight2 (Highlight and Highlight2 are the same in this case)
R1 button:  Advance camera
R2 button:  Pull camera back
START button:  Switch Texture Mapping ON/OFF
SELECT button:  Drawing AA1 LINE only <-> Also Drawing Shading

<Remarks>

Anti-aliasing of minor defects can be performed by drawing a Triangle or Triangle Strip with AA1 off, then overwriting the outline or candidate outline lines using AA1 LINE.

Backface clipping is effective for polyhedron shapes in which normal lines differ according to the surface, or for extremely detailed curved surface polygons. However, when there are few polygons or the normal lines are shared by various vertices, it may also act in the reverse manner and make the polyhedron shape more conspicuous.

You must decide whether or not to use backface clipping depending on the subject.

