/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0.2
 */
/* 
 *
 *      Copyright (C) 2002-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2003/01/08      ksh         First version
 */

#ifndef __callback_h_
#define __callback_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void sce_callback_initialize(void);
int sce_callback_open(const char *device, const char *pathname, int flags, int mode, int *filesize);
int sce_callback_read(int fd, const char *device, const char *pathname, void *buf, int offset, int size);
int sce_callback_close(int fd);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__callback_h_ */

