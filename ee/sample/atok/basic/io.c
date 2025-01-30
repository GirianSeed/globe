/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                      ATOK Library Sample
 *
 *                         Version 0.10
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.10
 */

#include <eekernel.h>
#include <sifdev.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include "app.h"

#define DAT_FILE_DEV    "host0:"

/*
 * DAT_FILE_DEV�ǻ��ꤵ�줿�ǥХ�������sceOpen()��Ȥä�
 * �ե����륵����ʬ��malloc�������֤ǥ꡼�ɤ���٤δؿ�
 * pfont, ccc������
 */

void *file_read(const char *path)
{
    char        file[256];
    size_t      size;
    void        *dptr;
    int         fd;

    sprintf(file, "%s%s", DAT_FILE_DEV, path);
    if ((fd = sceOpen(file, SCE_RDONLY)) < 0) {
        scePrintf("Cannot open file %s\n", file);
        exit(-1);
    }

    // size������
    size = sceLseek(fd, 0, SCE_SEEK_END);
    sceLseek(fd, 0, SCE_SEEK_SET);

    dptr = (void *) memalign(16, size);
    if (dptr == NULL) {
        scePrintf("Cannot allocate memory for file reading %s\n", file);
        exit(-1);
    }
    if (sceRead(fd, (u_int *)dptr, size) < 0) {
        scePrintf("Cannot read file %s\n", file);
        exit(-1);
    }
    FlushCache(0);
    sceClose(fd);
    return dptr;
}
