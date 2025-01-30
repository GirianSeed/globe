[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2001 Sony Computer Entertainment Inc.
                                      All Rights Reserved

This sample program uses libhttp or libhttps to perform URL escape and unescape processing.

<Description>
This sample program uses the libhttp or libhttps URL escape and unescape functions.

<File>
urlesc.c	:  Main program

<Execution>
$ make		: Compile
$ dsedb -r run urlesc.elf command <string>
			: Execute

URL escape or unescape processing is performed on string and the result is displayed.

command specifies whether escape or unescape processing is to be performed. The following two options can be specified.

		-escape		Escape processing
		-unescape	Unescape processing

<Controller Operations>
None

<Processing Summary>
1. Perform URL escape or unescape processing for the string specified by string.
2. Display the result.

