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

#define DEBUG /* デバッグプリントを表示しない場合はコメントアウトして下さい */

/* 組み合わせを設定する */
void set_net_cnf(sceNetcnfifData_t *data, char *ifc, char *dev)
{
    strcpy(data->attach_ifc, ifc);
    strcpy(data->attach_dev, dev);

#ifdef DEBUG
    sceNetcnfifDataDump(data);
#endif /* DEBUG */
}

/* 接続プロバイダ設定あるいは接続機器設定を設定する */
void set_attach_cnf(sceNetcnfifData_t *data, int type, int pattern)
{
    switch(type){
    case sceNetcnfifArg_type_dev:
	switch(pattern){
	case 0: /* 接続機器として SCE 製ネットワークアダプタのイーサネットアダプタ
		   を使用する設定 */
	    // data->dev_type = sceNetcnfifData_type_nic;
	    // strcpy(data->vendor, "SCE");
	    // strcpy(data->product, "Ethernet (Network Adaptor)");
	    data->phy_config = sceNetcnfifData_phy_config_auto;
	    break;
	case 1: /* 接続機器として SCE 製ネットワークアダプタ以外のイーサネットアダプタ
		   を使用する設定 */
	    // data->dev_type = sceNetcnfifData_type_eth;
	    // strcpy(data->vendor, "ethernet-vendor");
	    // strcpy(data->product, "ethernet-product");
	    data->phy_config = sceNetcnfifData_phy_config_auto;
	    break;
	case 2: /* 接続機器としてモデム・ TA を使用する設定 */
	    // data->dev_type = sceNetcnfifData_type_ppp;
	    // strcpy(data->vendor, "modem-vendor");
	    // strcpy(data->product, "modem-product");
	    data->dialing_type = sceNetcnfifData_dialing_type_tone;
	    strcpy(data->chat_additional, "ATM0");
	    strcpy(data->outside_number, "0");
	    strcpy(data->outside_delay, ",,,");
	    data->dev_idle_timeout = 600; /* 10分 == 600 秒 */
	    break;
	default:
	    scePrintf("[%s] unknown pattern (%d)\n", __FILE__, pattern);
	    break;
	}
	break;
    case sceNetcnfifArg_type_ifc:
	switch(pattern){
	case 0: /* 接続機器として SCE 製ネットワークアダプタのイーサネットアダプタ
		   を使用し、DHCP を使用しない設定 */
	    data->ifc_type = sceNetcnfifData_type_nic;
	    data->dhcp = sceNetcnfifData_dhcp_no_use;
	    strcpy(data->address, "192.168.0.1");
	    strcpy(data->netmask, "255.255.255.0");
	    strcpy(data->gateway, "192.168.0.2");
	    strcpy(data->dns1_address, "192.168.0.3");
	    strcpy(data->dns2_address, "192.168.0.4");
	    break;
	case 1: /* 接続機器として SCE 製ネットワークアダプタのイーサネットアダプタ
		   を使用し、DHCP を使用する設定 */
	    data->ifc_type = sceNetcnfifData_type_nic;
	    data->dhcp = sceNetcnfifData_dhcp_use;
	    strcpy(data->dns1_address, "192.168.0.1");
	    strcpy(data->dns2_address, "192.168.0.2");
	    strcpy(data->dhcp_host_name, "host_name");
	    break;
	case 2: /* 接続機器として SCE 製ネットワークアダプタ以外のイーサネットアダプタ
		   を使用し、DHCP を使用しない設定 */
	    data->ifc_type = sceNetcnfifData_type_eth;
	    data->dhcp = sceNetcnfifData_dhcp_no_use;
	    strcpy(data->address, "192.168.0.1");
	    strcpy(data->netmask, "255.255.255.0");
	    strcpy(data->gateway, "192.168.0.2");
	    strcpy(data->dns1_address, "192.168.0.3");
	    strcpy(data->dns2_address, "192.168.0.4");
	    break;
	case 3: /* 接続機器として SCE 製ネットワークアダプタ以外のイーサネットアダプタ
		   を使用し、DHCP を使用する設定 */
	    data->ifc_type = sceNetcnfifData_type_eth;
	    data->dhcp = sceNetcnfifData_dhcp_use;
	    strcpy(data->dns1_address, "192.168.0.1");
	    strcpy(data->dns2_address, "192.168.0.2");
	    strcpy(data->dhcp_host_name, "host_name");
	    break;
	case 4: /* 接続機器としてイーサネットアダプタを使用し、PPPoEを使用し、
		   DNSサーバアドレスを自動取得する設定 */
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
	case 5: /* 接続機器としてイーサネットアダプタを使用し、PPPoEを使用し、
		   DNSサーバアドレスを自動取得しない設定 */
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
	case 6: /* 接続機器としてモデム・ TA を使用し、DNS サーバアドレスを
		   自動取得する設定 */
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
	case 7: /* 接続機器としてモデム・ TA を使用し、DNS サーバアドレスを
		   自動取得しない設定 */
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
