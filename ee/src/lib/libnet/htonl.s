/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                          Libnet Library
 *
 *      Copyright (C) 2000-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version   Date        Design   Log
 *  --------------------------------------------------------------------
 *       1.0.0     2003/04/30  ksh      first version
 */
		   
#include <regdef.h>

	.text
	.p2align 3

	.globl	sceLibnetHtonl
	.globl	sceLibnetNtohl
sceLibnetHtonl:
sceLibnetNtohl:		
	.set	reorder		/* a0 = 0xaabbccdd */
	sll	v0,a0,24	/* v0 = 0xdd000000 */
	and	v1,a0,0xff00	/* v1 = 0x0000cc00 */
	sll	v1,v1,8		/* v1 = 0x00cc0000 */
	or	v0,v0,v1	/* v0 = 0xddcc0000 */
	srl	v1,a0,8		/* v1 = 0x00aabbcc */
	and	v1,v1,0xff00	/* v1 = 0x0000bb00 */
	or	v0,v0,v1	/* v0 = 0xddccbb00 */
	srl	v1,a0,24	/* v1 = 0x000000aa */
	or	v0,v0,v1	/* v0 = 0xddccbbaa */
	jr	ra
