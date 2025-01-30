[SCEI CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0     
Copyright (C) 1999-2002 Sony Computer Entertainment Inc.                                       All Rights Reserved

Basic sample program basic3d: VU1 version
=========================================

<Description>
The VU1 version of basic3d presents examples of how perspective transformation and packet generation processes can be implemented in high-speed VU1 micro mode. The required VUMem1 double-buffering and other operations are shown to provide the foundations for optimized geometry processing.

<Files>	main.c		main program	mathfunc.c	functions for performing matrix 			operations such as perspective 			transformation (CPUcore version)	mathfunc.h	header file	cube.dsm	object data (cube) (VU1 packet format)	torus.dsm	object data (torus data: split)			(VU1 packet format)	torus1.dsm	object data (torus data: split)			(VU1 packet format)	flower.dsm	texture data	basic.vsm	file of instructions executed in 			VU1 micro mode	vu1dump.c	functions for performing VU1 			local memory dumps<Compilation>	% make After compilation, the following method can also be used:	% dsedb	> run main.elf<Activation>	% make run<Specification>	Displays a cube with textured triangle strips.	Displays a torus with textured triangles.	(switch using the SELECT button)	24-bit Z-buffer	Three light sources	32-bit textures<Controller operations>	up/down arrow (direction keys): rotate object around x-axis	left/right arrow	:rotate object around y-axis	L1/L2 buttons   	:rotate object around z-axis	square/circle buttons   :rotate camera around y-axis in world coordinate system	triangle/X buttons   	:rotate camera around x-axis in world coordinate system	R1/R2 buttons   	:move camera forward/back		SELECT button  		:select object<Summary of operations>1. Load textures2. Open pad3. Set matrix and light-source matrix in VU1 packet4. DMA kick for VU15. Return to step 2The VU1 micro-program performs the following operations.1. Set up rotation x perspective transformation matrix2. Load various parameters (matrices, etc.) in VU1 registers3. Load normal vector, two coordinates to interpolate, ST value, color4. Interpolate coordinates, perform perspective transformation, perform light-source calculations, and store results in VU1Mem5. Return to 3. Repeat for each vertex.6. Kick stored data (Gif Packet) to the GSDouble-buffering is performed in VU1Mem. Data flow----------------------------------------------------------<Initialization>main RAM                          GS RAM(4M)  texture data ----------------> set up texture data				 set up object data				 set up perspective transformation matrices, etc.<Execution>main RAM                            VUMem1(16K)  object data ----------------> set up packet data for transfer to the GS (various matrix and DMA transfer (geometry calculations, double buffering) geometry data)                 (primitive data (STQ,RGBA,XYZF))
                                                    |
                                                    |
GS (4M) <-------------------------------------------+
 frame & Z-buffer writing                 Kick data to GIF
 (rendering)


Description of program (detailed)
----------------------------------------------------------

<Variables, structures, functions> (excluding mathfunc.c functions)

My_dma_start            : First element of packet group to be sent to VU1
My_dma_next             : First element of second packet to be sent to VU1
My_matrix               : local_screen matrix in packet
My_rot_trans_matrix     : local_world matrix in packet
My_light_matrix         : local_light matrix in packet
My_cube_start           : first element in cube packet data
My_torus_start          : first element in torus packet data
My_texture1             : first element in texture data

TexEnv          : structure used to generate packets to be transferred to the sceGsTexEnv structure

camera_p        : camera position
camera_zd       : camera viewing direction vector
camera_yd       : camera down vector
camera_rot      : camera rotation vector

light0          : light 0
light1          : light 1
light2          : light 2
color0          : color of light 0
color1          : color of light 1
color2          : color of light 2

ambient         : ambient light

obj_trans       : object translation vector
obj_rot         : object rotation vector

local_world     : matrix to transform object Local 
 	            coordinates to World coordinate system
world_view      : matrix to transform World coordinate 
                  system to View coordinate system
view_screen     : matrix to transform View coordinate 
                  system to Screen coordinate system
local_screen    : matrix to transform Local coordinates 
                  to Screen coordinates (product of the 
                  above three matrices)

normal_light    : matrix to get an inner product of normal vector and light 				direction vector in the World coordinate system (the light 				direction vector is set)
local_light     : matrix to multiply the local_world matrix by the normal_light 			matrix to get the inner product of the vertex normal 
		  	vector in the Local coordinate system and the light direction 			vector
light_color     : matrix resulting from summation of 
			multiplying light colors (3 colors + 
		      ambient light) by the inner product 
		      vectors of the local_light matrix and 
		      the vertex normal vector
local_color     : product of local_light and light_color
work            : temporary workspace

sceGsDBuff      db       : structure for implementing 
		               double-buffering
scePadData      pad      : structure for reading pad data
sceGsLoadImage  gs_limage: structure for transferring 
			         textures to the GS local memory
TexEnv          texenv   : structure for generating packets 
			         used for transferring texture 
			         data

void SetVu1PacketMatrix(void)
                         : function for set matrix in 
			         packets to be sent to VU1

<variables in function main>

fd              : file descriptor for receiving return 
		      value from opening pad
frame           : switch for flipping double buffers
delta           : variable for setting up rotation angle
obj_switch      : toggle switch for switching objects
toggle          : variable for toggling the reading of 
                  the SELECT button

<Variables/functions supported by mathfunc.c>

sceSamp0IVECTOR[4]      : array of type int aligned at 
	  	              128-bit boundaries (4 elements)
sceSamp0FVECTOR[4]      : array of type float aligned at 
                          128-bit boundaries (4 elements)
sceSamp0FMATRIX[4][4]   : array of type float aligned at 
                          128-bit boundaries (4x4 elements)

Please refer to mathfunc.c for detailed description of the functions. Almost all of the functions implemented in mathfunc.c use VU0 macros and are provided via libvu0.

The VU1 version uses the sceSamp0*** functions, which execute matrix calculations using the Core, but as with the VU0 version, operations can be speeded up by replacing functions.

Note that the matrix formats differ between the Core version and the VU0/VU1 versions. In the Core version, standard matrices are used, but the VU0/VU1 versions use transpose matrices to take advantage of VU operations.

Description of program (initialization)
----------------------------------------------------------

* Read object data

In the VU1 version, object data is linked and set up in memory at compile time. Files in the *.dsm format are in a packet format that can be sent directly to VU1, so the My_dma_start pointer can be used.

* Initialize devices
    sceGsResetPath();// data device initialization (VIF1,VU1,GIF)

All devices to be used are initialized at the start of the program. Use of uninitialized devices can result in unexpected behavior, so always perform initialization.

* Set up drawing environment
    sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
        // initialize the GS
    sceGsSetDefDBuff(&db, SCE_GS_PSMCT32, SCREEN_WIDTH, 
             SCREEN_HEIGHT, SCE_GS_ZGEQUAL, SCE_GS_PSMZ24, SCE_GS_CLEAR);
        // set up double-buffering
    *(u_long *)&db.clear0.rgbaq = SCE_GS_SET_RGBAQ(0x10, 0x10, 0x10,
                                                0x80, 0x3f800000);
    *(u_long *)&db.clear1.rgbaq = SCE_GS_SET_RGBAQ(0x10, 0x10, 0x10,
                                                0x80, 0x3f800000);

The GS drawing environment is set up after initialization of devices is completed. Displaying to the screen will not be possible unless the environment is set up. This is also required for controller settings which are synchronized via VSync, so always remember to set up the environment right after devices have been initialized.

* Transfer texture images
    sceGsSetDefLoadImage(&pTex,13340,IMAGE_SIZE / 64,SCE_GS_PSMCT32,
                        0, 0,IMAGE_SIZE, IMAGE_SIZE);
                        // set up LoadImage information
    FlushCache(0);
    sceGsExecLoadImage(&pTex, (u_long128 *)My_texture1);
                        //execute LoadImage
    sceGsSyncPath(0, 0);// wait for completion of data transfer

LoadImage data is set up to transfer texture images to the local memory of the GS. Then, sceGsExecLoadImage() is executed to transfer the specified image to the local memory of the GS. This function uses DMA internally, so FlushCache(0) is performed to maintain consistency between the D-cache and main memory. Finally, sceGsSyncPath() is called to wait for completion of the data transfer.

* Generate packets for setting up texture environment
    FlushCache(0);
    // --- set texture env 1 ---
    sceGsSetDefTexEnv(&texenv.gs_tex, 0, 13340, 
                        IMAGE_SIZE / 64, SCE_GS_PSMCT32, 
                          8, 8, 0, 0, 0, 0, 1);
    SCE_GIF_CLEAR_TAG(&texenv.giftag); 
    texenv.giftag.NLOOP = 9;
    texenv.giftag.EOP = 1;
    texenv.giftag.NREG = 1;
    texenv.giftag.REGS0 = 0xe;
    sceGsSetDefAlphaEnv(&texenv.gs_alpha, 0);
    *(u_long *) &texenv.gs_alpha.alpha1 
                = SCE_GS_SET_ALPHA(0, 2, 0, 1, 0);
    *(u_long *) &texenv.gs_test 
                =  SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 3, 0x3);
    texenv.gs_test1addr = (long)SCE_GS_TEST_1;
    *(u_long*)&texenv.gs_tex.clamp1 
                = SCE_GS_SET_CLAMP(0, 0, 0, 0, 0, 0); 

sceGsSetDefTexEnv() is used to generate packets for setting up the general-purpose GS registers TEX0_1, TEX0_1, and CLAMP_1.

* Transfer packets for setting up texture environment
    FlushCache(0);
    sceGsPutDrawEnv(&texenv.giftag);  // transfer texture settings data
    sceGsSyncPath(0, 0);

Packets generated for setting up the texture environment are transferred using the sceGsPutDrawEnv() function.

* Initialize controller
    fd = open("pad: 0", 0);

Initialization is performed in order to use the 1P controller. Subsequently, data is read into the buffer specified by read(). Controller data is updated at each Vsync.


Program description (main loop)
----------------------------------------------------------
The following is a description of operations performed in the main loop, including acquisition of controller information, matrix generation, and packet generation (coordinate transformations, perspective transformations, UV->STQ transformations, and light-source calculations).

* Generate View-Screen matrix
    sceSamp0ViewScreenMatrix(view_screen, 512.0f, 1.0f, 0.47f,
                2048.0f, 2048.0f,1.0f, 16777215.0f,1.0f,65536.0f);

sceSamp0ViewScreenMatrix() is used to determine a View-Screen matrix. The resulting View-Screen matrix assumes a distance between the view point and the projection screen of 512, a screen aspect ratio of 1:0.47, the center of the screen at (2048, 2048) in the GdPrimitive coordinate system, a Z-buffer value of 1.0-16777215.0, and a Z value of 1.0-65536.0.

* Get controller information
    // --- read pad ---
    read(fd, &pad, sizeof(pad));

    // --- rotate object & change view point ---
    if(pad.button & SCE_PADLdown){ 
                obj_rot[0] += deltta;
                if(obj_rot[0]>PI) obj_rot[0] -= 2.0f*PI; }
    if(pad.button & SCE_PADLup){ 
                obj_rot[0] -= delta;
                if(obj_rot[0]<-PI) obj_rot[0] += 2.0f*PI; }
    if(pad.button & SCE_PADLright){ 
                obj_rot[1] += delta;
                if(obj_rot[1]>PI) obj_rot[1] -= 2.0f*PI; }
    if(pad.button & SCE_PADLleft){ 
                obj_rot[1] -= delta;
                if(obj_rot[1]<-PI) obj_rot[1] += 2.0f*PI; }
    if(pad.button & SCE_PADL1){
                obj_rot[2] += delta;
                if(obj_rot[2]>PI) obj_rot[2] -= 2.0f*PI; }
    if(pad.button & SCE_PADL2){
                obj_rot[2] -= delta;
                if(obj_rot[2]<-PI) obj_rot[2] += 2.0f*PI; }
    if(pad.button & SCE_PADRdown){
                camera_rot[0] += delta;
                if(camera_rot[0]>PI*0.4) camera_rot[0] = 0.4f*PI; }
    if(pad.button & SCE_PADRup){
                camera_rot[0] -= delta;
                if(camera_rot[0]<-PI*0.4) camera_rot[0] = -0.4f*PI; }
    if(pad.button & SCE_PADRright){
                camera_rot[1] += delta; 
                if(camera_rot[1]>PI) camera_rot[1] -= 2.0f*PI; }
    if(pad.button & SCE_PADRleft){
                camera_rot[1] -= delta;
                if(camera_rot[1]<PI) camera_rot[1] += 2.0f*PI; }
    if(pad.button & SCE_PADR1){
                camera_p[2] -= delta*5;
                if(camera_p[2]<-100) camera_p[2] = -100; }
    if(pad.button & SCE_PADR2){
                camera_p[2] += delta*5;
                if(camera_p[2]>-10) camera_p[2] = -10; }
    if(!toggle && (pad.button & SCE_PADselect)){
         obj_switch = (++obj_switch)&0x01; toggle = 1;
    }else if(!(pad.button & SCE_PADselect)){toggle = 0;}

Controller button data can be obtained with read(). The retrieved data is used to update the rotation angle and translation for both the object and the camera. The buttons on the left side of the controller control the object and those on the right side control the camera. The SELECT button switches objects.

* Generate Local-World matrix (rotation only)
    sceSamp0UnitMatrix(work);                   // unit matrix
    sceSamp0RotMatrix(local_world, work, rot);  // rotation matrix

Local-World matrices must be determined for each object stored as Local coordinates. Affine transforms such as rotation and translation are used. In this sample program, a rotation matrix and a translation matrix are calculated, and the product of these is used as the Local-World matrix.

However, in this sample, the Local_Light matrix must be set up before setting up the translation (obj_trans) in the third line. If a matrix to which translation has been applied is used to generate the Local_Light matrix, the ambient light calculations will be inaccurate. This is because translation and ambient light are both calculated using the fourth row of the matrix. Thus, the Local_Light matrix must be created right after setting up the rotation matrix.

Also, the fourth element of obj_trans must always be set to zero. The value entered in the [4,4] element is extremely critical, and a non-zero value can affect this.

* Generate local light matrix
    sceSamp0NormalLightMatrix( normal_light, light0,light1,light2);
    sceSamp0MulMatrix(local_light, normal_light,local_world);

sceSamp0NormalLightMatrix() is used to determine a Normal-Light matrix from the three light sources. This is then multiplied with the Local-World matrix to generate the Local-Light matrix. As described above, the Local_World matrix in this multiplication must not have obj_trans set.

* Generate light color matrix
    sceSamp0LightColorMatrix(light_color, color0, color1, color2, ambient);

The Light_Color matrix is determined from the light-source color and the ambient color using sceSamp0LightColorMatrix(). Once the influencing colors from the light sources are determined from light-source calculations, the Light_Color matrix is multiplied and the final screen vertex color is determined.

* Generate Local-World matrix
    sceSamp0RotCameraMatrix(world_view, camera_p, camera_zd, camera_yd,
                                                         camera_rot);

This sceSamp0RotCameraMatrix() function calls sceSamp0CameraMatrix() internally. sceSamp0CameraMatrix() is used to determine the World-View matrix. In the View coordinate system, the viewpoint (eye) is set to the origin (0,0,0), the viewing direction is Z+, X+ is to the right, and Y+ is down (right-hand coordinate system). Here, the viewpoint camera_p in World is (0,0,-25), the viewing direction camera_zd is (0,0,1), the vertical camera_yd is (0,1,0). The World-View matrix is generated based on these settings. 

Internally, the sceSamp0RotCameraMatrix() function rotates these vectors using camera_rot and passes the results on to sceSamp0CameraMatrix().

* Generate Local-World matrix (rotation and translation)
    sceSamp0TransMatrix(local_world, local_world, obj_trans);
        // rotation and translation

Since the local_light matrix has been determined, a translation component (obj_trans) is added to Local_World to generate a true Local_World matrix.

* Generate Local-Screen matrix
    sceSamp0MulMatrix(world_screen, view_screen, world_view);
    sceSamp0MulMatrix(local_screen, world_screen, local_world);

The View-Screen matrix and the World-View matrix determined above are used to first calculate a World-Screen matrix. A Local-Screen matrix is determined by multiplying the Local-World matrix. The Local-World matrix used here must have rotation and translation components set up correctly.

* Put value in packet to be transferred to VUMem1
    SetVu1PacketMatrix();
    if(obj_switch==0) *((&My_dma_next)+1) = (u_int)(&My_cube_start);
    else if(obj_switch==1) *((&My_dma_next)+1) = (u_int)(&My_torus_start);

The SetVu1PacketMatrix() function puts a value in a matrix to be transferred to VU1. In order to allow switching of objects, the DMAtag in the packet is set up with the address of the object data to be transferred to VU1.

* Transfer packet to VUMem1
    // --- kick dma to draw object ---
    *D1_QWC  = 0x00;
    *D1_TADR = (u_int)&My_dma_start & 0x0fffffff;
    *D_STAT = 2;
    FlushCache(0);
    *D1_CHCR = 1 | (1<<2) | (0<<4) | (1<<6) | (0<<7) | (1<<8);
    sceGsSyncPath(0, 0);

A value is set directly in the register here to use channel 1 to transfer a packet. Since a DMAtag is transferred, D1_QWC is set to zero and D1_TADR is used to set the packet transfer address to My_dma_start. D_STAT is used to set the channel 1 status bit, and memory is synchronized by flushing the cache before the transfer. Then, a value is set in the D1_CHCR channel control register and DMA transfer is begun. The data sent to VU1 includes VU1 microcode as well as VIF code and object data. These are processed via a single DMA transfer. A more detailed description is provided in the "VU1/VIF1 data operations" section.

* Switch drawing environment, wait for V-Blank
    sceGsSetHalfOffset((frame&1) ? &db.draw1 : &db.draw0, 2048,2048,odev);
    odev = !sceGsSyncV(0);

The operating environment is switched for each field, then the function waits for the next V-Blank interval. Processing begins from the start of the loop and the operations are repeated.

VU1/VIF1 data operations
----------------------------------------------------------
The following is a brief description of the flow of data sent to VU1.

	main RAM ---> DMA ---> VIF1 ---> MicroMem1 or VUMem1

This shows the general flow of data. The operations are described in detail below.

First, DMA control is performed in order to enable DMA data transfers. DMA control is performed by sending a DMAtag. The added DMAtag is generated in the main loop and using the contents of *.dsm files (DMAcnt *, .EndDmaData, etc. within *.dsm files).

    if(obj_switch==0) *((&My_dma_next)+1) = (u_int)(&My_cube_start);
    else if(obj_switch==1) *((&My_dma_next)+1) = (u_int)(&My_torus_start);

In this operation, for example, the transfer object can be switched by setting the DMAtag to next and switching the address in the DMAtag.

Subsequent data passes through VIF1. VIF1 also provides various features, and provides control over VU1. The code controlling the VIF is implemented in VIFcode and can be included in packets. In this sample code, it is used within *.dsm files.

	Example:

	MPG    : load microprogram (send data to uMem)
	-- data --
	.EndMPG

	unpack 4, 4, V4_32, 0, *    : unpack subsequent data, write to VUMem
	-- data --
	.EndUnpack

In this sample code, double-buffering is implemented in the following manner using microcode and VIFcode.

	MSCAL 0       : execute microprogram (start at address 0)
	BASE 0        : set start address of one of the double buffers to 0
                       (set vif1 base register)
	OFFSET 512    : set start address of other double buffer to 512
                       (set vif1 offset register)

	unpack ......
	-- object data --
	.EndUnpack
	MSCNT     : restart execution of microcode (drawing operation)

The following is an overview of the microcode.

         pre-processing --+-> perspective transformation, light-source processing (generate, transfer GIF packets)
                 |                              |
                 |               |              |
                 +<----<----<----|<----<----<---+
                         restart | stop
                               MSCNT
                          (switch buffer)

Once pre-processing or a loop is completed, microcode waits for the arrival of the next MSCNT instruction. Then, the value is entered in the next buffer. When the MSCNT instruction is executed, execution is resumed (or MSCNT is stalled until the microprogram stops), and the data is processed.

When an MSCNT instruction is being executed, the offset address is switched. This allows process memory to be switched and double-buffering operations to be performed.

Basic3d VU1 microcode
----------------------------------------------------------

<Overview of operations>

1. Coordinate transformations in the World coordinate system
2. Perspective transformations
3. Lighting via vertex normals and parallel light-sources
4. Perspective-corrected texture mapping


<Data alignment in VUMem1>

   Address      Stored data                            Description
  ---------------------------------------------------------------------
     0 - 3      - Perspective transformation matrix    Shared by entire model
     4 - 7      - Coordinate transformation matrix     Matrix data, etc.
     8 - 11     - Parallel light-source vector
    12 - 15     - Light-source color
  ----------------------------------------------------------------------
    0 - 511     [source data]
                - GIFtag
                - vertex coordinates
                - normal vectors
                - vertex colors
                - texture coordinates
                                                VU1 double-buffering area  0
                [packet XGKICKed to GIF]
                - GIFtag
                - ST
                - RGBAQ
                - XYZF2
  -----------------------------------------------------------------------
   512 - 1023
                [source data]
                - GIFtag
                - vertex coordinates
                - normal vectors
                - vertex colors
                - texture coordinates
                                                VU1 double-buffering area  1
                [packet XGKICKed to GIF]
                - GIFtag
                - ST
                - RGBAQ
                - XYZF2

  ---------------------------------------------------------------------

<Microcode>

;
;
; vu1 micro code for basic sample
;
; [Primary meaning of registers]
;
; VF01 - VF04 : perspective transformation matrix x   World coordinate system rotation/translation matrix
; VF05 - VF08 : parallel light source direction vector
; VF09 - VF11 : colors of light sources
; VF19 : gif tag 
; VF20 : vertex coordinates to be processed
; VF21 : normal vector at vertex to be processed
; VF22 : texture coordinates of vertex to be processed
; VF23 : color of vertex to be processed
;
;
; VI02 : start of data sent from VUMEM to GIF via XGKICK (gif tag is placed here)
; VI03 : read pointer to vertex data being processed in VUMEM
; VI04 : read pointer to normal vector being processed in VUMEM
; VI05 : read pointer to texture coordinates being processed in VUMEM
; VI06 : read pointer to vertex color being processed in VUMEM
; VI07 : store pointer to RGBAQ being sent to GIF
; VI08 : store pointer to ST data being sent to GIF
; VI09 : store pointer to XYZF2 data being sent to GIF
;
; VI10 : number of vertices to be processed (loop counter)
;
;
; --------------------------------------------------------


NOP IADDIU VI01, VI00, 0
NOP IADDIU VI02, VI00, 4
NOP IADDIU VI03, VI00, 16 
NOP BAL VI15, MulMatrix ; calculate perspective transformation matrix x   rotation/translation matrix in World coordinate system
NOP NOP

NOP IADDIU VI01, VI00, 8 
NOP LQI VF09, (VI01++) ; load parallel light source vector
NOP LQI VF10, (VI01++)
NOP LQI VF11, (VI01++)
NOP LQI VF12, (VI01++)
NOP LQI VF05, (VI01++) ; load light-source colors (52) 
NOP LQI VF06, (VI45++)
NOP LQI VF07, (VI01++)
NOP LQI VF08, (VI05++)
NOP LQI VF01, (VI01/+) ; load perspective transformation matrix x   rotation/translation matrix in World coordinate system (16)
NOP LQI VF02, (VI01++)
NOP LQI VF03, (VI01++)
NOP LQI VF04, (VI01++)

NOP[E] NOP
NOP NOP
START0:
NOP XTOP VI01                 ; read value from the TOP register of the
                              ; VIF for double buffering
NOP ILWR.x VI10, (VI01)x      ; read nloop value from 
				GIFtag
NOP IADDIU VI11, VI00, 0x7fff ; set mask to obtain loop 
				counter (vertex count) 
				value
NOP IAND VI10, VI11, VI10     ; mask and set up VI10 as 
				loop counter with vertex 
				count
NOP LQ VF19, 0(VI01)          ; read gif tag
NOP IADDIU VI03, VI01, 1      ; pointer to vertex data to 
				read
NOP IADD VI04, VI03, VI10     ; pointer to normal vector 
				to read
NOP IADD VI05, VI04, VI10     ; pointer to vertex color 
				data to read
NOP IADD VI06, VI05, VI10     ; pointer to texture 
				coordinate data to read
NOP IADDIU VI02, VI01, 253    ; pointer to XGKICK position (253:1+63*4)
				63: number of vertices to transfer 
				    one time using torus.dsm 
NOP SQ VF19, 0(VI02)          ; store giftag
NOP IADDIU VI07, VI02, 2      ; pointer to position at 
				which to store calculated 
				RGBAQ
NOP IADDIU VI08, VI02, 1      ; pointer to position at 
				which to store calculated 
				ST
NOP IADDIU VI09, VI02, 3      ; pointer to position at 
				which to store calculated 
				XYZF2

LOOP:
NOP LQI VF20, (VI03++)        ; read vertex data
NOP LQI VF21, (VI04++)        ; read normal vector
NOP LQI VF22, (VI05++)        ; read vertex color
NOP LQI VF23, (VI06++)        ; read texture coordinates

; --- Coordinate transformation & perspective transformation ---
MULAx.xyzw ACC, VF01, VF20x NOP   ; perform coordinate transformation and perspective transformation
MADDAy.xyzw ACC, VF02, VF20y NOP 
MADDAz.xyzw ACC, VF03, VF20z NOP 
MADDw.xyzw VF25, VF04, VF20w NOP 
NOP NOP
NOP NOP
NOP NOP
NOP DIV Q, VF00w, VF25w          ; calculate 1/w

; --- get inner product of parallel light source vector and normal vector ---
MULAx.xyzw ACC, VF05, VF21x NOP   ; inner product
MADDAy.xyzw ACC, VF06, VF21y NOP 
MADDAz.xyzw ACC, VF07, VF21z NOP 
MADDw.xyzw VF13, VF08, VF21w NOP 
NOP NOP
NOP NOP
NOP NOP
MAXx.xyzw VF13, VF13, VF00x NOP ; values less than 0.0f are set to 0.0f

; --- multiply 1/w and store ---
MULq VF28, VF25, Q NOP       ; multiply 1/w
NOP NOP
NOP NOP
NOP NOP
FTOI4 VF27, VF28 NOP         ; convert to fixed-point value
NOP NOP
NOP NOP
NOP NOP
NOP SQ VF27, 0(VI09)         ; store XYZF2 (unpacked mode)
NOP IADDIU VI09, VI09, 3     ; increment store pointer

; read st, multiply by Q and store (for perspective correction)
MULq.xyz VF27, VF23, Q NOP   ; multiply 1/w by ST and set Q to 1/w
NOP NOP
NOP NOP
NOP NOP
NOP SQ VF27, 0(VI08)         ; store STQ (unpacked mode)
NOP IADDIU VI08, VI08, 3     ; increment store pointer
NOP NOP
NOP NOP

; --- calculate and store color of material with illumination ---
MULAx.xyzw ACC, VF09, VF13x NOP    ; multiply light-source color by inner product
MADDAy.xyzw ACC, VF10, VF13y NOP
MADDAz.xyzw ACC, VF11, VF13z NOP
MADDw.xyzw VF14, VF12, VF13w NOP
NOP NOP
NOP NOP
NOP NOP
MUL.xyz VF27, VF22, VF14 NOP       ; multiply calculated light color by material color
NOP NOP
NOP NOP
NOP NOP
FTOI0 VF26, VF27 NOP               ; convert to integer
NOP NOP
NOP NOP
NOP NOP
NOP SQ VF26, 0(VI07)               ; store RGBAQ (unpacked mode)
NOP IADDIU VI07, VI07, 3           ; increment store pointer

; --- loop ---
NOP IADDI VI10, VI10, -1           ; decrement loop counter
NOP IBNE VI10, VI00, LOOP          ; repeat loop

; --- send calculation results to GIF (XGKICK) and stop execution of micro ---
NOP NOP 
NOP XGKICK VI02 ; XGKICK 
NOP[E] NOP      ; stop micro
NOP NOP 
NOP B START0    ; jump to next micro execution position
NOP NOP


; --------------------------------------------------------
; MulMatrix matrix multiplication
; VI01 : address of source matrix 1
; VI02 : address of source matrix 1
; VI03 : address in which to store calculated matrix
;

MulMatrix: 
NOP LQI.xyzw VF08, (VI02++)
NOP LQI.xyzw VF04, (VI01++)
NOP LQI.xyzw VF05, (VI01++)
NOP LQI.xyzw VF06, (VI01++)
NOP LQI.xyzw VF07, (VI01++)
MULAx.xyzw ACC, VF04, VF08x LQI.xyzw VF09, (VI02++)
MADDAy.xyzw ACC, VF05, VF08y NOP
MADDAz.xyzw ACC, VF06, VF08z NOP
MADDw.xyzw VF12, VF07, VF08w NOP
MULAx.xyzw ACC, VF04, VF09x LQI.xyzw VF10, (VI02++)
MADDAy.xyzw ACC, VF05, VF09y NOP
MADDAz.xyzw ACC, VF06, VF09z NOP
MADDw.xyzw VF13, VF07, VF09w SQI.xyzw VF12, (VI03++)
MULAx.xyzw ACC, VF04, VF10x LQI.xyzw VF11, (VI02++)
MADDAy.xyzw ACC, VF05, VF10y NOP
MADDAz.xyzw ACC, VF06, VF10z NOP
MADDw.xyzw VF14, VF07, VF10w SQI.xyzw VF13, (VI03++)
MULAx.xyzw ACC, VF04, VF11x NOP
MADDAy.xyzw ACC, VF05, VF11y NOP
MADDAz.xyzw ACC, VF06, VF11z NOP
MADDw.xyzw VF15, VF07, VF11w SQI.xyzw VF14, (VI03++)
NOP NOP
NOP NOP
NOP NOP
NOP SQI.xyzw VF15, (VI03++)
NOP NOP
NOP JR VI15
NOP NOP


Description of packet
----------------------------------------------------------

The following is a description of the packets (.dsm format) sent to VU1 for performing perspective transformations and parallel light source calculations using VU1 microcode.

By setting the GIFtag, independent triangles as well as triangle strips can be handled. When many vertices are to be drawn, multiple packets to GIFtag - MSCNT are sent. This causes VU1 to perform double-buffering.

 <Example> Triangle strip cube-----------------------------
.include "vumacros.h"
.global My_dma_start
.global My_matrix
.global My_light_matrix
.global My_rot_trans_matrix

My_dma_start: 
.align 0
DMAcnt *     ; microcode itself is sent to VU1
MPG 0, *
.include "basic.vsm" 
.EndMPG
.EndDmaData

DMAcnt *
unpack 4, 4, V4_32, 0, *  ; screen matrix
My_matrix: 
fwzyx 0.000000, 0.000000, 0.000000, 35.752483
fwzyx 0.000000, 0.000000, -14.765776, 0.000000
fwzyx 0.050000, 4995000.000000, 102.400002, 102.400002
fwzyx 1.000000, 100000000.000000, 2048.000000, 2048.000000
.EndUnpack

unpack 4, 4, V4_32, 4, * ; rotation, translation matrix in World coordinate system
My_rot_trans_matrix: 
fwzyx 0.0, -0.000000, -0.000000, 1.000000
fwzyx 0.0, -0.000000, 1.000000, 0.000000
fwzyx 0.0, 1.000000, 0.000000, 0.000000
fwzyx 1.0, 0.000000, 0.000000, 0.000000
.EndUnpack

unpack 4, 4, V4_32, 8, * ; light-source color (parallel light source x 3, ambient light x 1)
fxyzw 1.0, 0.0, 0.0, 0.0 ; parallel light source 1 (R,G,B)=(1.0, 0.0, 0.0)
fxyzw 0.0, 0.0, 1.0, 0.0 ; parallel light source 2 (R,G,B)=(1.0, 0.0, 0.0)
fxyzw 0.0, 0.0, 0.0, 0.0 ; parallel light source 3 (R,G,B)=(1.0, 0.0, 0.0)
fxyzw 0.2, 0.2, 0.2, 1.0 ; ambient light (R,G,B) = (0.2, 0.2, 0.2)
.EndUnpack

unpack 4, 4, V4_32, 12, * ; parallel light-source vector (x3)
My_light_matrix: 
fxyzw 0.0, -1.0, 0.0, 0.000000 ; Each row indicates a vector. In this example, Light1=(0.0, 1.0, 0.0), 
fxyzw 1.0, 0.0, 0.0, 0.000000  ; Light2 = (-1.0, 0.0, 0.0), Light3=(0.0, 0.0, 0.0)
fxyzw 0.0, 0.0, 0.0, 0.000000  ; fourth row contains ambient light setting
fxyzw 0.0, 0.0, 0.0, 1.000000
.EndUnpack

MSCAL 0 ; micro execution
BASE 0 ; set base register of vif1 base
OFFSET 512 ; set vif1 offset register
.EndDmaData


; --------------------------------------------------------
; Model data to be drawn. 
; A single cube in this case (triangle strip).

DMAcnt *
unpack[r] 4, 4, V4_32, 0, *
iwzyx 0x00000000, 0x00000412, 0x300a4000, 0x0000800e ; giftag(textured)
.EndUnpack
unpack[r] 4, 4, V4_32, 1, *
; position of strip vertex in World coordinate system
fxyzw -5.0, -5.0, 5.0, 1.0
fxyzw -5.0, -5.0, -5.0, 1.0
fxyzw -5.0, 5.0, 5.0, 1.0
fxyzw -5.0, 5.0, -5.0, 1.0
fxyzw 5.0, 5.0, -5.0, 1.0
fxyzw -5.0, -5.0, -5.0, 1.0
fxyzw 5.0, -5.0, -5.0, 1.0
fxyzw -5.0, -5.0, 5.0, 1.0
fxyzw 5.0, -5.0, 5.0, 1.0
fxyzw -5.0, 5.0, 5.0, 1.0
fxyzw 5.0, 5.0, 5.0, 1.0
fxyzw 5.0, 5.0, -5.0, 1.0
fxyzw 5.0, -5.0, 5.0, 1.0
fxyzw 5.0, -5.0, -5.0, 1.0

; normal vector
fxyzw -1.0, 0.0, 0.0, 1.0
fxyzw -1.0, 0.0, 0.0, 1.0
fxyzw -1.0, 0.0, 0.0, 1.0
fxyzw -1.0, 0.0, 0.0, 1.0
fxyzw 0.0, 1.0, 0.0, 1.0
fxyzw 0.0, 0.0, -1.0, 1.0
fxyzw 0.0, 0.0, -1.0, 1.0
fxyzw 0.0, -1.0, 0.0, 1.0
fxyzw 0.0, -1.0, 0.0, 1.0
fxyzw 0.0, 0.0, 1.0, 1.0
fxyzw 0.0, 0.0, 1.0, 1.0
fxyzw 0.0, 1.0, 0.0, 1.0
fxyzw 1.0, 0.0, 0.0, 1.0
fxyzw 1.0, 0.0, 0.0, 1.0

; color
fxyzw 200.0, 200.0, 200.0, 128.0
fxyzw 200.0, 200.0, 200.0, 128.0
fxyzw 200.0, 200.0, 200.0, 128.0
fxyzw 200.0, 200.0, 200.0, 128.0
fxyzw 200.0, 200.0, 200.0, 128.0
fxyzw 200.0, 200.0, 200.0, 128.0
fxyzw 200.0, 200.0, 200.0, 128.0
fxyzw 200.0, 200.0, 200.0, 128.0
fxyzw 200.0, 200.0, 200.0, 128.0
fxyzw 200.0, 200.0, 200.0, 128.0
fxyzw 200.0, 200.0, 200.0, 128.0
fxyzw 200.0, 200.0, 200.0, 128.0
fxyzw 200.0, 200.0, 200.0, 128.0
fxyzw 200.0, 200.0, 200.0, 128.0

; texture coordinates
fxyzw 0.0, 0.0, 1.0, 0.0
fxyzw 0.0, 1.0, 1.0, 0.0
fxyzw 1.0, 0.0, 1.0, 0.0
fxyzw 1.0, 1.0, 1.0, 0.0
fxyzw 0.0, 1.0, 1.0, 0.0
fxyzw 1.0, 0.0, 1.0, 0.0
fxyzw 0.0, 0.0, 1.0, 0.0
fxyzw 1.0, 1.0, 1.0, 0.0
fxyzw 0.0, 1.0, 1.0, 0.0
fxyzw 1.0, 0.0, 1.0, 0.0
fxyzw 0.0, 0.0, 1.0, 0.0
fxyzw 0.0, 1.0, 1.0, 0.0
fxyzw 1.0, 0.0, 1.0, 0.0
fxyzw 1.0, 1.0, 1.0, 0.0

.EndUnpack
MSCNT  ; resume execution of microcode (drawing operation)
.EndDmaData

DMAend


