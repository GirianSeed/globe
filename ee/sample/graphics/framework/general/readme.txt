[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2001 Sony Computer Entertainment Inc.
                                      All Rights Reserved
                                                Mar. 2001

graphics framework general sample

<Description>

The objective of this sample program is to construct an integrated environment that incorporates the basic concepts and various functions of an integrated graphics environment under the category of sample/graphics/framework/. From now on we plan on successive version upgrades as the structure within sample/graphics/framework/ nears completion.

Refer to the document over.txt for an outline of the graphics framework.

<Files>
	*.h             : Prototype declaration headers
	anime.c		: Function group for animation
	camera.c	: Function group for camera 
			  setting process functions
	debug.c		: Debugger function group
	gmain.c		: Function group for data 
			  initialization, reads and 
			  refresh
	hrchy.c		: Hierarchy data process 
			  function group
	info.c		: Information display process 
			  function group
	light.c		: Model light setting process 
			  function group
	main.c		: Main program
	micro.c		: Micro Code/Data setting 
		          process function group
	mode.c		: Framework Application mode 
			  process function group
	model.c		: Model mode process function 
			  group
	object.c	: Framework object data process 
			  function group
	packbuf.c	: Buffer setting process function 
			  group for DMA/VIF packets
	pad.c		: Pad data process function group
	shape.c		: Shape data process function 
			  group for models
	share.c		: Shared polygon process function 
			  group
	tex2d.c		: 2D texture process function 
			  group
	vumem1.c	: VU1 Memory management function 
			  group

	micro/*.vcl	: VCL (VU Command Line) source
	micro/*.vsm	: VSM source

<data directory>
	Makefile	: make file
			  Additional targets can be included 
			  by adding the model name (name 
			  excluding .s files) to TARGETS.
	dino.s		: raw format data
	logo.s		: raw format data
	mbox.s		: raw format data
	APEWALK39.s	: raw format data
	earth.s		: raw format data
	shuttle.s	: raw format data

Model data and scene data included in this directory is data distributed by LightWave 3D 5.6 (NewTek USA) that was exported using Animation Saver v1.8, developed by D. Storm Inc.


<Run method>

	% make		  :Compile
	% make run	  :Execute

	You can also execute the program using 
	the following commands after compiling.

	% dsedb
	> run main.elf


<Controller operation>

	START button   		:	Mode switching 
					
					Model Mode <--
					    V         |
					Camera Mode   |
					    V         |
					Light Mode    |
					    V         |
					Micro Mode  ---

Pressing L1 while holding L2 will turn on/off operating information

Model-Mode:
-----------
Up, Down(direction keys) : Move menu cursor up/down 
			   (model selection)
Left, Right(direction keys)	: Move menu cursor 
				  left/right (change 
				  angle selection)
Triangle, Cross buttons	 : Move model select up/down  
			   (move absolute coordinate 
			   system Y-axis)
Square, Circle buttons	: Move model select left/right  
			  (move absolute coordinate 
			  system X-axis)
R1/R2 buttons		: Move model select forward/back 
			  (move absolute coordinate 
			  system Z-axis)	
SELECT button		: Make model visible / not 
			  visible (and load)

Camera-Mode:
------------
Up, Down (direction keys)	: Rotate camera X-axis
Left, Right (direction keys)	: Rotate camera Y-axis
L1L2 buttons   			: Rotate camera Z-axis
Triangle, Cross buttons  	: Move camera up/down  
				  (move absolute coordinate
				  system Y-axis)
Square, Circle buttons   	: Move camera left/right  
				  (move absolute coordinate
				  system X-axis)
R1R2 buttons   			: Move camera forward/back 
			          (move absolute coordinate 
				  system Z-axis)
SELECT button			: Reset camera

Light-Mode:
-----------
Up, Down (direction keys)	: Move menu cursor up/down
Left, Right (direction keys)	: Move menu cursor 
				  left/right 
L1L2 buttons   	: Change value of selection menu item
Triangle, Cross buttons   	: Move light up/down  
				  (move absolute coordinate 
				  system Y-axis)
Square, Circle buttons   	: Move light left/right  
				  (move absolute coordinate 
				  system X-axis))
R1R2 buttons   			: Move light forward/back  
				  (move absolute coordinate
				  system Z-axis)
SELECT button		: Change light number (Number 0~2)

Micro-Mode:
-----------
Up, Down (direction keys)	: Switch micro load
				  (base => cull => point 
				  light => spot light
				  =>fog => AA1-polygon 
				  AntiAlias => base)

FOG

L1/L2		: Lower/raise value of FOG start position
R1/R2		: Lower/raise value of FOG end position

AA1-polygon AntiAlias:

Triangle, Cross buttons   : Raise/lower AA1 cutoff value


<Process summary>
A summary of the process is given below.

	Initialization flow

	fwInit();
		1. Initialize DMA packet buffer
		2. Initialize GS
		3. Set GS FOG COLOR
		4. Initialize information
		5. Initialize Vu1 Memory management
		6. Initialize TIMER
		7. Initialize Mode
		8. Create GS Init/Reset Packet
		9. Initialize camera
		10. Initialize light
		11. Set light

	fwPadInit();
		1. Initialize packet data manager

Main loop

	fwPadCheck();	Create pad data

	fwMain();	Main loop
		1. Change MODE (accept START BUTTON)
		2. Execute MODE
		3. Calculation stage before DMA transfer 
		   (PreCalc)
		4. DMA transfer request (PostCalc)
		4. DMA transfer (KickDMA)
		5. Display information
		6. Return to 1

fwFinish();	End process

		1. End MODE process 
		2. End Information process


<Process for each microcode>

vu1basicV.vcl : VU1 microcode for parallel light source * 3 (VCL source)
vu1basicVo.vsm : VU1 microcode for parallel light source * 3 

In addition to transparency conversion, light source calculations are performed for normal and parallel light sources using the inner product to calculate vertex colors and draw images. Because clipping is not performed, all triangles are drawn, so improper drawing will occur depending on the location of objects (e.g. behind the camera).
		

vu1cullV.vcl	: VU1 microcode for parallel light source * 3 with Culling (VCL source)
vu1cullVo.vsm	: VU1 microcode for parallel light source * 3 with Culling (optimized)

View Volume Culling and Backface Clipping are performed in addition to vu1base calculations. Surface determination is performed for Backface Clipping using vertex rotation order.


vu1pointV.vcl	: VU1 microcode for point light source * 3 (VCL source)
vu1pointVo.vsm	: VU1 microcode for point light source * 3

In addition to transparency conversion, vertex colors are calculated by light source calculations using the inner product of three point light sources with the normals, and the inverse square of the distances between the light sources, then drawing is performed. View Volume Culling and Backface Clipping are similar to vu1cull. The intensity of the light source is set by P_LIGHT_CONTST in gmain.c.

vu1spotV.vcl	: VU1 microcode for spot light source * 3 (VCL source)
vu1spolVo.vsm	: VU1 microcode for spot light source * 3 

In addition to transparency conversion, vertex colors are calculated by light source calculations using the inner product of three spot light sources with the normals, the reciprocal of the distance between the light sources, and the calculated angle from the direction of the light sources, then drawing is performed. 

View Volume Culling and Backface Clipping are similar to vu1cull.
The intensity of the light source is set by S_LIGHT_CONTST and the expansion of the light source by S_LIGHT_ANGLE in gmain.c.

vu1fogV.vcl	: VU1 microcode for fog (parallel light source * 3 with Culling) (VCL source)
vu1fogVo.vsm	: VU1 microcode for fog (parallel light source * 3 with Culling)

Calculations for fog are also performed in addition to calculations for vu1cull. The fog start point is set by FOG_BEGIN in gmain.c. The saturation point is set by the Z-value of screen in FOG_END. The interval between FOG_BEGIN and FOG_END is linearly interpolated.

vu1antiV.vcl	: VU1 microcode for antialiasing using AA1 polygon (parallel light source * 3 with Culling) (VCL source)
vu1antiVo.vsm	: VU1 microcode for antialiasing using AA1 polygon (parallel light source * 3 with Culling)

In addition to calculations for vu1cull, antialiased images are drawn using the AA1 flag of the GS. In order to suppress substitute actions of the AA1 flag, turn the flag OFF when the polygons are leaning too much towards the camera. The boundary values of this leaning are set by ANTI_PARAM in gmain.c.

<Notes>
For details on the data formats refer to doc/es2raw.txt in the eS package.


<Limitations / precautions>

After startup, a data load will be issued from the host after the initial visible state for each model (MODEL On/Off goes to On in MODEL MODE). Because of this the display will go blank for a moment.

FOG color settings cannot be made dynamically. They are determined statically in _fwMainGSFogColorInit() called from within fwInit().

At present switching cannot be done using the switching flag for resident / non-resident textures created in a previous framework.

This is because the process order is determined in object.c by major structural changes and fwTex2dPostCalc() must be called in PostCalc.

Compatibility is possible for handling objects in object.c, depending on sequence changes.

In future releases, we plan on introducing sequence changes to allow resident / non-resident settings.

The matrix stack size of a hierarchical structure can be up to 20. To change the stack size edit hrchy.c and set the stack size.

The hierarchical structure and the rotation sequence of the animation are in YXZ order to match LightWave 3D.

To change, edit hrchy.c and anime.c and set the rotation sequence. In the future we plan on reflecting the rotation sequence in the data.

Tiling textures are not supported. Also, textures which exceed 0x7fff qwords in size cannot be used.

If all models are set to a visible state, a process may be dropped due to the debugging display (e.g. number of FRAMEs or CPU process time display).

The fwInfoDraw() process executed in info.c is used for debugging during development. Avoid using the same function in a game application.


--------------------------------------------------------
LightWave 3D(TM) is a trademark of NewTek USA.

