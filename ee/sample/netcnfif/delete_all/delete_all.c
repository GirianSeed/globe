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
 *                         delete_all.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 */

#include <eekernel.h>
#include <sifrpc.h>
#include <libmc.h>
#include <netcnfif.h>

#include "load_module.h"

#define FNAME "mc0:/BWNETCNF/BWNETCNF" /* �ݒ�Ǘ��t�@�C���p�X */

static sceNetcnfifArg_t if_arg; /* Netcnf Interface �ɕK�v�ȃf�[�^�̈� */

/* ���Ȃ��̃l�b�g���[�N�ݒ�t�@�C�����폜���� */
static int delete_all(char *fname)
{
    sceNetcnfifDeleteAll(fname);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] sceNetcnfifDeleteAll (%d)\n", __FILE__, if_arg.data);
	return (if_arg.data);
    }else{
	return (0);
    }
}

int main(void)
{
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

    /* ���Ȃ��̃l�b�g���[�N�ݒ�t�@�C�����폜 */
    if(delete_all(FNAME) < 0) goto done;

 done:
    scePrintf("[%s] complete\n", __FILE__);

    return (0);
}
