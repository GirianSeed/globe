[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2002 Sony Computer Entertainment Inc.
                                      All Rights Reserved


Common Network Configuration Interface Library
Sample Program (delete_all)


<Description>

This sample program uses the common network configuration interface library to delete all Your Network Configuration files.


<Files>

	Makefile	:  Make file
	delete_all.c	:  Main program
	load_module.c	:  Load module program
	load_module.h	:  Load module program headers


<Usage>

1. Follow the instructions in /usr/local/sce/ee/sample/netcnfif/add_entry/readme_j.txt to execute the add_entry sample program..

2. Execute make in the directory /usr/local/sce/ee/sample/netcnfif/delete_all.

	   > cd /usr/local/sce/ee/sample/netcnfif/delete_all
	   > make

3. Start up dsedb.

	   > dsedb

4. Execute the following.

	   dsedb S> reset;run ./delete_all.elf

5. All Your Network Configuration files will have been deleted when the following information is displayed.

	   [delete_all.c] complete
	   # TLB spad=0 kernel=1:12 default=13:36 extended=37:47
	   *** Unexpected reply - type=BREAKR result=PROGEND
	   dsedb S> 

<Check Method>

The following procedure can be used to confirm the configuration contents.

1. Execute make in the directory /usr/local/sce/ee/sample/hdd/shell.

	   > cd /usr/local/sce/ee/sample/hdd/shell
	   > make

2. Start up dsecons.

	   > dsecons

3. Start up dsedb.

	   > dsedb

4. Execute the following.

	   dsedb S> reset;run ./main.elf

5. Confirm the contents on the dsecons screen by moving to the device where the Your Network Configuration files were saved.

	   [host1:]/ $ mc0:
	   [mc0:]/ $ ls
	   d------rwx 0     0     0x0027     0         8 Jan 24  5:36 .
	   d------rw- 0     0     0x0026     0         0 Nov 30 16:14 ..
	   [mc0:]/ $ 

6. Confirm that no Your Network Configuration file directory (BWNETCNF) exists.

