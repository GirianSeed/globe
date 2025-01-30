/*                 -*- mode: c-mode; tab-width: 4; indent-tabs-mode: nil; -*-
 SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: resident.c,v 1.2 2003/03/25 11:45:47 tei Exp $
 */
/*
 * I/O Processor Library Sample Program
 *
 * Copyright (C) 2003 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * resident - resident.c
 *   coverage testing sample
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2003-02-03      isii        create
 */
#include <kernel.h>
#include <stdio.h>
#include <iopgcov.h>

#define BASE_priority  32

ModuleInfo Module = {"gcov_test_program_resident", 0x0101 };

int thread();

int start(int argc, char *argv[])
{
    int i;
    struct ThreadParam param;
    int thid;

    /* �v���O�����J�n���� sceIopgcovDoGlobalCtors() ���Ă�ł��������B*/
    sceIopgcovDoGlobalCtors();

    param.attr         = TH_C;
    param.entry        = thread;
    param.initPriority = BASE_priority;
    param.stackSize    = 0x800;
    param.option       = 0;
    thid = CreateThread(&param);
    if ( thid > 0 ) {
        StartThread(thid,0);
    }

    for ( i = 0; i < 100; i++ ) {
        if ( i % 10 == 0 )
            printf("bar\n");
        else
            printf("foo ");
    }
    printf(" resident end\n");

    /* �풓�I������ꍇ�́AsceIopgcovDoGlobalDtors() ���Ă΂Ȃ��ł��������B*/
    return RESIDENT_END;
}

int thread()
{
    int i, l;

    l = 0;
    while (1) {
        for ( i = 0; i < 100; i++ ) {
            if ( i % 10 == 0 )
                printf("var\n");
            else
                printf("hoo ");
        }
        printf("[%d]\n", ++l);
        /* �ȉ��̂悤�Ƀe�X�g�Ώۃv���O�������g�œr���o�߂̃J�o���b�W�f�[�^��
         * �o�͂����邱�Ƃ��\  */
        if ( (l % 10) == 9 )
            sceIopgcovOutputData();
        DelayThread(5*1000*1000);
    }
}
