[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Mipmap sample program

<Description of sample program>

Sample program demonstrating rendering using mipmaps

<Files>
        256.dsm
        128.dsm
        64.dsm
        32.dsm
        16.dsm
        8.dsm
        4.dsm           : textures for various mip levels
        main.c          : main function
        mathfunc.c      : functions for performing various 
			  math operations
        rect.dsm        : packet for rendering a slab
        basic.vsm       : rendering microcode
        mathfunc.h      : definitions of various parameters

<Execution>

        % make          : compile mipmap sample
        % make run      : run mipmap sample

The sample program can also be run by entering the following after compilation.

        % dsedb
        > run main.elf

<Controller operations>

       circle button     : stop forward/back motion of slab

<Notes>
The following is a summary of operations performed.

[main.c]

1. Set up double buffering. Load textures for various mip levels.
2. Set up texture registers (TEX0_1, TEX2_1, MIPTBP1_1, MIPTBP2_1, etc.)
3. Get input value from pad and use value to set parameters.
4. Set up MIPMAP (TEX1_1)
5. Set up matrices
6. Render mipmapped slab (right)
7. Set MIPMAP off (TEX1_1)
8. Set up matrices
9. Render non-mipmapped slab (left)
10. Return to step 3

The following operations are performed in the VU1 microcode.

[basic.vsm]

1. Set up rotation x-perspective transformation matrix
2. Load various parameters (matrices, etc.) into the VU1 registers
3. Load normal vector, vertex coordinates, ST value, color
4. Perform perspective transformation, light-source calculation, Q value, etc. and store results in VU1Mem
5. Return to step 3 and loop based to vertex count
6. XGKICK stored data (Gif Packet) to the GS
