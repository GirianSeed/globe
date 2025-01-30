[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 2000 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample Program Showing How to Obtain IOP-side USB Keyboard Driver Information From the EE

<Description>
This program obtains the data of the USB keyboard sample driver from the EE.

Before executing this program, you must make the IOP-side USB keyboard sample driver, which is located in the following directory:

	sce/iop/sample/usb/usbkeybd. 

Also, when starting up the program, connect the USB keyboard to either USB port.

<Caution>
This sample cannot be used simultaneously with the USB keyboard library (libusbkd.a).


<Files>
        main.c :  Sample program for using the USB 
		  keyboard driver
        usbkeybd.c :  Library-type USB keyboard 
		      acquisition routine (this is a
		      sample)
        usbkeybd.h :  Same header file


<Execution>
        $ make :  Compile
        $ make run :  Execute

After compilation, the following method can also be used for execution:

        $ dsedb
        > run main.elf

<Display>

The display is explained based on the following example.

usbkeybd1 : port=2,1,1 : LED[00] KEY[00 00 00 00 00 00 00 00 ]
|_______|   |________|   |_____|     |______________________|
    A            B          C                    D

        A :  Keyboard number (0-7) assigned by IOP.
             This is not a device ID.

        B :  Port to which USB keyboard is connected.
             For the example above, B indicates that the 
	     keyboard is connected to port 2 of the system 
	     unit --> port 1 of the first stage hub --> 
	     port 1 of the second stage hub.

        C :  LED status.
              bit 0 :  NUM LOCK
              bit 1 :  CAPS LOCK
              bit 2 :  SCROLL LOCK
              bit 3 :  COMPOSE
              bit 4 :  KANA
              bits 5-7 :  Not used

        D :  Raw data that was sent via an interrupt 
	     transfer by the USB keyboard.
             Some keyboards can send longer data.
              byte 0 :  Modifier keys
                       bit 0 :  Left-Ctrl
                       bit 1 :  Left-Shift
                       bit 2 :  Left-Alt (Win), 
			        Left-option (Mac)
                       bit 3 :  Left-Win(Win), 
				Left-Apple (Mac)
                       bit 4 :  Right-Ctrl
                       bit 5 :  Right-Shift
                       bit 6 :  Right-Alt (Win), 
			        Right-option (Mac)
                       bit 7 :  Right-Win (Win), 
				Right-Apple (Mac)
              byte 1 :  Reserved
              byte 2 :  Key code
                :
              byte 7 :  Key code

Key Codes
See the "HID Usage Tables Document," which is distributed from the web site http://www.usb.org/developers/devclass_docs/Hut1_11.pdf.


<Controller Operation>
	None

<Remarks>
        None
