[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
       Copyright (C) 2002 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Displaying Connection Information for libeenet 


<Description>
This sample program displays libeenet connection information. It provides almost exactly the same information as is output by the Linux "netstat -a" command.

<Files>
        netstat.c		:  Main program

<Functions>
        void sceEENetShowNetstat(void)

               Displays IPv4 connection information.

 
<Activating the program>

        $ make:  Compile

The program uses libeenet and links to libnetstat.a to call the functions described above. The functions must be called from a place where the libeenet library API can be used (somewhere between sceEENetInit() and sceEENetTerm()).

<Comments>
This program should only be used for debugging.

