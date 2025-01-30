/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/*
 *                     INET Library
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                         ball_game - gamed.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>


#include "common.h"
#include "gamed.h"
#include "physics.h"
#include "ntohf.h"

#include "pthread.h"


// globals.
int n_clients = 0;
int n_call_counter = 0;
pthread_mutex_t mutex0;
struct sockaddr_in reg_addr[MAX_CLIENTS];

// defines 
#define GAMED_TCP_PORT_LOGIN 8920


// prototypes.
void client_thread(void*);
int register_client(void);
int send_client_position(int, int, struct sockaddr_in*, u_int);
int reset_client_position(int);
int rcvmsg(int, void *, int);

// ------------------------------------------------------------------------
//
//
//
//

int main() {

    pthread_t th1;
    int i, sd1, flag, ret;
    struct sockaddr_in saddr, caddr;
    int addr_len;
    

    pthread_mutex_init(&mutex0, NULL);

    for(i = 0; i < MAX_CLIENTS; i++)
         memset(&reg_addr[i], 0, sizeof(struct sockaddr_in));

    if( 0 > (sd1 = socket(AF_INET, SOCK_STREAM, 0))) {
        perror("gamed : socket");
        exit(-1);
    }

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(GAMED_TCP_PORT_LOGIN);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    // set sockopt to reuse port.
    flag = 1;
    if(0 > (ret = setsockopt(sd1, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)))) {
        perror("gamed: setsockopt");
        exit(-1);
    }

    if(0 > bind(sd1, ( struct sockaddr* )&saddr, sizeof(saddr))) {
	perror("gamed: bind");
        exit(-1);
    }

    if(0 > listen(sd1, 5)) {
	perror("gamed: listen");
        exit(-1);
    }

    while(1) {
        int *pfd;	

        pfd = (int*) malloc(sizeof(int));
	if(0 > (*pfd = accept(sd1, ( struct sockaddr* )&caddr, &addr_len)))
	    perror("gamed: accept"), exit(-1);
		
	pthread_create(&th1, NULL, (void*) client_thread, (void*)pfd);
    }

    return 0;
}


// -------------------------------------------------------------------
//
//
//
//

void client_thread(void *arg) {

    int sd, sdudp, ret, i;
#if 0
    char control_msg[30];
#else
	struct ctlmsg ctlmsg;
#endif
    int client_id = -1;
    struct sockaddr_in client_addr, src_addr;
    socklen_t addr_len;
	u_int seq;
    
    sd = *((int*) arg);
    free(arg);

    pthread_detach(pthread_self());

    addr_len = sizeof(struct sockaddr_in);
    if(0 > getpeername(sd, ( struct sockaddr* )&client_addr, &addr_len)) {
	perror("getpeername()");
	return;
    }

#if 0   
    printf("sin_port = %d, sin_addr = %s\n", ntohs(client_addr.sin_port)
	                                   , inet_ntoa(client_addr.sin_addr));
#endif

    // create udp socket desc.
    if((sdudp = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      perror("udp socket()");
      return;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.sin_family = AF_INET;
    src_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if( bind(sdudp, ( struct sockaddr* )&src_addr, sizeof(src_addr)) < 0) {
	perror("bind");
	return;
    }


    for(;;) {

#if 0 /* ctlmsg */
        ret = rcvmsg(sd, control_msg, 1);
#else
        ret = rcvmsg(sd, &ctlmsg, sizeof(struct ctlmsg));
		//dump_byte(&ctlmsg, sizeof(struct ctlmsg));
		printf("size = %d\n", sizeof(struct ctlmsg));
		printf("type = %d, port = %d\n", ctlmsg.type, ntohs(ctlmsg.udp_port));
		if(ret != sizeof(struct ctlmsg))
			printf("ret = %d\n", ret);
#endif

        if(ret < 0) {
            perror("read error\n");
            n_clients--;
            shutdown(sd, 2);
            return;
        }
        
#if 0 /* ctlmsg */
        switch(control_msg[0]) {
#else
        switch(ctlmsg.type) {
#endif

	    case MSG_REG:
#if 0 /* ctlmsg */
        		ret = rcvmsg(sd, control_msg, 6);
#endif

                if(client_id >= 0) {
                    reset_client_position(client_id);
                } else {

#if 0 /* ctlmsg */
	            client_addr.sin_port = TcpGet2byte(control_msg);
#else
			client_addr.sin_port = ctlmsg.udp_port;
#endif

                    // check the client already registerd.
                    for(i = 0; i < n_clients; i++) {

		        if(!memcmp(&reg_addr[i], &client_addr, sizeof(client_addr))) {
                            client_id = i;
                            break;
                        }
                    }

                    if(client_id < 0) {
                        client_id = register_client();
                        memcpy(&reg_addr[client_id], &client_addr, sizeof(client_addr));
                    }
                }
                break;

	    case MSG_POSGET:
#if 0 /* ctlmsg */
        		ret = rcvmsg(sd, control_msg, 4);
#if 1
				if(ret != 4){
					printf("POSGET: ret = %d\n", ret);
					continue;
				}
#endif
				//fprintf(stderr, "ret = %d\n", ret);
	            seq = TcpGet4byte(control_msg);
				seq = ntohl(seq);
				//fprintf(stderr, "seq = %d\n", seq);
#else
			seq = ntohl(ctlmsg.num.seq);
#endif
                send_client_position(sdudp, client_id, &client_addr, seq);
                break;

	    case MSG_RESET:
                reset_client_position(client_id);
                break;

            default:
	        //fprintf(stderr, "invalid message type %x\n", control_msg[0]);
	        fprintf(stderr, "invalid message type 0x%x\n", ctlmsg.type);
			continue;
                shutdown(sd, 2);
                return;
        }
    }


    return;
}


// -------------------------------------------------------------------
//
//
//
//

int register_client(void) {

    
    int i;

    pthread_mutex_lock(&mutex0);
    // register & init
    for(i = 0; i < N_BALLS_PER_CLIENT; i++) {
        init_sphere(n_clients * N_BALLS_PER_CLIENT + i);
    }
    n_clients++;
    pthread_mutex_unlock(&mutex0);
    return (n_clients-1);
}

// -------------------------------------------------------------------
//
//
//
//

int reset_client_position(int i_client) {

    int i;

	//fprintf(stderr, "reseted!!!\n");

    // register & init
    pthread_mutex_lock(&mutex0);
    for(i = 0; i < N_BALLS_PER_CLIENT; i++) {
        init_sphere(i_client * N_BALLS_PER_CLIENT + i);
    }
    pthread_mutex_unlock(&mutex0);
    return 0;
}



// -------------------------------------------------------------------
//
//
//
//

int send_client_position(int sd, int client_id, struct sockaddr_in *c_addr, u_int seq) {

    struct pos_data *pos_data;
    struct pos_header  *pos_header;
    int size, i_sph, i, j, ret;
    char *buf = NULL, po[10000];
    
    size = 0;
    pos_header = (struct pos_header*) po;
    pos_header->type = POS_DATA;
    pos_header->n_cli = htons(n_clients);
    pos_header->n_ball = htons(N_BALLS_PER_CLIENT);
    pos_header->seq = htonl(seq);
    size += sizeof(struct pos_header);
    buf = po + size;

  
    // calc position.
    n_call_counter++;
    pthread_mutex_lock(&mutex0);
    if((n_call_counter % n_clients) == 0) {
        calc_position((int)(n_clients * N_BALLS_PER_CLIENT));
    }
    pthread_mutex_unlock(&mutex0);        

    i_sph = 0;
    for(i = 0; i < n_clients; i++) {
        for(j = 0; j < N_BALLS_PER_CLIENT; j++) {
            
            pos_data = (struct pos_data *)buf;
	    pos_data->n_cli = htons((u_short) 1);

	    /* calculate client's position */
	    pos_data->pos1.x_pos = spheres[i_sph].position[0];
	    pos_data->pos1.y_pos = spheres[i_sph].position[1];
	    pos_data->pos1.z_pos = spheres[i_sph].position[2];
	    pos_data->pos2.x_pos = spheres[i_sph].rotation[0];
	    pos_data->pos2.y_pos = spheres[i_sph].rotation[1];
	    pos_data->pos2.z_pos = spheres[i_sph].rotation[2];

#if 0
            printf("(%f, %f, %f) (%f, %f, %f)\n", 
		   spheres[i_sph].position[0],
		   spheres[i_sph].position[1],
		   spheres[i_sph].position[2],
		   spheres[i_sph].rotation[0],
		   spheres[i_sph].rotation[1],
		   spheres[i_sph].rotation[2]);
#endif

            buf += sizeof(struct pos_data);
	    size += sizeof(struct pos_data);
            i_sph++;
        }
    }

    ret = sendto(sd, po, size, 0, ( struct sockaddr* )c_addr, sizeof(struct sockaddr_in));
	printf("send_position: after send seq = %d\n", seq);

    if(ret != size) 
        return -1;

    return 0;
}

int rcvmsg(csock, buf, size)
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
