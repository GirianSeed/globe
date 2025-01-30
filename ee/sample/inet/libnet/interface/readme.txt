[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2002 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample Program Showing how to Handle Interface Events With the Libnet Library


<Description>
	This program shows how to handle interface events on the EE.
	An application can perform its own interface-related operations.

[Note]
	To use a USB Ethernet adapter with this sample program, an986.irx is required.
	Since the use of an986.irx is not permitted within a title (for details, see the technote dated 11/22/00 available on the developer website), an986.irx has not been included in the release package (under sce/iop/modules). Download it from the developer website under "Download" -> "Other Common Tools."

<Files>
        main.c:  Main program

<Execution Method>

        $ make			: Compile
        $ dsedb			: Start up dsedb
        > run main.elf <type>	: Execute main.elf
        <type>:  Sample number

<Controller Operation>
        None

<Comments>
        type 1:  DoInterfaceUp1()
        Brings up the interface using the conventional method.

        type 2:  DoInterfaceUp2():
        Brings up the interface without using a thread.
        Processing identical to sceLibnetWaitGetAddress() can be individually implemented by the EE application.

        type 3:  DoInterfaceUp3():
        Uses a thread to bring up the interface.
        While the interface is being brought up, operations such as drawing or user input can be performed in the main routine.



