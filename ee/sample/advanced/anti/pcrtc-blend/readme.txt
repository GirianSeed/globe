[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 2000  Sony Computer Entertainment Inc.
                                        All Rights Reserved


Sample program: Anti-aliasing using the PCRTC


<Description>
This program shows how to achieve an anti-aliasing effect using the PCRTC blend circuit.

A 640x448 VGA-level drawing area is held in a Dual Buffer, and the ODD and EVEN horizontal scan lines are averaged with the PCRTC blend circuit, then displayed.

To implement a 640x448 Dual Buffer that allows program display area updates to exceed 1/60 sec, the program uses a method in which the object is partitioned and the required textures are sequentially transferred to the shared texture work area before they are drawn.

<Files>
main.c:  	Main program
dma.c:  	DMA control routine
gscntrl.c:  	GS control routine
matrix.c:  	Matrix control routine
packet.c:  	Packet generation and link routine
fractalobj.c:  	Display object generation routine according to fractals
base.h:  	Configuration parameters, data structures, etc.
vu1mpg.dsm:  	VU1 microprogram packet
vu1mpg.vsm:  	VU1 microprogram

<Run method>
	% make:  Compile

	The following method can also be used to 
	run the program after compilation.

	% dsedb
	> run main.elf

<Controller functions>
Circle button:  	Control planet location
Square button:  	Control planet location
Triangle button:  	Control planet location
Cross button:  		Control planet location
R1 button:  		Control planet location
R2 button:  		Control planet location
START button:  		Reset planet location (home position)
right arrow (direction keys):  Control viewpoint location
left arrow:  		Control viewpoint location
up arrow:  		Control viewpoint location
down arrow:  		Control viewpoint location
L1 button:  		Control viewpoint location
L2 button:  		Control viewpoint location
SELECT button:  	Reset viewpoint location (home position)
Right stick:  		Rotate planet
R3 button:  		Reset planet rotation and 
			display/hide AA1 line mesh 
Left stick:  		Rotate light source
L3 button:  		Reset light source rotation 
			reset and switch ON/OFF AA using PCRTC

<Remarks>
If the AA1 line mesh is not displayed, the AA effect will hardly be noticeable.

<Explanation of program>
For an explanation of how the program works, please refer to the pcrtc-blend.ppt file.

