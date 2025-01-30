[SCEI CONFIDENTIAL DOCUMENT]
DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
                  Copyright (C) 1999 by Sony Computer Entertainment Inc.
                                                     All Rights Reserved

                                                        Sample Structure
========================================================================

Sample Directory Structure
------------------------------------------------------------------------
sce/ee/sample/
|--basic3d
|  |--core
|  |--vu0
|  +--vu1
|--deci2
|  +--system
|--device
|  |--file
|  +--pad
|--graphics
|  |--capture
|  |--clip_vu0
|  |--clip_vu1
|  |--clut
|  |--clut_csa
|  |--jointm
|  |  +--intr_key
|  |      |--mkdata
|  |      +--runtime
|  |--mipmap
|  |--refmap	
|  +--tex_swap
|--ipu
|  |--ezcube
|  |--ezcube3
|  +--ezmovie
|--kernel
|  +--thread
|--mpeg
|  +--ezmpeg
|--pc
|  +--dcache
|--pkt
|  |--balls
|  |--cube
|  +--mfifo
|--sif
|  |--sifcmd
|  +--sifrpc
|--spu2
|  |--rautodma
|  |--rseqplay
|  |--rstream
|  +--rvoice
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
Sample codes are shown as below.

basic3d:
	Samples provided for basic procedure comprehension to display
	3D objects
	basic3d/core	Sample written with cpu core instructions only
	                for algorithm comprehension
	basic3d/vu0	Sample in which main process is replaced with 
			VU0 macrocodes based on core sample
	basic3d/vu1	Sample in which process of core/vu0 is 
			replaced with VU1 microcodes 

deci2:
	deci2/system	Sample to execute System() on the host machine
			with the Deci2 protocol

device:
        device/file     Sample to manipulate files on the host machine
        device/pad      Sample to read the controller

graphics:
	graphics/capture	Sample to save contents of the frame
				buffer to a file on the host machine
	graphics/clip_vu0	Clipping sample (VU0)
	graphics/clip_vu1	Clipping sample (VU1)
	graphics/clut		Sample to display 4-bit/8-bit colored
				texture
	graphics/clut_csa	Sample to controll loading 4-bit texture
				CLUT to a temporary buffer with csa/cld 
				field
	graphics/jointm/intr_key/mkdata
				Sample to create key frame matrix animation
				data by interpolating the axis of rotation
	graphics/jointm/intr_key/runtime
				Sample to perform key frame matrix animation
				at runtime by interpolating the axis of 
				rotation
	graphics/mipmap		Sample to perform MIPMAP
	graphics/refmap		Sample to perform illumination calculation with
				specularity and reflection mapping
	graphics/tex_swap	Texture swapping sample

ipu:
        ipu/ezcube	Sample to map texture decoded with IPU to a
			3 dimensional polygon
	ipu/ezcube3	Sample to decode more than one compressed data
	ipu/ezmovie     Sample to simply reproduce moving pictures 
			with IPU

kernel:
	kernel/thread	Sample to draw objects by multi-thread
			feature

mpeg:
	mpeg/ezmpeg	Sample to reproduce MPEG2, including MPEG1, stream
			with IPU

pc:
	pc/dcache	Sample to count D chahe miss by performance
			counter	

pkt:
        pkt/balls       Sample to draw Sprite
        pkt/cube        Sample to draw polygon models
	pkt/mfifo	Sample to draw the object using MFIFO

sif:
	sif/sifcmd	SIF CMD protocol sample
	sif/sifrpc	SIF RPC protocol sample	

spu2:
	spu2/rautodma	Sample to produce sound with straight PCM input 
			by AutoDMA transfer feature
	spu2/rseqplay	Sample to reproduce MIDI sequence
	spu2/rstream	Sample to reproduce voice stream
	spu2/rvoice	Sample to produce voice

vu0:
        Samples which perform geometric processing via VU0
	vu0/tballs 	Sample in which three-dimensional balls move 
			around
	vu0/tdiff  	Sample to clip polygons
	vu0/tfog   	Sample to perform point lighting and object 
			clipping  
	vu0/tmip   	Sample to perform linear interpolation of MIMe 
			and MIPMODEL
	vu0/tskin  	Sample to display multiple models by reproducing 
			1 model with a matrix. 
	vu0/tsquare	Sample of VU0 macrocode which uses an in-line 
			assembler


vu1:
        Samples which perform geometric processing via VU1 
        The dvpasm format is used for the description of the microcode. 
	vu1/blow	Sample to draw particles
	vu1/bspline	Sample to draw VU1 b-spline curved surface
	vu1/deform	Sample to perform interpolation between two 
			geometric figures
	vu1/hako	Sample to perform transparency perspective 
			conversion
	vu1/iga		Sample to perform transparency perspective 
			conversion and lighting calculation


