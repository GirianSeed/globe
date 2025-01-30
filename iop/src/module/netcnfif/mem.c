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

#include <kernel.h>

#define	INIT_HEAP_SIZE	(1 * 1024)

static void *g_heap = NULL;

int my_create_heap(void)
{
	if (g_heap) {
		return (-2);
	}
	g_heap = CreateHeap(INIT_HEAP_SIZE, HEAP_AUTO_EXTEND);
	if (g_heap == NULL) {
		return (-1);
	}
	return (0);
}

void *my_alloc(int size)
{
	return (AllocHeapMemory(g_heap, size));
}

void my_free(void *ptr)
{
	if (ptr) {
		FreeHeapMemory(g_heap, ptr);
	}
}

void my_delete_heap(void)
{
	DeleteHeap(g_heap);
	g_heap = NULL;
}

/*** End of file ***/
