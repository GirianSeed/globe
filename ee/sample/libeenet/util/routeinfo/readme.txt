[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
        Copyright (C) 2003 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Sample Program for Displaying Routing Information on libeenet

<Description of the Sample>
This program is a sample library that displays routing 
information on libeenet.  It provides information that 
is approximately equivalent to the output produced by the 
route command in Linux.

<File>
        rtinfo.c      : Main program

<Function>
        void sceEENetShowRouteinfo(void)
                Displays IPv4 routing information.

<Activating the Program>
        $ make                          : Compiles the program

Link rtinfo.a to a program that uses libeenet to call 
the above function.  It must be called at a location where APIs 
for the libeenet are usable (i.e. between sceEENetInit() and 
sceEENetTerm()). 

<Note>
Do not use this program for any purpose other than debugging.
