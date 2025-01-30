[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
                  Copyright (C) 2003 Sony Computer Entertainment Inc.
                                                  All Rights Reserved
                                                            Dec. 2003

======================================================================
Basic Font Library (libpfont) Version 1.2 has been released.

This library enables you to input font data and string, and output 
drawing packets.

----------------------------------------------------------------------
Precautions
----------------------------------------------------------------------
Character colors can be set freely by using scePFontSetColor() function. 
However, CLUT with default setting may not change its color properly 
and the color may be uneven, caused by its special feature for black 
frame and also depending on the color set by using the function. 

The supported character code in this program; Unicode (UTF-8) allows 
you to convert into Unicode (UTF-8) with the use of libccc, 
the character code conversion library, which is downloadable 
from [Other Common Tools] on the SCE-NET, the developers support 
website. 

----------------------------------------------------------------------
Contents of this package
----------------------------------------------------------------------
libpfont
  |---doc(html,txt)             Documents
  |    |---pfont
  |    |---pfont_rf
  |    +---fontformat
  |---include
  |    +---libpfont.h           Header file
  |---lib
  |    +---libpfont.a           Basic Font Library
  |---modules
  |    +---libpfont.erx         Basic Font Module
  |---ilb
  |    +---libpfont.ilb         
  |---tool                      Data Creation Tool
  |---sample                    Sample
  |    |---basic
  |    +---advanced
  +---src                       Source file

-----------------------------------------------------------------------
Changes from the Previous Version
-----------------------------------------------------------------------
< Library >
- Libpfont library supports for ERX and adds a entry function.

-----------------------------------------------------------------------
Usage rights and restrictions
-----------------------------------------------------------------------
All rights and restrictions regarding the use of this software are
according to the contract concluded between your company and Sony
Computer Entertainment Inc.

-----------------------------------------------------------------------
Written notice related to trademarks 
-----------------------------------------------------------------------
"PlayStation" and PlayStation logos are registered trademarks of Sony
Computer Entertainment Inc.  Product names that are mentioned in this
software are trademarks and registered trademarks of their respective
owners.

-----------------------------------------------------------------------
Copyrights
-----------------------------------------------------------------------
Sample Font Data: (C)2002 Sony Computer Entertainment Inc.
These sample font data ("Data") and any materials created from the Data 
may be used for products of each licensee, provided that the Data and 
the materials created from the Data should be used for PlayStation 2 
application only. 
Unauthorized modification, assignment, public transmission (including 
making it transmittable), or lending of the Data, is prohibited.  
No reference to this license is required on its products.
