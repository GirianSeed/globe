/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *       Emotion Engine (I/O Processor) Library Sample Program
 *
 *                       - <http_redirect> -
 *
 *                          Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                       <http_redirect.h>
 *                <header for http_redirect function>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           Aug,28,2002     komaki      first version
 */

#ifndef _HTTP_REDIRECT_H_
#define _HTTP_REDIRECT_H_

#ifdef __cplusplus
extern "C" {
#endif

int http_redirect_init(int argc, char **argv);
int http_redirect(void);

#ifdef __cplusplus
}
#endif

#endif /* _HTTP_REDIRECT_H_ */
