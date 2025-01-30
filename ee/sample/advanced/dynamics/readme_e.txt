[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                     All Rights Reserved  

Rigid Body Dynamics Sample

< Description>
This sample is an implementation of various rigid body dynamics simulations. 
For detecting the collision between object pairs the collision detection
functions from advanced/collision are used.

For a more detailed description about the used algorithm and its limitations
see overview_e.txt	

< File >       
        main.c          Source code of the system handling the sample program
        sample*.c       Source codes of the sample programs
	dynamics.c      Functions for the rigid body simulation
	collision.c     Functions for collision detection
	vector.h        Inline functions containing vector/matrix
			calculation functions used in collision.c
        util.c          Source code of utility program
        camera.c        Program source for camera setting
        light.c         Program source for light-source setting
        sample_func.tbl File with sample program functions registered
        vu0Coll.vcl     VCL (VU Command Line) source for the Vu0-micro
			used by the collision algorithm
        vu0Coll.vsm     VSM source            
        data/*.bin      Binary data files
	data/*.s        ASCII data files

	<data-files>
		data/block.bin          model for rendering
		data/ellipsoid.bin      model for rendering
		data/dpyramid.bin       model for rendering
		data/plane1.bin         model for rendering
		data/plane2.bin         model for rendering
                data/plane3.bin         model for rendering
                data/cube.bin           model for rendering
                data/ball.bin           model for rendering

		(for gjkobj-file format see advanced/collision/overview_e.txt)
                data/cblock.bin         gjkobj-file for collision detection
                data/cellipsoid.bin     gjkobj-file for collision detection
		data/cdpyramid.bin      gjkobj-file for collision detection
		data/cplane1.bin        gjkobj-file for collision detection
		data/cplane2.bin        gjkobj-file for collision detection
		data/cplane3.bin        gjkobj-file for collision detection
                data/ccube.bin          gjkobj-file for collision detection
                data/cball.bin          gjkobj-file for collision detection

< Activating the Program >

        % make run


< Using the Controller >
	When activating the program, a sample menu appears.  This menu 
	allows you to start the sample program.
	
	< Menu Operation >
	Pressing the START button down allows the sample screens to 
	return to the menu screen.

	< Sample Mode Operation>
	 START button: Returns to the menu

	There are different operation modes available in the sample.
	Each mode is changed by pressing the SELECT button.  The current 
	mode and its usage is displayed on the screen.
 
        The available modes are:
	<Camera Move Mode>      set camera parameters
	<Force Mode>            apply an additional forces to selected object
	<Select Obj +Type Mode> select an object and set it type to ACTIVE or PASSIVE       	
			        ACTIVE...the object is participating
			                full in the simulation process. 
                                PASSIVE...the object feels no gravity and is considered 
					 to have a inverse mass of 0

< Notes >
  See also overview_e.txt as a reference.
  For an explanation of the collision algorithm see advanced/collision/*.txt
  The CPU and DMA usage displayed includes also the debug-messages.
  



