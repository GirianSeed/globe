[SCEI CONFIDENTIAL DOCUMENT]
DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
                  Copyright (C) 1999 by Sony Computer Entertainment Inc.
                                                   All Rights Reserved

Sample to Display Three-Dimensional Polygons

<Description of the Sample>	
	This is a sample program, which displays a cube by means of
  	transparency perspective conversion and rotates each of
 	the x, y and z axes independently with the controller. 
	This sample program also enables a camera to move centering 
	on the x and y axes. 

<File>	
	main.c

<Activating the Program>	
	% make		: Compiles the program.	
	% make run	: Executes the program.	

	After the compilation, the program is also executable 	
	with the following. 	
	% dsedb	
	> run main.elf

<Using the Controller>	
	Up/down directional button	: Rotates the x axis.
	Left/right directional button	: Rotates the y axis.
	L1/L2 button			: Rotates the z axis.
	R1/R2 button			: the cube moves in the z axis 
					  direction. 
	Triangle/Cross button		: the camera rotates the x axis.
	Circle/Square button 		: the camera rotates the y axis.
	start button			: Resets

<Note>	In this EB-2000, there is a bug in anti-ailiasing.
	Therefore, the following 3 codes have been added.

	/* Due to the GS bug, the below 3 packets are additionally 
	transferred.*/	
	sceVif1PkAddGsData(&packet, SCE_GS_SET_PRMODECONT(0));	
	sceVif1PkAddGsData(&packet, SCE_GS_SET_PRMODE(0, 0, 0, 0, 0, 0, 0, 0));	
	sceVif1PkAddGsData(&packet, SCE_GS_SET_PRMODECONT(1));  

	


