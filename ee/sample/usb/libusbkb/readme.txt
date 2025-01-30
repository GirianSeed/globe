[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2001 Sony Computer Entertainment Inc.
                                      All Rights Reserved

USB keyboard library (libusbkb.a) sample

<Description>
This program is a sample for the USB keyboard library (libusbkb.a).

<Precautions>
sce/ee/sample/usb/usbkeybd/ is a similar sample program, however, it uses usbd.irx. It should not be used together with this sample program.

<File>
	main.c:		Sample for using the USB keyboard library

<Run method>
        $ make          :Compile
        $ make run      :Execute

After compilation, the program can also be executed with the following method.
        $ dsedb
        > run main.elf

<Operation>
When a USB keyboard is connected to the USB bus of the PlayStation 2 and key input is performed, the input key will be displayed on-screen.

Pressing the "Katakana/Hiragana" key allows you to switch between "50 Sounds (gojuu on)" input and alphabetic input.

Pressing the "F1" key allows you to switch read mode between character input mode and packet mode.

Pressing the "Ctrl+Alt+Delete" keys simultaneously terminates the library, unloads the IOP module and terminates the sample programs.

<Display>
	This is described based on the following example.

	usbkeybd0 : port=1,1 : LED[01] MKEY[00] KEY['a' ]
        |_______|   |______|   |_____| |______| |_______|
            A           B         C        D        E
	
        A : Keyboard number alloted by IOP (0-7).
            This is not a device ID.

        B : The port to which USB keyboard has been connected.
            For example,
            port1--> Means that the keyboard has been connected to the port 1 of the first stage HUB

        C : LED state (0:OFF 1:ON)
              bit0  :NUM LOCK
              bit1  :CAPS LOCK
              bit2  :SCROLL LOCK
              bit3  :COMPOSE
              bit4  :KANA
              bit5-7:not used
	
	D : State of Modifier keys (0:OFF 1:ON)
              bit0:Left-Ctrl
              bit1:Left-Shift
              bit2:Left-Alt(Win),Left-option(Mac)
              bit3:Left-Win(Win),Left-Apple(Mac)
              bit4:Right-Ctrl
              bit5:Right-Shift
              bit6:Right-Alt(Win),Right-option(Mac)
              bit7:Right-Win(Win),Right-Apple(Mac)

        E : Pressed key
	    Example 1: 	['a' ]
Indicates that lower-case character 'a' has been pressed. 

            Example 2: 	[(1) ]
Indicates that the character '1' from the 10 keys has been pressed.

            Example 3: 	['\0']
                 	This is the OFF code.
This appears when a key has been released or when there is a change in the Shift, Ctrl, Alt, Win keys.

            Example 4: 	[803A]
This is a key code that cannot be converted to ASCII.
                 
                 bit0-7 :USB key code
                 bit8-13:Reserved
                 bit14  : When '1', shows that this is (one of the) 10 keys. 
                 bit15  : When '1', shows that this is a raw code. 
                 
For the key codes, 
refer to the "HID Usage Tables Document" distributed with the http://www.usb.org/developers/devclass_docs/Hut1_11.pdf"

            Example 5: 	['a' 'b']
Shows that both lower-case characters 'a' and 'b', have been pressed simultaneously.

            Example 6: [J C1]
This is a kana character. The same code as for a JIS half-width kana is returned.


