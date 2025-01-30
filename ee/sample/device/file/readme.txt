[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
     	Copyright (C) 1999 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Host machine file handling

<Description>

This sample program shows how to access a file on the host machine.

1. The file copy.c is created by copying the file main.c.
2. Incremental data is generated from every fourth byte, then written to a file named incdata.  incdata is then re-read to check whether the data is correct.
3. incdata is read in asynchronous mode to check whether it is being read correctly.

<Files>
	main.c

<Startup>
	% make :  Compile
	% make run :  Execute

The following method can also be used to execute the program after compilation.
	% dsedb
	> run main.elf

If the following message is displayed, processing terminates normally.

		sample end.

Confirm whether the file copy.c is the same as the file main.c.
	% diff main.c copy.c

<Controller operation>
	None

<Remarks>
	None

