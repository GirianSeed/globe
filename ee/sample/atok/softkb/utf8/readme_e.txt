[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library libccc Version 1.1
                Copyright (C) 2003 Sony Computer Entertainment Inc.
                                                   All Rights Reserved
                                                             Dec. 2003

======================================================================
The character code conversion library (libccc) Version 1.1 that converts 
Shift JIS and Unicode is being released.

----------------------------------------------------------------------
Caution
----------------------------------------------------------------------
- Multithread is not considered.
- 65536 of current conversion table are necessary for each of JIStoUCS2
  and UCS2toJIS which amount to 256k bytes.
- JIS is equivalent to "JIS X 0213".
- unicode is equivalent to "ver1.1".

----------------------------------------------------------------------
Contents of this Package
----------------------------------------------------------------------
 utf8
  +-libccc
     |---doc
     |    +---libref.txt
     |---ilb
     |    +---libccc.ilb
     |---include
     |    +---libccc.h          Header file
     |---lib
     |    |---jis2ucs.bin
     |    |---libccc.a          Character code conversion library
     |    +---ucs2jis.bin
     |---modules
     |    +---libccc.erx        Character code conversion module
     |---sample                 Sample
     +---src                    Source file

-----------------------------------------------------------------------
Changes
-----------------------------------------------------------------------
Libccc library supports for ERX and adds a entry function.
 
-----------------------------------------------------------------------
Permission and Restrictions on Use
-----------------------------------------------------------------------
The permission, restrictions, etc. on using this software conform to
the contract concluded between your company and our company (Sony
Computer Entertainment Inc.)

-----------------------------------------------------------------------
Notes on Trademarks
-----------------------------------------------------------------------
"PlayStation" and "PlayStation" logos are registered trademarks of Sony
Computer Entertainment Inc.
All other company names and product names included in this software are 
trademarks and registered trademarks of their respective owners. 


