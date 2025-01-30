[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2001 Sony Computer Entertainment Inc.
                                      All Rights Reserved


Sample Data for the HiG (High Level Graphics) Library


< Description of the Sample >
	The binary file used in the sample program for the HiG library is 
	created.
	It creates a little endian binary file from the EE assembler 
	source file with ee-objcopy.  The EE assembler source file
	can be converted with esconv contained in the eS package to 
	edit the resulting data from esconv.


< File >
	Makefile	: A make file 
	*.s		: A source file of the sample data 
	*.bin		: A binary file of the sample data
	*.tm2		: TIM2 format

< Usage >

        % make

	Add the converted file name using esconv to TARGETS in the makefile.
	A binary file is created with the make command.


< Note >
	"def.h" in the current directory and the constant that is defined
	by #define in /usr/local/sce/ee/include/libhip.h must be the same.
	As long as the data format conforms to the HiG library data format,
	data can be customized such as adding/deleting/recombining
	the plugin function or adding data.

	For details regarding the TIM2 format, refer to the TIM2 - 
	PlayStation 2 2D Graphics - File Format Specification on the 
        developer support web site.

< Outline of the Process >

	1. ee-gcc	: Creates the *.o object file from the *.s source 
			  file.

	2. ee-objcopy	: Creates the *.bin binary file from the *.o object
			  file.

	3. rm		: Deletes the *.o object file.
