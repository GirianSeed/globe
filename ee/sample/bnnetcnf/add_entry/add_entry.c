/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: add_entry.c,v 1.5 2003/07/18 04:33:49 ksh Exp $
 */
/* 
 * Emotion Engine Library Sample Program
 *
 * Copyright (C) 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * bnnetcnf/add_entry - add_entry.c
 *	"PlayStation BB Navigator" Network Configuration Read Library Sample Program
 *	main portion
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2002-10-12      tetsu       first version
 *      2002-10-20      tetsu       correspond to code-review
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <eekernel.h>
#include <sifrpc.h>
#include <libmc.h>
#include <netcnfif.h>
#include <bnnetcnf.h>

#include "load_module.h"

#define DEBUG /* �f�o�b�O�v�����g��\�����Ȃ��ꍇ�̓R�����g�A�E�g���ĉ����� */

#define IOPRP         "host0:/usr/local/sce/iop/modules/"IOP_IMAGE_file /* ���v���C�X���W���[���p�X */
#define WORKAREA_SIZE (0x1000) /* IOP �ɂ����郏�[�N�G���A�̃T�C�Y */
#define DEVICE        "hdd0:"
#define DEVICE_NAME   DEVICE"__sysconf"
#define FSNAME        "pfs0:"
#define FNAME         "mc0:/BWNETCNF/BWNETCNF" /* �ݒ�Ǘ��t�@�C���p�X */
#define NET_USR_NAME  "Combination1" /* �g�ݍ��킹�� */
#define IFC_USR_NAME  "SCE Ethernet Setting" /* �ڑ��v���o�C�_�ݒ薼 */

static sceNetcnfifArg_t if_arg; /* Netcnf Interface �ɕK�v�ȃf�[�^�̈� */

/* �v���g�^�C�v�錾 */
static void *my_malloc(size_t size);
static void my_free(void *addr);
static int add_entry(char *fname, int type, char *usr_name);
static int get_list(char *fname, int type, sceNetcnfifList_t **list);
static int check_capacity(char *fname);

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
 * �ݒ��ǉ�����
 */
static int add_entry(char *fname, int type, char *usr_name)
{
    sceNetcnfifAddEntry(fname, type, usr_name);
    while (sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if (if_arg.data < 0) {
	scePrintf("[%s] type(%d):sceNetcnfifAddEntry (%d)\n", __FUNCTION__, type, if_arg.data);
	return (if_arg.data);
    } else {
	return (0);
    }
}

/*
 * �ݒ�̃��X�g���擾����
 */
static int get_list(char *fname, int type, sceNetcnfifList_t **list)
{
    int r;

    /* �����擾 */
    sceNetcnfifGetCount(fname, type);
    while (sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if (if_arg.data <= 0) {
	scePrintf("[%s] type(%d):sceNetcnfifGetCount (%d)\n", __FUNCTION__, type, if_arg.data);
	return (if_arg.data);
    }
    r = if_arg.data;

    /* ���X�g���擾 */
    *list = (sceNetcnfifList_t *)memalign(64, r * sizeof(sceNetcnfifList_t));
    if (*list == NULL) {
	scePrintf("[%s] type(%d):memalign (0x%08x)\n", __FUNCTION__, type, *list);
	return (sceNETCNFIF_ALLOC_ERROR);
    }
    FlushCache(0);
    sceNetcnfifGetList(fname, type, *list, r);
    while (sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if (if_arg.data < 0) {
	scePrintf("[%s] type(%d):sceNetcnfifGetList (%d)\n", __FUNCTION__, type, if_arg.data);
	return (if_arg.data);
    }

#ifdef DEBUG
    {
	int i;
	scePrintf("[%s] get_list result\n", __FUNCTION__);
	for (i = 0; i < r; i++) {
	    scePrintf("%d,%d,%s,%s\n", (*list + i)->type, (*list + i)->stat, (*list + i)->sys_name, (*list + i)->usr_name);
	}
    }
#endif /* DEBUG */

    return (r);
}

/*
 * �f�o�C�X�̎c��e�ʂ��`�F�b�N����
 */
static int check_capacity(char *fname)
{
    int r;

    sceNetcnfifCheckCapacity(fname);
    while (sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if (if_arg.data < 0) {
	scePrintf("[%s] sceNetcnfifCheckCapacity (%d)\n", __FUNCTION__, if_arg.data);
	return (if_arg.data);
    }
    r = if_arg.data;

    return (r);
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

    /* �������[�J�[�h���̏����� */
    if ((r = sceMcInit()) < 0) {
	scePrintf("[%s] sceMcInit (%d)\n", __FUNCTION__, r);
	return (r);
    }

    return (0);
}

int main(void)
{
    static sceNetcnfifData_t data __attribute__((aligned(64)));
    static sceNetcnfifList_t *dev_list;
    static sceNetcnfifList_t *ifc_list;
    char                     tmp_str[256];
    int                      dev_num;
    int                      ifc_num;
    int                      addr;
    int                      id;
    int                      r;

    /* �K�v�ȏ��������� */
    if (initialize() < 0) goto done;

    /* �f�o�C�X�̎c��e�ʂ��`�F�b�N���� */
    if (check_capacity(FNAME) < 0) goto done;

    /* IOP ���̃��[�N�G���A���m�� */
    if (alloc_workarea() < 0) goto done;

    /* sceNetcnfifData �̓]����A�h���X���擾 */
    sceNetcnfifGetAddr();
    while (sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    addr = if_arg.addr;

    /* "PlayStation BB Navigator" �̃l�b�g���[�N�ݒ��ǂݍ��� */
    r = sceBnnetcnfRead(FSNAME, (SceBnnetcnfData *)&data, my_malloc, my_free);
    if (r < 0) {
	scePrintf("[%s] sceBnnetcnfRead (%d)\n", __FUNCTION__, -SCE_ERROR_ERRNO(r));
	goto done;
    }
#ifdef DEBUG
    sceNetcnfifDataDump(&data);
#endif /* DEBUG */

    /* sceNetcnfifData �� IOP �ɓ]�� */
    id = sceNetcnfifSendIOP((u_int)&data, (u_int)addr, (u_int)sizeof(sceNetcnfifData_t));
    while (sceNetcnfifDmaCheck(id));

    /* �ڑ��@��ݒ�̒ǉ� */
    sprintf(tmp_str, "%s/%s", data.vendor, data.product);
    if (add_entry(FNAME, sceNetcnfifArg_type_dev, tmp_str) < 0) goto done;

    /* �ڑ��v���o�C�_�ݒ�̒ǉ� */
    if (add_entry(FNAME, sceNetcnfifArg_type_ifc, IFC_USR_NAME) < 0) goto done;

    /* �ڑ��@��ݒ�̃��X�g���擾 */
    if ((dev_num = get_list(FNAME, sceNetcnfifArg_type_dev, &dev_list)) <= 0) goto done;

    /* �ڑ��v���o�C�_�ݒ�̃��X�g���擾 */
    if ((ifc_num = get_list(FNAME, sceNetcnfifArg_type_ifc, &ifc_list)) <= 0) goto done;

    /* �g�ݍ��킹��ݒ� */
    sceNetcnfifDataInit(&data);
    strcpy(data.attach_ifc, ifc_list[0].sys_name);
    strcpy(data.attach_dev, dev_list[0].sys_name);

    /* sceNetcnfifData �� IOP �ɓ]�� */
    id = sceNetcnfifSendIOP((u_int)&data, (u_int)addr, (u_int)sizeof(sceNetcnfifData_t));
    while (sceNetcnfifDmaCheck(id));

    /* �g�ݍ��킹�̒ǉ� */
    if (add_entry(FNAME, sceNetcnfifArg_type_net, NET_USR_NAME) < 0) goto done;

 done:
    sceNetcnfifFreeWorkarea();
    scePrintf("[add_entry] complete\n");

    return (0);
}
