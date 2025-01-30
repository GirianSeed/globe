[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
         Copyright (C) 2003 Sony Computer Entertainment Inc.
                                         All Rights Reserved

Sample Program for Outputting Interface Information on libeenet


<Description of the Sample>
This program is a sample library that displays interface 
information on libeenet.  It provides information that 
is approximately equivalent to the output produced by the 
ifconfig command in Linux.

<File>
        ifinfo.c      : Main program

<Functions>
        void sceEENetShowIfinfo(const char *ifname)
                Displays interface information specified with ifname.

        void sceEENetShowAllIfinfo(void)
                Displays all the interface information.

<Activating the Program>
        $ make                          : Compiles the program

Link libifinfo.a to a program that uses libeenet to call 
the above functions.  They must be called at a location where APIs 
for libeenet are usable (i.e. between sceEENetInit() and 
sceEENetTerm()). 

<Note>
Do not use this program for any purpose other than debugging.
