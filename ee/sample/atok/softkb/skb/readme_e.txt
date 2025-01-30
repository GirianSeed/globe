[SCE CONFIDENTIAL DOCUMENT]
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.5
                    Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                    All Rights Reserved


Software Keyboard Program Sample That Uses ATOK Library


<Dependency with the library>

- Structure of the program dependency with the library is as follows.


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

 This is a program that libfep.a operates libatok directly. 
 Source files are located under fep/.
 
 Others are programs of software keyboard for operating the ATOK 
 graphically.
  

< File structure >

	Makefile   : makefile	  Make libraries below
	fontwrap   : fontwrap.a	  Program related to libpfont
	resouce    : skbres.a     Software keyboard resource data
	fep        : libfep.a     Program related to software keyboard ATOK
	libsoftkb  : libsoftkb.a  Program related to running software
				  keyboard
	skb        : skb.a        Software keyboard control program


	-+---- fontwrap
	 |      +---- fontwrap.c  : libpfont wrapper function
	 |      +---- fontwrap.h  :
	 | 
	 +---- resource
	 |      +---- key Key layout file
	 |      |      +---- jp
	 |      |      |      +---- abc.bin     : Alphabet layout
	 |      |      |      +---- aiuHira.bin : Hiragana layout
	 |      |      |      +---- aiuKana.bin : Katakana layout
	 |      |      |      +---- kigo.bin    : Character list layout 
	 |      |      |      +---- num.bin     : Numeric layout
	 |      |      |      +---- qwerty.bin  : QWERTY (PC style) layout
	 |      |      +---- other
	 |      |             +---- dutchQ.bin   : Dutch PC layout
	 |      |             +---- cfrenchQ.bin : Canadian French PC layout
	 |      |             +---- frenchQ.bin  : French PC layout
	 |      |             +---- germanQ.bin  : German PC layout
	 |      |             +---- idiomChr.bin : Character list layout 
	 |	|				   with pronunciation 
	 |	|				   symbols
	 |      |             +---- italianQ.bin : Italian PC layout
	 |      |             +---- numF.bin     : Numeric layout
	 |      |             +---- portugQ.bin  : Portuguese PC layout
	 |      |             +---- spanishQ.bin : Spanish PC layout
	 |      |             +---- spSign.bin   : Sign list layout
	 |      |             +---- ukQ.bin      : UK PC layout
	 |      |             +---- usQ.bin      : US PC layout
	 |      +---- tex Texture files
	 |            +---- help Related to help contents
	 |            |      +---- skbHEng.tm2  : English
	 |            |      +---- skbHJp.tm2   : Japanese
	 |            |      +---- skbHFch.tm2  : French
	 |            |      +---- skbHSp.tm2   : Spanish
	 |            |      +---- skbHIta.tm2  : Italian
	 |            |      +---- skbHDch.tm2  : Dutch
	 |            |      +---- skbHPtg.tm2  : Portuguese
	 |            |      +---- skbHGmn.tm2  : German
	 |            |      +---- skbHItem.tm2 : Controller, etc.
	 |            +---- skb Software keyboard
	 |                   |
	 |                   +---- skbKey.tm2   : keytop character
	 |                   +---- skbBG.tm2    : Keyboard foundation
	 |                   +---- fepbar.tm2   : FEP state
	 |                   |                    (used with USB keyboard)
	 |                   +---- tField.tm2   : Text field
	 |                   +---- skbMdbar.tm2 : Mode bar (used with 
	 |		     |	 	  	  software keyboard)
	 |                   +---- skbBtn.tm2   : Key buttons and the 
	 |					  cursor
	 | 
	 +---- fep
	 |      +---- atok.inc   : ATOK key bind definition file
	 |      +---- ime.inc    : IME style key bind definition file
	 |      +---- fepskb.inc : Definition file for software keyboard
	 |      +---- fepkey.c   : Routine that processes the input 
	 |			   from USB keyboard
	 |      +---- fepskb.c   : Routine that processes the input from 
	 |			   software keyboard
	 |      +---- libfep.c   : Routine for using ATOK
	 |      +---- libfep.h   : Definition file for the routine for 
	 |			   using ATOK
	 |      +---- mtfifo.c   : FIFO
	 |      +---- mtfifo.h   : Definition file for FIFO
	 |      +---- util.c     : Utility routine
	 |                         (Hiragana -> Half size katakana, etc)
	 +---- libsoftkb
	 |      +---- help.inc    : Help definition file
	 |      +---- modebar.inc : Mode bar definition file
	 |      +---- libsoftkb.c : Software keyboard interface routine
	 |      +---- libsoftkb.h : Definition file for Software keyboard 
	 |			    interface routine
	 |      +---- sharedskb.h : Software keyboard internal definition
	 |      +---- skb_core.c  : Software keyboard core function group
	 |      +---- skb_list.c  : Software keyboard (list) function group
	 | 
	 +---- skb
	 |      +---- skb.c       : Software keyboard control sample
	 |      +---- draw.c      : Packet generation function
	 |      +---- fepbar.c    : Entry mode display funtion
	 |      +---- fepbar.h    :
	 |      +---- fepbar.inc  : Defines input mode texture UV value, etc.
	 |      +---- sysenv.c    : System configuration retrieval function
	 |      +---- sysenv.h    :
	 |      +---- textField.c : Text field function
	 |      +---- textField.h :
	 |      +---- tim2.c      : tim2 operation file
	 |      +---- tim2.h      :
	 |      +---- tim2wrap.c  : tim2.c wrap function
	 |      +---- tim2wrap.h  :
	 | 
	 +---- include            : Folder stores created library header 
	 |      +---- extypes.c   : Extension type definition
	 |      +---- shareddef.h : Software keyboard common definition
	 | 
	 +---- lib     : Folder stores created libraries


	< Keytop configuration file *.bin >
	- File format
	  The data is divided and structured roughly as follows.

	+----------------------------------------+
	| Header                                 |
	+----------------------------------------+
	| --- Sprite                             |
	+----------------------------------------+
	| --- Texture coordinate                 |
	+----------------------------------------+
	| --- Background                         |
	+----------------------------------------+
	| --- Key layout                         |
	+----------------------------------------+

	<Header : Structure lump_t>
		The header has offset and sizes from the beginning of the 
		files to the following information.
	
		Sprite information
		Texture UV information
		Background information
		Key layout information	


	<Sprite: Structure sprite_t>
		Cluster of sprite definitions.

	<Texture coordinate: Structure uv_t>
		Cluster of texture coordinate definitions

	<Background: Background structure >

	<Key layout: Structure button_t>
		Cluster of key information definitions


< Making library >

	% make clean  : Clean
	% make        : Make library

	<Change of make environment>

	In makefile, the following environment variables are configured 
	respectively.

	 ---------------------------------------------------------------------
	  RESOURCE_SOURCE = SKBRES_LINK        # Links as a library
	  RESOURCE_SOURCE = SKBRES_LOAD        # File load
	 ---------------------------------------------------------------------
	 - Enables you to select either read or link the data such as 
	   the texture and the key layout used with the software keyboard.

	 ---------------------------------------------------------------------
	  CDBOOT = 0                            # 0;host 1: cdrom    
	 ---------------------------------------------------------------------
	 - Enables you to select a path to read file from; CD disc or 
	   the host.

	 ---------------------------------------------------------------------
	  LINK_RESOURCE = REGION_LINK_ALL      # Use all
	  LINK_RESOURCE = REGION_J_ONLY        # Only Japanese resource 
						 is valid
	  LINK_RESOURCE = REGION_J_OTHER_ONLY  # Only overseas version 
						 of resource is valid
	 ---------------------------------------------------------------------
	- Enables you to select a resource that links or load 
	  (either is valid).
	  Whichever the argument of makefile specifies will be valid.
	  
	  Please note that if the resource is not linked or an attempt
	  was made to operate the resource, the program does not operate 
	  correctly.

	 ---------------------------------------------------------------------
	  LINK_ATOK = ATOK_LINK_USE           # Use ATOK library  
	  LINK_ATOK = ATOK_NOT_LINK           # Does not use ATOK library
	 ---------------------------------------------------------------------
	 - Enables you to select either use or not use ATOK library link.

	If any change has been made to the above environment variables 
  	in the makefile, be sure to execute the following command and 
	rebuild the library.

	% make define  : After make clean, common.def file is output
	% make 


< Screen parts description >

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

	<textField>
	Characters entered via software keyboard or USB keyboard are 
	output in the field.
	

	<Entry mode bar fepbar>
	Japanese version : ATOK entry mode display + Software keyboard 
			   display button

	Overseas version : Software keyboard display button

	<When software keyboard is activated>
        Entry mode list + The current selected entry mode

	<Software keyboard KBD >
	Activating the program indicates the text field, software 
	keyboard, and entry mode bar as shown above.
	
	Note:
	The last configuration configured by the user reads 
	__system/conf/system.ini of the hard disk drive, which may not 
	display the software keyboard correctly depending on the 
	configuration.
	
	Whilst using software keyboard, USB keyboard is used, then it 
	is recognized that USB keyboard is being used to input 
	characters and the software keyboard disappears from the 
	screen.
	
	However, if you wish to use the software keyboard again, on top
	right corner of screen, the software keyboard icon is located
	besides the entry mode bar (Japanese version). Check if the 
	cursor is right on this icon and press the circle button on the 
	controller to display the software keyboard again.


< Operation description >

	<When software keyboard is activated>
	SELECT button   : Entry mode switch

	- Entering Japanese
	   Circle button 	: Enter characters/confirm partially
	   Triangle button 	: Space/convert
	   Square button 	: Delete
	   Eks button 		: Cancel
	   Up, down, right and left 
	   directional buttons	: Move characters (move phrase and 
				  extend phrase)
	   SELECT button	: Switch entry mode (switch keytops)
	   START button		: Confirm all
	   L1 button		: (None)
	   L2 button 		: Move caret (to left)
	   R1 button 		: Shift
	   R2 button 		: Move caret (to right)

	- During conversion
	   Circle button 	: Confirm the entry
	   Eks button 		: Return to entry mode
	   Triangle button 	: Display conversion choices
	   Left and right directional 
	   buttons   		: Move phrase (extend selected phrase)
	   R1+Left or right directional 
	   buttons		: Increase/decrease phrase
	  
	- Whilst selecting choices
	   Circle button 	: Confirm the entry
	   Eks button 		: Return to entry mode
	   Triangle button 	: Next choice
	   Up and down directional 
	   buttons		: Select choice
	   Left and right directional 
	   buttons		: Select choice and move phrase

	- Direct entry
	   Circle button 	: Input characters
	   Triangle button 	: Space
	   Square button 	: Delete
	   Eks button 		: Cancel
	   Up, down, right and left 
	   directional buttons	: Move character
	   START button 	: Confirm the entry
	   L1 button		: (None)
	   L2 button 		: Move caret (to left)
	   R1 button 		: Shift
	   R2 button 		: Move caret (to right)

	<When USB keyboard is activated>
	   Circle button : Display software keyboard
	   Input via USB keyboard.

	<Switch entry mode>
	Pressing F12 or SELECT button switches fepbar display as 
	follows according to the mode being used. 
	Pressing [half/full size] as reguraly done on PC switches between
	kanji entry and direct entry (toggle) mode.

		Hiragana entry                	[Ç†](blue) ATOK/ON
		Full size katakana entry        [ÉA](blue) ATOK/ON
		Half size katakana entry        [_±](blue) ATOK/ON
		Full size alphabet entry        [Ç`](blue) ATOK/ON
		Half size alphabet entry        [_A](blue) ATOK/ON
		Direct entry                    [_A]       ATOK/OFF
		Hiragana fixed entry            [Ç†]       ATOK/ON
		Full size katakana fixed entry  [ÉA]       ATOK/ON
		Half size katakana fixed entry  [_±]       ATOK/ON
		Full size alphabet fixed entry  [Ç`]       ATOK/ON
		Half size alphabet fixed entry  [_A]       ATOK/ON

	  
	 < Key bind >
	  Differs depending on the system configuration.
	  The last used state is stored in the value.
	  Configuration 1 : IME style (default)
	  Configuration 2: ATOK style

	 < Alphabet entry and kana entry >
	  Differs depending on the system configuration.
	  The last used state is stored in the value.
	  
	 < AltGr European language-oriented keyboard >
	  Normally, on European language-oriented keyboard, AltGr key
	  exists on the bottom right instead of Alt key.


< Program flow >

	According to the entered information, the program processes 
	character code entry, executes ATOK commands, retrieves the 
	state from the ATOK, then displays the data. 

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
skb/  |      |        Text box          /fontwrap      Display strings
 +------------+    +---------------+  +--------------++--------------+ 
 |  skb.c     |----|  textField    |--|  fontwrap.c  ||  libpfont.a  | 
 +------------+    +---------------+  +--------------++--------------+ 
  | | | 
  | | | Implement text field, software keyboard, and FEP.
  | | | Transfer message from pad to software keyboard.
  | | | Message on USB keyboard is transferred directly to FEP.
  | | | 
  | | |                      +===============================================+
  | | | Controller info. ->  #    libsoftkb/           Entry via on-screen   #
  | | |			     #			       keyboard              #
  | | +----------------------+-->--+-------------+      +--------------+     #
  | |                        #     |             |--<>--|  skb_core.c  |     #
  | V Entry via USB keyboard #     |             |      +--------------+     #
  | |                        #     | libsoftkb.c |--<>--|  skb_list.c  |     #
  | |   +--------------------+--<--|             |      +--------------+     #
  | |   | <- Character code  #     +-------------+       Char list entry     #
  | |   |    & control code  #                                               #
  | |   |                    #                      +========================+
  | |   |                    #                      #
  | V +=V====================+ Interprets key code  #  +=====================+
  | | # |  fep/                and converts to      #  #                     #
  | | # |                      ATOK command         #  #  ATOK library       #
  | | # | +----------+        +----------------+    #  #  +------------+     #
  | | # +-+          |---<>---|    fepkey.c    |-->-+--+--| libatok.a  |     #
  | +-+---+  fep.c   |        +----------------+    #  #  |            |     #
  |   #   |          |---<>---|    fepskb.c    |-->-+--+--|            |     #
  +-- +---|          |        +----------------+    #  #  |            |     #
      #   |          |---<-----------------------------+--|            |     #
      #   +----------+     <<Confirmed string/      #  #  |            |     #
      #                      converted string       #  #  +------------+     #
      #   - When ATOK/ON, confirmed strings are sent#  #                     #
      #	    from FEP to libatok.a via fepkey.c,     #  +========|============+
      #     fepskb.c. In this case, key bind is     #           |       
      #     attached.                               #           |
      #   - When ATOK/OFF, confirmed strings are    #           X       
      #     sent from FEP directly to skb.c         #           | HDD access
      #                                             #    +-------------+
      #                                             #    |   pfs.irx   |
      +=============================================+    +-------------+


< External interface >

	Configure the followings to compiler option -I$ search path.

	    utf8/libccc/include
	    libpfont/include
	    include 

	    Link lib/*.a 

	Also, include the following files.
	    #include <sys/types.h>
	    #include <eetypes.h>

	    #include <libccc.h>
	    #include <libpfont.h>
	    #include <libfep.h>
	    #include <libsoftkb.h>
	    #include <skb.h>


	<Procedure for using software keyboard control sample>

	  Init_SKBConfig(SKB_ARG*arg, char *fsName, short keyType, 
	  short repWait, short repSpeed ); // Environment initialization
	    |
	    v
	  SKB_Init(void);     // Software keyboard initialization
	    |
	    v
	  SKB_SetInhibit_MaskChar( u_int (*ucs4)[10], int count );
		   // Characters entered via keyboard (ÅJÅKetc.) are 
		      registered even if they are inhibit input characters. 
	  SKB_SetInhibit_Enable( u_int (*ucs4)[10], int count ); 
		   // Register inhibit input characters
	    |
	    v
	  SKB_Open( KEYBOARD_SET* kbSet, u_int editType, u_int* usc4 ); 
		 // Open software keyboard
	    ^      |
	    |      v
	         SKB_SetUsbKeyborad( int kbStat, USBKBDATA_t* ,int numLock ); 
	    ^	 // Retrieve usb key information
	    |     |
		  v
	    ^    SKB_Control( u_long ulPad );
	    |	 // Transfer and control pad
	    ^     |
	    |     v
	         SKB_MakeVif1CallPacket( int clipOffX, int clipOffY );
		 // Make drawing packet 
	    ^      |
	    |      v
	    SKB_Close( char* );
	    // Close software keyboard
	       |
	       v
	  SKB_Exit(void);      // Exit software keyboard


< Note >

  Details of system configuration are as follows.

	<System software system configuration>
	<Keyboard>
	- Keyboard type (DEF= English keyboard)
		Japanese keyboard <--> English keyboard

	- Time until repeat starts (DEF= Standard)
		Short <--> Standard <--> Long

	- Speed of key repeat (DEF= Standard)
		Slow <--> Standard <--> Fast

	<Mouse>
	- Mouse (DEF= For right-handed person)
		Right-handed <--> Left-handed

	- Mouse cursor speed (DEF= Standard)
		Slow <--> Standard <--> Fast

	<Japanese entry>
	- Japanese entry (DEF= Roman character entry)
		Roman character entry <--> Kana entry

	- Key bind (DEF= Configuration 1)
		Configuration 1 <--> Configuration 2

	<SOFTKB> *The state is stored by user.
		  This sample does not perform storing.

	- Software keyboard ON/OFF(DEF= ON)
		ON <--> OFF
		(software keyboard use <--> USB keyboard use)

	- Type of alphabet layout keyboard (DEF= QWERTY)
		Alphabet layout <--> QWERTY


< Character code >

	- ATOK                    SJIS
	- FEP                     UCS2 (u_int type)
	- Software keyboard       UCS2 (u_int type)
	- Text field	  	  UCS2 (u_int type)
	- FONT                    UTF8

< Cautions >

	- Please note that this program contains Shift JIS strings in 
	  the source, which may be affected by the environment variable, 
	  LANG.

	  Before using this program, specify Shift JIS in the LANG 
	  environment variable.
	  
	  For .bashrc: 
	  $ export LANG=C-SJIS

	  For .cshrc:
	  $ setenv LANG C-SJIS 


