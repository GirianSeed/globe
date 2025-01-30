[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
       Copyright (C) 2000 Sony Computer Entertainment Inc.
                                      All Rights Reserved


Sample showing the optimization process

<Description>

This sample program shows EE CORE optimization can change the speed of a program.


<Optimization process>

The program optimized.c defines VER 1, 2, 3, 4 and 9 to change the optimization level. Peak CPU cycles and the values of performance counters 0, 1 are output to the dsedb console. The performance counter 0 indicates the number of CPU cycles and performance counter 1 indicates the number of cycles for a DCACHE miss.

The version is defined at the start of a program and indicates the optimization process.

<Files>

	original.c   Original before any 
		     optimization (no make)
        optimized.c  Program that includes the 
		     optimization process
	work.dsm 
	work.vsm
	makefile  Executes make on optimized.c.
	vu0.dsm (used only with VER5)
	vu0.vsm (used only with VER5)

	

<Run method>

        % make          : Compile
        % make run      : Execute

        You can also execute the program using the 
	following commands after compiling.

        % dsedb
        > run opti.elf

<Controller operation>

        Up, Down, Left, Right (direction keys)        
		 :Rotate camera up / down / left / right
