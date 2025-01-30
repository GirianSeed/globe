/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.7
 */
/*
 *                      Emotion Engine Library
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <libgraph.h>
#include <sifdev.h>

#define MAXWAIT	0x1000000

#ifdef SCE_GS_FUNC_001

sceGsGParam *sceGsGetGParam(void)
{
    static sceGsGParam gp = {
		1, 2, 1, 3, 0, 0,
    };

    return (&gp);
}

#endif //SCE_GS_FUNC_001

#ifdef SCE_GS_FUNC_002
void sceGsResetPath(void)
{
    u_int tmp = 0;
    static	u_int init_vif_regs[8]__attribute__((aligned(16))) = {
		0x01000404,
		0x20000000,
		0x00000000,
		0x05000000,
		0x06000000,
		0x03000000,
		0x02000000,
		0x04000000,
    };

    DPUT_VIF1_FBRST(1);
    // for DMA missmatch error bug
    DPUT_VIF1_ERR(2);
    __asm__ __volatile__("sync.l");
    __asm__ __volatile__("
		cfc2 %0, $vi28
		ori  %0, %0, 0x0200
		ctc2 %0, $vi28
		sync.p
		":: "r" (tmp)); /* VU1 reset */
    DPUT_VIF1_FIFO(*(u_long128 *)&(init_vif_regs[0]));
    DPUT_VIF1_FIFO(*(u_long128 *)&(init_vif_regs[4]));
    DPUT_GIF_CTRL(0x00000001); /* GIF reset */
}
#endif //SCE_GS_FUNC_002

#ifdef SCE_GS_FUNC_003
void sceGsSetDefDispEnv(sceGsDispEnv *disp, short psm, short w, short h, short dx, short dy)
{
    sceGsGParam *gp;

    gp = sceGsGetGParam();
    *(u_long *)&disp->pmode  = (u_long)0x0000000000000066;
    if(gp->sceGsInterMode){ // interlace
		if(gp->sceGsFFMode) // frame
			*(u_long *)&disp->smode2 = (u_long)3;
		else
			*(u_long *)&disp->smode2 = (u_long)1;
    }else{	//non-interlace
		*(u_long *)&disp->smode2 = (u_long)2;
    }
    *(u_long *)&disp->dispfb = ((((u_long)psm & 0xf) << 15) |
								((u_long)(((w+63) >> 6) & 0x3f) << 9));
    if(gp->sceGsOutMode == SCE_GS_NTSC){
		if(gp->sceGsInterMode == SCE_GS_INTERLACE){
			*(u_long *)&disp->display = 
				(((u_long)(((2560+w-1)/w)-1) << 23) |
				 ((u_long)((h<<(gp->sceGsFFMode?1:0))-1) << 44) |
				 (((u_long)636+(dx*((2560+w-1)/w)))&0xfff)|
				 ((u_long)((50+dy) & 0xfff)<<12) |
				 ((u_long)((((2560+w-1)/w)*w)-1) << 32));
		}else{
			*(u_long *)&disp->display = 
				(((u_long)(((2560+w-1)/w)-1) << 23) |
				 ((u_long)(h-1) << 44) |
				 (((u_long)636+(dx*((2560+w-1)/w)))&0xfff)|
				 ((u_long)((25+dy) & 0xfff)<<12) |
				 ((u_long)((((2560+w-1)/w)*w)-1) << 32));
		}
    }else
		if(gp->sceGsOutMode == SCE_GS_PAL){
			if(gp->sceGsInterMode == SCE_GS_INTERLACE){
				*(u_long *)&disp->display = 
					(((u_long)(((2560+w-1)/w)-1) << 23) |
					 ((u_long)((h<<(gp->sceGsFFMode?1:0))-1) << 44) |
					 (((u_long)656 + (dx*((2560+w-1)/w)))&0xfff)|
					 ((u_long)((72+dy) & 0xfff)<<12) |
					 ((u_long)((((2560+w-1)/w)*w)-1) << 32));
			}else{
				*(u_long *)&disp->display = 
					(((u_long)(((2560+w-1)/w)-1) << 23) |
					 ((u_long)(h-1) << 44) |
					 (((u_long)656 + (dx*((2560+w-1)/w)))&0xfff)|
					 ((u_long)((36+dy) & 0xfff)<<12) |
					 ((u_long)((((2560+w-1)/w)*w)-1) << 32));
			}

		}else{
			scePrintf("sceGsDefDispEnv:Not support displaymode for %d!!\n", gp->sceGsOutMode);
		}
    *(u_long *)&disp->bgcolor = (u_long)0;
}

#endif //SCE_GS_FUNC_003

#ifdef SCE_GS_FUNC_004
void sceGsPutDispEnv(sceGsDispEnv *disp)
{
    sceGsGParam *gp;

    gp = sceGsGetGParam();
    DPUT_GS_PMODE(*(u_long *)&disp->pmode);   //PMODE
    DPUT_GS_SMODE2(*(u_long *)&disp->smode2);  //SMODE2
    DPUT_GS_DISPFB2(*(u_long *)&disp->dispfb);  //DISPFB2
    DPUT_GS_DISPLAY2(*(u_long *)&disp->display); //DISPLAY2
    DPUT_GS_BGCOLOR(*(u_long *)&disp->bgcolor); //BGCOLOR
}
#endif //SCE_GS_FUNC_004

#ifdef SCE_GS_FUNC_005
short sceGszbufaddr(short psm, short w, short h)
{
    sceGsGParam *gp;
    short nw, nh;

    gp = sceGsGetGParam();
    nw = (w+63)/64;

    if(psm & 0x2){
		nh = (h+63)/64;
    }else{
		nh = (h+31)/32;
    }
    if((gp->sceGsInterMode == SCE_GS_INTERLACE) && (gp->sceGsFFMode == SCE_GS_FIELD))
		return (nw*nh);
    else
		return (nw*nh*2);
}
#endif //SCE_GS_FUNC_005

#ifdef SCE_GS_FUNC_006
short sceGszbufaddr(short psm, short w, short h);
int sceGsSetDefDrawEnv(sceGsDrawEnv1 *draw, short psm,
					   short w, short h, short ztest, short zpsm)
{
    draw->frame1addr = (long)SCE_GS_FRAME_1; //FRAME_1
    *(u_long *)&draw->frame1 =
		SCE_GS_SET_FRAME(0, ((w+63)>>6)&0x3f, psm&0xf, 0);
    draw->zbuf1addr = (long)SCE_GS_ZBUF_1; //ZBUF_1
    if(ztest == 0){
		*(u_long *)&draw->zbuf1 =
			SCE_GS_SET_ZBUF(sceGszbufaddr(psm, w, h), zpsm&0xf, 1);
    }else{
		*(u_long *)&draw->zbuf1 =
			SCE_GS_SET_ZBUF(sceGszbufaddr(psm, w, h), zpsm&0xf, 0);
    }

    draw->xyoffset1addr = (long)SCE_GS_XYOFFSET_1; //XYOFFSET_1
    *(u_long *)&draw->xyoffset1 =
		SCE_GS_SET_XYOFFSET_1(((((u_long)2048)-(w>>1))<<4),
							  (((u_long)2048 - (h>>1))<<4));

    draw->scissor1addr = (long)SCE_GS_SCISSOR_1; //SCISSOR_1
    *(u_long *)&draw->scissor1 = SCE_GS_SET_SCISSOR(0, (w-1), 0, (h-1));

    draw->prmodecontaddr = (long)SCE_GS_PRMODECONT; //PRMODECONT
    draw->prmodecont.AC = 1;

    draw->colclampaddr = (long)SCE_GS_COLCLAMP; //COLCLAMP
    draw->colclamp.CLAMP = 1;

    draw->dtheaddr = (long)SCE_GS_DTHE; //DTHE
    if(psm & 0x2)
		draw->dthe.DTHE = 1; // enable
    else
		draw->dthe.DTHE = 0; // disable

    draw->test1addr = (long)SCE_GS_TEST_1; //TEST_1
    if(ztest){
		*(u_long *)&draw->test1 = 
			SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, ztest&0x3);
    }else{
		//		*(u_long *)&draw->test1 = (u_long)0;
		// for ZTE bug
		*(u_long *)&draw->test1 = 
			SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, SCE_GS_ZALWAYS);
    }
    __asm__ __volatile__("sync.l");

    return 8;
}
#endif //SCE_GS_FUNC_006

#ifdef SCE_GS_FUNC_007
int sceGsSetDefClear(sceGsClear *cp, short ztest, short x, short y,
					 short w, short h, u_char r, u_char g, u_char b, u_char a, u_int z)
{
    cp->testaaddr = (long)SCE_GS_TEST_1; //TEST_1
	//	if(ztest){
    *(u_long *)&cp->testa = 
		SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, 1);
	/*	}else{
	 *(u_long *)&cp->testa = (u_long)0;
	 }*/

    cp->primaddr = (long)SCE_GS_PRIM; //PRIM
    *(u_long *)&cp->prim = (u_long)SCE_GS_PRIM_SPRITE; //SPRITE

    cp->rgbaqaddr = (long)SCE_GS_RGBAQ; //RGBAQ
    *(u_long *)&cp->rgbaq = 
		(u_long) SCE_GS_SET_RGBAQ(r, g, b, a, 0x3f800000);

    cp->xyz2aaddr = (long)SCE_GS_XYZ2; //XYZ2
    *(u_long *)&cp->xyz2a = SCE_GS_SET_XYZ(x<<4, y<<4, z);
	
    cp->xyz2baddr = (long)SCE_GS_XYZ2; //XYZ2
    *(u_long *)&cp->xyz2b = SCE_GS_SET_XYZ((x+w)<<4, (y+h)<<4, z);

    cp->testbaddr = (long)SCE_GS_TEST_1; //TEST_1
    if(ztest){
		*(u_long *)&cp->testb =
			SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, ztest&0x3);
    }else{
		//		*(u_long *)&cp->testb = (u_long)0;
		// for ZTE bug
		*(u_long *)&cp->testb = 
			SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, SCE_GS_ZALWAYS);
    }
    __asm__ __volatile__("sync.l");

    return 6;
}
#endif //SCE_GS_FUNC_007

#ifdef SCE_GS_FUNC_008
int sceGsPutDrawEnv(sceGifTag *giftag)
{
    u_int vcnt = 0;

    while(DGET_D2_CHCR() & 0x0100){
		if((vcnt++)>MAXWAIT){
			scePrintf("sceGsPutDrawEnv: DMA Ch.2 does not terminate\r\n");
			return -1;
		}
    }

    DPUT_D2_QWC((giftag->NLOOP+1));
    if(((u_int)giftag & 0x70000000) == 0x70000000)
		DPUT_D2_MADR((((u_int)giftag & 0x0fffffff) | 0x80000000));
    else
		DPUT_D2_MADR(((u_int)giftag & 0x0fffffff));
    *D2_CHCR = (1 << 8) | 1;
    return 0;
}
#endif //SCE_GS_FUNC_008

#ifdef SCE_GS_FUNC_009
short sceGszbufaddr(short psm, short w, short h);
void sceGsSetDefDBuff(sceGsDBuff *db, short psm, short w, short h, 
					  short ztest, short zpsm, short clear)
{
    sceGsGParam *gp;
    short  zbp;

    gp = sceGsGetGParam();
    sceGsSetDefDispEnv(&db->disp[0], psm, w, h, 0, 0);
    sceGsSetDefDispEnv(&db->disp[1], psm, w, h, 0, 0);
    sceGsSetDefDrawEnv(&db->draw0, psm, w, h, ztest, zpsm);
    sceGsSetDefDrawEnv(&db->draw1, psm, w, h, ztest, zpsm);
    if(clear){
		sceGsSetDefClear(&db->clear0, ztest, 2048 - (w>>1), 
						 2048 - (h>>1), w, h, 0, 0, 0, 0, 0);
		sceGsSetDefClear(&db->clear1, ztest, 2048 - (w>>1),
						 2048 - (h>>1), w, h, 0, 0, 0, 0, 0);
    }

    SCE_GIF_CLEAR_TAG(&db->giftag0);
    db->giftag0.NLOOP = (clear?14:8);
    db->giftag0.EOP = 1;
    db->giftag0.NREG = 1;
    db->giftag0.REGS0 = 0xe; // A_D
    SCE_GIF_CLEAR_TAG(&db->giftag1);
    db->giftag1.NLOOP = (clear?14:8);
    db->giftag1.EOP = 1;
    db->giftag1.NREG = 1;
    db->giftag1.REGS0 = 0xe; // A_D

    zbp = sceGszbufaddr(psm, w, h);

    if(((gp->sceGsInterMode == SCE_GS_INTERLACE) && (gp->sceGsFFMode == SCE_GS_FRAME)) ||
       (gp->sceGsInterMode == SCE_GS_NOINTERLACE)){
		db->disp[1].dispfb.FBP = zbp>>1;
		db->draw0.frame1.FBP = zbp>>1;
    }

}
#endif //SCE_GS_FUNC_009

#ifdef SCE_GS_FUNC_010
int sceGsSwapDBuff(sceGsDBuff *db, int id)
{
    int ret = 0;

    sceGsPutDispEnv(&db->disp[id&1]);
    if(id&1)
		ret = sceGsPutDrawEnv(&db->giftag1);
    else
		ret = sceGsPutDrawEnv(&db->giftag0);

    return ret;
}
#endif //SCE_GS_FUNC_010

#ifdef SCE_GS_FUNC_012
int sceGsSyncPath(int mode, u_short timeout)
{
    register u_int reg;
    u_int vcnt = 0;
    int ret = 0;

    if(mode == 0){ /* blocking mode */
		while(DGET_D1_CHCR() & 0x0100){
			if((vcnt++) > MAXWAIT){
				scePrintf("sceGsSyncPath: DMA Ch.1 does not terminate\r\n");
				scePrintf("\t<D1_CHCR=%08x:", DGET_D1_CHCR());
				scePrintf("D1_TADR=%08x:", DGET_D1_TADR());
				scePrintf("D1_MADR=%08x:", DGET_D1_MADR());
				scePrintf("D1_QWC=%08x>\r\n", DGET_D1_QWC());
				scePrintf("\t<D2_CHCR=%08x:", DGET_D2_CHCR());
				scePrintf("D2_TADR=%08x:", DGET_D2_TADR());
				scePrintf("D2_MADR=%08x:", DGET_D2_MADR());
				scePrintf("D2_QWC=%08x>\r\n", DGET_D2_QWC());
				scePrintf("\t<VIF1_STAT=%08x:", DGET_VIF1_STAT());
				scePrintf("GIF_STAT=%08x>\r\n", DGET_GIF_STAT());

				return -1;

			}
		}
		while(DGET_D2_CHCR() & 0x0100){
			if((vcnt++) > MAXWAIT){
				scePrintf("sceGsSyncPath: DMA Ch.2 does not terminate\r\n");
				scePrintf("\t<D1_CHCR=%08x:", DGET_D1_CHCR());
				scePrintf("D1_TADR=%08x:", DGET_D1_TADR());
				scePrintf("D1_MADR=%08x:", DGET_D1_MADR());
				scePrintf("D1_QWC=%08x>\r\n", DGET_D1_QWC());
				scePrintf("\t<D2_CHCR=%08x:", DGET_D2_CHCR());
				scePrintf("D2_TADR=%08x:", DGET_D2_TADR());
				scePrintf("D2_MADR=%08x:", DGET_D2_MADR());
				scePrintf("D2_QWC=%08x>\r\n", DGET_D2_QWC());
				scePrintf("\t<VIF1_STAT=%08x:", DGET_VIF1_STAT());
				scePrintf("GIF_STAT=%08x>\r\n", DGET_GIF_STAT());


				return -1;

			}
		}
		while(DGET_VIF1_STAT() & 0x1f000003){
			if((vcnt++) > MAXWAIT){
				scePrintf("sceGsSyncPath: VIF1 does not terminate\r\n");
				scePrintf("\t<D1_CHCR=%08x:", DGET_D1_CHCR());
				scePrintf("D1_TADR=%08x:", DGET_D1_TADR());
				scePrintf("D1_MADR=%08x:", DGET_D1_MADR());
				scePrintf("D1_QWC=%08x>\r\n", DGET_D1_QWC());
				scePrintf("\t<D2_CHCR=%08x:", DGET_D2_CHCR());
				scePrintf("D2_TADR=%08x:", DGET_D2_TADR());
				scePrintf("D2_MADR=%08x:", DGET_D2_MADR());
				scePrintf("D2_QWC=%08x>\r\n", DGET_D2_QWC());
				scePrintf("\t<VIF1_STAT=%08x:", DGET_VIF1_STAT());
				scePrintf("GIF_STAT=%08x>\r\n", DGET_GIF_STAT());

				return -1;

			}
		}
		__asm__ __volatile("cfc2 %0,$vi29" :"=r"(reg):);
		while(reg & 0x0100){
			if((vcnt++) > MAXWAIT){
				scePrintf("sceGsSyncPath: VU1 does not terminate\r\n");
				scePrintf("\t<D1_CHCR=%08x:", DGET_D1_CHCR());
				scePrintf("D1_TADR=%08x:", DGET_D1_TADR());
				scePrintf("D1_MADR=%08x:", DGET_D1_MADR());
				scePrintf("D1_QWC=%08x>\r\n", DGET_D1_QWC());
				scePrintf("\t<D2_CHCR=%08x:", DGET_D2_CHCR());
				scePrintf("D2_TADR=%08x:", DGET_D2_TADR());
				scePrintf("D2_MADR=%08x:", DGET_D2_MADR());
				scePrintf("D2_QWC=%08x>\r\n", DGET_D2_QWC());
				scePrintf("\t<VIF1_STAT=%08x:", DGET_VIF1_STAT());
				scePrintf("GIF_STAT=%08x>\r\n", DGET_GIF_STAT());

				return -1;

			}
			__asm__ __volatile("cfc2 %0,$vi29" :"=r"(reg):);
		}
		while(DGET_GIF_STAT() & 0x0c00){
			if((vcnt++) > MAXWAIT){
				scePrintf("sceGsSyncPath: GIF does not terminate\r\n");
				scePrintf("\t<D1_CHCR=%08x:", DGET_D1_CHCR());
				scePrintf("D1_TADR=%08x:", DGET_D1_TADR());
				scePrintf("D1_MADR=%08x:", DGET_D1_MADR());
				scePrintf("D1_QWC=%08x>\r\n", DGET_D1_QWC());
				scePrintf("\t<D2_CHCR=%08x:", DGET_D2_CHCR());
				scePrintf("D2_TADR=%08x:", DGET_D2_TADR());
				scePrintf("D2_MADR=%08x:", DGET_D2_MADR());
				scePrintf("D2_QWC=%08x>\r\n", DGET_D2_QWC());
				scePrintf("\t<VIF1_STAT=%08x:", DGET_VIF1_STAT());
				scePrintf("GIF_STAT=%08x>\r\n", DGET_GIF_STAT());

				return -1;

			}
		}
		return 0;
    }else{ /* non blocking mode */
		if(DGET_D1_CHCR() & 0x0100) ret = ret | 0x1;
		if(DGET_D2_CHCR() & 0x0100) ret = ret | 0x2;
		if(DGET_VIF1_STAT() & 0x1f000003) ret = ret | 0x4;
		__asm__ __volatile("cfc2 %0, $vi29" :"=r"(reg):);
		if(reg & 0x0100) ret = ret | 0x8;
		if(DGET_GIF_STAT() & 0x0c00) ret = ret | 0x10;
		return ret;
    }
}
#endif //SCE_GS_FUNC_012

#ifdef SCE_GS_FUNC_013
int sceGsSetDefTexEnv(sceGsTexEnv *tp, short flush, short tbp0, short tbw,
					  short psm, short w, short h, short tfx, short cbp, short cpsm,
					  short cld, short filter)
{
    if(flush)
		tp->texflushaddr = (long)SCE_GS_TEXFLUSH;
    else
		tp->texflushaddr = (long)SCE_GS_NOP;
    *(u_long *)&tp->texflush = (u_long)0;
    tp->tex01addr = (long)SCE_GS_TEX0_1;
    *(u_long *)&tp->tex01 = SCE_GS_SET_TEX0(tbp0, tbw, psm, w, h, 1,
											tfx, cbp, cpsm, 0, 0, cld); 
    tp->tex11addr = (long)SCE_GS_TEX1_1;
    *(u_long *)&tp->tex11 = 
		SCE_GS_SET_TEX1(0, 0, filter & 1, filter, 0, 0, 0);
    tp->clamp1addr = (long)SCE_GS_CLAMP_1;
    *(u_long *)&tp->clamp1 = SCE_GS_SET_CLAMP(1, 1, 0, 0, 0, 0);

    __asm__ __volatile__("sync.l");

    return 4;
}
#endif //SCE_GS_FUNC_013

#ifdef SCE_GS_FUNC_014
int sceGsSetDefAlphaEnv(sceGsAlphaEnv *ap, short pabe)
{
    ap->alpha1addr = (long)SCE_GS_ALPHA_1;
	/* As*Cs + (1-As)*Cd */
    *(u_long *)&ap->alpha1 = SCE_GS_SET_ALPHA(0, 1, 0, 1, 0);
    ap->pabeaddr = (long)SCE_GS_PABE;
    *(u_long *)&ap->pabe = pabe;
    ap->texaaddr = (long)SCE_GS_TEXA;
    *(u_long *)&ap->texa = SCE_GS_SET_TEXA(127, 1, 129);
    ap->fba1addr = (long)SCE_GS_FBA_1;
    *(u_long *)&ap->fba1 = SCE_GS_SET_FBA(0);

    __asm__ __volatile__("sync.l");

    return 4;
}
#endif //SCE_GS_FUNC_014

#ifdef SCE_GS_FUNC_015
int sceGsSetDefLoadImage(sceGsLoadImage *lp, short dbp, short dbw, short dpsm,
						 short x, short y, short w, short h)
{
    int size = 0;

    switch(dpsm){
    case SCE_GS_PSMCT32:
    case SCE_GS_PSMZ32:
		size = ((int)w*(int)h)>>2;
		break;
    case SCE_GS_PSMCT24:
    case SCE_GS_PSMZ24:
		size = ((int)w*(int)h*(int)3)>>4;
		break;
    case SCE_GS_PSMCT16:
    case SCE_GS_PSMCT16S:
    case SCE_GS_PSMZ16:
    case SCE_GS_PSMZ16S:
		size = ((int)w*(int)h)>>3;
		break;
    case SCE_GS_PSMT8:
    case SCE_GS_PSMT8H:
		size = ((int)w*(int)h)>>4;
		break;
    case SCE_GS_PSMT4:
    case SCE_GS_PSMT4HL:
    case SCE_GS_PSMT4HH:
		size = ((int)w*(int)h)>>5;
		break;
    }
    if(size > 32767){
		scePrintf("sceGsSetDefLoadImage: too big size\r\n");
		return 0;
    }

    SCE_GIF_CLEAR_TAG(&lp->giftag0);
    lp->giftag0.NLOOP = 4;
    lp->giftag0.NREG = 1;
    lp->giftag0.REGS0 = 0xe; // A_D

    *(u_long *)&lp->bitbltbuf =
		SCE_GS_SET_BITBLTBUF(0, 0, 0, dbp, dbw, dpsm);
    lp->bitbltbufaddr = (long)SCE_GS_BITBLTBUF;

    *(u_long *)&lp->trxpos = SCE_GS_SET_TRXPOS(0, 0, x, y, 0);
    lp->trxposaddr = (long)SCE_GS_TRXPOS;

    *(u_long *)&lp->trxreg = SCE_GS_SET_TRXREG(w, h);
    lp->trxregaddr = (long)SCE_GS_TRXREG;

    *(u_long *)&lp->trxdir = SCE_GS_SET_TRXDIR(0);
    lp->trxdiraddr = (long)SCE_GS_TRXDIR;

    SCE_GIF_CLEAR_TAG(&lp->giftag1);
    lp->giftag1.NLOOP = size;
    lp->giftag1.EOP = 1;
    lp->giftag1.FLG = SCE_GIF_IMAGE;

    __asm__ __volatile__("sync.l");

    return 6;
}
#endif //SCE_GS_FUNC_015

#ifdef SCE_GS_FUNC_016
#define tSCE_VIF1_NOP(irq) ((u_int)(irq) << 31)

#define tSCE_VIF1_MSKPATH3(msk, irq) ((u_int)(msk) | ((u_int)0x06 << 24) | ((u_int)(irq) << 31))

#define tSCE_VIF1_FLUSHA(irq) (((u_int)0x13 << 24) | ((u_int)(irq) << 31))

#define tSCE_VIF1_DIRECT(count, irq) ((u_int)(count) | ((u_int)(0x50) << 24) | ((u_int)(irq) << 31))

int sceGsSetDefStoreImage(sceGsStoreImage *sp, short sbp, short sbw, short spsm, short x, short y, short w, short h)
{
    sp->vifcode[0] = tSCE_VIF1_NOP(0);
    sp->vifcode[1] = tSCE_VIF1_MSKPATH3(0x8000, 0);
    sp->vifcode[2] = tSCE_VIF1_FLUSHA(0);
    sp->vifcode[3] = tSCE_VIF1_DIRECT(6, 0);

    SCE_GIF_CLEAR_TAG(&sp->giftag);
    sp->giftag.NLOOP = 5;
    sp->giftag.EOP = 1;
    sp->giftag.NREG = 1;
    sp->giftag.REGS0 = 0xe; // A_D

    *(u_long *)&sp->bitbltbuf = 
		SCE_GS_SET_BITBLTBUF(sbp, sbw, spsm, 0, 0, 0);
    sp->bitbltbufaddr = (long)SCE_GS_BITBLTBUF;

    *(u_long *)&sp->trxpos = SCE_GS_SET_TRXPOS(x, y, 0, 0, 0);
    sp->trxposaddr = (long)SCE_GS_TRXPOS;

    *(u_long *)&sp->trxreg = SCE_GS_SET_TRXREG(w, h);
    sp->trxregaddr = (long)SCE_GS_TRXREG;

    *(u_long *)&sp->finish = (u_long) 0;
    sp->finishaddr = (long)SCE_GS_FINISH;

    *(u_long *)&sp->trxdir = SCE_GS_SET_TRXDIR(1);
    sp->trxdiraddr = (long)SCE_GS_TRXDIR;

    __asm__ __volatile__("sync.l");

    return 7;
}
#endif //SCE_GS_FUNC_016

#ifdef SCE_GS_FUNC_017
int sceGsExecLoadImage(sceGsLoadImage *lp, u_long128 *srcaddr)
{
    u_int vcnt = 0;

    while(DGET_D2_CHCR() & 0x0100){
		if((vcnt++) > MAXWAIT){
			scePrintf("sceGsExecLoadImage: DMA Ch.2 does not terminate\r\n");
			return -1;
		}
    }

    DPUT_D2_QWC(6);
    if(((u_int)lp & 0x70000000) == 0x70000000)
		DPUT_D2_MADR((((u_int)lp & 0x0fffffff) | 0x80000000));
    else
		DPUT_D2_MADR(((u_int)lp & 0x0fffffff));
    DPUT_D2_CHCR((1 | (1<<8)));

    while(DGET_D2_CHCR() & 0x0100){
		if((vcnt++) > MAXWAIT){
			scePrintf("sceGsExecLoadImage: DMA Ch.2 does not terminate\r\n");
			return -1;
		}
    }
    DPUT_D2_QWC(lp->giftag1.NLOOP);
    if(((u_int)srcaddr & 0x70000000) == 0x70000000)
		DPUT_D2_MADR((((u_int)srcaddr & 0x0fffffff) | 0x80000000));
    else
		DPUT_D2_MADR(((u_int)srcaddr & 0x0fffffff));
    DPUT_D2_CHCR((1 | (1 << 8)));

    return 0;
}
#endif //SCE_GS_FUNC_017

#ifdef SCE_GS_FUNC_018
int sceGsExecStoreImage(sceGsStoreImage *sp, u_long128 *dstaddr)
{
    static	u_int init_mp3[4] __attribute__((aligned(16))) = {
		0x06000000,
		0x00000000,
		0x00000000,
		0x00000000,
    };

    u_int vcnt = 0;
    int w, h, i;
    int dmasizeq = 0, allsizeq, rsizeq = 0;
    int remq = 0, remb = 0, ah = 0, sizeb;
    u_char tmpbuf[16];
    u_long oldIMR;

    w = sp->trxreg.RRW;
    h = sp->trxreg.RRH;

    switch(sp->bitbltbuf.SPSM){
    case SCE_GS_PSMCT32:
    case SCE_GS_PSMZ32:
		sizeb = (((int)w*(int)h)<<2);
		remb = sizeb&0xf;
		remq = (sizeb>>4)&0x7;
		dmasizeq = (sizeb>>4)&0xfffffff8;
		if(remb == 0){
			rsizeq = 0;
			ah = h;
		}else{
			ah = (h+3)&0xfffffffc;
			allsizeq = ((int)w*ah)>>2;
			rsizeq = allsizeq - dmasizeq - remq - 1;
		}
		break;
    case SCE_GS_PSMCT24:
    case SCE_GS_PSMZ24:
		sizeb = ((int)w*(int)h*3);
		remb = sizeb &0xf;
		remq = (sizeb>>4)&0x7;
		dmasizeq = (sizeb>>4)&0xfffffff8;
		if(remb == 0){
			rsizeq = 0;
			ah = h;
		}else{
			ah = (h + 15) & 0xfffffff0;
			allsizeq = ((int)w*ah*3)>>4;
			rsizeq = allsizeq - dmasizeq - remq - 1;
		}
		break;
    case SCE_GS_PSMCT16:
    case SCE_GS_PSMCT16S:
    case SCE_GS_PSMZ16:
    case SCE_GS_PSMZ16S:
		sizeb = (((int)w*(int)h)<<1);
		remb = sizeb & 0xf;
		remq = (sizeb >> 4) & 0x7;
		dmasizeq = (sizeb>>4)& 0xfffffff8;
		if(remb == 0){
			rsizeq = 0;
			ah = h;
		} else {
			ah = (h + 7) & 0xfffffff8;
			allsizeq = ((int)w*ah)>>3;
			rsizeq = allsizeq - dmasizeq - remq - 1;
		}
		break;
    case SCE_GS_PSMT8:
    case SCE_GS_PSMT8H:
		sizeb = ((int)w*(int)h);
		remb = sizeb & 0xf;
		remq = (sizeb >> 4) & 0x7;
		dmasizeq = (sizeb >> 4) & 0xfffffff8;
		if(remb == 0){
			rsizeq = 0;
			ah = h;
		}else{
			ah = (h+7) & 0xfffffff8;
			allsizeq = ((int)w*ah)>>4;
			rsizeq = allsizeq - dmasizeq - remq - 1;
		}
		break;
    case SCE_GS_PSMT4:
    case SCE_GS_PSMT4HL:
    case SCE_GS_PSMT4HH:
		sizeb = (((int)w*(int)h)>>1);
		remb = sizeb & 0xf;
		remq = (sizeb >> 4) & 0x7;
		dmasizeq = (sizeb >> 4) & 0xfffffff8;
		if(remb == 0){
			rsizeq = 0;
			ah = h;
		}else{
			ah = (h + 7) & 0xfffffff8;
			allsizeq = ((int)w*ah)>>5;
			rsizeq = allsizeq - dmasizeq - remq - 1;
		}
		break;
    }

    if(remb != 0){
		*(u_long *)(((u_int)(&(sp->trxreg))) | 0x20000000) =
			(u_long)(sp->trxreg.RRW) | ((u_long)ah << 32);
    }

    while(DGET_D1_CHCR() & 0x0100){
		if((vcnt++) > MAXWAIT){
			scePrintf("sceGsExecStoreImage: DMA Ch.1 does not terminate\r\n");
			return -1;
		}
    }

    oldIMR = GsPutIMR(GsGetIMR() | 0x0200);
    DPUT_GS_CSR(0x02);
	
    DPUT_D1_QWC(7);
    if(((u_int)sp & 0x70000000) == 0x70000000)
		DPUT_D1_MADR((((u_int)sp & 0x0fffffff) | 0x80000000));
    else
		DPUT_D1_MADR(((u_int)sp & 0x0fffffff));
    DPUT_D1_CHCR((1 | (1<<8)));

    while(DGET_D1_CHCR() & 0x0100){
		if((vcnt++) > MAXWAIT){
			scePrintf("sceGsExecStoreImage: DMA Ch.1 does not terminate\r\n");
			return -1;
		}
    }
	
    while((DGET_GS_CSR() & 0x02) == 0){
		if((vcnt++) > MAXWAIT){
			scePrintf("sceGsExecStoreImage: GS does not terminate\r\n");
			DPUT_VIF1_FIFO(*(u_long128 *)init_mp3);
			return -1;
		}
    }

    *VIF1_STAT = 0x00800000;
    DPUT_GS_BUSDIR((u_long)0x00000001);

    if(dmasizeq){
		DPUT_D1_QWC(dmasizeq);

		if(((u_int)dstaddr & 0x70000000) == 0x70000000)
			DPUT_D1_MADR((((u_int)dstaddr & 0x0fffffff) | 0x80000000));
		else
			DPUT_D1_MADR(((u_int)dstaddr & 0x0fffffff));

		DPUT_D1_CHCR(1<<8);

		while(DGET_D1_CHCR() & 0x0100){
			if((vcnt++) > MAXWAIT){
				scePrintf("sceGsExecStoreImage: DMA Ch.1 (GS->MEM) does not terminate\r\n");
				DPUT_GS_CSR((u_long)0x00000100);
				DPUT_GS_BUSDIR((u_long)0);
				DPUT_GIF_CTRL(0x00000001);
				DPUT_VIF1_FBRST(1);
				return -1;
			}
		}
    }

    for(i = 0; i < remq; i++){
		while((*VIF1_STAT & 0x1f000000) == 0){
			if((vcnt++) > MAXWAIT){
				scePrintf("sceGsExecStoreImage: Enough data does not reach VIF1\n");
				DPUT_GS_CSR((u_long)0x00000100);
				DPUT_GS_BUSDIR((u_long)0);
				DPUT_GIF_CTRL(0x00000001);
				DPUT_VIF1_FBRST(1);
				return -1;
			}
		}
		
		((u_long128 *)dstaddr)[dmasizeq+i] = DGET_VIF1_FIFO();
    }

    if(remb){
		while((*VIF1_STAT & 0x1f000000) == 0){
			if((vcnt++) > MAXWAIT){
				scePrintf("sceGsExecStoreImage: Enough data does not reach VIF1\n");
				DPUT_GS_CSR((u_long)0x00000100);
				DPUT_GS_BUSDIR((u_long)0);
				DPUT_GIF_CTRL(0x00000001);
				DPUT_VIF1_FBRST(1);
				return -1;
			}
		}
		*(u_long128 *)&tmpbuf = DGET_VIF1_FIFO();
		for(i = 0; i < remb; i++){
			((u_char *)((u_long128 *)&dstaddr[dmasizeq+remq]))[i] = 
				tmpbuf[i];
		}
		for(i = 0; i < rsizeq; i++){
			while((*VIF1_STAT & 0x1f000000) == 0){
				if((vcnt++) > MAXWAIT){
					scePrintf("sceGsExecStoreImage: Enough data does not reach VIF1\n");
					DPUT_GS_CSR((u_long)0x00000100);
					DPUT_GS_BUSDIR((u_long)0);
					DPUT_GIF_CTRL(0x00000001);
					DPUT_VIF1_FBRST(1);
					return -1;
				}
			}
			*(u_long128 *)&tmpbuf = DGET_VIF1_FIFO();
		}
    }

    *VIF1_STAT = 0;
    DPUT_GS_BUSDIR((u_long)0);

    GsPutIMR(oldIMR);
    DPUT_GS_CSR(0x02);

    DPUT_VIF1_FIFO(*(u_long128 *)init_mp3);

    return 0;
}
#endif //SCE_GS_FUNC_018

#ifdef SCE_GS_FUNC_019
int *sceGsSyncVCallback(int (*func)(int))
{
    sceGsGParam *gp;
    int *ret;
        
    gp = sceGsGetGParam();
    ret = (int *)gp->sceGsVSCfunc;

    // unregister 
    if(func == NULL){
		DisableIntc(INTC_VBON);
		RemoveIntcHandler(INTC_VBON, gp->sceGsVSCid);
		gp->sceGsVSCid = 0;
		gp->sceGsVSCfunc = NULL;
    }
    else {  // register
		if(gp->sceGsVSCfunc != NULL){ // if the old handler exists, then remove it
			DisableIntc(INTC_VBON);
			RemoveIntcHandler(INTC_VBON, gp->sceGsVSCid);
		}	    

		gp->sceGsVSCfunc = (volatile int (*)(int))func;
		gp->sceGsVSCid = AddIntcHandler(INTC_VBON, func, -1);

		EnableIntc(INTC_VBON);
    }
    return (ret);
}
#endif //SCE_GS_FUNC_019

#ifdef SCE_GS_FUNC_020
u_long sceGsPutIMR(u_long imr)
{
    u_long ret;

    ret = GsGetIMR();
    GsPutIMR(imr);

    return (ret);
}
#endif //SCE_GS_FUNC_020

#ifdef SCE_GS_FUNC_021
void sceGsSetHalfOffset(sceGsDrawEnv1 *draw, short centerx, short centery, short  harfoff)
{
    *(u_long *)&draw->xyoffset1 = 
		SCE_GS_SET_XYOFFSET_1(
							  (((u_long)centerx - 
								(u_long)((draw->scissor1.SCAX1+1)>>1))
							   <<4),
							  (((u_long)centery -
								(u_long)((draw->scissor1.SCAY1+1)>>1))
							   <<4) + (harfoff?8:0));
}
#endif //SCE_GS_FUNC_021

#ifdef SCE_GS_FUNC_022
short sceGszbufaddr(short psm, short w, short h);
int sceGsSetDefDrawEnv2(sceGsDrawEnv2 *draw, short psm,
						short w, short h, short ztest, short zpsm)
{
    draw->frame2addr = (long)SCE_GS_FRAME_2; //FRAME_2
    *(u_long *)&draw->frame2 =
		SCE_GS_SET_FRAME(0, (w>>6)&0x3f, psm&0xf, 0);
    draw->zbuf2addr = (long)SCE_GS_ZBUF_2; //ZBUF_2
    if(ztest == 0){
		*(u_long *)&draw->zbuf2 =
			SCE_GS_SET_ZBUF(sceGszbufaddr(psm, w, h), zpsm&0xf, 1);
    }else{
		*(u_long *)&draw->zbuf2 =
			SCE_GS_SET_ZBUF(sceGszbufaddr(psm, w, h), zpsm&0xf, 0);
    }

    draw->xyoffset2addr = (long)SCE_GS_XYOFFSET_2; //XYOFFSET_2
    *(u_long *)&draw->xyoffset2 =
		SCE_GS_SET_XYOFFSET_2(((((u_long)2048)-(w>>1))<<4),
							  (((u_long)2048 - (h>>1))<<4));

    draw->scissor2addr = (long)SCE_GS_SCISSOR_2; //SCISSOR_2
    *(u_long *)&draw->scissor2 = SCE_GS_SET_SCISSOR(0, (w-1), 0, (h-1));

    draw->prmodecontaddr = (long)SCE_GS_PRMODECONT; //PRMODECONT
    draw->prmodecont.AC = 1;

    draw->colclampaddr = (long)SCE_GS_COLCLAMP; //COLCLAMP
    draw->colclamp.CLAMP = 1;

    draw->dtheaddr = (long)SCE_GS_DTHE; //DTHE
    if(psm & 0x2)
		draw->dthe.DTHE = 1; // enable
    else
		draw->dthe.DTHE = 0; // disable

    draw->test2addr = (long)SCE_GS_TEST_2; //TEST_2
    if(ztest){
		*(u_long *)&draw->test2 = 
			SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, ztest&0x3);
    }else{
		//		*(u_long *)&draw->test2 = (u_long)0;
		// for ZTE bug
		*(u_long *)&draw->test2 = 
			SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, SCE_GS_ZALWAYS);
    }
    __asm__ __volatile__("sync.l");

    return 8;
}
#endif //SCE_GS_FUNC_022

#ifdef SCE_GS_FUNC_023
int sceGsSetDefClear2(sceGsClear *cp, short ztest, short x, short y,
					  short w, short h, u_char r, u_char g, u_char b, u_char a, u_int z)
{
    cp->testaaddr = (long)SCE_GS_TEST_2; //TEST_2
	//	if(ztest){
    *(u_long *)&cp->testa = 
		SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, 1);
	/*	}else{
	 *(u_long *)&cp->testa = (u_long)0;
	 }*/

    cp->primaddr = (long)SCE_GS_PRIM; //PRIM
    *(u_long *)&cp->prim = (u_long)SCE_GS_PRIM_SPRITE | (u_long)SCE_GS_PRIM_CTXT2; //SPRITE

    cp->rgbaqaddr = (long)SCE_GS_RGBAQ; //RGBAQ
    *(u_long *)&cp->rgbaq = 
		(u_long) SCE_GS_SET_RGBAQ(r, g, b, a, 0x3f800000);

    cp->xyz2aaddr = (long)SCE_GS_XYZ2; //XYZ2
    *(u_long *)&cp->xyz2a = SCE_GS_SET_XYZ(x<<4, y<<4, z);
	
    cp->xyz2baddr = (long)SCE_GS_XYZ2; //XYZ2
    *(u_long *)&cp->xyz2b = SCE_GS_SET_XYZ((x+w)<<4, (y+h)<<4, z);

    cp->testbaddr = (long)SCE_GS_TEST_2; //TEST_2
    if(ztest){
		*(u_long *)&cp->testb =
			SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, ztest&0x3);
    }else{
		//		*(u_long *)&cp->testb = (u_long)0;
		// for ZTE bug
		*(u_long *)&cp->testb = 
			SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, SCE_GS_ZALWAYS);
    }
    __asm__ __volatile__("sync.l");

    return 6;
}
#endif //SCE_GS_FUNC_023

#ifdef SCE_GS_FUNC_024
int sceGsSetDefTexEnv2(sceGsTexEnv2 *tp, short flush, short tbp0, short tbw,
					   short psm, short w, short h, short tfx, short cbp, short cpsm,
					   short cld, short filter)
{
    if(flush)
		tp->texflushaddr = (long)SCE_GS_TEXFLUSH;
    else
		tp->texflushaddr = (long)SCE_GS_NOP;
    *(u_long *)&tp->texflush = (u_long)0;
    tp->tex02addr = (long)SCE_GS_TEX0_2;
    *(u_long *)&tp->tex02 = SCE_GS_SET_TEX0(tbp0, tbw, psm, w, h, 1,
											tfx, cbp, cpsm, 0, 0, cld); 
    tp->tex12addr = (long)SCE_GS_TEX1_2;
    *(u_long *)&tp->tex12 = 
		SCE_GS_SET_TEX1(0, 0, filter & 1, filter, 0, 0, 0);
    tp->clamp2addr = (long)SCE_GS_CLAMP_2;
    *(u_long *)&tp->clamp2 = SCE_GS_SET_CLAMP(1, 1, 0, 0, 0, 0);

    __asm__ __volatile__("sync.l");

    return 4;
}
#endif //SCE_GS_FUNC_024

#ifdef SCE_GS_FUNC_025
int sceGsSetDefAlphaEnv2(sceGsAlphaEnv2 *ap, short pabe)
{
    ap->alpha2addr = (long)SCE_GS_ALPHA_2;
	/* As*Cs + (1-As)*Cd */
    *(u_long *)&ap->alpha2 = SCE_GS_SET_ALPHA(0, 1, 0, 1, 0);
    ap->pabeaddr = (long)SCE_GS_PABE;
    *(u_long *)&ap->pabe = pabe;
    ap->texaaddr = (long)SCE_GS_TEXA;
    *(u_long *)&ap->texa = SCE_GS_SET_TEXA(127, 1, 129);
    ap->fba2addr = (long)SCE_GS_FBA_2;
    *(u_long *)&ap->fba2 = SCE_GS_SET_FBA(0);

    __asm__ __volatile__("sync.l");

    return 4;
}
#endif //SCE_GS_FUNC_025

#ifdef SCE_GS_FUNC_026
void sceGsSetHalfOffset2(sceGsDrawEnv2 *draw, short centerx, short centery, short  halfoff)
{
    *(u_long *)&draw->xyoffset2 = 
		SCE_GS_SET_XYOFFSET_2(
							  (((u_long)centerx - 
								(u_long)((draw->scissor2.SCAX1+1)>>1))
							   <<4),
							  (((u_long)centery -
								(u_long)((draw->scissor2.SCAY1+1)>>1))
							   <<4) + (halfoff?8:0));
}
#endif //SCE_GS_FUNC_026

#ifdef SCE_GS_FUNC_027
short sceGszbufaddr(short psm, short w, short h);
void sceGsSetDefDBuffDc(sceGsDBuffDc *db, short psm, short w, short h, 
						short ztest, short zpsm, short clear)
{
    sceGsGParam *gp;
    short  zbp;

    gp = sceGsGetGParam();
    sceGsSetDefDispEnv(&db->disp[0], psm, w, h, 0, 0);
    sceGsSetDefDispEnv(&db->disp[1], psm, w, h, 0, 0);
    sceGsSetDefDrawEnv(&db->draw01, psm, w, h, ztest, zpsm);
    sceGsSetDefDrawEnv2(&db->draw02, psm, w, h, ztest, zpsm);
    sceGsSetDefDrawEnv(&db->draw11, psm, w, h, ztest, zpsm);
    sceGsSetDefDrawEnv2(&db->draw12, psm, w, h, ztest, zpsm);
    if(clear){
		sceGsSetDefClear(&db->clear0, ztest, 2048 - (w>>1), 
						 2048 - (h>>1), w, h, 0, 0, 0, 0, 0);
		sceGsSetDefClear(&db->clear1, ztest, 2048 - (w>>1),
						 2048 - (h>>1), w, h, 0, 0, 0, 0, 0);
    }

    SCE_GIF_CLEAR_TAG(&db->giftag0);
    db->giftag0.NLOOP = (clear?22:16);
    db->giftag0.EOP = 1;
    db->giftag0.NREG = 1;
    db->giftag0.REGS0 = 0xe; // A_D
    SCE_GIF_CLEAR_TAG(&db->giftag1);
    db->giftag1.NLOOP = (clear?22:16);
    db->giftag1.EOP = 1;
    db->giftag1.NREG = 1;
    db->giftag1.REGS0 = 0xe; // A_D

    zbp = sceGszbufaddr(psm, w, h);

    if(((gp->sceGsInterMode == SCE_GS_INTERLACE) && (gp->sceGsFFMode == SCE_GS_FRAME)) ||
       (gp->sceGsInterMode == SCE_GS_NOINTERLACE)){
		db->disp[1].dispfb.FBP = zbp>>1;
		db->draw01.frame1.FBP = zbp>>1;
		db->draw02.frame2.FBP = zbp>>1;
    }

}
#endif //SCE_GS_FUNC_027

#ifdef SCE_GS_FUNC_028
int sceGsSwapDBuffDc(sceGsDBuffDc *db, int id)
{
    int ret = 0;

    sceGsPutDispEnv(&db->disp[id&1]);
    if(id&1)
		ret = sceGsPutDrawEnv(&db->giftag1);
    else
		ret = sceGsPutDrawEnv(&db->giftag0);

    return ret;
}
#endif //SCE_GS_FUNC_028

#ifdef SCE_GS_FUNC_029
u_long sceGsGetIMR(void)
{
    return GsGetIMR();
}
#endif //SCE_GS_FUNC_029

#ifdef SCE_GS_FUNC_030
u_long isceGsPutIMR(u_long imr)
{
    u_long ret;
    u_long iGsGetIMR(void);
    u_long iGsPutIMR(u_long arg);

    ret = iGsGetIMR();
    iGsPutIMR(imr);

    return (ret);
}
#endif //SCE_GS_FUNC_030

#ifdef SCE_GS_FUNC_031
u_long isceGsGetIMR(void)
{
    u_long iGsGetIMR(void);
    return iGsGetIMR();
}
#endif //SCE_GS_FUNC_031

