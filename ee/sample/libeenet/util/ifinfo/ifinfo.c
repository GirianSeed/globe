/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *        Emotion Engine (I/O Processor) Library Sample Program
 *
 *                     - <libeenet ifinfo> -
 *
 *                         Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          <ifinfo.c>
 *             <sample function of "ifconfig" command>
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
#include <netinet/in.h>

#define MAX_IFNUM 3

static char buf[256];
static char *iftype_str[] = {
	"Unknown",
	"Local Loopback",
	"Ethernet",
	"PPP",
	"PPPoE",
};
static struct sceEENetIfname ifnames[MAX_IFNUM];

#define PRINTF(...) scePrintf(__VA_ARGS__)

void sceEENetShowIfinfo(const char *ifname) {
	int buflen;
	struct sceEENetIfstat ifs;

	/* 1st line - ifname, Vendor & Product */
	PRINTF("%s\t", ifname);
	PRINTF("Vendor:");
	buflen = 256;
	if (sceEENetGetIfinfo(ifname, sceEENET_IFINFO_VENDOR_NAME, buf, &buflen) == 0)
		PRINTF("%s", buf);
	else
		PRINTF("(N/A)");
	PRINTF("  Product:");
	buflen = 256;
	if (sceEENetGetIfinfo(ifname, sceEENET_IFINFO_PRODUCT_NAME, buf, &buflen) == 0)
		PRINTF("%s", buf);
	else
		PRINTF("(N/A)");
	PRINTF("\n");

	/* 2nd line - Link encap & HWaddr */
	PRINTF("\t");
	PRINTF("Link encap:");
	buflen = 256;
	if (sceEENetGetIfinfo(ifname, sceEENET_IFINFO_IFTYPE, buf, &buflen) == 0)
		PRINTF("%s  ", iftype_str[*((int *)buf)]);
	else
		PRINTF("(error)  ");
	buflen = 256;
	if (sceEENetGetIfinfo(ifname, sceEENET_IFINFO_MACADDR, buf, &buflen) == 0)
		PRINTF("HWaddr %s", sceEENetEtherNtoa((struct sceEENetEtherAddr *)buf));
	PRINTF("\n");

	/* 3rd line - Media type (if available) */
	buflen = 256;
	if (sceEENetGetIfinfo(ifname, sceEENET_IFINFO_MEDIA, buf, &buflen) == 0) {
		PRINTF("\t");
		PRINTF("Media:");
		if (*((u_short *)buf) & sceEENET_MEDIA_10_T)
			PRINTF("10baseT ");
		else if (*((u_short *)buf) & sceEENET_MEDIA_100_TX)
			PRINTF("100baseTX ");
		if (*((u_short *)buf) & sceEENET_MEDIA_HDX)
			PRINTF("half-duplex ");
		else if (*((u_short *)buf) & sceEENET_MEDIA_FDX)
			PRINTF("full-duplex ");
		if (*((u_short *)buf) & sceEENET_MEDIA_FLOW)
			PRINTF("with flow-control ");
		if (*((u_short *)buf) & sceEENET_MEDIA_AVALID) {
			if (*((u_short *)buf) & sceEENET_MEDIA_ACTIVE)
				PRINTF("(active)");
			else
				PRINTF("(no carrier)");
		}
		PRINTF("\n");
	}

	/* 4th line - addr, Bcast & Mask (if assigned) */
	buflen = 256;
	if (sceEENetGetIfinfo(ifname, sceEENET_IFINFO_ADDR, buf, &buflen) == 0) {
		PRINTF("\t");
		PRINTF("inet addr:%s  ", sceEENetInetNtoa(*((struct sceEENetInAddr *)buf)));
		buflen = 256;
		if (sceEENetGetIfinfo(ifname, sceEENET_IFINFO_BROADADDR, buf, &buflen) == 0)
			PRINTF("Bcast:%s  ", sceEENetInetNtoa(*((struct sceEENetInAddr *)buf)));
		buflen = 256;
		if (sceEENetGetIfinfo(ifname, sceEENET_IFINFO_NETMASK, buf, &buflen) == 0)
			PRINTF("Mask:%s", sceEENetInetNtoa(*((struct sceEENetInAddr *)buf)));
		PRINTF("\n");
	}

	/* 5th line - flags */
	PRINTF("\t");
	buflen = 256;
	if (sceEENetGetIfinfo(ifname, sceEENET_IFINFO_IFFLAGS, buf, &buflen) == 0) {
		if (*((u_short *)buf) & IFF_UP)
			PRINTF("UP ");
		if (*((u_short *)buf) & IFF_BROADCAST)
			PRINTF("BROADCAST ");
		if (*((u_short *)buf) & IFF_LOOPBACK)
			PRINTF("LOOPBACK ");
		if (*((u_short *)buf) & IFF_POINTOPOINT)
			PRINTF("POINTOPOINT ");
		if (*((u_short *)buf) & IFF_RUNNING)
			PRINTF("RUNNING ");
		if (*((u_short *)buf) & IFF_PROMISC)
			PRINTF("PROMISC ");
		if (*((u_short *)buf) & IFF_ALLMULTI)
			PRINTF("ALLMULTI ");
		if (*((u_short *)buf) & IFF_MULTICAST)
			PRINTF("MULTICAST ");
		PRINTF(" ");
	}
	buflen = 256;
	PRINTF("MTU:");
	if (sceEENetGetIfinfo(ifname, sceEENET_IFINFO_MTU, buf, &buflen) == 0)
		PRINTF("%d", *((int *)buf));
	else
		PRINTF("(error)");
	PRINTF("\n");

	/* 6th - 8th line - statistics */
	if (sceEENetGetIfstat(ifname, &ifs) == 0) {
		PRINTF("\tRX packets:%d errors:%d dropped:%d\n",
			ifs.ifs_ipackets, ifs.ifs_ierrors, ifs.ifs_iqdrops);
		PRINTF("\tTX packets:%d errors:%d\n",
			ifs.ifs_opackets, ifs.ifs_oerrors);
		PRINTF("\tcollisions: %d noproto:%d\n",
			ifs.ifs_collisions, ifs.ifs_noproto);
	}
}

void sceEENetShowAllIfinfo(void) {
	int num = MAX_IFNUM;
	struct sceEENetIfname *ifn;

	sceEENetGetIfnames(ifnames, &num);
	for (ifn = ifnames; ifn < ifnames + num; ifn++) {
		sceEENetShowIfinfo(ifn->ifn_name);
		PRINTF("\n");
	}
}
