[SCE CONFIDENTIAL DOCUMENT]
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.5
	           Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                   All Rights Reserved

ATOK Library Sample Program


< Description of the program >

	This is a sample program that demonstrates examples of how ATOK 
	library (libatok) is used.

	The environment for operating the ATOK library requires that 
	the hard disk drive unit for "PlayStation 2" is connected to 
	the console and the system software must be installed correctly.

	The following packages are also required for running this program. 
	Please download them from the developers support website and 
	uncompress in current directories accordingly.
	
	- Shift JIS <-> UTF8 character code conversion library package 
	  (libccc )
	- Simplified font library package (libpfont)


< Files >

	Makefile         : Make file
	main.c           : Main program
	pad.c            : Program related to the controller 
	pad.h            : Header related to the controller 
	kbd.c            : Program related to the USB keyboard 
	kbd.h            : Header related to the USB keyboard

	Below skb/       : Program related to the Software Keyboard 
			   That Uses ATOK Library (libpfont is used)
	                   See skb/readme_e.txt for details.

 < Shift JIS <-> UTF8 character code conversion library package (libccc) >

	This program uses the Shift JIS <-> UTF8 character code conversion 
	library package (libccc) which has been separately released as 
	a character code conversion library for Shift JIS and UTF8 on 
	the developers website. Since this package will be needed for 
	executing make command, download it from the developers support 
 	website and uncompress in the program directory accordingly.


< Simplified font library package (libpfont) >

	This program uses the library package which has been separately 
	released as simplified font library (libpfont). Since this 
	package will be needed for executing make command, download it 
	from the developers support website and uncompress in the program 
	directory accordingly.


< Activating the program >

	% make clean  	: Clean
	% make       	: Compile
	% make run    	: Execute


< Using the controller >

	Circle button	: Open the keyboard
	Eks button	: Close the keyboard
	START button	: Confirm the character entry

	For more details of operation, please refer to skb/readme_e.txt.


< Restrictions and precautions >

	- Please note that this program contains Shift JIS strings in 
	  the source, which may be affected by the environment variable, 
	  LANG.

	  Before using this program, specify Shift JIS in the LANG 
	  environment variable.
	  
	  For .bashrc: 
	  $ export LANG=C-SJIS

	  For .cshrc:
	  $ setenv LANG C-SJIS

