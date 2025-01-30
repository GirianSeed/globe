/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4.2
 */
/* 
 *                      Emotion Engine Library
 *                          Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                    <libhttp[s] - http_methods.h>
 *                          <http methods>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           Nov,05,2001     komaki      first version
 */

typedef enum {
	sceHTTPM_OPTIONS,
	sceHTTPM_GET,
	sceHTTPM_HEAD,
	sceHTTPM_POST,
	sceHTTPM_PUT,
	sceHTTPM_DELETE,
	sceHTTPM_TRACE,
	sceHTTPM_CONNECT
} sceHTTPMethod_t;
