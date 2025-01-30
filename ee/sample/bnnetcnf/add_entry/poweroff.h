/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: poweroff.h,v 1.1 2002/10/12 08:09:26 tetsu Exp $
 */
/* 
 * Emotion Engine Library Sample Program
 *
 * Copyright (C) 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * bnnetcnf/add_entry - poweroff.c
 *	"PlayStation BB Navigator" Network Configuration Read Library Sample Program
 *	"PlayStation BB Unit" poweroff program header
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2002-10-12      tetsu      first version
 */

#ifndef _SCE_POWEROFF_H
#define _SCE_POWEROFF_H

#ifdef __cplusplus
extern "C" {
#endif

void prepare_poweroff(void);
void destory_prepare_poweroff(void);

#ifdef __cplusplus
}
#endif

#endif /* _SCE_POWEROFF_H */
