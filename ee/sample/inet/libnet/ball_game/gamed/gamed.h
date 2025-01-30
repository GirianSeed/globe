/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                     INET Library
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                         ball_game - gamed.h
 */

#ifndef __GAMED_H__
#define __GAMED_H__

#include <sys/socket.h>
#include <netinet/in.h>


#define MAX_CLIENTS 20
#define N_BALLS_PER_CLIENT 10


struct gamed_msg {
	char type;
	struct gamed_data {
		struct sockaddr_in cli_addr; /* client's ip addr */
		float rand;
	} data;
};


#endif // __GAMED_H__

