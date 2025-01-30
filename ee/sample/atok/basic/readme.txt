[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
          Copyright (C) 2003 Sony Computer Entertainment Inc.
                                         All Rights Reserved

ATOK Library Sample Program

<Description>
        This sample program demonstrates the basic use of the ATOK library (libatok). 
It also includes many comments. 

        The following are required by the ATOK library to run the sample program. 

	- The browser that is on the HDD Utility Disk provided with the PlayStation 2 hard disk drive unit must be installed.
	- The PlayStation BB Navigator that is on the PlayStation BB Navigator Disk provided with the PlayStation BB Unit must also be installed.

        The following packages are used by the program. They should be downloaded from the developer support website and unpacked in the current directory.

        - Shift JIS <-> UTF8 character code conversion library package (libccc)
        - Simple font library package (libpfont)

<Files>
        Makefile	:  Makefile
        main.c		:  Main program
                 	   atok library operations are only performed in this file. 
        app.h		:  Various application configuration headers
        ccc.c		:  libccc-related program 
        ccc.h		:  libccc-related headers
        font.c		:  font display-related program
        font.h		:  font display-related headers
        info.c		:  Screen information-related program
        info.h		:  Screen information-related headers
        io.c		:  I/O-related program
        io.h		:  I/O-related headers
        pad.c		:  Controller-related program
        pad.h		:  Controller-related headers      

<Shift JIS <-> UTF8 character code conversion library package (libccc)> 
<Simple font library package (libpfont)> 
        These packages are required by the sample program and can be downloaded from the developer support website. They should be unpacked in their own directories before running make. The directory paths are set in environment variables in the Makefile (see below).

<Makefile>
        The following environment variables must be set in the Makefile. They specify the paths to the directories where the libccc and libpfont packages were unpacked. 

          PFONT_PATH	:  libpfont directory path
          CCC_PATH	:  libccc directory path

        Note that the default setting for each package is the directory of the sample program (atok/basic). 

<Execution>
        % make clean	:  Clean
        % make		:  Compile
        % make run	:  Execute 

<Controller operation>
        See the information in info.c. Controller operation depends on the current mode. 

<Restrictions and precautions>
        - Note that the program may be affected by the setting of the LANG environment variable because the source code contains strings encoded in Shift JIS. 

          Before running the program, set the LANG environment variable to C-SJIS (Shift JIS). 

         For .bashrc: 
         $ export LANG=C-SJIS

         For .cshrc:
         $ setenv LANG C-SJIS

        - The ATOK engine and dictionary files needed by the ATOK library must be installed by installing the browser that is on the HDD Utility Disk provided with the PlayStation 2 hard disk drive unit, and by installing the PlayStation BB Navigator that is on the PlayStation BB Navigator Disk provided with the PlayStation BB Unit.

	  Note that to use the ATOK library, a PlayStation 2 hard disk drive must be connected to the DTL-T10000(H) and each disk that will be used must be installed on it.

	- The HDD Utility Disk provided with the PlayStation 2 hard disk drive unit and the PlayStation BB Navigator Disk provided with the PlayStation BB Unit will work only on a PlayStation 2 console or Debugging Station (DTL-H10100 or DTL-H30100 and later). Note that these disks will not work on a DTL-T10000 or on Debugging Stations other than the models mentioned above.



