/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4
 */
/* 
 *                Controller Vibration Library
 *                          Version 2.01
 *                           Shift-JIS
 *
 *         Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         libvib - libvib.h
 *                     header file of libvib
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      2.01          2001-03-09        nozomu      the first version
 *
 */

#ifndef _LIBVIB_H_
#define _LIBVIB_H_

int sceVibGetProfile( int, unsigned char* );
int sceVibSetActParam( int, int, unsigned char*, int, unsigned char* );

#endif
