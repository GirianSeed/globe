[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
	Copyright (C) 2002 Sony Computer Entertainment Inc.
                                        All Rights Reserved

	ATOK Library Sample Program


<Description>

This sample program shows how to use the ATOK library (libatok).

To use the ATOK library, you must have a PlayStation 2 hard disk drive installed with:
- The browser that is on the "HDD Utility Disk" that comes with the PlayStation 2 Hard Disk Drive Unit.
- The PlayStation BB Navigator that is on the "PlayStation BB Navigator Disk" that comes with the PlayStation BB Unit.

The following packages are also used by the program, so they should be downloaded and unpacked in the current directory.

	- Shift JIS <-> UTF8 character code conversion library package (libccc)
	- Simple font library package (libpfont)



<Files>

	Makefile	:  Make file
	main.c		:  Main program
	pad.c		:  Controller-related program
	pad.h		:  Controller-related header
	kbd.c		:  USB keyboard-related program
	kbd.h		:  USB keyboard-related header

	files under skb/:  Software keyboard-related programs that use the ATOK library (using libpfont). For details, see skb/readme_j.txt.



<Shift JIS <-> UTF8 character code conversion library package (libccc)>
	The Shift JIS <-> UTF8 character code conversion library package is used by the sample program. This package has been released on the developer website as a separate library for performing Shift JIS and UTF8 character code conversions. Since it is required by make, download the package from the developer support website and unpack it in the sample program directory.

<Simple font library package (libpfont)>
	The simple font library (libpfont) package is used by the sample program. This package has been released on the developer website as a separate library. Since it is required by make, download the package from the developer website and unpack it in the sample program directory.


<Execution method>
	% make clean	:  Clean
	% make		:  Compile
	% make run	:  Execute


<Controller operations>

	Circle button	:  Open keyboard
	Cross button	:  Cancel and end
	START button	:  Decide and end

	For details about other operations, see skb/readme.txt.


<Notes>

	- Note that this program may be affected by the setting of the environment variable LANG because the source code contains Shift JIS strings.

	  When using this program, specify Shift JIS for the environment variable LANG.

	 For .bashrc:
	 $ export LANG=C-SJIS

	 For .cshrc:
	 $ setenv LANG C-SJIS

       - The ATOK engine and dictionary files necessary to execute the 
          ATOK library are installed by installing the browser on the
          HDD utility disc included with the hard disk drive and network
          adaptor , and the PlayStation BB Navigator on the 
          PlayStation BB Navigator disc included with the 
          PlayStation BB Unit.

          Therefore, it is necessary to connect the hard disk drive with 
          the browser and PlayStation BB Navigator installed from the 
          respective discs to the DTL-T10000(H).

        - The HDD utility disc that comes with the hard disk drive and 
          network adaptor, and the "PlayStation BB Navigator" disc that 
          comes with the PlayStation BB Unit run only on the 
          PlayStation 2 console and Debugging Station(DTL-H10100,
          DTL-H30100 or later).
          Be aware that the above discs do not operate on 
          a debugging station other than those mentioned above and 
          DTL-T10000.

