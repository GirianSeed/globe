[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0	
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program demonstrating I/O block transfer (previously known as AutoDMA transfer)

<Description>

The SPU2's I/O block transfer feature is used to play straight PCM data from IOP memory.

The sample code also shows libsd (IOP library) remote execution from the EE using libsdr.

To play a wave file, specify the name of a PCM file for PCM_FILENAME and specify a file size for PCM_SIZE.

The wave file format is 16-bit, little-endian, signed straight PCM. The left channel and the right channel must be interleaved at 512-byte intervals. Sample code for an interleaving tool is provided at the end of this document.

libsd.irx and sdrdrv.irx must be running on the IOP. Load status is displayed with dsicons, so if the program doesn't work correctly, check to be sure that data is being loaded properly.

<Files>
	main.c	
	knot.int (/usr/local/sce/data/sound/wave)

<Execution>
	% make		: compile
	% make run	: run

The code can also be executed using the following method after compilation.
	% dsedb
	> run main.elf

The program is working properly if it plays back music in stereo.

<Controller operations>
	None

<Notes>
	None

<Sample code for interleaving>

/*
   WAVE interleave tool for SPU2 I/O Block Trans

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


