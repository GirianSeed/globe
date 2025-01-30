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

/* �C�x���g */
#define IEV_None       (-1) /* �����C�x���g���������Ă��Ȃ� */
#define IEV_Attach     (1)  /* �ڑ��@�킪�ڑ����ꂽ */
#define IEV_Detach     (2)  /* �ڑ��@�킪�����ꂽ */
#define IEV_Start      (3)  /* sceINETCTL_IEV_Start �Ɠ��` */
#define IEV_Stop       (4)  /* sceINETCTL_IEV_Stop �Ɠ��` */
#define IEV_Error      (5)  /* sceINETCTL_IEV_Error �Ɠ��`(Busy_Retry, Timeout, Authfailed, Busy ������) */
#define IEV_Conf       (6)  /* sceINETCTL_IEV_Conf �Ɠ��` */
#define IEV_NoConf     (7)  /* sceINETCTL_IEV_NoConf �Ɠ��` */
#define IEV_Up         (8)  /* Up �\�ȃC���^�t�F�[�X�� Up �v�������� */
#define IEV_Down       (9)  /* Down �\�ȃC���^�t�F�[�X�� Down �v�������� */
#define IEV_Retry      (10) /* sceINETCTL_IEV_Retry �Ɠ��` */
#define IEV_Busy_Retry (11) /* �b�����Ȃ̂Ŏ��̐ڑ���d�b�ԍ��Ƀ_�C�A�������� */
#define IEV_Timeout    (12) /* �^�C���A�E�g�Őؒf���� */
#define IEV_Authfailed (13) /* �F�؎��s */
#define IEV_Busy       (14) /* �b����(�ڑ��@��Ɉˑ����܂�) */
#define IEV_NoCarrier  (15) /* ����̐ڑ��@�킪�������Ȃ��A���邢�͉�����ڑ�����Ă��Ȃ�(�ڑ��@��Ɉˑ����܂�) */

/* Sifrpc �p */
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
