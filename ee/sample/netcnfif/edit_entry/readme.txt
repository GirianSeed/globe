[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2002 Sony Computer Entertainment Inc.
                                      All Rights Reserved


Common Network Configuration Interface Library
Sample Program (edit_entry)


<Description>

This sample program uses the common network configuration interface library to edit a Your Network Configuration file.


<Files>

	Makefile		:  Makefile
	edit_entry.c		:  Main program
	load_module.c		:  Load module program
	load_module.h		:  Load module program headers
	set_configuration.c	:  Configuration program
	set_configuration.h	:  Configuration program headers


<Usage>

1. Follow the instructions in  /usr/local/sce/ee/sample/netcnfif/add_entry/readme_j.txt to execute the add_entry sample program.

2. Since the program obtains information for the hardware settings, it will only connect the hardware that is used.

(*) This sample program assumes that the hardware device will use the an986.irx driver, and loads the appropriate IOP module. If another hardware device is to be used for making the connection, the program must be changed so that the appropriate IOP module is loaded.

(*) an986.irx is assumed to be in /usr/local/sce/iop/modules.

3. Execute make in the directory /usr/local/sce/ee/sample/netcnfif/edit_entry.

	   > cd /usr/local/sce/ee/sample/netcnfif/edit_entry
	   > make

4. Start up dsedb.

	   > dsedb

5. Execute the following.

	   dsedb S> reset;run ./edit_entry.elf

6. Editing has completed when the following information is displayed.

	   [edit_entry.c] complete
	   # TLB spad=0 kernel=1:12 default=13:36 extended=37:47
	   *** Unexpected reply - type=BREAKR result=PROGEND
	   dsedb S> 

<Check Method>

The following procedure can be used to check the contents of the configuration.

1. Confirm that sceNetcnfifSetFNoDecode(sceNetcnfifArg_f_no_decode_on); is executed in /usr/local/sce/ee/sample/netcnfif/edit_entry/edit_entry.c. (If this is not the case, the contents of the Your Network Configuration file cannot be verified because they will be encoded.)

2. Execute make in the directory /usr/local/sce/ee/sample/hdd/shell.

	   > cd /usr/local/sce/ee/sample/hdd/shell
	   > make

3. Start up dsecons.

	   > dsecons

4. Start up dsedb.

	   > dsedb

5. Execute the following.

	   dsedb S> reset;run ./main.elf

6. Confirm the contents on the dsecons screen by moving to the device where the Your Network Configuration file was saved.
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

7. Confirm that the directories and files shown above exist.

8. Confirm the contents of the configuration management file (BWNETCNF).

The contents depend on the following definitions, which are set in /usr/local/sce/ee/sample/netcnfif/edit_entry/edit_entry.c.

	   #define IFC_NEW_USR_NAME "Ethernet Setting"

	   [mc0:]/BWNETCNF $ cat BWNETCNF
	   1,1,ifc000.cnf,Ethernet Setting
	   2,1,dev000.cnf,ethernet-vendor/ethernet-product
	   0,1,net000.cnf,Combination1

	   [mc0:]/BWNETCNF $ 

9. Confirm the contents of the hardware settings (dev000.cnf) and Internet service provider settings (ifc000.cnf).

The contents depend on the following definitions, which are set in /usr/local/sce/ee/sample/netcnfif/edit_entry/edit_entry.c.

	   #define DEV_PATTERN (0)
	   #define IFC_PATTERN (0)

	   - About dev000.cnf

	     <For #define DEV_PATTERN (0):  (The following are settings for using a network adapter as the hardware device)>

	     [mc0:]/BWNETCNF $ cat dev000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type nic
	     vendor "SCE"
	     product "Ethernet (Network Adaptor)"
	     phy_config auto

	     [mc0:]/BWNETCNF $ 

	     <For #define DEV_PATTERN (1):  (The following are settings for using an Ethernet adapter other than a network adapter as the hardware device)>

	     [mc0:]/BWNETCNF $ cat dev000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type eth
	     vendor "ethernet-vendor"
	     product "ethernet-product"
	     phy_config auto

	     [mc0:]/BWNETCNF $ 

	     <For #define DEV_PATTERN (2):  The following are settings for using a modem or TA as the hardware device)>

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

	     <For #define IFC_PATTERN (0):  The following are settings for using a network adapter as the hardware device and not using DHCP)>

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

	     <For #define IFC_PATTERN (1):  The following are settings for using a network adapter as the hardware device and using DHCP)>

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type nic
	     dhcp
	     dhcp_host_name "host_name"
	     nameserver add 192.168.0.1
	     nameserver add 192.168.0.2

	     [mc0:]/BWNETCNF $ 

	     <For #define IFC_PATTERN (2):  The following are settings for using an Ethernet adapter other than a network adapter as the hardware device and not using DHCP)>

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

	     <For #define IFC_PATTERN (3):  The following are settings for using an Ethernet adapter other than a network adapter as the hardware device and using DHCP)>

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type eth
	     dhcp
	     dhcp_host_name "host_name"
	     nameserver add 192.168.0.1
	     nameserver add 192.168.0.2

	     [mc0:]/BWNETCNF $ 

	     <For #define IFC_PATTERN (4):  The following are settings for using an Ethernet adapter as the hardware device, using PPPoe, and for automatically obtaining the DNS server address)>

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

	     <For #define IFC_PATTERN (5):  The following are settings for using an Ethernet adapter as the hardware device, using PPPoe, and not automatically obtaining the DNS server address)>

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

	     <For #define IFC_PATTERN (6):  The following are settings for using a modem or TA as the hardware device and automatically obtaining the DNS server address)>

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

	     <For #define IFC_PATTERN (7):  The following are settings for using a modem or TA as the hardware device and not automatically obtaining the DNS server address)>

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
