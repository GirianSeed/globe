/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0.2
 */
/* 
 *      Copyright (C) 2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2003/09/18      ksh       First version
 */

#ifndef __mem_h_
#define __mem_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int my_create_heap(void);
void *my_alloc(int size);
void my_free(void *ptr);
void my_delete_heap(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __mem_h_ */

