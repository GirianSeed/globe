/* SCE CONFIDENTIAL
   "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
*/
/*
 *     Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                   All Rights Reserved.
 *
 */

#ifndef __misc__
#define __misc__

#define UNUSED_ALWAYS(a)	(void)(a)

#if	defined(__DEBUG__)
#	define UNUSED(a)	(void)(a)
#else
#	define UNUSED(a)
#endif

#define bulkof(a)	(sizeof(a) / sizeof(a[0]))


void* GET_UNCACHED_ACCEL_ADR(void* ptr);
void* GET_UNCACHED_ADR(void* ptr);
#define GET_PHISICAL_ADR(ptr)	((u_int)(ptr) & 0x0fffffffU)
#define GET_DMASPR_ADR(ptr)		(((u_int)(ptr) & 0x000003ff0U) | 0x80000000U)
#define GET_DMA_ADR(ptr)		((7 == ((u_int)(ptr) >> 28)) ? GET_DMASPR_ADR(ptr) : GET_PHISICAL_ADR(ptr))


void SYNC_L(void);
void SYNC_P(void);

int _EIntr(void);
int _DIntr(void);


#undef EI
#undef DI
#define EI()	_EIntr()
#define	DI()	_DIntr()

#include "misc_inl.c"

#endif /* __misc__ */
