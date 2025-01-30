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
 *                         event.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2001.12.28      tetsu       First version
 *       1.2            2002.02.05      tetsu       Add ModuleInfo
 */

#include <kernel.h>
#include <inet/inetctl.h>
#include <usbmload.h>

#include "get_event.h"
#include "up_down.h"

ModuleInfo Module = {"event", 0x0101};

int start(int argc, char *argv[])
{
    struct ThreadParam th_param;
    int th_id0, th_id1;

    /* USB ���f���EEthernet �@��h���C�o�̃I�[�g���[�h������ */
    sceUsbmlActivateCategory("Ether");
    sceUsbmlActivateCategory("Modem");
    sceUsbmlEnable();

    /* �C�x���g���擾����X���b�h���쐬 */
    th_param.attr         = TH_C;
    th_param.entry        = get_event;
    th_param.initPriority = USER_LOWEST_PRIORITY - 2;
    th_param.stackSize    = 0x800;
    th_param.option       = 0;

    th_id0 = CreateThread(&th_param);

    /* �C���^�t�F�[�X���A�b�v�E�_�E������X���b�h���쐬 */
    th_param.attr         = TH_C;
    th_param.entry        = up_down;
    th_param.initPriority = USER_LOWEST_PRIORITY - 1;
    th_param.stackSize    = 0x800;
    th_param.option       = 0;

    th_id1 = CreateThread(&th_param);

    /* get_event ���������� */
    get_event_init();

    /* �X���b�h���X�^�[�g */
    if(th_id0 > 0 && th_id1 > 0){
	StartThread(th_id0, 0);
	StartThread(th_id1, 0);
	return (RESIDENT_END);
    }else{
	return (NO_RESIDENT_END);
    }
}
