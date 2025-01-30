[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Clipping sample program (VU0)

<Description>

This program displays 27 cubes and can switch between object clipping based on the display area using boundary boxes and polygon clipping based on the GS rendering area. For boundary box clipping, the clipping area is set to a smaller range compared to the display area to allow the clipping operation to be more easily observed. The far-clipping plane surface or the near-clipping plane is displayed using alpha-blending. Switching between the two is also possible.

<File>
        main.c	        : Main program
        vu0.c      	: Functions for array operations 
			  such as coordinate transforms 
			  and perspective transforms
        cube.s          : object data (cube)
        torus1.s        : object data (torus data: split)
        flower.dsm   	: texture data

<Execution>
	% make		: compile
	% make run	: run

	After compilation the following can be used:

	% dsedb
	> run main.elf

<Controller operations>
	up/down arrows (direction keys) :rotate around 
					  x-axis (object)
	left/right arrows	: rotate around y-axis 
				  (object)
	L1/L2 buttons		: rotate around z-axis 
                                  (object)

	triangle/X buttons 	: rotate around x-axis 
				  (viewpoint)
	square/circle buttons	: rotate around y-axis 
				  (viewpoint)
	R1/R2 buttons		: move along z-axis 
				  (viewpoint)
	left joystick		: move along x-axis, 
				  y-axis (viewpoint)
	
	SELECT button		: switch between objects 
				  (cube/torus)
	START button		: object clipping ON/OFF
	left joystick		: clipping based on the GS 
				  rendering area can be 
				  switched ON/OFF by 
				  pushing in the left 
				  joystick (*1)
	right joystick		: clipping based on the 
				  object clipping plane 
				  can be switched 
				  (NEAR/FAR) by pushing 
				  in the right joystick

<Notes>

This sample program should be run with the ANALOG mode switch set to ON.

(*1) With object clipping OFF, use the R2 button to move the object way off the screen. Then turn the left joystick ON to apply clipping to the polygon.
