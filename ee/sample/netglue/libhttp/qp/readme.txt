[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
       Copyright (C) 2001 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program that uses libhttp or libhttps to perform quoted-printable encoding.

<Description>
This is a sample program that uses the quoted-printable encoding function of libhttp or libhttps.

<File>
	qp.c	: Main program

<Run Method>
	$ make	: Compile
	$ dsedb -r run qp.elf command <in_file> <out_file>	: Execute

Quoted-printable encoding or decoding is performed on in_file and the result is saved in out_file.
command specifies whether encoding or decoding is to be performed. The following two commands can be specified.
		-encode	Encoding
		-decode	Decoding

<Controller Operation>
	None

<Processing Summary>
1. Read an appropriate amount of data from in_file into memory.
2. Perform quoted-printable encoding or decoding on the data that was read.
3. Write the processed data to out_file.
4. If there is more data to be read from in_file, return to step 2.

