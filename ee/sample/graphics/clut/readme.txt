[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
        Copyright (C) 1999 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Sample program showing how to display 4bit/8bit CLUT textures

<Description>

This sample program displays 4bit/8bit CLUT textures

<Files>
        clut.c          : main program
        tex8.dsm        : 8-bit texture data
        tex4.dsm        : 4-bit texture data

<Execution>
        % make          : compile
        % make run      : run

The program can also be run by entering the following commands after compilation:
        % dsedb
        > run clut.elf

<Controller operations>
	None

<Summary of operations>

The following is a summary of operations.

1. Initialize rendering settings
2. Load 4, 8-bit indexed images to the GS frame buffer
3. Generate packets
4. Set up index array for 8-bit clut settings
5. Update 4, 8-bit cluts based on frame number for clut animation and send to GS.
6. Swap double buffers
7. Render left sprite (8-bit texture)
8. Render right sprite (4-bit texture)
9. Return to step 5

