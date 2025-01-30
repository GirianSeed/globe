[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
        Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program showing how to create key frame matrix animation data by rotation axis interpolation

<Description>

This program is located in the intr_key directory and performs key frame animation for a joint matrix using a method known as rotation axis interpolation.

The program in the mkdata directory creates a parameter sequence for rotation axis interpolation from the original key frame matrix sequence and outputs the newly created sequence to a file.

<Files>
	main.c		: Main functions and input/output 
			  functions
	axis.c		: Rotation axis interpolation-
			  related functions
	matrix.c	: Matrix arithmetic functions
	matseq.c	: Input data (key frame matrix 
			  sequence)
	axis.h		: Header file
	matrix.h	: Header file
	axisseq.ref	: Parameter sequence data


<Execution>
	% make:  Compile
	% make run:  Execute

This program can also be executed by the following method.
	% dsedb
	> run *.elf
	
The program completes normally when the contents of the output file axisseq.data are the same as the contents of axisseq.ref.

<Controller operation>
	None

<Notes>

The output data is written to the axisseq.data file in text format.This data has the same format as the data that will be used by the sample runtime program. To output data in binary format, change writeMatrix(), writeAxis(), or other functions appropriately.
