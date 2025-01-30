[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Basic sample program basic3d: Core version
==========================================

<Description>
The Core version of the basic3d sample program shows the mathematical foundations needed for 3D graphics processing.  The program illustrates how to implement mathematical functions that perform matrix operations, perspective transformations, and so on, using only the EE Core.

In order to make it easy to implement the VU0 and VU1 versions of these operations, equivalent functions are implemented in the VU0 and VU1 versions of the basic3d program. The results of calculations are also used to directly generate packets bound for the GS, in order to provide a basic overview of DMA and GS operations.


<Files>

     core.c         main program
     mathfunc.c     functions to perform matrix 
		    operations such as perspective 
                    transformations (Core versions)
     mathfunc.h	    header files for functions to 
		    perform matrix operations such as 
		    perspective transformations
     cube.s         object data (cube)
     torus1.s       object data (torus data: split)
     flower.dsm     texture data

<Compilation>
     % make

<Activation>
     % make run

After compilation, the following method can also be used:

     % dsedb
     > run core.elf

<Specifications>
Displays cube with textured triangle strips.
Displays torus with textured triangles.
(Switch with SELECT button)

24-bit Z-buffer
Three light sources
32-bit textures

<Controller operations>
up arrow/down arrow (direction keys): rotate object 
				       around x-axis
left arrow/right arrow: rotate object around y-axis
L1/L2 buttons: 	 	 rotate object around z-axis
square/circle buttons:  rotate camera around y-axis 
			 in world coordinate system
triangle/X buttons: 	 rotate camera around x-axis 
			 in world coordinate system
R1/R2 buttons: 	 	 turn camera forward, backward
SELECT button: 		 switch objects

<Summary of operations>
1. load textures
2. open pad
3. set up perspective transformation, light-source calculation matrices, etc.
4. perform perspective transformations and light-source calculations and generate packets to be sent to the GS
6. DMA kick to the GS
7. go back to step 2

Data flow
----------------------------------------------------------

<Initialization>
Main RAM                          GS RAM(4M)
texture data ----------------> set up texture data
  set up object data
  set up perspective transform matrices, etc.

<Execution>
Main RAM                                scratch pad (16K)
Object data ------------------> set up packet data to be transferred 
         			to the GS perspective transformations, 
	        light-source calculations 	(primitive data
                     (CPUcore geometry)    (STQ, RGBA, XYZF) arrays)
                                                    |
                                                    |
internal GS RAM(4M) <-------------------------------------+
 write frame & Z buffer         DMA transfer (add DMAtag, GIFtag)
 (rendering)


Description of program (detailed)
----------------------------------------------------------

* Variables/structures/functions (not including mathfunc.c functions)

        SampleCubeDataHead[]     : pointer to object data 
				   (cube)
        SampleTorus1DataHead[]   : pointer to object data 
				   (torus)
        My_texture1[]            : pointer to texture data

        TexEnv          : structure for generating packets 
			  to transfer sceGsTexEnv structures
        QWdata          : shared structure to make 128-bit
  			  data easier to handle
        GifPacket       : structure for generating packets 
			  to be transferred to Gif(GS)
        ObjData         : structure for managing object 
                          data in memory

        camera_p        : camera position
        camera_zd       : viewing direction vector for 
			  camera
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

        local_world     : matrix for transforming object's 
		          local coordinates to World 
			  coordinate system
        world_view      : matrix for transforming World 
			  coordinate system to View 
			  coordinate system
        view_screen     : matrix for transforming View 
			  coordinate system to Screen 
			  coordinate system
        local_screen    : matrix for transforming Local 
			  coordinates to Screen coordinates 
			  (product of the previous three 
			  matrices)

        normal_light    : matrix for getting the inner 
			  product of a vertex normal 
			  vector and a light direction 
			  vector in the World coordinate 
	 		  system (light direction vector 
			  is set)
        local_light     : matrix multiplied to normal_light
			  matrix and local_world matrix in 
			  order to obtain the inner product 
			  of the vertex normal vector and 
			  the light direction vector in 
			  the Local coordinate system
        light_color     : matrix for getting summation of 
			  products between light colors 
			  (three colors + ambient light) 
			  and inner product of local_light
			  matrix and vertex normal vector
        local_color     : matrix resulting from product of 
			  local_light and light_color

        work            : matrix providing temporary 
			  workspace

        scePadData      pad     : structure for reading 
				  pad data
        sceGsDBuff      db      : structure for double 
				  buffering
        sceGsLoadImage  pTex    : structure for 
				  transferring textures 
			          to the local memory in 
				  the GS
        sceDmaChan      *dmaVif : structure for DMA 
			          transfers of data
    	TexEnv          texenv  : structure for generating 
				  packets for transferring 
				  texture data

        LoadObj(ObjData *o, u_int *Head)
                        : function to take object data in 
			  the main RAM and load it into 
			  and manipulate it in the 
			  ObjData structure

        ReleaseObj(ObjData *o)
                        : function to release loaded object 
			  data

        MakePacket(ObjData *obj, int num)
                        : function for performing 
			  perspective transformations, 
		          light-source calculations, 
			  etc. on object data and 
			  generating packets to be 
			  transferred to the GS

* variables in function main

        i, j            : counters for for loops, etc.
        fd              : file descriptor for obtaining 
			  return value when pad is opened
        fr              : switch used for flipping double 
			  buffers
        delta           : variable for setting up rotation 
			  angle
        obj[2]          : variable for registering objects 
			  (cube & torus)
        obj_switch      : toggle switch for switching 
			  objects
        toggle          : variable for toggling the 
			  reading of the SELECT button

* Variables and functions supported in the mathfunc.c

        sceSamp0IVECTOR[4]      : int array (4 elements) 
				  aligned at 128-bit 
				  boundary
        sceSamp0FVECTOR[4]      : float array (4 elements) 
				  aligned at 128-bit
				  boundary
        sceSamp0FMATRIX[4][4]   : float array 
				  (4x4 elements) aligned at 
				  128-bit boundary

For a detailed description of functions, please refer to mathfunc.c. 
Almost all the functions from mathfunc.c are implemented using Vu0 macros and are provided through libvu0. The functions in the Core version are named sceSamp0******, while corresponding functions are named sceVu0****** in the VU0 version.

Please note that the array formats of the matrices are different in the Core version from the VU0/VU1 versions. The Core version uses standard arrays, but the VU0/VU1 versions are transposed to allow VU features to be used more effectively.


Description of program (initialization)
----------------------------------------------------------

* Load object data
    LoadObj(&obj[0], SampleCubeDataHead);
    LoadObj(&obj[1], SampleTorus1DataHead);

The object data used in the sample program is loaded from SampleCubeDataHead and SampleCubeDataHead into the ObjData structure.

* Initialize device
    sceGsResetPath();    // initialize data devices (VIF1,VU1,GIF)
    sceDmaReset(1); // initialize DMA (DMA)

All devices that are to be used are initialized at the start of the program. Unexpected behavior may result if uninitialized devices are used, so always remember to initialize.

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

Once the devices have been initialized, the GS drawing environment is set up. Displaying to the screen will not be possible unless the environment is also set up. This is also required for controller settings which are synchronized via VSync, so always remember to set up the environment right after devices have been initialized.

* Transfer texture images
    sceGsSetDefLoadImage(&pTex,13340,IMAGE_SIZE / 64,SCE_GS_PSMCT32,
                        0, 0,IMAGE_SIZE, IMAGE_SIZE);
        // set up LoadImage data
    FlushCache(0);
    sceGsExecLoadImage(&pTex, (u_long128 *)My_texture1);
        // execute LoadImage
    sceGsSyncPath(0, 0);// wait for data transfer to finish

LoadImage data is set up to transfer texture images to the local memory of the GS. Then, sceGsExecLoadImage() is executed to transfer the specified image to the local memory of the GS. This function uses DMA internally, so FlushCache(0) is performed to provide consistency between the D-cache and the main memory. Finally, sceGsSyncPath() is called to wait for completion of the data transfer.

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
    sceGsSyncPath(0, 0);// wait for completion of data transfer

Packets generated for setting up the texture environment are transferred using Normal DMA via PATH3.

* Initialize controller
    fd = open("pad:0", 0);

Initialization is performed for the 1P controller. Subsequent read() operations will load data into the specified buffer. The controller data will be updated at each VSync.


Program description (main loop)
----------------------------------------------------------

* Generate View-Screen matrix
    sceSamp0ViewScreenMatrix(view_screen, 512.0f, 1.0f, 0.47f,
                2048.0f, 2048.0f,1.0f, 16777215.0f,1.0f,65536.0f);

sceSamp0ViewScreenMatrix() is used to determine the View-Screen matrix.
The resulting View-Screen matrix assumes a distance between the view point and the projection screen of 512, a screen aspect ratio of 1:0.47, the center of the screen at (2048, 2048) in the GdPrimitive coordinate system, a Z-buffer value of 1.0-16777215.0, and a Z value of 1.0-65536.0.

In the Core version, initialization is performed outside the main loop.
During execution, this matrix is rarely modified.

* Get controller information
    // --- read pad ---
    read(fd, &pad, sizeof(pad));

    // --- object rotate & change view point ---
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

Controller button data can be obtained with read(). The data retrieved here is used to update the rotation angle and translation for both the object and the camera. The buttons on the left side of the controller controls the object and the right side controls the camera. The SELECT button switches between objects.

* Generate Local-World matrix (rotation only)
    sceSamp0UnitMatrix(work);                   // unit matrix
    sceSamp0RotMatrix(local_world, work, rot);  // rotation matrix

Local-World matrices must be determined for each object stored as Local coordinates. Affine transforms such as rotation and translation are used. In this sample program, a rotation matrix and a translation matrix are calculated, and the product of these is used as the Local-World matrix. However, in this sample, the Local_Light matrix must be set up before setting up the translation (obj_trans) in the third line. If a matrix to which translation has been applied is used to generate the
Local_Light matrix, the ambient light calculations will be inaccurate. This is because translation and ambient light are both calculated using the fourth row of the matrix. Thus, the Local_Light matrix must be created right after setting up the rotation matrix.

Also, the fourth element of obj_trans must always be set to zero.
The value entered in the [4,4] element is extremely critical, and a non-zero value can affect this.

* Generate local light matrix
    sceSamp0NormalLightMatrix(normal_light, light0,light1,light2);
    sceSamp0MulMatrix(local_light, normal_light,local_world);

sceSamp0NormalLightMatrix() is used to determine a Normal-Light matrix from the three light sources. This is then multiplied with the Local-World matrix to generate the Local-Light matrix. As described above, obj_trans must not be set for the Local_World matrix in this multiplication.

* Generate light color matrix
    sceSamp0LightColorMatrix(light_color, color0, color1, color2, ambient);

The Light_Color matrix is determined from the light-source color and the ambient color using sceSamp0LightColorMatrix(). Once the influencing colors from the light sources are determined from light-source calculations, the Light_Color matrix is multiplied and the final screen vertex color is determined.

* Generate World-View matrix
    sceSamp0RotCameraMatrix(world_view, camera_p, camera_zd, camera_yd,
                                                         camera_rot);

This sceSamp0RotCameraMatrix() function calls sceSamp0CameraMatrix() internally. The sceSamp0CameraMatrix() is used to determine the
World-View matrix. In the View coordinate system, the view point (eye) is set to the origin (0,0,0), the viewing direction is Z+, X+ is to the right, and Y+ is down (right-hand coordinate system). Here, the view point camera_p in World is (0,0,-25), the viewing direction camera_zd is (0,0,1), the vertical camera_yd is (0,1,0). The World-View matrix is generated based on these settings.

Within the sceSamp0RotCameraMatrix() function, camera rotation is performed by rotating these vectors beforehand using camera_rot, and passing the results to sceSamp0CameraMatrix().

* Generate Local-World matrix (rotation and translation)
    sceSamp0TransMatrix(local_world, local_world, obj_trans);
        // rotation & translation

Once the Local_Light matrix has been created, a translation component (obj_trans) is added to the Local_World matrix to generate a true Local_World matrix.

* Generate Local-Screen matrix
    sceSamp0MulMatrix(world_screen, view_screen, world_view);
    sceSamp0MulMatrix(local_screen, world_screen, local_world);

The View-Screen matrix and the World-View matrix determined above are used to first calculate a World-Screen matrix. A Local-Screen matrix is determined by multiplying the Local-World matrix.

The Local_World matrix multiplied here must have rotation and translation components correctly set.

* Generate packet
    MakePacket(&obj[obj_switch], i);

Packet generation is performed in MakePacket(). The obj parameter is a pointer to the ObjData structure for the object to be displayed. This can be switched to easily change the displayed object.

* Initialize packet (MakePacket())
    pack->size = 0;
    pack->buf = (QWdata *)0x70000000;//SPR

The packet write address is set to 0x70000000. This is the start of the scratchpad region so the size of the packet must not be greater than 16 KB, the scratchpad size. Note that if packets are generated in main memory, there are restrictions on the values that can be used for QWC in DMA transfers, so packet size should be kept less than 1 MB. The VU1 version of the sample program uses VUMem1 double buffering, so the torus data is split into blocks of about 4 KB to prevent overflows.

* Generate DMAtag, GIFtag (MakePacket())
    //DMAtag
    pack->buf[pack->size].ul128 = (u_long128)0;
    pack->buf[pack->size++].ui32[0] = 0x70000000|(obj->vertexNum[num]*3+1);
    //GIFtag
    pack->buf[pack->size].ul64[0]
        = SCE_GIF_SET_TAG(obj->vertexNum[num], 1, 1,obj->prim,
                                                SCE_GIF_PACKED,3);
    pack->buf[pack->size++].ul64[1] = 0x0412L;

DMAtag and GIFtag must be added to the start of the packet. Source Chain mode is used for DMA. The size of outgoing packets is vertex count x 3 (STQ,RGBA,XYZF2)+1(GIFtag). The DMAtag ID is end (0x70000000) (because there is only one DMA packet). PACKED mode (SCE_GIF_PACKED) is used for GIF, and primitive settings are made in the GIFtag (obj->prim). The REGS value is set to 0x412 (STQ, RGBA,XYZF2).

* Perspective transform (MakePacket())
    Q = sceSamp0RotTransPers(v01, local_screen, vertex[j], 1);
    sceSamp0ScaleVector(tex, texUV[j], Q); //for Perspective correction

The sceSamp0RotTransPers() function is used to perform perspective transformations. Detailed descriptions are provided in mathfunc.c. Output results (v01) are int arrays in a format that can be directly passed to the GS (GS coordinate system). The return value Q is type float, holding the reciprocal of the fourth element of v01. This is used for perspective corrections with texture coordinates (S, T, Q). texUV is set to texture UV coordinates (U, V, 1.0 ,0). These values are multiplied by Q to to determine STQ texture.

* Light-source calculations (MakePacket())
    sceSamp0NormalColorVector(c01, local_light, light_color,
                         normal[j], color[j]);

Vertex color after light-source calculations is determined using the vertex normal vector and color data, as well as light vector and color data. Results are returned in a format that can be directly passed to the GS. Internally, once the inner product of the light vector and the vertex normal vector is determined, the function performs clipping of the inner product in the range of 0.0 to 1.0. Furthermore, once the effects of three light sources are added, clipping is performed in the range of 0.0 - 255.0.

* Save to packet (MakePacket())
    pack->buf[pack->size++].ul128 = *((u_long128*)tex);
    pack->buf[pack->size++].ul128 = *((u_long128*)c01);
    pack->buf[pack->size++].ul128 = *((u_long128*)v01);

The determined values are placed in the packet in the following sequence: (S,T,Q) (R,G,B,A) (X,Y,Z,F).

* Transfer to the GS
    MakePacket(&obj, i);
    FlushCache(0);
    sceDmaSend(dmaVif, (u_long128*)((u_int)obj.pack[i].buf|0x80000000));
    sceGsSyncPath(0, 0);

Primitive packets generated with MakePacket() are transferred using Source Chain DMA via PATH3.
There is a maximum packet size, so this operation is repeated according to the number of divided blocks in the object.

* Switch drawing environment, wait for V-Blank
    if(fr&0x01)
        sceGsSetHalfOffset(&db.draw1,2048,2048,sceGsSyncV(0)^0x01);
    else
        sceGsSetHalfOffset(&db.draw0,2048,2048,sceGsSyncV(0)^0x01);

The environment is switched after each field, then the function waits for the start of the next V-Blank interval.
