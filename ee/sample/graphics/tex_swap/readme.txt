[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample program showing texture swapping

<Description>
This program swaps textures and draws at the same time

<Files>

        buff0.dsm       : packet to set triple buffering 
			  transfer status to 0
        buff1.dsm       : packet to set triple buffering 
			  transfer status to 1
        buff2.dsm       : packet to set triple buffering 
			  transfer status to 2
        grid.dsm        : terrain model
        m.vsm           : without drawing microcode 
			  optimization
        m_opt.vsm       : with drawing microcode 
			  optimization
        main.c          : main function
        mat.dsm         : terrain texture
        mathfunc.c      : various math functions
        mathfunc.h      : definitions of various parameters
        matrix.dsm      : template for matrix packets
        no[1-4].dsm     : textures 1 through 4
        packet.dsm      : packet data to be sent through 
			  path1/path2
        phys.c          : function to calculate ball 
			  position and set the values in 
			  packets
        sphere.dsm      : model for ball
        tex_swap.h      : parameters and prototype 
			  declarations

<Execution>

        % make          : compile texture swapping sample
        % make run      : run texture swapping sample

The program can also be run using the following method after compiling.

        % dsedb
        > run main.elf

<Controller operations>
        Å™Å´Å©Å® (direction keys)       : viewpoint rotation
	START button :	reset ball

<Notes>

The following is a summary of the operations performed.

        [main.c]

1. Set up double buffer, initialize parameters
2. Set up registers relating to textures
3. Swap double buffers
4. Calculate ball coordinates and enter values in matrix
5. Get pad input value and set viewpoint based on value
6. Activate DMA on ch.1 and perform drawing
7. Go back to step 3


The VU1 microprocessor will perform the operations shown below.

        [m.vsm]

1. Set up rotation x perspective transformation matrix
2. Load various parameters such as matrices into VU1 registers
3. Load normal vector, vertex coordinates, ST value, color
4. Perform perspective transformation, calculate light source, Q value, etc. and store in VU1Mem
5. Go back to step 3 and repeat based on number of vertices
6. XGKICK the stored data (Gif Packets) to the GS

        [packet.dsm]

The following is a description of the packet flow:

1. Load microcode
2. Set up environment for drawing terrain, set up texture buffer
3. Transfer terrain texture
4. Draw terrain
5. Transfer matrix for drawing ball 1
6. Set up buffer for transferring ball 1 texture 
7. Transfer ball 1 texture
8. Draw ball 1
9. Repeat steps 5 through 8 based on the number of balls. Four types of ball textures are transferred in sequence (256x256, 32-bit RGBA)
