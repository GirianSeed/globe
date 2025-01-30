[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 2000 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program: Reflection mapping, specular 
+ Anti-aliasing by LINE (one-pass version)


<Description>
This program adds an AA1 LINE anti-aliasing function to the sample program that performs specular brightness calculations and reflection mapping.

<Keypoint>
AA1 Lines are arranged in a vertical and horizontal mesh, and the arrangement differs from Triangle Strip data that is used for shading.

If the number of AA1 Lines that are collected at each vertex of the polygon differs, the alpha blend rate will change and the edge vertices will appear angular. As a result, smooth antialiasing cannot be achieved if AA1 Lines are drawn for the diagonals of the polygon.

In addition, although AA1 Lines must be drawn from the interior towards the front to achieve completely smooth antialiasing, this sample program does not perform the required sorting to do that.

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

        The following method can also be used to 
	run the program after compilation.

        % dsedb
        > run main.elf

<Controller functions>

up, down, left, right  (direction keys):  Rotate camera up, down, left and right
Triangle, Cross, Square, Circle buttons:  Rotate light up, down, left and right
L1 button:  Switch anti-aliasing ON/OFF
L2 button:  Toggle TFX: Modulate -> Decal -> Highlight -> Highlight2 (Highlight and Highlight2 are the same in this case)
R1 button:  Advance camera
R2 button:  Pull camera back
START button:  Switch texture mapping ON/OFF
SELECT button:  Draw AA1 LINE only <-> Also draw Shading

<Remarks>

Anti-aliasing of minor defects can be performed by drawing a Triangle or Triangle Strip with AA1 off, then overwriting the outline or candidate outline lines using AA1 LINE.

Although refmap-AAline overdraws a reflection mapping, this sample program uses the texture function Highlight to create a similar reflection in one-pass.

Therefore, more complete anti-aliasing can be achieved.


