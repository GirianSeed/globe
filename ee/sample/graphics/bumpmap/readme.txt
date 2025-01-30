[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
        Copyright (C) 1999 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Bump mapping sample program

<Description>

This program shows how to perform pseudo-bump mapping

<Files>

        grid.dsm        : grid model packet
        m.vsm           : microcode for adding given 
			  offset to st and performing 
			  calculations
        m_opt.vsm       : optimized version of m.vsm
        main.c          : main
        mat.dsm         : pattern texture data for mat
        packet.dsm      : packet for perspective transform 
			  matrices and for setting up GS
        textbump.dsm    : bump texture for the string 
			  "BUMP MAP"
        wavebump.dsm    : bump texture for wave pattern

<Execution>

        % make          : compile bump map sample
        % make run      : run bump map sample

The program can also be executed in the following manner after compilation

        % dsedb
        > run main.elf

<Controller functions>
        direction keys   	: move viewpoint
        circle/square buttons  : change direction of light
        START button     	: turn bumps ON/OFF
        SELECT button    	: change bump texture

<Notes>

Bump mapping operations in this sample program are performed in the following manner.

1. The image from the original model is rendered with textures.
2. Using the rendered frame buffer, the same model is rendered with bump textures, and this is subtracted from the frame buffer colors. When doing this, the texture coordinate st of the model is offset slightly from the original position toward the direction of the light source.
3. The model is rendered with the bump texture without offsetting st. This time color is added to the frame buffer.

With steps 2 and 3, light and shade appears on the original image based on the st value offset. This provides bump-like effects.

<Summary of operations>

The following is a summary of the operations that are performed by the program.

[main.c]
1. Reset graphics parameters.
2. Set up double buffers.
3. Set up texture parameters.
4. Load microcode, textures.
5. Set up various parameters based on pad input.
6. Kick rendering DMA.

The following operations are performed by VU1 microcode.

[m.vsm]
1. Set up rotation x perspective transform matrix.
2. Load various parameters (matrices, etc.) in VU1 registers.
3. Load normal vectors, vertex coordinates, ST values, colors.
4. Calculate perspective transform, light-source, Q values, etc. and store results in VU1Mem. Add offset to st value.
5. Return to step 3. Repeat based on number of vertices.
6. XGKICK stored data (GIF packet) to the GS.
