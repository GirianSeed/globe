/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                         - <file> -
 *
 *                         Version <0.10>
 *                           Shift-JIS
 *
 *      Copyright (C) 2000-2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            <sample.c>
 *                     <main function of file read>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.10            Mar,26,1999     hakamatani  first version
 *      0.20            Jan,31,2001     koji        pfs version
 *      0.21            Jul,31,2001     koji        add some comments
 */

#include <stdio.h>
#include <string.h>
#include <eekernel.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <libcdvd.h>

#define MODPATH		"host0:/usr/local/sce/iop/modules/"
#define RW_SIZE		0x8000
#define STACKSZ		(512 * 16)
#define SECTOR_SIZE	512

typedef unsigned long long u64;

static u_char buffer[RW_SIZE] __attribute__((aligned(64)));
static u_char stack[STACKSZ] __attribute__ ((aligned(16)));


static int ShowPartition(char *devname) {
    int i, r, n, fd, sub;
    char size[16];
    char type[16];
    struct sce_dirent dirbuf;
    struct FsType {
	char   *str;
	u_short type;
    } fstype[] = {
	{"EMPTY",    0x0000},
	{"PFS",      0x0100},
	{"EXT2",     0x0083},
	{"EXT2SWAP", 0x0082},
    };

    if ((fd = sceDopen(devname)) < 0) {
	printf("cannot open device %s, %d\n", devname, fd);
	return fd;
    }

    printf("No      Size       Fs sub Id\n");

    n = 0;
    while ((r = sceDread(fd, &dirbuf)) > 0) {
	if (dirbuf.d_stat.st_size >= 0x00200000)
	    sprintf(size, "%4dGB", dirbuf.d_stat.st_size/0x00200000);
	else
	    sprintf(size, "%4dMB", dirbuf.d_stat.st_size/0x00000800);


	strcpy(type, "UNKNOWN");
	for (i=0; i<sizeof(fstype)/sizeof(struct FsType); i++)
	    if (dirbuf.d_stat.st_mode == fstype[i].type) 
		strcpy(type, fstype[i].str);

	sub = dirbuf.d_stat.st_attr & 0x01;
	printf("%03d:%c %s %8s  %2d %-16s\n", n++, (sub ? 'S' : 'M'),
		size, type, dirbuf.d_stat.st_private[0], dirbuf.d_name);
    }
    sceDclose(fd);
    return r;
}


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


static int ShowDir(char *dir) {
    int r, fd;
    struct sce_dirent dirbuf;

    if ((fd = sceDopen(dir)) < 0) {
	printf("cannot open directory %s, %d\n", dir, fd);
	return fd;
    }
    while ((r = sceDread(fd, &dirbuf)) > 0) {
	ShowStat(&dirbuf.d_stat);
	printf(" %s\n", dirbuf.d_name);
    }
    sceDclose(fd);
    return r;
}


void PowerOffThread(void *arg) {
    int sid = (int)arg;
    int stat;
    while(1) {
	WaitSema(sid);				// wait until sid is signaled
	printf("power off request has come.\n");
	// close all files
	sceDevctl("pfs:", PDIOC_CLOSEALL, NULL, 0, NULL, 0);
	// dev9 power off, need to power off PS2
	while (sceDevctl("dev9x:", DDIOC_OFF, NULL, 0, NULL, 0) < 0) ;
	// PS2 power off
	while (!sceCdPowerOff(&stat) || stat) ;
    }
}


// callback handler for power off switch is pushed
void PowerOffHandler(void *arg) {
    int sid = (int)arg;
    iSignalSema(sid);
}


static void PreparePowerOff(void) {
    struct ThreadParam tparam;
    struct SemaParam   sparam;
    int tid;
    int sid;

    sparam.initCount = 0;
    sparam.maxCount  = 1;
    sparam.option    = 0;
    sid = CreateSema(&sparam);

    ChangeThreadPriority(GetThreadId(), 2);
    tparam.stackSize = STACKSZ;
    tparam.gpReg = &_gp;
    tparam.entry = PowerOffThread;
    tparam.stack = (void *)stack;
    tparam.initPriority = 1;
    tid = CreateThread(&tparam);		// create thread
    StartThread(tid, (void *)sid);		// start thread

    // add callback handler when power off switch is pushed
    sceCdPOffCallback(PowerOffHandler, (void *)sid);
}


// example how to write partition extended attribute
static int WritePartitionInfo(int fd) {
    int i, r;
    // this must be aligned by 64
    static u_char sbuf[SECTOR_SIZE*8] __attribute__((aligned(64)));

    memset(sbuf, 0, SECTOR_SIZE*8);
    for (i=0; i<SECTOR_SIZE*8; i++) sbuf[i] = 0x5a;

    printf("write partition info.\n");
    if ((r = sceWrite(fd, sbuf, SECTOR_SIZE*8)) < 0) {
	printf("write failed.\n");
	return r;
    }

    printf("verify partition info.\n");
    if ((r = sceLseek(fd, 0, SEEK_SET)) < 0) {
	printf("seek  failed.\n");
	return r;
    }

    memset(sbuf, 0, SECTOR_SIZE*8);

    if ((r = sceRead(fd, sbuf, SECTOR_SIZE*8)) < 0) {
	printf("read  failed.\n");
	return r;
    }

    for (i=0; i<SECTOR_SIZE*8; i++)
	if (sbuf[i]!= 0x5a) printf("data failed\n");

    printf("done.\n");
    return r;
}


int main(void) {
    u_char *cp;
    int i, r, fd, rfd, wfd, len, *ip, zonesz;
     // -o: # of partitions can open at a time
     // -n: # of buffers hdd module use
    char hddarg[] = "-o" "\0" "4" "\0" "-n" "\0" "20";
     // -m: # of mounts at a time
     // -o: # of opens at a time
     // -n: # of buffers pfs module use
    char pfsarg[] = "-m" "\0" "4" "\0" "-o" "\0" "10" "\0" "-n" "\0" "40";
    int cmode = SCE_STM_RWXU | SCE_STM_RWXG | SCE_STM_RWXO;

    sceSifInitRpc(0);

    // load all hdd related modules
    if (sceSifLoadStartModule(MODPATH"dev9.irx", 0, NULL, &r) < 0) {
	printf("Can't load module dev9.\n");
	return 1;
    }
    if (r == NO_RESIDENT_END) return 1;

    if (sceSifLoadStartModule(MODPATH"atad.irx", 0, NULL, &r) < 0) {
	printf("Can't load module atad.\n");
	return 1;
    }
    if (r == NO_RESIDENT_END) return 1;

    if (sceSifLoadStartModule(MODPATH"hdd.irx", sizeof(hddarg),
						    hddarg, &r) < 0) {
	printf("Can't load module hdd.\n");
	return 1;
    }
    if (r == NO_RESIDENT_END) return 1;

    if (sceSifLoadStartModule(MODPATH"pfs.irx", sizeof(pfsarg),
						    pfsarg, &r) < 0) {
	printf("Can't load module pfs.\n");
	return 1;
    }
    if (r == NO_RESIDENT_END) return 1;

    PreparePowerOff();				// prepare to power off

    printf("sample start.\n");

    printf("Format hdd0.\n");
    if ((r = sceFormat("hdd0:", NULL, NULL, 0)) < 0) {
	printf("cannot format disk %d\n.", r);
	return 1;
    }

    printf("Create main Partition.\n");
    if ((fd = sceOpen("hdd0:test,fpasswd,rpasswd,256M,PFS",
						SCE_CREAT|SCE_RDWR)) < 0) {
	printf("could not create partition, %d.\n", fd);
	return 1;
    }

    if ((r = WritePartitionInfo(fd)) < 0) {	// write to extended attribute
	printf("could not write partition info, %d.\n", r);
	return 1;
    }

    printf("Add 8 sub partitions.\n");
    for (i=0; i<8; i++) {
	char psize[8] = "128M";
	if ((r = sceIoctl2(fd, HIOCADDSUB, psize,strlen(psize)+1,NULL,0)) < 0) {
	    printf("could not add partition, %d.\n", r);
	    return 1;
	}
    }
    sceClose(fd);
    if ((r = ShowPartition("hdd0:")) < 0) {		// list partitions
	printf("could not list partition, %d.\n", r);
	return 1;
    }

    printf("Format pfs.\n");
    zonesz = 8192;
    if ((r = sceFormat("pfs:", "hdd0:test,fpasswd", &zonesz, sizeof(int))) < 0){
	printf("cannot format partition, %d.\n", r);
	return 1;
    }

    printf("Mount pfs.\n");
    if ((r = sceMount("pfs0:", "hdd0:test,fpasswd", 0, NULL, 0)) < 0) {
	printf("cannot mount partition, %d.\n", r);
	return 1;
    }

    // this sample makes a copy of this file
    printf("\tsample0: make a copy of main.c as copy.c.\n");

    printf("\t\topen file main.c.\n");		// read onlye open
    if ((rfd = sceOpen("host:main.c", SCE_RDONLY)) < 0) {
	printf("can't open file main.c, %d.\n", rfd);
	return 1;
    }

    printf("\t\topen file copy.c.\n");		// write open
    wfd = sceOpen("pfs0:copy.c", SCE_WRONLY|SCE_TRUNC|SCE_CREAT, cmode);
    if (wfd < 0 ) {
	printf("Can't open file copy.c, %d.\n", wfd);
	return 1;
    }

    // get filesize
    len = sceLseek(rfd, 0, SCE_SEEK_END);	// get file size
    sceLseek(rfd, 0, SCE_SEEK_SET);		// reset offset
    printf("\t\tsize of main.c is %d.\n", len);
    while (len > 0) {
	r = sceRead(rfd, buffer, RW_SIZE);	// read data
	sceWrite(wfd, buffer, r);		// write data
	len -= r;
    }
    printf("\t\tcopy done.\n");
    sceClose(rfd);
    sceClose(wfd);



    // this sample write/read/verify incremental data
    printf("\tsample1: write/read/verify incremental data.\n");

    printf("\t\twrite open: incdata.\n");		// write open
    wfd = sceOpen("pfs0:incdata", SCE_RDWR|SCE_TRUNC|SCE_CREAT, cmode);
    if (wfd < 0 ) {
	printf("Can't open file, %d.\n", wfd);
	return 1;
    }

    printf("\t\tcreate incremental data.\n");		// make incremental data
    ip = (int *)buffer;
    for (i=0; i<RW_SIZE/4; i++) ip[i] = i;

    printf("\t\twrite incremental data.\n");		// write data
    if ((r = sceWrite(wfd, ip, RW_SIZE)) != RW_SIZE) {
	printf("can't write data size %d, but %d.\n", RW_SIZE, r);
	return 1;
    }
    printf("\t\tclose: incdata.\n");
    sceClose(wfd);

    printf("\t\tread open: incdata.\n");
    if ((rfd = sceOpen("pfs0:incdata", SCE_RDONLY)) < 0) {
	printf("can't open file, %d\n", rfd);
	return 1;
    }

    printf("\t\tclear buffer.\n");
    memset(buffer, 0, RW_SIZE);

    printf("\t\tread data.\n");
    cp = (u_char *)buffer;
    // read data, read size intentionally small and fractional
    while ((r = sceRead(rfd, cp, 0x112)) > 0) cp += r;

    printf("\t\tclose: incdata.\n");
    sceClose(rfd);

    printf("\t\tverify data.\n");			// verify data
    ip = (int *)buffer;
    for (i=0; i<RW_SIZE/4; i++)
	if (ip[i] != i) printf("verify error src %x dst %x\n", i, ip[i]);
    printf("sample end.\n");

    if ((r = ShowDir("pfs0:/")) < 0) {
	printf("could not list directory, %d.\n", r);
	return 1;
    }

    if ((r = sceUmount("pfs0:")) < 0) {			// unmount fs
	printf("cannot umount partition\n");
	return 1;
    }
    return 0;
}
