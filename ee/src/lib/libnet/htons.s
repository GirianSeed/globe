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

	.globl	sceLibnetHtons
	.globl	sceLibnetNtohs
sceLibnetHtons:
sceLibnetNtohs:
	.set	reorder		/* a0 = 0xXXXXaabb */
	sll	v0,a0,8		/* v0 = 0xXXaabb00 */
	and	v0,v0,0xff00	/* v0 = 0x0000bb00 */
	srl	v1,a0,8		/* v1 = 0x00XXXXaa */
	and	v1,v1,0x00ff	/* v1 = 0x000000aa */
	or	v0,v0,v1	/* v0 = 0x0000bbaa */
	jr	ra
