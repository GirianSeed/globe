[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 2000 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Sample program for obtaining IOP-side USB mouse driver information from the EE

<Description>

This program obtains USB mouse data from the EE side.

For execution, it is necessary to Make the IOP-side USB mouse sample driver, which is in the following directory: sce/iop/sample/usb/usbmouse

Before starting the program, connect the USB mouse to either USB port.

<Files>
        main.c        : USB mouse driver usage example
	usbmouse.c    : Sample routine to get USB mouse data
        usbmouse.h    : Header file


<Execution>
        $ make          :	Compile
        $ make run      :	Execute

After compilation, execution is possible using the following method:
        $ dsedb
        > run main.elf

< Messages >

The following examples are used to explain messages.

usbmouse3 : port=2,4 Button:00 X:  9 Y: -2 W: 12 [00 00 00]
|_______|   |______| |_______| |_________| |___| |________|
    A           B        C          D        E       F

	A : Mouse number (0-7) assigned by IOP
    	    Not device ID number

	B : Port the USB mouse is connected to.
            Example B shows a mouse connected to port 
	    2 of the DTL-T10000 and port 4 of the HUB.

        C : Status of mouse button
            bit 0    : Button 1 (0: OFF, 1: ON)
            bit 1    : Button 2 (0: OFF, 1: ON)
            bit 2    : Button 3 (0: OFF, 1: ON)
            bits 3-7 : Device-specific

        D : USB mouse displacement
        
        E : Wheel displacement 
	    (Not displayed when there is no wheel.)

        F : Device-dependent data 
	    (Not displayed when there is no 
	     device-dependent data.)

<Controller operation>
	None

<Notes>
        None