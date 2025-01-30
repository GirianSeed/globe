[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
       Copyright (C) 2002 Sony Computer Entertainment Inc.
 				       All Rights Reserved
 
Vu0 Micro Mode Sample/Fur Ball Sample


<Description>
	This sample program uses the Vu0 in micro mode. It executes operations simultaneously in the EE core and Vu0 to represent fur (or string) growing on a ball.
	The dynamics calculations that are used to represent fur are executed by Vu0, and the detection of a collision between a fur particle's position (one strand of fur consists of four vertices or particles) and the ball is performed by the EE Core.
	HiG/HiP is used for rendering.

<Note>
	vcl 1.33 or later is required to create the *.vsm files for this sample program.
 
<Vu0 and EE data flows>
	First, the data (particle positions) are transferred from main memory (MM) to Vu0 memory using DMA.
	Next, when the data is processed in Vu0, it is transferred to the scratch pad memory (SPR), then returned to main memory.
	Since there is a limited amount of Vu0 memory available, the data is processed by dividing it into slices.

	While the EE Core is processing the N-th data slice in the SPR, the old (N-2)-th data slice is written back to main memory from the SPR using DMA. At the same time, the (N-1)-th data slice is also processed by Vu0. (See the table below.)
	SPR double buffering is used to perform these operations simultaneously.

        +--------------------------------------------------------------------+
EE Core:|            EE Core collision detection, slice N                    |
        + -----------------+----+-------------------+----+-------------------+
DMA:    | MM->Vu0,slice N-1|    | SPR->MM,slice N-2 |    | Vu0->SPR,slice N-1|
        +------------------+----+-------------------+----+-------------------+
Vu0:                       |  Vu0 phys. sim.,slice N-1   |
     	                   +-----------------------------+
 
<Physical Simulation>
	The motion and constraints of each particle are calculated by Vu0 (for the most part, the distance between particles is fixed), and collisions between particles and the ball are processed in the EE Core.

	This sample program uses verlet integration for the physical simulation. Verlet integration uses the x position and previous x_old position to update the x position of a particle without saving the particle's position or speed.

 	  x_new=2*x-x_old+faccel*tstep*tstep
 	  x_old=x
 	  x=x_new
 
	After a particle is updated, the following constraints apply.

	Condition a:  For the most part, the distance between neighboring particles is fixed. (Vu0)
	Condition b:  A particle must not pass through the ball. (EE Core)
 
<Rendering>
	Since particles are located in world coordinates, no transformation from local coordinates to world coordinates is performed.
	To execute lighting calculations, one normal line is required for each particle. An approximate value for the normal is obtained using a normalized vector from the center of the ball to each particle.

<Files>
	main.c			Source code of system part of sample program
	furball.c		Sample program source code
	furball.h		Sample program inline functions
	util.c			Utility program source code
	camera.c		Camera setup program source code
	light.c			Light source-related program source code
	sample_func.tbl		Sample program function registration file
	hairsim.vcl		Vu0-micro VCL (VU Command Line) source code used for physical simulation of particles
	hairshading.vcl		Vu0-micro VCL (VU Command Line) source code used for particle rendering
	makevsm			*.vcl -> *.vsm script
	*.vsm			VSM source code
	*.bin			Binary data file
	*.s			ASCII data file
 
<data-files>
	data/hair4.bin    	Rendering model
	data/ball.bin     	Rendering model
 
<Execution Method>
 
	% make run
 
<Controller Operation>
	When the program is started up, the sample menu will be displayed. The program begins with this menu.
 	  
<Menu Operation>
	You can return to the menu screen from the sample screen by pressing the START button.
 
<Sample Mode Operation>
	START button:  Return to the menu
 
	This sample program has multiple operating modes. The mode can be switched by pressing the SELECT button. For information about the current mode and usage method, refer to the screen display.
 	  
<Model Movement Modes>
	Left/Right/Up/Down (direction buttons) and L1/L2 buttons	:  Rotate object
	Square/Circle/Triangle/X/R1/R2 buttons			:  Translate object
 
<Parameter Modes>
	Triangle/X buttons		:  Change fur stiffness
	Square/Circle buttons		:  Change fur length
	R1/R2 buttons			:  Change simulation time (fur motion speed:  slow/fast)
 
<Light Source Control Modes>
	Left/Right (direction buttons)	:  Switch light to be controlled (0 to 2)
	Up/Down direction buttons)	:  Switch light component to be controlled (Point, Vector, Color, Intensity, Angle)
 
 	  - For Point
 		  Square/Circle buttons	:  Move X-axis light position 
 		  Triangle/X buttons:  Move Y-axis light position 
 		  R1/R2 buttons		:  Move Z-axis light position 
 	  
 	  - For Vector
 		  Triangle/X buttons:  Change value of X-direction light Vector
 		  Square/Circle buttons	:  Change value of Y-direction light Vector
 		  R1/R2 buttons		:  Change value of Z-direction light Vector
 
 	  - For Color
 		  Triangle/X buttons:  Change Red value of light Color
 		  Square/Circle buttons	:  Change Green value of light Color
 		  R1/R2 buttons		:  Change Blue value of light Color
 
 	  - For Intensity
 		  Triangle/X buttons:  Change Intensity value
 
 	  - For Angle
 		  Triangle/X buttons:  Change light Angle
 
	This sample program only uses directional light sources.
 
<Camera Control Modes>
	Up/Down (direction buttons)	:  Rotate X-axis
	Left/Right (direction buttons)	:  Rotate Y-axis
	L1/L2 buttons			:  Rotate Z-axis
	Square/Circle buttons		:  Move camera left or right (x-direction movement)
	Triangle/X buttons		:  Move camera up or down (y-direction movement)
	R1/R2 buttons			:  Move camera forward or back (z-direction movement)
	
	The camera rotates around the world coordinate origin.

