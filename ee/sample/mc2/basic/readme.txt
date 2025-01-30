[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2002 Sony Computer Entertainment Inc.
			              All Rights Reserved

Sample Program Showing how to Access a PS2 Memory Card

<Description>
	This program uses Memory Card Library 2 to access a PS2 Memory Card.

<Files>
	main.c

<Execution Method>

	% make			:  Compile

	To start up dsedb after compilation, execute the following.
	% dsedb
	> run main.elf

<Controller Operation>
        START button		:  Confirm formatting
        Arbitrary button	:  Confirm repetition

<Comments>
	The following processing is repeatedly executed for the PS2 Memory Card that is inserted in MEMORY CARD Slot 1.
	The current processing function and frame count are displayed on-screen.

	sceMc2GetInfo()
		V
	Is a PS2 Memory Card inserted? (no) ==> sceMc2GetInfo()
		V
	Is it unformatted? (Yes) ==> START button ==> sceMc2Format()
		V
	sceMc2GetDir()
		V
	Are there files?  (no) ==> sceMc2CreateFile
		V
	sceMc2WriteFile()
		V
	sceMc2ReadFile()
		V
	sceMc2DeleteFile()
		V
        Press any button
		V
	Repeat


	(*)  The multitap (SCPH-1070) is not supported.

