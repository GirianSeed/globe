/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/*
 *                     I/O Processor Library 
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        string.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.4.0
 */

#ifndef	_STRING_H
#define	_STRING_H

#define LMAX 256

#ifndef _TYPES_H
#include <sys/types.h>
#endif
#include <memory.h>

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif
extern char *strcat (char *, char *);
extern char *strncat(char *, char *, int);
extern int   strcmp (/* char *, char * */);	/* To avoid conflicting */
extern int   strncmp(char *, char *, int);
extern char *strcpy (/* char *, char * */);	/* To avoid conflicting */
extern char *strncpy(char *, char *, int);
extern int   strlen (/* char * */);		/* To avoid conflicting */
extern char *index  (char *, char);
extern char *rindex (char *, char);

extern char *strchr (char *, char);
extern char *strrchr(char *, char);
extern char *strpbrk(char *, char *);
extern int   strspn (char *, char *);
extern int   strcspn(char *, char *);
extern char *strtok (char *, char *);
extern char *strstr (char *, char *);
#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#define strdup(p)	( strcpy(malloc(strlen(p)+1),p); )

#endif	/* _STRING_H */

