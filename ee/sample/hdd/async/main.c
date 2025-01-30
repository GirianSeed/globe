/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
// Version 0.00
// ASCII
//
// Copyright (C) 2001 Sony Computer Entertainment Inc. All Rights Reserved.
//
// Version        Date            Design      Log
// --------------------------------------------------------------------
// 0.00           05/07/01        koji        1st version

#include <stdio.h>
#include <string.h>
#include <eekernel.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <libcdvd.h>
#include "libaio.h"

#define MODPATH		"host0:/usr/local/sce/iop/modules/"
#define BUFSZ		0x10000

typedef unsigned long long u64;


static void ShowMode(u_short mode) {
    int i;
    char *p, str[12];
    p = str;
    *p++ = (SCE_STM_ISDIR(mode)) ? 'd' : (SCE_STM_ISLNK(mode)) ? 'l': '-';
    for (i=8; i>=0; i--) {
	if ((1 << i) & mode) {
	    if (i == 9) *p++ = 's';
	    else if ((i%3) == 2) *p++ = 'r';
	    else if ((i%3) == 1) *p++ = 'w';
	    else if ((i%3) == 0) *p++ = 'x';
	} else *p++ = '-';
    }
    *p = '\0';
    printf("%s", str);
}


static void ShowStat(struct sce_stat *ss) {
    char *strmonth[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
			  "Jal", "Aug", "Sep", "Oct", "Nov", "Dec",};
    u64 size = ((u64)ss->st_hisize << 32) | ss->st_size;

    ShowMode(ss->st_mode);
    printf(" %-5d %-5d 0x%04x %5d %9d %s %2d %2d:%02d",
	    ss->st_private[0], ss->st_private[1], ss->st_attr,
	    ss->st_private[2], (u_int)size,
	    strmonth[ss->st_mtime[5]-1],
	    ss->st_mtime[4], ss->st_mtime[3], ss->st_mtime[2]);
}


static void GetStat(char *path) {
    int r;
    struct sce_stat ss;

    if ((r = sceGetstat(path, &ss)) < 0) {
	printf("cannot get stat for %s, %d\n", path, r);
	return;
    }
    ShowStat(&ss);
    printf("\n");
}


static int LoadHddModules(void) {
    int r;
    char hddarg[] = "-o" "\0" "4" "\0" "-n" "\0" "20";
    char pfsarg[] = "-m" "\0" "4" "\0" "-o" "\0" "10" "\0" "-n" "\0" "64";

    if (sceSifLoadStartModule(MODPATH"dev9.irx", 0, NULL, &r) < 0) {
	printf("Can't load module dev9.\n");
	return 0;
    }
    if (r == NO_RESIDENT_END) {
	printf("load failed:dev9.\n");
	return 0;
    }

    if (sceSifLoadStartModule(MODPATH"atad.irx", 0, NULL, &r) < 0) {
	printf("Can't load module atad.\n");
	return 0;
    }
    if (r == NO_RESIDENT_END) {
	printf("load failed:atad.\n");
	return 0;
    }

    if (sceSifLoadStartModule(MODPATH"hdd.irx", sizeof(hddarg),
						    hddarg, &r) < 0) {
	printf("Can't load module hdd.\n");
	return 0;
    }
    if (r == NO_RESIDENT_END) {
	printf("load failed:hdd.\n");
	return 0;
    }

    if (sceSifLoadStartModule(MODPATH"pfs.irx", sizeof(pfsarg),
						    pfsarg, &r) < 0) {
	printf("Can't load module pfs.\n");
	return 0;
    }
    if (r == NO_RESIDENT_END) {
	printf("load failed:pfs.\n");
	return 0;
    }
    return 1;
}


static int FormatDisk(char *dev) {
    int r;
    printf("Format %s.\n", dev);
    if ((r = sceFormat(dev, NULL, NULL, 0)) < 0) {
	printf("cannot format disk %d\n.", r);
	return 0;
    }
    return 1;
}


static int CreateAndFormatPartition(char *dev, char *name) {
    int i, r, fd;
    char p[256];
    int zonesz = 8192;
    char chsize[8] = "128M";

    sprintf(p, "%s%s,,,256M,PFS", dev, name);
    printf("create \"%s\" partition.\n", name);
    if ((fd = sceOpen(p, SCE_CREAT|SCE_RDWR)) < 0) {
	printf("could not create partition, %d.\n", fd);
	return 0;
    }

    for (i=0; i<8; i++) {
	if ((r = sceIoctl2(fd,HIOCADDSUB,chsize,strlen(chsize)+1,NULL,0)) < 0) {
	    printf("could not add partition, %d.\n", r);
	    return 0;
	}
    }
    sceClose(fd);

    sprintf(p, "%s%s", dev, name);
    printf("format \"%s\" partition.\n", name);
    if ((r = sceFormat("pfs:", p, &zonesz, sizeof(int))) < 0){
	printf("cannot format partition, %d.\n", r);
	return 0;
    }
    return 1;
}


int main(void) {
    int cmd;
    int r, rfd, wfd;
    static char buf[BUFSZ] __attribute__((aligned(64)));

    ChangeThreadPriority(GetThreadId(), 64);

    sceSifInitRpc(0);
    if (!LoadHddModules()) return 1;
    AioInit(32);				// initialize libaio

#if 1
    if (!FormatDisk("hdd0:")) return 1;
    if (!CreateAndFormatPartition("hdd0:", "test")) return 1;
#endif

    printf("mount \"%s\" partition.\n", "test");
    if ((r = sceMount("pfs0:", "hdd0:test", 0, NULL, 0)) < 0) {
	printf("cannot mount partition, %d.\n", r);
	return 1;
    }

    // this sample makes a copy of this file
    printf("\tsample: make a copy of main.c as copy.c\n");


    printf("\t\topen file main.c.\n");		// read open
    AioOpen("host:main.c", SCE_RDONLY, 0);
    AioCmdSync(0, &cmd, &rfd);			// wait until open done
    if (rfd < 0 ) {
	printf("can't open file main.c, %d.\n", rfd);
	return 1;
    }


    printf("\t\topen file copy.c.\n");		// write open
    AioOpen("pfs0:copy.c", SCE_RDWR|SCE_TRUNC|SCE_CREAT, SCE_STM_RWXUGO);
    AioCmdSync(0, &cmd, &wfd);
    if (wfd < 0 ) {
	printf("can't open file copy.c, %d.\n", wfd);
	return 1;
    }

    while (AioRead(rfd, buf, BUFSZ), AioCmdSync(0, &cmd, &r), r > 0) {
	AioWrite(wfd, buf, r);
	AioCmdSync(0, &cmd, &r);
	if (r < 0) {
	    printf("can't write to copy.c, %d.\n", r);
	    break;
	}
    }

    printf("\t\tcopy done.\n");
    sceClose(rfd);
    sceClose(wfd);

    printf("\n");
    GetStat("pfs0:copy.c");
    printf("\n");

    if ((r = sceUmount("pfs0:")) < 0) {
	printf("cannot umount partition\n");
	return 1;
    }
    return 0;
}
