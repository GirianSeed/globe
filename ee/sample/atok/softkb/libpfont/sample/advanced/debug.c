/* SCE CONFIDENTIAL
   "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
*/
/*
 *     Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                   All Rights Reserved.
 *
 */

#include "debug.h"

# include <stdarg.h>
# include <stdio.h>

#if defined(__TEST__) || defined(__DEBUG__)

static char _dbg_print_work[_DBGPRINTWORKSIZE];

void _assert(int cond, char const* format, ...){
	if(cond){
		return;
	}

	{
		va_list ap;
		va_start(ap, format);
		vsprintf(_dbg_print_work, format, ap);
		va_end(ap);
		PRINTF(_dbg_print_work);
		PRINTF("\n");	// 強制的に前の出力をフラッシュさせる
		__asm__ ( "break" );
	}
}

void _warn(char const* format, ...){
	va_list ap;
	va_start(ap, format);
	vsprintf(_dbg_print_work, format, ap);
	va_end(ap);
	PRINTF("warnning: %s", _dbg_print_work);
}

void _trace(char const* format, ...){
	va_list ap;
	va_start(ap, format);
	vsprintf(_dbg_print_work, format, ap);
	va_end(ap);
	PRINTF("trace: %s", _dbg_print_work);
}

#endif /* defined(__TEST__) || defined(__DEBUG__) */

// [eof]
