/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/*
 *                     INET Library
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                         linux_daemon - load_testd.c
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define BUFSIZE 2048

int sockfd, csock;

int rcv_msg(csock, buf, size)
    int csock;
    void *buf;
    int size;
{
    int len, left = size;

    while(left > 0){
        len = read(csock, buf, left);
        if(len < 0){
            perror("gamed: read");
            return -1;
        }
        else if (len == 0)
            break;
        left -= len;
        buf += len;
    }

    return (size - left);
}

int main()
{
	static char buf[BUFSIZE];
	int pktsize;
	struct sockaddr_in saddr, caddr;
	int pid, addr_len, ret;
	int val;

	if(0 > (sockfd = socket(AF_INET,SOCK_STREAM,0)) )
		perror("gamed: socket"), exit(-1);

	val = 1;
	if((ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&val, sizeof(int))) < 0){
		fprintf(stderr, "setsockopt error.\n");
		return -1;
	}
	
	bzero((char *) &saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(9012);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(0 > bind(sockfd, ( struct sockaddr* )&saddr, sizeof(saddr)))
		perror("gamed: bind"), exit(-1);
	
	if(0 > listen(sockfd, 5))
		perror("gamed: listen"),exit(-1);

	while(1){
		
		addr_len = sizeof(struct sockaddr_in);

		if(0 > (csock = accept(sockfd, ( struct sockaddr* )&caddr, &addr_len)))
			perror("gamed: accept"), exit(-1);
		
		if((pid = fork()) < 0){
			perror("fork");
			exit(-1);
		}
		else if (pid == 0) { /* child process */
			close(sockfd);

			//srand((unsigned)time(NULL) & getpid());

			if((ret = rcv_msg(csock, (char *)&pktsize, 4)) < 0)
				perror("main: rcv_msg");
			pktsize = ntohl(pktsize);
			fprintf(stderr, "pktsize = %d\n", pktsize);

			/* get msg */
			while(1) {
				if((ret = rcv_msg(csock, buf, pktsize)) < 0)
					perror("main: rcv_msg");
				else if(ret == 0){
					close(csock);
					exit(0);
				}
				else if(ret != pktsize)
					fprintf(stderr, "read byte is not %d but %d\n", pktsize, ret);

				if((ret = write(csock, buf, pktsize)) < 0)
					perror("main: write");
				else if(ret != pktsize)
					fprintf(stderr, "written byte is not %d but %d\n", pktsize, ret);
			}

			close(csock);
			exit(0);
		}

		close(csock);
	}
}
