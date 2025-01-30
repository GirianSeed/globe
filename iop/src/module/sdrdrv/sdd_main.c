/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                   I/O Proseccor sample Library
 *                          Version 0.50
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                       rs2drv.irx - rsd_main.c
 *                           entry function
 *
 *   Version   Date            Design    Log
 *  --------------------------------------------------------------------
 *   0.30      Jun.17.1999     morita    first checked in
 *   0.50      Aug.18.1999     morita    SpuStSetCore etc. added.
 *                                       rewrite for new siflib.
 *             May.14.2000     kaol      Thread priority changed.
 *   2.0.0     Jul.24.2000     kaol      Thread priority can be changed
 *                                        dynamically from outside.
 */

#include <kernel.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>		/* atoi() */
#include <ctype.h>		/* isdigit() */
#include <sif.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <libsd.h>
#include <sdrcmd.h>
#include "sdr_i.h"

ModuleInfo Module = {"sdr_driver", 0x0401 };

extern int sce_sdr_loop(void);	/* sdd_com.c */

int initial_priority_main;	/* ���C���X���b�h�̃v���C�I���e�B�����l */
int initial_priority_cb;	/* �R�[���o�b�N�X���b�h�̃v���C�I���e�B�����l */

volatile int thid_main = 0;	/* ���C���X���b�h�̃X���b�h ID */
volatile int thid_cb   = 0;	/* �R�[���o�b�N�X���b�h�̃X���b�h ID (sdd_cb.c) */

sceSifQueueData rpc_qd;		/* SIF RPC request queue data */
sceSifServeData rpc_sd;		/* SIF RPC server data */

static int
module_start (int argc, char *argv [])
{
    /* sdrdrv.tbl����, ���[�e�B���e�B ioplibgen �ɂ���ăG���g���e�[�u����
     * ��������܂��B�G���g���e�[�u���̃��x�����ɂ�, '���C�u������_entry' ��
     * �����܂��B
     */
    extern libhead sdrdrv_entry; /* ���C�u������_entry ���Q�� */
    int code, oldei;		/* for Cpu{Suspend,Resume}Intr() */
    struct ThreadParam param;
    int i, pval;
    char *p;

    /*
     * ���W���[���̏풓�󋵂��`�F�b�N
     */
    CpuSuspendIntr (&oldei);
    code = RegisterLibraryEntries (&sdrdrv_entry);
    CpuResumeIntr (oldei);
    if (code != KE_OK) {
	/* ���ɓ����̏풓���C�u����������̂œo�^�Ɏ��s */
	return NO_RESIDENT_END; /* �I�����ă���������ދ� */
    }

    Kprintf ("SDR driver version 4.0.1 (C) SCEI\n");

    /*
     * �����I�v�V����: �e�X���b�h�̃v���C�I���e�B�����l
     */
    initial_priority_main = PRIORITY_MODULE_SDRDRV;
    initial_priority_cb   = PRIORITY_MODULE_SDRDRV;
    thid_main = 0;		/* ���C���X���b�h���쐬 */
    thid_cb   = 0;		/* �R�[���o�b�N�X���b�h���쐬 */

    /*
     * �����I�v�V�����̉��
     */
    for (i = 1; i < argc; i ++) {
	if (! strncmp ("thpri=", argv [i], 6)) {
	    p = argv [i] + 6; /* strlen("thpri=") == 6 */
	    if (isdigit (*p)) {
		/* ���C���X���b�h�̃v���C�I���e�B */
		pval = atoi (p);
		if (pval < USER_HIGHEST_PRIORITY ||
		    pval > USER_LOWEST_PRIORITY) {
		    Kprintf (" SDR driver error: invalid priority %d\n", pval);
		    pval = PRIORITY_MODULE_SDRDRV;
		}
		initial_priority_main = pval;
	    }
	    while (isdigit (*p)) p ++;
	    if (*p == ',' && isdigit (*(p + 1))) {
		/* �R�[���o�b�N�X���b�h�̃v���C�I���e�B */
		p ++;
		pval = atoi (p);
		if (pval < USER_HIGHEST_PRIORITY ||
		    pval > USER_LOWEST_PRIORITY) {
		    Kprintf (" SDR driver error: invalid priority %d\n", pval);
		    pval = PRIORITY_MODULE_SDRDRV;
		}
		initial_priority_cb = pval;
	    }
	    /* �c��̕�����͖�������� */

	    /* �w�肳�ꂽ�v���C�I���e�B�̃`�F�b�N */
	    if (initial_priority_main > initial_priority_cb) {
		/* �R�[���o�b�N�X���b�h�̃v���C�I���e�B��
		 * ���C���X���b�h�̃v���C�I���e�B���A
		 * �������Ⴍ (�l�Ƃ��Ă͑傫��) �Ȃ���΂����Ȃ� */
		Kprintf (" SDR driver ERROR:\n");
		Kprintf ("   callback th. priority is higher than main th. priority.\n");
		initial_priority_cb = initial_priority_main;
	    }
	    Kprintf (" SDR driver: thread priority: main=%d, callback=%d\n",
		     initial_priority_main, initial_priority_cb);
	}
	/* else { �s���ȃI�v�V�����͒P�ɖ������� } */
    }
	
    /* sdr thread start */
    param.attr         = TH_C;
    param.entry        = sce_sdr_loop;
    param.initPriority = initial_priority_main;
    param.stackSize    = 0x800;
    param.option       = 0;
    thid_main = CreateThread (&param);
    if (thid_main > 0) {
	StartThread (thid_main, 0);	/* ���C���X���b�h���s */
	Kprintf (" Exit rsd_main \n");
	return REMOVABLE_RESIDENT_END;	/* �������ɏ풓 (�A�����[�h�\) */
    } else {
	return NO_RESIDENT_END;		/* �I�����ă���������ދ� */
    }
}

static int
module_stop (int argc, char *argv [])
{
    extern libhead sdrdrv_entry; /* ���C�u������_entry ���Q�� */
    int code, oldei;		/* for Cpu{Suspend,Resume}Intr() */

    CpuSuspendIntr (&oldei);
    code = ReleaseLibraryEntries (&sdrdrv_entry);
    CpuResumeIntr (oldei);
    if (code != KE_OK) {
	return REMOVABLE_RESIDENT_END; /* �A�����[�h���s (�풓) */
    }

    /* Remove interrupt handlers */
#ifdef SCE_OBSOLETE
    sceSdSetTransCallback (0, (SD_TRANS_CBProc)NULL);
    sceSdSetTransCallback (1, (SD_TRANS_CBProc)NULL);
    sceSdSetIRQCallback ((SD_IRQ_CBProc)NULL);
#endif
    sceSdSetTransIntrHandler (0, (sceSdTransIntrHandler)NULL, (void *)NULL);
    sceSdSetTransIntrHandler (1, (sceSdTransIntrHandler)NULL, (void *)NULL);
    sceSdSetSpu2IntrHandler  ((sceSdSpu2IntrHandler)NULL, (void *)NULL);

    /* Thread destroying */
    if (thid_cb > 0) {
	/* Destroy callback thread */
	TerminateThread (thid_cb);
	DeleteThread (thid_cb);
	thid_cb = 0;
    }
    if (thid_main > 0) {
	/* Stop and remove SIF RPC server service */
	sceSifRemoveRpc (&rpc_sd, &rpc_qd);
	sceSifRemoveRpcQueue (&rpc_qd);

	/* Destroy RPC communication thread */
	TerminateThread (thid_main);
	DeleteThread (thid_main);
	thid_main = 0;
    }

    Kprintf (" sdrdrv: unloaded! \n");
    return NO_RESIDENT_END;		/* �I�����ă���������ދ� */
}

int
start (int argc, char *argv [])
{
    if (argc >= 0) return (module_start (argc, argv));
    else           return (module_stop (-argc, argv));
}

int
sceSdrChangeThreadPriority (int priority_main, /* main thread */
			    int priority_cb)   /* callback thread */
{
    struct ThreadInfo mythinfo;
    int ret;
    
    /*
     * �����ŗ^����ꂽ�v���C�I���e�B�l�̑Ó����𒲂ׂ�
     */
    if (priority_main < USER_HIGHEST_PRIORITY ||
	priority_main > USER_LOWEST_PRIORITY) {
	return KE_ILLEGAL_PRIORITY;
    }
    if (priority_cb < USER_HIGHEST_PRIORITY ||
	priority_cb > USER_LOWEST_PRIORITY) {
	return KE_ILLEGAL_PRIORITY;
    }
    if (priority_main > priority_cb) {
	/* �R�[���o�b�N�X���b�h�̃v���C�I���e�B�̓��C���X���b�h��
	 *  �v���C�I���e�B���A�������Ⴍ (�l�Ƃ��Ă͑傫��) �Ȃ����
	 *  �����Ȃ� */
	Kprintf (" SDR driver ERROR:\n");
	Kprintf ("   callback th. priority is higher than main th. priority.\n");
	priority_cb = priority_main;
    }
#if 0
    Kprintf (" SDR driver: change thread priority: main=%d, callback=%d\n",
	     priority_main, priority_cb);
#endif

    /*
     * ���̃X���b�h�Ɏ��s����D���Ȃ��悤�ɁA��U���X���b�h��
     * �v���C�I���e�B���L�^������ɍō��ɏグ�Ă����B
     */
    ReferThreadStatus (TH_SELF, &mythinfo);
    ChangeThreadPriority (TH_SELF, MODULE_INIT_PRIORITY);
        
    /*
     * ���C�u�����̊Ǘ����̃X���b�h�Q�̃v���C�I���e�B��K�؂ȏ��Ԃ�
     * ChangeThreadPriority() ����B
     */
    /* ���C���X���b�h */
    if (thid_main > 0) {
	if ((ret = ChangeThreadPriority (thid_main, priority_main)) < 0) {
	    return ret;
	}
    } /* else { �K���쐬����Ă��� } */
    /* �R�[���o�b�N�X���b�h */
    if (thid_cb > 0) {
	if ((ret = ChangeThreadPriority (thid_cb, priority_cb)) < 0) {
	    return ret;
	}
    } else {
	initial_priority_cb = priority_cb; /* �쐬���̏����l�� */
    }
    
    /* ���X���b�h�̃v���C�I���e�B�����ɖ߂� */
    ChangeThreadPriority (TH_SELF, mythinfo.currentPriority);
    
    return KE_OK;
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* DON'T ADD STUFF AFTER THIS */
