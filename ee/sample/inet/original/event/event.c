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
 *                         event.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.02.18      tetsu       Change directory
 */

#include <stdio.h>
#include <string.h>
#include <eekernel.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <devfont.h>
#include <libgraph.h>
#include <libpad.h>
#include <netcnfif.h>

#include "common.h"
#include "load_module.h"
#include "gs_init.h"
#include "rpc.h"
#include "disp.h"
#include "pad.h"

#define DEBUG /* �f�o�b�O�v�����g��\�����Ȃ��ꍇ�̓R�����g�A�E�g���ĉ����� */

#define MODEM /* Ethernet �ڑ��@����g���ꍇ�̓R�����g�A�E�g���ĉ����� */

#ifdef MODEM
#define DB_PATH          SCETOP "conf/net/net.db"            /* �ݒ�Ǘ��t�@�C���p�X */
#define COMBINATION_NAME "Combination2"                                 /* �g�ݍ��킹�� */
#else
#define DB_PATH          SCETOP "conf/net/net.db"            /* �ݒ�Ǘ��t�@�C���p�X */
#define COMBINATION_NAME "Combination3"                                 /* �g�ݍ��킹�� */
#endif
#define IOPRP            SCETOP "iop/modules/" IOP_IMAGE_file /* ���v���C�X���W���[���p�X */

#define WORKAREA_SIZE (0x1000) /* IOP �ɂ����郏�[�N�G���A�̃T�C�Y */

static sceNetcnfifArg_t if_arg; /* Netcnf Interface �ɕK�v�ȃf�[�^�̈� */

/* �ڑ����� */
void connect(int *connect_phase)
{
    static sceNetcnfifData_t data __attribute__((aligned(64)));
    static int addr, id;
    
    switch(*connect_phase){
    case 1: /* �ݒ�̓��e���擾���� */
	sceNetcnfifDataInit(&data);
	FlushCache(0);
	sceNetcnfifLoadEntry(DB_PATH, sceNetcnfifArg_type_net, COMBINATION_NAME, &data);
	(*connect_phase)++;
	break;
    case 2: /* sceNetcnfifData �̓]����A�h���X���擾 */
	if(sceNetcnfifCheck()) break;
	sceNetcnfifGetResult(&if_arg);
	if(if_arg.data < 0){
	    scePrintf("[%s] sceNetcnfifLoadEntry (%d)\n", __FUNCTION__, if_arg.data);
	    *connect_phase = 0;
	    break;
	}
	sceNetcnfifGetAddr();
	(*connect_phase)++;
	break;
    case 3: /* sceNetcnfifData �� IOP �ɓ]�� */
	if(sceNetcnfifCheck()) break;
	sceNetcnfifGetResult(&if_arg);
	if(if_arg.data < 0){
	    scePrintf("[%s] sceNetcnfifGetAddr (%d)\n", __FUNCTION__, if_arg.data);
	    *connect_phase = 0;
	    break;
	}
	addr = if_arg.addr;
	id = sceNetcnfifSendIOP((unsigned int)&data, (unsigned int)addr, (unsigned int)sizeof(sceNetcnfifData_t));
	(*connect_phase)++;
	break;
    case 4: /* IOP ���� sceNetCnfEnv �̈�� sceNetcnfifData �̓��e��ݒ� */
	if(sceNetcnfifDmaCheck(id)) break;
	sceNetcnfifSetEnv(sceNetcnfifArg_type_net);
	(*connect_phase)++;
	break;
    case 5: /* �ڑ� */
	if(sceNetcnfifCheck()) break;
	sceNetcnfifGetResult(&if_arg);
	if(if_arg.data < 0){
	    scePrintf("[%s] sceNetcnfifSetEnv (%d)\n", __FUNCTION__, if_arg.data);
	    *connect_phase = 0;
	    break;
	}
	addr = if_arg.addr;
	rpc_up(addr); /* UP �\�ȃC���^�t�F�[�X�� UP */
	(*connect_phase)++;
	break;
    }
}

int main(void)
{
    static char str_tmp[256];
    static char ip_addr[256];
    static char line_speed[256];
    static char phone_number[256];
    int console, frame;
    int event_flag = 0, event_type = IEV_None, event_id = -1, event_count = 0, up_event_id = -1;
    u_short pad_info;
    static int connect_phase = 0;

    sceSifInitRpc(0);
    while(!sceSifRebootIop(IOPRP)); /* IOP ���u�[�g�E���W���[���u������ */
    while(!sceSifSyncIop());        /* �I����҂� */
    sceSifInitRpc(0);
    sceSifLoadFileReset();
    sceFsReset();

    /* �K�v�ȃ��W���[�������[�h */
    load_module();

    /* Sifrpc �T�[�o�ƃo�C���h */
    rpc_setup();

    /* Sifrpc �T�[�o�ƃo�C���h(netcnfif) */
    sceNetcnfifInit();
    sceNetcnfifSetup();

    /* IOP ���̃��[�N�G���A���m�� */
    sceNetcnfifAllocWorkarea(WORKAREA_SIZE);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] sceNetcnfifAllocWorkarea (%d)\n", __FUNCTION__, if_arg.data);
	return (0);
    }

    /* f_no_decode ��ݒ�(�^�C�g���Ɏg�p����ꍇ�͕K��
       sceNetcnfifArg_f_no_decode_off �ɐݒ肷�邱��) */
    sceNetcnfifSetFNoDecode(sceNetcnfifArg_f_no_decode_on);

    /* GS �������� */
    gs_init();

    /* �t�H���g�V�X�e�������� */
    sceDevConsInit();
    console = sceDevConsOpen(OFFX + (16 << 4), OFFY + (24 << 4), 75, 30);
    sceDevConsClear(console);

    /* disp ���������� */
    init_disp();

    /* �p�b�h�̏����� */
    pad_setup();

    frame = 0;
    sceGsSyncPath(0,0);

    while(1){
	sceGsSwapDBuff(&db, frame);
	++frame;

	/* �p�b�h�����擾 */
	pad_info = pad_read();
	if(pad_info & SCE_PADstart){
	    if(connect_phase){
		if(connect_phase >= 7){
		    while(rpc_check1());
		    rpc_down(); /* DOWN �\�ȃC���^�t�F�[�X�� DOWN */
		}
		connect_phase = 0;
	    }else{
		if(!connect_phase) connect_phase = 1;
	    }
	}

	/* �ڑ����� */
	connect(&connect_phase);

	/* Inet Event Check */
	if(rpc_check0() == 0 && rpc_get_event() == IEV_None){
	    rpc_wait_event(-1); /* interface_id == -1 �őS�ẴC���^�t�F�[�X���Ď� */
	}else{
	    if(IEV_None != (event_type = rpc_get_event())){
		event_id   = rpc_get_id();
		switch(event_type){
		case IEV_Up:
		    connect_phase++;
		    up_event_id  = event_id;
		    strcpy(phone_number, rpc_get_phone_number());
		    break;
		case IEV_Retry:
		    strcpy(phone_number, rpc_get_phone_number());
		    break;
		case IEV_Start:
		    strcpy(ip_addr, rpc_get_ip_addr());
		    strcpy(line_speed, rpc_get_line_speed());
		    break;
		case IEV_Detach:
		    if(event_id != up_event_id) break;
		case IEV_Down:
		case IEV_Stop:
		case IEV_Error:
		case IEV_Timeout:
		case IEV_Authfailed:
		case IEV_Busy:
		case IEV_NoCarrier:
		    connect_phase = 0;
		    break;
		}
		event_flag = 1;
		rpc_init(); /* rpc_get_event() �œ����� event_type �� IEV_None �ɖ߂� */
	    }
	}

	/* ���ʂ�`�� */
	sceDevConsClear(console);
	sceDevConsLocate(console, 0, 0);
	sceDevConsPrintf(console, "Inet Event Sample Program\n");
	sceDevConsPrintf(console, "=========================\n");
	sceDevConsPrintf(console, "\n");
	if(connect_phase){
	    sceDevConsPrintf(console, " Please push START to disconnect.\n");
	}else{
	    sceDevConsPrintf(console, " Please push START to connect.\n");
	}
	sceDevConsPrintf(console, "\n");
	if(event_flag){
	    switch(event_type){
	    case IEV_Attach:
		sprintf(str_tmp, " %03d: Attach(id = %d)\n", ++event_count, event_id);
		break;
	    case IEV_Detach:
		sprintf(str_tmp, " %03d: Detach(id = %d)\n", ++event_count, event_id);
		break;
	    case IEV_Start:
		sprintf(str_tmp, " %03d: Start(id = %d) %s %s\n", ++event_count, event_id, ip_addr, line_speed);
		break;
	    case IEV_Stop:
		sprintf(str_tmp, " %03d: Stop(id = %d)\n", ++event_count, event_id);
		break;
	    case IEV_Error:
		sprintf(str_tmp, " %03d: Error(id = %d)\n", ++event_count, event_id);
		break;
	    case IEV_Conf:
		sprintf(str_tmp, " %03d: Conf(id = %d)\n", ++event_count, event_id);
		break;
	    case IEV_NoConf:
		sprintf(str_tmp, " %03d: NoConf(id = %d)\n", ++event_count, event_id);
		break;
	    case IEV_Up:
		sprintf(str_tmp, " %03d: Up(id = %d) %s\n", ++event_count, event_id, phone_number);
		break;
	    case IEV_Down:
		sprintf(str_tmp, " %03d: Down(id = %d)\n", ++event_count, event_id);
		break;
	    case IEV_Retry:
		sprintf(str_tmp, " %03d: Retry(id = %d) %s\n", ++event_count, event_id, phone_number);
		break;
	    case IEV_Busy_Retry:
		sprintf(str_tmp, " %03d: Busy_Retry(id = %d)\n", ++event_count, event_id);
		break;
	    case IEV_Timeout:
		sprintf(str_tmp, " %03d: Timeout(id = %d)\n", ++event_count, event_id);
		break;
	    case IEV_Authfailed:
		sprintf(str_tmp, " %03d: Authfailed(id = %d)\n", ++event_count, event_id);
		break;
	    case IEV_Busy:
		sprintf(str_tmp, " %03d: Busy(id = %d)\n", ++event_count, event_id);
		break;
	    case IEV_NoCarrier:
		sprintf(str_tmp, " %03d: NoCarrier(id = %d)\n", ++event_count, event_id);
		break;
	    }
#ifdef DEBUG
	    scePrintf("%s", str_tmp);
#endif /* DEBUG */
	    add_disp(str_tmp);
	    event_flag = 0;
	}
	print_disp(console);
	FlushCache(0);
	sceDevConsDraw(console);

	if(frame & 0x01){
	    sceGsSetHalfOffset(&db.draw1, 2048, 2048, sceGsSyncV(0) ^ 0x01);
	}else{
	    sceGsSetHalfOffset(&db.draw0, 2048, 2048, sceGsSyncV(0) ^ 0x01);
	}
	sceGsSyncPath(0,0);
    }

    return (0);
}
