/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                     INET Library
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                         daemon - main.h
 */

#define NETBUFSIZE 512
#define RPCSIZE NETBUFSIZE * 4
#define MAX_CONNECTION 200
#define STACKSIZE 0x4000
#define PRIO 63
#define PASSIVE_PORT 9012

struct connection_data {
    int exist;
	int fin;
	int sid;
	int tid;
	char *stack;
};

extern int passive_sema;
extern int fin_sema;
extern struct connection_data connection[];
