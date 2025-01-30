/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *      Inet Event Sample Program
 *
 *                         Version 1.1
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         rpc.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.02.18      tetsu       Change directory
 */

#include <stdio.h>
#include <string.h>
#include <eetypes.h>
#include <sifrpc.h>

#include "common.h"

/* Sifrpc �p */
static sceSifClientData cd0;
static sceSifClientData cd1;
static u_char buf0[SSIZE] __attribute__((aligned(64)));
static u_char buf1[SSIZE] __attribute__((aligned(64)));

/* Sifrpc �p����M�o�b�t�@�̏�Ԃ������l�ɖ߂� */
void rpc_init(void)
{
    rpc_data_t *p;

    p = (rpc_data_t *)buf0;

    p->interface_id  = -1;
    p->event         = IEV_None;
    bzero(p->ip_addr, sizeof(256));
    bzero(p->line_speed, sizeof(256));
    bzero(p->phone_number, sizeof(256));
}

/* Sifrpc �T�[�o�ƃo�C���h */
void rpc_setup(void)
{
    int i;

    rpc_init();

    while(1){
	if(sceSifBindRpc(&cd0, GET_EVENT, SIF_RPCM_NOWAIT) < 0){
	    while(1);
	}
	while(sceSifCheckStatRpc((sceSifRpcData *)&cd0) != 0);
	if(cd0.serve != 0) break;
	i = 0x10000;
	while(i --);
    }

    while(1){
	if(sceSifBindRpc(&cd1, UP_DOWN, SIF_RPCM_NOWAIT) < 0){
	    while(1);
	}
	while(sceSifCheckStatRpc((sceSifRpcData *)&cd1) != 0);
	if(cd1.serve != 0) break;
	i = 0x10000;
	while(i --);
    }
}

/* Sifrpc �����󋵂̔���(get_event) */
int rpc_check0(void)
{
    return (sceSifCheckStatRpc((sceSifRpcData *)&cd0));
}

/* Sifrpc �����󋵂̔���(up_down) */
int rpc_check1(void)
{
    return (sceSifCheckStatRpc((sceSifRpcData *)&cd1));
}

/* �C�x���g���擾���� */
int rpc_get_event(void)
{
    rpc_data_t *p;

    p = (rpc_data_t *)buf0;

    return (p->event);
}

/* �C���^�t�F�[�X ID ���擾���� */
int rpc_get_id(void)
{
    rpc_data_t *p;

    p = (rpc_data_t *)buf0;

    return (p->interface_id);
}

/* ������x���擾���� */
char *rpc_get_line_speed(void)
{
    rpc_data_t *p;

    p = (rpc_data_t *)buf0;

    return (p->line_speed);
}

/* IP �A�h���X���擾���� */
char *rpc_get_ip_addr(void)
{
    rpc_data_t *p;

    p = (rpc_data_t *)buf0;

    return (p->ip_addr);
}

/* �ڑ���d�b�ԍ����擾���� */
char *rpc_get_phone_number(void)
{
    rpc_data_t *p;

    p = (rpc_data_t *)buf0;

    return (p->phone_number);
}

/* �C�x���g��҂� */
void rpc_wait_event(int interface_id)
{
    rpc_data_t *p;

    p = (rpc_data_t *)buf0;

    p->interface_id = interface_id;

    sceSifCallRpc(&cd0, WAIT_EVENT, SIF_RPCM_NOWAIT, &buf0[0], SSIZE, &buf0[0], SSIZE, 0, 0);
}

/* UP ���� */
void rpc_up(int addr)
{
    rpc_data_t *p;

    p = (rpc_data_t *)buf1;

    p->addr = addr;

    sceSifCallRpc(&cd1, UP, SIF_RPCM_NOWAIT, &buf1[0], SSIZE, &buf1[0], SSIZE, 0, 0);
}

/* DOWN ���� */
void rpc_down(void)
{
    rpc_data_t *p;

    p = (rpc_data_t *)buf1;

    sceSifCallRpc(&cd1, DOWN, SIF_RPCM_NOWAIT, &buf1[0], SSIZE, &buf1[0], SSIZE, 0, 0);
}
