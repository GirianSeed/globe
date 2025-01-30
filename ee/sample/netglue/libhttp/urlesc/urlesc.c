/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                         - urlesc -
 *                        Version 1.0.0
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         <urlesc.c>
 *                      <main functions>
 *
 *       Version   Date           Design              Log
 *  --------------------------------------------------------------------
 *       1.0.0     Oct,22,2001    komaki              first version
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <eeregs.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libhttp.h>

#define ERR_STOP	while(1)

int main(int argc, char **argv){
	int escape = 0;
	char *data;

	if (argc < 2){
usage:
		scePrintf("usage: run urlesc.elf (-escape|-unescape) \"string\"\n");
		ERR_STOP;
	}

	if (strcmp(argv[1], "-escape") == 0)
		escape = 1;
	else if (strcmp(argv[1], "-unescape") == 0)
		escape = 0;
	else
		goto usage;

	if (escape)
		data = sceURLEscape(argv[2]);
	else
		data = sceURLUnescape(argv[2]);
	scePrintf("%s\n", data);
	free(data);

	return 0;
}
