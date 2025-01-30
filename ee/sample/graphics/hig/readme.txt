[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2002 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample Program for the HiG (High Level Graphics) Library 


< Description of the Sample >

	This is a sample program to allow the plug-in that is registered 
	via the user interface for the HiG library to be executed.  
	Plugin refers to the ones provided by SCE.


< File >
	main.c		Source code of the system of the sample program
	sample0-32.c	Source codes of sample 0-32 programs
	util.c		Source code of utility program
	camera.c	Program source for camera setting
	light.c		Program source for light-source setting 
	sample_func.tbl	File with sample program functions registered
	micro.dsm	DVP source
	data/*.bin	Binary data file
	micro/*.vcl	VCL (VU Command Line) source
	micro/*.vsm	VSM source


< Activating the Program >

        % make run


< Using the Controller >
	This sample consists of a group of sample programs for 
	introducing the features of the high level graphics library "HiG".
	When activating the program, a sample menu appears.  This menu 
	allows you to select and start each sample program.
	
	< Menu Operation >
	 	Up/Down directional button: 	Scrolls the menu cursor
	 	Circle button: 			Enters menu (activating 
						the sample)

	Pressing the START button down allows the sample screens to 
	return to the menu screen.

	< Sample Mode Operation >
	 	START button: 			Returns to the menu

	The following operations are common to most samples.  For details, 
	refer to the descriptions of the samples and the messages 
	displayed on the screen after activating them.

	Some samples have a concept of mode.  Each mode is changed by 
	pressing the SELECT button.  The current mode is displayed on 
	the screen.
 
	CAMERA CTRL: Camera Control Mode
	 
	 	Up/Down directional button: 	Rotates around the x axis
	 	Right/Left directional button:	Rotates around the y axis
	 	L1/L2 button: 			Rotates around the z axis
	 	Square/Circle button: 		Moves in the direction of 
						the x axis
	 	Triangle/Eks button: 		Moves in the direction of 
						the y axis
		R1/R2 button: 			Moves in the direction of 
						the z axis

		A camera rotates centering around the origin point of 
		the world coordinate system.	

	MICRO CHANGE: Micro Switch Mode

	 	Square/Circle button: 		Switches microprograms

	LIGHT CTRL: Light Control Mode

	 	Right/Left directional button:	Switches light numbers
	 	Up/Down directional button: 	Switches parameters
				    		(DIRECTION, POS/ANGLE, 
						 COL/INTENS)

		- DIRECTION
	  	  Square/Circle button:	Increments/Decrements x coordinate
	  	  Triangle/Eks button: 	Increments/Decrements y coordinate
	  	  R1/R2 button: 	Increments/Decrements z coordinate

		- POS/ANGLE
	  	  Square/Circle button: Moves in the direction of 
					the x axis
	  	  Triangle/Eks button: 	Moves in the direction of 
					the y axis
	  	  R1/R2 button: 	Moves in the direction of 
					the z axis
	  	  L1/L2 button: 	Narrows/Widens angles

		- COL/INTENS
	  	  Square/Circle button: Increments/Decrements R value
	  	  Triangle/Eks button: 	Increments/Decrements G value
	  	  R1/R2 button: 	Increments/Decrements B value
	  	  L1/L2 button: 	Heightens/Lowers intensity


< Description of the Sample >

SAMPLE0
	An object display sample using the output data of esconv.
	Data is loaded internally from the host.
	A group of sample data that can be played back simply by 
	replacing.

	* Related files
		sample0.c 
		data/dino.bin
		data/shuttle.bin
		data/tri.bin
		data/logo.bin
		data/cowstrip.bin
		data/mult.bin
		data/APEWALK39.bin
		data/xyz_anim.bin

	<SAMPLE0 Operation>
		SELECT button: Replaces data

SAMPLE1
	A sample to display two objects concurrently using the output
	data of esconv.
	Data is loaded internally from the host.

	* Related files
		sample1.c 
		data/dino.bin
		data/shuttle.bin

SAMPLE2
	A sample to replace two objects by the BLOCK operation function 
	using the output data of esconv.  
	Data is loaded internally from the host.

	* Related files
		sample2.c 
		data/dino.bin
		data/shuttle.bin

	<SAMPLE2 Operation>
		SELECT button: Replaces objects

SAMPLE3
	A sample to construct plugin blocks/data blocks from the raw
	data that plugins can use with the BLOCK operation function.
	Data is loaded internally from the host.

	* Related files
		sample3.c 
		data/ball_basem.bin
		data/ball_hrchy.bin
		data/ball_micro.bin
		data/ball_anim.bin
		data/ball_key.bin
		data/ball_val.bin
		data/ball1_tex2d.bin
		data/ball1_shape.bin

SAMPLE4
	A sample to construct plugin blocks/data blocks from the raw
	data that plugins can use with the BLOCK operation function, 
	and to operate LOD (Level Of Detail) that enables the model to 
	be switched according to the camera position.

	* Related files
		sample4.c 
		data/ball_basem.bin
		data/ball_hrchy.bin
		data/ball_micro.bin
		data/ball_anim.bin
		data/ball_key.bin
		data/ball_val.bin
		data/ball1_shape.bin
		data/ball1_tex2d.bin
		data/ball2_shape.bin
		data/ball2_tex2d.bin
		data/ball3_shape.bin
		data/ball3_tex2d.bin
		data/ball4_shape.bin
		data/ball4_tex2d.bin

SAMPLE5
	A sample of primitives which can be drawn with the graphics 
	plugin library "HiP".
	This sample displays 12 models in total, i.e. point, line, 
	line strip, triangle, triangle strip, and triangle fan with 
	and without texture.

	* Related files
		sample5.c 
		data/point.bin
		data/line.bin
		data/linestrip.bin
		data/triangle.bin
		data/tristrip.bin
		data/trifan.bin
		data/point_t.bin
		data/line_t.bin
		data/linestrip_t.bin
		data/triangle_t.bin
		data/tristrip_t.bin
		data/trifan_t.bin

SAMPLE6
	A sample for switching microcodes.

	* Related files
		sample6.c 
		data/ballS.bin

SAMPLE7
	A sample for using es2raw data (framework data) with HiG.

	* Related files
		sample7.c 
		data/mbox_raw.bin

SAMPLE8
	A sample to operate internal portion of the HiG data.
	The program uses an access function and changes the values 
	within the HiG data to control the presence of animations and 
	textures.
	
	* Related files
		sample8.c 
		data/APEWALK39.bin
		data/dino.bin

SAMPLE9
	A TIM2 sample.

	* Related files
		sample9.c 
		data/compmip.tm2
		data/abc-24.tm2
		data/abc-32.tm2
		data/info8-16.tm2
		data/info8-32-128.tm2
		data/Lysol1.tm2
		data/planeclut4.tm2
		data/tim2_raw.bin

SAMPLE10
	SAMPLE1 with a feature for performing a CPU process and a DMA 
	transfer via double buffering.  This sample also controls 
	animation frames. 

	* Related files
		sample10.c 
		data/APEWALK39.bin

	<SAMPLE10 Operation>
		L1 button: Playback in the forward direction
		L2 button: Playback in the reverse direction

SAMPLE11
	A sample for synthesizing two screens using PCRTC.

	* Related files
		sample11.c 
		data/dino.bin
		data/subwin.raw

	<SAMPLE11 Operation>
		Right/Left/Up/Down directional button: Moves a subwindow
		Square/Circle/Triangle/Eks/L1/L2 button: Controls a 
					    scaling factor of a subwindow

SAMPLE12
	A sample with memory-resident or transient textures.

	* Related files
		sample12.c 
		data/restex.bin

	<SAMPLE12 Operation>
		SELECT button: Switches between resident and transient

SAMPLE13
	A sample to create subwindows (multiwindows) using GS service.

	* Related files
		sample13.c 
		data/shuttle.bin 
		data/curtain.bin
			
	<SAMPLE13 Operation>
		Right/Left/Up/Down directional button: Moves the lower 
						right subwindow
		Square/Circle/Triangle/Eks/R1/R2 button: Moves Object #2
		
SAMPLE14
	A drawing sample in texture area using GS service.
			
	* Related files
		sample14.c 
		data/APEWALK39.bin 
		data/curtain.bin
	
	<SAMPLE14 Operation>
		Right/Left/Up/Down directional and L1/L2 buttons: Rotates 
						a texture object
		Square/Circle/Triangle/Eks/R1/R2 button: Moves a texture 
						object
		
SAMPLE15
	A motion blur sample.
			
	* Related files
		sample15.c
		data/APEWALK39.bin
			
	<SAMPLE15 Operation>
		L1/L2 button: Changes blur intensity
		Right/Left/Up/Down directional button: Rotates a camera
		Square/Circle/Triangle/Eks button: Moves a camera

SAMPLE16
	Emboss bump-mapping sample.
	
	* Related files
		sample16.c
		micro/vu1embossPoint.vsm
		data/emboss0.bin 
		data/emboss1.bin
		
	<SAMPLE16 Operation>
	  ALPHA CTRL:
		Square/Circle button: Base object alpha value
		Triangle/Eks button: Bump object alpha value
	  BUMP CTRL:
		Square/Circle button: Emboss shift amount

SAMPLE17
	CLUT bump-mapping sample.

	* Related files
		sample17.c
		data/clutbump0.bin 
		data/clutbump1.bin
			
	<SAMPLE17 Operation>
		Square/Circle button: ambient alpha value
		Triangle/Eks button: diffuse alpha value
		R1/R2 button: specular alpha value
		L1/L2 button: shininess value
		CLUT alpha = ambient alpha + diffuse alpha * (light.normal) + specular alpha * (light.normal)^shininess
	
		(light.normal) is the inner product of a light vector and 
		a normal vector.

SAMPLE18
	Shadow mapping sample.
			
	* Related files
		sample18.c
		micro/vu1shadowTex.vsm
		micro/vu1shadowSTQCull.vsm
		data/shadowmap0.bin 
		data/shadowmap1.bin
		
	<SAMPLE18 Operation>
		Right/Left/Up/Down directional and L1/L2 buttons: Rotates 
						an object
		Square/Circle/Triangle/Eks/R1/R2 button: Moves an object
		Square/Circle button: Shadow alpha value

SAMPLE19
	Light mapping sample.

	* Related files
		sample19.c
		micro/vu1lightmapSTQCull.vsm
		data/wall.bin 
		data/litmap.bin 
		data/spot.raw
		
	<SAMPLE19 Operation>
		Light Mode operation

SAMPLE20
	Reflection/Refraction sample.

	* Related files
		sample20.c
		micro/vu1reflectS.vsm 
		micro/vu1refractS.vsm
		data/reftorus.bin 
		data/refback.raw

	<SAMPLE20 Operation>
	  MICRO CHANGE:
        	Square/Circle button: Reflection/Refraction switching
	  PARAMETER CTRL: (Only for refraction)
		Square/Circle button: Refractive index
		Triangle/Eks button: Scaling factor

SAMPLE21
	A dynamic reflection/refraction sample using a fisheye lens 
	effect.

	* Related files
		sample21.c
		micro/vu1reflectR.vsm
		micro/vu1refractR.vsm
		micro/vu1fisheye.vsm
		data/reflect.bin
		data/reflect_fish.bin
		data/refract.bin
		data/refract_fish.bin
		data/refroom.bin
		data/refobj.bin
		
	<SAMPLE21 Operation>
	  OBJ CTRL:
		Right/Left/Up/Down directional and L1/L2 buttons: Rotates 
						an object
		Square/Circle/Triangle/Eks/R1/R2 button: Moves an object
	  REFRACT CTRL:
		Square/Circle button: Refractive index
		Triangle/Eks button: Scaling factor
		R1/R2 button: Z-shift amount
	  REFLECT CTRL:
		Triangle/Eks button: Scaling factor
		R1/R2 button: Z-shift amount
	  Blending CTRL:
		Square/Circle button: Reflection alpha value
		Triangle/Eks button: Refraction alpha value

SAMPLE22
	A specular light mapping sample.

	* Related files
		sample22.c
		micro/vu1reflectS.vsm
		data/teapot.bin
		
	<SAMPLE22 Operation>
	  PARAMETER CTRL:
		Triangle/Eks button: Scaling factor

SAMPLE23
	A full-screen anti-aliasing sample.

	* Related files
		sample23.c
		data/APEWALK39.bin
		data/dino.bin
		
	<SAMPLE23 Operation>
		Circle button: Sets anti-aliasing on/off
		Triangle/Eks button: Changes drawing buffer depth
		L1/L2 button: Changes blur intensity
		Right/Left/Up/Down directional button: Rotates a camera
		R1/R2 button: Moves a camera

SAMPLE24
	A sample using a fisheye lens effect to perform drawing.

	* Related files
		sample24.c
		micro/vu1cullVo.vsm:  Microprogram for normal perspective 
				      transformation
		micro/vu1fisheye.vsm: Microprogram for perspective 
				      transformation with a fisheye lens 
				      effect
		data/refroom.bin
		
	<SAMPLE24 Operation>
	  MICRO CHANGE:
        	Square/Circle button: Switches fish-eye and normal 
				      perspective transformations 
				   				      
	  PARAMETER CTRL: 
		Square/Circle button: Decreases/Increases image size
		
SAMPLE25
	A sample to display multiple copies of a model with different 
	animations.  
	This sample multiplexes the Shape plugin and necessary plugin 
	blocks/data blocks.

	* Related files
		sample25.c 
		data/cubeman.bin
		data/animation.bin

SAMPLE26
	A Clip Plug sample that performs clipping to objects.
	The sample handles the camera screen as the screen with a 
	resolution of 320 by 224, not as the regular screen with 
	640 by 448. Red lines are displayed with libgp as a screen frame.

	Also, clipping the objects with the unit of primitive of Vu1 
	would affect the clarity and simplicity of the sample, therefore,
	micro code used is basicVo without the Primitive Clip. 
	
	When the objects are removed from the screen frame (red line) 
	completely, they are not displayed or DMA transferred. 
	In this way, you can verify that DMA load decreases.
		
	* Related files
		sample26.c
		data/clip.bin

SAMPLE27
	A sample displaying at a frame rate slower than 1/60 fps. 
	This is a sample to perform drawing at a slower frame rate, which 
	demonstrates the following three modes.
			
	INTERLACE, V448
		Draws an image with progressive resolution and displays the 
		interlaced image. Vertical resolution does not decrease.
			
	NON-INTERLACE, V224
		Displays a non-interlaced image. Vertical resolution drops 
		to half. However, the consumed VRAM can be small. 
			
 	INTERLACE, V224
		Draws/displays the image as at 1/60 fps. 
		The image blurs in vertical direction.
			
	* Related files
		sample27.c
		data/APEWALK39.bin
	
	<SAMPLE27 Operation>
	   	SELECT button: Changes display method

SAMPLE28
	A radiosity sample.

	* Related files
		sample28.c
		data/radtea.bin
		micro/vu1noshade.vsm
		micro/vu1vertexCspec.vsm
		micro/vu1cullSo.vsm

	<SAMPLE28 Operation>
	  MICRO CHANGE:
		Triangle/Eks button: Switches microcodes

SAMPLE29
	An object-cloning sample. 
        It clones tree.bin data.

	* Related files
		sample29.c
		data/tree.bin

SAMPLE30
	A skin deformation sample via VU1.
	It can process four bones and four weights at the maximum.

	* Related files
		sample30.c
		micro/vu1skin.vsm
		data/skin1.bin
		data/under.tm2
		data/over.tm2

	<SAMPLE30 Operation>
		SELECT button: Switches modes
	  	Bone Ctrl Mode:
		    Right/Left/Up/Down directional button: Rotates a bone
                    Square/Circle button: Changes the bone to operate
		Root Ctrl Mode:
		    Square/Circle/Triangle/Eks/R1/R2 button: Rotates and 
							moves an object
SAMPLE31
	A skin deformation sample via VU0.
	It can process multiple bones and four weights.

	* Related files
		sample31.c
		mtxdisp.c
		data/skin0.bin

	<SAMPLE31 Operation>
		SELECT button: Switches modes
		Bone Ctrl Mode:
		    Right/Left/Up/Down directional button: Rotates a bone
                    L1/L2/R1/R2 button: Moves a bone
		    Square/Circle button: Changes the bone to operate

SAMPLE32
	A sample to perform Texture Change Animation using Tim2 texture 
	that has multiple picture data items

	* Related files
		sample32.c
		data/water.bin 
		data/ani_w.tm2

	<SAMPLE32 Operation>
	  Playback Mode:
	        SELECT button: Suspends animation
	        Up/Down directional button: Changes direction
	        Triangle/Eks button: Skips ratio
	  Suspend Mode:
	        SELECT button: Resumes animation
	        Up/Down directional button: Changes direction
	        Triangle/Eks button: Skips ratio
	        Right/Left directional button: Advances frames

< Loading Data >
	In this sample program, data can be loaded from the host.
	Therefore, you should create the necessary data described above 
	in the host prior to loading the data.  The data path is described 
	in the relative path name of the directory that contains the 
	sample execution file, main.elf.
	Place the binary data under the data directory in the directory 
	that includes main.elf.


< Data >
        The following model data used in this sample has been exported by 
	using 'eS Export for LightWave 3D' developed by D-Storm Inc. based 
	on the data distributed by 'LightWave 3D' from NewTek Inc. in the 
	U. S.

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
	
	When USER_PROG_main() returns a value other than 0,
		USER_PROG_exit()
	is called in the following frame to proceed to the exit process.

	A simple procedure of adding a user's own program named MY_PROGRAM 
	is as follows:

		1. Add DEFFUNC(MY_PROGRAM,comment) to sample_func.tbl.

		2. Create a file that contains the following.
			void MY_PROGRAM_init()
			int MY_PROGRAM_main()
			void MY_PROGRAM_exit()

		3. Add the file created in the step 2 above to 
		   the source definition area of the make file.

		4. Execute the make file.


< Notes on Trademark >
	LightWave 3D(TM) is a trademark of NewTek, Inc. in the U. S.


