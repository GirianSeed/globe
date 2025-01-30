/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
// libaio.c - Asynchronous ps2 file I/O wrapper functions
//
// Copyright (C) 2001 Sony Computer Entertainment Inc., All Rights Reserved.
//
// Version        Date            Design       Log
// --------------------------------------------------------------------
// 0.00           May,07,2001     Koji Tashiro 1st version

#define	asm	__asm__
#define INLINE	__inline__
#define UNUSED	__attribute__((unused))

#include <stdio.h>
#include <string.h>
#include <eekernel.h>
#include <sifdev.h>
#include "libaio.h"

#define STACK_SIZE	8192		// worker thread stack size

typedef enum {
    AIOC_NONE,
    AIOC_OPEN,
    AIOC_CLOSE,
    AIOC_READ,
    AIOC_WRITE,
    AIOC_LSEEK,
    AIOC_LSEEK64,
    AIOC_CHSTAT,
    AIOC_GETSTAT,
    AIOC_RENAME,
    AIOC_REMOVE,
    AIOC_READLINK,
    AIOC_SYMLINK,
    AIOC_CHDIR,
    AIOC_MKDIR,
    AIOC_RMDIR,
    AIOC_DOPEN,
    AIOC_DCLOSE,
    AIOC_DREAD,
    AIOC_SYNC,
    AIOC_FORMAT,
    AIOC_MOUNT,
    AIOC_UMOUNT,
    AIOC_DEVCTL,
    AIOC_IOCTL2,
} AioCmdType;

typedef struct {
    char	name0[256];		// buffer for pass filename
    char	name1[256];
    char	buf[4096];		// other common buffer
    int		a[8];			// integer argument list
    long	al;			// long argument
    AioCmdType	ccmd;			// current command
    AioCmdType	lcmd;			// last    command
    int		result;
} Aio __attribute__((aligned(64)));


static Aio aio;
static int tid;				// thread id for worker thread
static int ssid;			// semaphore for start command
static int esid;			// semaphore for end   command


static int RegisterCmd(AioCmdType cmd) {// register command exclusively
    int r = 0;
    DI();
    if (aio.ccmd) {			// executing another command
	r = -aio.ccmd;
	printf("aio: overlapped operation %d, while %d\n", cmd, aio.ccmd);
    } else {
	aio.ccmd = cmd;			// register command
    }
    EI();
    return r;
}


// mode 0: block mode
// mode 1: non-block mode
int AioCmdSync(int mode, int *cmd, int *result) {
    int r;
    switch (mode) {
	case 0:
	    WaitSema(esid);		// block until command completion
	    if (cmd) *cmd = aio.lcmd;
	    if (result) *result = aio.result;
	    return 1;

	case 1:
	    r = PollSema(esid);		// check running
	    if (cmd) {
		if (aio.ccmd) *cmd = aio.ccmd;
		else          *cmd = aio.lcmd;
	    }
	    if (result) *result = aio.result;
	    if (r == -1) return 0;	// still running
	    else         return 1;
    }
    return -1;
}


int AioOpen(const char *filename, int flag, u_int mode) {
    int r;
    if ((r = RegisterCmd(AIOC_OPEN)) < 0) return r;
    strcpy(aio.name0, filename);
    aio.a[0] = flag;
    aio.a[1] = (int)mode;
    SignalSema(ssid);
    return 0;
}

int AioClose(int fd) {
    int r;
    if ((r = RegisterCmd(AIOC_CLOSE)) < 0) return r;
    aio.a[0] = fd;
    SignalSema(ssid);
    return 0;
}

int AioRead(int fd, void *buf, int len) {
    int r;
    if ((r = RegisterCmd(AIOC_READ)) < 0) return r;
    aio.a[0] = fd;
    aio.a[1] = (int)buf;
    aio.a[2] = len;
    SignalSema(ssid);
    return 0;
}

int AioWrite(int fd, const void *buf, int len) {
    int r;
    if ((r = RegisterCmd(AIOC_WRITE)) < 0) return r;
    aio.a[0] = fd;
    aio.a[1] = (int)buf;
    aio.a[2] = len;
    SignalSema(ssid);
    return 0;
}

int AioLseek(int fd, int offset, int how) {
    int r;
    if ((r = RegisterCmd(AIOC_LSEEK)) < 0) return r;
    aio.a[0] = fd;
    aio.a[1] = offset;
    aio.a[2] = how;
    SignalSema(ssid);
    return 0;
}

int AioLseek64(int fd, long offset, int how) {
    int r;
    if ((r = RegisterCmd(AIOC_LSEEK64)) < 0) return r;
    aio.a[0] = fd;
    aio.al   = offset;
    aio.a[1] = how;
    SignalSema(ssid);
    return 0;
}

int AioChstat(const char *filename, const struct sce_stat *stat, u_int cbit) {
    int r;
    if ((r = RegisterCmd(AIOC_CHSTAT)) < 0) return r;
    strcpy(aio.name0, filename);
    memcpy(aio.buf, (const void *)stat, sizeof(struct sce_stat));
    aio.a[0] = (int)aio.buf;
    aio.a[1] = (int)cbit;
    SignalSema(ssid);
    return 0;
}

int AioGetstat(const char *filename, struct sce_stat *buf) {
    int r;
    if ((r = RegisterCmd(AIOC_GETSTAT)) < 0) return r;
    strcpy(aio.name0, filename);
    aio.a[0] = (int)buf;
    SignalSema(ssid);
    return 0;
}

int AioRename(const char *filename, const char *newname) {
    int r;
    if ((r = RegisterCmd(AIOC_RENAME)) < 0) return r;
    strcpy(aio.name0, filename);
    strcpy(aio.name1, newname);
    SignalSema(ssid);
    return 0;
}

int AioRemove(const char *filename) {
    int r;
    if ((r = RegisterCmd(AIOC_REMOVE)) < 0) return r;
    strcpy(aio.name0, filename);
    SignalSema(ssid);
    return 0;
}

int AioReadlink(const char *filename, void *buf, u_int len) {
    int r;
    if ((r = RegisterCmd(AIOC_READLINK)) < 0) return r;
    strcpy(aio.name0, filename);
    aio.a[0] = (int)buf;
    aio.a[0] = len;
    SignalSema(ssid);
    return 0;
}

int AioSymlink(const char *filename, const char *newname) {
    int r;
    if ((r = RegisterCmd(AIOC_SYMLINK)) < 0) return r;
    strcpy(aio.name0, filename);
    strcpy(aio.name1, newname);
    SignalSema(ssid);
    return 0;
}

int AioChdir(const char *filename) {
    int r;
    if ((r = RegisterCmd(AIOC_CHDIR)) < 0) return r;
    strcpy(aio.name0, filename);
    SignalSema(ssid);
    return 0;
}

int AioMkdir(const char *filename, int flag) {
    int r;
    if ((r = RegisterCmd(AIOC_MKDIR)) < 0) return r;
    strcpy(aio.name0, filename);
    aio.a[0] = flag;
    SignalSema(ssid);
    return 0;
}

int AioRmdir(const char *filename) {
    int r;
    if ((r = RegisterCmd(AIOC_RMDIR)) < 0) return r;
    strcpy(aio.name0, filename);
    SignalSema(ssid);
    return 0;
}

int AioDopen(const char *filename) {
    int r;
    if ((r = RegisterCmd(AIOC_DOPEN)) < 0) return r;
    strcpy(aio.name0, filename);
    SignalSema(ssid);
    return 0;
}

int AioDclose(int fd) {
    int r;
    if ((r = RegisterCmd(AIOC_DCLOSE)) < 0) return r;
    aio.a[0] = fd;
    SignalSema(ssid);
    return 0;
}

int AioDread(int fd, struct sce_dirent *buf) {
    int r;
    if ((r = RegisterCmd(AIOC_DREAD)) < 0) return r;
    aio.a[0] = fd;
    aio.a[1] = (int)buf;
    SignalSema(ssid);
    return 0;
}

int AioSync(const char *devname, int flag) {
    int r;
    if ((r = RegisterCmd(AIOC_SYNC)) < 0) return r;
    strcpy(aio.name0, devname);
    aio.a[0] = flag;
    SignalSema(ssid);
    return 0;
}

int AioFormat(const char *fsname, const char *devname, const void *arg, int len){
    int r;
    if ((r = RegisterCmd(AIOC_FORMAT)) < 0) return r;
    strcpy(aio.name0, fsname);
    if (devname) {
	strcpy(aio.name1, devname);
	aio.a[0] = (int)aio.name1;
    } else {
	aio.a[0] = (int)NULL;
    }
    if (arg) {
	memcpy(aio.buf, arg, (size_t)len);
	aio.a[1] = (int)aio.buf;
    } else {
	aio.a[1] = (int)NULL;
    }
    aio.a[2] = len;
    SignalSema(ssid);
    return 0;
}

int AioMount(const char *fsname, const char *devname,
					int flag, const void *arg, int len) {
    int r;
    if ((r = RegisterCmd(AIOC_MOUNT)) < 0) return r;
    strcpy(aio.name0, fsname);
    if (devname) {
	strcpy(aio.name1, devname);
	aio.a[0] = (int)aio.name1;
    } else {
	aio.a[0] = (int)NULL;
    }
    aio.a[1] = flag;
    if (arg) {
	memcpy(aio.buf, arg, (size_t)len);
	aio.a[2] = (int)aio.buf;
    } else {
	aio.a[2] = (int)NULL;
    }
    aio.a[3] = len;
    SignalSema(ssid);
    return 0;
}

int AioUmount(const char *fsname) {
    int r;
    if ((r = RegisterCmd(AIOC_UMOUNT)) < 0) return r;
    strcpy(aio.name0, fsname);
    SignalSema(ssid);
    return 0;
}

int AioDevctl(const char *devname, int request,
			    const void *wbuf, int wlen, void *rbuf, int rlen) {
    int r;
    if ((r = RegisterCmd(AIOC_DEVCTL)) < 0) return r;
    strcpy(aio.name0, devname);
    aio.a[0] = request;
    if (wbuf) {
	memcpy(aio.buf, wbuf, (size_t)wlen);
	aio.a[1] = (int)aio.buf;
    } else {
	aio.a[1] = (int)NULL;
    }
    aio.a[2] = wlen;
    aio.a[3] = (int)rbuf;
    aio.a[4] = rlen;
    SignalSema(ssid);
    return 0;
}

int AioIoctl2(int fd, int request,
			    const void *wbuf, int wlen, void *rbuf, int rlen) {
    int r;
    if ((r = RegisterCmd(AIOC_IOCTL2)) < 0) return r;
    aio.a[0] = fd;
    aio.a[1] = request;
    if(wbuf) {
	memcpy(aio.buf, wbuf, (size_t)wlen);
	aio.a[2] = (int)aio.buf;
    } else {
	aio.a[2] = (int)NULL;
    }
    aio.a[3] = wlen;
    aio.a[4] = (int)rbuf;
    aio.a[5] = rlen;
    SignalSema(ssid);
    return 0;
}



static void threadmain(void *param) {
    int r;
    Aio *p = (Aio *)param;

    while(1) {
	if (WaitSema(ssid) < 0) ExitDeleteThread();

	switch (p->ccmd) {			// dispatch command
	    case AIOC_NONE:
		r = 0; break;
	    case AIOC_OPEN:
		r = sceOpen(p->name0, p->a[0], (u_int)p->a[1]); break;
	    case AIOC_CLOSE:
		r = sceClose(p->a[0]); break;
	    case AIOC_READ:
		r = sceRead(p->a[0], (void *)p->a[1], p->a[2]); break;
	    case AIOC_WRITE:
		r = sceWrite(p->a[0], (void *)p->a[1], p->a[2]); break;
	    case AIOC_LSEEK:
		r = sceLseek(p->a[0], p->a[1], p->a[2]); break;
	    case AIOC_LSEEK64:
		r = sceLseek64(p->a[0], p->al, p->a[1]); break;
	    case AIOC_CHSTAT:
		r = sceChstat(p->name0, (struct sce_stat *)p->a[0], p->a[1]);
		break;
	    case AIOC_GETSTAT:
		r = sceGetstat(p->name0, (struct sce_stat *)p->a[0]); break;
	    case AIOC_RENAME:
		r = sceRename(p->name0, p->name1); break;
	    case AIOC_REMOVE:
		r = sceRemove(p->name0); break;
	    case AIOC_READLINK:
		r = sceReadlink(p->name0, (void *)p->a[0], (size_t)p->a[1]);
		break;
	    case AIOC_SYMLINK:
		r = sceSymlink(p->name0, p->name1); break;
	    case AIOC_CHDIR:
		r = sceChdir(p->name0); break;
	    case AIOC_MKDIR:
		r = sceMkdir(p->name0, p->a[0]); break;
	    case AIOC_RMDIR:
		r = sceRmdir(p->name0); break;
	    case AIOC_DOPEN:
		r = sceDopen(p->name0); break;
	    case AIOC_DCLOSE:
		r = sceDclose(p->a[0]); break;
	    case AIOC_DREAD:
		r = sceDread(p->a[0], (struct sce_dirent *)p->a[1]); break;
	    case AIOC_SYNC:
		r = sceSync(p->name0, p->a[0]); break;
	    case AIOC_FORMAT:
		r = sceFormat(p->name0, (char *)p->a[0],
				(void *)p->a[1], p->a[2]); break;
	    case AIOC_MOUNT:
		r = sceMount(p->name0, (char *)p->a[0], p->a[1],
				(void *)p->a[2], p->a[3]); break;
	    case AIOC_UMOUNT:
		r = sceUmount(p->name0); break;
	    case AIOC_DEVCTL:
		r = sceDevctl(p->name0, p->a[0],
				(void *)p->a[1], (u_int)p->a[2],
				(void *)p->a[3], (u_int)p->a[4]); break;
	    case AIOC_IOCTL2:
		r = sceIoctl2(p->a[0], p->a[1],
				(void *)p->a[2], (u_int)p->a[3],
				(void *)p->a[4], (u_int)p->a[5]); break;
	    default:
		r = -1; printf("aio: unknown i/o operation.\n"); break;
	}

	// notify command completion to calling thread using semaphore
	p->result = r;
	p->lcmd = p->ccmd;			// save the last command
	p->ccmd = AIOC_NONE;
	SignalSema(esid);
    }
}


int AioInit(int priority) {
    struct SemaParam sp;
    struct ThreadParam tp;
    static u_char stack[STACK_SIZE] __attribute__((aligned(16)));

    sp.initCount = 0;
    sp.maxCount  = 1;
    sp.option    = 0;
    ssid = CreateSema(&sp);			// create semaphore

    sp.initCount = 0;
    sp.maxCount  = 1;
    sp.option    = 0;
    esid = CreateSema(&sp);			// create semaphore

    tp.entry        = threadmain;
    tp.stack        = stack;
    tp.stackSize    = sizeof(stack);
    tp.initPriority = priority;
    tp.gpReg        = &_gp;
    tid = CreateThread(&tp);			// create worker thread

    memset(&aio, 0, sizeof(aio));
    StartThread(tid, (void *)&aio);
    return 0;
}

void AioTerminate(void) {
    DeleteSema(ssid);
    DeleteSema(esid);
}

