[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2001 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample program for manipulating HDD files

<Description>
This sample program shows how to access an HDD file.

1. The contents of main.c are copied to create copy.c.
2. Incremental data is created every four bytes and written to the file incdata. incdata is read and checked to make sure that it is correct.

<File>
	main.c

<Run Method>
	% make:  Compile
	% make run:  Execute

The following method can also be used to run the program after it is compiled.
	% dsedb
	> run main.elf

The program has terminated normally when this is displayed: 
		sample end
	
<Controller Operation>
	None

<Remarks>
	None
