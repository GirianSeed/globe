[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2001 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample program showing how to control actuators

<Description>
       This program uses the vibration library to control actuators.

<Files>
        main.c

<Run Method>
      % make	: Compile

      After compilation, run dsedb with the following commands.
	% dsedb
	> run main.elf

<Controller Operation>
	(For a DUALSHOCK connection)
	Up-arrow	:  Rotate small motor
	Down-arrow	:  Stop small motor
	Left-arrow	:  Rotate large motor
	Right-arrow	:  Stop large motor


<Screen Description>
      SOCKET NUM	:  Virtual socket number
      PROFILE		:  Button profile
      VIB_PROFILE	:  Vibration profile
      STATUS		:  Controller state
      PAD DATA		:  Controller button information

<Notes>
      Multitaps are not supported.

