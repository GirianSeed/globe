/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: netcnfif_data.c,v 1.3 2003/09/05 07:45:43 ksh Exp $
 */
/* 
 *      Netcnf Interface Library
 *
 *                         Version 1.2
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
 *       1.2            2002.10.19      tetsu       Change path of netcnfif.h
 */

#include <eekernel.h>
#include <string.h>
#include <netcnfif.h>

/* sceNetcnfifData 構造体を初期化する */
void sceNetcnfifDataInit(sceNetcnfifData_t *data)
{
    memset(data, 0, sizeof(sceNetcnfifData_t));

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
    scePrintf("-----------------------\n");
    scePrintf("attach_ifc       : \"%s\"\n", data->attach_ifc);
    scePrintf("attach_dev       : \"%s\"\n", data->attach_dev);
    scePrintf("dhcp_host_name   : \"%s\"\n", data->dhcp_host_name);
    scePrintf("address          : \"%s\"\n", data->address);
    scePrintf("netmask          : \"%s\"\n", data->netmask);
    scePrintf("gateway          : \"%s\"\n", data->gateway);
    scePrintf("dns1_address     : \"%s\"\n", data->dns1_address);
    scePrintf("dns2_address     : \"%s\"\n", data->dns2_address);
    scePrintf("phone_numbers1   : \"%s\"\n", data->phone_numbers1);
    scePrintf("phone_numbers2   : \"%s\"\n", data->phone_numbers2);
    scePrintf("phone_numbers3   : \"%s\"\n", data->phone_numbers3);
    scePrintf("auth_name        : \"%s\"\n", data->auth_name);
    scePrintf("auth_key         : \"%s\"\n", data->auth_key);
    scePrintf("peer_name        : \"%s\"\n", data->peer_name);
    scePrintf("vendor           : \"%s\"\n", data->vendor);
    scePrintf("product          : \"%s\"\n", data->product);
    scePrintf("chat_additional  : \"%s\"\n", data->chat_additional);
    scePrintf("outside_number   : \"%s\"\n", data->outside_number);
    scePrintf("outside_delay    : \"%s\"\n", data->outside_delay);
    scePrintf("ifc_type         : \"%d\"\n", data->ifc_type);
    scePrintf("mtu              : \"%d\"\n", data->mtu);
    scePrintf("ifc_idle_timeout : \"%d\"\n", data->ifc_idle_timeout);
    scePrintf("dev_type         : \"%d\"\n", data->dev_type);
    scePrintf("phy_config       : \"%d\"\n", data->phy_config);
    scePrintf("dialing_type     : \"%d\"\n", data->dialing_type);
    scePrintf("dev_idle_timeout : \"%d\"\n", data->dev_idle_timeout);
    scePrintf("dhcp             : \"%d\"\n", data->dhcp);
    scePrintf("dns1_nego        : \"%d\"\n", data->dns1_nego);
    scePrintf("dns2_nego        : \"%d\"\n", data->dns2_nego);
    scePrintf("f_auth           : \"%d\"\n", data->f_auth);
    scePrintf("auth             : \"%d\"\n", data->auth);
    scePrintf("pppoe            : \"%d\"\n", data->pppoe);
    scePrintf("prc_nego         : \"%d\"\n", data->prc_nego);
    scePrintf("acc_nego         : \"%d\"\n", data->acc_nego);
    scePrintf("accm_nego        : \"%d\"\n", data->accm_nego);
    scePrintf("-----------------------\n");
}
