[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved


[ASCII/binary conversion program for 3D icon files]

This tool converts 3D icon files, which are in binary format, to ASCII. Files converted to ASCII can also be converted back into binary. Thus, the ASCII output from a 3D authoring tool can be converted to binary.

iconconv [-b] <input> <output>

The binary icon file given as input will be written as output. Adding -b will convert an input ASCII file to binary and will write the result to output.

ASCII/binary conversion of textures is currently not supported. After editing the ASCII and converting to binary, use the following commands to add a texture to a file.

iconconv -b ascii.ico binary.ico
cat binary.ico tex.raw > icon.ico