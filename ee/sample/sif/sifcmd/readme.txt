[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
        Copyright (C) 1999 Sony Computer Entertainment Inc.
                                        All Rights Reserved

SIF CMD Protocol Sample Program

<Description>
	
This program shows how to use the SIF CMD protocol.

The program performs the following actions in the order shown.
         1.  Initialize SIF CMD
         2.  Register the buffer and command functions
         3.  Call destination-side command functions
         4.  Display command function results


<Files>
	main.c

<Execution>
	% dsreset 0 0
	% make:  Compile

After compilation, start up dsedb.
	% dsedb

Next, open a separate window and start up dsidb from an IOP-side sample (iop/sample/sif/sifcmd).

In the EE sample-side window, execute the following.
	> run main.elf

In the IOP sample-side window, execute the following.
	> mstart iopmain.irx

The program has completed normally if the following is displayed. 
	test0 = 10 test1 = 20


<Controller operation>
	None

<Notes>
