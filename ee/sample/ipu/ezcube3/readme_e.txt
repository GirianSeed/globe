[SCEI CONFIDENTIAL DOCUMENT]
DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
                  Copyright (C) 1999 by Sony Computer Entertainment Inc.
                                                     All Rights Reserved

The sample program for decoding more than one compressed data

<Description of the Sample>
	This sample program is the extended version of the program in 
	/sample/ipu/ezcube. It decodes more than one compressed data
	one by one, DMA transfers to the texture buffer and shows
	them as a texture. It uses IPU efficiently by decoding the next
	data immediately after finishing decoding, with DMA ending
	interruption from IPU.

	
<File>
	main.c
	ldimage.c
	ezcube.c
	ezcube.h
	ezmovie.h
	ez.dat
	rugby.dat
	movie3.dat
	456.dat
	1_6.dat
	wada.dat

<Activating the Program>
	% make		: Compiles the program
	% make run	: Executes the program

	After the compilation, the program is also executable with the
	following.
	
	% dsedb
	> run main.elf

<Using the Controller>
	Up/Down directional button	:Rotation around x-axis
       	Left/Right directional button   :Rotation around y-axis
 	L1,L2 button          :  Rotation around z-axis 
	R1,R2 button	      :	 Translation of the cube around z-axis
        Triangle/Cross button :  Rotation around x-axis of the camera 
        Circle/Square button  :  Rotation around y-axis of the camera 
	Start button          :  Reset

<Note>

	The bug about antialiasing was found in GS of EB-2000 and EB-2000s
	Three lines below have been added for that reason.
	/* Transfers three packets below additionally due to the bug in GS
	 */
	sceVif1PkAddGsAD(&packet, SCE_GS_PRMODECONT, 
	SCE_GS_SET_PRMODECONT(0));

	sceVif1PkAddGsAD(&packet, SCE_GS_PRMODE,
			 SCE_GS_SET_PRMODE(0,0,0,0,0,0,0,0));
	sceVif1PkAddGsAD(&packet, SCE_GS_PRMODECONT, 
	SCE_GS_SET_PRMODECONT(1));

