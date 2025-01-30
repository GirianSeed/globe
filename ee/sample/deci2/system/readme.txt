[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program showing how to call the system() function on the host using the DECI2 protocol


<Description>

In this program, a program from the target side uses the DECI2 protocol to call the system() function on the host and the results are displayed.

<Files>
	main.c        : Main program on the target side
	pdriver.c     : Sample protocol driver program
	pdriver.h     : Sample protocol driver header
	linux/main.c  : Main program on the host side
	linux/deci2.h : DECI2 protocol header
	linux/dcmp.h  : DCMP protocol header
	linux/netmp.h : NETMP protocol header

<Execution>
	$ make		: compile
	
	After compiling, enter the following:
	$ cd linux
	$ ./hsys -d target name (or IP address)
	Then, open a new window and
	
	$ make run
	
	or

	$ dsedb
	> run main.elf

<Controller operations>
	None


<Notes>

In the sample protocol defined here, the protocol number = 0xe000. The format of the packets coming after the DECI2 header is as shown below.

       3                   2                   1                  
     1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                            Result                             |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                                                          Data ...  |
                                                       -+-+-+-+-+-+-+

Result 
------

Set to 0 if packet is a command execution request from the target program. Set to the return value of system() if packet is a response from the host.

Data
----
Null-terminated data.

If the packet is a request from the target program to execute a command, the command string is entered here. If the packet is a response from the host, the standard output resulting from the execution of the command via system() is entered here.

<Summary of operations>

The following is a summary of the operations performed.

On the host side:
1. Establish a TCP/IP-level connection with dsnetm
2. Establish a deci2-level connection using the netmp protocol CONNECT message
3. Wait indefinitely for a request from the target
4. When a request to execute a command is received from the target, the command is executed. The results are first saved in a temporary file and then sent back to the target.


On the target side
1. Register the protocol driver
2. Send command to the host
3. Wait indefinitely for the results of the command to come back
4. Display results when they come back

