[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved


Sample program illustrating anti-aliasing (sorting and polygon clipping)

<Description>

This program shows how to safely use the anti-aliasing features of the GS.

Anti-aliasing smooths edges of primitives through alpha blending. Pixels located on a primitive boundary to which alpha blending is applied will not have their Z-values re-written. (A z-test is performed.) Thus, to provide ideal anti-aliasing, the rendering primitives must be sorted by depth.

However, in practical terms, performing a 100% sort of individual polygons is difficult. Therefore, in this sample program, drawing objects are divided appropriately and these blocks are sorted and drawn.

With anti-aliasing, z-testing is performed up to one pixel beyond the primitive boundary and the primitive is drawn (the z-values are not re-written). Thus, when using interpolation to determine the z-values of pixels beyond the boundary, the calculation can produce an overflow (value exceeds the Zmax value). Consequently, these pixels will be drawn so that they are projected in front of the polygon, producing an unnatural flicker. More specifically, this phenomenon occurs when the polygon is at an acute angle to the screen (crushed flat).

To prevent this flicker, this sample program uses Back Face Clipping to clip polygons with very small areas. This prevents polygons from being drawn that are at an acute angle.

<Files>
        main.c          : main program
	mathfunc.h	: header file for matrix and math 
			  functions
	mathfunc.c	: matrix and math functions
	packet.dsm	: packet file for data used by VU1
	aa_clip.vsm	: VU1 microcode
	cup0.dsm	: object data (cup divided 0-6)
	cup1.dsm	:
	cup2.dsm	:
	cup3.dsm	:
	cup4.dsm	:
	cup5.dsm	:
	cup6.dsm	:

<Execution>
        % make          : compile
        % make run      : run

The sample code can also be run as follows after compilation.
        % dsedb
        > run main.elf

<Controller operations>
        up arrow/down arrow:     rotate around x-axis 
				 (viewpoint)
        left arrow/right arrow:  rotate around y-axis 
				 (viewpoint)
        L1L2    : move along z-axis (viewpoint, 
		  forward/back)
        R1      : turn off anti-aliasing
	R2	: turn off clipping for crushed polygons

<Notes>

Warning:

When clipping is performed by calculating the polygon area with the GS coordinate system, care must be taken when calculating the threshold value.

For example, if calculations are performed with a fixed threshold value, a polygon may disappear simply because the viewpoint and the object become far apart. To prevent this, the threshold value must be controlled so that clipping takes into account the distance between polygons and the viewpoint.

The size of objects will be inversely proportional to the distance. In other words, area diminishes according to an inverse square law. The polygon to be clipped will be almost flat, i.e., almost in the form of a line. Thus, the outer product values can be assumed to diminish in a relatively linear fashion (i.e., inversely proportional to the distance).

In this program, clipping area is calculated using the inverse of the distance from the viewpoint to a representative point on the object.

The following is a summary of operations performed by the program.

===== main.c =====
main():

1. Set up double buffer
2. Set up alpha-blending register for GS (ALPHA1)
3. Open pad, read information from pad
4. Set up matrix
5. Perform Z-sorting on object blocks
6. Create VU1 transfer packets according to sort results (change DMref tags)
7. Calculate clipping areas, insert into packets
8. Transfer packets to VU1 and draw
9. Go back to 3.

VU1 microcode operations

[aa_clip.vsm]

1. Set up various matrices in registers VF01 - VF16
2. Set various counters (e.g., strip vertex count)
3. Set up GIF tag load/store, RGBAQ, ST, store pointer for XYZF
4. Perform perspective transformation of vertex
5. Calculate luminance values from inner product of normal vector and light vector
6. Determine vertex color from luminance value and light color
7. Calculate outer product from 3 vertices after perspective transformation, and calculate polygon area
8. If at or less than threshold value, set the XYZ3 flag and re-clip
9. Loop through 1-8 for each vertex, and XGKICK
