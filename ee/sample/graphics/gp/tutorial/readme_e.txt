[SCE CONFIDENTIAL DOCUMENT]
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                  All Rights Reserved

Basic Sample of Using the Basic Graphics Library libgp

< Description of the Sample Program >

	This is an implementation sample, which introduces the most basic 
	use of the Basic Graphics Library libgp.

< Files >
	main.c		Main program source
			Switches and clears the screen, and allows for 
			input with the controller
	oneprim.c	Draws one sprite packet only
	prims.c		Use a chain to draw three packets
	defines.h	Common header file
	Makefile	Make file

< Activating the Program >

	 % make run

< Using the Controller >
	SELECT button			Switches the drawing routines.
	Directional buttons		Swithch the items to control. 
					(available only when prims.c is 
					executed) 
	Circle/Triangle/Eks/ 
	Square button			Control packet positions.
	
< Description of the Sample Program >
	oneprim.c	
		Draws one sprite packet only.
		This sample performs DMA transfer and draws one packet 
		independently, and does not use chain.
    	    
		oneprim_init()  	Generates sprite packet and 
					configures the contents.

		oneprim_draw()  	Updates the packet contents 
					and practically draws. 
					(transfers the packet
					independently)

		oneprim_exit()  	Discards the memory area used 
					for the packet.

	prims.c		
		Transferring multiple packets successively requires 
		the use of a chain as in this sample. 
		Put three packets with different Z values on one chain 
		then transfer.
    	        
		prims_init()  		Generates three packets and 
					configures the contents. 
					Generates and initializes a 
					chain, and registers the packets 
					in the chain.

    	 	prims_draw()  		Updates the packet contents 
					and practically draws 
					(transfer the chain)

    	    	prims_exit()  		Discards the memory area used 
					for the packets and chain

       	main.c
    	    	pad_init() 		Perform initialization for input
					with the controller.

  	    	pad_read() 		Reads the input with the controller

    	    	db_init() 		Initializes GS double buffering 
					(for drawing/displaying)

   	    	db_swap() 		Switches GS double buffering 
					(for drawing/displaying) 

   		Actual drawings with the use of the gp are called from 
		mode_list[mode].draw_func(paddata, padtrig); in while loop. 

	defines.h		
		Declares constants and inline functions used commonly 
		in the files. 
		Converts vertex coordinates; convert_x() and convert_y() 
		into the values on GS primitive coordinate.
		
		Refer to the comments and library manual (the overview 
		and reference) in the program for more details.
    	
		For textured drawing and the method of perspective 
		conversion, see other sample programs such as zsort.
    	

< Notes >
	The operation may not be optimal as readability is more enhanced
	in this program.
	For instance, a generic index function is used with an argument, 
	type to obtain the index function, however, specific function is
	to be used for respective types in terms of operation efficiency.

