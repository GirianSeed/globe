/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                      Emotion Engine Library
 *                          Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                       <libeenet - ifinfo.h>
 *                  <header for "ifconfig" command>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           Dec,12,2002     komaki      first version
 */

#ifndef _IFINFO_H_
#define	_IFINFO_H_

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

void sceEENetShowIfinfo(const char *ifname);
void sceEENetShowAllIfinfo(void);

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif /* !_IFINFO_H_ */
