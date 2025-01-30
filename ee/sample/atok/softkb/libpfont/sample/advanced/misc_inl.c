/* SCE CONFIDENTIAL
   "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
*/
/*
 *     Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                   All Rights Reserved.
 *
 */

#undef __STRICT_ANSI__
#include <math.h>
#include <eekernel.h>

extern __inline__ void* GET_UNCACHED_ADR(void* ptr){
	FlushCache(WRITEBACK_DCACHE);
	return (void*)(GET_PHISICAL_ADR(ptr) | 0x20000000U);
}

extern __inline__ void* GET_UNCACHED_ACCEL_ADR(void* ptr){
	FlushCache(WRITEBACK_DCACHE);
	return (void*)(GET_PHISICAL_ADR(ptr) | 0x30000000U);
}

extern __inline__ void SYNC_L(void){
	__asm__ volatile("sync.l");
}

extern __inline__ void SYNC_P(void){
	__asm__ volatile("sync.p");
}

extern __inline__ int _EIntr(void){
	__asm__ volatile("sync.l");
#if 1
	__asm__ volatile("ei");
	return 0;
#else
	u_int stat;
	__asm__ volatile("mfc0	%0, $12" : "=r"(stat));
	__asm__ volatile("ei");

	return (stat & 0x00010000U) ? 1 : 0;
#endif
}

extern __inline__ int _DIntr(void){

	int result = 0;
	u_int const mask = 0x00010000U;
	u_int stat;

	__asm__ volatile("mfc0	%0, $12" : "=r"(stat));

	if(0 != (stat & mask)){
		__asm__ volatile(
			".p2align 3
			0:
			di
			sync.p
			mfc0	%0, $12
			and	%0, %0, %1
			bne	$0, %0, 0b"
			: "=&r"(stat) : "r"(mask));

		result = 1;
	}

	return result;
}



/* [eof] */

