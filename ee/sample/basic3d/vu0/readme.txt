[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 1999 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Basic sample program basic3d: VU0 version
=========================================

<Description>
The Core version of basic3d presented math functions for matrix operations, perspective transformations, etc. as well as the mathematical foundations needed for 3D graphics. In the VU0 version, sections of the code are replaced with VU0 macro instructions as a first step in optimization.

VU0 macro instructions are coprocessor instructions that can be used in assembler format, just like CPU instructions. Up to four sets of data can be processed at once, making VU0 operations suitable for matrix operations and vector calculations.

The purpose of the VU0 version of basic3d is to show how VU0 macro instructions can be used to replace the code for matrix and other arithmetic operations that are found in the Core version of the program. This will serve as the basis for an understanding of how to implement further optimizations using VU0 and VU1 microcode.

<Files>
     main.c     main program
     vu0.c      functions for matrix operations such 
		as coordinate transformations and 
		perspective transformations
     cube.s     object data (cube)
     torus1.s   object data (torus data: split)
     torus.s	object data (torus data: split)	
     flower.dsm texture data

<Execution>
     % make         : compilation
     % make run     : execution

After compilation, the following method can also be used:

     % dsedb
     > run main.elf

<Controller operations>
     up/down arrow (direction keys): rotate around x-axis
			              (object)
     left/right arrow:       	rotate around y-axis 
				(object)
     L1/L2 buttons :            rotate around z-axis 
				(object)
     triangle/X buttons:       rotate around x-axis 
				(viewpoint)
     square/circle buttons:    rotate around y-axis 
				(viewpoint)
     R1/R2 buttons:             rotate around z-axis 
				(viewpoint)
     SELECT button:             switch object (CUBE/TORUS)

<Specification>
     Display cube with textured triangle strips.
     Display torus with textured triangles.

     24-bit Z-buffer
     Three light sources
     32-bit textures

<Summary of operations>
1. load textures
2. open pad
3. set up perspective transformation, light-source calculation matrices, etc.
4. perform perspective transformations and light-source calculations and generate packets to be sent to the GS
6. DMA kick to the GS
7. go back to step 2

Data flow
---------------------------------------------------------

<Initialization>
Main RAM                          GS RAM(4M)
texture data ----------------> set up texture data
  set up object data
  set up perspective transformation matrices, etc.

<Execution>
Main RAM                                scratch pad (16K)
Object data ------------------> set up packet data to be transferred to the GS
        perspective transformations, light-source calculations (primitive data
                     (VU0 geometry)    (STQ, RGBA, XYZF) arrays)
                                                    |
                                                    |
internal GS RAM(4M) <-------------------------------------+
 write frame & Z buffer         DMA transfer (add DMAtag, GIFtag)
 (rendering)


Description of program (detailed)
----------------------------------------------------------

* Variables/structures/functions (not including mathfunc.c functions)



SampleCubeDataHead[]     : pointer to object data (cube)
SampleTorus1DataHead[]   : pointer to object data (torus)
My_texture1[]            : pointer to texture data

TexEnv          : structure for generating packets to 
		  transfer sceGsTexEnv structures
QWdata          : shared structure to make 128-bit data 
		  easier to handle
GifPacket       : structure for generating packets to be 
		  transferred to Gif(GS)
ObjData         : structure for managing object data in 
		  memory

camera_p        : camera position
camera_zd       : viewing direction vector for camera
camera_yd       : down vector for camera
camera_rot      : rotation vector for camera

light0          : light 0
light1          : light 1
light2          : light 2
color0          : color of light 0
color1          : color of light 1
color2          : color of light 2

ambient         : ambient light

obj_trans       : translation vector for object
obj_rot         : rotation vector for object

local_world     : matrix for transforming object's local 
		  coordinates to World coordinate system
world_view      : matrix for transforming World coordinate
		  system to View coordinate system
view_screen     : matrix for transforming View coordinate 
		  system to Screen coordinate system
local_screen    : matrix for transforming Local 
		  coordinates to Screen coordinates 
		  (product of the previous three matrices)

normal_light    : matrix for getting the inner product of 
		  a vertex normal vector and a light 
		  direction vector in the World coordinate 
		  system (light direction vector is set)
local_light     : matrix multiplied to normal_light matrix 
		  and local_world matrix in order to 
	          obtain the inner product of the vertex 
                  normal vector and the light direction 
		  vector in the Local coordinate system
light_color     : matrix for getting summation of products 
		  between light colors (three colors + 
		  ambient light) and inner product of 
		  local_light matrix and vertex normal 
		  vector
local_color     : matrix resulting from product of 
		  local_light and light_color

work            : matrix providing temporary workspace

scePadData      pad     : structure for reading pad data
sceGsDBuff      db      : structure for double buffering
sceGsLoadImage  pTex    : structure for transferring 
			  textures to the local memory in 
			  the GS
sceDmaChan      *dmaVif : structure for DMA transfers of 
			  data
TexEnv          texenv  : structure for generating packets 
			  for transferring texture data

LoadObj(ObjData *o, u_int *Head)
                        : function to take object data in 
			  the main RAM and load it into 
		          and manipulate it in the ObjData 
			  structure

ReleaseObj(ObjData *o)
                        : function to release loaded 
			  object data

MakePacket(ObjData *obj, int num)
                        : function for performing 
			  perspective transformations, 
			  light-source calculations, etc. 
			  on object data and generating 
			  packets to be transferred to 
			  the GS

* variables in function main

i, j 	: counters for for loops, etc.
fd      : file descriptor for obtaining return value 
	  when pad is opened
fr      : switch used for flipping double buffers
delta   : variable for setting up rotation angle
obj     : variable for registering objects


Description of program (initialization)
---------------------------------------------------------

* Load object data
    LoadObj(&obj[0], SampleCubeDataHead);
    LoadObj(&obj[1], SampleTorus1DataHead);

The object data used in the sample program is loaded from SampleCubeDataHead and SampleTorus1DataHead into the ObjData structure.

* Initialize device
    sceGsResetPath();    // initialize data devices (VIF1,VU1,GIF)
    sceDmaReset(1); // initialize DMA (DMA)
    sceVpu0Reset(); // initialize VU0 (VIF0,VU0)

All devices that are to be used are initialized at the start of the program. Unexpected behavior may result if uninitialized devices are used, so always remember to initialize.

* Set up drawing environment
    sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
          // initialize the GS
    sceGsSetDefDBuff(&db, SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT,
               SCE_GS_ZGEQUAL, SCE_GS_PSMZ24, SCE_GS_CLEAR);
                    // set up double-buffering
    *(u_long *)&db.clear0.rgbaq = SCE_GS_SET_RGBAQ(0x10, 0x10, 0x10,
                         0x80, 0x3f800000);
    *(u_long *)&db.clear1.rgbaq = SCE_GS_SET_RGBAQ(0x10, 0x10, 0x10,
                                   0x80, 0x3f800000);

Once the devices have been initialized, the GS drawing environment is set up. Displaying to the screen will not be possible unless the environment is set up. This is also required for controller settings which are synchronized via VSync, so always remember to set up the environment right after devices have been initialized.

* Transfer texture images
    sceGsSetDefLoadImage(&pTex,13340,IMAGE_SIZE / 64,SCE_GS_PSMCT32,
               0, 0,IMAGE_SIZE, IMAGE_SIZE);// set up LoadImage data
    FlushCache(0);
    sceGsExecLoadImage(&pTex, (u_long128 *)My_texture1);// execute LoadImage
    sceGsSyncPath(0, 0); // wait for data transfer to finish

LoadImage data is set up to transfer texture images to the local memory of the GS. Then, sceGsExecLoadImage() is executed to transfer the specified image to the local memory of the GS. This function uses DMA internally, so FlushCache(0) is performed to provide consistency between the D-cache and the main memory. Finally, sceGsSyncPath() is called to wait for completion of the data transfer. This completes the image transfer to the local memory of the GS.

* Generate packets to set up texture environment
    FlushCache(0);
    texenv.size = sceGsSetDefTexEnv(&texenv.gs_tex, 0, 13340,
     IMAGE_SIZE / 64,SCE_GS_PSMCT32, 8, 8, 0, 0, 0, 0, 1);
          // generate texture settings data
    SCE_GIF_CLEAR_TAG(&texenv.giftag);
    texenv.giftag.NLOOP = texenv.size;
    texenv.giftag.EOP = 1;
    texenv.giftag.NREG = 1;
    texenv.giftag.REGS0 = 0xe;
    *(u_long*)&texenv.gs_tex.clamp1
     = SCE_GS_SET_CLAMP(0, 0, 0, 0, 0, 0);

sceGsSetDefTexEnv() is used to generate packets for setting up texture-related general-purpose GS registers TEX0_1, TEX0_1, CLAMP_1

* Transfer packets for setting up texture environment
    FlushCache(0);
    dmaVif = sceDmaGetChan(SCE_DMA_GIF);
    dmaVif->chcr.TTE = 1;
    sceDmaSendN(dmaVif, &texenv.giftag, texenv.size+1);
                    // transfer texture set-up data
    sceGsSyncPath(0, 0); // wait for completion of data transfer

Packets generated for setting up the texture environment are transferred using Normal DMA via PATH3.

* Initialize controller
    fd = open("pad:0", 0);

Initialization is performed for the 1P controller. Subsequent read() operations will load data into the specified buffer. The controller data will be updated at each VSync.

Program description (main loop)
----------------------------------------------------------

The following is a description of the operations performed in the main loop such as controller data acquisition, matrix generation, and packet generation (coordinate transformations, perspective transformations, UV->STQ transformations, light-source calculations).

* Generate View-Screen matrix
    sceVu0ViewScreenMatrix(view_screen, 512.0f, 1.0f, 0.47f,
         2048.0f, 2048.0f,1.0f, 16777215.0f,1.0f,65536.0f);

sceVu0ViewScreenMatrix() is used to determine the View-Screen matrix. The resulting View-Screen matrix assumes a distance between the view point and the projection screen of 512, a screen aspect ratio of 1:0.47, the center of the screen at (2048, 2048) in the GdPrimitive coordinate system, a Z-buffer value of 1.0-16777215.0, and a Z value of 1.0-65536.0.

* Get controller information
    if(pad.button & SCE_PADLdown){
        obj_rot[0] += delta; if(obj_rot[0]>PI) obj_rot[0] -= 2.0f*PI; }
    if(pad.button & SCE_PADLup){
        obj_rot[0] -= delta; if(obj_rot[0]<-PI) obj_rot[0] += 2.0f*PI; }
    if(pad.button & SCE_PADLright){
        obj_rot[1] += delta; if(obj_rot[1]>PI) obj_rot[1] -= 2.0f*PI; }
    if(pad.button & SCE_PADLleft){
        obj_rot[1] -= delta; if(obj_rot[1]<-PI) obj_rot[1] += 2.0f*PI; }
    if(pad.button & SCE_PADL1){
        obj_rot[2] += delta; if(obj_rot[2]>PI) obj_rot[2] -= 2.0f*PI; }
    if(pad.button & SCE_PADL2){
        obj_rot[2] -= delta; if(obj_rot[2]<-PI) obj_rot[2] += 2.0f*PI; }
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
     if(camera_rot[1]<-PI) camera_rot[1] += 2.0f*PI; }
    if(pad.button & SCE_PADR1){
        camera_p[2] -= delta*5; if(camera_p[2]<-100) camera_p[2] = -100; }
    if(pad.button & SCE_PADR2){
        camera_p[2] += delta*5; if(camera_p[2]>-10) camera_p[2] = -10; }
    if(!toggle && (pad.button & SCE_PADselect)){
        obj_switch = (++obj_switch)&0x01; toggle = 1;
    }else if(!(pad.button & SCE_PADselect)){toggle = 0;}

Controller button data can be obtained with read(). The data retrieved here is used to update the rotation angle and translation for both the object and the camera. The buttons on the left side of the controller control the object and those on the right side control the camera.

* Generate Local-World matrix (rotation only)
    sceVu0UnitMatrix(work);                       // unit matrix
    sceVu0RotMatrix(local_world, work, obj_rot);  // rotation matrix

Local-World matrices must be determined for each object stored as Local coordinates. Affine transformations such as rotation and translation are used. In this sample program, a rotation matrix and a translation matrix are calculated, and the product of these is used as the Local-World matrix. However, in this sample, the Local_Light matrix must be set up before setting up the translation (obj_trans) in the third line. If a matrix to which translation has been applied is used to generate the Local_Light matrix, the ambient light calculations will be inaccurate. This is because translation and ambient light are both calculated using the fourth row of the matrix. Thus, the Local_Light matrix must be created right after setting up the rotation matrix.

Also, the fourth element of obj_trans must always be set to zero. The value entered in the [4,4] element is extremely critical, and a non-zero value can affect this.

* Generate local light matrix
    sceVu0NormalLightMatrix(normal_light, light0,light1,light2);
    sceVu0MulMatrix(local_light, normal_light,local_world);

sceVu0NormalLightMatrix() is used to determine a Normal-Light matrix from the three light sources. This is then multiplied with the Local-World matrix to generate the Local-Light matrix. As described above, the local_world matrix in this multiplication must be set up for rotation only.

* Generate light color matrix
    sceVu0LightColorMatrix(light_color, color0, color1, color2, ambient);
The Light_Color matrix is determined from the light-source color and the ambient color using sceVu0LightColorMatrix(). Once the influencing colors from the light sources are determined from light-source calculations, the Light_Color matrix is multiplied and the final screen vertex color is determined.

* Generate Local-World matrix (rotation and translation)
    sceVu0TransMatrix(local_world, local_world, obj_trans);
                         // rotation & translation

Since the local_light matrix has been determined, a translation component is added to local_world before the local_screen matrix is generated.

* Generate World-View matrix
    sceVu0RotCameraMatrix(world_view, camera_p, camera_zd,camera_yd,
                                   camera_rot);

This sceVu0RotCameraMatrix() function calls sceVu0CameraMatrix() internally. The sceVu0CameraMatrix() is used to determine the World-View matrix. In the View coordinate system, the view point (eye) is set to the origin (0,0,0), the viewing direction is Z+, X+ is to the right, and Y+ is down (right-hand coordinate system). Here, the view point camera_p in World is (0,0,-25), the viewing direction camera_zd is (0,0,1), the vertical camera_yd is (0,1,0). The World-View matrix is generated based on these settings. Internally, the sceVu0RotCameraMatrix() function rotates these vectors using camera_rot and passes the results on to sceVu0CameraMatrix().

* Generate Local-Screen matrix
    sceVu0MulMatrix(work, world_view, local_world);         //Local-View
    sceVu0MulMatrix(local_screen, view_screen, work);        //Local-Screen

The View-Screen matrix and the World-View matrix determined above are used to first calculate a World-Screen matrix. A Local-Screen matrix is determined by multiplying the Local-World matrix.

* Generate packet
    MakePacket(&obj[obj_switch], i);

Packet generation is performed in MakePacket(). A more detailed description is provided later.

* Transfer packet to the GS
    FlushCache(0);
    sceDmaSend(dmaVif,(u_long128*)((u_int)pBase&0x3ff0|0x80000000));
    sceGsSyncPath(0, 0);

Primitive packets generated with MakePacket() are transferred using Source Chain DMA via PATH3.

* Switch drawing environment, wait for V-Blank
    if(fr&0x01)
        sceGsSetHalfOffset(&db.draw1,2048,2048,sceGsSyncV(0)^0x01);
    else
        sceGsSetHalfOffset(&db.draw0,2048,2048,sceGsSyncV(0)^0x01);

Environment is switched for each field and then the function waits for the next V-Blank interval to begin.

Program description (packet generation)
---------------------------------------------------------

The following is a description of how MakePacket() generates packets.

* Initialize packet
    pack->size = 0;
    pack->buf = (QWdata *)0x70000000;//SPR

The packet write position is brought to the start. The packet generation region is set to the start of the scratchpad. When using the scratchpad, make sure that the size of the packet does not exceed 16 KB--the size of the scratchpad. If using main memory, the packet size must be less than 1 MB due to restrictions on QWC settings for DMA transfers.

* Generate DMAtag, GIFtag
    //DMAtag
    pack->buf[pack->size].ul128 = (u_long128)0;
    pack->buf[pack->size++].ui32[0]
        = 0x70000000 | (obj->vertexNum[num]*3 + 1);

    //GIFtag
    pack->buf[pack->size].ul64[0]
     = SCE_GIF_SET_TAG(obj->vertexNum[num], 1, 1,obj->prim,
          SCE_GIF_PACKED,3);
    pack->buf[pack->size++].ul64[1] = 0x0412L;

DMAtag and GIFtag must be added to the start of the packet. Source Chain mode is used for DMA. The size of outgoing packets is vertex count x 3 (STQ,RGBA,XYZF2)+1(GIFtag). The DMAtag ID is end (because there is only one DMA packet). PACKED mode (SCE_GIF_PACKED) is used for GIF, and primitive settings are made in the GIFtag. The REGS value is set to 0x412 (STQ,RGBA,XYZF2).

* Generate STQ, RGBA, XYZ
    sceVu0RotTransPersNClipCol(&pack->buf[pack->size].ul128, local_screen,
         vertex,normal, texUV,color, local_light,light_color,
         obj->vertexNum[num]);

sceVu0RotTransPersNClipCol() is used to determine packet contents (STQ, RGBA, XYZ).


Program description (VU0 macro instructions)
---------------------------------------------------------

The following is a description of the operations performed in sceVu0RotTransPersNClipCol(). This function, which serves as the core of this sample program, uses VU0 macro instructions to perform coordinate transformations, perspective transformations, light-source calculations, etc. Please refer to vu0.c.

* Matrix settings
    # Local-World matrix settings
    lqc2  vf4,0x0(%1)    #set local_world matrix[0]
    lqc2  vf5,0x10(%1)   #set local_world matrix[1]
    lqc2  vf6,0x20(%1)   #set local_world matrix[2]
    lqc2  vf7,0x30(%1)   #set local_world matrix[3]
    # Local-Light matrix settings
    lqc2  $vf17,0x0(%6)  #set local_light matirix[0]
    lqc2  $vf18,0x10(%6) #set local_light matirix[1]
    lqc2  $vf19,0x20(%6) #set local_light matirix[2]
    # Light-Color matrix settings
    lqc2  $vf21,0x0(%7)  #set light_color matrix[0]
    lqc2  $vf22,0x10(%7) #set light_color matrix[1]
    lqc2  $vf23,0x20(%7) #set light_color matrix[2]
    lqc2  $vf20,0x30(%7) #set light_color matrix[3]

First, the local_world matrices, the local_light matrices, and the light_color matrices are entered in the VU0 VF registers.

* Load vertex, normal, vertex color, ST
    lqc2  vf8,0x0(%2)    #load XYZ
    lqc2  $vf24,0x0(%4)  #load NORMAL
    lqc2  $vf25,0x0(%5)  #load COLOR
    lqc2  $vf27,0x0(%8)  #load ST

The vertex coordinates, normal, vertex color, and texture coordinates needed for packet generation are loaded in registers.

* Coordinate transformation
    # (X0,Y0,Z0,W0)=[SCREEN/LOCAL]*(X,Y,Z,1)
    vmulax.xyzw     ACC, vf4,vf8
    vmadday.xyzw    ACC, vf5,vf8
    vmaddaz.xyzw    ACC, vf6,vf8
    vmaddw.xyzw     vf12,vf7,vf8

The Local-Screen matrix and the vertex coordinates are multiplied to determine the screen coordinates. The value of W is identical to the Z value in the View coordinate system.

* Perspective transformation
    # (X1,Y1,Z1,1)=(X0/W0,Y0/W0,Z0/W0,W0/W0)
    vdiv    Q,vf0w,vf12w
    vwaitq
    vmulq.xyzw vf12,vf12,Q
    vftoi4.xyzw     vf13,vf12

The screen coordinates are multiplied by 1/W. The coordinates are then converted to fixed-point values to be used in GIF packets. The 1/W value calculated here is saved so that it can be multiplied with the texture coordinates later.

* Determine light-source influence
    # (L1,L2,L3)=[LLM](Nx,Ny,Nz)
    # LLM: local light matrix
    # L1,L2,L3: light-source influence
    # Nx,Ny,Nz: normal vector
    vmulax.xyzw    ACC, $vf17,$vf24
    vmadday.xyzw   ACC, $vf18,$vf24
    vmaddz.xyzw    $vf24,$vf19,$vf24
    vmaxx.xyz      $vf24,$vf24,$vf0 # negative values set to 0

The light-source influence is determined by multiplying the local light matrix and the normal vector. The result will be negative if the direction of the light source and the normal vector are the same. In such cases, 0 is used.

* Determine light-source influence color
    # (LTr,LTg,LTb,LTw)=[LCM](L1,L2,L3,1)
    # LCM: light color matrix
    # LTr,LTg,LTb: light-source influence color
    vmulax.xyzw    ACC, $vf21,$vf24
    vmadday.xyzw   ACC, $vf22,$vf24
    vmaddaz.xyzw   ACC, $vf23,$vf24
    vmaddw.xyzw    $vf24,$vf20,$vf0

The light color matrix and the light-source influence are multiplied to determine the light-source influence color.

* Determine on-screen vertex color
    # (RR,GG,BB) = (R,G,B)*(LTr,LTg,LTb)
    # R,G,B: vertex color
    # RR,GG,BB: on-screen vertex color
    vmul.xyzw  $vf26,$vf24,$vf25
    # [0..255] saturation
    vmaxx.xyz  $vf26,$vf26,$vf0
    lui        $2,0x437f
    ctc2  $2,$vi21
    vnop
    vnop
    vminii.xyz $vf26,$vf26,I
    vftoi0.xyzw     $vf26,$vf26

The on-screen vertex color is determined by multiplying the light-source influence color and the vertex color. The result must be in the range of 0-255, so clipping is performed outside this range. The result is also converted to a fixed-point value to be used in GIF packets.

* Determine ST
    # (S,T,Q) = (s,t,1)/w
    vmulq.xyz  $vf28,$vf27,Q

The 1/W value saved from the perspective transform is multiplied to perform perspective correction.

* Save STQ, RGBA, XYZ values
    sqc2  $vf28,0x0(%0)       #store STQ
    addi  %0,0x10
    sqc2  $vf26,0x0(%0)       #store RGBA
    addi  %0,0x10
    sqc2  vf13,0x0(%0)        #store XYZ
    addi  %0,0x10
    #
    addi  %3,-1
    addi  %2,0x10
    addi  %4,0x10
    addi  %5,0x10
    addi  %8,0x10
    bne        $0,%3,_rotTPNCC_loop


The STQ, RGBA, and XYZ values are saved in the packet according to the sequence used in GIFtag. If sending GIF, the Q value in the RGBAQ instruction is set to the value saved in the internal register by the preceding ST instruction, so note sequence. This concludes the sceVu0RotTransPersNClipCol() main loop. The operations beginning with vertex loading are repeated according to the specified vertex count while incrementing the various addresses.
