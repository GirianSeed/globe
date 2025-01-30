[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2002 Sony Computer Entertainment Inc.
                                        All Rights Reserved



Software Keyboard Sample Programs that use the ATOK Library


<Library dependencies>

- Dependency relationships between the libraries are shown below.


     |                                                      |
     +---------------------------------------------+        |
     |  skb.a                                      |        |
     |                                             |        |
     |                                             |        |
     +------------------------------------------------------+
     |  libfep.a    + libsoftkb.a + skbres.a | fontwrap.a   |
     |              |         +---+          |              |
     |              |         |              |              |
     +------------------------------------------------------+
     | libatok.a    |libccc.a |              | libpfont.a   | 
     +------------------------+              +--------------+

  libfep.a is a sample program that directly controls libatok.
  The source files are in the fep/ folder.

  The others are software keyboard sample programs that graphically control ATOK.



<File structure>

	Makefile	:  Make file  Creates the following libraries.
	fontwrap	:  fontwrap.a  libpfont-related program
	resource	:  skbres.a  Software keyboard resource data
	fep		:  libfep.a  Software keyboard ATOK-related program
	libsoftkb	:  libsoftkb.a  Software keyboard operation-related program
	skb		:  skb.a  Software keyboard control program


	-+---- fontwrap
	 |      +---- fontwrap.c: libpfont wrapper functions
	 |      +---- fontwrap.h:
	 | 
	 +---- resource
	 |      +---- Key layout files
	 |      |      +---- jp
	 |      |      |      +---- abc.bin	:  ABC layout
	 |      |      |      +---- aiuHira.bin	:  aiueo hiragana layout
	 |      |      |      +---- aiuKana.bin	:  aiueo katakana layout
	 |      |      |      +---- kigo.bin	:  Character summary layout
	 |      |      |      +---- num.bin	:  Digit layout
	 |      |      |      +---- qwerty.bin	:  QWERTY (PC style) layout
	 |      |      +---- other
	 |      |             +---- dutchQ.bin	:  Dutch PC-style layout
	 |      |             +---- cfrenchQ.bin:  Canadian French PC-style 
         |      |             |                    layout
	 |      |             +---- frenchQ.bin	:  French PC-style layout
	 |      |             +---- germanQ.bin	:  German PC-style layout
	 |      |             +---- idiomChr.bin:  Character summary layout 
         |      |             |                    with pronunciation symbols
	 |      |             +---- italianQ.bin:  Italian PC-style layout
	 |      |             +---- numF.bin	:  Digit layout
	 |      |             +---- portugQ.bin	:  Portuguese PC-style layout
	 |      |             +---- spanishQ.bin:  Spanish PC-style layout
	 |      |             +---- spSign.bin	:  Symbol summary layout
	 |      |             +---- ukQ.bin	:  United Kingdom PC-style layout
	 |      |             +---- usQ.bin	:  United States PC-style layout
	 |      +---- tex  Texture files
	 |            +---- help Help-related
	 |            |      +---- skbHEng.tm2	:  English
	 |            |      +---- skbHJp.tm2	:  Japanese
	 |            |      +---- skbHFch.tm2	:  French
	 |            |      +---- skbHSp.tm2	:  Spanish
	 |            |      +---- skbHIta.tm2	:  Italian
	 |            |      +---- skbHDch.tm2	:  Dutch
	 |            |      +---- skbHPtg.tm2	:  Portuguese
	 |            |      +---- skbHGmn.tm2	:  German
	 |            |      +---- skbHItem.tm2	:  Controller, etc.
	 |            +---- skb  Software keyboard
         |                   |
	 |                   +---- skbKey.tm2	:  Keytop characters
	 |                   +---- skbBG.tm2	:  Keyboard base
	 |                   +---- fepbar.tm2	:  FEP state (used by USB 
         |                   |                     keyboard)
	 |                   +---- tField.tm2	:  Text field
	 |                   +---- skbMdbar.tm2	:  Mode bar (used by software 
         |                   |                     keyboard)
	 |                   +---- skbBtn.tm2	:  Key buttons, cursor, etc.
   	 | 
	 +---- fep
	 |      +---- atok.inc	:  ATOK key binding definition file
	 |      +---- ime.inc	:  IME-style key binding definition file
	 |      +---- fepskb.inc:  Software keyboard definition file
	 |      +---- fepkey.c	:  Routine for processing input from USB keyboard
	 |      +---- fepskb.c	:  Routine for processing input from software
         |      |                  keyboard
	 |      +---- libfep.c	:  Routine for using ATOK
	 |      +---- libfep.h	:  Definition file for routine for using ATOK
	 |      +---- mtfifo.c	:  FIFO
	 |      +---- mtfifo.h	:  FIFO definition file
	 |      +---- util.c	:  Utility routines (hiragana -> half-width 
         |                         katakana, etc.)
	 +---- libsoftkb
	 |      +---- help.inc		:  Help definition file
	 |      +---- modebar.inc	:  Mode bar definition file
	 |      +---- libsoftkb.c	:  Software keyboard interface routine
	 |      +---- libsoftkb.h	:  Software keyboard interface routine 
         |      |                          definition file
	 |      +---- sharedskb.h	:  Software keyboard internal definitions
	 |      +---- skb_core.c	:  Software keyboard core function group
	 |      +---- skb_list.c	:  Software keyboard list function group
	 | 
	 +---- skb
	 |      +---- skb.c		:  Software keyboard control sample
	 |      +---- draw.c		:  Packet creation functions
	 |      +---- fepbar.c		:  Input mode display functions
	 |      +---- fepbar.h		:  
	 |      +---- fepbar.inc	:  Definition of input mode texture UV value, etc.
	 |      +---- sysenv.c		:  System configuration acquisition functions
	 |      +---- sysenv.h		:  
	 |      +---- textField.c	:  Text field functions
	 |      +---- textField.h	:  
	 |      +---- tim2.c		:  tim2 control file
	 |      +---- tim2.h		:  
	 |      +---- tim2wrap.c	:  tim2.c wrapper functions
	 |      +---- tim2wrap.h	:  
	 | 
	 +---- include			:  Folder that contains generated library headers
	 |      +---- extypes.c		:  Type extension definitions
	 |      +---- shareddef.h	:  Software keyboard common definitions
         |
	 +---- lib	:  Folder that contains generated libraries


	<Keytop configuration file *.bin >
	- File format
	  The data is broadly divided as shown in the following data structure.

	+----------------------------------------+
	| Headers                                |
	+----------------------------------------+
	| --- sprites                            |
	+----------------------------------------+
	| --- texture coordinates                |
	+----------------------------------------+
	| --- backgrounds                        |
	+----------------------------------------+
	| --- key layouts                        |
	+----------------------------------------+

	<Header	:  Structure lump_t>
	This structure has offsets and sizes from the beginning of the file to the following: 
            sprite information
            texture UV information
            background information
            key layout information


	<Sprite	:  Structure sprite_t>
		This structure is a collection of sprite definitions.

	<Texture coordinates	:  Structure uv_t>
		This structure is a collection of texture coordinate definitions.

	<Background	:  Background structure>

	<Key layout	:  Structure button_t>
		This structure is a collection of key information definitions.

<Creating the library>

	% make clean	:  Clean
	% make		:  Create library

	<Changing the Make environment>

	 The following environment variables are set in the Makefile.

	 ---------------------------------------------------------------------
	  RESOURCE_SOURCE = SKBRES_LINK        # Link as library
	  RESOURCE_SOURCE = SKBRES_LOAD        # Load file
	 ---------------------------------------------------------------------
	 - These select whether textures and key layout data that will be used by the software keyboard will be linked or read in.

	 ---------------------------------------------------------------------
	  CDBOOT = 0                            # 0: host; 1: cdrom    
	 ---------------------------------------------------------------------
	 - This selects whether the file load path will be from a CD disc or the host.

	 ---------------------------------------------------------------------
	  LINK_RESOURCE = REGION_LINK_ALL      # Use all resources
	  LINK_RESOURCE = REGION_J_ONLY        # Only Japanese-version resources are valid
	  LINK_RESOURCE = REGION_J_OTHER_ONLY  # Only overseas-version resources are valid
	 ---------------------------------------------------------------------
	 - These select the resources to be linked or loaded (which ones are valid).
	   They become valid when specifed as an argument of the Makefile.
	   Note that if an attempt is made to manipulate resources that have not been linked, abnormal program operation will result.

	 ---------------------------------------------------------------------
	  LINK_ATOK = ATOK_LINK_USE           # Use ATOK library.
	  LINK_ATOK = ATOK_NOT_LINK           # Do not use ATOK library.
	 ---------------------------------------------------------------------
	 - These select whether the ATOK library is to be linked.


	If you change any of the foregoing environment variables in the Makefile, be sure to execute the following commands to rebuild the library.

	% make define	:  Output common.def file after executing make clean
	% make 


<Screen description>

     +--------------------------------------------------------+
     | textField                                              |
     +--------------------------------------------------------+
     | KBD                                       |    +--------+
     |                                           |    | fepbar |
     |                                           |    +--------+
     |                                           |
     |                                           |
     |                                           |
     |                                           | +----------+
     +-------------------------------------------+ | modebar  |
                                                   +----------+

	<Text field  textField>
	Outputs characters entered from software keyboard or USB keyboard.

	<Input mode bar  fepbar>
	Japanese version:  ATOK input mode display + software keyboard display buttons

	Overseas version:  Software keyboard display buttons

	<When software keyboard is used>
        Input mode list + currently selected input mode

	<Software keyboard  KBD>
	When the program is started up, the text field, software keyboard, and input mode bar described above are displayed.

	Note:
	Since the user's last settings are read from __system/conf/system.ini on the hard disk drive, there may be times when the software keyboard is not displayed, depending on those settings.

	If a key on the USB keyboard is pressed during software keyboard input, it is considered to have been entered from the USB keyboard, and the software keyboard will disappear from the screen.
	If you want to enter data from the software keyboard again at this time, since the cursor is aligned on top of the software keyboard display button on the right side of the input mode bar (in the Japanese version), you only need to press the Circle button on the controller to display the software keyboard.

<Description of operations>
	<When the software keyboard is used>
	SELECT button:  Switch input mode

	- Japanese input
	Circle button:  Enter character, or confirm part
	Triangle button:  Space or convert
	Square button:  Delete
	Cross button:  Cancel
	Up/Down/Left/Right (direction keys):  Move character (move phrase, expand or contract phrase)
	SELECT button:  Switch input mode (switch keytops)
	START button:  Confirm all
	L1 button:  (None)
	L2 button:  Move caret (left)
	R1 button:  Shift
	R2 button:  Move caret (right)

	- During conversion
	   Circle button:  Decide
	   Cross button:  Return to input
	   Triangle button:  Display conversion candidates
	   Left/Right (direction keys):  Move clause (change in-focus clause)
	   R1 + Left/Right (direction keys):  Expand or contract clause
	  
	- During candidate selection
	   Circle button:  Decide
	   Cross button:  Return to input
	   Triangle button:  Next candidate
	   Up/Down (direction keys):  Select candidate
	   Left/Right (direction keys):  Select candidate or move clause.


	- Direct input
	Circle button:  Input character
	Triangle button:  Space
	Square button:  Delete
	Cross button:  Cancel
	Up/Down/Left/Right (direction keys):  Move character
	SELECT button:  Switch input mode (switch keytops)
	START button:  Decide
	L1 button:  (None)
	L2 button:  Move caret (left)
	R1 button:  Shift
	R2 button:  Move caret (right)

	<When USB keyboard is used>
	Circle button:  Display software keyboard

        USB keyboard input.

	<Switch input mode>
	By pressing the F12 key or SELECT button, the fepbar display will change according to the mode in use as follows:
	Hiragana input				(blue) ATOK/ON
	Full-width katakana input		(blue) ATOK/ON
	Half-width katakana input		(blue) ATOK/ON
	Full-width alphabetic input		(blue) ATOK/ON
	Half-width alphabetic input		(blue) ATOK/ON
	Direct input				       ATOK/OFF
	Hiragana fixed input			       ATOK/ON
	Full-width katakana fixed input		       ATOK/ON
	Half-width katakana fixed input		       ATOK/ON
	Full-width alphabetic fixed input	       ATOK/ON
	Half-width alphabetic fixed input	       ATOK/ON

	Kanji input/direct input (toggle) mode is enabled with normal [half-width/full width].

	<Key bindings>
	Key bindings are determined by a system setting.
	The value saved is the last state that was used.
	Setting 1:  IME style (default)
	Setting 2:  ATOK style 

	<Alphabet input or kana input>
	These are determined by a system setting.
	The value saved is the last state that was used.

	<AltGr European-type keyboard>
	On a normal European-type keyboard, an AltGr key replaces the Alt key on the right side.


<Program flow>

The character code is input from the input information, ATOK command operations are performed, and the state is acquired from ATOK and displayed.


 +-----------+  +-----------+ 
 | libpad.a  |  |libusbkb.a | 
 +----+------+  +-----+-----+ 
 |   pad.c   |  |   kbd.c   | 
 +-----------+  +-----------+ 
      |               |  
      |---------------+     Character code conversion
 +------------+             +------------+
 |   main.c   |-----+---<>--|  libccc.a  |
 +------------+     |       +------------+
      |      +------+                
skb/  |      |        Text box         /fontwrap      String display. 
 +------------+    +---------------+  +--------------++--------------+ 
 |  skb.c     |----|  textField    |--|  fontwrap.c  ||  libpfont.a  | 
 +------------+    +---------------+  +--------------++--------------+     
  | | | 
  | | | Text field, software keyboard, FEP implementation.
  | | | Messages from the pad are sent to the software keyboard.
  | | | Messages from the USB keyboard are sent directly to the FEP.
  | | | 
  | | |                      +===============================================+
  | | | Controller       ->  #    libsoftkb/           Screen keyboard input #
  | | | information          #                                               #
  | | +----------------------+-->--+-------------+      +--------------+     #
  | |                        #     |             |--<>--|  skb_core.c  |     #
  | V USB keyboard input     #     |             |      +--------------+     #
  | |                        #     | libsoftkb.c |--<>--|  skb_list.c  |     #
  | |   +--------------------+--<--|             |      +--------------+     #
  | |   | <- Character codes #     +-------------+      Character list input #
  | |   |    & control codes #                                               #
  | |   |                    #                      +========================+
  | |   |                    #                      #
  | V +=V====================+ Interpret key code   #  +=====================+
  | | # |                      and convert to       #  #                     #
  | | # |  fep/                ATOK command         #  #  ATOK library       #
  | | # | +----------+        +----------------+    #  #  +------------+     #
  | | # +-+          |---<>---|    fepkey.c    |-->-+--+--| libatok.a  |     #
  | +-+---+  fep.c   |        +----------------+    #  #  |            |     #
  |   #   |          |---<>---|    fepskb.c    |-->-+--+--|            |     #
  +-- +---|          |        +----------------+    #  #  |            |     #
      #   |          |---<-----------------------------+--|            |     #
      #   +----------+     <<Decision string/       #  #  +------------+     #
      #                      conversion string>>    #  #                     #
      #  - When ATOK is On, the converted           #  +========|============+
      #    string is sent from the FEP to           #           |
      #    libatok.a via fepkey.c or fepskb.c.      #           X       
      #    In this case, key bindings are attached. #           | HDD access
      #  - When ATOK is Off, the converted          #    +-------------+
      #    string is sent from the FEP directly     #    |   pfs.irx   |
      #    to skb.c.                                #    +-------------+
      +=============================================+    




<External interface>
      Set the following as the search path for the -I$ compiler option.

	    utf8/libccc/include
	    libpfont/include
	    include 

	    Link lib/*.a.

	The following files should also be included.
	    #include <sys/types.h>
	    #include <eetypes.h>

	    #include <libccc.h>
	    #include <libpfont.h>
	    #include <libfep.h>
	    #include <libsoftkb.h>
	    #include <skb.h>


	<Sample usage procedure for controlling the software keyboard>

	  Init_SKBConfig(SKB_ARG*arg, char *fsName, short keyType, 
	  short repWait, short repSpeed ); // Initialize environment
	    V
	  SKB_Init(void);     // Initialize software keyboard
	    V
	  SKB_SetInhibit_MaskChar( u_int (*ucs4)[10], int count );     
		    // Register input-prohibited characters that can be input from the keyboard
	  SKB_SetInhibit_Enable( u_int (*ucs4)[10], int count );       
		   // Register input-prohibited characters
	    V
	  SKB_Open( KEYBOARD_SET* kbSet, u_int editType, u_int* usc4 ); 
		 // Open software keyboard
	    ^       V
	    ^   SKB_SetUsbKeyboard( int kbStat, USBKBDATA_t* ,int numLock ); 
		 // Get usb key information
	    ^       V
	    ^    SKB_Control( u_long ulPad );                                 
		 // Pass pad, control
	    ^       V
	    ^    SKB_MakeVif1CallPacket( int clipOffX, int clipOffY );
		 // Create drawing packet
	    ^       V
	    SKB_Close( char* );                                           
	    // Close software keyboard
	       V
	  SKB_Exit(void);      // Exit software keyboard



<Remarks>
Detailed information about system settings follows.

	<System software system settings>
	<Keyboard>
	- Keyboard type (DEF= English keyboard)
		Japanese keyboard <--> English keyboard

	- Interval until repeat begins (DEF= standard)
		Short <--> Standard <--> Long

	- Repeat speed (DEF= standard)
		Slow <--> Standard <--> Fast

	<Mouse>
	- Mouse (DEF= For right-handed use)
		Right handed <--> Left handed

	- Mouse cursor speed (DEF= standard)
		Slow <--> Standard <--> Fast

	<Japanese input>
	- Japanese input (DEF= romaji input)
		romaji input <--> kana input

	- Key bindings (DEF= setting 1)
		setting 1 <--> setting 2

	<SOFTKB> (*) The state is saved according to user operation.
		   This sample does not perform any save operation.
	- Software keyboard ON/OFF (DEF= ON)
		ON <--> OFF
		(software keyboard use <--> USB keyboard use)

	- Alphabet layout keyboard type (DEF= QWERTY)
		ABC layout <--> QWERTY

<About character codes>
	- ATOK                  SJIS
	- FEP                   UCS2 (type is u_int)
	- Software keyboard	UCS2 (type is u_int)
	- Text field		UCS2 (type is u_int)
	- FONT			UTF8

<Precautions>
	- Note that this program may be affected by the environment variable LANG because the source code contains Shift JIS strings.

	  When this program is used, specify Shift JIS for the environment variable LANG.

	 For .bashrc:
	 $ export LANG=C-SJIS

	 For .cshrc:
	 $ setenv LANG C-SJIS
