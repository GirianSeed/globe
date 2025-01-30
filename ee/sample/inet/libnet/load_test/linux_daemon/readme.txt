[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2000 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample that uses TCP to send and receive packets between a client and server on UNIX. (server program)


<Description>

This program is a UNIX version of the sample program that performs a TCP passive open and sends and receives data with a connected client. The program has been verified to run on Linux.
Operation is the same as /usr/local/sce/ee/sample/inet/load_test/daemon.

<Files>
        main.c        : Main program


<Run method>

        $ make		: Compile
        $ ./load_testd    : Start server program

Once the server starts, a connection is established from the client program and communication begins.
Client programs that run on the EE and IOP should be prepared.
Refer to both documents below.

/usr/local/sce/ee/sample/inet/load_test/client/readme.txt
/usr/local/sce/iop/sample/inet/load_test/client/readme.txt

<Note>

Use this sample to understand the comparison between inet API and BSD socket API as well as the way they operate together.
 
