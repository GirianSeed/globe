[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
        Copyright (C) 1999 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Sample program showing how to perform key frame matrix animation by rotation axis interpolation

<Description>

This program shows how to perform key frame animation for a joint matrix using a method known as rotation axis interpolation.

Rotation axis interpolation is one of the methods for interpolating matrices.  It has the following features.

*  Since interpolation can be performed by using a clean axis, the number of key frames can easily be reduced.
*  The amount of parameter data that must be maintained is relatively small.
*  The relationships between parameters and actual physical quantities are easy to understand and easy to handle.

<Files>

	mkdata/
	This directory is used to calculate rotation axis interpolation parameters from the original matrix key frame sequence.

	runtime/
	This directory is used to perform the actual key frame matrix animation by using the parameters that were created in the mkdata/ directory.

<Execution>
	Refer to the documents in each directory.

<Controller operation>
	Refer to the documents in each directory.

<Notes>

[Introduction to rotation axis interpolation]

Rotation axis interpolation is a method that uses matrix rotation to interpolate between several matrices.  The method of interpolating between two matrices is explained below.

Simple linear interpolation is performed separately for the translation part.  Only the rotation part will be explained here.

Interpolation is performed between MA, which is the joint matrix for a given frame, and MB, which is the joint matrix after n frames. At this time, we can consider that "MB is a matrix obtained by rotating MA." If we let the rotation axis be represented by <alpha> and the rotation angle by <theta>, the interpolated matrices will be as follows:

	Frame 0: MA
	Frame 1: Matrix obtained by rotating MA by 
		 <theta>/n around rotation axis <alpha>
	Frame 2: Matrix obtained by rotating MA by 
		 2*(<theta>/n) around rotation axis <alpha>
	Frame 3: Matrix obtained by rotating MA by 
		 3*(<theta>/n) around rotation axis <alpha>
		  :         :
	Frame k: Matrix obtained by rotating MA by 
		 k*(<theta>/n) around rotation axis <alpha>
		  :         :
	Frame n: Matrix obtained by rotating MA by 
		 n*(<theta>/n) around rotation axis <alpha>
	
[Maintaining data]

The data required for runtime is as follows.
	Rotation axis <alpha>:  3-parameter vector 
				(ax,ay,az) Since this 
				determines the rotation 
				axis direction, a 12-bit 
				integer for each component 
				is sufficient.
	Rotation angle (<theta>/n):  Although it will also 
				     depend on the data, 
                                     this generally is 
				     float data
	Number of interpolation frames n:  Integer 
					   corresponding 
					   to the longest 
					   interpolation 
					   frame count
	Translation amount (T/n):  3-parameter vector 
				   (tx,ty,tz) This 
				   generally is float data

Although dependent on the actual data, generally a total of 128 bits is sufficient, consisting of 64 bits for the rotation angle (<theta>/n) and translation amount (T/n) per key frame, and 64 bits for the rotation axis <alpha> and number of interpolation frames n.

[Other applications]

Besides simple key frame animation, rotation axis interpolation can also be used to:

- change the rotation axis direction according to conditions, or
- interpolate between two rotation axes and generate an animation that passes through that interpolation from several animations.


[Calculation method for rotation axis interpolation]

1. Method of obtaining rotation axis <alpha> and rotation angle <theta>

If we let MB=MR MA, then the eigenvector of MR will be the rotation axis <alpha>.  MR is obtained simply as MR=MB MAt.  For information about how to obtain the eigenvector, see the function EigenVector() within mkdata/matrix.c or refer to commercially available books or publications related to matrix arithmetic.

2.  Method of obtaining the matrix for rotating by <theta>/n around rotation axis <alpha> If we let <alpha>=(x,y,z), s=sin(<theta>/n), and c=cos(<theta>/n), then the rotation portion of the matrix Mr that rotates by the angle r around the rotation axis <alpha> can be calculated as follows.

{[ (1-c) x^2 + c,   (1-c) x y + z s, (1-c) x z - y s ],
 [ (1-c) x y - z s, (1-c) y^2 + c,   (1-c) y z + x s ],
 [ (1-c) x z + y s, (1-c) y z - x s, (1-c) z^2 + c ]}

Also refer to the function a2m() within runtime/axis.c.
