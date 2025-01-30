[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample runtime program showing how to perform key frame matrix animation by rotation axis interpolation

<Description>

The program in the intr_key directory performs key frame animation for a joint matrix using a method known as rotation axis interpolation.

This program in the runtime directory performs the actual key frame animation by using a parameter sequence for rotation axis interpolation.

<Files>
	main.c		: Main functions and general 
			  display-related functions
	matintr.c	: Key frame animation-related 
			  functions
	matintr.h	: Header file
	axis.c		: Rotation axis interpolation-
			  related function
	axis.h		: Header file
	aseq.c		: For reading parameter sequence 
		          data
	aseq.h		: Header file
	axisseq.dat	: Actual parameter sequence data 
			 (data that was created in mkdata)
	basic.vsm	: VU1 microprogram
	dma.dsm		: General data for DMA
	torso.dsm	: Torso model data
	head.dsm	: Head model data
	arm.dsm		: Arm model data
	leg.dsm		: Leg model data

<Execution>
	% make:  Compile
	% make run:  Execute
	
After compilation, the program can also be executed using the following method:

	% dsedb
	> run main.elf

<Controller operation>
	circle, triangle, X, square, R1, R2 buttons:  
				Change camera position
	START button:  Replay

<Notes>

The parameter sequence for rotation axis interpolation can be created by using the sample program in the mkdata directory.
