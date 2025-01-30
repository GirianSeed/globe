[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

The sample program of operating files in HDD

< Description of the Sample >

	
	This is a sample program of accessing files in HDD.

	1. Create file copy.c by copying contents of file main.c.
	2. Create incremental data in increment of 4 bytes and write it
	   into a file incdata. Read the file incdata again to check the
	   validity.

< Files >
	main.c

< Activating the Program >
        $ make          : Compiles the program
        $ make run      : Executes the program

	After compilation, it is executable with the method below
	% dsedb
	> run main.elf

	With the display of "sample end.", perform normal termination.

< Using the Controller >
	None

< Note >
	None

