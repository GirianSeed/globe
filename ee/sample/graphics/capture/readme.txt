[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
        Copyright (C) 1999 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Sample program showing how to save the contents of the frame buffer

<Description of the sample program>

This program saves a rendered frame buffer in the frame buffer of the host machine. The rendered image itself is provided in sample/vu1/deform.

<Files>
        capture.c       : main program
        metal.dsm       : texture data
        sphere.dsm      : object data (DMA packet)
        mime.vsm        : VU1 microprogram

<Execution>
        % make          : compile
        % make run      : execute

The following can be used instead, after compilation:
        % dsedb
        > run capture.elf

<Controller operations>
        circle button   : stop rotation while continuing 
			  deformation
        X button        : capture and terminate program

<Summary of operations>

The following is a summary of the operations performed for capturing. (Refer to the deform sample program for coverage of the rendering operation)

1. The X button is pressed.
2. Render next frame in the other frame buffer without performing matrix updates for animation
3. Call local StoreImage() function
4. Open host file
5. Allocate memory for storing odd/even frame buffer contents
6. Local-->Host transfer of contents in even field (sceGsExecStoreImage())
7. Local-->Host transfer of contents in odd field
8. Write transferred image with odd/even interlaced onto file on the host
9. Release allocated memory
10. Release file descriptor
11. Terminate program

<Notes>

The output file is in 32bit-rgba raw data format.
