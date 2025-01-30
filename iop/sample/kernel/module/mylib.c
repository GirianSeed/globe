/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/*
 *                      Emotion Engine Library
 *                          Version 0.1.0
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         mylib.c
 *                         develop library
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.1.0
 */

#include <stdio.h>
#include <kernel.h>

/* �������ɏ풓���郂�W���[����, �ȉ��̂悤�Ƀ��W���[������
 * ���W���[���o�[�W������t���Ă����ƕ֗��ł��B
 */
ModuleInfo Module = {"My_Memory_Resident_Lib", 0x0101 };

/* ================================================================
 * 	�풓���C�u�����Ƃ��Ă̏������G���g��
 * ================================================================ */

int MyLibInit()
{
    /* mylib.tbl����, ���[�e�B���e�B loplibgen �ɂ���ăG���g���e�[�u����
     * ��������܂��B�G���g���e�[�u���̃��x�����ɂ�, '���C�u������_entry' ��
     *  �����܂��B
     */
    extern libhead mylib_entry; /* ���C�u������_entry ���Q�� */

    if( RegisterLibraryEntries(&mylib_entry) != 0 ) {
	/* ���ɓ����̏풓���C�u����������̂œo�^�Ɏ��s */
	return NO_RESIDENT_END; /* �I�����ă���������ދ� */
    }
    return RESIDENT_END; /* �I�����ď풓���� */
}

/* ================================================================
 * 	�풓���C�u�����̊e�G���g���̒�`
 * ================================================================ */

void libentry1(int i)
{
    printf("mylib: libentry1(%d)\n", i);
}

void internal_libentry2(int i)
{
    printf("mylib: libentry2(%d)\n", i);
}

