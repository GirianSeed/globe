[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
                     Copyright (C) 2003 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

                                                        Sample Structure
========================================================================

Sample Directory Structure
------------------------------------------------------------------------
Samples marked with an "(*)" are new in this release.

sce/ee/sample/
|--advanced
|  |--anti
|  |   |--aa1-feather
|  |   |--mountain
|  |   |--pcrtc-blend
|  |   |--refmap-448
|  |   |--refmap-4times
|  |   |--refmap-noAA
|  |   |--refmap-onepass-AAline
|  |   |--silhouette
|  |   +--texmap-onepass-AAline
|  |--collision
|  |--dynamics
|  |--furball
|  +--optimize
|      |--fluid
|      +--particle
|--atok    
|  |--basic    (*)
|  +--softkb
|--basic3d
|  |--core
|  |--vu0
|  +--vu1
|--bnnetcnf
|  |--add_entry
|  +--load_entry
|--cdvd
|  |--smp_ee
|  |--fio_ee
|  +--OLD
|      +--mpgst_cd
|--deci2
|  |--mouse
|  +--system
|--device
|  +--file
|--erx
|  |--combine
|  |--freemem
|  |--modlist
|  |--overlay
|  |--sifsreg
|  +--unload
|--graphics
|  |--anti
|  |--bumpmap
|  |--capture
|  |--clip_vu0
|  |--clip_vu1
|  |--clut
|  |--clut_csa
|  |--framework
|  |   +--general
|  |--gp
|  |   |--balls
|  |   |--filter
|  |   |--multiwin
|  |   |--texsort
|  |   |--tutorial
|  |   |--withhig
|  |   +--zsort
|  |--hig
|  |--jointm
|  |   +--intr_key
|  |       |--mkdata
|  |       +--runtime
|  |--mipmap
|  |--point_l
|  |--refmap
|  |--scis_atest
|  |--scis_vu0
|  |--scis_vu1
|  |--spot_l
|  |--tex_swap
|  +--textrans
|      |--4BIT
|      |--8BIT
|      +--bitconv
|--hdd
|  |--async
|  |--basic
|  +--shell
|--ilink
|--inet 
|  |--libinsck
|  |   |--echo_server
|  |   |--echo_server_single
|  |   +--http_get
|  |--libnet
|  |   |--ball_game
|  |   |--echo_server
|  |   |--echo_server_single
|  |   |--http_get
|  |   |--interface
|  |   +--load_test
|  |--OLD
|  |   |--libnet
|  |   +--setapp
|  +--original
|      +--event
|--ipu
|  |--ezcube
|  |--ezcube3
|  +--ezmovie
|--kernel
|  |--cache
|  |--priority
|  |--rotate
|  |--semaphore
|  |--thread
|  +--vsync
|--libeenet
|  |--echo_server_single
|  |--ent_cnf
|  |--http_get
|  |--http_get_nblk
|  +---util
|       |--ifinfo
|       |--netstat
|       +--routeinfo
|--mc
|  |--basic
|  +--icon
|--mc2
|  +--basic
|--mcx
|  +--basic
|--mpeg
|  |--ezmpeg
|  |--ezmpegstr
|  |--mpegstr
|  |   +--OLD
|  +--mpegvu1
|      +--OLD
|--netcnfif
|  |--add_entry
|  |--convauth
|  |--delete_all
|  |--delete_entry
|  |--edit_entry
|  |--load_entry
|  +--set_latest_entry
|--netglue
|  +--libhttp
|      |---abort
|      |--auth
|      |--base64
|      |--blocking
|      |--chunk
|      |--cookie
|      |--mime
|      |--normal
|      |--proxy
|      |--qp 
|      |--redirect
|      +--urlesc
|--pad
|  |--basic
|  |--dual2
|  |--gun
|  +--mtap
|--pad2
|  |--basic
|  +--vib
|--pc
|  +--dcache
|--pkt
|  |--balls
|  |--cube
|  +--mfifo
|--scf
|--sif
|  |--sifcmd
|  +--sifrpc
|--sound
|  |--ezbadpcm
|  |--ezbgm
|  |--ezmidi
|  |--ezsein     (*)
|  |--rblock
|  |--sqsoft
|  +--voicesdr
|--sk
|  |--ctrlsq
|  |--ctrlstr
|  +--playsq
|--OLD
|  +--spu2
|      |--rautodma
|      |--rseqplay
|      |--rstream
|      +--rvoice
|--timer
|  +--fps
|--usb
|  |--libusbkb
|  |--usbkeybd
|  +--usbmouse
|--vu0
|  |--tballs
|  |--tdiff
|  |--tfog
|  |--tmip
|  |--tskin
|  +--tsquare
+--vu1
   |--blow
   |--bspline
   |--deform
   |--hako
   +--iga


Sample Index
------------------------------------------------------------------------
Sample codes are shown below.

advanced:
        advanced/anti/aa1-feather       AA1 (hardware antialiasing function
                                        of GS) sample
        advanced/anti/mountain          Sample for antialiasing by AA1 line
                                        redrawing
        advanced/anti/pcrtc-blend       Antialiasing by PCRTC merge circuit
                                        sample
        advanced/anti/refmap-448        Reflection mapping/specular sample
                                        (double resolution in V direction)
        advanced/anti/refmap-4times     Reflection mapping/specular sample
                                        (antialiasing by sub-pixel shift)
        advanced/anti/refmap-noAA       Reflection mapping/specular sample
        advanced/anti/refmap-onepass-AAline
                                        One path reflection mapping/
                                        specular sample (with AA1 line
                                        antialiasing - single drawing)
        advanced/anti/silhouette        sample program to perform 
                                        antialiasing to object edges via a 
                                        two-dimensional filter
        advanced/anti/texmap-onepass-AAline
                                        One path texture mapping/specular 
                                        Sample (with AA1 line antialiasing
                                        - single drawing)
        advanced/collision              Sample program to judge collision
        advanced/dynamics               Rigid body dynamics simulation 
                                        sample program
        advanced/furball                VU0 micro mode sample/fur ball sample
        advanced/optimize/fluid         Sample showing the program's 
                                        operating-speed variation according
                                        to optimization levels of the EE
                                        core.
        advanced/optimize/particle      Sample showing the program's 
                                        operating-speed variation according
                                        to optimization levels of the EE
                                        core.

atok:
        basic           Sample program that shows basic examples of how 
                        ATOK library (libatok) is used
        softkb          Sample program uses ATOK library (libatok)

basic3d:
        Samples provided for basic procedure comprehension to display
        3D objects
        basic3d/core    Sample written with cpu core instructions only
                        for algorithm comprehension
        basic3d/vu0     Sample in which main process is replaced with 
                        VU0 macrocodes based on core sample
        basic3d/vu1     Sample in which process of core/vu0 is 
                        replaced with VU1 microcodes 

bnnetcnf:
        bnnetcnf/add_entry      Sample program to read the contents of the 
                                network configuration files for the 
                                PlayStation BB Navigator and output 
                                Your Network Configuration files
        bnnetcnf/load_entry     Sample program to read the contents of the 
                                network configuration files for 
                                the PlayStation BB Navigator and establish 
                                a connection based on them

cdvd:
        cdvd/smp_ee             Sample to call the CD/DVD-ROM drive command
                                function
        cdvd/fio_ee             Sample program to access CD/DVD media via
                                the standard I/O
        cdvd/OLD/mpgst_cd       Sample to read the data of MPEG-2 stream
                                from a CD/DVD drive and plays it back
                                Note: As the function of this sample is
                                      incorporated into the mpeg/mpegstr 
                                      sample, it won't be updated any more.

deci2:
        deci2/mouse     Sample to download the program using DECI2
                        protocol and to simulate the mouse movement on
                        the host machine
        deci2/system    Sample to execute System() on the host machine
                        with the DECI2 protocol

device:
        device/file     Sample to manipulate files on the host machine

erx:
        erx/combine     Sample to reconfigure static libraries into
                         ERX modules
        erx/freemem     Sample to display the free space in liberx
                        memory management
        erx/modlist     Sample to display list of modules
        erx/overlay     Sample to change from the overlay environment
                        to ERX
        erx/sifsreg     Sample to display the SIF software registers
                        on the EE side
        erx/unload      Sample to unload modules

graphics:
        graphics/anti           Antialias sample (sort & polygon clip)
        graphics/bumpmap        Bump mapping sample
        graphics/capture        Sample to save contents of the frame
                                buffer to a file on the host machine
        graphics/clip_vu0       Clipping sample (VU0)
        graphics/clip_vu1       Clipping sample (VU1)
        graphics/clut           Sample to display 4-bit/8-bit colored
                                texture
        graphics/clut_csa       Sample to control loading 4-bit texture
                                CLUT to a temporary buffer with csa/cld 
                                field
        graphics/framework/general
                                Sample to realize the integrated environment
                                for graphics with basic concepts and various
                                functions
        graphics/framework/general/micro
                                Instruction samples for creating *.vsm from 
                                *.vcl have been added.
        graphics/gp/balls       Sample of balls
        graphics/gp/filter      Sample of image filtering process
        graphics/gp/multiwin    Sample that operates using HiG GS service 
                                drawing environment
        graphics/gp/texsort     Sample of texture sort
        graphics/gp/tutorial    Tutorial sample program for implementing 
                                Basic Graphics Library (libgp)
        graphics/gp/withhig     Sample of combination with HiG  
        graphics/gp/zsort       Z sort sample

        graphics/hig            The sample program for the HiG 
                                (High Level Graphics) Library
                SAMPLE0         Object display sample using the output 
                                data of es2hig.
                SAMPLE1         Sample to display two objects concurrently
                                using the output data of es2hig.
                SAMPLE2         Sample to switch two objects by the BLOCK 
                                operation function using the output data of 
                                es2hig.
                SAMPLE3         Sample to construct the plug-in block/data
                                Block from the raw data that can use plug-
                                ins with the BLOCK operation function.
                SAMPLE4         Sample to construct the plug-in block/data
                                Block using the BLOCK operation function and 
                                to operate LOD (level of detail) that 
                                enables the model to be switched according 
                                to the camera position.
                SAMPLE5         Sample to display the STRIP data.
                SAMPLE6         Sample for switching microcodes
                SAMPLE7         Sample for using es2raw data (framework 
                                data) with HiG
                SAMPLE8         Sample having modes equivalent to those 
                                of the framework
                SAMPLE9         TIM2 sample
                SAMPLE10        Sample for performing a CPU process and a 
                                DMA transfer via double buffering
                SAMPLE11        Sample for synthesizing two screens 
                                using PCRTC
                SAMPLE12        Sample with memory-resident or transient 
                                textures

                SAMPLE13        Sample to create subwindows (multiwindows) 
                                using GS service.
                SAMPLE14        Drawing sample in texture area using GS 
                                service
                SAMPLE15        Motion blur sample.
                SAMPLE16        Emboss bump-mapping sample 
                SAMPLE17        CLUT bump-mapping sample
                SAMPLE18        Shadow mapping sample
                SAMPLE19        Light mapping sample
                SAMPLE20        Reflection/Refraction sample
                SAMPLE21        Dynamic reflection/refraction sample 
                                using a fisheye lens
                SAMPLE22        Specular light mapping sample
                SAMPLE23        Full-screen anti-aliasing sample
                SAMPLE24        Rendering sample using a fisheye lens 
                                effect
                SAMPLE25        Sample to clone an object
                SAMPLE26        Sample of object clipping
                SAMPLE27        Sample that displays images at slower 
                                frame rate
                SAMPLE28        Sample that uses vertex color
                SAMPLE29        Sample of cloning by using micro-code
                SAMPLE30        Skin deformation sample with VU1
                SAMPLE31        Skin deformation sample with VU0 
                SAMPLE32        Tim2 texture animation sample
        graphics/hig/micro      Instruction samples for creating *.vsm from 
                                *.vcl have been added.
        graphics/jointm/intr_key/mkdata
                                Sample to create key frame matrix animation
                                data by interpolating the axis of rotation
        graphics/jointm/intr_key/runtime
                                Sample to perform key frame matrix 
                                animation at runtime by interpolating the 
                                axis of rotation
        graphics/mipmap         Sample to perform MIPMAP
        graphics/point_l        Sample to perform point lighting
        graphics/refmap         Sample to perform illumination calculation 
                                with specularity and reflection mapping
        graphics/scis_atest     Scissoring sample program using the alpha test
        graphics/scis_vu0       Scissoring sample program (CORE&VU0)
        graphics/scis_vu1       Scissoring sample program (VU1)
        graphics/spot_l         Sample to demonstrate the spotlight
        graphics/tex_swap       Texture swapping sample
        graphics/textrans/4BIT  Sample to transfer the 4 bit-256x256 
                                texture as the 32 bit-128x64 texture
        graphics/textrans/8BIT  Sample to transfer the 8 bit-256x256 
                                texture as the 32 bit-128x128 texture
        graphics/textrans/bitconv
                                Off-line sample to convert the bit order 
                                of the raw image in PSMT4/8 to transferable 
                                bit order in PSMCT32

hdd:
        hdd/async               Sample to access a general-purpose 
                                asynchronous file using multithreads
        hdd/basic               Sample to access HDD files
        hdd/shell               Sample of shell-like command line tool

ilink:
        ilink           Sample for the communication via i.LINK(IEEE1394)
                        connector

inet:
        inet/libinsck/echo_server
                                Sample of echo server operating on 
                                libinsck
        inet/libinsck/echo_server_single
                                Echo server sample of a single thread for 
                                operations in the libinsck library
        inet/libinsck/http_get  Sample to obtain remote files via http 
                                protocol using libinsck
        inet/libnet/ball_game   Sample program for server-client data 
                                exchange via UDP packets in the INET library
        inet/libnet/echo_server Echo server sample for operations in the 
                                libnet library
        inet/libnet/echo_server_single
                                Echo server sample of a single thread for 
                                operations in the libnet library
        inet/libnet/http_get    Sample program for obtaining a remote file 
                                using the http protocol in the INET library
        inet/libnet/interface   Sample program to operate interface events 
                                using the Libnet library
        inet/libnet/load_test   Sample for sending/receiving packets between
                                the client and server using TCP
        inet/OLD/libnet         Libnet library sample program for 
                                transparent handling of the INET library 
                                in EE (EE program)
        inet/OLD/setapp         Network setting application samples
        inet/original/event     Sample that retrieves an event occurred by 
                                the connection to the network using the 
                                libnet library or connecting/disconnecting 
                                hardware, and displays the event on the 
                                screen

ipu:
        ipu/ezcube      Sample to map texture decoded with IPU to a
                        3 dimensional polygon
        ipu/ezcube3     Sample to decode more than one compressed data
        ipu/ezmovie     Sample to simply reproduce moving pictures 
                        with IPU

kernel:
        kernel/cache    Sample for controlling the cache of the EE core
        kernel/priority Sample to enhance understanding of thread priority
        kernel/rotate   Sample program for the thread ready queue rotation
        kernel/semaphore
                        Sample to control thread operations using a VSync 
                        interrupt and a semaphore
        kernel/thread   Sample to draw objects by multi-thread
                        feature
        kernel/vsync    Sample program for the VSyncStart interrupt

libeenet:
        libeenet/echo_server_single
                                A single thread echo server sample program
        libeenet/ent_cnf        Sample used for http_get,http_get2
        libeenet/http_get       Sample program for obtaining remote file 
                                Through HTTP protocol using libeenet library
        libeenet/http_get_nblk   Sample program that uses a non-blocking 
                                socket
        libeenet/util/ifinfo     Sample program that displays interface 
                                information
        libeenet/util/netstat    Sample program that displays connection 
                                information
        libeenet/util/routeinfo  Sample program that displays routing 
                                information

mc:
        mc/basic        Sample to check the basic functions of memory card
                        library
        mc/icon         Tool operating on linux system to generate 3-D icons

mc2:
        mc2/basic       Sample program to access the memory card (PS2) using 
                        the memory card library 2       

mcx:
        mcx/basic       Sample program to check the basic features of 
                        PDA libraries

mpeg:
        mpeg/ezmpeg     Sample to reproduce MPEG-2, including MPEG-1, 
                        stream with IPU
        mpeg/ezmpegstr  Sample minimal program to play back an MPEG2 stream 
                        with sound for PlayStation 2 (PSS)
        mpeg/mpegstr    Sample to reproduce MPEG-2 file with sound for
                        PlayStation (PSS file) from the hard disk 
                        of the host and CD/DVD. (Sample using libsdr)
        mpeg/mpegstr/OLD
                        Sample to reproduce MPEG-2 file with sound for 
                        PlayStation (PSS file) from the hard disk 
                        of the host and CD/DVD. (Sample using librspu2)
        mpeg/mpegvu1    Sample to perform the color conversion using the 
                        VU1 instead of the IPU. (Sample using libsdr)
        mpeg/mpegvu1/OLD
                        Sample to perform the color conversion using the 
                        VU1 instead of the IPU. (Sample using librspu2)

netcnfif:
        Samples that uses common network configuration interface library
        netcnfif/add_entry      Sample to add Your Network Configuration file
        netcnfif/convauth       Sample uses sceNetcnfifConvAuthname()
        netcnfif/delete_all     Sample to delete Your Network Configuration 
                                file completely
        netcnfif/delete_entry   Sample to delete all of Your Network 
                                Configuration file
        netcnfif/edit_entry     Sample to edit Your Network Configuration 
                                file
        netcnfif/load_entry     Sample to retrieve the contents of Your 
                                Network Configuration file
        netcnfif/set_latest_entry
                                Sample to edit the list of Your Network 
                                Configuration file

netglue:
        netglue/libhttp/abort     Sample program that establishes connections 
                                  and suspends transactions via sceHTTPAbort()
        netglue/libhttp/auth      Sample of authentication processing using 
                                  libhttp
        netglue/libhttp/base64    Sample of BASE64 processing
        netglue/libhttp/blocking  Blocking version of http_test using libhttp
        netglue/libhttp/chunk     Sample of chunk transfer processing using 
                                  libhttp
        netglue/libhttp/cookie    Sample of cookie processing using libhttp
        netglue/libhttp/mime      Gets a specified URI and performs 
                                  MIME processing using libhttp
        netglue/libhttp/normal    Performs GET, HEAD, or POST for a 
                                  specified URI using libhttp
        netglue/libhttp/proxy     Gets a specified URI via proxy using libhttp
        netglue/libhttp/qp        Sample of quoted-printable processing 
        netglue/libhttp/redirect  Sample of redirection processing using 
                                  libhttp
        netglue/libhttp/urlesc    Sample of URL escape and unescape 
                                  processing 

pad:
        pad/basic       Sample to display the controller information
        pad/dual2       The sample program for displaying the information
                        on pressure sensitivity of analog controller 
                        (DUALSHOCK 2)
        pad/gun         Sample to obtain the coordinate information on gun 
                        controller and display on the screen
        pad/mtap        Sample to operate the controller via multitap.

pad2:
        pad2/basic      Sample program for displaying information on the
                        controller
        pad2/vib        Sample program for controlling the actuator
                        using vibration libirary
pc:
        pc/dcache       Sample to count D cache miss by performance
                        counter 

pkt:
        pkt/balls       Sample to draw sprite
        pkt/cube        Sample to draw polygon models
        pkt/mfifo       Sample to draw the object using MFIFO

scf:
        scf             Sample to obtain the system setting information on
                        PlayStation 2

sif:
        sif/sifcmd      SIF CMD protocol sample
        sif/sifrpc      SIF RPC protocol sample 

sound:
        sound/ezadpcm   Sample to play back BGM with ADPCM data streaming 
                        from the disk
        sound/ezbgm     Sample to reproduce the sound data (WAV) with 
                        streaming from the disk
        sound/ezmidi    Sample to create music and sound effects with MIDI
        sound/ezsein    Sample to process Sound Effects with SE-stream
        sound/rblock    Sample to execute the I/O block transfer 
                        (formerly the AutoDMA transfer) 
        sound/sqsoft    Sample to play the MIDI by the software synthesizer
        sound/voicesdr  Sample which plays back simple sound by using the 
                        low level sound  library (EE:libsdr)

sk:
        sk/ctrlsq       Playback of various types of music data with 
                        standard kit/sound system
        sk/ctrlstr      Sample program to play back ADPCM stream data 
                        with standard Kit/sound system 
        sk/playsq       Music score data (SQ data/MIDI) playback with 
                        standard kit/sound system

OLD/spu2:
        (Sample using librspu2)
        spu2/rautodma   Sample to produce sound with straight PCM input 
                        by AutoDMA transfer feature
        spu2/rseqplay   Sample to reproduce MIDI sequence
        spu2/rstream    Sample to reproduce voice stream
        spu2/rvoice     Sample to produce voice

timer:
        timer/pfs       Sample to measure the VSYNC/HSYNC frequencies of 
                        each screen mode using the timer library
usb:
        usb/libusbkb    Sample of USB keyboard library (libusbkb.a)
        usb/usbkeybd    Sample to get data of the USB keyboard from IOP 
                        to EE
        usb/usbmouse    Sample to get data of the USB mouse driver from 
                        IOP to EE

vu0:
        Samples which perform geometric processing via VU0
        vu0/tballs      Sample in which three-dimensional balls move 
                        around
        vu0/tdiff       Sample to clip polygons
        vu0/tfog        Sample to perform point lighting and object 
                        clipping  
        vu0/tmip        Sample to perform linear interpolation of MIME 
                        and MIPMODEL
        vu0/tskin       Sample to display multiple models by reproducing 
                        1 model with a matrix. 
        vu0/tsquare     Sample of VU0 macrocode which uses an in-line 
                        assembler

vu1:
        Samples which perform geometric processing via VU1 
        The dvpasm format is used for the description of the microcode. 
        vu1/blow        Sample to draw particles
        vu1/bspline     Sample to draw VU1 b-spline curved surface
        vu1/deform      Sample to perform interpolation between two 
                        geometric figures
        vu1/hako        Sample to perform Vu1 transparency perspective 
                        conversion
        vu1/iga         Sample to perform Vu1 transparency perspective 
                        conversion and lighting calculation

