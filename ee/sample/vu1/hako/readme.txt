[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Perspective transformation using VU1

<Description>

This sample program shows how to perform perspective transformation (only) using VU1.

<Files>
	sample.c	Main program
	work.dsm	Object data (DMA packet)
	work.vsm	VU1 microprogram

<Startup>
	% make :  Compile
	% make run :  Execute

The following method can also be used to execute the program after compilation.
	% dsedb
	> run blow.elf

<Controller operation>
	None

<Remarks>

Processing steps are briefly described below.

1. Prepare rotation matrix of object to be displayed (RotMatrix()).
2. Overwrite rotation matrix of DMA packet (work.dsm) (SetMatrix()).
3. Perform DMA transfer of prepared DMA packet to path1.
4. Wait for drawing of previous frame and VSync (sceGsSyncPath(), sceGsSyncV()).
5. Return to 2.

Note
Optimization has not been performed for work.vsm to improve readability.
