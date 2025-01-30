/*                 -*- mode: c-mode; tab-width: 4; indent-tabs-mode: nil; -*-
 SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 * I/O Processor Library
 *
 * Copyright (C) 2002-2003 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * iopgcovout - iopgcovout.c
 *   resident module for iop gcov data output runtime routine
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2002-12-26      isii        create
 */

#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include "iopgcovout.h"

#define rewind(fd)  lseek(fd, 0, SEEK_SET)

int verbose = 1; /* 0 or 1 or 2 */
int block = 1;

ModuleInfo Module = {GCOV_OUT_Module, 0x0101};

static SceGcovLink sGcovLink_root = {
    GCOVMAGIC, NULL,   &sGcovLink_root, &sGcovLink_root,   NULL, NULL, NULL
};

void AddGcovLink(SceGcovLink *newlink, int oldei);
void RemoveGcovlink(SceGcovLink *gcovlink, int oldei);
SceGcovLink *SearchGcovLink(int modid);
void LinkModules();
void __bb_exit_func(SceGcovLink *gcovlink, int oldei);

/* この関数は必ずテキストセグメントの先頭にいなければならない。 */
void AddGcovLink(SceGcovLink *newlink, int oldei)
{
    unsigned long oldgp;
    __asm__ volatile( "  move %0, $gp; la  $gp, _gp" : "=r" (oldgp)
                      : /* no input */ : /* clobber */ "memory" );

    if ( block == 0 
        && newlink->modinfo != NULL
        && newlink->next == NULL && newlink->prev == NULL
        && newlink->outputfunc == NULL
        && newlink->terminatefunc == NULL
        && strncmp(newlink->magic, sGcovLink_root.magic, 8) == 0 ) {
        newlink->prev = sGcovLink_root.prev;
        newlink->next = &sGcovLink_root;
        newlink->prev->next = newlink;
        sGcovLink_root.prev = newlink;
        newlink->outputfunc = __bb_exit_func;
        newlink->terminatefunc = RemoveGcovlink;
        CpuResumeIntr(oldei);
        printf("GCOV start logging module: %s 0x%x\n",
               newlink->modinfo->name, newlink);
    } else
        CpuResumeIntr(oldei);

    __asm__ volatile( "  move $gp, %0"  : /* no output */
                      : "r" (oldgp) : /* clobber */ "memory" );
}

void RemoveGcovlink(SceGcovLink *gcovlink, int oldei)
{
    if ( gcovlink->next != NULL && gcovlink->prev != NULL ) {
        gcovlink->next->prev = gcovlink->prev;
        gcovlink->prev->next = gcovlink->next;
        gcovlink->prev = gcovlink->next = NULL;
    }
    gcovlink->outputfunc = NULL;
    gcovlink->terminatefunc = NULL;
    __bb_exit_func (gcovlink, oldei);
    printf("GCOV stop logging module: %s 0x%x\n",
           gcovlink->modinfo->name, gcovlink);
}

/* この include はここから前に移動してはいけない。*/
#include "gcov_io.h"

void __bb_exit_func (SceGcovLink *gcovlink, int oldei)
{
    struct bb *ptr;
    char *filename;
    int maxfnamelen;
    int i, err, fd;
    unsigned long oldgp;
    struct bb *bb_head;

    __asm__ volatile( "  move %0, $gp; la  $gp, _gp" : "=r" (oldgp)
                      : /* no input */ : /* clobber */ "memory" );

    bb_head = gcovlink->bb_head;
    CpuResumeIntr(oldei);
    
    if ( block )        goto bb_exit_func_end;
    if ( bb_head == 0 ) goto bb_exit_func_end;

    maxfnamelen=0;
    for (ptr = bb_head; ptr!=(struct bb *)0; ptr=ptr->next){
        if (strlen(bb_head->filename) > maxfnamelen)
            maxfnamelen = strlen(bb_head->filename);
    }
    maxfnamelen += 7; /* strlen("host1:")+1 */
    filename = (char *)__builtin_alloca(maxfnamelen);
    
    i = strlen (bb_head->filename) - 3;

    if (strcmp (bb_head->filename+i, ".da") != 0) {
        /* Must be -fprofile-arcs not -a.
           Dump data in a form that gcov expects.  */
        goto bb_exit_func_end;
    }
    
    /* output counters */
    for (ptr = bb_head; ptr != (struct bb *) 0; ptr = ptr->next) {
        strcpy(filename, "host1:"); strcat(filename,ptr->filename);
        if ( verbose ) printf("\nOpen %s\n", filename);
        fd = open(filename, O_RDWR|O_CREAT);
        if ( fd < 0 ) {
            printf("file '%s' can't open\n", filename);
            continue;
        }
        err =  read(fd, &i, sizeof(i));
        if ( err < 0 ) {
            printf("file '%s' read error\n", filename);
            close(fd);
            continue;
        }
        if ( err == sizeof(i) ) {
            long n_counts = 0;
            
            rewind(fd);
            if (__read_long (&n_counts, fd, 8) != 0) {
                printf ("arc profiling: Can't read output file %s.\n",
                        ptr->filename);
                close(fd);
                continue;
            }
            if (n_counts == ptr->ncounts) {
                int i;
                if ( verbose ) printf("merge log\n");
                for (i = 0; i < n_counts; i++) {
                    long v = 0;
                    if (__read_long (&v, fd, 8) != 0) {
                        printf ("arc profiling: Can't read output file %s.\n",
                                ptr->filename);
                        close(fd);
                        break;
                    }
                    if ( verbose > 1 ) printf("  %d: %d += %d\n",i, ptr->counts[i] , v);
                    CpuSuspendIntr(&oldei);
                    ptr->counts[i] += v;
                    CpuResumeIntr(oldei);
                }
            } else {
                if ( verbose ) printf("new log\n");
            }
        }
        rewind (fd);
        if (__write_long (ptr->ncounts, fd, 8) != 0) {
            printf ("arc profiling: Error writing output file %s.\n",
                    ptr->filename);
        } else {
            int j;
            long *count_ptr = ptr->counts;
        
            int ret = 0;
            for (j = ptr->ncounts; j > 0; j--) {
                long outputdata;
                CpuSuspendIntr(&oldei);
                outputdata = *count_ptr;
                /* clean up counters */
                *count_ptr = 0;
                CpuResumeIntr(oldei);
                if (verbose>1) printf("%d: %d\n",ptr->ncounts-j, outputdata);
                if (__write_long (outputdata, fd, 8) != 0) {
                    ret=1;
                    break;
                }
                count_ptr++;
            }
            if (ret)
                printf ( "arc profiling: Error writing output file %s.\n",
                         ptr->filename);
        }
        if ( verbose ) printf("Close %s\n", filename);
        close(fd);
    }
 bb_exit_func_end:
    __asm__ volatile( "  move $gp, %0"  : /* no output */
                      : "r" (oldgp) : /* clobber */ "memory" );
}

/* modid で指定したモジュール内に SceGcovLink 構造体があるか探す */
SceGcovLink *SearchGcovLink(int modid)
{
    SceGcovLink *result;
    int *ip, *ep, magic;
    ModuleStatus modstat;

    ReferModuleStatus(modid, &modstat);
    magic = *(int*)(&sGcovLink_root);
    ip = (int *)(modstat.text_addr+modstat.text_size);
    ep = (int *)(modstat.text_addr+modstat.text_size+modstat.data_size);
    for (  ;  ip < ep ; ip++ ) {
        if ( *ip == magic ) {
            result = (SceGcovLink *)ip;
            if ( result->modinfo != NULL
                && strncmp(result->magic, sGcovLink_root.magic, 8) == 0 )
                return result;
        }
    }
    return NULL;
}
     
/* メモリ上の全モジュールを検査し、SceGcovLink 構造体をもつモジュールを
 * 管理用のリスト sGcovLink_root につなぐ。 */
void LinkModules()
{
    int  *idlist, tmplist[2], idcount, oldei, i;
    SceGcovLink *gpl;

    CpuSuspendIntr(&oldei);
    GetModuleIdList( tmplist, 1, &idcount);
    idlist = AllocSysMemory(SMEM_High, sizeof(int)*(idcount+1), NULL);
    if ( idlist != NULL ) {
        idcount = GetModuleIdList( idlist, idcount, NULL);
        for ( i = 0; i < idcount ; i++ ) {
            gpl = SearchGcovLink(idlist[i]);
            if ( gpl != NULL ) {
                AddGcovLink(gpl, oldei);
                CpuSuspendIntr(&oldei);
            }
        }
        FreeSysMemory(idlist);
    }
    CpuResumeIntr(oldei);
}

int module_start(int argc, char *argv[])
{
    int  idlist[2], idcount;

    if ( argc != 0 ) {
        GetModuleIdListByName(Module.name, idlist, 2, &idcount);
        if ( idcount != 1 ) {
            printf("\"%s\" alrady exist. \n", Module.name);
            return NO_RESIDENT_END;
        }
    }
    block = 0;
    printf("Now \"%s\" resident\n", Module.name);
    LinkModules();
    return REMOVABLE_RESIDENT_END;
}

int module_stop(int argc, char *argv[])
{
    int oldei;
    block = 1;

    CpuSuspendIntr(&oldei);
    while ( sGcovLink_root.prev != &sGcovLink_root
           || sGcovLink_root.next != &sGcovLink_root ) {
        RemoveGcovlink(sGcovLink_root.prev, oldei);
        CpuSuspendIntr(&oldei);
    }
    CpuResumeIntr(oldei);
    return NO_RESIDENT_END;
}

int start(int argc, char *argv[])
{
    if ( argc >= 0 ) return module_start(argc,argv);
    else             return module_stop(-argc,argv);
}
