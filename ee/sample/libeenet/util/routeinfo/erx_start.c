/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *        Emotion Engine (I/O Processor) Library Sample Program
 *
 *                     - <libeenet routeinfo> -
 *
 *                         Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        <erx_start.c>
 *                    <erx start function>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           Apr,02,2003     komaki      first version
 */

#include <eekernel.h>
#include <liberx.h>

#include "routeinfo.h"

/* module information */
const SceErxModuleInfo Module = {
	"rtinfo",
	0x0201
};

int start(int reason, int argc, const char *const argv[])
{
	(void)argc;
	(void)argv;

	switch (reason) {
		case SCE_ERX_REASON_START:
			/* module starting */
			sceEENetShowRouteinfo();
			return (SCE_ERX_NO_RESIDENT_END);

		case SCE_ERX_REASON_STOP:
			/* module stopping */
			return (SCE_ERX_NO_RESIDENT_END);
	}

	/* unknown reason code */
	return (SCE_ERX_NO_RESIDENT_END);
}
