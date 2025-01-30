[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample program demonstrating multi-threading

<Description of sample program>

This sample program renders objects using multi-threading. Semaphores are used to switch between threads. Two control threads, one DMA kick thread, and multiple object threads are used. A single object thread is used to display a single object.

<Files>

	thread.c

<Execution>

	% make		: compile
	% make run	: run

The sample program can also be executed by entering the following after compilation.

	% dsedb
	> run thread.elf

<Controller operations>

	circle/X buttons	: create, delete thread
	triangle button		: randomly reset rotation
				  angle
	square button		: initialize rotation angle
	R1/R2 buttons		: change object spacing
	left/right arrow (direction keys) : displace object along 
				    X-axis
	up/down arrow buttons	: displace object along 
				  Y-axis
	L1/L2 buttons		: displace object along 
				  Z-axis
	START button		: pause
	SELECT+triangle button	: initialize (random)
	SELECT+square button	: initialize (ordered)
	SELECT+R1 button	: set object spacing to 0
	SELECT+R2 button	: initialize object spacing
	SELECT+left/right arrow buttons : initialize X-axis 
					   position of object
	SELECT+up/down arrow buttons	: initialize Y-axis 
					  position of object
	SELECT+L1/L2 buttons	: initialize Z-axis position 
				  of object

[Semaphore]

packet_sema: 
	indicates whether data can be added to packet
	(* will work even if not available)
send_signal: 
   	indicates that addition of data to packet has been 	completed and that DMA transfer can take place.
rotate_signal: 
	indicates that operation has been completed and that
	control can be transferred to a different object.

[Operations of the different threads]

Object thread: 
If packet_sema can be obtained, data is added to the packet. rotate_signal is sent after releasing packet_sema.

Control thread:
When control comes around, a kick_signal is sent, followed by a rotate_signal.
Control reaching the thread indicates that processing of objects has been
completed.

DMA kick thread:
Waits for kick_signal after packet is prepared and packet_sema is registered.
When kick_signal is received, post-processing is 
performed on the packet and DMA transfer is performed.

Object rotate thread:
Waits for rotate_signal. When the signal arrives, the priority2 threads being controlled are rotated and control is transferred to a different
object.

[Illustration of thread structure]

     priority
	1	object rotate thread
	10	DMA kick thread
	20	object thread
		object thread
		:
		control thread

