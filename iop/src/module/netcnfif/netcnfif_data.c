/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/* 
 *      Netcnf Interface Library
 *
 *                         Version 1.1
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         netcnfif_data.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 */

#include <stdio.h>
#include <string.h>
#include <netcnf.h>

#include "../../../../common/include/netcnfif.h"

/* sceNetcnfifData 構造体を初期化する */
void sceNetcnfifDataInit(sceNetcnfifData_t *data)
{
    bzero(data, sizeof(sceNetcnfifData_t));

    data->ifc_type = sceNetcnfifData_type_no_set;
    data->mtu = sceNetcnfifData_mtu_no_set;
    data->ifc_idle_timeout = sceNetcnfifData_idle_timeout_no_set;
    data->dev_type = sceNetcnfifData_type_no_set;
    data->phy_config = sceNetcnfifData_phy_config_no_set;
    data->dialing_type = sceNetcnfifData_dialing_type_no_set;
    data->dev_idle_timeout = sceNetcnfifData_idle_timeout_no_set;
    data->dhcp = sceNetcnfifData_dhcp_no_set;
    data->dns1_nego = sceNetcnfifData_dns_nego_no_set;
    data->dns2_nego = sceNetcnfifData_dns_nego_no_set;
    data->f_auth = sceNetcnfifData_f_auth_off;
    data->auth = sceNetcnfifData_auth_chap_pap;/* あなたのネットワーク設定ファイルでは
						  chap/pap モード以外になり得ないので chap/pap に設定しておく */
    data->pppoe = sceNetcnfifData_pppoe_no_set;
    data->prc_nego = sceNetcnfifData_prc_nego_no_set;
    data->acc_nego = sceNetcnfifData_acc_nego_no_set;
    data->accm_nego = sceNetcnfifData_accm_nego_no_set;
}

/* sceNetcnfifData 構造体をダンプする */
void sceNetcnfifDataDump(sceNetcnfifData_t *data)
{
    printf("-----------------------\n");
    printf("attach_ifc       : \"%s\"\n", data->attach_ifc);
    printf("attach_dev       : \"%s\"\n", data->attach_dev);
    printf("dhcp_host_name   : \"%s\"\n", data->dhcp_host_name);
    printf("address          : \"%s\"\n", data->address);
    printf("netmask          : \"%s\"\n", data->netmask);
    printf("gateway          : \"%s\"\n", data->gateway);
    printf("dns1_address     : \"%s\"\n", data->dns1_address);
    printf("dns2_address     : \"%s\"\n", data->dns2_address);
    printf("phone_numbers1   : \"%s\"\n", data->phone_numbers1);
    printf("phone_numbers2   : \"%s\"\n", data->phone_numbers2);
    printf("phone_numbers3   : \"%s\"\n", data->phone_numbers3);
    printf("auth_name        : \"%s\"\n", data->auth_name);
    printf("auth_key         : \"%s\"\n", data->auth_key);
    printf("peer_name        : \"%s\"\n", data->peer_name);
    printf("vendor           : \"%s\"\n", data->vendor);
    printf("product          : \"%s\"\n", data->product);
    printf("chat_additional  : \"%s\"\n", data->chat_additional);
    printf("outside_number   : \"%s\"\n", data->outside_number);
    printf("outside_delay    : \"%s\"\n", data->outside_delay);
    printf("ifc_type         : \"%d\"\n", data->ifc_type);
    printf("mtu              : \"%d\"\n", data->mtu);
    printf("ifc_idle_timeout : \"%d\"\n", data->ifc_idle_timeout);
    printf("dev_type         : \"%d\"\n", data->dev_type);
    printf("phy_config       : \"%d\"\n", data->phy_config);
    printf("dialing_type     : \"%d\"\n", data->dialing_type);
    printf("dev_idle_timeout : \"%d\"\n", data->dev_idle_timeout);
    printf("dhcp             : \"%d\"\n", data->dhcp);
    printf("dns1_nego        : \"%d\"\n", data->dns1_nego);
    printf("dns2_nego        : \"%d\"\n", data->dns2_nego);
    printf("f_auth           : \"%d\"\n", data->f_auth);
    printf("auth             : \"%d\"\n", data->auth);
    printf("pppoe            : \"%d\"\n", data->pppoe);
    printf("prc_nego         : \"%d\"\n", data->prc_nego);
    printf("acc_nego         : \"%d\"\n", data->acc_nego);
    printf("accm_nego        : \"%d\"\n", data->accm_nego);
    printf("-----------------------\n");
}
