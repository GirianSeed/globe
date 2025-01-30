/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: read_env.c,v 1.4 2002/10/19 12:32:57 tetsu Exp $
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
 *                         read_env.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 *       1.2            2002.01.31      tetsu       Modified get_attach()'s bug
 *       1.3            2002.02.10      tetsu       Correspond to sceNetCnfConvS2A()
 *       1.4            2002.10.19      tetsu       Change path of netcnfif.h
 */

#include <stdio.h>
#include <string.h>
#include <netcnf.h>
#include <netcnfif.h>

static int ns_count;

static int get_cmd(sceNetcnfifData_t *data, sceNetCnfCommand_t *p, int *ns_count)
{
    int r = 0;
    sceNetCnfRoutingEntry_t *re;

    switch(p->code){
    case sceNetCnf_CMD_ADD_NAMESERVER:
	switch(*ns_count){
	case 0: /* �v���C�}�� DNS */
	    r = sceNetCnfAddress2String(data->dns1_address, sizeof(data->dns1_address), (sceNetCnfAddress_t *)(p + 1));
	    *ns_count = *ns_count + 1;
	    break;
	case 1: /* �Z�J���_�� DNS */
	    r = sceNetCnfAddress2String(data->dns2_address, sizeof(data->dns2_address), (sceNetCnfAddress_t *)(p + 1));
	    *ns_count = *ns_count + 1;
	    break;
	}
	break;
    case sceNetCnf_CMD_ADD_ROUTING:
	re = (sceNetCnfRoutingEntry_t *)(p + 1);
	r = sceNetCnfAddress2String(data->gateway, sizeof(data->gateway), &re->gateway); /* Default Gateway */
	break;
    }

    return (r);
}

static int get_attach(sceNetcnfifData_t *data, sceNetCnfInterface_t *p, int type)
{
    int i, r = 0;
    char *str;
    sceNetCnfCommand_t *cmd;

    switch(type){
    case 1:
	/* �f�o�C�X���C���̎�� */
	data->ifc_type = p->type;

	/* DHCP ���g�p����E���Ȃ� */
	data->dhcp = p->dhcp;

	/* DHCP �p�z�X�g�� */
	if(p->dhcp_host_name != NULL) strcpy(data->dhcp_host_name, p->dhcp_host_name);

	/* IP �A�h���X */
	if(p->address != NULL) strcpy(data->address, p->address);

	/* �l�b�g�}�X�N */
	if(p->netmask != NULL) strcpy(data->netmask, p->netmask);

	/* �f�t�H���g���[�^(�Ō�̂��̂��L��), �v���C�}�� DNS/�Z�J���_�� DNS(�ŏ��̂Q���L��) */
	for(ns_count = 0, cmd = p->cmd_head; cmd != NULL; cmd = cmd->forw){
	    r = get_cmd(data, cmd, &ns_count);
	    if(r < 0) return (r);
	}

	/* �ڑ���d�b�ԍ�1-3(�ŏ�����R�Ԗڂ܂ł̓d�b�ԍ����L��) */
	for(i = 0; i < sceNetCnf_MAX_PHONE_NUMBERS; i++){
	    if(NULL == (str = p->phone_numbers[i])) continue;
	    switch(i){
	    case 0:
		strcpy(data->phone_numbers1, str);
		break;
	    case 1:
		strcpy(data->phone_numbers2, str);
		break;
	    case 2:
		strcpy(data->phone_numbers3, str);
		break;
	    }
	}

	/* ���[�U ID */
	if(p->auth_name != NULL) strcpy(data->auth_name, p->auth_name);

	/* �p�X���[�h */
	if(p->auth_key != NULL) strcpy(data->auth_key, p->auth_key);

	/* �ڑ���̔F�ؖ� */
	if(p->peer_name != NULL) strcpy(data->peer_name, p->peer_name);

	/* DNS �T�[�o�A�h���X�������擾����E���Ȃ�(�v���C�}�� DNS) */
	data->dns1_nego = p->want.dns1_nego;

	/* DNS �T�[�o�A�h���X�������擾����E���Ȃ�(�Z�J���_�� DNS) */
	data->dns2_nego = p->want.dns2_nego;

	/* �F�ؕ��@��ݒ肷��E���Ȃ� */
	data->f_auth = p->allow.f_auth;

	/* �F�ؕ��@ */
	data->auth = p->allow.auth;

	/* PPPoE ���g�p���� */
	data->pppoe = p->pppoe;

	/* PFC �l�S�V�G�[�V�����̋֎~ */
	data->prc_nego = p->want.prc_nego;

	/* ACFC �l�S�V�G�[�V�����̋֎~ */
	data->acc_nego = p->want.acc_nego;

	/* ACCM �l�S�V�G�[�V�����̋֎~ */
	data->accm_nego = p->want.accm_nego;

	/* MTU �� MRU �̐ݒ� */
	data->mtu = p->mtu;

	/* ����ؒf�ݒ� */
	data->ifc_idle_timeout = p->idle_timeout;
	break;

    case 2:
	/* �f�o�C�X���C���̎�� */
	data->dev_type = p->type;

	/* �x���_�� */
	if(p->vendor != NULL) strcpy(data->vendor, p->vendor);

	/* �v���_�N�g�� */
	if(p->product != NULL) strcpy(data->product, p->product);

	/* �C�[�T�l�b�g�ڑ��@��̓��샂�[�h */
	data->phy_config = p->phy_config;

	/* �ǉ� AT �R�}���h */
	if(p->chat_additional != NULL){
	    if((r = sceNetCnfConvS2A(p->chat_additional, data->chat_additional, sizeof(data->chat_additional))) < 0) return (r);
	}

	/* �O�����M�ԍ��ݒ� */
	if(p->outside_number != NULL) strcpy(data->outside_number, p->outside_number);
	if(p->outside_delay != NULL) strcpy(data->outside_delay, p->outside_delay);

	/* �_�C�A�����@ */
	data->dialing_type = p->dialing_type;

	/* ����ؒf�ݒ� */
	data->dev_idle_timeout = p->idle_timeout;
	break;
    }

    return (r);
}

static int get_net(sceNetcnfifData_t *data, sceNetCnfRoot_t *p)
{
    int r = 0;
    sceNetCnfPair_t *pair;

    /* ��ԍŌ�� sceNetCnfPair �̐ݒ肪 sceNetcnfifData �Ɏc�� */
    for(pair = p->pair_head; pair != NULL; pair = pair->forw){
	/* sceNetcnfifData ������������ */
	sceNetcnfifDataInit(data);

	/* �t�@�C�������擾 */
	strcpy(data->attach_ifc, pair->attach_ifc);
	strcpy(data->attach_dev, pair->attach_dev);

	/* ATTACH_CNF �̓��e���擾 */
	if(pair->ifc){
	    r = get_attach(data, pair->ifc, 1);
	}
	if(pair->dev){
	    r = get_attach(data, pair->dev, 2);
	}
    }

    return (r);
}

int sceNetcnfifReadEnv(sceNetcnfifData_t *data, sceNetCnfEnv_t *e, int type)
{
    int r = 0;

    switch(type){
    case 0:
	/* NET_CNF �� ATTACH_CNF �� sceNetCnfEnv ���� sceNetcnfifData �֓ǂݍ��� */
	r = get_net(data, e->root);
	break;
    case 1:
    case 2:
	/* ATTACH_CNF �� sceNetCnfEnv ���� sceNetcnfifData �֓ǂݍ��� */
	r = get_attach(data, e->ifc, type);
	break;
    default:
	printf("[%s] unknown type (%d)\n", __FUNCTION__, type);
	break;
    }

    return (r);
}
