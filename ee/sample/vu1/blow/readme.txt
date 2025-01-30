[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
    Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Particle drawing using VU1

<Description>

This sample program shows how to use VU1 to draw particles from three-dimensional coordinates while adding motion blur.

<Files>
        blow.c      :  Main program
        data.dsm    :  Particle and particle reflection data 
		       packets
        draw.vsm    :  Microcode for drawing ground
        firebit.dsm :  Particle texture
        fireref.dsm :  Particle reflection texture
        grid.dsm    :  Ground data packets
        p.vsm       :  Microcode for drawing particles
        p2.vsm      :  Particle reflection
        part.dat    :  Particle data string
        part1.dsm   :  part.dat with attached VIFtag
        part10.dsm  :  Collection of 10 part1.dsm objects
        spart.dat   :  Particle reflection data string
        spart1.dsm  :  spart.dat with attached VIFtag
        spart10.dsm :  Collection of 10 spart1.dsm objects
        src.dsm     :  Particle velocity vector and color 
		       storage area
        srcpart.dat :  Data strings included in src.dsm
        wood.dsm    :  Ground texture

<Startup>
	% make :  Compile
	% make run :  Execute

The following method can also be used to execute the program after compilation.
	% dsedb
	> run blow.elf

<Controller operation>
        Direction arrow keys:  Move viewpoint
        <circle> button:  Stop particles

<Remarks>

Processing steps are briefly described below.

[blow.c]
1. Set double buffer and load texture.
2. Initialize parameters and particles.
3. Set perspective transformation matrix and normal offset vector to line of sight (for particle expansion) in packet.
4. Draw ground.
5. Calculate coordinates of particle and particle reflection (SetParticlePosition).
6. Draw particle reflection.
7. Draw particle.
8. Return to 3.

The following processing is performed within VU1 microcode.

[draw.vsm]
1. Set rotation x perspective transformation matrix.
2. Load various types of parameters (such as matrices) in VU1 registers.
3. Load normal vector, vertex coordinates, ST value, and color.
4. Perform perspective transformation and light source calculations and store results in VU1Mem.
5. Return to 3. and repeat for number of vertices.
6. XGKICK stored data (Gif Packet) to GS.

[p.vsm]
1. Load various types of parameters (such as offset, perspective transformation matrix, and ST) in VU1 registers.
2. Load coordinates, velocity vector, and color.
3. Move coordinates according to velocity vector (for motion blurring).
4. Perform perspective transformation of two points corresponding to coordinates obtained in 3. and coordinates to which offset was added.
5. Create quadrilateral (triangle strip due to 4 points) having line segment connecting two points of 4. as diagonal line.
6. Store result of 5. and corresponding ST and color in VU1Mem.
7. Return to 2. and repeat for number of particles.
8. XGKICK stored data (Gif Packet) to GS.

[p2.vsm]
p2.vsm is the result when motion blur has been removed from p.vsm.  Double buffering is also performed in VU1Mem.
	
Note
Optimization has not been performed for p.vsm and p2.vsm to improve readability.

