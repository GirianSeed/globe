[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

2-shape interpolation using VU1

<Description>

This sample program shows how to perform perspective transformation and light source calculation using VU1, and interpolate between two geometric shapes.

<Files>
        deform.c :  Main program
        metal.dsm :  Texture data
        sphere.dsm :  Object data (DMA packet)
        mime.vsm :  VU1 microprogram

<Startup>
	% make :  Compile
	% make run :  Execute

The following method can also be used to execute the program after compilation.
	% dsedb
	> run deform.elf

<Controller operation>
        <circle> button:  Stop rotation while continuing 
			  deformation

<Remarks>

Processing steps are briefly described below.

1. Load texture.
2. Open controller.
3. Set matrix, interpolation weight, and light source matrix in packet to be passed to VU1.
4. Perform DMA kick to VU1.
5. Return to 2.

The following processing is performed within the VU1 microcode.
1. Set rotation x perspective transformation matrix.
2. Load various types of parameters (such as matrices) in VU1 registers.
3. Load normal vector, two coordinates to interpolate, ST value, and color.
4. Perform coordinate interpolation, perspective transformation, and light source calculation and store results in VU1Mem.
5. Return to 3. and repeat for number of vertices.
6. XGKICK stored data (Gif Packet) to GS.



