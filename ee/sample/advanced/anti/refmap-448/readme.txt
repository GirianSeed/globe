[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
        Copyright (C) 2000 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Sample program: Reflection mapping, specular V direction double resolution


<Description>
This program performs specular brightness calculations and reflection mapping.

For anti-aliasing, objects are drawn in a 640x448 buffer and used bilinearly to perform reduced drawing in a 640x224 display buffer.

<Files>

buff0.dsm:  	Packet for setting up texture transfer
buff1.dsm:  	Packet for setting up TEX0_1 register
r.vsm:  	Microcode for specular, reflection 
		mapping (not optimized)
r_opt.vsm:  	Microcode for specular, reflection 
		mapping (optimized)
main.c:  	Main function
mathfunc.c:  	Various math functions
mathfunc.h:  	Definitions of various parameters
mug.dsm:  	Mug model data (texture off)
mug_tex.dsm:  	Mug model data (texture on)
packet.dsm:  	Packet data flowing to path1/path2
matrix.c:  	Function for setting various types 
		of matrices
reftex.dsm:  	Texture for reflection mapping

<Run method>

        % make:  Compile sample
        % make run:  Run sample

        The following method can also be used 
	to run the program after compilation.

        % dsedb
        > run main.elf

<Controller functions>

up, down, left, right  (direction keys):  Rotate camera up, down, left and right
Triangle, Cross, Square, Circle buttons:  Rotate light up, down, left and right
L1 button:  Display shading
L2 button:  Display line drawing
R1 button:  Advance camera
R2 button:  Pull camera back
SELECT button:  Bilinear filter (50%, 50% blend)
START button:  25%, 50%, 25% blend

<Remarks>
None

<GS buffer>
0-69	 Display buffer	640x224 PSMCT32
70-139   Drawing buffer	640x448 PSMCT16 Dither ON
140-279  Z buffer	640x448	PSMZ24
300-	 Reflection mapping texture


