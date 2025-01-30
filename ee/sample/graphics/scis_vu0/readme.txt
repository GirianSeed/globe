[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
        Copyright (C) 1999 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Scissoring sample program (CORE and VU0 versions)

<Description>

This program shows how to use CORE and VU0 macro mode to cut polygons at clipping boundaries and to generate new polygons that do not protrude from boundaries.

The program handles both Triangles and Triangle Strips.

The yellow lines on the screen indicate the clipping area and can be controlled using the controller. This area is generally set to (x,y)=(+/-2048) and the polygons are adjusted so that they do not protrude from the clipping area. Also, the region in which Z-buffering is valid (near plane and far plane along the z axis) is defined so the polygons do not protrude from this area as well.

Objects cut by the near plane will appear as cross-sections.

In general, scissoring is used to prevent polygons near the viewpoint from being displayed in an unnatural manner.

Examples include ground, roads and walls.


<Files>

        main.c          : main program
        main.h          : prototype declarations for main 
			  program
        defines.h       : prototype declarations used in 
			  the .c files
        cube.s          : object data (cube)
        torus1.s        : object data (donut)
        tex128.dsm      : texture data
        remake.c        : function group for automatically 
			  generating polygons clipped at 
			  clipping planes

<Execution>

        % make          : compile
        % make run      : run

The program can also be run in the following manner after compiling.
        % dsedb
        > run main.elf

<Controller functions>

        up arrow/down arrow (direction keys)      : rotate around x axis 
				    (viewpoint)
        left arrow/right arrow    : rotate around y axis 
				    (viewpoint)
        L1/L2 buttons                 : move along z axis 
				    (viewpoint, forward/
				     back)
        triangle/x buttons           : change clipping area 
				    (y direction)
        square/circle buttons           : change clipping area 
				    (x direction)
        R1/R2 buttons                    : change clipping area 
				    (xy direction)
        SELECT button             : switch models (cube, 
				    torus)
        START button              : switch scissoring 
				    polygon display 
                                    (TriangleFan or 
				     LineStrip)

<Notes>

Warning

1. The following issue must be taken into account when the nearZ and farZ boundary planes in the view volume are associated with Zbuffer values.

nearZ and farZ plane calculations are usually performed using floating-point operations. However, since Zbuffer values are represented using integers, they will be converted to integers by instructions such as FTOI. This operation will generally be performed such that the values obtained after conversion to integer will be sufficiently close to the expected Z values. However, there are some cases where this might not be the case.

For example, consider a 24-bit Zbuffer where the value of a point on the nearZ plane (e.g., Z=1.0) will be 16777215.0 (16777215.0->FTOI->0xffffff). If the value is represented as a 32-bit floating-point number, the mantissa will be 23 bits. Thus, the number of reliable significant digits will be only six or seven. As a result, it is quite possible for calculation errors to take place so that the 1.0->16777215.0 operation will produce a value greater than 16777215.0. This will result in an incorrect Zbuffer value of zero.

In order to avoid this problem, steps must be taken such as sacrificing the Zbuffer width slightly by using 16777000.0 as the value corresponding to the nearZ plane.

With programs like this sample, where the nearZ plane is critical, this issue relating to Z value calculation precision in Zbuffer operations should be carefully considered.

2. When calculating cross-sections, the intersection points between the clipping plane and the polygon sides must be determined. This involves interpolation of vertex and other information (vertex, normal, color, textureUV). In these cases, special care must be taken for normal interpolation.

When displaying a polygon using Gouraud shading, the color at a point on a side is determined through "linear interpolation of the colors at the vertices," which are the end points. If lighting color is determined by simply performing linear interpolation on the normal, the result will be different.

To overcome this problem, linear interpolation must be performed on the color to be passed to the GS after calculating the lighting for the vertices. In other words, the elements needed for interpolation are: vertex, GScolor, and textureUV.


Overview of methods

New polygons are generated and rendered using TriangleFans using the assumption that polygons formed from cutting are convex polygons.

Triangular polygons have three sides. When these are cut by a plane, they intersect on two sides. A new polygon can then be created by connecting these two intersecting points.

Next, this polygon is cut using a new plane. This will also result in two intersecting points, and these can similarly be connected to form a new polygon.

By recursively performing cutting operations in this manner, a polygon can ultimately be generated that will fit within the necessary area (in the clipping plane group).

In this program, cutting operations are performed for each of six clipping planes (+x,-x, +y,-y, +z,-z) so that a final scissoring polygon is generated.

(Reference) Computer Graphics --principles and practice--
                  SECOND EDITION
        authors:  James Foley, Andries van Dam,
                         Steven Feiner, John Hughes
         publisher: ADDISON-WESLEY PUBLISHING COMPANY 
             
         Section 3.14 Clipping Polygons (pp124-127)


The following is a summary of the operations performed.

===== main.c =====
main():

1. Set up double buffers, load textures, open pad.
2. Read information from pad.
3. Generate GS transfer packet from object data (with clipping).
4. Perform scissoring on clipped polygons.
5. Generate packets for scissoring polygons and transfer to GS.
6. Transfer packets of clipped object data to GS.
7. Return to step 2.


Description of functions  

InitNodeArraySet():
Function for initializing the NodeArraySet structure, which handles vertex arrays prepared for TriangleFan.
        
InitScissorPlane();
Initializes the ScissorPlaneArray, which holds information on clipping planes.

ResetNodeArraySet();
Function for initializing vertex array structures used in recursive scissoring.

FlipNodeArray();
Function for flipping TriangleFan vertex arrays to allow efficient use of double-buffering.

PushTriangleNodeArray();
Function to register and save three-sided polygon information to be passed on for scissoring operations.
        
ClipCheck();
When finding cross-sections during scissoring operations, this function checks clipping between connected vertices in a TriangleFan and sees if they straddle a clipping plane.

InterpNode();
After a cross-section is found, this function determines an intersection point between a plane and a point to generate a new connecting vertex.

ScissorTriangle();
Function to perform scissoring operations on a three-sided polygon.

DrawScissorPolygon();
Function to perform perspective transform on a scissored polygon (TriangleFan) and generate and transfer a GS transfer packet.

MakePacket();
Performs perspective transform with clipping of standard object data, and generates GS transfer packet.
The clipped three-sided polygons is passed on for scissoring operations.
