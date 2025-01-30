[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
        Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program demonstrating clipping (Backface&Viewvolume) (VU1)

<Description>

This sample program performs backface clipping and view-volume clipping of polygons using the VU1. The program handles both triangles and triangle strips. All operations are performed polygon by polygon. Scissoring, etc. is not performed. Thus, polygons having even one vertex outside the clipping area will be broken. White lines on the screen indicate the clipping area, which can be manipulated using the controller. This area is meant to be set the same as the GS clipping area (+/-2048), with polygons being prevented from going outside this area. Also, polygons that have gone behind the viewpoint are not displayed.

<Files>
        main.c          : main program
	main.h          : prototype declarations of the 
			  main program
	defines.h	: prototype declarations used in 
			  the various .c files
        vu1pack.c       : function to generate VU1 transfer 
			  packets
        cube.dsm        : VU1 transfer packet (data) file 
			  (cube)
        torus.dsm       : VU1 transfer packet (data) file 
			  (donut)
	basic.dsm       : file for loading VU1 microcode
	basic.vsm       : rendering microcode with no 
			  clipping
	clip.dsm        : file for loading VU1 microcode
        clip.vsm        : rendering microcode including 
			  backface and view-volume clipping
        clip_opt.vsm    : optimized version of clip.vsm
        tex128.dsm      : texture data
        tex64.dsm	: texture data

<Execution>
        % make          : compile
        % make run      : run

Alternatively, the following can be used after compiling
        % dsedb
        > run main.elf

<Controller operations>
        up/down arrows (direction keys): rotate around 
					  x-axis (viewpoint)
        left/right buttons  	: rotate around y-axis 
				  (viewpoint)
        L1/L2 buttons   	: move along z-axis 
				  (viewpoint, forward/back)
        triangle/X buttons  	: change clipping area 
				  (y direction)
        square/circle buttons  : change clipping area 
				  (x direction)
        R1/R2 buttons   	: change clipping area 
				  (xy direction)
        SELECT button     	: change model 
				  (cube, torus)
	START button	 	: clipping ON/OFF

<Overview of operations>

The following is an overview of the operations performed.

===== main.c =====
main():

1. Set up double buffer, load textures
2. Open pad, transfer microcode
3. Set up blending coefficient, initialize deformation noise distribution
4. Read information from pad
5. Generate VU1 transfer packets
6. Transfer packets to VU1 and render
7. Return to 4.

SetViewScreenClipMatrix():
Calculate matrix (view_clip) used to project the vertices to the clipping coordinate system. Also determine the perspective transformation matrix (view_screen). The clipping coordinate system is a coordinate system in which the view volume is projected to a cube having corners at (-1.0, -1.0, -1.0) and (1.0, 1.0, 1.0). Thus, the coordinates after projection can be clipped on all axes at absolute value 1.0.

SetGifClipLine():
Function for generating GIF packets used to render the boundary lines of the clipping area. Perspective transformations take place directly at VU0, and GIF packet data is generated at that point.
	

===== vu1packet.c =====
Function group for generating packets to be transferred to VU1.

Vu1DmaPacket structure:
Structure used to generate and manage packets to be transferred to VU1. Packets that provide unified management of various types of object data and use the minimum required DMAKICKs are generated. Also, packet data is designed to operate (be generated) in double buffers.

InitVu1DmaPacket():
Function to initialize the Vu1DmaPacket structure. Pointers to two management tables (packets) are passed as parameters. This is used for double-buffering of packets.

GetVu1ObjHead():
Returns the starting address of the currently active packet.

AddVu1ObjPacket():
Adds object data compiled from a .dsm file and residing in memory (already in VU1 packet format).

CloseVu1ObjPacket():
Closes currently active packet and switches double buffers. Can be used with DMAKICK to return the address of the active packet as the return value.

GetVu1EnvHead():
Returns a pointer to a structure used to preset matrix data to VU1.

CloseVu1EnvPacket():
Closes matrix data (packet) used by VU1 and returns pointer to packet.


==== clip.vsm(VU1 micro code) ====

[clip.vsm]

1. Set up and pre-calculate various matrices in registers VF01 - VF16
2. Set rendering attribute flag (triangle or triangle strip)
3. Set various counters (e.g., strip vertex count)
4. Set up GIF tag load/store pointers and RGBAQ, ST, XYZF store pointers
5. Perform perspective transformations of vertex
6. Calculate inner products of normal vector and light vector
7. Project vertex to clipping coordinates and set flag (VI01) (use CLIP instruction to evaluate last three points based on absolute value range of absolute value 1.0 for X, Y, and Z)
8. Perform backface clipping and set flag (VI12) (Manage perspective transformation coordinates of last three points via VF22-VF23.) Determine normal vector of plane and determine positive/negative. With triangle strip, positive and negative are detected in an alternating manner so invert signs while detecting.)
9. Take sum of two flags and branch. Determine whether XYZ2 or XYZ3 is to be kicked.
10. Determine vertex color based on inner product with normal vector.
11. Loop based on vertex count (return to step 5)
12. Loop based on object block count (return to step 4. Switch double buffers)
13. Loop based on object count (return to step 2).


