[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample to display controller information

<Description>

This program performs a series of controller controls using the controller library such as obtaining button information and setting the actuator. However, actuator control is only compatible with DUALSHOCK.
	
<Files>
        main.c

<Activation>

	% make		: Compile

	After compilation run dsedb and execute
	% dsedb
	> run main.elf
	

<Controller operations>

	(when connecting a DUALSHOCK)
	up arrow	: rotate small motor  
	down arrow	: stop small motor	
	left arrow	: rotate large motor 
	right arrow	: stop large motor   

<Remarks>

Insert the controller into controller port 1 (port 0)
Not compatible with the Multi tap (SCPH-1070).
	
