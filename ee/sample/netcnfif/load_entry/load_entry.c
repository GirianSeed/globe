/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/* 
 *      Netcnf Interface Library Sample Program
 *
 *                         Version 1.8
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         load_entry.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 *       1.2            2002.01.31      tetsu       Correspond to libnet
 *       1.3            2002.01.31      tetsu       Remove get_list()
 *       1.4            2002.01.31      tetsu       Change "Combination1"'s definition
 *       1.5            2002.02.13      tetsu       Correspond to pppoe connection(up)
 *       1.6            2002.02.13      tetsu       Correspond to pppoe connection(down)
 *       1.7            2002.02.15      tetsu       Add sceLibnetWaitGetInterfaceID()
 *       1.8            2002.04.10      tetsu       Modified sceLibnetWaitGetInterfaceID()'s Argument
 *                                                  Modified sceLibnetWaitGetAddress()'s Argument
 */

#include <malloc.h>
#include <eekernel.h>
#include <sifrpc.h>
#include <libmc.h>
#include <netcnfif.h>
#include <libnet.h>
#include <libmrpc.h>

#include "load_module.h"

#define DEBUG /* �f�o�b�O�v�����g��\�����Ȃ��ꍇ�̓R�����g�A�E�g���ĉ����� */

#define IOPRP "host0:../../../../iop/modules/"IOP_IMAGE_file /* ���v���C�X���W���[���p�X */

#define WORKAREA_SIZE (0x1000) /* IOP �ɂ����郏�[�N�G���A�̃T�C�Y */

#define FNAME "mc0:/BWNETCNF/BWNETCNF" /* �ݒ�Ǘ��t�@�C���p�X */
#define NET_USR_NAME "Combination1"    /* �g�ݍ��킹�� */

static sceNetcnfifArg_t if_arg; /* Netcnf Interface �ɕK�v�ȃf�[�^�̈� */

/* �ݒ�̓��e���擾���� */
static int load_entry(char *fname, int type, char *usr_name, sceNetcnfifData_t *data)
{
    FlushCache(0);
    sceNetcnfifLoadEntry(fname, type, usr_name, data);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] type(%d):sceNetcnfifLoadEntry (%d)\n", __FILE__, type, if_arg.data);
	return (if_arg.data);
    }else{
#ifdef DEBUG
	sceNetcnfifDataDump(data);
#endif /* DEBUG */
	return (0);
    }
}

/* IOP ���� sceNetCnfEnv �̈�� sceNetcnfifData �̓��e��ݒ� */
static int set_env(int type, sceNetcnfifData_t *data)
{
    int addr, id;

    /* sceNetcnfifData �̓]����A�h���X���擾 */
    sceNetcnfifGetAddr();
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    addr = if_arg.addr;

    /* sceNetcnfifData �� IOP �ɓ]�� */
    id = sceNetcnfifSendIOP((unsigned int)data, (unsigned int)addr, (unsigned int)sizeof(sceNetcnfifData_t));
    while(sceNetcnfifDmaCheck(id));

    /* IOP ���� sceNetCnfEnv �̈�� sceNetcnfifData �̓��e��ݒ� */
    sceNetcnfifSetEnv(type);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] type(%d):sceNetcnfifSetEnv (%d)\n", __FILE__, type, if_arg.data);
	return (0);
    }else{
	return (if_arg.addr);
    }
}

int main(void)
{
    static sceNetcnfifData_t data __attribute__((aligned(64)));
    static sceSifMClientData cd;
    static unsigned int g_net_buf[512] __attribute__((aligned(64)));
    static int if_id[sceLIBNET_MAX_INTERFACE];
    static sceInetAddress_t myaddr;
    static char myptrname[128];
    int r, addr;

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

    /* IOP ���̃��[�N�G���A���m�� */
    sceNetcnfifAllocWorkarea(WORKAREA_SIZE);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] sceNetcnfifAllocWorkarea (%d)\n", __FILE__, if_arg.data);
	goto done;
    }

    /* f_no_decode ��ݒ�(�^�C�g���Ɏg�p����ꍇ�͕K��
       sceNetcnfifArg_f_no_decode_off �ɐݒ肷�邱��) */
    sceNetcnfifSetFNoDecode(sceNetcnfifArg_f_no_decode_on);

    /* �ݒ�̓��e���擾���� */
    sceNetcnfifDataInit(&data);
    if(load_entry(FNAME, sceNetcnfifArg_type_net, NET_USR_NAME, &data) < 0) goto done;

    /* IOP ���� sceNetCnfEnv �̈�� sceNetcnfifData �̓��e��ݒ� */
    if((addr = set_env(sceNetcnfifArg_type_net, &data)) == 0) goto done;

    /* Libnet �ɐݒ�𔽉f */
    if((r = sceLibnetSetConfiguration(&cd, g_net_buf, addr)) < 0){
	scePrintf( "[%s] sceLibnetSetConfiguration (%d)\n", __FILE__, r);
	goto done;
    }

    /* �g�p�\�ȃC���^�t�F�[�X ID ���擾 */
    if((r = sceLibnetWaitGetInterfaceID(&cd, g_net_buf, if_id, sceLIBNET_MAX_INTERFACE)) < 0){
      scePrintf( "[%s] sceLibnetGetInterfaceID (%d)\n", __FILE__, r);
      goto done;
    }

    /* �ڑ� */
    if((r = sceInetCtlUpInterface(&cd, g_net_buf, 0)) < 0){
	scePrintf( "[%s] sceInetCtlUpInterface (%d)\n", __FILE__, r);
	goto done;
    }

    /* �A�h���X���擾���� */
    if((r = sceLibnetWaitGetAddress(&cd, g_net_buf, if_id, sceLIBNET_MAX_INTERFACE, &myaddr, 0)) < 0){
	scePrintf( "[%s] sceLibnetWaitGetAddress (%d)\n", __FILE__, r);
	goto done;
    }
    if((r = sceInetAddress2String(&cd, g_net_buf, myptrname, sizeof(myptrname), &myaddr)) < 0){
	scePrintf( "[%s] sceInetAddress2String (%d)\n", __FILE__, r);
	goto done;
    }
#ifdef DEBUG
    scePrintf( "address: \"%s\"\n", myptrname);
#endif /* DEBUG */

    /* �ؒf */
    if((r = sceInetCtlDownInterface(&cd, g_net_buf, 0)) < 0){
	scePrintf( "[%s] sceInetCtlDownInterface (%d)\n", __FILE__, r);
	goto done;
    }
 done:
    sceNetcnfifFreeWorkarea();
    scePrintf("[%s] complete\n", __FILE__);

    return (0);
}
