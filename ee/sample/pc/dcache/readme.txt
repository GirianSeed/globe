[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program showing how to count D Cache misses using the Performance Counter

<Description >

This sample program counts D Cache misses using the Performance Counter. Four examples are provided.

In the first example, counter 0 is set to the "Load Completed" event and counter 1 is set to the "D Cache miss" event. lq instructions to the cache area are measured. A D cache miss will be generated on the initial access, but since there are 64 bytes in each cache line the next three accesses will not generate a D cache miss.

In the next example, the items described above are measured for the non-cacheable area.

In the last two examples, the above two operations are performed with the counter 0 event set to "CPU cycle".

<Files>
	main.c        : main program

<Execution>
	$ make		: compile
	$ make run	: run

The sample program can also be run by entering the following after compilation.
	$ dsedb
	> run main.elf

<Notes>
	none
