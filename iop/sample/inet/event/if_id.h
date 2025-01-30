/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *      Inet Event Sample Program
 *
 *                         Version 1.2
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         if_id.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2001.12.28      tetsu       First version
 *       1.2            2002.02.05      tetsu       Add flag's enum
 *                                                  Add search_if_id()
 *                                                  Add search_eth_if_id()
 *                                                  Add get_if_id_flag()
 *                                                  Change add_if_id()
 */

#ifndef __if_id_h_
#define __if_id_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* flag */
enum
{
    PPP,           /* デバイスレイヤの種別が ppp で、PPPoE を使用しない */
    PPP_and_PPPoE, /* デバイスレイヤの種別が ppp で、PPPoE を使用する */
    ARP            /* デバイスレイヤの種別が eth あるいは nic */
};

int search_if_id(int id);
int search_eth_if_id(int id);
int get_if_id_stat(int id);
int get_if_id_flag(int id);
int add_if_id(int id, int stat, int flag, unsigned char bus_type, unsigned char *bus_loc, char *vendor, char *product);
int change_if_id(int id, int stat);
int delete_if_id(int id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__if_id_h_ */
