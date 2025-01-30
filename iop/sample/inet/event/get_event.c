/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *      Inet Event Sample Program
 *
 *                         Version 1.6
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         get_event.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2001.12.28      tetsu       First version
 *       1.2            2002.01.29      tetsu       Add IEV_NoCarrier
 *                                                  Change how to copy return value
 *                                                  Change how to print scePPPCC_GetCurrentStatus's result
 *                                                  Change check_flag()
 *                                                  Add IEV_Busy, IEV_NoCarrier's comments
 *                                                  Add IEV_Busy_Retry, IEV_Timeout, IEV_Authfailed,
 *                                                      IEV_Busy, IEV_NoCarrier's debug print
 *       1.3            2002.02.04      tetsu       Modified scePPPCC_GetLastResultString's debug print
 *       1.4            2002.02.05      tetsu       Change check_flag()
 *       1.5            2002.02.05      tetsu       Correspond to PPPoE
 *       1.6            2002.02.18      tetsu       Change path of common.h
 */

#include <stdio.h>
#include <string.h>
#include <kernel.h>
#include <sifrpc.h>
#include <netcnf.h>
#include <inet/inet.h>
#include <inet/inetctl.h>
#include <inet/pppctl.h>
#include <inet/netdev.h>
#include <usbmload.h>

#include "../../../../ee/sample/inet/original/event/common.h"
#include "if_id.h"

#define DEBUG /* �f�o�b�O�v�����g��\�����Ȃ��ꍇ��
		 �R�����g�A�E�g���ĉ����� */

/* �C�x���g�t���O ID */
static int event_id;

/* �C�x���g�n���h���p */
#define HANDLER_RESULT_NUM_MAX (10) /* �ۑ��ł���C�x���g�̍ő吔 */

static sceInetCtlEventHandlers_t eh;
static int handler_result_num;                          /* �ۑ�����Ă���C�x���g�̐� */
static int handler_result_type[HANDLER_RESULT_NUM_MAX]; /* �C�x���g��ۑ�����o�b�t�@ */
static int handler_result_id  [HANDLER_RESULT_NUM_MAX]; /* �C�x���g��ۑ�����o�b�t�@ */
static int interface_id;                                /* �C���^�t�F�[�X ID */

static char line_speed[256];   /* ������x */
static char ip_addr[256];      /* IP �A�h���X */
static char phone_number[256]; /* �ڑ���d�b�ԍ� */

/* �t���O�̃`�F�b�N */
static int check_flag(int id)
{
    static char tmp_str[256];
    static int flag;
    int r = ARP;

    sceInetInterfaceControl(id, sceInetCC_GetFlags, &flag, sizeof(flag));

    if(flag & sceInetDevF_PPP){
	sceInetInterfaceControl(id, sceInetCC_GetModuleName, tmp_str, sizeof(tmp_str));
	if(strcmp(tmp_str, "ppp,pppoe") == 0){
	    r = PPP_and_PPPoE;
	}else{
	    r = PPP;
	}
    }

    return (r);
}

/* �o�X��ʂ̃`�F�b�N */
static unsigned char check_bus_type(int id)
{
    static unsigned char bus_type;

    sceInetInterfaceControl(id, sceInetCC_GetBusType, &bus_type, sizeof(bus_type));

    return (bus_type);
}

/* �f�o�C�X�ʒu���̃`�F�b�N */
static unsigned char *check_bus_loc(int id)
{
    static unsigned char bus_loc[31];

    bzero(bus_loc, sizeof(bus_loc));
    if(check_bus_type(id) == sceInetBus_USB){
	sceInetInterfaceControl(id, sceInetCC_GetBusLoc, bus_loc, sizeof(bus_loc));
    }

    return (bus_loc);
}

/* �x���_���̃`�F�b�N */
static char *check_vendor(int id)
{
    static char vendor[256];

    bzero(vendor, sizeof(vendor));
    sceInetInterfaceControl(id, sceInetCC_GetVendorName, vendor, sizeof(vendor));

    return (vendor);
}

/* �v���_�N�g���̃`�F�b�N */
static char *check_product(int id)
{
    static char product[256];

    bzero(product, sizeof(product));
    sceInetInterfaceControl(id, sceInetCC_GetDeviceName, product, sizeof(product));

    return (product);
}

/* �C�x���g�n���h�� */
static void event_handler(int id, int type)
{
    if(interface_id == -1) goto TYPE_CHECK;
    if(interface_id == id){
 TYPE_CHECK:
	/* �����̃C�x���g�ɑΉ�(��x�� HANDLER_RESULT_NUM_MAX �܂ŁA����ȏ�͕ۏ؂��Ă܂���) */
	if(0 < ++handler_result_num){
	    int i;
	    for(i = handler_result_num - 1; 0 <= i; i--){
		handler_result_type[i + 1] = handler_result_type[i];
		handler_result_id[i + 1]   = handler_result_id[i];
	    }
	}

	/* �C���^�t�F�[�X ID ���`�F�b�N */
	handler_result_id[0] = id;

	/* �C�x���g�^�C�v���`�F�b�N */
	switch(type){
	case sceINETCTL_IEV_Attach:
	    {
		if(add_if_id(id, IEV_Attach, check_flag(id), check_bus_type(id), check_bus_loc(id),
			     check_vendor(id), check_product(id)) < 0){
		    handler_result_type[0] = IEV_None;
		}else{
		    if(get_if_id_flag(id) == PPP_and_PPPoE){
			handler_result_type[0] = IEV_None;
		    }else{
			handler_result_type[0] = IEV_Attach;
		    }
		}
	    }
	    break;
	case sceINETCTL_IEV_Detach:
	    {
		if(search_if_id(id) < 0){
		    handler_result_type[0] = IEV_None;
		}else{
		    if(get_if_id_flag(id) == PPP_and_PPPoE){
			handler_result_type[0] = IEV_None;
			delete_if_id(id);
		    }else{
			handler_result_type[0] = IEV_Detach;
			delete_if_id(id);
		    }
		}
	    }
	    break;
	case sceINETCTL_IEV_Start:
	    {
		static sceInetAddress_t address;
		static char tmp_str[256];
		int i, j;

		if(change_if_id(id, IEV_Start) < 0){
		    handler_result_type[0] = IEV_None;
		}else{
		    handler_result_type[0] = IEV_Start;

		    /* IP �A�h���X���擾 */
		    bzero(ip_addr, sizeof(ip_addr));
		    sceInetInterfaceControl(id, sceInetCC_GetAddress, &address, sizeof(sceInetAddress_t));
		    sceInetAddress2String(ip_addr, sizeof(ip_addr), &address);

		    /* ������x���擾 */
		    bzero(line_speed, sizeof(line_speed));
		    if(get_if_id_flag(id) == PPP){
			sceInetInterfaceControl(id, scePPPCC_GetLastResultString, tmp_str, sizeof(tmp_str));
#ifdef DEBUG
			printf("[scePPPCC_GetLastResultString] %s\n", tmp_str);
#endif /* DEBUG */
			for(i = 0, j = 0; tmp_str[i] != '\0'; i++){
			    /* ������x�����擾���� */
			    if('0' <= tmp_str[i] && tmp_str[i] <= '9'){
				line_speed[j++] = tmp_str[i];
			    }
			    /* ������x�擾��A�X�y�[�X�A���s�A������������ break */
			    if(j){
				if(tmp_str[i] == ' ' || tmp_str[i] == '\t' ||
				   tmp_str[i] == '\n' || tmp_str[i] == '\r' || tmp_str[i] == '\0')
				    break;
			    }
			}
			line_speed[j] = '\0';
		    }

		    /* PPPoE �p ppp I/F �̏ꍇ�A�����Ɏg�p���Ă��� eth I/F �� id �ɏ��������� */
		    if(get_if_id_flag(id) == PPP_and_PPPoE){
			handler_result_id[0] = search_eth_if_id(id);
		    }
		}
	    }
	    break;
	case sceINETCTL_IEV_Stop:
	    {
		if(change_if_id(id, IEV_Stop) < 0){
		    handler_result_type[0] = IEV_None;
		}else{
		    handler_result_type[0] = IEV_Stop;

		    /* PPPoE �p ppp I/F �̏ꍇ�A�����Ɏg�p���Ă��� eth I/F �� id �ɏ��������� */
		    if(get_if_id_flag(id) == PPP_and_PPPoE){
			handler_result_id[0] = search_eth_if_id(id);
		    }
		}
	    }
	    break;
	case sceINETCTL_IEV_Error:
	    {
		static int pstate;
		static char tmp_str[256];

		if(search_if_id(id) < 0){
		    handler_result_type[0] = IEV_None;
		}else{
		    /* ���_�C�A������ IEV_Error ��Ԃ��Ȃ� */
		    sceInetCtlGetState(id, &pstate);
		    switch(pstate){
		    case sceINETCTL_S_STOPPED:
			bzero(tmp_str, sizeof(tmp_str));
			if(get_if_id_flag(id) == PPP || get_if_id_flag(id) == PPP_and_PPPoE){
			    sceInetInterfaceControl(id, scePPPCC_GetCurrentStatus, tmp_str, sizeof(tmp_str));
#ifdef DEBUG
			    printf("[scePPPCC_GetCurrentStatus] %s\n", tmp_str);
#endif /* DEBUG */
			}
			if(strcmp(tmp_str, "TIMEOUT Dead") == 0){
			    if(change_if_id(id, IEV_Timeout) < 0){
				handler_result_type[0] = IEV_None;
			    }else{
				handler_result_type[0] = IEV_Timeout;
			    }
			}else if(strcmp(tmp_str, "AUTH Failed") == 0){
			    if(change_if_id(id, IEV_Authfailed) < 0){
				handler_result_type[0] = IEV_None;
			    }else{
				handler_result_type[0] = IEV_Authfailed;
			    }
			}else if(strcmp(tmp_str, "ABORT BUSY") == 0){ /* ���̕�����͐ڑ��@��ˑ��ł� */
			    if(change_if_id(id, IEV_Busy) < 0){
				handler_result_type[0] = IEV_None;
			    }else{
				handler_result_type[0] = IEV_Busy;
			    }
			}else if(strcmp(tmp_str, "ABORT NO CARRIER") == 0){ /* ���̕�����͐ڑ��@��ˑ��ł� */
			    if(change_if_id(id, IEV_NoCarrier) < 0){
				handler_result_type[0] = IEV_None;
			    }else{
				handler_result_type[0] = IEV_NoCarrier;
			    }
			}else{
			    if(change_if_id(id, IEV_Error) < 0){
				handler_result_type[0] = IEV_None;
			    }else{
				handler_result_type[0] = IEV_Error;
			    }
			}
			break;
		    default:
			if(change_if_id(id, IEV_Busy_Retry) < 0){
			    handler_result_type[0] = IEV_None;
			}else{
			    handler_result_type[0] = IEV_Busy_Retry;
			}
			break;
		    }

		    /* PPPoE �p ppp I/F �̏ꍇ�A�����Ɏg�p���Ă��� eth I/F �� id �ɏ��������� */
		    if(get_if_id_flag(id) == PPP_and_PPPoE){
			handler_result_id[0] = search_eth_if_id(id);
		    }
		}
	    }
	    break;
	case sceINETCTL_IEV_Up:
	    {
		static int pstate;

		/* STARTING ��ԂɈڍs���ĂȂ���� IEV_Up ��Ԃ��Ȃ� */
		sceInetCtlGetState(id, &pstate);
		switch(pstate){
		case sceINETCTL_S_STARTING:
		    if(change_if_id(id, IEV_Up) < 0){
			handler_result_type[0] = IEV_None;
		    }else{
			if(get_if_id_flag(id) == PPP_and_PPPoE){
			    handler_result_type[0] = IEV_None;
			}else{
			    handler_result_type[0] = IEV_Up;

			    /* �ڑ���d�b�ԍ����擾 */
			    bzero(phone_number, sizeof(phone_number));
			    if(get_if_id_flag(id) == PPP){
				sceInetInterfaceControl(id, scePPPCC_GetPhoneNumber, phone_number, sizeof(phone_number));
			    }
			}
		    }
		    break;
		default:
		    handler_result_type[0] = IEV_None;
		    break;
		}
	    }
	    break;
	case sceINETCTL_IEV_Down:
	    {
		/* IEV_Start, IEV_Up, IEV_Retry ��ԂɈڍs���Ă��Ȃ���� IEV_Down ��Ԃ��Ȃ� */
		switch(get_if_id_stat(id)){
		case IEV_Start:
		case IEV_Up:
		case IEV_Retry:
		case IEV_Busy_Retry:
		    if(change_if_id(id, IEV_Down) < 0){
			handler_result_type[0] = IEV_None;
		    }else{
			if(get_if_id_flag(id) == PPP_and_PPPoE){
			    handler_result_type[0] = IEV_None;
			}else{
			    handler_result_type[0] = IEV_Down;
			}
		    }
		    break;
		default:
		    handler_result_type[0] = IEV_None;
		    break;
		}
	    }
	    break;
	case sceINETCTL_IEV_Retry:
	    {
		if(change_if_id(id, IEV_Retry) < 0){
		    handler_result_type[0] = IEV_None;
		}else{
		    handler_result_type[0] = IEV_Retry;

		    /* �ڑ���d�b�ԍ����擾 */
		    bzero(phone_number, sizeof(phone_number));
		    if(get_if_id_flag(id) == PPP){
			sceInetInterfaceControl(id, scePPPCC_GetPhoneNumber, phone_number, sizeof(phone_number));
		    }
		}
	    }
	    break;
	default:
	    {
		/* ���̑��̃C�x���g�͕Ԃ��Ȃ� */
		handler_result_type[0] = IEV_None;
	    }
	    break;
	}
    }
    SetEventFlag(event_id, 0x1);
}

/* Sifrpc �T�[�o */
static void *get_event_server(u_int fno, void *data, int size)
{
    rpc_data_t *p;
    u_long result;

    p = (rpc_data_t *)data;

    switch(fno){
    case WAIT_EVENT:
	/* ������ */
	interface_id = p->interface_id;

	if(handler_result_num < 0){
	    /* �C�x���g���N����܂ő҂� */
	    WaitEventFlag(event_id, 0x1, EW_OR | EW_CLEAR, &result);
	}
	break;
    }

    /* �Ԃ�l��ݒ� */
    if(handler_result_num != -1){
	p->event        = handler_result_type[handler_result_num];
	p->interface_id = handler_result_id[handler_result_num];

	memcpy(p->line_speed, line_speed, sizeof(line_speed));
	memcpy(p->ip_addr, ip_addr, sizeof(ip_addr));
	memcpy(p->phone_number, phone_number, sizeof(phone_number));

	handler_result_num--;
    }else{
	p->event = IEV_None;
    }
#ifdef DEBUG
    switch(p->event){
    case IEV_Attach:
	printf("[WAIT_EVENT] Attach(id = %d)\n", p->interface_id);
	break;
    case IEV_Detach:
	printf("[WAIT_EVENT] Detach(id = %d)\n", p->interface_id);
	break;
    case IEV_Start:
	printf("[WAIT_EVENT] Start(id = %d) %s %s\n", p->interface_id, ip_addr, line_speed);
	break;
    case IEV_Stop:
	printf("[WAIT_EVENT] Stop(id = %d)\n", p->interface_id);
	break;
    case IEV_Error:
	printf("[WAIT_EVENT] Error(id = %d)\n", p->interface_id);
	break;
    case IEV_Conf:
	printf("[WAIT_EVENT] Conf(id = %d)\n", p->interface_id);
	break;
    case IEV_NoConf:
	printf("[WAIT_EVENT] NoConf(id = %d)\n", p->interface_id);
	break;
    case IEV_Up:
	printf("[WAIT_EVENT] Up(id = %d) %s\n", p->interface_id, phone_number);
	break;
    case IEV_Down:
	printf("[WAIT_EVENT] Down(id = %d)\n", p->interface_id);
	break;
    case IEV_Retry:
	printf("[WAIT_EVENT] Retry(id = %d) %s\n", p->interface_id, phone_number);
	break;
    case IEV_Busy_Retry:
	printf("[WAIT_EVENT] Busy_Retry(id = %d)\n", p->interface_id);
	break;
    case IEV_Timeout:
	printf("[WAIT_EVENT] Timeout(id = %d)\n", p->interface_id);
	break;
    case IEV_Authfailed:
	printf("[WAIT_EVENT] Authfailed(id = %d)\n", p->interface_id);
	break;
    case IEV_Busy:
	printf("[WAIT_EVENT] Busy(id = %d)\n", p->interface_id);
	break;
    case IEV_NoCarrier:
	printf("[WAIT_EVENT] NoCarrier(id = %d)\n", p->interface_id);
	break;
    }
#endif /* DEBUG */

    return (data);
}

/* Sifrpc �X���b�h */
void get_event(void)
{
    static u_int buf[SSIZE / 4];
    sceSifQueueData qd;
    sceSifServeData sd;

    sceSifInitRpc(0);
    sceSifSetRpcQueue(&qd, GetThreadId());
    sceSifRegisterRpc(&sd, GET_EVENT, get_event_server, (void *)&buf[0], 0, 0, &qd);
    sceSifRpcLoop(&qd);
}

/* get_event ���̏����� */
void get_event_init(void)
{
    struct EventFlagParam event_param;
    int i;

    /* �C�x���g�t���O�̍쐬 */
    event_param.attr        = EA_SINGLE;
    event_param.initPattern = 0;
    event_param.option      = 0;

    event_id = CreateEventFlag(&event_param);

    /* �C�x���g�n���h����o�^ */
    eh.func = event_handler;
    sceInetCtlRegisterEventHandler(&eh);

    /* �C�x���g�n���h���Ŏg�p����ϐ��̏����� */
    interface_id       = -1;
    handler_result_num = -1;
    for(i = 0; i < HANDLER_RESULT_NUM_MAX; i++){
	handler_result_type[i] = -1;
	handler_result_id[i]   = -1;
    }
}
