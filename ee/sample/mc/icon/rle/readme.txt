[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                      All Rights Reserved


[Texture compression program for 3D Memory Card icons]

Run-length encoding can be applied to textures used in 3D icon files that have been saved to a Memory Card. This program performs this run-length encoding.

Usage: rle [-x] <input> <output>

The file input is compressed and written to output.
Adding -x uncompresses the file specified by input and writes the result to output.

The textures used for icons are PSMCT16 with a high occurrence of two-byte continuity. Thus, run-length encoding is performed in two-byte units. In some cases compression will not work on an image and the final size will be greater than the original. In such cases, use textures without compression.

As an example, textures and objects can be combined into a single icon file by following the steps below.

1. Create an icon file aaa.ico with no texture using only the model section and the animation section. Set parameters for model section attributes (TEX=1, RLE=1).
2. Prepare the texture as raw PSMCT16 data (tex.raw).
3. Compress using 'rle tex.raw tex.rle'
4. Create a single file using 'cat aaa.ico tex.rle > ICON.ico'.
