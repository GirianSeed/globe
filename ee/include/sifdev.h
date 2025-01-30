/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/*
 *                      Emotion Engine Library
 *                          Version 0.1.0
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         sifdev.h
 *                         develop library
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.1.0         
 */

#ifndef _SIFDEV_H_DEFS
#define _SIFDEV_H_DEFS

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

/* pad data struct */
typedef struct {
	unsigned int cnt; /* updata count*/
	unsigned int status; /* pad status */
	unsigned int button; /* 16 buttons */
	unsigned char r3h;
	unsigned char r3v;
	unsigned char l3h;
	unsigned char l3v;
	unsigned int button2; /* 16 buttons */
	unsigned char r3h2;
	unsigned char r3v2;
	unsigned char l3h2;
	unsigned char l3v2;
} scePadData;

/* pad status */
#define SCE_PAD_NORMAL 0x1
#define SCE_PAD_ANALOG 0x2

/* keys config */
#define SCE_PADLup     (1<<12)
#define SCE_PADLdown   (1<<14)
#define SCE_PADLleft   (1<<15)
#define SCE_PADLright  (1<<13)
#define SCE_PADRup     (1<< 4)
#define SCE_PADRdown   (1<< 6)
#define SCE_PADRleft   (1<< 7)
#define SCE_PADRright  (1<< 5)
#define SCE_PADi       (1<< 9)
#define SCE_PADj       (1<<10)
#define SCE_PADk       (1<< 8)
#define SCE_PADl       (1<< 3)
#define SCE_PADm       (1<< 1)
#define SCE_PADn       (1<< 2)
#define SCE_PADo       (1<< 0)
#define SCE_PADh       (1<<11)
#define SCE_PADL1      SCE_PADn
#define SCE_PADL2      SCE_PADo
#define SCE_PADR1      SCE_PADl
#define SCE_PADR2      SCE_PADm
#define SCE_PADstart   SCE_PADh
#define SCE_PADselect  SCE_PADk

/* Flag for sceOpen() */
#define SCE_RDONLY      0x0001
#define SCE_WRONLY      0x0002
#define SCE_RDWR        0x0003
#define SCE_NBLOCK      0x0010  /* Non-Blocking I/O */
#define SCE_APPEND      0x0100  /* append (writes guaranteed at the end) */
#define SCE_CREAT       0x0200  /* open with file create */
#define SCE_TRUNC       0x0400  /* open with truncation */
#define SCE_NOBUF       0x4000  /* no device buffer and console interrupt */
#define SCE_NOWAIT      0x8000  /* asyncronous i/o */

/* SCE local usage */
#define SCE_USING       0x10000000  /* io descript usage bit */
#define SCE_NOWBDC      0x20000000  /* not write back d cashe */

/* Seek Code */
#define SCE_SEEK_SET	0
#define SCE_SEEK_CUR	1
#define SCE_SEEK_END	2

/* Ioctl Code */
#define SCE_FS_EXECUTING	0x1

#define SCE_PAD_ADDRESS		0x1

/* Error codes */
#define	SCE_ENXIO	6	/* No such device or address */
#define	SCE_EBADF	9	/* Bad file number */
#define	SCE_ENODEV	19	/* No such device */
#define	SCE_EINVAL	22	/* Invalid argument */
#define	SCE_EMFILE	24	/* Too many open files */

typedef struct {
  unsigned int epc;
  unsigned int gp;
  unsigned int sp;
  unsigned int dummy;  
} sceExecData;

int  sceOpen(char *filename, int flag);
int  sceClose(int fd);
int  sceRead(int fd, void *buf, int nbyte);
int  sceWrite(int fd, void *buf, int nbyte);
int  sceLseek(int fd, int offset, int whence);
int  sceIoctl(int fd, int req, void *);

void sceSifInitIopHeap(void);
void *sceSifAllocIopHeap(int);
int sceSifFreeIopHeap(void *);
int sceSifLoadIopHeap(char *, void *);

/* extern int errno; */

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif /* _SIFDEV_H_DEFS */

/* End of File */
