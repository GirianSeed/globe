[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                       Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                       All Rights Reserved


	Common Network Configuration Interface Library
	Sample Program (load_entry)


<Description of the Sample>

	This sample program is Common Network Configuration Interface 
	Library which retrieves the contents of Your Network 
	Configuration file.
	This program then uses the retrieved contents of Your Network 
	Configuration file and test a connection (uses Libnet library to 
	connect, gets the address, and then disconnects).

<Files>

	Makefile	: Make file
	load_entry.c	: Main program
	load_module.c	: Load module program
	load_module.h	: Load module program header


< Activate the Program >

	1. Use /usr/local/sce/ee/sample/netcnfif/add_entry to configure 
	   the Hardware and Internet Service Provider settings to connect 
	   to the memory card (8MB)(for PlayStation 2) correctly, and 
	   register the settings in the Combination 1. Insert the memory 
	   card (8MB)(for PlayStation 2) into MEMORY CARD slot 1.

	   * This program is assumed to use the hardware, "SCE/Ethernet 
	     (Network Adaptor)" to load IOP module. In case the connection 
	     is made by using others, then the program needs to be modified 
	     to load appropriate IOP modules.

	   * If you wish to use an encoded Your Network Configuration file 
	     for each "PlayStation 2," then sceNetcnfifSetFNoDecode
	     (sceNetcnfifArg_f_no_decode_on); must be commented out.

	2. Execute make command with
	   /usr/local/sce/ee/sample/netcnfif/load_entry as follows.

	   > cd /usr/local/sce/ee/sample/netcnfif/load_entry
	   > make

	3. Activate dsedb.

	   > dsedb

	4. Execute the following command.

	   dsedb S> reset;run ./load_entry.elf

	5. If #define DEBUG is defined in 
	   /usr/local/sce/ee/sample/netcnfif/load_entry/load_entry.c 
	   then the contents of Your Network Configuration file will be
	   output as follows.
	   
	   -----------------------
	   attach_ifc       : "ifc000.cnf"
	   attach_dev       : "dev000.cnf"
	   dhcp_host_name   : "(null)"
	   address          : "192.168.0.1"
	   netmask          : "255.255.255.0"
	   gateway          : "192.168.0.2"
	   dns1_address     : "192.168.0.3"
	   dns2_address     : "192.168.0.4"
	   phone_numbers1   : "(null)"
	   phone_numbers2   : "(null)"
	   phone_numbers3   : "(null)"
	   auth_name        : "(null)"
	   auth_key         : "(null)"
	   peer_name        : "(null)"
	   vendor           : "SCE"
	   product          : "Ethernet (Network Adaptor)"
	   chat_additional  : "(null)"
	   outside_number   : "(null)"
	   outside_delay    : "(null)"
	   ifc_type         : "-1"
	   mtu              : "-1"
	   ifc_idle_timeout : "-1"
	   dev_type         : "3"
	   phy_config       : "1"
	   dialing_type     : "-1"
	   dev_idle_timeout : "-1"
	   dhcp             : "0"
	   dns1_nego        : "255"
	   dns2_nego        : "255"
	   f_auth           : "0"
	   auth             : "0"
	   pppoe            : "255"
	   prc_nego         : "255"
	   acc_nego         : "255"
	   accm_nego        : "255"
	   -----------------------
	   address: "192.168.0.1"
	   [load_entry.c] complete
	   # TLB spad=0 kernel=1:12 default=13:36 extended=37:47
	   *** Unexpected reply - type=BREAKR result=PROGEND
	   dsedb S> 
