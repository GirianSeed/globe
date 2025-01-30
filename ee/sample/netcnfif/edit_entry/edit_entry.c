/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/* 
 *      Netcnf Interface Library Sample Program
 *
 *                         Version 1.4
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         edit_entry.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 *       1.2            2002.01.31      tetsu       Add get_dev_info()
 *       1.3            2002.02.10      tetsu       Add check_additional_at()
 *       1.4            2002.05.05      tetsu       Add check_special_provider()
 */

#include <stdio.h>
#include <malloc.h>
#include <eekernel.h>
#include <sifrpc.h>
#include <libmc.h>
#include <netcnfif.h>
#include <libnet.h>
#include <libmrpc.h>

#include "load_module.h"
#include "set_configuration.h"

#define DEBUG /* �f�o�b�O�v�����g��\�����Ȃ��ꍇ�̓R�����g�A�E�g���ĉ����� */

#define IOPRP "host0:../../../../iop/modules/"IOP_IMAGE_file /* ���v���C�X���W���[���p�X */

#define WORKAREA_SIZE (0x1000) /* IOP �ɂ����郏�[�N�G���A�̃T�C�Y */

#define FNAME "mc0:/BWNETCNF/BWNETCNF"      /* �ݒ�Ǘ��t�@�C���p�X */
#define IFC_NEW_USR_NAME "Ethernet Setting" /* �V�����ڑ��v���o�C�_�ݒ薼 */

#define DEV_PATTERN (1) /* �ڑ��@��ݒ�p�^�[�� */
#define IFC_PATTERN (3) /* �ڑ��v���o�C�_�ݒ�p�^�[�� */

static sceNetcnfifArg_t if_arg; /* Netcnf Interface �ɕK�v�ȃf�[�^�̈� */

/* �ҏW����ݒ肪����v���o�C�_�̂��̂��ǂ����`�F�b�N���� */
static int check_special_provider(char *fname, int type, char *usr_name)
{
    sceNetcnfifCheckSpecialProvider(fname, type, usr_name);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] type(%d):sceNetcnfifCheckSpecialProvider (%d)\n", __FILE__, type, if_arg.data);
	return (if_arg.data);
    }else{
	return (0);
    }
}

/* �ݒ��ҏW���� */
static int edit_entry(char *fname, int type, char *usr_name, char *new_usr_name)
{
    sceNetcnfifEditEntry(fname, type, usr_name, new_usr_name);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] type(%d):sceNetcnfifEditEntry (%d)\n", __FILE__, type, if_arg.data);
	return (if_arg.data);
    }else{
	return (0);
    }
}

/* �ݒ�̃��X�g���擾���� */
static int get_list(char *fname, int type, sceNetcnfifList_t **list)
{
    int r;

    /* �����擾 */
    sceNetcnfifGetCount(fname, type);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data <= 0){
	scePrintf("[%s] type(%d):sceNetcnfifGetCount (%d)\n", __FILE__, type, if_arg.data);
	return (if_arg.data);
    }
    r = if_arg.data;

    /* ���X�g���擾 */
    *list = (sceNetcnfifList_t *)memalign(64, r * sizeof(sceNetcnfifList_t));
    if(*list == NULL){
	scePrintf("[%s] type(%d):memalign (0x%08x)\n", __FILE__, type, *list);
	return (sceNETCNFIF_ALLOC_ERROR);
    }
    FlushCache(0);
    sceNetcnfifGetList(fname, type, *list, r);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] type(%d):sceNetcnfifGetList (%d)\n", __FILE__, type, if_arg.data);
	return (if_arg.data);
    }

#ifdef DEBUG
    {
	int i;
	scePrintf("[%s] get_list result\n", __FILE__);
	for(i = 0; i < r; i++){
	    scePrintf("%d,%d,%s,%s\n", (*list + i)->type, (*list + i)->stat, (*list + i)->sys_name, (*list + i)->usr_name);
	}
    }
#endif /* DEBUG */

    return (r);
}

/* �f�o�C�X�̎c��e�ʂ��`�F�b�N���� */
static int check_capacity(char *fname)
{
    int r;

    sceNetcnfifCheckCapacity(fname);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] sceNetcnfifCheckCapacity (%d)\n", __FILE__, if_arg.data);
	return (if_arg.data);
    }
    r = if_arg.data;

    return (r);
}

/* �ǉ� AT �R�}���h���`�F�b�N���� */
static int check_additional_at(char *additional_at)
{
    if(*additional_at != '\0'){
	sceNetcnfifCheckAdditionalAT(additional_at);
	while(sceNetcnfifCheck());
	sceNetcnfifGetResult(&if_arg);
	if(if_arg.data < 0){
	    scePrintf("[%s] sceNetcnfifCheckAdditionalAT (%d)\n", __FILE__, if_arg.data);
	    return (if_arg.data);
	}
    }
    return (0);
}

/* �f�o�C�X���C���̎�ʁA�x���_���A�v���_�N�g�����擾���� */
static int get_dev_info(sceSifMClientData *cd, unsigned int *net_buf, int *dev_type, char *vendor, int vendor_size, char *product, int product_size)
{
    int interface_id_num, interface_id_list[256], i, r;

    /* �S�Ẵl�b�g���[�N�C���^�t�F�[�X���X�g�̐��� ID ���擾 */
    if((interface_id_num = sceInetGetInterfaceList(cd, net_buf, interface_id_list, 256)) <= 0){
	scePrintf("[%s] sceInetGetInterfaceList (%d)\n", __FILE__, interface_id_num);
	return (interface_id_num);
    }

    /* �S�Ẵl�b�g���[�N�C���^�t�F�[�X���X�g�Ɗ��ݒ���`�F�b�N
       (�Ō�ɔ������ꂽ�ڑ��@�킪�L���ɂȂ邽�߁A�g�p����ڑ��@��ȊO�̐ڑ��@�킪�ڑ�����Ă����ꍇ
        �Ӑ}���Ȃ��ݒ肪�ł���\��������܂��B) */
    for(i = 0; i < interface_id_num; i++){
	/* �f�o�C�X���C���̎�ʂ��擾 */
	if((r = sceInetInterfaceControl(cd, net_buf, interface_id_list[i], (int)sceInetCC_GetFlags, dev_type, (int)sizeof(int))) < 0){
	    scePrintf("[%s] sceInetCC_GetFlags (%d)\n", __FILE__, r);
	    return (r);
	}
	if(*dev_type & sceInetDevF_ARP){
	    if(*dev_type & sceInetDevF_NIC){
		*dev_type  = sceNetcnfifData_type_nic;
	    }else{
		*dev_type  = sceNetcnfifData_type_eth;
	    }
	}else{
	    *dev_type  = sceNetcnfifData_type_ppp;
	}

	/* �x���_�����擾 */
	if((r = sceInetInterfaceControl(cd, net_buf, interface_id_list[i], (int)sceInetCC_GetVendorName, vendor, vendor_size)) < 0){
	    scePrintf("[%s] sceInetCC_GetVendorName (%d)\n", __FILE__, r);
	    return (r);
	}

	/* �v���_�N�g�����擾 */
	if((r = sceInetInterfaceControl(cd, net_buf, interface_id_list[i], (int)sceInetCC_GetDeviceName, product, product_size)) < 0){
	    scePrintf("[%s] sceInetCC_GetDeviceName (%d)\n", __FILE__, r);
	    return (r);
	}
    }

    return (interface_id_num);
}

int main(void)
{
    static sceNetcnfifData_t data __attribute__((aligned(64)));
    static sceNetcnfifList_t *dev_list, *ifc_list;
    static sceSifMClientData cd;
    static unsigned int g_net_buf[512] __attribute__((aligned(64)));
    char tmp_str[256];
    int dev_num, ifc_num;
    int addr, id, r;

    sceSifInitRpc(0);
    while(!sceSifRebootIop(IOPRP)); /* IOP ���u�[�g�E���W���[���u������ */
    while(!sceSifSyncIop());        /* �I����҂� */
    sceSifInitRpc(0);
    sceSifLoadFileReset();
    sceFsReset();

    /* �K�v�ȃ��W���[�������[�h */
    load_module();

    /* Sifrpc �T�[�o�ƃo�C���h */
    sceNetcnfifInit();
    sceNetcnfifSetup();

    /* Libnet �֌W�֐��̏����� */
    sceSifMInitRpc(0);
    if((r = sceLibnetInitialize(&cd, sceLIBNET_BUFFERSIZE, sceLIBNET_STACKSIZE, sceLIBNET_PRIORITY)) < 0){
	scePrintf( "[%s] sceLibnetInitialize (%d)\n", __FILE__, r);
	goto done;
    }
    if((r = sceLibnetRegisterHandler(&cd, g_net_buf)) < 0){
	scePrintf( "[%s] sceLibnetRegisterHandler (%d)\n", __FILE__, r);
	goto done;
    }

    /* �������[�J�[�h���̏����� */
    sceMcInit();

    /* �f�o�C�X�̎c��e�ʂ��`�F�b�N���� */
    if(check_capacity(FNAME) < 0) goto done;

    /* IOP ���̃��[�N�G���A���m�� */
    sceNetcnfifAllocWorkarea(WORKAREA_SIZE);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] sceNetcnfifAllocWorkarea (%d)\n", __FILE__, if_arg.data);
	goto done;
    }

    /* �ڑ��@��ݒ�̃��X�g���擾 */
    if((dev_num = get_list(FNAME, sceNetcnfifArg_type_dev, &dev_list)) <= 0) goto done;

    /* �ڑ��v���o�C�_�ݒ�̃��X�g���擾 */
    if((ifc_num = get_list(FNAME, sceNetcnfifArg_type_ifc, &ifc_list)) <= 0) goto done;

    /* f_no_decode ��ݒ�(�^�C�g���Ɏg�p����ꍇ�͕K��
       sceNetcnfifArg_f_no_decode_off �ɐݒ肷�邱��) */
    sceNetcnfifSetFNoDecode(sceNetcnfifArg_f_no_decode_on);

    /* sceNetcnfifData �̓]����A�h���X���擾 */
    sceNetcnfifGetAddr();
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    addr = if_arg.addr;

    /* �ڑ��@��ݒ�Ɛڑ��v���o�C�_�ݒ��ݒ� */
    sceNetcnfifDataInit(&data);
    set_attach_cnf(&data, sceNetcnfifArg_type_dev, DEV_PATTERN);
    if(check_additional_at(data.chat_additional) < 0) goto done;
    set_attach_cnf(&data, sceNetcnfifArg_type_ifc, IFC_PATTERN);

    /* �f�o�C�X���C���̎�ʁA�x���_���A�v���_�N�g�����擾���� */
    if(get_dev_info(&cd, g_net_buf, &data.dev_type, data.vendor, sizeof(data.vendor), data.product, sizeof(data.product)) <= 0) goto done;
#ifdef DEBUG
    sceNetcnfifDataDump(&data);
#endif /* DEBUG */

    /* sceNetcnfifData �� IOP �ɓ]�� */
    id = sceNetcnfifSendIOP((unsigned int)&data, (unsigned int)addr, (unsigned int)sizeof(sceNetcnfifData_t));
    while(sceNetcnfifDmaCheck(id));

    /* ����v���o�C�_�̐ݒ肩�ǂ����`�F�b�N���� */
    if(check_special_provider(FNAME, sceNetcnfifArg_type_ifc, ifc_list[0].usr_name) < 0) goto done;

    /* �ڑ��@��ݒ�̕ҏW */
    sprintf(tmp_str, "%s/%s", data.vendor, data.product);
    if(edit_entry(FNAME, sceNetcnfifArg_type_dev, dev_list[0].usr_name, tmp_str) < 0) goto done;

    /* �ڑ��v���o�C�_�ݒ�̕ҏW */
    if(edit_entry(FNAME, sceNetcnfifArg_type_ifc, ifc_list[0].usr_name, IFC_NEW_USR_NAME) < 0) goto done;

 done:
    sceNetcnfifFreeWorkarea();
    scePrintf("[%s] complete\n", __FILE__);

    return (0);
}
