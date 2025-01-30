[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Point light source sample program

<Description>

This program performs point light source processing of two light sources.

Point light sources generally have intensities that are inversely proportional to the square of the distance. To illustrate this effect, the program also provides a pattern that is inversely proportional to the distance.


<Files>

        main.c          : main program
        main.h          : prototype declarations for main 
			  program
        defines.h       : prototype declarations for the 
			  .c files
	vu1pack.c       : functions for generating VU1 
			  data packets
	tex256.dsm      : texture data file
        cube.dsm        : object data (cube)
	torus.dsm       : object data (torus, 1024 
			  polygons)
	grid200.dsm     : object data (plane, 200 polygons)
	grid1800.dsm    : object data (uneven plane, 1800 
			  polygons)
	plight.dsm      : packets for transferring VU1 
			  microcode
	pl.vsm          : VU1 microcode (inverse 
			  proportion to distance)
	pl_opt.vsm      : VU1 microcode (optimized version)
	pl2.vsm         : VU1 microcode (inverse 
			  proportion to square of distance)
	pl_opt2.vsm     : VU1 microcode (optimized square 
			  version)
	basic.vsm       : VU1 microcode (for cube 
			  rendering)

<Execution>

        % make          : compile
        % make run      : run

The following method can also be used after compiling.
        % dsedb
        > run main.elf

<Controller operations>

        up/down arrows (direction keys)	: move red light source 
				  (forward/back)
        left/right arrow buttons   	: move red light source 
				  (left/right)
        L1/L2 buttons    	: move red light source 
				  (up/down)
        triangle/X buttons	: move blue light source 
      			          (forward/back)
        square/circle buttons	: move blue light source 
				  (left/right)
        R1/R2 buttons 		: move blue light source 
				  (up/down)
        SELECT button  		: change model (plane, 
				  unevent plane)
	START button		: switch light intensity 
				  attenuation function 
				 (distance or square of 
				  distance)


<Notes>

Summary of calculation methods

To calculate the color of a vertex, a vector is drawn to the light source and an inner product of this and the normal vector is calculated. The intensity of the diffuse reflection light is determined from this inner product. The distance from the light source also influences the intensity of the light. A summary of the calculations are presented below.

	Vertex_point(VP) : polygon vertex coordinates
	Vertex_color(VC) : polygon vertex colors (before 
			   color calculations)
	Vertex_normal(VN): polygon vertex normal vectors
	Light_point(LP)  : light-source coordinates
	Light_color(LC)  : light-source color
	coef             : coefficient
	[dot]            : inner product
	*                : product

Attenuation is inversely proportional to distance
	  (RGB) = ( ((VP-LP)/|VP-LP| [dot] VN * coef/|VP-LP|) * LC ) * VC
	      = coef/|VP-LP|^2 * ((VP-LP) [dot] VN) * LC * VC

Attenuation is inversely proportional to square of distance
	  (RGB) = ( ((VP-LP)/|VP-LP| [dot] VN * coef/|VP-LP|^2) * LC ) * VC
	      = coef/|VP-LP|^3 * ((VP-LP) [dot] VN) * LC * VC


Note that when actually performing the calculations, normalization, etc. will need to be performed.


Warning

1. Objects will generally be defined in the local coordinate system, but point light-source coordinates are usually defined in world coordinates. The data transferred to VU1 will be "object -> local coordinates" so determining a vector between the point light source and the object will not work.

There are a number of ways to avoid this problem.

One way is to split up the Local -> Screen transformation matrix, used in perspective transformations, into two steps: Local -> World and World -> Screen. The vector can be calculated between the two steps. However, extra VU1 microcode calculations will be required, so this method is not highly recommended.

A second way is to do a World -> Local inversion of the point light source beforehand and to provide the resulting point light source coordinates to VU1. This way, there is no calculation overhead on VU1. The sample program uses this method.

2. Color calculations for the point light source are performed for each polygon vertex. Since the intensity of light will generally be proportional to the inverse of the distance (squared), color calculations will not be accurate at the pixel level. This is especially true when the polygons are rough.

Thus, polygon resolution must be taken into account in order to maintain quality in point light source calculations.
