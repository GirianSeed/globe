/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                      ATOK Library Sample
 *
 *                         Version 0.10
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.10
 */
#ifndef __CCC_H__
#define __CCC_H__

extern void ccc_init(void);
extern const sceCccUTF8 *ccc_conv_sjis2utf8(const char *sjis);
extern int ccc_get_conv_length(void);

#endif /* __CCC_H__ */
