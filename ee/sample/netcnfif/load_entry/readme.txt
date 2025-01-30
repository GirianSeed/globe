[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2002 Sony Computer Entertainment Inc.
                                      All Rights Reserved


Common Network Configuration Interface Library
Sample Program (load_entry)


<Description>

This sample program uses the common network configuration interface library to get the contents of a Your Network Configuration file. The program also performs a connection test using that configuration. (The connection test uses the Libnet library to connect, get an address, then disconnect.)


<Files>

	Makefile	:  Makefile
	load_entry.c	:  Main program
	load_module.c	:  Load module program
	load_module.h	:  Load module program headers


<Usage>

1. Use a program such as /usr/local/sce/ee/sample/netcnfif/add_entry to create proper hardware settings and Internet service provider settings on a PS2 memory card, then register them in combination 1 (Combination1) on the memory card. Insert the memory card in slot 1.

(*) This sample program assumes that an "SCE/Ethernet (Network Adapter)" will be used as the hardware device, and loads the appropriate IOP module. If another hardware device is to be used for making the connection, the program must be changed so that the appropriate IOP module is loaded.

(*) When an encoded Your Network Configuration File is to be used for each PlayStation 2, comment out sceNetcnfifSetFNoDecode(sceNetcnfifArg_f_no_decode_on);.

2. Execute make in the directory /usr/local/sce/ee/sample/netcnfif/load_entry.

	   > cd /usr/local/sce/ee/sample/netcnfif/load_entry
	   > make

3. Start up dsedb.

	   > dsedb

4. Execute the following.

	   dsedb S> reset;run ./load_entry.elf

5. If #define DEBUG has been defined in /usr/local/sce/ee/sample/netcnfif/load_entry/load_entry.c, the contents of the Your Network Configuration file will be output as follows.

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
