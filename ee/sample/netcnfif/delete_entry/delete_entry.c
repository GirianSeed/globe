/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *      Netcnf Interface Library Sample Program
 *
 *                         Version 1.1
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         delete_entry.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 */

#include <malloc.h>
#include <eekernel.h>
#include <sifrpc.h>
#include <libmc.h>
#include <netcnfif.h>

#include "load_module.h"

#define DEBUG

#define FNAME "mc0:/BWNETCNF/BWNETCNF" /* �ݒ�Ǘ��t�@�C���p�X */

static sceNetcnfifArg_t if_arg; /* Netcnf Interface �ɕK�v�ȃf�[�^�̈� */

/* �ݒ���폜���� */
static int delete_entry(char *fname, int type, char *usr_name)
{
    sceNetcnfifDeleteEntry(fname, type, usr_name);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] type(%d):sceNetcnfifDeleteEntry (%d)\n", __FILE__, type, if_arg.data);
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

int main(void)
{
    static sceNetcnfifList_t *net_list, *dev_list, *ifc_list;
    int net_num, dev_num, ifc_num, i;

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

    /* �������[�J�[�h���̏����� */
    sceMcInit();

    /* �g�ݍ��킹�̃��X�g���擾 */
    if((net_num = get_list(FNAME, sceNetcnfifArg_type_net, &net_list)) <= 0) goto done;

    /* �ڑ��@��ݒ�̃��X�g���擾 */
    if((dev_num = get_list(FNAME, sceNetcnfifArg_type_dev, &dev_list)) <= 0) goto done;

    /* �ڑ��v���o�C�_�ݒ�̃��X�g���擾 */
    if((ifc_num = get_list(FNAME, sceNetcnfifArg_type_ifc, &ifc_list)) <= 0) goto done;

    /* �g�ݍ��킹�̍폜 */
    for(i = 0; i < net_num; i++){
	if(delete_entry(FNAME, sceNetcnfifArg_type_net, net_list[i].usr_name) < 0) goto done;
    }

    /* �ڑ��@��ݒ�̍폜 */
    for(i = 0; i < dev_num; i++){
	if(delete_entry(FNAME, sceNetcnfifArg_type_dev, dev_list[0].usr_name) < 0) goto done;
    }

    /* �ڑ��v���o�C�_�ݒ�̍폜 */
    for(i = 0; i < ifc_num; i++){
	if(delete_entry(FNAME, sceNetcnfifArg_type_ifc, ifc_list[0].usr_name) < 0) goto done;
    }

 done:
    scePrintf("[%s] complete\n", __FILE__);

    return (0);
}
