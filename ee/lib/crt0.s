/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/*
 *                      Emotion Engine Library
 *                          Version 0.10
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                       libkernel - crt0.s
 *                        kernel libraly
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *
 */

#ifdef __mips16
	.set nomips16	// This file contains 32 bit assembly code.
#endif

#define	TSTACK_SIZ  1024
#define	HSTACK_SIZ  4096
#define	ARG_SIZ     256 + 16*4 + 1*4

	.set noat
    	.set noreorder
	.global ENTRYPOINT
	.global _start
	.ent	_start
	.text			# 0x00200000
	nop
	nop
ENTRYPOINT:
_start:
	lui	$28, %hi(_gp)
	addiu	$28, $28, %lo(_gp)
zerobss:
	lui	$2, %hi(_fbss)
	lui	$3, %hi(_end)
	addiu	$2, $2, %lo(_fbss)
	addiu	$3, $3, %lo(_end)
1:
	sq	$0, 0($2)
	sltu	$1, $2, $3
	bne	$1, $0, 1b
	addiu	$2, $2, 16
 
	lui	$4, %hi(_root)
	lui	$5, %hi(_thread)
	lui	$6, %hi(_thread_stack)
	lui	$8, %hi(_gp)
	lui	$9, %hi(_launch)
	lui	$10,%hi(_args)
	addiu	$4, $4, %lo(_root)
	addiu	$5, $5, %lo(_thread)
	addiu	$6, $6, %lo(_thread_stack)
	addiu	$7, $0, TSTACK_SIZ
	addiu	$8, $8, %lo(_gp)
	addiu	$9, $9, %lo(_launch)
	addiu	$10,$10,%lo(_args)
	addiu	$3, $0, 3	#
	syscall

	lui	$5, %hi(_end)
	addiu	$4, $0, -1
	addiu	$5, $5, %lo(_end)
	addiu	$3, $0, 123	#
	syscall

	lui	$4, %hi(_gp)
	addiu	$4, $4, %lo(_gp)
	addiu	$3, $0, 63	#
	syscall
	move	$29, $2

	jal	FlushCache	# D Cache
	move	$4, $0

	lui	$2, %hi(_args)
	addiu	$2, $2, %lo(_args)
	lw	$4, 0($2)
	addiu	$5, $2, 4

	jal	main		# call main program
	nop

	j	Exit
	move	$4, $2
	.end	_start

	.global	_exit
	.ent	_exit
_exit:
	j	Exit		# Exit(0)
	move	$4, $0
	.end	_exit
    
#if 0
	.global __main
	.ent	__main
__main:	
	jr	$31
	nop
	.end	__main
#endif
	.align	3
	.ent	_root
_root:
	nop
	nop
	addiu	$3, $0, 35	# ExitThread (35)
	syscall
	.end	_root

	.align	4
	.ent	_thread
_thread:
	nop
	nop
	b	_thread
	nop
	.end	_thread

	.align	4
	.ent	_launch
_launch:
	nop
	nop
	lui	$29, %hi(_hstack_end)
	jal	$3
	addiu	$29, $29, %lo(_hstack_end)
	addiu	$3, $0, -5	# Landing (-5)
	syscall
	.end	_launch
    
	.bss
	.global	_thread_stack
	.align	6
_thread_stack:
	.space	TSTACK_SIZ
	.align	6
_hstack:.space	HSTACK_SIZ
_hstack_end:
	.align	6
_args: .space	ARG_SIZ
