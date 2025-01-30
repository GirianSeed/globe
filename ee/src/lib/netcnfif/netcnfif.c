/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/* 
 *      Netcnf Interface Library
 *
 *                         Version 1.4
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         netcnfif.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 *       1.2            2002.01.31      tetsu       Add release version string
 *       1.3            2002.02.10      tetsu       Add sceNetcnfifCheckAdditionalAT()
 *       1.4            2002.05.05      tetsu       Add sceNetcnfifCheckSpecialProvider()
 */

#include <stdio.h>
#include <string.h>
#include <eetypes.h>
#include <sifrpc.h>

#include "../../../../common/include/netcnfif.h"
#include "netcnfif_data.h"
#include "dma.h"

/* Sifrpc �p */
static sceSifClientData cd;
static u_char buf[SCE_NETCNFIF_SSIZE] __attribute__((aligned(64)));

/* no_decode �t���O */
static int _f_no_decode = 0;

/* Sifrpc �p����M�o�b�t�@�̏�Ԃ������l�ɖ߂� */
void sceNetcnfifInit(void)
{
    sceNetcnfifArg_t *p;

    p = (sceNetcnfifArg_t *)buf;

    bzero(p, sizeof(sceNetcnfifArg_t));
}

/* Sifrpc �T�[�o�ƃo�C���h */
void sceNetcnfifSetup(void)
{
    int i;

    while(1){
	if(sceSifBindRpc(&cd, SCE_NETCNFIF_INTERFACE, SIF_RPCM_NOWAIT) < 0){
	    while(1);
	}
	while(sceSifCheckStatRpc((sceSifRpcData *)&cd) != 0);
	if(cd.serve != 0) break;
	i = 0x10000;
	while(i --);
    }
}

/* Sifrpc �����󋵂̔���(netcnf_if) */
int sceNetcnfifCheck(void)
{
    return (sceSifCheckStatRpc((sceSifRpcData *)&cd));
}

/* Sifrpc �T�[�r�X�̌��ʂ��擾 */
void sceNetcnfifGetResult(sceNetcnfifArg_t *result)
{
    sceNetcnfifArg_t *p;

    p = (sceNetcnfifArg_t *)buf;
    memcpy(result, p, sizeof(sceNetcnfifArg_t));
}

/* no_decode �t���O��ݒ肷�� */
void sceNetcnfifSetFNoDecode(int f_no_decode)
{
    _f_no_decode = f_no_decode;
}

/* �l�b�g���[�N�ݒ�t�@�C���̐����擾 */
void sceNetcnfifGetCount(char *fname, int type)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, fname);
    p->type = type;

    sceSifCallRpc(&cd, SCE_NETCNFIF_GET_COUNT, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* �l�b�g���[�N�ݒ�t�@�C���̃��X�g���擾 */
void sceNetcnfifGetList(char *fname, int type, sceNetcnfifList_t *addr, int size)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, fname);
    p->type = type;
    p->addr = (int)addr;
    p->data = size;

    sceSifCallRpc(&cd, SCE_NETCNFIF_GET_LIST, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* �l�b�g���[�N�ݒ�t�@�C���̓��e���擾 */
void sceNetcnfifLoadEntry(char *fname, int type, char *usr_name, sceNetcnfifData_t *addr)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, fname);
    p->type = type;
    strcpy(p->usr_name, usr_name);
    p->addr = (int)addr;
    p->f_no_decode = _f_no_decode;

    sceSifCallRpc(&cd, SCE_NETCNFIF_LOAD_ENTRY, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* �l�b�g���[�N�ݒ�t�@�C���̒ǉ� */
void sceNetcnfifAddEntry(char *fname, int type, char *usr_name)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, fname);
    p->type = type;
    strcpy(p->usr_name, usr_name);
    p->f_no_decode = _f_no_decode;

    sceSifCallRpc(&cd, SCE_NETCNFIF_ADD_ENTRY, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* �l�b�g���[�N�ݒ�t�@�C���̕ҏW */
void sceNetcnfifEditEntry(char *fname, int type, char *usr_name, char *new_usr_name)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, fname);
    p->type = type;
    strcpy(p->usr_name, usr_name);
    strcpy(p->new_usr_name, new_usr_name);
    p->f_no_decode = _f_no_decode;

    sceSifCallRpc(&cd, SCE_NETCNFIF_EDIT_ENTRY, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* �l�b�g���[�N�ݒ�t�@�C���̍폜 */
void sceNetcnfifDeleteEntry(char *fname, int type, char *usr_name)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, fname);
    p->type = type;
    strcpy(p->usr_name, usr_name);

    sceSifCallRpc(&cd, SCE_NETCNFIF_DELETE_ENTRY, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* �l�b�g���[�N�ݒ�t�@�C���̃��X�g��ҏW */
void sceNetcnfifSetLatestEntry(char *fname, int type, char *usr_name)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, fname);
    p->type = type;
    strcpy(p->usr_name, usr_name);

    sceSifCallRpc(&cd, SCE_NETCNFIF_SET_LATEST_ENTRY, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* ���Ȃ��̃l�b�g���[�N�ݒ�t�@�C�����폜 */
void sceNetcnfifDeleteAll(char *fname)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, fname);

    sceSifCallRpc(&cd, SCE_NETCNFIF_DELETE_ALL, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* �f�o�C�X�̎c��e�ʂ��`�F�b�N */
void sceNetcnfifCheckCapacity(char *fname)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, fname);

    sceSifCallRpc(&cd, SCE_NETCNFIF_CHECK_CAPACITY, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* �ǉ� AT �R�}���h���`�F�b�N */
void sceNetcnfifCheckAdditionalAT(char *additional_at)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, additional_at);

    sceSifCallRpc(&cd, SCE_NETCNFIF_CHECK_ADDITIONAL_AT, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* ����v���o�C�_�̐ݒ�ł��邩�ǂ������`�F�b�N */
void sceNetcnfifCheckSpecialProvider(char *fname, int type, char *usr_name)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, fname);
    p->type = type;
    strcpy(p->usr_name, usr_name);
    p->f_no_decode = _f_no_decode;

    sceSifCallRpc(&cd, SCE_NETCNFIF_CHECK_SPECIAL_PROVIDER, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* IOP ���̎�M�̈�(sceNetcnfifData)�̃A�h���X���擾 */
void sceNetcnfifGetAddr(void)
{
    sceNetcnfifInit();
    sceSifCallRpc(&cd, SCE_NETCNFIF_GET_ADDR, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* IOP ���̃��[�N�G���A���m�� */
void sceNetcnfifAllocWorkarea(int size)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    p->data = size;

    sceSifCallRpc(&cd, SCE_NETCNFIF_ALLOC_WORKAREA, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* IOP ���̃��[�N�G���A����� */
void sceNetcnfifFreeWorkarea(void)
{
    sceNetcnfifInit();
    sceSifCallRpc(&cd, SCE_NETCNFIF_FREE_WORKAREA, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* IOP ���� sceNetCnfEnv �̈�� sceNetcnfifData �̓��e��ݒ� */
void sceNetcnfifSetEnv(int type)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    p->type = type;
    p->f_no_decode = _f_no_decode;

    sceSifCallRpc(&cd, SCE_NETCNFIF_SET_ENV, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}
