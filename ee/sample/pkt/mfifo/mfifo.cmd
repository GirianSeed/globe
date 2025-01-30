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
 *                       libkernel - app.cmd
 *                        kernel libraly
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.11            May.10.1999     horikawa
 */

GROUP(-lc -lkernl -lgcc)
ENTRY(ENTRYPOINT)
SECTIONS {
	.indata		0x00100000: { *(.indata) }
	.text		0x00200000: {
		crt0.o(.text)
		*(.text)
	}
	.reginfo		  : { KEEP(*(.reginfo)) }
	.data		ALIGN(128): { *(.data) }
	.rodata		ALIGN(128): { *(.rodata) }
	.rdata		ALIGN(128): { *(.rdata) }
	_gp = ALIGN(128) + 0x7ff0;
	.lit8       	ALIGN(128): { *(.lit8) }
	.lit4       	ALIGN(128): { *(.lit4) }
	.sdata		ALIGN(128): { *(.sdata) }
	.sbss		ALIGN(128): { _fbss = .; *(.sbss) *(.scommon) }
	.bss		ALIGN(128): { *(.bss) }
	.mfifo		ALIGN(0x40000): { *(.mfifo) }
	.gcc_except_table ALIGN(128): { *(.gcc_except_table) }
	end = .;
	_end = .;
	.spad		0x70000000: {
		 crt0.o(.spad)
		 *(.spad)
	}
}
