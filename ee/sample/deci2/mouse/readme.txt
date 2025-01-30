[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample program showing how to download a program and simulate the motion of the mouse on the host, using the DECI2 protocol.


<Description>

This program uses the DECI2 protocol to download a program and simulate mouse motion.  This is done by providing the downloaded program with the host's mouse coordinates.

<Files>
	main.c        : main program on the target side
	vu0.c         : matrix and math functions
	pdriver.c     : sample protocol driver program
	pdriver.h     : sample protocol driver header
	linux/main.c  : main program on the host side
	linux/main.h  : header file
	linux/child.c : child process program
	linux/deci2.h : DECI2 protocol header
	linux/dbgp.h  : DBGP protocol header
	linux/dbgp.c  : DBGP program
	linux/dcmp.h  : DCMP protocol header
	linux/dcmp.c  : DCMP program
	linux/elf.c   : ELF analysis routine
	linux/list.h  : list manipulation header
	linux/list.c  : list manipulation routine
	linux/netmp.h : NETMP protocol header
	linux/netmp.c : NETMP program

<Execution>
	$ make		: compile
	$ make run	: run
	
	Or, after compiling,
	$ linux/mmouse main.elf


<Controller operations>
	None
 

<Notes>

This sample program uses a protocol number of 0xe000 and uses the following format for packets that come after the DECI2 header. X-Windows must be running to run this program.          

       3                   2                   1                  
     1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                               x                               |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                               y                               |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                               b                               |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	x 
	  x coordinate

	y
	  y coordinate

	b
	  button information

<Summary of operations>	

The following is a summary of the operations performed by the program.

On the host:
1. Establish TCP/IP connection with dsnetm
2. Using the netmp protocol CONNECT message, establish a deci2 connection
3. Reset target
4. Wait for connection with the EE to be established
5. Download program
6. Wait for the target-side program to open the new protocol
7. If a mouse event is generated, send the packet described above.

On the target:
1. Register the protocol driver
2. Render using mouse information from the host
