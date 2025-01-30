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
 *                         common.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.02.18      tetsu       Change directory
 */

#ifndef __common_h_
#define __common_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* イベント */
#define IEV_None       (-1) /* 何もイベントが発生していない */
#define IEV_Attach     (1)  /* 接続機器が接続された */
#define IEV_Detach     (2)  /* 接続機器が抜かれた */
#define IEV_Start      (3)  /* sceINETCTL_IEV_Start と同義 */
#define IEV_Stop       (4)  /* sceINETCTL_IEV_Stop と同義 */
#define IEV_Error      (5)  /* sceINETCTL_IEV_Error と同義(Busy_Retry, Timeout, Authfailed, Busy を除く) */
#define IEV_Conf       (6)  /* sceINETCTL_IEV_Conf と同義 */
#define IEV_NoConf     (7)  /* sceINETCTL_IEV_NoConf と同義 */
#define IEV_Up         (8)  /* Up 可能なインタフェースに Up 要求をした */
#define IEV_Down       (9)  /* Down 可能なインタフェースに Down 要求をした */
#define IEV_Retry      (10) /* sceINETCTL_IEV_Retry と同義 */
#define IEV_Busy_Retry (11) /* 話し中なので次の接続先電話番号にダイアル準備中 */
#define IEV_Timeout    (12) /* タイムアウトで切断した */
#define IEV_Authfailed (13) /* 認証失敗 */
#define IEV_Busy       (14) /* 話し中(接続機器に依存します) */
#define IEV_NoCarrier  (15) /* 相手の接続機器が応答しない、あるいは回線が接続されていない(接続機器に依存します) */

/* Sifrpc 用 */
#define SSIZE      (4096)
#define GET_EVENT  (0x11)
#define UP_DOWN    (0x12)
#define WAIT_EVENT (0)
#define UP         (0)
#define DOWN       (1)

typedef struct rpc_data{
    int interface_id;
    int event;
    int no_decode;
    int addr;
    char combination_name[256];
    char db_path[256];
    char ip_addr[256];
    char line_speed[256];
    char phone_number[256];
} rpc_data_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__common_h_ */
