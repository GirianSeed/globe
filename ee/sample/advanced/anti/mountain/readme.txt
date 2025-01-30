[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
        Copyright (C) 2000 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program: anti-aliasing by line overwriting (no Z sort)

<Description>
This program shows how to achieve an anti-aliasing effect by overwriting the AA1 line.

An anti-aliasing effect can be obtained by overwriting the AA1 line on the edges of a polygon. This program illustrates this by creating two-dimensional mesh data, then creates line data and overwrites it.

This enables aliasing of the silhouettes to be reduced and T gaps at mesh LOD change points to be reduced.

<Files>
	
Makefile:  	make file
clip.c:  	Clipping routine
devinit.c:  	Device initialization routine
dma_util.c:  	DMA routine
gterrain.c:  	Terrain generation routine
gterrain_packet.c:  Terrain packet generation routine
main.c:  	Main program
matrix_util.c:  Matrix calculation routine
misc_packet.c:  Other packet generation routine
random.c:  	Random number routine
texload.c:  	Texture loading routine
view_util.c:  	Calculation routine from matrix from viewpoint
terrain.h:  	Structures
constant.h:  	Constants
scale.h:  	Scale-dependent constants
testconst.h:  	VU1 microprogram constants
*.raw:  	Texture files
work.dsm:  	DMA packet
fsub.vsm:  	VU1 microprogram

<Run method>
	% make:  Compile

	The following method can also be used to run 
	the program after compilation.

	% dsedb
	> run main.elf

<Controller functions>
	up arrow (direction key):  Forward
	down arrow:  Backward
	right arrow:  Right
	left arrow:  Left
	Circle button:  Rotate right
	Square button:  Rotate left
	Triangle button:  Rotate down
	Cross button:  Rotate up
	R1 button:  Move further away
	R2 button:  Move closer
	L1 button:  Move up
	L2 button:  Move down

	SELECT button +
	R1 button:  Polygon OFF
	L1 button:  Polygon ON
	L2 button:  AA1 line OFF
	R2 button:  AA1 line ON
	
<Explanation of program> 
For a description of the program's memory map and other information, refer to the mountain.ppt file. 

