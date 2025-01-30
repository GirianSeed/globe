[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
         Copyright (C) 2002 Sony Computer Entertainment Inc.
                                         All Rights Reserved

Sample Programs Showing the Most Elementary Ways of Using the Basic Graphics Library libgp

<Description>

	These sample programs show the most elementary ways of using the basic graphic library libgp.

<Files>

	main.c		Main program source code
			Switches and clears screens, performs controller input
	oneprim.c	Draws only one sprite packet
	prims.c		Uses chains to draw three packets
	defines.h	Common header file
	Makefile	Make file

<Execution method>

        % make run

<Controller operations>

	SELECT button				Switches drawing routine
	Up/down/left/right (direction keys)	Switches manipulation target (only for prims.c)
	Circle/Triangle/Cross/Square buttons	Changes packet position
	
<Description>

       oneprim.c	
	    This sample program draws a single sprite packet by DMA transferring one packet. It does not use chains.

    	    oneprim_init()	Creates a sprite packet and sets up its contents.
    	    oneprim_draw()	Updates packet contents and performs actual drawing (single transfer of packet).
    	    oneprim_exit()	Frees memory area used by packet.


       prims.c	
    	    This sample program shows how to use a chain to consecutively transfer multiple packets.
    	    The program transfers three packets with different Z values by connecting them together in a single chain.
    
    	    prims_init()	Creates three packets and sets up their contents.
    			  	Creates and initializes a chain and adds the packets to the chain.

    	    prims_draw()	Updates packet contents and performs actual drawing (chain transfer).

    	    prims_exit()  	Frees memory area used by packets and chain.


       main.c
    	    pad_init()	Performs initialization processing for controller input.
    	    pad_read()	Reads controller input.

    	    db_init()	Initializes GS double buffering (for drawing or display).
    	    db_swap()	Switches GS double buffering (for drawing or display).

    	    The actual drawing by the gp is called from
    		  mode_list[mode].draw_func(paddata, padtrig);
    	    in a while loop.

       defines.h
    	    Declares constants and inline functions to be shared by each file.
    	    convert_x(), convert_y()	Converts vertex coordinate values to GS primitive coordinates.

    	For further details, see the comments in the programs and the library manuals (overview and reference).
    
    	For information about textured drawing or perspective transformations, refer to other samples (such as zsort).

<Notes>

	These samples have not been optimzed for efficiency because they were designed for readability. For example, the function that obtains the index is a general-purpose index function that takes the type as an argument. However, a more efficient approach would be to use special-purpose functions that are type-specific.

