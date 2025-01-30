[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2001 Sony Computer Entertainment Inc.
                                      All Rights Reserved

This sample program uses libhttp or libhttps to perform BASE64 encoding.

<Description>
This sample program uses the libhttp or libhttps BASE64 encoding function.

<File>
base64.c	: Main program


<Execution>
$ make		: Compile
$ dsedb -r run base64.elf command <in_file> <out_file>
			: Execute

in_file is BASE64 encoded or decoded and the result is saved in out_file.

command specifies whether encoding or decoding is to be performed. The following two options can be specified.
	-encode	Encoding
	-decode	Decoding

<Controller Operations>
None

<Processing Summary>
1. Read an appropriate amount of data from in_file into memory.
2. Perform BASE64 encoding or decoding on the data that was read.
3. Write the processed data to out_file.
4. If the reading of data from in_file is not complete, return to step 2.

