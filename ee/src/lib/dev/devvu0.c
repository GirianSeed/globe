/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                      Emotion Engine Library
 *                          Version 0.01
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        libpkt - devvu0.c
 *                         develop library
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.01           Mar,29,1999     shibuya
 *       0.02           Apr,29,2003     hana
 */

#include <eekernel.h>
#include <eeregs.h>
#include "devvu0.h"


static int sceDevVu0Sync();

static u_int sceDevVu0CheckBusy()
{
	register u_int reg;
	__asm__ volatile ("cfc2 %0, $vi29" : "=r"(reg)); // vpu stat

	if (0x00000001 == (reg & 0x0000000f)) {
		return 1;
	} /*  vu0 busy ?  */
	return 0;
}

void sceDevVu0Reset(void)
{
	register u_int reg;
	__asm__ volatile ("cfc2 %0, $vi28" : "=r"(reg));

	reg |= 0x00000002;
	__asm__ volatile ("ctc2 %0, $vi28" : : "r"(reg));
}

int sceDevVu0Pause(void)
{
	int result = 0;

	switch (sceDevVu0Sync()) {
	case 1:
	case 2:
		break;

	case 4:
		{
			register u_int reg;
			__asm__ volatile ("cfc2 %0, $vi28" : "=r"(reg));

			reg |= 0x00000001;
			__asm__ volatile ("ctc2 %0, $vi28" : : "r"(reg));
		}
	case 0:
	case 3:
		result = 1;
		break;
	}

	return result;
}

int sceDevVu0Continue(void)
{
	int result = 0;

	switch (sceDevVu0Sync()) {
	case 0:
	case 4:
		break;

	case 1:
	case 2:
	case 3:
		{
			sceDevVu0Exec(sceDevVu0GetTpc());
		}
		result = 1;
		break;
	}

	return result;
}

void sceDevVu0PutDBit(int bit)
{
	register u_int reg;
	__asm__ volatile ("cfc2 %0, $vi28" : "=r"(reg));

	if (1 == bit) {
		reg |= 0x00000004;

	} else {
		reg &= ~0x00000004;
	}
	__asm__ volatile ("ctc2 %0, $vi28" : : "r"(reg));
}

void sceDevVu0PutTBit(int bit)
{
	register u_int reg;
	__asm__ volatile ("cfc2 %0, $vi28" : "=r"(reg));

	if (1 == bit) {
		reg |= 0x00000008;

	} else {
		reg &= ~0x00000008;
	}
	__asm__ volatile ("ctc2 %0, $vi28" : : "r"(reg));
}

int sceDevVu0GetDBit(void)
{
	register u_int reg;
	__asm__ volatile ("cfc2 %0, $vi28" : "=r"(reg));

	return (int)((reg >> 2) & 1);
}

int sceDevVu0GetTBit(void)
{
	register u_int reg;
	__asm__ volatile ("cfc2 %0, $vi28" : "=r"(reg));

	return (int)((reg >> 3) & 1);
}

static int sceDevVu0Sync(void)
{
	int status = 0;
	register u_int reg;
	__asm__ volatile ("cfc2 %0, $vi29" : "=r"(reg));

	if (reg & 0x00000002) {
		status = 1;				//  D bit stop

	} else if (reg & 0x00000004) {
		status = 2;				//  T bit stop

	} else if (reg & 0x00000008) {
		status = 3;				//  ForceBreak stop

	} else if (reg & 0x00000001) {	//  vu0 busy
		status = 4;

	} else {
		status = 0;				//  vu0 idle
	}

	return status;
}

void sceDevVu0Exec(u_short addr)
{
	register u_int reg = addr;
	__asm__ volatile (
		"ctc2 %0, $vi27\n"
		"vcallmsr $vi27\n"
		: : "r"(reg)
	);
}

u_short sceDevVu0GetTpc(void)
{
	register u_int reg;
	__asm__ volatile ("cfc2 %0, $vi26" : "=r"(reg));

	return (u_short) reg;
}

int sceDevVu0GetCnd(sceDevVu0Cnd * cnd)
{

	register u_int reg;
	register u_int base = (u_int) (cnd->vf);

	if (sceDevVu0CheckBusy()) {
		return 0;
	}
	
	__asm__ volatile (
		"sqc2 $vf00, 0x0000(%0)\n"
		"sqc2 $vf01, 0x0010(%0)\n"
		"sqc2 $vf02, 0x0020(%0)\n"
		"sqc2 $vf03, 0x0030(%0)\n"
		"sqc2 $vf04, 0x0040(%0)\n"
		"sqc2 $vf05, 0x0050(%0)\n"
		"sqc2 $vf06, 0x0060(%0)\n"
		"sqc2 $vf07, 0x0070(%0)\n"
		"sqc2 $vf08, 0x0080(%0)\n"
		"sqc2 $vf09, 0x0090(%0)\n"
		"sqc2 $vf10, 0x00a0(%0)\n"
		"sqc2 $vf11, 0x00b0(%0)\n"
		"sqc2 $vf12, 0x00c0(%0)\n"
		"sqc2 $vf13, 0x00d0(%0)\n"
		"sqc2 $vf14, 0x00e0(%0)\n"
		"sqc2 $vf15, 0x00f0(%0)\n"
		"sqc2 $vf16, 0x0100(%0)\n"
		"sqc2 $vf17, 0x0110(%0)\n"
		"sqc2 $vf18, 0x0120(%0)\n"
		"sqc2 $vf19, 0x0130(%0)\n"
		"sqc2 $vf20, 0x0140(%0)\n"
		"sqc2 $vf21, 0x0150(%0)\n"
		"sqc2 $vf22, 0x0160(%0)\n"
		"sqc2 $vf23, 0x0170(%0)\n"
		"sqc2 $vf24, 0x0180(%0)\n"
		"sqc2 $vf25, 0x0190(%0)\n"
		"sqc2 $vf26, 0x01a0(%0)\n"
		"sqc2 $vf27, 0x01b0(%0)\n"
		"sqc2 $vf28, 0x01c0(%0)\n"
		"sqc2 $vf29, 0x01d0(%0)\n"
		"sqc2 $vf30, 0x01e0(%0)\n"
		"sqc2 $vf31, 0x01f0(%0)\n"
		
		: : "r"(base) : "memory"
	);

	__asm__ volatile ("cfc2 %0, $vi16" : "=r"(reg));
	cnd->status = reg;
	__asm__ volatile ("cfc2 %0, $vi17" : "=r"(reg));
	cnd->mac = reg;
	__asm__ volatile ("cfc2 %0, $vi18" : "=r"(reg));
	cnd->clipping = reg;
	__asm__ volatile ("cfc2 %0, $vi20" : "=r"(reg));
	cnd->r = reg;
	__asm__ volatile ("cfc2 %0, $vi21" : "=r"(reg));
	cnd->i = reg;
	__asm__ volatile ("cfc2 %0, $vi22" : "=r"(reg));
	cnd->q = reg;

	__asm__ volatile ("cfc2 %0, $vi0":"=r"(reg):);
	cnd->vi[0] = (u_short) reg;
	__asm__ volatile ("cfc2 %0, $vi1":"=r"(reg):);
	cnd->vi[1] = (u_short) reg;
	__asm__ volatile ("cfc2 %0, $vi2":"=r"(reg):);
	cnd->vi[2] = (u_short) reg;
	__asm__ volatile ("cfc2 %0, $vi3":"=r"(reg):);
	cnd->vi[3] = (u_short) reg;
	__asm__ volatile ("cfc2 %0, $vi4":"=r"(reg):);
	cnd->vi[4] = (u_short) reg;
	__asm__ volatile ("cfc2 %0, $vi5":"=r"(reg):);
	cnd->vi[5] = (u_short) reg;
	__asm__ volatile ("cfc2 %0, $vi6":"=r"(reg):);
	cnd->vi[6] = (u_short) reg;
	__asm__ volatile ("cfc2 %0, $vi7":"=r"(reg):);
	cnd->vi[7] = (u_short) reg;
	__asm__ volatile ("cfc2 %0, $vi8":"=r"(reg):);
	cnd->vi[8] = (u_short) reg;
	__asm__ volatile ("cfc2 %0, $vi9":"=r"(reg):);
	cnd->vi[9] = (u_short) reg;
	__asm__ volatile ("cfc2 %0, $vi10":"=r"(reg):);
	cnd->vi[10] = (u_short) reg;
	__asm__ volatile ("cfc2 %0, $vi11":"=r"(reg):);
	cnd->vi[11] = (u_short) reg;
	__asm__ volatile ("cfc2 %0, $vi12":"=r"(reg):);
	cnd->vi[12] = (u_short) reg;
	__asm__ volatile ("cfc2 %0, $vi13":"=r"(reg):);
	cnd->vi[13] = (u_short) reg;
	__asm__ volatile ("cfc2 %0, $vi14":"=r"(reg):);
	cnd->vi[14] = (u_short) reg;
	__asm__ volatile ("cfc2 %0, $vi15":"=r"(reg):);
	cnd->vi[15] = (u_short) reg;

	return 1;
}

int sceDevVu0PutCnd(sceDevVu0Cnd * cnd)
{
	register u_int reg;
	register u_int base = (u_int)(cnd->vf);

	if (sceDevVu0CheckBusy()) {
		return 0;
	}

	__asm__ volatile (
		"lqc2 $vf00, 0x0000(%0)\n"
		"lqc2 $vf01, 0x0010(%0)\n"
		"lqc2 $vf02, 0x0020(%0)\n"
		"lqc2 $vf03, 0x0030(%0)\n"
		"lqc2 $vf04, 0x0040(%0)\n"
		"lqc2 $vf05, 0x0050(%0)\n"
		"lqc2 $vf06, 0x0060(%0)\n"
		"lqc2 $vf07, 0x0070(%0)\n"
		"lqc2 $vf08, 0x0080(%0)\n"
		"lqc2 $vf09, 0x0090(%0)\n"
		"lqc2 $vf10, 0x00a0(%0)\n"
		"lqc2 $vf11, 0x00b0(%0)\n"
		"lqc2 $vf12, 0x00c0(%0)\n"
		"lqc2 $vf13, 0x00d0(%0)\n"
		"lqc2 $vf14, 0x00e0(%0)\n"
		"lqc2 $vf15, 0x00f0(%0)\n"
		"lqc2 $vf16, 0x0100(%0)\n"
		"lqc2 $vf17, 0x0110(%0)\n"
		"lqc2 $vf18, 0x0120(%0)\n"
		"lqc2 $vf19, 0x0130(%0)\n"
		"lqc2 $vf20, 0x0140(%0)\n"
		"lqc2 $vf21, 0x0150(%0)\n"
		"lqc2 $vf22, 0x0160(%0)\n"
		"lqc2 $vf23, 0x0170(%0)\n"
		"lqc2 $vf24, 0x0180(%0)\n"
		"lqc2 $vf25, 0x0190(%0)\n"
		"lqc2 $vf26, 0x01a0(%0)\n"
		"lqc2 $vf27, 0x01b0(%0)\n"
		"lqc2 $vf28, 0x01c0(%0)\n"
		"lqc2 $vf29, 0x01d0(%0)\n"
		"lqc2 $vf30, 0x01e0(%0)\n"
		"lqc2 $vf31, 0x01f0(%0)\n"
		: : "r"(base)
	);

	reg = cnd->status;
	__asm__ volatile ("ctc2 %0, $vi16"::"r"(reg));
	
	//reg = cnd->mac;
	//__asm__ volatile ("ctc2 %0, $vi17"::"r"(reg)); // MACƒtƒ‰ƒO‚Í•œŒ³‚µ‚È‚¢
	
	reg = cnd->clipping;
	__asm__ volatile ("ctc2 %0, $vi18"::"r"(reg));
	reg = cnd->r;
	__asm__ volatile ("ctc2 %0, $vi20"::"r"(reg));
	reg = cnd->i;
	__asm__ volatile ("ctc2 %0, $vi21"::"r"(reg));
	reg = cnd->q;
	__asm__ volatile ("ctc2 %0, $vi22"::"r"(reg));
	
	reg = cnd->vi[0];
	__asm__ volatile ("ctc2 %0, $vi0"::"r"(reg));
	reg = cnd->vi[1];
	__asm__ volatile ("ctc2 %0, $vi1"::"r"(reg));
	reg = cnd->vi[2];
	__asm__ volatile ("ctc2 %0, $vi2"::"r"(reg));
	reg = cnd->vi[3];
	__asm__ volatile ("ctc2 %0, $vi3"::"r"(reg));
	reg = cnd->vi[4];
	__asm__ volatile ("ctc2 %0, $vi4"::"r"(reg));
	reg = cnd->vi[5];
	__asm__ volatile ("ctc2 %0, $vi5"::"r"(reg));
	reg = cnd->vi[6];
	__asm__ volatile ("ctc2 %0, $vi6"::"r"(reg));
	reg = cnd->vi[7];
	__asm__ volatile ("ctc2 %0, $vi7"::"r"(reg));
	reg = cnd->vi[8];
	__asm__ volatile ("ctc2 %0, $vi8"::"r"(reg));
	reg = cnd->vi[9];
	__asm__ volatile ("ctc2 %0, $vi9"::"r"(reg));
	reg = cnd->vi[10];
	__asm__ volatile ("ctc2 %0, $vi10"::"r"(reg));
	reg = cnd->vi[11];
	__asm__ volatile ("ctc2 %0, $vi11"::"r"(reg));
	reg = cnd->vi[12];
	__asm__ volatile ("ctc2 %0, $vi12"::"r"(reg));
	reg = cnd->vi[13];
	__asm__ volatile ("ctc2 %0, $vi13"::"r"(reg));
	reg = cnd->vi[14];
	__asm__ volatile ("ctc2 %0, $vi14"::"r"(reg));
	reg = cnd->vi[15];
	__asm__ volatile ("ctc2 %0, $vi15"::"r"(reg));

	return 1;
}

/* [eof] */

