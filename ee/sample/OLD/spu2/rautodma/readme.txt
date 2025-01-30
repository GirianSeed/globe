[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 1.6
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program illustrating AutoDMA transfer

<Description of sample program>

This sample program generates sounds via straight PCM input using the AutoDMA transfer feature of the librspu2 provisional sound library. To playback a waveform, enter a name of a PCM file for PCM_FILENAME and a file size in PCM_SIZE. The format of the waveform must be 16-bit, little-endian, signed straight PCM with the left and right channels interleaved at 512-byte intervals. A sample interleaving tool is included at the end of this document. An AutoDMA transfer API that does not require interleaved data is scheduled for a future release.

<File>
	main.c	
	knot.int (/usr/local/sce/data/sound/wave)

<Execution>
	% make		: compile
	% make run	: run

	The sample program can also be run by 
	entering the following after compilation:

	% dsedb
	> run main.elf

	The program is functioning properly if 
	music is played back in stereo.

<Controller operations>
	none

<Notes>
	none

<Sample interleaving tool>

/*
   WAVE interleave tool for SPU2 AutoDMA

   usage: raw2int <src_left> <src_right> <dst>
 */

#include <stdio.h>
#include <sys/types.h>
#include <string.h>

FILE *fsrc[2];
FILE *fdst;
FILE *ferr;

#define FILE_NAME_LENGTH 1024
char src_name[2][FILE_NAME_LENGTH];
char dst_name[FILE_NAME_LENGTH];
char buff[512];

void convert(void)
{
	int count;

	while( 1 )
	{
		count = fread( buff, 1, 512, fsrc[0] );
		if (count < 512)
		    break;
		fwrite( buff, 1, 512, fdst );
		count = fread( buff, 1, 512, fsrc[1] );
		fwrite( buff, 1, count, fdst );
		if(count < 512) break;
	}
	return;
}

int main ( int argc,  char *argv[] )
{
	short i;

	if( argc < 4){
		printf("usage: raw2int <src_left> <src_right> <dst>\n");
		return -1;
	}
		
	for( i = 0; i < 32; i++ ) {
		src_name[0][i] = 0;
		src_name[1][i] = 0;
	}
	for (i = 0; i < 2; i++) {
	    sprintf( src_name[i], "%s", argv[i+1] );
	    if( (fsrc[i] = fopen( src_name[i], "rb")) == NULL ){
		fprintf( stderr, "Can't open output file:%s\n", src_name[i] );
		return -1;
	    }
	}

	sprintf( dst_name, argv[3] );
	if( (fdst = fopen( dst_name, "wb" )) == NULL ) {
		fprintf( stderr, "Can't open output file:%s\n", dst_name );
		return -1;
	}

	printf( "Processing: %s + %s -> %s ..." 
		, src_name[0], src_name[1], dst_name );

	convert();

	if( fsrc[0] != 0 ) fclose( fsrc[0] );
	if( fsrc[1] != 0 ) fclose( fsrc[1] );
	if( fdst != 0 ) fclose( fdst );
	printf( "done\n" );

	return 0;
}


