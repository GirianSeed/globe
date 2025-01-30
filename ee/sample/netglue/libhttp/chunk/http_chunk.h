/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *       Emotion Engine (I/O Processor) Library Sample Program
 *
 *                         - <http_chunk> -
 *
 *                          Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         <http_chunk.h>
 *                 <header for http_chunk function>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           Aug,28,2002     komaki      first version
 */

#ifndef _HTTP_CHUNK_H_
#define _HTTP_CHUNK_H_

#ifdef __cplusplus
extern "C" {
#endif

int http_chunk_init(int argc, char **argv);
int http_chunk(void);

#ifdef __cplusplus
}
#endif

#endif /* _HTTP_CHUNK_H_ */
