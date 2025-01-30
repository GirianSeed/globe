[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2002 Sony Computer Entertainment Inc.
                                      All Rights Reserved


Common Network Configuration Interface Library
Sample Program (set_latest_entry)


<Description>

This sample program uses the common network configuration interface library to edit a list of Your Network Configuration files.


<Files>

	Makefile		:  Makefile
	set_latest_entry.c	:  Main program
	load_module.c		:  Load module program
	load_module.h		:  Load module program headers
	set_configuration.c	:  Configuration program
	set_configuration.h	:  Configuration program headers



<Usage>

1. Insert a memory card (PS2) which does not contain any Your Network Configuration files into memory card slot 1.

2. Execute make in the directory /usr/local/sce/ee/sample/netcnfif/set_latest_entry.

	   > cd /usr/local/sce/ee/sample/netcnfif/set_latest_entry
	   > make

3. Start up dsedb.

	   > dsedb

4. Execute the following.

	   dsedb S> reset;run ./set_latest_entry.elf

5. If #define DEBUG has been defined in /usr/local/sce/ee/sample/netcnfif/set_latest_entry/set_latest_entry.c, the list of Your Network Configuration files will be output as follows.

	   [set_latest_entry.c] get_list result
	   1,1,ifc002.cnf,Modem Setting
	   1,1,ifc001.cnf,PPPoE Setting
	   1,1,ifc000.cnf,Ethernet Setting
	   [set_latest_entry.c] get_list result
	   1,1,ifc000.cnf,Ethernet Setting
	   1,1,ifc001.cnf,PPPoE Setting
	   1,1,ifc002.cnf,Modem Setting
	   [set_latest_entry.c] complete
	   # TLB spad=0 kernel=1:12 default=13:36 extended=37:47
	   *** Unexpected reply - type=BREAKR result=PROGEND
	   dsedb S> 
