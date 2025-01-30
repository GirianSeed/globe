[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
        Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Reflection Mapping - Specular Sample

<Description>

This sample program shows how to calculate specular illumination and perform reflection mapping.

<Files>

        buff0.dsm:  Packet for setting texture transfers
        buff1.dsm:  Packet for setting TEX0_1 register
        r.vsm:  Microcode (not optimized) for specular 
		reflection mapping
        r_opt.vsm:  Microcode (optimized) for specular 
		    reflection mapping
        main.c:  Main function
        mathfunc.c:  Functions that perform various kinds 
		     of numerical calculations
        mathfunc.h:  Defines various types of parameters
        mug.dsm:  Mug model data (texture off)
        mug_tex.dsm:  Mug model data (texture on)
        packet.dsm:  Packet data flowing to path1/path2
        matrix.c:  Functions that set various types of 
		   matrices
        reftex.dsm:  Textures for reflection mapping

<Execution>

        % make:  Compiles sample
        % make run:  Executes sample

This program can also be executed by the following method after compilation.
        % dsedb
        > run main.elf

<Controller operation>
	up-arrow/down-arrow/left-arrow/right-arrow (direction keys):  Rotate camera up, down, left, and right
	triangle/X/square/circle buttons:  
		Rotate light up, down, left, and right

<Notes>
	None

