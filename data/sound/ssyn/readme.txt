[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
                  Copyright (C) 2000 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

Phoneme Data for Software Synthesizer (Evaluation version)

<Description of the Sample>

	This is a sample program for the phoneme data(SSB) for the CSL
	Software Synthesizer (libssyn).  This is an evaluation version
	and the complete version is planned to be released in the future.

	Tone array is GM compatible with an SC88 style apart from the
	specification method for the drum part.  With the GM, the 
	channel #10 acts as the drum part.  On top of that, the drum part
	can be specified exclusively in the GS.  There is no concept of
	drum part in the CSL Software Synthesizer and the drum set is
	allocated to #120 of BankChange(MSB), thus it is required to
	set BankChange (MSB)120 to the channel which is to be used as
	a drum part.

	The feature enabling any part of BankChange(MSB) to be converted
	to the arbitrary value for making a conversion from SMF to SQ in
	JAM is available.  However, it does not add the BankChange(MSB)
	specification to a truck, therefore, the conversion is not
	carried out if the BankChange is not included.
	
	The SSB editor, that is planned to be embedded in the next
	releasing tool (JAM), is designed to create individual SSB files.
	The existing data can be edited so unnecessary data can be
	deleted to compact the data.

	This phoneme data is free for use as long as it is used for the
	PlayStation2 title otherwise it is prohibited.

<File>
	sce.ssb

<Note>
	None


