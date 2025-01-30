[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 2000 Sony Computer Entertainment Inc.
                                       All Rights Reserved


Checking basic features of the Memory Card library

<Description>

This program uses the Memory Card library to perform operations such as format, directory list and generation of icon.sys.

When accessing a Memory Card connected via a Multi tap, define the #define UseMultiTap within the source as 1.


<Files>
	main.c
	slime1.ico
	slime2.ico
	slime3.ico


<Execution>

        % make          : compile

After compiling, run using dsedb.
        % dsedb
        > run main.elf

<Controller operations>

Main menu:
	Arrow keys ... Move menu selection.
	START button ... Run the selected menu item.

1. get dir
	up arrow/down arrow (direction keys)... Move entry mark.
	L1/L2 button ...  Scroll entry list if there are 
			  16 entries or more.
	START button ...  Enter subdirectory only if a 
			  directory entry is marked.
	circle button ... Select marked entry.
			  The selected entry name is 
		          displayed directly below the menu.
			  The selected menu item will be 
			  used for 3. file status.
	X button ...      Delete marked entry.
	SELECT button ... Exit get dir menu.
			  The get dir menu will also be 
			  exited if an error occurs.

2. mkdir + icon ... Create directory using the title name 
		    as the entry name, generate icon.sys, 
		    and copy icon file. These operations 
		    are a minimum requirement for saving 
		    game data. Game data is placed in the 
		    directory created here. Any filename 
		    can be used.

	   left, right arrow ... Move cursor.
	   up, down arrow ...    Change entry name.
	   X button ...     	 Backspace
	   square button ...     Change character under 
				 cursor to 0.
	   triangle button ...   Change character under 
				 cursor to A.
	   circle button ...     Change character under 
				 cursor to a.
	   R1 button ...         Insert space.
	   START button ...      Initialize directory 
			         using the specified 
			         title name.
	   SELECT button ...     Cancel directory creation.

3. file status ... 1. Displays, changes status of file 
		      selected from the get dir menu.

	   L1/L2 buttons ... Move item mark.
	   left, right arrow ... Move cursor within item.
	   up, down arrow ... Change value of item.
	   START button ... Apply change to file.
	   SELECT button ... Cancel change.

	   attribute: file attributes
		1 ... sceMcFileAttrPS1	    PS1 data
		p ... sceMcFileAttrPDAExec  PDA application
		d ... SCE_STM_D		    directory
		c ... SCE_STM_C		    copy protected
		x ... SCE_STM_X		    executable
		w ... SCE_STM_W		    writable
		r ... SCE_STM_R		    readable

	file size: file size is displayed only if the d 
		   attribute is x

	create: date and time at which file was created

	modify: data and time at which file was last 
		modified

4. format ...          		Formats the Memory Card.
	   circle button ...    Format
	   any other button ... Return to main menu without
			        formatting.

5. un-format ...       		Puts the Memory Card in an 
				unformatted state.
	   circle button ...    Perform unformatting. When 
				the unformatting is
			        completed, the Memory Card 
				should be removed.
	   any other button ... Return to main menu without
				formatting

6. rename ... 			Change the file (directory) 
				name.
	   left,right arrow ... Move the cursor.
	   up,down arrow ... 	Correct the file (directory) name.
	   X button ... 	Backspace 
	   square button ... 	Change character above 
				cursor to 0.
	   triangle button ... 	Change character above 
				cursor to A.
	   circle button ... 	Change character above 
				cursor to a.
	   R1 button ... 	Insert space.
	   START button ... 	Reflect file content 
				corrections in the file 
			        (directory) name. 
	   SELECT button ... 	Cancel content corrections. 


7. unload.irx ... 		Unloads/reloads the IOP module 
				for libmc.
	Any button (first) ... 	Unloads the module.
	Any button (second) ... Reloads the module.
	Any button (third) ... 	Removes from menu.


<Notes>
	None
