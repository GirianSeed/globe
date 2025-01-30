[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
        Copyright (C) 1999 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Sample program showing B-spline rendering using VU1

<Description>

This sample program uses the VU1 to render a B-spline surface based on given control points.

<Files>
        main.c          : main program
        bs.c            : function for generating packets
        noise.h         : prototype declarations for noise function
        noise.c         : noise function
        b_data.dsm      : packets (matrices, blending factors, packets, etc.)
        b.vsm           : B-spline rendering microcode
        grid.h          : control point data for grid
        torus.h         : control point data for torus
        flower.dsm      : texture data
        b_opt.vsm       : optimized version of b.vsm

<Execution>
        % make          : compile
        % make run      : run

The sample program can also be run by entering the following after compilation.
        % dsedb
        > run main.elf

<Controller operations>
        arrow keys       : move viewpoint
        SELECT button    : switch models (grid, torus)
        R1 button        : switch between textured and wire-frame rendering

<Notes>

For information on packet structure and microcode, please refer to default.htm.
	
The following is a summary of operations.

===== main.c =====

main():
1. Set up double buffering. Load textures.
2. Open pad. Transfer microcode.
3. Set up blending coefficient. Initialize noise distribution for deformation.
4. Read information from pad. Set up rendering packets, matrices.
5. Enter control point coordinates in packet
6. Send packet to VU1 and render
7. Set up control point for next frame
8. Return to step 4.

SetBlendingCoeff():
1. Calculate blending function when t is divided by 4.
   Calculate derivative of the blending function.
2. In the microcode, 36 triangles resulting from 4x4 control points are divided into three triangle strips. Combinations of values calculated from (1) corresponding to the vertices of these triangle strips are stored in the packet.
        
CreateViewingMatrix():
1. Set up matrix for performing screen transformation

SetViewPosition():
1. Set up viewpoint

MoveLight():
1. Set up light-source direction

===== bs.c =====
CreateControlPointArea():
1. Of the U, V control points in the data for rendering the packet template in b_data.dsm, copy an appropriate number to the scratch pad depending on whether U, V are closed.
        
SetControlPoint():
1. In the packet created in the scratch pad by CreateControlPointArea(), set up the control point coordinates to be actually rendered.

===== noise.c ====
1. Initialize noise distribution, interpolation function

===== b.vsm(VU1 micro code) ====

[b.vsm]
1.  Set up perspective transformation matrix in registers VF28 - VF31
2.  Set various counters (e.g., strip vertex count)
3.  Set VF01 - VF16 to 4x4 control point to be calculated
4.  Set up GIF tag load/store and RGBAQ, ST, XYZF store pointers
5.  Load blending coefficient for calculation
6.  Calculate tangent vector for calculated point and determine normal vector from outer product
7.  Determine inner product of normal vector and light vector from parallel light source to calculate illumination
8.  Load blending coefficient and determine physical coordinates after triangle division
9.  Load ST value stored beforehand, add offset, and determine actual ST value
10. Using FTOI, etc., store RGBAQ, ST, XYZF to the position in VUmem indicated by store pointer
11. Loop according to vertex count (return to step 5)
12. Loop according to packet count (return to step 2)
13. To create strip seams, invert the sign of Z of the seam packet
14. XGKICK to GIF

