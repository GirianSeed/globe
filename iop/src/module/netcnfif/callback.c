/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *      Copyright (C) 2002-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2003/01/08      ksh         first version
 */

#include <stdio.h>
#include <string.h>
#include <sifrpc.h>
#include <netcnfif.h>
#include <dirent.h>

static	sceSifClientData gcd;
static	u_int gbuf[SCE_NETCNFIF_SSIZE / 4] __attribute__((aligned(64)));

void	sce_callback_initialize(void)
{
	int i;

	for ( ; ; ) {
		if (sceSifBindRpc(&gcd, SCE_NETCNFIF_INTERFACE, 0) < 0) {
			while(1);
		}
		while (sceSifCheckStatRpc((sceSifRpcData *)&gcd) != 0);
		if (gcd.serve != 0) {
			break;
		}
		i = 0x10000;
		while (i--);
    }
}

int sce_callback_open(const char *device, const char *pathname, int flags, int mode, int *filesize)
{
	int *rpcbuf = (int *)gbuf;
	int devicelen;
	int pathnamelen;
	int ssize;
	int rsize;
	int ret;

	rpcbuf[0] = flags;
	rpcbuf[1] = mode;
	devicelen = strlen(device) + 1;
	pathnamelen = strlen(pathname) + 1;
	rpcbuf[2] = devicelen;
	rpcbuf[3] = pathnamelen;
	memcpy((char *)&rpcbuf[4] + 0, device, devicelen);
	memcpy((char *)&rpcbuf[4] + devicelen, pathname, pathnamelen);

	ssize = sizeof(int) * 4 + devicelen + pathnamelen;
	rsize = sizeof(int) * 2;

	ret = sceSifCallRpc(
		&gcd,
		SCE_NETCNFIF_CALLBACK_OPEN,
		0,
		&gbuf[0],
		SCE_NETCNFIF_PAD(ssize),
		&gbuf[0],
		SCE_NETCNFIF_PAD(rsize),
		0,
		0
	);
	if (ret < 0) {
		return (-1);
	}
	*filesize = rpcbuf[1];

	return (rpcbuf[0]);
}

int sce_callback_read(int fd, const char *device, const char *pathname, void *buf, int offset, int size)
{
	int *rpcbuf = (int *)gbuf;
	int devicelen;
	int pathnamelen;
	int ssize;
	int rsize;
	int ret;

	rpcbuf[0] = fd;
	devicelen = strlen(device) + 1;
	pathnamelen = strlen(pathname) + 1;
	rpcbuf[1] = devicelen;
	rpcbuf[2] = pathnamelen;
	rpcbuf[3] = offset;
	rpcbuf[4] = size;
	memcpy((char *)&rpcbuf[5], device, devicelen);
	memcpy((char *)&rpcbuf[5] + devicelen, pathname, pathnamelen);

	ssize = sizeof(int) * 5 + devicelen + pathnamelen;
	rsize = sizeof(int) * 1 + size;

	ret = sceSifCallRpc(
		&gcd,
		SCE_NETCNFIF_CALLBACK_READ,
		0,
		&gbuf[0],
		SCE_NETCNFIF_PAD(ssize),
		&gbuf[0],
		SCE_NETCNFIF_PAD(rsize),
		0,
		0
	);
	if (ret < 0) {
		return (-1);
	}

	memcpy(buf, &gbuf[1], size);

	return (rpcbuf[0]);
}

int sce_callback_close(int fd)
{
	int *rpcbuf = (int *)gbuf;
	int ssize;
	int rsize;
	int ret;

	rpcbuf[0] = fd;

	ssize = sizeof(int);
	rsize = sizeof(int);

	ret = sceSifCallRpc(
		&gcd,
		SCE_NETCNFIF_CALLBACK_CLOSE,
		0,
		&gbuf[0],
		SCE_NETCNFIF_PAD(ssize),
		&gbuf[0],
		SCE_NETCNFIF_PAD(rsize),
		0,
		0
	);
	if (ret < 0) {
		return (-1);
	}

	return (rpcbuf[0]);
}

/*** End of file ***/

