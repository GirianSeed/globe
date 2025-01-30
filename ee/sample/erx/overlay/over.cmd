/* SCE CONFIDENTIAL */
/*
 *      Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *        1.00          Mar.14.2002      yoshi       for overlay
 */
_stack_size = 0x00100000;
_stack = -1;
_heap_size = -1;

GROUP(-lc -lkernl -lgcc)
ENTRY(ENTRYPOINT)
SECTIONS {
	.text	0x00100000 : {
		crt0.o(.text)
		*(EXCLUDE_FILE(balls/* iga/* ezmpeg/*) .text)
	}
	.text_nop : { QUAD(0) }
	.ctors : {
		KEEP (*crtbegin.o(.ctors))
		KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
		KEEP (*(SORT(.ctors.*)))
		KEEP (*(.ctors))
		QUAD(0)
	}
	.dtors : {
		KEEP (*crtbegin.o(.dtors))
		KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
		KEEP (*(SORT(.dtors.*)))
		KEEP (*(.dtors))
		QUAD(0)
	}
	.reginfo 	: { KEEP(*(.reginfo)) }
	.data 		ALIGN(128) : { *(EXCLUDE_FILE(balls/* iga/* ezmpeg/*) .data) }
	.rodata 	ALIGN(128) : { *(EXCLUDE_FILE(balls/* iga/* ezmpeg/*) .rodata) }
	.rdata 		ALIGN(128) : { *(EXCLUDE_FILE(balls/* iga/* ezmpeg/*) .rdata) }
	.gcc_except_table	ALIGN(128): {
		*(EXCLUDE_FILE(balls/* iga/* ezmpeg/*) .gcc_except_table)
	}
	_gp = ALIGN(128) + 0x7ff0;
	.lit8 		ALIGN(128) : { *(EXCLUDE_FILE(balls/* iga/* ezmpeg/*) .lit8) }
	.lit4 		ALIGN(128) : { *(EXCLUDE_FILE(balls/* iga/* ezmpeg/*) .lit4) }
	.sdata 		ALIGN(128) : { *(EXCLUDE_FILE(balls/* iga/* ezmpeg/*) .sdata) }
	.sbss 		ALIGN(128) : { _fbss = .; *(.sbss) }
	.scommon 	ALIGN(128) : { *(.scommon) }
	.bss 		ALIGN(128) : { *(.bss) *(COMMON) }

	/* overlay area */
	/* �ŏ��� overlay �͎����I�� ALIGN(256) �����炵�� */
	. = ALIGN(256);
	OVERLAY : {
		.balls {
			balls/*(.text)
			QUAD(0)
			. = ALIGN(128);
			balls/*(.data .rodata .scommon .bss COMMON)
		}
		.iga {
			iga/*(.text)
			QUAD(0)
			. = ALIGN(128);
			iga/*(.data .rodata .vutext .vudata .vubss .scommon .bss COMMON)
		}
		.ezmpeg {
			ezmpeg/*(.text)
			QUAD(0)
			. = ALIGN(128);
			ezmpeg/*(.data .rodata .scommon .bss COMMON)
		}
	}
	/* �I�[�o�[���C�A�h���X�i���s�A�h���X�j�̒�` */
	/* main.c ���ŎQ�Ƃ��Ă���B*/
	_overlay_addr = ADDR(.balls);

	/* ���� OVERLAY �w��q�ɂ���Ĉȉ��̃V���{������`�����B   */
	/* __load_start_balls = LOADADDR(.balls)                     */
	/* __load_stop_balls = (LOADADDR(.balls)+SIZEOF(.balls))     */
	/* __load_start_iga = LOADADDR(.iga)                         */
	/* __load_stop_iga = (LOADADDR(.iga)+SIZEOF(.iga))           */
	/* __load_start_ezmpeg = LOADADDR(.ezmpeg)                   */
	/* __load_stop_ezmpeg = (LOADADDR(.ezmpeg)+SIZEOF(.ezmpeg))  */
	/*                                                           */
	/* �A�� LOADADDR() ��elf��̃I�t�Z�b�g�A�h���X�ł���A       */
	/* �{�T���v�����ǂݍ���Ŏ��s����A�h���X�ł͂Ȃ����ɒ��ӁB  */
	/* ���s�A�h���X�́A�O�q�̂悤�� ADDR() �œ�����B          */

	end = .;
	_end = .;
	/* _heap_end = . + _heap_size */
	.spad		0x70000000: {
		 crt0.o(.spad)
		 *(.spad)
	}
}
