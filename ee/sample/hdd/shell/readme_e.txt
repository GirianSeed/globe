[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

The sample program of operating partitions in HDD

< Description of the Sample >
	This is a sample program of command line tools similar to shell.
	Input is made in another window using dsecons.
	After boot, auto-mounting is attempted when partitions are
	detected. However, mounting fails if passwords are set on
	partitions or partitions are created but not formatted.
	Please ignore errors issued in these cases.
	
	
< Files >
	main.c

< Activating the Program >
        $ make          : Compiles the program
        $ make run      : Executes the program

	After compilation, it is executable with the method below
	% ds[ei]db
	> run main.elf

	Input is made in another window using dsecons.

< Using the Controller >
	None

<Note>
	None
