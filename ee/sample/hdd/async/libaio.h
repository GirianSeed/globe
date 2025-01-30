/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
// libaio.h - Asynchronous ps2 file I/O wrapper functions
//
// Copyright (C) 2001 Sony Computer Entertainment Inc., All Rights Reserved.
//
// Version        Date            Design       Log
// --------------------------------------------------------------------
// 0.00           May,07,2001     Koji Tashiro 1st version

#ifndef _LIBAIO_H
#define _LIBAIO_H

int  AioInit(int priority);
void AioTerminate(void);
int  AioCmdSync(int mode, int *cmd, int *result);

int AioOpen(const char *filename, int flag, u_int mode);
int AioClose(int fd);
int AioRead(int fd, void *buf, int len);
int AioWrite(int fd, const void *buf, int len);
int AioLseek(int fd, int offset, int how);
int AioLseek64(int fd, long offset, int how);
int AioChstat(const char *filename, const struct sce_stat *stat, u_int cbit);
int AioGetstat(const char *filename, struct sce_stat *buf);
int AioRename(const char *filename, const char *newname);
int AioRemove(const char *filename);
int AioReadlink(const char *filename, void *buf, u_int len);
int AioSymlink(const char *filename, const char *newname);
int AioChdir(const char *filename);
int AioMkdir(const char *filename, int flag);
int AioRmdir(const char *filename);
int AioDopen(const char *filename);
int AioDclose(int fd);
int AioDread(int fd, struct sce_dirent *buf);
int AioSync(const char *devname, int flag);
int AioFormat(const char *fsname, const char *devname, const void *arg, int len);
int AioMount(const char *fsname, const char *devname,
					int flag, const void *arg, int len);
int AioUmount(const char *fsname);
int AioDevctl(const char *devname, int request,
			    const void *wbuf, int wlen, void *rbuf, int rlen);
int AioIoctl2(int fd, int request,
			    const void *wbuf, int wlen, void *rbuf, int rlen);

#endif	// _LIBAIO_H
