/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2003/01/15      ksh         first version
 */

#include <eekernel.h>
#include <stdio.h>
#include <string.h>
#include <netcnfif.h>
#include <libmc2.h>

typedef struct sceNetcnfifMc2 {
	SceMc2SocketParam param;
	int socket;
	int count;
} sceNetcnfifMc2_t;


static sceNetcnfifMc2_t gmc2;
static u_long128 gdmabuf[SCE_MC2_DMA_BUFFER_MAX] __attribute__((aligned (64)));

static void initialize_mc2_socket(void)
{
	memset(&gmc2, 0, sizeof(gmc2));
	gmc2.param.port = -1;
	gmc2.count  = 0;
}

static int create_mc2_socket(int port)
{
	int id;
	int ret;

	id = sceMc2SearchSocket(port, 0);
	if (id >= 0) {
		gmc2.socket = id;
	}
	else if (gmc2.param.port == -1) {
		gmc2.param.option = SCE_MC2_SPECIFIC_PORT;
		gmc2.param.port   = port;
		gmc2.param.slot   = 0;
		ret = sceMc2CreateSocket(&gmc2.param, gdmabuf);
		if (ret < 0) {
			return (-1);
		}
		gmc2.socket = ret;
	}
	gmc2.count += 1;
	return (0);
}

static int delete_mc2_socket(void)
{
	gmc2.count -= 1;
	if (gmc2.count <= 0) {
		if (gmc2.param.port != -1) {
			sceMc2DeleteSocket(gmc2.socket);
		}
		initialize_mc2_socket();
	}
	return (0);
}

static int my_open(const char *device, const char *pathname, int flags, int mode, int *filesize)
{
	SceMc2DirParam dir;
	int port;
	int result;
	int ret;

	(void)flags;
	(void)mode;

	port = SCE_MC2_PORT_1;
	if (strncmp(device, "mc1", 3) == 0) {
		port = SCE_MC2_PORT_2;
	}
	ret = create_mc2_socket(port);
	if (ret < 0) {
		return (-1);
	}

	ret = sceMc2GetInfo(gmc2.socket, NULL);
	if (ret < 0) {
		if ((ret & 0xffff) != SCE_ENEW_DEVICE) {
			delete_mc2_socket();
			return (-1);
		}
	}

	ret = sceMc2GetDir(gmc2.socket, pathname, 0, 1, &dir, &result);
	if (ret < 0) {
		delete_mc2_socket();
		return (-1);
	}
	*filesize = 0;
	if (result == 1) {
		*filesize = dir.size;
	}

	return(gmc2.count);
}

static int my_read(int fd, const char *device, const char *pathname, void *buf, int offset, int size)
{
	int ret;

	(void)fd;
	(void)device;

	ret = sceMc2ReadFile(gmc2.socket, pathname, buf, offset, size);
	return(ret);
}

static int my_close(int fd)
{
	(void)fd;

	delete_mc2_socket();
	return(0);
}

int sceNetcnfifCreateMc2Thread(int priority)
{
	static u_char stack[0x1000] __attribute__((aligned (16)));
	sceNetcnfifCallback_t callback;
	int tid;

	callback.type = sceNetcnfifCallback_type_mc;
	callback.open = my_open;
	callback.read = my_read;
	callback.close = my_close;

	initialize_mc2_socket();
	tid = sceNetcnfifCreateCallbackThread(&callback, stack, 0x1000, priority);
	return (tid);
}

int sceNetcnfifDeleteMc2Thread(int tid)
{
	sceNetcnfifDeleteCallbackThread(tid);
	return (sceNETCNFIF_OK);
}

/*** End of file ***/


