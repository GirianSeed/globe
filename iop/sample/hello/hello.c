/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/* 
 *              I/O Processor Library Sample Program
 * 
 *                         - hello -
 * 
 * 
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 * 
 *                            hello.c
 * 
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.4.0            
 */

#include <stdio.h>
#include <kernel.h>

int main(int argc, char *argv[])
{
    int i;

    printf("Hello ! \n");
    for( i = 0; i < argc ; i ++ )
	printf("  argv[%d] = %s\n",i, argv[i]);
    return 0;
}
