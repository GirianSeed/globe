/* SCE CONFIDENTIAL
   "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
*/
/*
 *     Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                   All Rights Reserved.
 *
 */

#ifndef __debug__
#define __debug__

#define _DBGPRINTWORKSIZE	(1024)

#include <eekernel.h>
#define PRINTF			scePrintf

void _warn(char const* format, ...);
void _trace(char const* format, ...);
void _assert(int cond, char const* str, ...);

#if defined(__TEST__) || defined(__DEBUG__)
#	define ASSERT(a)	_assert(a, "%s: %d: assertion failed: \"%s\"", __FILE__, __LINE__, #a)
#	define PUTPOS()		PRINTF("%s: %d: ", __FILE__, __LINE__)
#else
#	define ASSERT(a)
#	define PUTPOS()
#endif


#if	defined(__DEBUG__)
#	define VERIFY(v)	ASSERT(a)
#	define WARN		(0) ? (void)(0) : PUTPOS(), _warn
#	define TRACE	(0) ? (void)(0) : PUTPOS(), _trace
#else
#	define VERIFY(v)
#	define WARN		(1) ? (void)(0) : _warn
#	define TRACE	(1) ? (void)(0) : _trace
#endif


#endif /* __debug__ */

