[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

This program shows how to use the csa/cld fields to control the loading of a 4-bit texture CLUT into a temporary buffer.

<Description>

This program uses the csa/cld fields to control the loading of a 4-bit texture CLUT into a temporary buffer. 4-bit CLUTs are loaded 16 at a time into the temporary buffer, and three sets of these are prepared beforehand.

<Files>

        clut_csa.c    : main program
        tex4.dsm      : 4-bit texture data, CLUT data (48)

<Execution>
        % make          : compile
        % make run      : run

The program can also be run by executing the following commands after compiling:

        % dsedb
        > run clut_csa.elf

<Controller operations>
	None

<Summary of operations>

The following is a summary of operations performed.

1. Initialize rendering environment, DMA
2. Load 4 index image into GS frame buffer (My_indeximg4)
3. Load CLUT into frame buffer (My_clut4_1, 4_2, 4_3)
4. Initialize drawing packet
5. Swap double buffers
6. Set up packet for each frame
6-1. Set up 16 4-bit CLUTs to be loaded into a temporary buffer as a single 8-bit CLUT. Don't load if the 4-bit CLUTs are the same as the previous frame (same CBP).
6-2. Without loading CLUTs from the CLUT buffer, use the value of the offset in the csa field (0-15) to specify a 4-bit CLUT that will be used to draw the current frame.
7. Render sprite
8. Go back to step 5.
