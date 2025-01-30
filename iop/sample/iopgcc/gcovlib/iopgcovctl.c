/*                 -*- mode: c-mode; tab-width: 4; indent-tabs-mode: nil; -*-
 SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 * I/O Processor Library
 *
 * Copyright (C) 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * iopgcovctl - iopgcovctl.c
 *   iop's modules gcov data control program
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2002-12-26      isii        create
 */

/*
 Usage:
     iopgcovctl <command>
       command:
          -list      - list logging modules
          -stop      - stop and unload Module:"iop_gcov_output"

     iopgcovctl [<module_name>]...           - gcov data output to files
*/


#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include "iopgcovout.h"

ModuleInfo Module = {"iop_gcov_control", 0x0101};

static SceGcovLink gcovlink = {
    GCOVMAGIC, &Module,    NULL, NULL,   NULL,   NULL, NULL
};

void Usage();
void DoList();
void DoOutput(int nummod, char *modlist[]);
void DoUnload();

int start(int argc, char *argv[])
{
    int  idlist[2], idcount, oldei;
    SceGcovFuncPtr  funcp;
    ModuleStatus modstat;

    CpuSuspendIntr(&oldei);
    GetModuleIdListByName(GCOV_OUT_Module, idlist, 2, &idcount);
    if ( idcount==1 && ReferModuleStatus(idlist[0], &modstat) == KE_OK ) {
		/* ダミーの SceGcovLink を登録して、gcov の管理リストを得る。*/
        funcp = (SceGcovFuncPtr)modstat.text_addr;
        funcp(&gcovlink, oldei);
        if ( argc == 1 )
            DoOutput(0, NULL);
        else if ( strcmp("-list",argv[1]) == 0 )
            DoList();
        else if ( strcmp("-stop",argv[1]) == 0 )
            DoUnload();
        else if ( argv[1][0] == '-' )
            Usage();
        else
            DoOutput(argc-1, argv+1);
        CpuSuspendIntr(&oldei);
        funcp = gcovlink.terminatefunc;
        if ( funcp != NULL ) funcp(&gcovlink, oldei);
        else                CpuResumeIntr(oldei);
    } else {
        CpuResumeIntr(oldei);
        printf("Module \"%s\" not found\n", GCOV_OUT_Module);
    }
    return NO_RESIDENT_END;
}

void Usage()
{
    printf(
           "\nUsage:\n"
           " iopgcovctl <command>\n"
           "   command:\n"
           "      -list      - list logging modules\n"
           "      -stop      - stop and unload Module:\"iop_gcov_output\"\n"
           "\n"
           " iopgcovctl [<module_name>]...\n"
           "\n"
           );
}

/* gcov のデータ収集中のモジュールのリスト表示する */
void DoList()
{
    int oldei;
    SceGcovLink *glp, *nglp;

    CpuSuspendIntr(&oldei);
    glp = gcovlink.next;
    if ( glp != NULL && glp != &gcovlink )
        do {
            nglp = glp->next;
            CpuResumeIntr(oldei);
            if ( glp->outputfunc != NULL )
                printf("  0x%x: %s\n", glp, glp->modinfo->name);
            CpuSuspendIntr(&oldei);
            glp = nglp;
        } while ( glp != NULL && glp != &gcovlink );
    CpuResumeIntr(oldei);    
}

/* gcov のデータをファイルに出力する
 *   nummod == 0 の場合は、全モジュールが対象。
 *   nummod  > 0 の場合は、modlist で指定したモジュール名のみ。
 */
void DoOutput(int nummod, char *modlist[])
{
    int oldei, idxmod;
    SceGcovLink *glp, *nglp;

    CpuSuspendIntr(&oldei);
    glp = gcovlink.next;
    if ( glp != NULL && glp != &gcovlink ) {
        do {
            nglp = glp->next;
            if ( nummod == 0 ) idxmod = 1;
            else {
                for ( idxmod = nummod; idxmod > 0 ; idxmod -- )
                    if ( strcmp(glp->modinfo->name, modlist[idxmod-1]) == 0 )
                        break;
            }       
            if ( idxmod > 0 && glp->outputfunc != NULL )
                glp->outputfunc(glp,oldei);
            else
                CpuResumeIntr(oldei);
        
            CpuSuspendIntr(&oldei);
            glp = nglp;
        } while ( glp != NULL && glp != &gcovlink );
    }
    CpuResumeIntr(oldei);    
}

/* iopgconvout.irx(モジュール名 "iop_gcov_output") を unload する。*/
void DoUnload()
{
    int oldei, modid;
    SceGcovLink *glp, *glproot;
    SceGcovFuncPtr  funcp;

    CpuSuspendIntr(&oldei);
    glproot = NULL;
    for ( glp=gcovlink.next; glp!=NULL && glp!=&gcovlink ; glp=glp->next ) {
        if ( glp->modinfo == NULL && glp->bb_head == NULL
            && glp->outputfunc == NULL && glp->terminatefunc  == NULL ) {
            glproot = glp;
            break;
        }
    }
    if ( glproot != NULL ) {
        modid = SearchModuleByAddress( glproot );
        funcp = gcovlink.terminatefunc;
        if ( funcp != NULL ) funcp(&gcovlink, oldei);
        else                CpuResumeIntr(oldei);
        if ( modid >= KE_OK
            && StopModule(modid, 0, NULL, NULL) >= KE_OK 
            &&  UnloadModule(modid) >= KE_OK )
            printf("Unload Module:%s  module id=%d\n",GCOV_OUT_Module, modid);
    } else {
        CpuResumeIntr(oldei);
    }
}
