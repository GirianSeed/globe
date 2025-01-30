[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2001 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample program showing how to display controller information

<Description>
This program uses controller library 2 to get button information and the button profile.

<Files>
        main.c

<Run Method>
      % make	: Compile

After compilation, run dsedb with the following commands.
	% dsedb
	> run main.elf

<Controller Operation>
      None

<Screen Description>
      SOCKET NUM	:  Virtual socket number
      PROFILE		:  Button profile
      STATUS		:  Controller state
      PAD DATA		:  Controller button information

<Notes>
Multitaps are not supported.


