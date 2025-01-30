[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sprite drawing

<Description>

This sample program illustrates high-performance drawing. The program first creates packets in scratch RAM, then by overwriting only the x,y fields, draws polygons directly from the scratch RAM using VIF1 via PATH2.

DMA transfers to scratch RAM are also used for reading the ball database.

<Files>
	balltex.s
	main.c

<Startup method>
	% make :  Compile
	% make run :  Execute

The following method can also be used to execute the program after compilation.

	% dsedb
	> run main.elf

<Controller operation>
	<up-arrow> button:  Adds balls
	<down-arrow> button:  Eliminates balls

<Remarks>
	None

