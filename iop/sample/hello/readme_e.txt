[SCEI CONFIDENTIAL DOCUMENT]
DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
                  Copyright (C) 1999 by Sony Computer Entertainment Inc.
                                                     All Rights Reserved

The Sample Program for the hello print 

< Description of the Sample >

 Subsequent to displaying "hello !\n", the arguments which were passed to
 the program are appeared one by one.

< File >

	thread.c

< Activating the Program >

	% make							:Compiling 
	% dsreset 0 2 ; dsistart hello 	 arbitrary arguments    :Executing 

	Also executable with the following. 
	% dsidb
	> reset 0 2 ; mstart hello 	  arbitrary arguments
