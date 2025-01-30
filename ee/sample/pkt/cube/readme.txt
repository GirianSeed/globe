[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Three-dimensional polygon display

<Description>

This sample program shows how to use perspective transformation to display a cube, then rotate the cube independently around the x-, y-, and z-axes using the controller.

<Files>
	main.c

<Startup>
	% make :  Compile
	% make run :  Execute

	The following method can also be used to execute 
	the program after compilation.
	% dsedb
	> run main.elf

<Controller operation>
	up-arrow/down-arrow (direction keys):  x-axis rotation
	left-arrow/right-arrow:  y-axis rotation
	L1/L2 buttons:  z-axis rotation
	R1/R2 buttons:  move the cube in the direction of the z 
			    axis
	Triangle/X buttons: x-axis rotation of camera
	Circle/Square buttons: y-axis rotation of camera
	START button: reset



