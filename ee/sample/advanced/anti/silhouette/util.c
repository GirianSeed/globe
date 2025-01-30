/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *
 *      Copyright (C) 1998-2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version   Date           Design              Log
 *  --------------------------------------------------------------------
 *       1.00      Nov,07,2001    aoki
 */

#include <eekernel.h>
#include <eeregs.h>
#include <stdio.h>
#include <sifdev.h>
#include <malloc.h>
#include <stdarg.h>
#include <libpad.h>
#include <libdev.h>
#include <libgp.h>
#include "util.h"

u_long128 pad_dma_buf[scePadDmaBufferMax]  __attribute__((aligned (64)));

void pad_init(void)
{

    /* --- OPEN pad --- */
    scePadInit(0);
    scePadPortOpen(0, 0, pad_dma_buf);
}

u_int pad_read(void)
{
    u_char rdata[32];

	/* --- read pad --- */
    if(scePadRead(0, 0, rdata) > 0){
	return 0xffff ^ ((rdata[2] << 8) | rdata[3]);
    }
    else{
	return 0;
    }
}

static void *my_file_read(const char *file)
{
	size_t	size;
	void	*dptr;
	int	fd;

	if ((fd = sceOpen(file, SCE_RDONLY)) < 0) {
		printf("Can't open file %s\n", file);
		while (1)
			;
	}
	size = sceLseek(fd, 0, SCE_SEEK_END);
	sceLseek(fd, 0, SCE_SEEK_SET);

	dptr = memalign(64, size);
	if (dptr == NULL) {
		printf("Can't allocate memory for file reading %s\n", file);
		while (1)
			;
	}
	if (sceRead(fd, (u_int *)dptr, size) < 0) {
		printf("Can't read file %s\n", file);
		while (1)
			;
	}
	FlushCache(0);
	sceClose(fd);
	return dptr;
}



static int	consID;		/* debug console ID */
void draw_debug_print_init(void)
{
    sceDevConsInit();
    consID = sceDevConsOpen(0, 0, 79, 40);
}

void draw_debug_print(int x, int y, char *fmt, ...)
{
	va_list	ap;
	char	strings[640 / 4];

	va_start(ap, fmt);
	vsprintf(strings, fmt, ap);
	va_end(ap);

	if (strings[0]) {
		sceDevConsLocate(consID, x, y);
		sceDevConsPrintf(consID, strings);
	}	
}

void draw_debug_print_exec(void)
{
	sceDevConsDrawS(consID);
	sceDevConsClear(consID);
}

static const char *tim2_file[]={
    "host0:ki0.tm2",
    "host0:ki1.tm2"
};
static sceGpLoadImage bg_li[2];

void init_bg_texture()
{
    sceGpTextureArg arg;
    static u_int *bg_tm2[2];
    int i;
    
    arg.tbw = arg.tpsm = arg.tw = arg.th = arg.cbp = arg.cpsm = -1;
    arg.tx = arg.ty = 0;
    
    for (i=0; i<2; i++){
	bg_tm2[i]=my_file_read(tim2_file[i]);
	if (bg_tm2[i]==NULL) printf("%s:%d\n", __FILE__, __LINE__);

	    sceGpInitLoadImage(&bg_li[i]);

	    //	    arg.tbp=ctx->fbp*32;
	    sceGpSetLoadImageByArgTim2(&bg_li[i], &arg, bg_tm2[i], 0, 0, 0);
	    sceGpSetPacketMode(&bg_li[i], 1);
	    //	    bg_li[i]->dmanext.id=DMA_END_ID;


    }
    FlushCache(0);
}



void load_bg_texture(int fbp, int field)
{
    bg_li[field].bitbltbuf.DBP=fbp*32;
    SyncDCache(&bg_li[field].bitbltbuf, &bg_li[field].bitbltbuf);
    sceGsSyncPath(0, 0);
    sceGpKickPacket2(&bg_li[field], SCE_GP_PATH3);
}


