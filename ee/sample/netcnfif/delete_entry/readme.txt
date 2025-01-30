[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2002 Sony Computer Entertainment Inc.
                                      All Rights Reserved


Common Network Configuration Interface Library
Sample Program (delete_entry)


<Description>

This sample program uses the common network configuration interface library to delete a Your Network Configuration file.


<Files>

	Makefile	:  Makefile
	delete_entry.c	:  Main program
	load_module.c	:  Load module program
	load_module.h	:  Load module program headers


<Usage>

1. Follow the instructions in /usr/local/sce/ee/sample/netcnfif/add_entry/readme_j.txt to execute the add_entry sample program.

2. Execute make in the directory /usr/local/sce/ee/sample/netcnfif/delete_entry.

	   > cd /usr/local/sce/ee/sample/netcnfif/delete_entry
	   > make

3. Start up dsedb.

	   > dsedb

4. Execute the following.

	   dsedb S> reset;run ./delete_entry.elf

5. Deletion will have completed when the following information is displayed.

	   [delete_entry.c] complete
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

5. Confirm the contents on the dsecons screen by moving to the device where the Your Network Configuration file was saved.

	   [host1:]/ $ mc0:
	   [mc0:]/ $ ls
	   d------rwx 0     0     0x0027     0         8 Jan 24  5:36 .
	   d------rw- 0     0     0x0026     0         0 Nov 30 16:14 ..
	   d------rwx 0     0     0x0027     0         9 Jan 24  8:39 BWNETCNF
	   [mc0:]/ $ cd BWNETCNF
	   [mc0:]/BWNETCNF $ ls
	   d------rwx 0     0     0x0027     0         0 Jan 24  5:36 .
	   d------rwx 0     0     0x0027     0         0 Nov 30 16:14 ..
	   -------rwx 0     0     0x0017     0     33688 Jan 24  5:36 SYS_NET.ICO
	   -------rwx 0     0     0x0017     0       964 Jan 24  5:36 icon.sys
	   -------rwx 0     0     0x0017     0         0 Jan 24  8:39 BWNETCNF
	   [mc0:]/BWNETCNF $ 

6. Confirm that the directories and files shown above exist.

7. Confirm the contents of the configuration management file (BWNETCNF).

	   [mc0:]/BWNETCNF $ cat BWNETCNF

	   [mc0:]/BWNETCNF $ 

