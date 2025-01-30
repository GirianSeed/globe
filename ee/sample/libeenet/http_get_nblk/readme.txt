[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
        Copyright (C) 2002 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Getting a Remote File With the HTTP Protocol Using libeenet (Non-blocking Version)


<Description>
        This sample program uses libeenet to connect to a remote http port and download a file. The socket is handled in non-blocking mode.

[ Note ]
        ent_eth.irx is required when using USB Ethernet adapter in 
        this sample program.
        Since the use of ent_eth.irx in titles is prohibited, it is not 
        included under sce/iop/modules in the release package. 
        Please download it from the developer support website.

<File>
        http.c				:  Main program

<Execution Method>
        $ make				:  Compile
        $ dsedb -r run http.elf [saddr]	:  Execute

        saddr is a WWW server address (such as www.scei.co.jp).
	When this program is executed, the file http://<saddr>/index.html is downloaded using HTTP and saved locally.

<Controller Operation>
	None

<Comments>
        This program assumes the connection environment is DHCP (USB Ethernet), DHCP (Network adaptor), PPPoE (USB Ethernet), PPPoE (Network adaptor) or PPP (modem).
To change the connection environment, switch the following #define macros which appear in the beginning of this program.

        #define HDD_ETHERNET                : DHCP (Network adaptor)
        #define HDD_ETHERNET_WITH_PPPOE     : PPPoE (Network adaptor)
        #define USB_ETHERNET                : DHCP (USB Ethernet)
        #define USB_ETHERNET_WITH_PPPOE     : PPPoE (USB Ethernet)
        #define MODEM                       : PPP (modem)

	To use PPP (modem), change the following #define statements, which are NULL strings in iopmodules.h, to match the PPP connection environment and modem that are used.

	#define USE_CONF_NAME		""	: Configuration name within net.db
	#define IOP_MOD_MODEMDRV	""	: Module path name
	#define MODEMDRV_ARG		""	: Argument

	To use a USB modem, the USBD module must be loaded in addition to the modules specified above. If additional modules must be loaded because of the type of modem to be used, modify http.c appropriately.

	To change the IP address, to change the PPPoE user name, or to use PPP, change the configuration file indicated by the connection environment configuration name "Combination?" (where ? is a number) as appropriate. The correspondence between configuration name and configuration file for each connection environment is shown below.

        DHCP (Network adaptor)       Combination6         net005.cnf
        PPPoE (Network adaptor)      Combination7         net006.cnf
        DHCP (USB Ethernet)          Combination4         net003.cnf
        PPPoE (USB Ethernet)         Combination5         net004.cnf
        PPP (modem)                  Combination2         net001.cnf

	In addition, the program accesses an unencoded text configuration file located under /usr/local/sce/conf/net. Note that this is done because this program is used as a development sample and an individually encoded configuration file must be used by an official title.
	For information about how to use individually encoded files, refer to the sample configuration application.

	Since only one interface is specified per configuration file in the current connection environment configuration files, the program also assumes that multiple devices will not be used simultaneously.
	This program runs under the condition that exactly one target device is connected.
	Note that multiple connections to the same device are also not considered.

<Processing Summary>
	1. Initialize libeenet, load various modules.
	2. Connect to the remote port.
	3. Send GET message.
	4. Receive data.
	5. Unload various modules.
	6. Terminate libeenet.

