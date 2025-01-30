/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                I/O Processor Library Sample Program
 *
 *                             - sqdump -
 *
 *                            Version 1.00
 *                              Shift-JIS
 *
 *      Copyright (C) 2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            Apr,24,2003     gens        create
 */
#ifndef _MYASSERT_H_
#define _MYASSERT_H_

#ifndef _DEBUG /* { */

/* _DEBUG ‚ª’è‹`‚³‚ê‚Ä‚¢‚È‚¢Žž‚Í–³Œø‚Æ‚È‚é. */
/* not finished yet */
#define my_assert(e, msg)

#else  /* }{ */

#include <kernel.h>
#include <stdio.h>

#define my_assert(e, msg) { \
	if(!(e)) { \
		printf("** Assertion ! %s: %d (%s)\n", __FILE__, __LINE__, #e); \
		printf("** %s\n", msg); \
		while(1) WaitVblank(); \
	}}

#endif /* } */

#endif
