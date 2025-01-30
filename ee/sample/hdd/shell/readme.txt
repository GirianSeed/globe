[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2001 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample program for manipulating HDD partitions

<Description>
This sample program is a command line tool that is similar to the shell. Input is performed using dsecons running in a separate window.

After this program is started up, mounting is automatically attempted if a partition is found. However, mounting will fail if a password has been set for the partition, or if the partition has been created but has not been formatted. Errors that occur at this time should be ignored. 

<File>
	main.c

<Run Method>
	% make:  Compile
	% make run:  Execute

The following method can also be used to run the program after it has been compiled.
	% ds[ei]db
	> run main.elf

Input is performed using dsecons running in a separate window.

<Controller Operation>
	None

<Remarks>
	None
