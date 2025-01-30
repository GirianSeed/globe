/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/* 
 *      Netcnf Interface Library
 *
 *                         Version 1.3
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         interface.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 *       1.2            2002.02.06      tetsu       Modified SCE_NETCNFIF_EDIT_ENTRY
 *       1.3            2002.02.10      tetsu       Modified SCE_NETCNFIF_CHECK_ADDITIONAL_AT
 */

#include <stdio.h>
#include <string.h>
#include <kernel.h>
#include <sifrpc.h>
#include <netcnf.h>

//#define DEBUG /* �f�o�b�O�v�����g��\�����Ȃ��ꍇ�̓R�����g�A�E�g���ĉ����� */

#include "../../../../common/include/netcnfif.h"
#include "dma.h"
#include "netcnfif_data.h"
#include "netcnf_env.h"
#include "read_env.h"
#include "write_env.h"

static sceSifServeData sd; /* RPC �T�[�o�f�[�^ */
static sceSifQueueData qd; /* RPC ���N�G�X�g�L���[�f�[�^ */

static void *mem_area = NULL; /* ���[�N�G���A�|�C���^ */
static int mem_area_size = 0; /* ���[�N�G���A�T�C�Y */

static int oldstat; /* ���荞�݂ƃf�B�X�p�b�`�̏�Ԃ̕ۑ��̈� */

/* Sifrpc �T�[�o */
static void *sceNetcnfifInterfaceServer(u_int fno, void *buf, int size)
{
    sceNetcnfifArg_t *p;
    sceNetCnfList_t *list_iop;
    sceNetcnfifList_t *list_ee;
    int i, r = 0, id, redial_count;
    static sceNetCnfEnv_t env;
    static sceNetcnfifData_t data;
    static char chat_additional[256];

    p = (sceNetcnfifArg_t *)buf;

    switch(fno){
    case SCE_NETCNFIF_GET_COUNT: /* �l�b�g���[�N�ݒ�t�@�C���̐����擾 */
	r = sceNetCnfGetCount(p->fname, p->type);
	break;

    case SCE_NETCNFIF_GET_LIST: /* �l�b�g���[�N�ݒ�t�@�C���̃��X�g���擾 */
	r = sceNetCnfGetCount(p->fname, p->type);
	if(r < 0) break;

	CpuSuspendIntr(&oldstat);
	list_iop  = (sceNetCnfList_t *)AllocSysMemory(0, (u_long)(r * sizeof(sceNetCnfList_t)), NULL);
	if(!list_iop){
	    CpuResumeIntr(oldstat);
	    r = sceNETCNFIF_ALLOC_ERROR;
	    break;
	}
	list_ee  = (sceNetcnfifList_t *)AllocSysMemory(0, (u_long)(p->data * sizeof(sceNetcnfifList_t)), NULL);
	if(!list_ee){
	    FreeSysMemory((void *)list_iop); 
	    CpuResumeIntr(oldstat);
	    r = sceNETCNFIF_ALLOC_ERROR;
	    break;
	}
	CpuResumeIntr(oldstat);

	r = sceNetCnfGetList(p->fname, p->type, list_iop);
	if(r < 0){
	    CpuSuspendIntr(&oldstat);
	    FreeSysMemory((void *)list_iop); 
	    FreeSysMemory((void *)list_ee); 
	    CpuResumeIntr(oldstat);
	    break;
	}

	for(i = 0; i < p->data && i < r; i++){
	    memcpy(list_ee + i, list_iop + i, sizeof(sceNetCnfList_t));
	}
#ifdef DEBUG
	printf("[SCE_NETCNFIF_GET_LIST]\n");
	for(i = 0; i < r; i++){
	    printf("%d,%d,%s,%s\n", list_ee[i].type, list_ee[i].stat, list_ee[i].sys_name, list_ee[i].usr_name);
	}
#endif /* DEBUG */

	id = sceNetcnfifSendEE((u_int)list_ee, (u_int)p->addr, (u_int)(p->data * sizeof(sceNetcnfifList_t)));
	while(sceNetcnfifDmaCheck(id));
	CpuSuspendIntr(&oldstat);
	FreeSysMemory((void *)list_iop); 
	FreeSysMemory((void *)list_ee); 
	CpuResumeIntr(oldstat);
	break;

    case SCE_NETCNFIF_LOAD_ENTRY: /* �l�b�g���[�N�ݒ�t�@�C���̓��e���擾 */
	sceNetcnfifEnvInit(&env, mem_area, mem_area_size, p->f_no_decode);
	r = sceNetCnfLoadEntry(p->fname, p->type, p->usr_name, &env);
	if(r < 0) break;
#ifdef DEBUG
	printf("[SCE_NETCNFIF_LOAD_ENTRY]\n");
	sceNetcnfifEnvDump(&env);
#endif /* DEBUG */

	sceNetcnfifDataInit(&data);
	r = sceNetcnfifReadEnv(&data, &env, p->type);
	if(r < 0) break;
#ifdef DEBUG
	printf("[SCE_NETCNFIF_LOAD_ENTRY]\n");
	sceNetcnfifDataDump(&data);
#endif /* DEBUG */

	id = sceNetcnfifSendEE((u_int)&data, (u_int)p->addr, (u_int)sizeof(sceNetcnfifData_t));
	while(sceNetcnfifDmaCheck(id));
	break;

    case SCE_NETCNFIF_ADD_ENTRY: /* �l�b�g���[�N�ݒ�t�@�C���̒ǉ� */
#ifdef DEBUG
	printf("[SCE_NETCNFIF_ADD_ENTRY]\n");
	sceNetcnfifDataDump(&data);
#endif /* DEBUG */
	sceNetcnfifEnvInit(&env, mem_area, mem_area_size, p->f_no_decode);
	r = sceNetcnfifWriteEnv(&env, &data, p->type);
	if(r < 0) break;
#ifdef DEBUG
	printf("[SCE_NETCNFIF_ADD_ENTRY]\n");
	sceNetcnfifEnvDump(&env);
#endif /* DEBUG */

	r = sceNetCnfAddEntry(p->fname, p->type, p->usr_name, &env);
	break;

    case SCE_NETCNFIF_EDIT_ENTRY: /* �l�b�g���[�N�ݒ�t�@�C���̕ҏW */
#ifdef DEBUG
	printf("[SCE_NETCNFIF_EDIT_ENTRY]\n");
	sceNetcnfifDataDump(&data);
#endif /* DEBUG */
	/* ���m�̐ݒ荀�ڕۑ��̂��߂ɕҏW�O�̐ݒ��ǂݍ��� */
	sceNetcnfifEnvInit(&env, mem_area, mem_area_size, p->f_no_decode);
	r = sceNetCnfLoadEntry(p->fname, p->type, p->usr_name, &env);
	if(r < 0) break;
	/* �g�p������ base �����炷 */
	env.mem_ptr = (void *)(((int)env.mem_ptr + 3) & ~3);
	env.mem_base = env.mem_ptr;
	r = sceNetcnfifWriteEnv(&env, &data, p->type);
	if(r < 0) break;
#ifdef DEBUG
	printf("[SCE_NETCNFIF_EDIT_ENTRY]\n");
	sceNetcnfifEnvDump(&env);
#endif /* DEBUG */

	r = sceNetCnfEditEntry(p->fname, p->type, p->usr_name, p->new_usr_name, &env);
	break;

    case SCE_NETCNFIF_DELETE_ENTRY: /* �l�b�g���[�N�ݒ�t�@�C���̍폜 */
	r = sceNetCnfDeleteEntry(p->fname, p->type, p->usr_name);
	break;

    case SCE_NETCNFIF_SET_LATEST_ENTRY: /* �l�b�g���[�N�ݒ�t�@�C���̃��X�g��ҏW */
	r = sceNetCnfSetLatestEntry(p->fname, p->type, p->usr_name);
	break;

    case SCE_NETCNFIF_DELETE_ALL: /* ���Ȃ��̃l�b�g���[�N�ݒ�t�@�C�����폜 */
	r = sceNetCnfDeleteAll(p->fname);
	break;

    case SCE_NETCNFIF_CHECK_CAPACITY: /* �f�o�C�X�̎c��e�ʂ��`�F�b�N */
	r = sceNetCnfCheckCapacity(p->fname);
	break;

    case SCE_NETCNFIF_CHECK_ADDITIONAL_AT: /* �ǉ� AT �R�}���h���`�F�b�N */
	r = sceNetCnfConvA2S(p->fname, chat_additional, sizeof(chat_additional));
	break;

    case SCE_NETCNFIF_GET_ADDR: /* IOP ���̎�M�̈�(sceNetcnfifData)�̃A�h���X���擾 */
	p->addr = (int)&data;
	break;

    case SCE_NETCNFIF_ALLOC_WORKAREA: /* IOP ���̃��[�N�G���A���m�� */
	mem_area_size = p->data;
	CpuSuspendIntr(&oldstat);
	mem_area  = AllocSysMemory(0, (u_long)mem_area_size, NULL);
	if(!mem_area){
	    CpuResumeIntr(oldstat);
	    r = sceNETCNFIF_ALLOC_ERROR;
	    break;
	}
	CpuResumeIntr(oldstat);
	break;

    case SCE_NETCNFIF_FREE_WORKAREA: /* IOP ���̃��[�N�G���A���m�� */
	if(mem_area){
	    CpuSuspendIntr(&oldstat);
	    FreeSysMemory(mem_area); 
	    CpuResumeIntr(oldstat);
	}
	mem_area_size = 0;
	mem_area = NULL;
	break;

    case SCE_NETCNFIF_SET_ENV: /* IOP ���� sceNetCnfEnv �̈�� sceNetcnfifData �̓��e��ݒ� */
#ifdef DEBUG
	printf("[SCE_NETCNFIF_SET_ENV]\n");
	sceNetcnfifDataDump(&data);
#endif /* DEBUG */
	sceNetcnfifEnvInit(&env, mem_area, mem_area_size, p->f_no_decode);
	r = sceNetcnfifWriteEnv(&env, &data, p->type);
	if(r < 0) break;

	if(env.root && env.root->pair_head && env.root->pair_head->ifc){
	    /* redial_count ��ݒ肷�� */
	    for(i = 0, redial_count = 0; i < sceNetCnf_MAX_PHONE_NUMBERS; i++){
		if(NULL == (env.root->pair_head->ifc->phone_numbers[i])) continue;
		switch(i){
		case 0: redial_count++; break;
		case 1: redial_count++; break;
		case 2: redial_count++; break;
		}
	    }
	    env.root->pair_head->ifc->redial_count = redial_count - 1;

	    /* PPPoE �łȂ��ڑ��v���o�C�_�ݒ�� type �� ppp �łȂ��ꍇ��
	       type ���킹������B*/
	    if(env.root->pair_head->ifc->pppoe != 1 &&
	       env.root->pair_head->ifc->type != sceNetCnf_IFC_TYPE_PPP){
        	env.root->pair_head->ifc->type = env.root->pair_head->dev->type;
	    }
	}
#ifdef DEBUG
	printf("[SCE_NETCNFIF_SET_ENV]\n");
	sceNetcnfifEnvDump(&env);
#endif /* DEBUG */
	p->addr = (int)&env;
	break;
    }

    /* �Ԃ�l��ݒ� */
    p->data = r;

    return (buf);
}

/* Sifrpc �T�[�r�X�X�^�[�g */
void sceNetcnfifInterfaceStart(void)
{
    static u_int buf[SCE_NETCNFIF_SSIZE / 4];

    sceSifInitRpc(0);
    sceSifSetRpcQueue(&qd, GetThreadId());
    sceSifRegisterRpc(&sd, SCE_NETCNFIF_INTERFACE, sceNetcnfifInterfaceServer, (void *)&buf[0], 0, 0, &qd);
    sceSifRpcLoop(&qd);
}

/* Sifrpc �T�[�r�X�X�g�b�v */
void sceNetcnfifInterfaceStop(void)
{
    /* ���[�N�G���A����� */
    if(mem_area){
	CpuSuspendIntr(&oldstat);
	FreeSysMemory(mem_area); 
	CpuResumeIntr(oldstat);
	mem_area_size = 0;
	mem_area = NULL;
    }

    /* Sifrpc �T�[�r�X�̒�~ */
    sceSifRemoveRpc(&sd, &qd);
    sceSifRemoveRpcQueue(&qd);
}
