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

/* メモリに常駐するモジュールは, 以下のようにモジュール名と
 * モジュールバージョンを付けておくと便利です。
 */
ModuleInfo Module = {"My_Memory_Resident_Lib", 0x0101 };

/* ================================================================
 * 	常駐ライブラリとしての初期化エントリ
 * ================================================================ */

int MyLibInit()
{
    /* mylib.tblから, ユーティリティ loplibgen によってエントリテーブルが
     * 生成されます。エントリテーブルのラベル名には, 'ライブラリ名_entry' が
     *  つけられます。
     */
    extern libhead mylib_entry; /* ライブラリ名_entry を参照 */

    if( RegisterLibraryEntries(&mylib_entry) != 0 ) {
	/* 既に同名の常駐ライブラリがいるので登録に失敗 */
	return NO_RESIDENT_END; /* 終了してメモリから退去 */
    }
    return RESIDENT_END; /* 終了して常駐する */
}

/* ================================================================
 * 	常駐ライブラリの各エントリの定義
 * ================================================================ */

void libentry1(int i)
{
    printf("mylib: libentry1(%d)\n", i);
}

void internal_libentry2(int i)
{
    printf("mylib: libentry2(%d)\n", i);
}

