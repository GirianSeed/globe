/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/*
 *                     I/O Processor Library 
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        stdio.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.4.0
 */

#ifndef	_STDIO_H
#define	_STDIO_H
#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

extern int  open(char *filename, int flag);
extern int  close(int fd);
extern long lseek(int fd, long offset, int whence);
extern int  read(int fd, void *buf, long nbyte);
extern int  write(int fd, void *buf, long nbyte);
extern int  ioctl(int fd, long request, long arg);

extern int  printf(const char *fmt, ...);
extern int  getchar();
extern int  putchar(int c);
extern int  puts(char *s);
extern char *gets(char *s);

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	/* _STDIO_H */


