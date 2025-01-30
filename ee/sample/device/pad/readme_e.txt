[SCEI CONFIDENTIAL DOCUMENT]
DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
                  Copyright (C) 1999 by Sony Computer Entertainment Inc.
                                                     All Rights Reserved

Sample to Read Controller

<Description of the Sample>
	This is a sample program which reads the controller and 
	displays the bit pattern of it. 

<File>
	main.c

<Activating the Program>
	% make		: Compiles the program.
	% make run	: Executes the program.

	After the compilation, the program is also executable 
	with the following. 
	% dsedb
	> run main.elf

	The bit pattern of the controller is displayed.
	Whether or not the bit pattern changes is confirmed when 
	the number following "cnt" increases and the controller buttons 
	are pressed. 

<Using the Controller>
	Not defined.

<Note>
	This becomes a sample for two controllers by removing the comment 
	(/* marks) from "/* #define PADS */" and enabling "#define PADS". 

