[SCEI CONFIDENTIAL DOCUMENT]
DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
                  Copyright (C) 1999 by Sony Computer Entertainment Inc.
                                                     All Rights Reserved

The sample program for the Multithread 

< Description of the Sample >
These are basic samples for generating and activating threads, 
operating a thread priority and synchronizing threads and so on.


< File >

	createth.c		Basic sample for such as generating 
				and activating threads
	sleepth.c		Sample for SleepThread()/WakeupThread() 
	eventth.c		Sample for SetEventFlag()/WaitEventFlag() 
	startthargs.c		Sample for activating threads

< Activating the Program >

	% make					:Compiles the program

    createth execution

	Firstly, execute the following on another window
	% dsicons 

	subsequently execute the below

	% dsreset 0 2; dsistart createth.irx

	State of the 6 threads activation is displayed on the dsicons executed 
	window.


	Also executable with the following
	% dsidb
	> reset 0 2 ; mstart createth.irx


    sleepth execution

	Firstly, execute the following on another window
	% dsicons 

	subsequently execute the below

	% dsreset 0 2; dsistart sleepth.irx

	The prompt '0..5,up,down > ' appears on the dsicons executed window.
	Then the command input thread which is in an input waiting state 
	with a prompt appeared and another 6 threads are activated.   
	If type "up" and hit the RETURN key, a priority of the input waiting		thread mentioned earlier is raised in comparison with another 6 threads.  	

	If type "down" and  hit the RETURN key, a prompt appears on display
	and the waiting thread is lowered in a priority than another 6 threads.

	Type 0 to 5 and hit the RETURN key to make the corresponding thread 
	wake-up.

	Also executable with the following
	% dsidb
	> reset 0 2 ; mstart sleepth.irx


    eventth execution

	Firstly, execute the following on another window
	% dsicons 

	subsequently execute the below

	% dsreset 0 2; dsistart eventth.irx

	The prompt '0..5,a,up,down > ' appears on the dsicons executed window.
	Then the command input thread which is in an input waiting state 
	with a prompt appeared and another 6 threads are activated.   
	If type "up" and hit the RETURN key, a priority of the input waiting		thread mentioned earlier is raised in comparison with another  
	6 threads. 
	
	If type "down" and  hit the RETURN key, a prompt appears on display
	and the waiting thread is lowered in a priority than another 6 threads.
	
	Type 0 to 5 and hit the RETURN key to set an event flag corresponding
	bit.
	
	Type "a" and hit the RETURN key to set all of the event flag bits.

 
	Also executable with the following
	% dsidb
	> reset 0 2 ; mstart eventth.irx


    startthargs execution

	Firstly, execute the following on another window
	% dsicons 
	
	subsequently execute the below

	% dsreset 0 2; dsistart startthargs.irx

	State of activating arguments of the thread is displayed on the dsicons 	executed window.

	Also executable with the following
	% dsidb
	> reset 0 2 ; mstart startthargs.irx
