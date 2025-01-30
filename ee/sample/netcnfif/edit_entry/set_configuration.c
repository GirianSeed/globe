/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/* 
 *      Netcnf Interface Library Sample Program
 *
 *                         Version 1.3
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         set_configuration.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 *       1.2            2002.01.31      tetsu       Modified to set none on dev_type, vendor, product
 *       1.3            2002.02.10      tetsu       Modified chat_additional
 */

#include <string.h>
#include <eekernel.h>
#include <netcnfif.h>

#define DEBUG /* �f�o�b�O�v�����g��\�����Ȃ��ꍇ�̓R�����g�A�E�g���ĉ����� */

/* �g�ݍ��킹��ݒ肷�� */
void set_net_cnf(sceNetcnfifData_t *data, char *ifc, char *dev)
{
    strcpy(data->attach_ifc, ifc);
    strcpy(data->attach_dev, dev);

#ifdef DEBUG
    sceNetcnfifDataDump(data);
#endif /* DEBUG */
}

/* �ڑ��v���o�C�_�ݒ肠�邢�͐ڑ��@��ݒ��ݒ肷�� */
void set_attach_cnf(sceNetcnfifData_t *data, int type, int pattern)
{
    switch(type){
    case sceNetcnfifArg_type_dev:
	switch(pattern){
	case 0: /* �ڑ��@��Ƃ��� SCE ���l�b�g���[�N�A�_�v�^�̃C�[�T�l�b�g�A�_�v�^
		   ���g�p����ݒ� */
	    // data->dev_type = sceNetcnfifData_type_nic;
	    // strcpy(data->vendor, "SCE");
	    // strcpy(data->product, "Ethernet (Network Adaptor)");
	    data->phy_config = sceNetcnfifData_phy_config_auto;
	    break;
	case 1: /* �ڑ��@��Ƃ��� SCE ���l�b�g���[�N�A�_�v�^�ȊO�̃C�[�T�l�b�g�A�_�v�^
		   ���g�p����ݒ� */
	    // data->dev_type = sceNetcnfifData_type_eth;
	    // strcpy(data->vendor, "ethernet-vendor");
	    // strcpy(data->product, "ethernet-product");
	    data->phy_config = sceNetcnfifData_phy_config_auto;
	    break;
	case 2: /* �ڑ��@��Ƃ��ă��f���E TA ���g�p����ݒ� */
	    // data->dev_type = sceNetcnfifData_type_ppp;
	    // strcpy(data->vendor, "modem-vendor");
	    // strcpy(data->product, "modem-product");
	    data->dialing_type = sceNetcnfifData_dialing_type_tone;
	    strcpy(data->chat_additional, "ATM0");
	    strcpy(data->outside_number, "0");
	    strcpy(data->outside_delay, ",,,");
	    data->dev_idle_timeout = 600; /* 10�� == 600 �b */
	    break;
	default:
	    scePrintf("[%s] unknown pattern (%d)\n", __FILE__, pattern);
	    break;
	}
	break;
    case sceNetcnfifArg_type_ifc:
	switch(pattern){
	case 0: /* �ڑ��@��Ƃ��� SCE ���l�b�g���[�N�A�_�v�^�̃C�[�T�l�b�g�A�_�v�^
		   ���g�p���ADHCP ���g�p���Ȃ��ݒ� */
	    data->ifc_type = sceNetcnfifData_type_nic;
	    data->dhcp = sceNetcnfifData_dhcp_no_use;
	    strcpy(data->address, "192.168.0.1");
	    strcpy(data->netmask, "255.255.255.0");
	    strcpy(data->gateway, "192.168.0.2");
	    strcpy(data->dns1_address, "192.168.0.3");
	    strcpy(data->dns2_address, "192.168.0.4");
	    break;
	case 1: /* �ڑ��@��Ƃ��� SCE ���l�b�g���[�N�A�_�v�^�̃C�[�T�l�b�g�A�_�v�^
		   ���g�p���ADHCP ���g�p����ݒ� */
	    data->ifc_type = sceNetcnfifData_type_nic;
	    data->dhcp = sceNetcnfifData_dhcp_use;
	    strcpy(data->dns1_address, "192.168.0.1");
	    strcpy(data->dns2_address, "192.168.0.2");
	    strcpy(data->dhcp_host_name, "host_name");
	    break;
	case 2: /* �ڑ��@��Ƃ��� SCE ���l�b�g���[�N�A�_�v�^�ȊO�̃C�[�T�l�b�g�A�_�v�^
		   ���g�p���ADHCP ���g�p���Ȃ��ݒ� */
	    data->ifc_type = sceNetcnfifData_type_eth;
	    data->dhcp = sceNetcnfifData_dhcp_no_use;
	    strcpy(data->address, "192.168.0.1");
	    strcpy(data->netmask, "255.255.255.0");
	    strcpy(data->gateway, "192.168.0.2");
	    strcpy(data->dns1_address, "192.168.0.3");
	    strcpy(data->dns2_address, "192.168.0.4");
	    break;
	case 3: /* �ڑ��@��Ƃ��� SCE ���l�b�g���[�N�A�_�v�^�ȊO�̃C�[�T�l�b�g�A�_�v�^
		   ���g�p���ADHCP ���g�p����ݒ� */
	    data->ifc_type = sceNetcnfifData_type_eth;
	    data->dhcp = sceNetcnfifData_dhcp_use;
	    strcpy(data->dns1_address, "192.168.0.1");
	    strcpy(data->dns2_address, "192.168.0.2");
	    strcpy(data->dhcp_host_name, "host_name");
	    break;
	case 4: /* �ڑ��@��Ƃ��ăC�[�T�l�b�g�A�_�v�^���g�p���APPPoE���g�p���A
		   DNS�T�[�o�A�h���X�������擾����ݒ� */
	    data->ifc_type = sceNetcnfifData_type_ppp;
	    strcpy(data->peer_name, "*");
	    data->f_auth = sceNetcnfifData_f_auth_on;
	    data->dhcp = sceNetcnfifData_dhcp_no_use;
	    strcpy(data->auth_name, "userid");
	    strcpy(data->auth_key, "password");
	    data->dns1_nego = sceNetcnfifData_dns_nego_on;
	    data->dns2_nego = sceNetcnfifData_dns_nego_on;
	    data->prc_nego = sceNetcnfifData_prc_nego_off;
	    data->acc_nego = sceNetcnfifData_acc_nego_off;
	    data->accm_nego = sceNetcnfifData_accm_nego_off;
	    data->mtu = sceNetcnfifData_mtu_default;
	    data->pppoe = sceNetcnfifData_pppoe_use;
	    data->ifc_idle_timeout = 0;
	    break;
	case 5: /* �ڑ��@��Ƃ��ăC�[�T�l�b�g�A�_�v�^���g�p���APPPoE���g�p���A
		   DNS�T�[�o�A�h���X�������擾���Ȃ��ݒ� */
	    data->ifc_type = sceNetcnfifData_type_ppp;
	    strcpy(data->peer_name, "*");
	    data->f_auth = sceNetcnfifData_f_auth_on;
	    data->dhcp = sceNetcnfifData_dhcp_no_use;
	    strcpy(data->auth_name, "userid");
	    strcpy(data->auth_key, "password");
	    strcpy(data->dns1_address, "192.168.0.1");
	    strcpy(data->dns2_address, "192.168.0.2");
	    data->prc_nego = sceNetcnfifData_prc_nego_off;
	    data->acc_nego = sceNetcnfifData_acc_nego_off;
	    data->accm_nego = sceNetcnfifData_accm_nego_off;
	    data->mtu = sceNetcnfifData_mtu_default;
	    data->pppoe = sceNetcnfifData_pppoe_use;
	    data->ifc_idle_timeout = 0;
	    break;
	case 6: /* �ڑ��@��Ƃ��ă��f���E TA ���g�p���ADNS �T�[�o�A�h���X��
		   �����擾����ݒ� */
	    data->ifc_type = sceNetcnfifData_type_ppp;
	    strcpy(data->peer_name, "*");
	    data->f_auth = sceNetcnfifData_f_auth_on;
	    data->dhcp = sceNetcnfifData_dhcp_no_use;
	    strcpy(data->auth_name, "userid");
	    strcpy(data->auth_key, "password");
	    strcpy(data->phone_numbers1, "11-1111-1111");
	    strcpy(data->phone_numbers2, "22-2222-2222");
	    strcpy(data->phone_numbers3, "33-3333-3333");
	    data->dns1_nego = sceNetcnfifData_dns_nego_on;
	    data->dns2_nego = sceNetcnfifData_dns_nego_on;
	    break;
	case 7: /* �ڑ��@��Ƃ��ă��f���E TA ���g�p���ADNS �T�[�o�A�h���X��
		   �����擾���Ȃ��ݒ� */
	    data->ifc_type = sceNetcnfifData_type_ppp;
	    strcpy(data->peer_name, "*");
	    data->f_auth = sceNetcnfifData_f_auth_on;
	    data->dhcp = sceNetcnfifData_dhcp_no_use;
	    strcpy(data->auth_name, "userid");
	    strcpy(data->auth_key, "password");
	    strcpy(data->phone_numbers1, "11-1111-1111");
	    strcpy(data->phone_numbers2, "22-2222-2222");
	    strcpy(data->phone_numbers3, "33-3333-3333");
	    strcpy(data->dns1_address, "192.168.0.1");
	    strcpy(data->dns2_address, "192.168.0.2");
	    break;
	default:
	    scePrintf("[%s] unknown pattern (%d)\n", __FILE__, pattern);
	    break;
	}
	break;
    default:
	scePrintf("[%s] unknown type (%d)\n", __FILE__, type);
	break;
    }

#ifdef DEBUG
    sceNetcnfifDataDump(data);
#endif /* DEBUG */
}
