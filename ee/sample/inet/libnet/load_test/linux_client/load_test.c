/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                     INET Library
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                         linux_client - load_test.c
 */

#include <sys/types.h>
#include <sys/wait.h>
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
#define SERVER_PORT 9012

#define dump_byte(p, size)  ({ \
	int i; \
	for(i = 0; i < (size); i++) \
	printf("%02x", *((char *)(p) + i) & 0xff); \
	printf("\n"); \
	})

static int sockfd;
static int pktsize = 0;
static int th_num = 0;
static char server_name[64];

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

int main(int argc, char **argv)
{
	static char buf[BUFSIZE], mem[BUFSIZE];
	struct sockaddr_in saddr;
	int pid, addr_len, ret, i;
	int w = 1500;
	int ps;
	int fid;
	int *ch_pid;

	if(argc != 4){
		printf("usage: %s <saddr> <th_num> <pktsize>\n", argv[0]);
		return -1;
	}

	strcpy(server_name, argv[1]);

	if ((th_num = atoi(argv[2])) <= 0){
		printf("th_num = %d\n", th_num);
		return -1;
	}

	if ((pktsize = atoi(argv[3])) <= 0){
		printf("pktsize = %d\n", pktsize);
		return -1;
	}
	ch_pid = (int *)malloc(sizeof(int *) * th_num);

	for(i = 0; i < th_num; i++) {
		fid = i;
		if((pid = fork()) < 0){
			perror("fork");
			exit(-1);
		}
		else if (pid == 0) { /* child process */
	
			if(0 > (sockfd = socket(AF_INET,SOCK_STREAM,0)) )
				perror("gamed: socket"), exit(-1);
			
			bzero((char *) &saddr, sizeof(saddr));
			saddr.sin_family = AF_INET;
			saddr.sin_port = htons(SERVER_PORT);
			saddr.sin_addr.s_addr = inet_addr(server_name);
		
			addr_len = sizeof(struct sockaddr_in);
		
			if(0 > connect(sockfd, ( struct sockaddr* )&saddr, sizeof(struct sockaddr_in)))
				perror("client: connect"), exit(-1);
		
			ps = htonl(pktsize);
			ret = write(sockfd, &ps, 4);
		
			srand((unsigned)time(NULL) & getpid());

			while(w-- > 0) {
				if(w%100 == 0)
					printf("thread %d : w = %d\n", fid, w);

				for(i = 0; i < pktsize; i++)
					buf[i] =
						(int)((double)rand() / ((double)RAND_MAX + 1) * pktsize);
				memcpy(mem, buf, pktsize);
		
				if((ret = write(sockfd, buf, pktsize)) < 0)
					perror("main: write");
				else if(ret != pktsize)
					fprintf(stderr, "write count is not %d byte, but %d byte\n", pktsize, ret);
		
				if((ret = rcv_msg(sockfd, buf, pktsize)) < 0)
					perror("main: read");
				else if(ret == 0){
					close(sockfd);
					exit(0);
				}
				else if(ret != pktsize)
					fprintf(stderr, "read count is not %d byte, but %d byte\n", pktsize, ret);
		
				if(memcmp(buf, mem, pktsize))
					fprintf(stderr, "sent data and receipt data is not same.\n");
			}
		
			close(sockfd);
			exit(0);
		}
		else {
			ch_pid[fid] = pid;
		}
	}
	i = th_num;
	while(i){
		wait(NULL);
		i--;
	}

	return 0;

}
