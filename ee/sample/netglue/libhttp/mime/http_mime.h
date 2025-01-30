/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *       Emotion Engine (I/O Processor) Library Sample Program
 *
 *                         - <http_mime> -
 *
 *                          Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         <http_mime.h>
 *                 <header for http_mime function>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           Aug,28,2002     komaki      first version
 */

#ifndef _HTTP_MIME_H_
#define _HTTP_MIME_H_

#ifdef __cplusplus
extern "C" {
#endif

int http_mime_init(int argc, char **argv);
int http_mime(void);

#ifdef __cplusplus
}
#endif

#endif /* _HTTP_MIME_H_ */
