[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2000 Sony Computer Entertainment Inc.
                                      All Rights Reserved


graphics framework sample data

<Description>

Creates binary files used in the graphics framework sample. Uses ee-objcopy to create little-endian binary files from EE assembler source files. The EE assembler source files can be converted using es2raw (included with the eS package).


<Files>
	Makefile	: Makefile
	*.s		: Sample data source file 
	*.bin		: Sample data binary file 

<Run method>

        % make

Add filenames converted with es2raw to TARGETS of the Makefile, then use the make command to create binary files.

<Notes>

Refer to the document "es2raw.txt" in the eS package for details on the data formats.

<Process summary>

	1. ee-gcc	: Generate *.o object file 
			  from *.s source file

	2. ee-objcopy	: Generate *.bin binary file 
			  from *.o object file

	3. rm		: Delete *.o object file 
