[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
                     Copyright (C) 2003 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

                                                        Sample Structure
========================================================================

Sample Directory Structure
------------------------------------------------------------------------
Samples marked with an "(*)" are new in this release.

sce/iop/sample/
|--cdvd
|   |--smp_iop
|   |--stmadpcm
|   |--stmread
|   +--stmspcm
|--hello
|--hdd
|   |--basic
|   +--shell
|--ilink
|--inet
|   |--event
|   |--ftp
|   |--http_get
|   |--load_test
|   |--inetlog
|   |--modemapi
|   |--netstat
|   |--OLD
|   |   |--libnet
|   |   +--setapp
|   |--ping
|   +--tracert
|--iopgcc
|--kernel
|   |--hardtime
|   |--module
|   +--thread
|--libeenet
|   +--ent_cnf
|--sif
|   |--sifcmd
|   +--sifrpc
|--sound
|   |--bypass16
|   |--enadpcm
|   |--ezadpcm
|   |--ezbgm
|   |--ezmidi
|   |--ezsein               (*)
|   |--hdplay
|   |--msbasic
|   |--sebasic
|   |--semidi
|   |--sesqhard
|   |--sfxbasic
|   |--sqdump
|   |--sqhard
|   |--sqsoft
|   |--sqsong
|   +--voice
|--OLD
|   +--spu2
|       |--autodma
|       |--seqplay
|       |--stream
|       +--voice
+--usb
    |--activate
    |--selfunld
    |--usbdesc
    |--usbkeybd
    |--usbloadf
    |--usbmouse
    +--usbspkr


Sample Index
------------------------------------------------------------------------
Sample codes are shown below.

cdvd:
        cdvd/smp_iop    Sample to call CD/DVD-ROM Drive command
                        function on IOP
        cdvd/stmadpcm   Sample to reproduce ADPCM streaming
        cdvd/stmread    Sample to read file using stream functions from
                        CD/DVD drive.
        cdvd/stmspcm    Sample to reproduce straight PCM streaming

hello:
        hello           Sample to display "hello !" 

hdd:
        hdd/basic       Sample to operate HDD files
        hdd/shell       Sample to operate HDD partitions

ilink:
        ilink           Sample for the communication via i.LINK(IEEE1394)
                        Connector

inet:
        inet/event      Sample that retrieves an event occurred by the 
                        connection to the network using the libnet library 
                        or connecting/disconnecting hardware, and displays 
                        the event on the screen
        inet/ftp        Sample for communicating with remote host via 
                        File Transfer Protocol (FTP) using inet library
        inet/http_get   Sample program for obtaining a remote file using 
                        the http protocol in INET library
        inet/load_test  Sample for sending/receiving packets between
                        the client and server using TCP
        inet/inetlog    Sample program for displaying/saving log messages
                        from INET library
        inet/modemapi   sample of API which directly operates a modem
                        from an application
        inet/netstat    Sample program for displaying connection information
                        in INET library
        inet/OLD/libnet Libnet library sample program for transparent 
                        handling of INET library in EE (IOP program)
        inet/OLD/setapp Network setting application samples
        inet/ping       Sample program for confirming whether data has 
                        reached the host using INET library
        inet/tracert    Sample Program that Investigates Network Route 
                        Using inet Library
iopgcc:                 A library to use the coverage test feature of 
                        iop-gcc and a usage sample

kernel:
        kernel/hardtime Sample using hardware timer functions
        kernel/module   Sample to show how to create a resident library
                        module
        kernel/thread   Sample to generate a thread and boot-up, 
                        Operate a thread priority, synchronize threads

libeenet:
        libeenet/ent_cnf
                        Samples that use the network library (libeenet) 
                        run on EE 

sif:
        sif/sifcmd      SIF CMD protocol sample
        sif/sifrpc      SIF RPC protocol sample

sound:
        sound/bypass16  Sample of waveform data streaming using SPU2 
                        bypass process
        sound/enadpcm   Sample of SPU2 waveform data encoding process 
                        module (spucodec) on IOP
        sound/ezadpcm   Sample to play back BGM with ADPCM data streaming 
                        from disk
        sound/ezbgm     Sample to reproduce sound data (WAV) with streaming
                        from the disk
        sound/ezmidi    Sample to create music and sound effects with MIDI
        sound/ezsein    Sample to process Sound Effects with SE-stream
        sound/hdplay    Sample Program for Obtaining Information in Sound 
                        Data (HD) with sdhd and Generating Sound with SPU2
        sound/msbasic   Sample that uses CSL MIDI stream generation 
                        (modmsin) module
        sound/sebasic   SE playback with hardware synthesizer
        sound/semidi    SE/MIDI playback with hardware synthesizer sample
        sound/sesqhard  Sample for SE sequence playback with 
                        hardware synthesizer
        sound/sfxbasic  Sample that uses CSL MIDI stream generation 
                        (modmsin) and CSL sound effects stream generation 
                        (modsein) modules
        sound/sqdump    Sample for the simplified sound data (SQ) access 
                        library
        sound/sqhard    Sample to play MIDI by the hardware synthesizer
        sound/sqsoft    Sample to play MIDI by the software synthesizer
        sound/sqsong    Sample program for playing back SONG via hardware
                        synthesizer
        sound/voice     Sample to play the voice

OLD/spu2:
        (Sample using libspu2)
        spu2/autodma    Sample to reproduce sound by the straight PCM 
                        input using the AutoDMA transfer function of the
                        Interim sound library (libspu2)
        spu2/seqplay    Sample to reproduce MIDI sequence on the IOP
                        using Interim sound library(libspu2, libsnd2)
        spu2/stream     Sample to reproduce voice stream by the SPU2 
                        on IOP using Interim sound library
                        (libspu2)
        spu2/voice      Sample to reproduce SPU2 voice on IOP using
                        Interim sound library (libspu2)

usb:
        usb/activate    Sample program which activates categories 
                        (for USB module auto-loader)
        usb/selfunld    Sample program enabling self-deletion of the USB 
                        device driver
        usb/usbdesc     Sample to dump static descriptor of the USB device
        usb/usbkeybd    Sample driver for USB keyboard.
        usb/usbloadf    Sample which enters a module-loading function 
                        in USB module auto-loader
        usb/usbmouse    Sample presenting how to use USB driver (USBD)
        usb/usbspkr     Sample to perform Isochronous transfer


Preliminary Arrangements for Sample Compilation   
------------------------------------------------
None
