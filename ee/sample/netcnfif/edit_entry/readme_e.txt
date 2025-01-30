[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                       Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                       All Rights Reserved


	Common Network Configuration Interface Library
	Sample Program (edit_entry)


<Description of the Sample>

	This sample program is Common Network Configuration Interface 
	Library which edits Your Network Configuration file.

<Files>
	

	Makefile		: Make file
	edit_entry.c		: Main program
	load_module.c		: Load module program
	load_module.h		: Load module program header
	set_configuration.c	: Configuration program
	set_configuration.h	: Configuration program header


<Activate the Program>

	1. Follow the instruction in 
	   /usr/local/sce/ee/sample/netcnfif/add_entry/readme_j.txt 
	   to execute add_entry program.
	   
	2. The program retrieves information on the Hardware setting 
	   and therefore, connect only the hardware to be used.

	  	* This program is assumed to use the hardware, which the 
		  driver is an986.irx to load IOP module. In case the 
		  connection is made by using others, then the program 
		  needs to be modified to load appropriate IOP modules.
 
		* an986.irx is assumed to be under 
		  /usr/local/sce/iop/modules

	3. Execute make command with 
	   /usr/local/sce/ee/sample/netcnfif/edit_entry as follows.

	   > cd /usr/local/sce/ee/sample/netcnfif/edit_entry
	   > make

	4. Activate dsedb.

	   > dsedb

	5. Execute the following command.

	   dsedb S> reset;run ./edit_entry.elf

	6. If the following indication appears, then the edition has 
	   been completed.

	   [edit_entry.c] complete
	   # TLB spad=0 kernel=1:12 default=13:36 extended=37:47
	   *** Unexpected reply - type=BREAKR result=PROGEND
	   dsedb S> 

<Verify the Configuration>

	Follow the procedure below to verify the configuration.

	1. Check if sceNetcnfifSetFNoDecode(sceNetcnfifArg_f_no_decode_on); 
	   is activated in 
	   /usr/local/sce/ee/sample/netcnfif/edit_entry/edit_entry.c.
	   (If this is not activated, the contents cannot be checked as 
	   Your Network Configuration file is encoded)

	2. Execute make command with /usr/local/sce/ee/sample/hdd/shell 
	   as follows.

	   > cd /usr/local/sce/ee/sample/hdd/shell
	   > make

	3. Activate dsecons.

	   > dsecons

	4. Activate dsedb.

	   > dsedb

	5. Execute the following command.

	   dsedb S> reset;run ./main.elf

	6. Move to the device that saves Your Network Configuration file 
	   and verify the contents on the dsecons screen.

   [host1:]/ $ mc0:
   [mc0:]/ $ ls
   d------rwx 0     0     0x0027     0         8 Jan 24  5:36 .
   d------rw- 0     0     0x0026     0         0 Nov 30 16:14 ..
   d------rwx 0     0     0x0027     0         9 Jan 24  6:32 BWNETCNF
   [mc0:]/ $ cd BWNETCNF
   [mc0:]/BWNETCNF $ ls
   d------rwx 0     0     0x0027     0         0 Jan 24  5:36 .
   d------rwx 0     0     0x0027     0         0 Nov 30 16:14 ..
   -------rwx 0     0     0x0017     0     33688 Jan 24  5:36 SYS_NET.ICO
   -------rwx 0     0     0x0017     0       964 Jan 24  5:36 icon.sys
   -------rwx 0     0     0x0017     0       115 Jan 24  6:32 dev000.cnf
   -------rwx 0     0     0x0017     0       108 Jan 24  6:32 BWNETCNF
   -------rwx 0     0     0x0017     0       133 Jan 24  6:32 ifc000.cnf
   -------rwx 0     0     0x0017     0       100 Jan 24  5:36 net000.cnf
   [mc0:]/BWNETCNF $ 

	7. Check if the above directories and files exist.

	8. Check the contents of configuration management file 
	   (BWNETCNF).
	   
	   The contents vary depending on the following definitions set 
	   in /usr/local/sce/ee/sample/netcnfif/edit_entry/edit_entry.c.
	   
	   #define IFC_NEW_USR_NAME "Ethernet Setting"

	   [mc0:]/BWNETCNF $ cat BWNETCNF
	   1,1,ifc000.cnf,Ethernet Setting
	   2,1,dev000.cnf,ethernet-vendor/ethernet-product
	   0,1,net000.cnf,Combination1

	   [mc0:]/BWNETCNF $ 

	9. Verify the contents of Hardware setting (dev000.cnf) and 
	   Internet Service Provider setting (ifc000.cnf).
	   The contents 
	   vary depending on the following definitions set in 
	   /usr/local/sce/ee/sample/netcnfif/edit_entry/edit_entry.c/
	   
	   #define DEV_PATTERN (0)
	   #define IFC_PATTERN (0)

	   - About dev000.cnf

	     < For #define DEV_PATTERN (0):
	       	(Setting for using the network adaptor as hardware)>

	     [mc0:]/BWNETCNF $ cat dev000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type nic
	     vendor "SCE"
	     product "Ethernet (Network Adaptor)"
	     phy_config auto

	     [mc0:]/BWNETCNF $ 

	     < For #define DEV_PATTERN (1):
		(Setting for using other Ethernet adaptor besides the network 
		adaptor as hardware)>

	     [mc0:]/BWNETCNF $ cat dev000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type eth
	     vendor "ethernet-vendor"
	     product "ethernet-product"
	     phy_config auto

	     [mc0:]/BWNETCNF $ 

	     < For #define DEV_PATTERN (2):
		(Setting for using modem/TA as hardware)>

	     [mc0:]/BWNETCNF $ cat dev000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type ppp
	     vendor "modem-vendor"
	     product "modem-product"
	     chat_additional "\"\" ATM0 OK \\c"
	     outside_number "0"
	     outside_delay ",,,"
	     dialing_type tone
	     idle_timeout 600

	     [mc0:]/BWNETCNF $ 

	   - About ifc000.cnf

	     < For #define IFC_PATTERN (0):
		(Setting for using the network adaptor as hardware without 
		using DHCP) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type nic
	     -dhcp
	     address "192.168.0.1"
	     netmask "255.255.255.0"
	     route add -net 0.0.0.0 gw 192.168.0.2 netmask 0.0.0.0
	     nameserver add 192.168.0.3
	     nameserver add 192.168.0.4

	     [mc0:]/BWNETCNF $ 

	     < For #define IFC_PATTERN (1):
		(Setting for using the network adaptor as hardware, and for 
		using DHCP) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type nic
	     dhcp
	     dhcp_host_name "host_name"
	     nameserver add 192.168.0.1
	     nameserver add 192.168.0.2

	     [mc0:]/BWNETCNF $ 

	     < For #define IFC_PATTERN (2):
		(Setting for using other Ethernet adaptor besides the network 
		adaptor as hardware without using DHCP)>

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type eth
	     -dhcp
	     address "192.168.0.1"
	     netmask "255.255.255.0"
	     route add -net 0.0.0.0 gw 192.168.0.2 netmask 0.0.0.0
	     nameserver add 192.168.0.3
	     nameserver add 192.168.0.4

	     [mc0:]/BWNETCNF $ 

	     < For #define IFC_PATTERN (3):
		(Setting for using other Ethernet adaptor besides the network 
		adaptor as hardware, and for using DHCP) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type eth
	     dhcp
	     dhcp_host_name "host_name"
	     nameserver add 192.168.0.1
	     nameserver add 192.168.0.2

	     [mc0:]/BWNETCNF $ 

	     < For #define IFC_PATTERN (4):
		(Setting for using Ethernet adaptor as hardware, for using 
		PPPoE, and for getting DNS server address automatically) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type ppp
	     -dhcp
	     auth_name "userid"
	     auth_key "password"
	     peer_name "*"
	     idle_timeout 0
	     -want.accm_nego
	     -want.prc_nego
	     -want.acc_nego
	     want.dns1_nego
	     want.dns2_nego
	     allow.auth chap/pap
	     pppoe
	     mtu 1454
	     route add -net 0.0.0.0 netmask 0.0.0.0

	     [mc0:]/BWNETCNF $ 

	     < For #define IFC_PATTERN (5):
		(Setting for using Ethernet adaptor as hardware, and for using
		 PPPoE without getting DNS server address) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type ppp
	     -dhcp
	     auth_name "userid"
	     auth_key "password"
	     peer_name "*"
	     idle_timeout 0
	     -want.accm_nego
	     -want.prc_nego
	     -want.acc_nego
	     allow.auth chap/pap
	     pppoe
	     mtu 1454
	     route add -net 0.0.0.0 netmask 0.0.0.0
	     nameserver add 192.168.0.1
	     nameserver add 192.168.0.2

	     [mc0:]/BWNETCNF $ 

	     < For #define IFC_PATTERN (6):
		(Setting for using modem/TA as hardware, and for getting DNS 
		server address automatically)>

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type ppp
	     -dhcp
	     auth_name "userid"
	     auth_key "password"
	     peer_name "*"
	     want.dns1_nego
	     want.dns2_nego
	     allow.auth chap/pap
	     phone_number0 "11-1111-1111"
	     phone_number1 "22-2222-2222"
	     phone_number2 "33-3333-3333"
	     route add -net 0.0.0.0 netmask 0.0.0.0

	     [mc0:]/BWNETCNF $ 

	     < For #define IFC_PATTERN (7):
		(Setting for using modem/TA as hardware without getting DNS 
		server address automatically)>

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type ppp
	     -dhcp
	     auth_name "userid"
	     auth_key "password"
	     peer_name "*"
	     allow.auth chap/pap
	     phone_number0 "11-1111-1111"
	     phone_number1 "22-2222-2222"
	     phone_number2 "33-3333-3333"
	     route add -net 0.0.0.0 netmask 0.0.0.0
	     nameserver add 192.168.0.1
	     nameserver add 192.168.0.2

	     [mc0:]/BWNETCNF $ 
