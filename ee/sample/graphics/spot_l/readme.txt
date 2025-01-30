[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Spotlight sample program

<Description>

This program shows how to represent a spotlight.

A spotlight, of course, is like a directed point light source. Spotlights are generally designed so that there is not much diffusion of light, and the light gives the appearance of having low attenuation. The attenuation function for the light can be adjusted according to the way it is being used.

In this sample, attenuation is in inverse proportion to the distance (power of 1). There is support for three spotlights.

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
	spot.dsm        : VU1 microcode transfer packet
	sp3.vsm         : VU1 microcode (inversely 
			  proportional to distance)
	sp3_opt.vsm     : VU1 microcode (optimized version)
	basic.vsm       : VU1 microcode (for cube rendering)

<Execution>

        % make          : compile
        % make run      : run

The following method can also be used after compiling.
        % dsedb
        > run main.elf

<Controller operations>

        up-arrow/down-arrow (direction keys)   	: change direction of red 
			  spotlight beam 
			  (forward/back)        
	left-arrow/right-arrow    	: change direction of red 
			  spotlight beam 
			  (left/right)
        L1/L2 buttons   	: change direction of blue and 
			  green spotlight beams 
			  (up/down)
        triangle/X buttons	: move light source for red 
			  spotlight 
                          (forward/back)
        square/circle buttons	: move light source for red 
			  spotlight 
                          (left/right)
        R1/R2 buttons    	: move light source for red 
			  spotlight 
                          (up/down)
        SELECT button  	: change model (plane, uneven 
			  plane)

<Notes>

Summary of calculation methods

Calculating a spotlight requires the light-source coordinates and a light beam vector (direction of light). An inner product (cos(p)) of the beam vector and a vector extending from the light source to a polygon vertex is used to calculate the directional intensity of the beam. Specifically, the following functions are used.

	Vertex_point(VP) : polygon vertex coordinates
	Vertex_color(VC) : polygon vertex colors (before 
			   color calculations)
	Vertex_normal(VN): polygon vertex normal vectors
	Light_point(LP)  : light-source coordinates
	Light_color(LC)  : light-source color
	Light_vector(LV) : beam vector
	Intensity(I)     : directional intensity of beam 
			   (how it spreads)
	a, b, c          : coefficients
	[dot]            : inner product
	*                : product

	Intensity(I) = (cos(p)^2 - a) * b ;
	     : cos(p) = (VP-LP) [dot] LV / |VP-LP|  : |LV| = 1
	     where a, b are constants that fulfill (1 - a) * b = 1

As can be seen by graphing this function, the function will show high values only at the center, i.e., in the direction of the beam. For (a,b), a range of between (0.8, 5) and (0.99, 100) would be good.

The most important reason for using this function is that the amount of calculations performed is minimized. Since the square of cos(p) is used, there is no need to take a square root for normalization. For one vertex, the optimized microcode performs calculations in 43 instructions, allowing spotlights to be processed at around twice the number of instructions as the microcode for standard flat light.

	Vertex color formula
	  (RGB) = I * (VP-LP)/|VP-LP| [dot] VN * c/|VP-LP| * LC * VC
	      = I * c/|VP-LP|^2 * ((VP-LP) [dot] VN) * LC * VC

Since 1/|VP-LP|^2 is used in I, the final formula is able to efficiently use the results for 1/|VP-LP|^2 twice.

Note that when actually performing the calculations, normalization, etc. will need to be performed.


Warning

1. Objects will generally be defined in the local coordinate system, but point light-source coordinates are usually defined as world coordinates. The data transferred to VU1 will be "object -> local coordinates" so determining a vector between the point light source and the object will not work.

There are a number of ways to avoid this problem.

One way is to split up the Local -> Screen transformation matrix, used in perspective transformations, into two steps: Local -> World and World -> Screen. The vector can be calculated between the two steps. However, extra VU1 microcode calculations will be required, so this method is not highly recommended.

A second way is to do a World -> Local inversion of the point light source beforehand and to provide the resulting point light source coordinates to VU1. This way, there is no calculation overhead on VU1. The sample program uses this method.

2. Color calculations for the point light source are performed for each polygon vertex. Since the spotlight is directed, there will be a light/dark boundary between the space inside and outside the light. However, this cannot be neatly represented when there is linear interpolation of colors between polygon vertices (Gouraud shading). This is especially true when the polygons are rough.

Thus, polygon resolution must be taken into account in order to maintain quality in spotlight calculations.
