[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 2.5
      Copyright (C) 2000 Sony Computer Entertainment Inc.
                                      All Rights Reserved

libnet library sample (EE-side program) that transparently handles the inet library from the EE 

<Description>
This is an EE-side program of the libnet library that transparently handles inet from the EE. Using the SIF system, libnet can transparently call functions from the EE on the IOP. Consequently, it is not necessary to write a separate IOP program which handles inet for network-compatible applications that run on the EE. Generally, inet can be handled from the EE.

<Files>
        libnet.c        : Main program
        libnet.h        : include file

<Run method>
        $ make          : Compile

The library that is created, libnet.a, is linked to the EE program that uses libnet.


<Functions using libnet>

Functions that libnet provides are basically the same as the functions provided by inet. Refer to the description of inet for details of each function. 

More specifically, a difference is that the following
have been added to the arguments of each function:
	sceSifMClientData *cd; 
	u_int *net_buf;

Here, cd is used as client data for the MSIFRPC system (multithread compatible SIFRPC) and the buffer indicated by net_buf is used as a send and receive buffer for SIF. This buffer must be 64 bytes aligned.

For details refer to each sample that uses libnet.h or libnet.

<IOP modules loaded when using libnet>
The following modules must be loaded in order, when using libnet.

1. Modules required for inet

<When using USB ether>
 inet.irx
 netcnf.irx 
 inetctl.irx
 an986.irx
 usbd.irx

<When using PPP>
 inet.irx
 netcnf.irx
 inetctl.irx
 ppp.irx
 usbd.irx
 usbmload.irx


2. Modules required for MSIFRPC
 msifrpc.irx

3. Modules required for libnet
 libnet.irx

<Notes>
Internally libnet uses libmrpc (multi-thread compatible SIFRPC). Consequently, function calls can be issued from multiple threads on the EE at arbitrary times.
