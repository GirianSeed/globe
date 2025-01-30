[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 2000 Sony Computer Entertainment Inc.
                                       All Rights Reserved


Sample program: Confirming PDA library basic functionality

<Description>

This program confirms the usage of PDA library functions and corresponding responses to function calls.

To access a memory card that is connected via a multitap, define 1 for #define UseMultiTap within the source.
	

<File>
	main.c


<Run method>

        % make:  Compile

        Start up dsedb after compilation. 
	Then execute the following.

        % dsedb
        > run vsync.elf

<Controller functions>

Main menu:
Direction keys ... Use to select a menu item to mark.
START button ... Executes the marked menu item.


1. get dir ... Displays a file list (libmc.a function)
up/down (direction keys) ... Used to move the entry mark.
L1 or L2 button ... Scrolls the entry list when there are 16 or more entries.
SELECT button ... Exits the get dir menu.
		Exits the get dir menu even when an error occurs.


2. pda info ... Gets / sets information that can be handled using sceMcxGetInfo() / sceMcxSetInfo().
left/right/up/down ... Used to move the selection item.
L1, L2, R1, or R2 button ... Changes the value of the selection item.
START button ... Reflects the modified contents in the PDA.
	(Day of week is automatically calculated.)
SELECT button ...Cancels modified contents and returns to main menu.

Running PDA application ... Displays the number of the application that is executing or sets the application number to be executed.

Speaker ... Displays or sets the enabled state of the speaker.

Infrared ... Displays or sets the enabled state of the infrared transmission function.

Flash write ... Displays or sets the writability status of the flash ROM from the PDA application.

LED on ... Displays or sets the enabled state of the LED.

Serial No ... Displays the serial number of the PDA.

Date ... Displays or sets the contents of the real-time clock. 


3. disp trans ... Turns the display ON or OFF during a PDA file transfer.
left/right/up/down ... Used to move the selection item.
L1, L2, R1, or R2 button ... Changes the value of the selection item.
START button ... If pressed while [start] is selected, starts the display.
		If pressed while [stop] is selected, forcibly terminates the display.
SELECT  button ... Cancels the setting.

dir ... Specifies the transfer direction (does not affect an application already started).

time ... Sets the time until the display is to stop, if [stop] has not been executed.


4. read dev ... Executes reading for the device entry.
left/right/up/down ... Used to move the selection item.
L1, L2, R1, or R2 button ... Changes the value of the selection item.
START button ... Executes the setting.
SELECT button ... Cancels the setting.

device No. ... Specifies the device number to be called.

param size ... Specifies the number of bytes in the fixed-length parameter section.

data size ... Specifies the number of bytes in the variable-length data section.

The upper of the two memory dumps shows the contents of the fixed-length parameter section, and the lower one shows the contents of the variable-length data section.


5. write dev ... Executes writing for the device entry.
left/right/up/down ... Used to move the selection item.
L1, L2, R1, or R2 button ... Changes the value of the selection item.
START button ... Executes the setting.
SELECT button ... Cancels the setting.
Cross button ... Clears the contents of the variable-length data part.

device No. ... Specifies the device number to be called.

param size ... Specifies the number of bytes in the fixed-length parameter section.

data size ... Specifies the number of bytes in the variable-length data section.

The upper of the two memory dumps shows the contents of the fixed-length parameter section, and the lower one shows the contents of the variable-length data section.

The buffer for the variable-length data section is shared with the buffer for 4. read dev and 6. edit mem, and the execution results of using the 4. or 6. menu items can be used by this menu item. To clear the buffer, press the Cross button.


6. edit mem ... Displays or alters the PDA memory contents.
left/right/up/down ... Used to move the selection item.
L1, L2, R1, or R2 button ... Changes the value of the selection item.
(If pressed when current offset is selected, it enables you to view the contents of a separate address, without re-entering the offset.)

START button ... Transfers the modified contents to the PDA.
SELECT button ... Cancels the modified contents.

current offset ... Displays the starting address of the memory dump that is currently being displayed.

offset = ... Displayed when this menu is entered or when the SELECT button is pressed. Enter a value to change the starting address of the memory dump.

Depending on the address that is accessed, the PDA may cause a bus error, or the stored data may not be able to be read in its current form.


7. edit UIFS ... Displays or alters the user interface status.
left/right/up/down ... Used to move the selection item.
L1, L2, R1, or R2 button ... Changes the value of the selection item.
START button ... Transfers the altered contents to the PDA.
SELECT button ... Cancels the altered contents.

Alarm ... Displays or sets the alarm time and alarm ON/OFF status.

key lock ... Displays or sets the key lock status.

Volume ... Displays or sets the speaker volume.

Area code ... Displays the PDA destination.

Rtc set ... Displays or sets the flag indicating that the real-time clock was set.

Font addr ... Displays the storage address of the font that is used for the LCD display.


8. LED display ... Turns the LED ON or OFF.
left/right ... Used to select on or off.
START button ... Transfers the selected contents to the PDA.
SELECT button ... Cancels the selected contents.

9. unload .irx ... Unloads/reloads the IOP module for libmcx.
Any button (first) ... 	Unloads the module.
Any button (second) ... Reloads the module.
Any button (third) ... 	Removes from menu.


<Remarks>
None
