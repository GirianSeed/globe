/* 
 *
 *  C lang start support asm routine for IOP native kernel
 *
 *  Copyright 1998 by Sony Computer Entertainment Inc.
 *  All Rights Reserved
 * 
 * $Id: crt0.s,v 1.1 1999/08/15 09:35:59 tei Exp $
 */

#include <cpureg.h>

#define RESIDENT_END	0
#define NO_RESIDENT_END	1

	.lcomm	save_sp, 4

	.text
	.globl	start
	.globl	exit
	.globl	__main

	.ent	start
start:
	
	subu	sp, (16+4)*4
	sw	s0, 4*4(sp)
	sw	s1, 5*4(sp)
	sw	s2, 6*4(sp)
	sw	s3, 7*4(sp)
	sw	s4, 8*4(sp)
	sw	s5, 9*4(sp)
	sw	s6, 10*4(sp)
	sw	s7, 11*4(sp)
	sw	fp, 12*4(sp)
	sw	ra, 13*4(sp)
	sw	sp, save_sp

#if 0
	sw	a0, 14*4(sp)
	sw	a1, 15*4(sp)
	sw	a2, 16*4(sp)
	sw	a3, 17*4(sp)
	jal	???init???	if need
	lw	a0, 14*4(sp)
	lw	a1, 15*4(sp)
	lw	a2, 16*4(sp)
	lw	a3, 17*4(sp)
#endif

	jal	main
	.end	start

	.ent	exit
exit:	
	lw	sp, save_sp
	lw	s0, 4*4(sp)
	lw	s1, 5*4(sp)
	lw	s2, 6*4(sp)
	lw	s3, 7*4(sp)
	lw	s4, 8*4(sp)
	lw	s5, 9*4(sp)
	lw	s6, 10*4(sp)
	lw	s7, 11*4(sp)
	lw	fp, 12*4(sp)
	lw	ra, 13*4(sp)

	addu	sp, (16+4)*4
	li	v0, NO_RESIDENT_END
	j	ra
	.end	exit

	.ent	__main
__main:	j	ra
	.end	__main

