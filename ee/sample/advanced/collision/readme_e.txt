[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                     All Rights Reserved  

Collision Detection Sample

< Description>
This sample uses a GJK-algorithm implementation for collision detection.
For rendering the HiG/HiP library is used.
	
< File >       
        main.c          Source code of the system of the sample program
        sample.c        Source codes of the sample programs
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
                data/cblock.bin         gjkobj-file for collision detection
                data/cellipsoid.bin     gjkobj-file for collision detection
 

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
	<Camera Move Mode>     set camera parameters
	<Light Ctrl Mode>      set light parameters (directional lighting)
	<Obj Crl Mode>         move/rotate object0
	<Coll Crl Mode>        choose collision mode:
        COLLISION CHECK : checks if the objects are intersecting or not
                          (uses is_collision() function)
        CALC. DISTANCE : calculates the closest distance between the 
			 objects (calc_dist() is used)
        FOLLOW SHORTEST DISTANCE : moves object1 into the direction of 
			 the shortest distance until the distance
			 between the objects is smaller	than 0.01 
			(calc_dist() is used)

< Outline of Usage of the Collision Detection Functions >

  alloc_gjkobj_neighbors()             Allocates the next neighbor table for
                                       the collision objects

  init_collision()                     Loads the Vu0 micro-code into Vu0
				       program-memory

  gjk_init()                           Initializes the GJKObj and Simplex
				       structures used in the sample

  is_collision()                       Checks if a collision happened

  calc_dist()                          Calculates the shortest distance 
				       between the objects.

  free_gjkobj_neighbors()              Frees the memory used for the next 
				       neighbor table

< Notes >
  See also overview_e.txt as a reference for the used algorithm.

  The CPU and DMA usage displayed includes also the debug-messages 
  and the debug-boxes for displaying the closest points between the
  objects.
  

< Loading Data >
	In this sample program, data can be loaded from the host.
	Therefore, you should create the necessary data described above 
	in the host prior to loading the data.  The data path is described 
	in the relative path name of the directory that contains the 
	sample execution file, main.elf.
	Place the binary data under the data directory in the directory 
	that includes main.elf.

< Appendix >
	* Sample Additions by the User
	Sample programs can be added easily to these sample programs as 
	described below.

	DEFFUNC( ... ) is described in the sample_func.tbl.

	Enter the name of the sample you want to add in parentheses.

	For example, if DEFFUNC(USER_PROG) is entered, 
		> USER_PROG
	is added to the main menu of the sample program.

	By selecting this item with the circle button,
		USER_PROG_init()
	is called once then after
		USER_PROG_main()
	is called in every frame.

	As long as USER_PROG_main() returns 0, 
	USER_PROG_main() is called.
	
	When USER_PROG_main() returns the value other than 0,
		USER_PROG_exit()
	is called in the following frame to proceed to the exit process.

	A simple procedure of adding a user's own program named MY_PROGRAM 
	is as follows:

		1. Add DEFFUNC(MY_PROGRAM,comment) to the sample_func.tbl.

		2. Create a file that contains the following.
			void MY_PROGRAM_init()
			int MY_PROGRAM_main()
			void MY_PROGRAM_exit()

		3. Add the file created in the step 2 above to 
		   the source definition area of the make file.

		4. Execute the make file.

