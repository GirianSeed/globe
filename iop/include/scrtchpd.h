/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/* $Id: scrtchpd.h,v 1.1.2.1 2002/02/19 10:33:35 xokano Exp $ */

/*
 *                     I/O Processor System Services
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         scratchpad.h
 *                         ScratchPad allocation services
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           2000/12/20      tei
 */
#ifndef _SCRATCHPAD_H_
#define _SCRATCHPAD_H_

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

extern void *AllocScratchPad(int mode);
extern int FreeScratchPad(void *alloced_addr);

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif /* _SCRATCHPAD_H_ */
