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
 *                         <routeinfo.c>
 *               <sample function of "route" command>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           Dec,10,2002     komaki      first version
 */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include <libeenet.h>

#include <sys/socket.h>
#include <net/if.h>
#include <net/route.h>

#define MAX_ROUTEINFO_NUM 10

#define PRINT_BUF_SIZE 256

static char print_buf[PRINT_BUF_SIZE];
static struct sceEENetRouteinfo head[MAX_ROUTEINFO_NUM];

#define PRINTF(...) \
    do { \
        sceSnprintf(print_buf, PRINT_BUF_SIZE, __VA_ARGS__), scePrintf("%s", print_buf); \
    }while(0)

#define NPRINTF(n, ...) \
    do { \
	n += sceSnprintf(print_buf, PRINT_BUF_SIZE, __VA_ARGS__), scePrintf("%s", print_buf); \
    }while(0)


void sceEENetShowRouteinfo(void) {
	int n, num;
	struct sceEENetRouteinfo *rinfo;

	PRINTF("Internet:\n");
	PRINTF("%-16s%-16s%-16s%-6s%-6s\n", "Destination", "Gateway", "Netmask", "Flags", "Interface");

	num = MAX_ROUTEINFO_NUM;
	sceEENetGetRouteinfo(head, &num);

	for (rinfo = head; rinfo < head + num; rinfo++) {
		PRINTF("%-16s", sceEENetInetNtoa(rinfo->ri_dstaddr));

		if (rinfo->ri_flags & RTF_GATEWAY || rinfo->ri_flags & RTF_HOST)
			PRINTF("%-16s", sceEENetInetNtoa(rinfo->ri_gateway));
		else
			PRINTF("%-16s", "*");

		if (!(rinfo->ri_flags & RTF_HOST))
			PRINTF("%-16s", sceEENetInetNtoa(rinfo->ri_netmask));
		else
			PRINTF("%-16s", "*");
		n = 0;
		if (rinfo->ri_flags & RTF_UP)
			NPRINTF(n, "U");
		if (rinfo->ri_flags & RTF_GATEWAY)
			NPRINTF(n, "G");
		if (rinfo->ri_flags & RTF_HOST)
			NPRINTF(n, "H");
		if (rinfo->ri_flags & RTF_DYNAMIC)
			NPRINTF(n, "D");
		if (rinfo->ri_flags & RTF_MODIFIED)
			NPRINTF(n, "M");
		if (rinfo->ri_flags & RTF_STATIC)
			NPRINTF(n, "S");
		PRINTF("%*s", 6 - n, "");
		PRINTF("%-6s", rinfo->ri_ifname);
		PRINTF("\n");
	}
}
