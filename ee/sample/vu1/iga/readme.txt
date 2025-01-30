[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Perspective transformation and light source calculation using VU1

<Description>

This sample program shows how to perform high-speed perspective transformation and light source calculation using VU1.

<Files>
	sample.c :  Main program
	devinit.c :  Device initialization routine
	tex.h :  Texture data
	work.dvpasm :  Object data (DMA packet)
	work.vuasm :  VU1 microprogram

<Startup>
	% make :  Compile
	% make run :  Execute

The following method can also be used to execute the program after compilation.
	% dsedb
	> run sample.elf

<Controller operation>
	up-arrow (direction key) :  Accelerate forward
	down-arrow  :  Accelerate backward
	SELECT button    ÅF Stop (while pressed)

<Remarks>

Processing steps are briefly described below.

1. Prepare two path1 DMA packet areas (dpmem[2]).
2. Prepare required matrices for drawing object in SPRAM (SetMatrix()).
3. Transfer two matrices to DMA packet areas (SetMatrix()).
4. Wait for drawing of previous frame and VSync (sceGsSyncPath(), sceGsSyncV()).
5. Perform DMA transfer of prepared DMA packet to path1.
6. Return to 2.

