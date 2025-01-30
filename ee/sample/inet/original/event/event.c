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

#define DEBUG /* デバッグプリントを表示しない場合はコメントアウトして下さい */

#define MODEM /* Ethernet 接続機器を使う場合はコメントアウトして下さい */

#ifdef MODEM
#define DB_PATH          SCETOP "conf/net/net.db"            /* 設定管理ファイルパス */
#define COMBINATION_NAME "Combination2"                                 /* 組み合わせ名 */
#else
#define DB_PATH          SCETOP "conf/net/net.db"            /* 設定管理ファイルパス */
#define COMBINATION_NAME "Combination3"                                 /* 組み合わせ名 */
#endif
#define IOPRP            SCETOP "iop/modules/" IOP_IMAGE_file /* リプレイスモジュールパス */

#define WORKAREA_SIZE (0x1000) /* IOP におけるワークエリアのサイズ */

static sceNetcnfifArg_t if_arg; /* Netcnf Interface に必要なデータ領域 */

/* 接続処理 */
void connect(int *connect_phase)
{
    static sceNetcnfifData_t data __attribute__((aligned(64)));
    static int addr, id;
    
    switch(*connect_phase){
    case 1: /* 設定の内容を取得する */
	sceNetcnfifDataInit(&data);
	FlushCache(0);
	sceNetcnfifLoadEntry(DB_PATH, sceNetcnfifArg_type_net, COMBINATION_NAME, &data);
	(*connect_phase)++;
	break;
    case 2: /* sceNetcnfifData の転送先アドレスを取得 */
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
    case 3: /* sceNetcnfifData を IOP に転送 */
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
    case 4: /* IOP 側の sceNetCnfEnv 領域に sceNetcnfifData の内容を設定 */
	if(sceNetcnfifDmaCheck(id)) break;
	sceNetcnfifSetEnv(sceNetcnfifArg_type_net);
	(*connect_phase)++;
	break;
    case 5: /* 接続 */
	if(sceNetcnfifCheck()) break;
	sceNetcnfifGetResult(&if_arg);
	if(if_arg.data < 0){
	    scePrintf("[%s] sceNetcnfifSetEnv (%d)\n", __FUNCTION__, if_arg.data);
	    *connect_phase = 0;
	    break;
	}
	addr = if_arg.addr;
	rpc_up(addr); /* UP 可能なインタフェースを UP */
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
    while(!sceSifRebootIop(IOPRP)); /* IOP リブート・モジュール置き換え */
    while(!sceSifSyncIop());        /* 終了を待つ */
    sceSifInitRpc(0);
    sceSifLoadFileReset();
    sceFsReset();

    /* 必要なモジュールをロード */
    load_module();

    /* Sifrpc サーバとバインド */
    rpc_setup();

    /* Sifrpc サーバとバインド(netcnfif) */
    sceNetcnfifInit();
    sceNetcnfifSetup();

    /* IOP 側のワークエリアを確保 */
    sceNetcnfifAllocWorkarea(WORKAREA_SIZE);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] sceNetcnfifAllocWorkarea (%d)\n", __FUNCTION__, if_arg.data);
	return (0);
    }

    /* f_no_decode を設定(タイトルに使用する場合は必ず
       sceNetcnfifArg_f_no_decode_off に設定すること) */
    sceNetcnfifSetFNoDecode(sceNetcnfifArg_f_no_decode_on);

    /* GS を初期化 */
    gs_init();

    /* フォントシステム初期化 */
    sceDevConsInit();
    console = sceDevConsOpen(OFFX + (16 << 4), OFFY + (24 << 4), 75, 30);
    sceDevConsClear(console);

    /* disp 環境を初期化 */
    init_disp();

    /* パッドの初期化 */
    pad_setup();

    frame = 0;
    sceGsSyncPath(0,0);

    while(1){
	sceGsSwapDBuff(&db, frame);
	++frame;

	/* パッド情報を取得 */
	pad_info = pad_read();
	if(pad_info & SCE_PADstart){
	    if(connect_phase){
		if(connect_phase >= 7){
		    while(rpc_check1());
		    rpc_down(); /* DOWN 可能なインタフェースを DOWN */
		}
		connect_phase = 0;
	    }else{
		if(!connect_phase) connect_phase = 1;
	    }
	}

	/* 接続処理 */
	connect(&connect_phase);

	/* Inet Event Check */
	if(rpc_check0() == 0 && rpc_get_event() == IEV_None){
	    rpc_wait_event(-1); /* interface_id == -1 で全てのインタフェースを監視 */
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
		rpc_init(); /* rpc_get_event() で得られる event_type を IEV_None に戻す */
	    }
	}

	/* 結果を描画 */
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
