[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
        Copyright (C) 2002 Sony Computer Entertainment Inc.
                                        All Rights Reserved 

Rigid Body Dynamics Sample

<Description>

This program executes several rigid body dynamics simulations. 

To detect a collision between objects, the program uses the collision detection functions of the samples in advanced/collision.

For further information about the algorithms that are used and restrictions, refer to overview.txt.


<Files>

	main.c		:  Sample program system source code
	sample*.c	:  Sample program source code
	dynamics.c	:  Rigid body simulation functions
	collision.c	:  Collision detection functions
	vector.h	:  Inline functions containing vector/matrix arithmetic functions used by collision.c
        util.c		:  Utility program source code
        camera.c	:  Program source code for camera settings
        light.c		:  Program source code related to light source settings
        sample_func.tbl	:  Files containing registered sample program functions
        vu0Coll.vcl	:  VCL (VU command line) source code related to Vu0 microprogram used by collision algorithm
       vu0Coll.vsm	:  VSM source code
       data/*.bin	:  Binary data files
	 data/*.s	:  ASCII data files


	<Data Files>
		data/block.bin		:  Drawing model
		data/ellipsoid.bin	:  Drawing model
		data/dpyramid.bin	:  Drawing model
		data/plane1.bin		:  Drawing model
		data/plane2.bin		:  Drawing model
		data/plane3.bin		:  Drawing model
		data/cube.bin		:  Drawing model
		data/ball.bin		:  Drawing model

(For information about the gjkobj-file format, refer to advanced/collision/overview_j.txt.)
            	data/cblock.bin		:  Collision detection gjkobj-file 
            	data/cellipsoid.bin	:  Collision detection gjkobj-file 
		data/cdpyramid.bin	:  Collision detection gjkobj-file 
		data/cplane1.bin	:  Collision detection gjkobj-file 
		data/cplane2.bin	:  Collision detection gjkobj-file 
		data/cplane3.bin	:  Collision detection gjkobj-file 
		data/ccube.bin		:  Collision detection gjkobj-file 
		data/cball.bin		:  Collision detection gjkobj-file 

<Execution>

        % make run


<Controller Operations>

When the program is started up, the sample menu is displayed. The program begins with this menu.

<Menu Operations>

The START button can be pressed to return to the menu screen from the sample screen.

	<Sample Mode Operations>
	 START button:  Return to the menu

This sample can be used in various operating modes.

Pressing the SELECT button changes the mode. When the button is pressed, the currently selected mode and usage method are displayed on-screen.
 
The following modes can be selected.

	<Camera Move Mode>  Set camera parameters
	<Light Ctrl Mode>   Set light parameters (directional light)
	<Force Mode>  Apply force to selected object
	<Select Obj + Type Mode>  Select object and set it to ACTIVE or PASSIVE
		ACTIVE ... Object participates in all simulation processes.
		PASSIVE ... Gravity does not act on the object and the object is treated as if it has infinite mass.


<Remarks>

See also overview.txt.
	
For a description of the collision algorithm, see advanced/collision/*.txt.

CPU and DMA usage also include amounts used for debug messages.

  
