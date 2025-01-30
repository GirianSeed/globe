/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: load_entry.c,v 1.3 2002/11/25 06:00:53 xokano Exp $
 */
/* 
 * Emotion Engine Library Sample Program
 *
 * Copyright (C) 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * bnnetcnf/load_entry - load_entry.c
 *	"PlayStation BB Navigator" Network Configuration Read Library Sample Program
 *	main portion
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2002-10-12      tetsu       first version
 *      2002-10-20      tetsu       correspond to code-review
 */

#include <malloc.h>
#include <eekernel.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <netcnfif.h>
#include <libnet.h>
#include <libmrpc.h>
#include <bnnetcnf.h>

#include "load_module.h"

#define DEBUG /* �f�o�b�O�v�����g��\�����Ȃ��ꍇ�̓R�����g�A�E�g���ĉ����� */

#define IOPRP         "host0:/usr/local/sce/iop/modules/"IOP_IMAGE_file /* ���v���C�X���W���[���p�X */
#define WORKAREA_SIZE (0x1000) /* IOP �ɂ����郏�[�N�G���A�̃T�C�Y */
#define DEVICE        "hdd0:"
#define DEVICE_NAME   DEVICE"__sysconf"
#define FSNAME        "pfs0:"

static sceNetcnfifArg_t if_arg; /* Netcnf Interface �ɕK�v�ȃf�[�^�̈� */

/* �v���g�^�C�v�錾 */
static void *my_malloc(size_t size);
static void my_free(void *addr);
static int set_env(int type, sceNetcnfifData_t *data);

/*
 * �Ǝ��d�l�� malloc()
 */
static void *my_malloc(size_t size)
{
    void *addr;

    addr = malloc(size);
    scePrintf("[%s] size = %d, addr = 0x%08x\n", __FUNCTION__, size, (u_int)addr);

    return (addr);
}

/*
 * �Ǝ��d�l�� free()
 */
static void my_free(void *addr)
{
    free(addr);
    scePrintf("[%s] addr = 0x%08x\n", __FUNCTION__, (u_int)addr);
}

/*
 * IOP ���� sceNetCnfEnv �̈�� sceNetcnfifData �̓��e��ݒ�
 */
static int set_env(int type, sceNetcnfifData_t *data)
{
    int addr;
    int id;

    /* sceNetcnfifData �̓]����A�h���X���擾 */
    sceNetcnfifGetAddr();
    while (sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    addr = if_arg.addr;

    /* sceNetcnfifData �� IOP �ɓ]�� */
    id = sceNetcnfifSendIOP((u_int)data, (u_int)addr, (u_int)sizeof(sceNetcnfifData_t));
    while (sceNetcnfifDmaCheck(id));

    /* IOP ���� sceNetCnfEnv �̈�� sceNetcnfifData �̓��e��ݒ� */
    sceNetcnfifSetEnv(type);
    while (sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if (if_arg.data < 0) {
	scePrintf("[%s] type(%d):sceNetcnfifSetEnv (%d)\n", __FUNCTION__, type, if_arg.data);
	return (0);
    } else {
	return (if_arg.addr);
    }
}

/*
 * IOP ���̃��[�N�G���A���m�ۂ���
 */
static int alloc_workarea(void)
{
    int r;

    sceNetcnfifAllocWorkarea(WORKAREA_SIZE);
    while (sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if (if_arg.data < 0) {
	scePrintf("[%s] sceNetcnfifAllocWorkarea (%d)\n", __FUNCTION__, if_arg.data);
	return (if_arg.data);
    }
    r = if_arg.data;

    return (r);
}

/*
 * �K�v�ȏ���������
 */
int initialize(void)
{
    int r;

    sceSifInitRpc(0);
    while (sceSifRebootIop(IOPRP) == 0); /* IOP ���u�[�g�E���W���[���u������ */
    while (sceSifSyncIop() == 0);        /* �I����҂� */
    sceSifInitRpc(0);
    sceSifLoadFileReset();
    sceFsReset();

    /* �K�v�ȃ��W���[�������[�h */
    load_module();

    /* DEVICE_NAME �� FSNAME �Ƀ}�E���g */
    if ((r = sceDevctl(DEVICE, HDIOC_STATUS, NULL, 0, NULL, 0)) == 0) {
	if ((r = sceMount(FSNAME, DEVICE_NAME, SCE_MT_RDONLY, NULL, 0)) < 0) {
	    scePrintf("[%s] sceMount (%d)\n", __FUNCTION__, r);
	    return (r);
	}
    } else {
	scePrintf("[%s] sceDevctl (%d)\n", __FUNCTION__, r);
	return (r);
    }

    /* Sifrpc �T�[�o�ƃo�C���h */
    sceNetcnfifInit();
    sceNetcnfifSetup();

    return (0);
}

int main(void)
{
    static sceNetcnfifData_t data __attribute__((aligned(64)));
    static sceSifMClientData cd;
    static u_int             g_net_buf[512] __attribute__((aligned(64)));
    static int               if_id[sceLIBNET_MAX_INTERFACE];
    static sceInetAddress_t  myaddr;
    static char              myptrname[128];
    int                      r;
    int                      addr;

    /* �K�v�ȏ��������� */
    if (initialize() < 0) goto done;

    /* Libnet �֌W�֐��̏����� */
    sceSifMInitRpc(0);
    if ((r = sceLibnetInitialize(&cd, sceLIBNET_BUFFERSIZE, sceLIBNET_STACKSIZE, sceLIBNET_PRIORITY)) < 0) {
	scePrintf( "[%s] sceLibnetInitialize (%d)\n", __FUNCTION__, r);
	goto done;
    }
    if ((r = sceLibnetRegisterHandler(&cd, g_net_buf)) < 0) {
	scePrintf( "[%s] sceLibnetRegisterHandler (%d)\n", __FUNCTION__, r);
	goto done;
    }

    /* IOP ���̃��[�N�G���A���m�� */
    if (alloc_workarea() < 0) goto done;

    /* "PlayStation BB Navigator" �̃l�b�g���[�N�ݒ��ǂݍ��� */
    r = sceBnnetcnfRead(FSNAME, (SceBnnetcnfData *)&data, my_malloc, my_free);
    if (r < 0) {
	scePrintf("[%s] sceBnnetcnfRead (%d)\n", __FUNCTION__, -SCE_ERROR_ERRNO(r));
	goto done;
    }
#ifdef DEBUG
    sceNetcnfifDataDump(&data);
#endif /* DEBUG */

    /* IOP ���� sceNetCnfEnv �̈�� sceNetcnfifData �̓��e��ݒ� */
    if ((addr = set_env(sceNetcnfifArg_type_net, &data)) == 0) goto done;

    /* Libnet �ɐݒ�𔽉f */
    if ((r = sceLibnetSetConfiguration(&cd, g_net_buf, addr)) < 0) {
	scePrintf( "[%s] sceLibnetSetConfiguration (%d)\n", __FUNCTION__, r);
	goto done;
    }

    /* �g�p�\�ȃC���^�t�F�[�X ID ���擾 */
    if ((r = sceLibnetWaitGetInterfaceID(&cd, g_net_buf, if_id, sceLIBNET_MAX_INTERFACE)) < 0) {
      scePrintf( "[%s] sceLibnetGetInterfaceID (%d)\n", __FUNCTION__, r);
      goto done;
    }

    /* �ڑ� */
    if ((r = sceInetCtlUpInterface(&cd, g_net_buf, 0)) < 0) {
	scePrintf( "[%s] sceInetCtlUpInterface (%d)\n", __FUNCTION__, r);
	goto done;
    }

    /* �A�h���X���擾���� */
    if ((r = sceLibnetWaitGetAddress(&cd, g_net_buf, if_id, sceLIBNET_MAX_INTERFACE, &myaddr, 0)) < 0) {
	scePrintf( "[%s] sceLibnetWaitGetAddress (%d)\n", __FUNCTION__, r);
	goto done;
    }
    if ((r = sceInetAddress2String(&cd, g_net_buf, myptrname, sizeof(myptrname), &myaddr)) < 0) {
	scePrintf( "[%s] sceInetAddress2String (%d)\n", __FUNCTION__, r);
	goto done;
    }
#ifdef DEBUG
    scePrintf( "address: \"%s\"\n", myptrname);
#endif /* DEBUG */

    /* �ؒf */
    if ((r = sceInetCtlDownInterface(&cd, g_net_buf, 0)) < 0) {
	scePrintf( "[%s] sceInetCtlDownInterface (%d)\n", __FUNCTION__, r);
	goto done;
    }
 done:
    sceNetcnfifFreeWorkarea();
    scePrintf("[load_entry] complete\n");

    return (0);
}
