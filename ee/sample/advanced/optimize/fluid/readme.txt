[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2000 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample showing the optimization process

<Description>

This sample program shows EE CORE optimization can change the speed of a program.


<Optimization process>

The program optimized.c defines VER 1, 2, 3 and 4 to change the optimization level. Peak CPU cycles and the values of performance counters 0, 1 are output to the dsedb console. The performance counter 0 indicates the number of CPU cycles and performance counter 1 indicates the number of cycles for a DCACHE miss.

The version is defined at the start of a program and indicates the optimization process.

<Files>

	optimized.c  Program that includes the 
		     optimization process
        work.dsm 
        work.vsm
        Makefile 
	original.c  Program before any optimization 
		    (no make)       

<Run method>

        % make          :Compile
        % make run      :Execute

        You can also execute the program using the 
	following commands after compiling.

        % dsedb
        > run optimized.elf

<Controller operation>

        Up, Down, Left, Right (direction keys)  :Rotate 
		     camera up / down / left / right
        Triangle, Circle, R1 button   :Raise water surface
        Cross, Square button	      :Lower water surface

