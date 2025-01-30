/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/*
 *                     I/O Processor Library 
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        loadcore.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.4.0
 */

#ifndef _LOADCORE_H
#define _LOADCORE_H

#define RESIDENT_END	0
#define NO_RESIDENT_END	1
#define FAREWELL_END	1

typedef struct _libhead {
    struct _libhead *next;
    struct _libcaller *client;
    unsigned short  version;
    unsigned short  flags;
    char	    name[8];
} libhead ;

#define LIBMAGIC	(0x41e00000)
typedef struct _libcaller {
    unsigned long   magic;
    struct _libcaller  *client;
    unsigned short  version;
    unsigned short  flags;
    char	    name[8];
} libcaller;

typedef struct _moduleinfo {
    char		*name;
    unsigned short	version;
} ModuleInfo;

void FlushIcache();
void FlushDcache();

int RegisterLibraryEntries(libhead *lib);
int ReleaseLibraryEntries(libhead *lib);

typedef struct bootparam {
    unsigned short	value;
    unsigned char	key;
    unsigned char	extwords;
    unsigned long	eword[0];
} BootParam;

BootParam *QueryBootMode(int key);
#define QueryBootParam(key)	QueryBootMode(key)

/* key */
#define KEY_DECItype		1
#define KEY_EEbootparam		2
#define KEY_IOPbootparam	3

/* IOPbootparam */
#define IBP_no_sif_init		(1<<0)
#define IBP_no_iop_service	(1<<1)
#define IBP_no_auto_load	(1<<2)

#endif /* _LOADCORE_H */
